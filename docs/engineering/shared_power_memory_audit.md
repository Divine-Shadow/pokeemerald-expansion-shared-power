# Shared Power Memory Audit

## Scope

This audit covers post-fork memory and cleanup risks found while comparing the current branch against the trusted upstream merge-base `84f5aa6dae62da212efa2e8de11585bad189a031` (`origin/master`). The focus was Shared Power and nearby battle, capture, and Pokemon summary graphics paths, because prior graphical failures were traced to oversized Shared Power battle-state arrays.

This began as a static audit report. Fix-status notes below record follow-up patches while preserving the original root-cause context.

## Smoking Guns

### Shared Power switch-in ability overrides could leak into `gBattleMons` (fixed)

`SharedPower_PushAbilityOverride` saves the current battler ability, marks an override active, then writes the pooled ability directly into `gBattleMons[battler].ability` (`src/battle_shared_power.c:76`). Before the fix, `SharedPower_TrySwitchInAbilities` restored an existing override when it entered (`src/battle_shared_power.c:451`), but the successful effect path returned before calling `SharedPower_PopAbilityOverride` (`src/battle_shared_power.c:478`).

That meant a switch-in ability that produced a script effect could leave the battler's in-battle ability overwritten by the pooled ability. The leak was especially easy to hit when the effect was the last queued ability, because the function only rewinds `switchInBattlerCounter` if `index < count` (`src/battle_shared_power.c:482`). In that last-ability case, the same battler was not guaranteed to re-enter `SharedPower_TrySwitchInAbilities`, so the entry restore did not run.

Why this matters:

- This is definite missed cleanup in post-fork Shared Power code.
- Later battle and capture-adjacent code that reads `gBattleMons[battler].ability` or `GetBattlerAbility` can see the pooled ability as if it were native.
- The catch path does contain ability-dependent logic, such as Dream Ball checking `GetBattlerAbility(gBattlerTarget) == ABILITY_COMATOSE` (`src/battle_script_commands.c:13918`).
- This does not look like a direct persistent caught-Pokemon struct write: caught mons are pulled from party storage via `GetBattlerMon` (`include/battle.h:1225`) and copied by `GiveMonToPlayer` (`src/pokemon.c:3295`). It is still a clear in-battle state corruption source during the same encounter.

Fix status: the successful effect path now restores the original ability before returning while preserving the queued popup metadata for the battle script. Longer term, avoid mutating `gBattleMons[battler].ability` for pooled popup display and pass the displayed ability through explicit Shared Power state instead.

### Pokemon summary manager probing could dereference a freed manager (fixed)

`DestroyMonSpritesGfxManager` freed the selected manager but did not clear `sMonSpritesGfxManagers[managerId]` (`src/pokemon.c:6564`). That missing null assignment was preexisting, but post-fork summary-screen changes made it dangerous by using `MonSpritesGfxManager_GetSpritePtr` as an out-of-battle existence probe (`src/pokemon_summary_screen.c:1266`).

The close path destroys the manager when the summary screen created it (`src/pokemon_summary_screen.c:1686`). Before the fix, on the next summary open, `MonSpritesGfxManager_GetSpritePtr` loaded the still-non-null stale pointer and checked `gfx->active` (`src/pokemon.c:6591`). If the freed block had been reused or partially overwritten, this was a use-after-free read and could return stale sprite pointers.

Why this matters:

- This is a concrete graphical memory hazard exposed by post-fork code.
- It matches the class of earlier graphical failures: stale heap-backed sprite manager data can corrupt summary rendering.
- It can plausibly appear after catching Pokemon if the post-capture flow opens summary, party, Pokedex, evolution, or other graphics-heavy screens after a manager was destroyed.

Fix status: `DestroyMonSpritesGfxManager` now sets `sMonSpritesGfxManagers[managerId] = NULL` on every destroy path after freeing or invalidating the manager. A broader summary-screen ownership cleanup can still replace the sprite-pointer probe with an explicit manager-liveness check, or create the manager unconditionally when the owner knows it needs one.

## High-Risk Findings

### Shared Power move-end and end-turn case IDs were stored in `u8` (fixed)

`struct BattleStruct` stored `sharedPowerMoveEndCaseId` and `sharedPowerEndTurnCaseId` as `u8` arrays (`include/battle.h:802`). The helper functions accepted `u32 caseId` and compared that full value against the `u8` storage (`src/battle_util.c:3335`, `src/battle_end_turn.c:143`).

The end-turn callers build case IDs with high-byte phase data, such as `(ENDTURN_WEATHER_DAMAGE << 8) | weather`, `(ENDTURN_FIRST_EVENT_BLOCK << 8) | FIRST_EVENT_BLOCK_ABILITIES`, and `ENDTURN_ABILITIES << 8` (`src/battle_end_turn.c:357`, `src/battle_end_turn.c:719`, `src/battle_end_turn.c:1706`). Storing those values in `u8` truncated the phase byte.

This was unlikely to overflow an array because the iterator still bounds-checks against `sharedPowerPoolCount`, but it could make distinct phases collide. Since the same field controls whether the iterator index resets, a collision could skip, repeat, or resume the wrong pooled ability list.

Fix status: both case ID arrays are now `u16`, and Shared Power has regression coverage where one battler processes Rain Dish and Speed Boost in separate end-turn phases during the same rainy turn.

### `SharedPower_ClearBattleState` cleared override metadata without restoring (fixed)

`SharedPower_ClearBattleState` zeroed Shared Power popup and override fields (`src/battle_shared_power.c:222`) but did not first call `SharedPower_RestoreOriginalAbility` for active overrides. The battle-start use was probably benign because battle mons are being initialized, but the function was unsafe as a general cleanup primitive: if it ever ran while an override was active, it discarded the saved original ability and left the overwritten `gBattleMons[battler].ability` in place.

Fix status: `SharedPower_ClearBattleState` now restores all active popup ability overrides before zeroing Shared Power fields.

### Shared Power bitset helpers relied on external ability bounds (fixed)

The bitset helpers indexed Shared Power arrays with `ability >> 3` without internal bounds checks (`src/battle_shared_power.c:16`, `src/battle_shared_power.c:32`, `src/battle_shared_power.c:48`). Normal pool insertion validated `ability < ABILITIES_COUNT` (`src/battle_shared_power.c:186`), so this was mostly protected in the intended path.

The risk was defensive: if another bug corrupted an ability value or called these helpers directly with an invalid ability, the byte index could escape arrays sized to `SHARED_POWER_POOL_BYTES`. `IsAbilitySuppressedFor` also indexed `gAbilitiesInfo[ability]` without validating the ability (`src/battle_shared_power.c:265`).

Fix status: the bitset helpers and `IsAbilitySuppressedFor` now reject `ABILITY_NONE` and `ability >= ABILITIES_COUNT` before indexing Shared Power bitsets or `gAbilitiesInfo`.

### Wild held-item broker adds static EWRAM pressure

`src/wild_held_item_broker.c` adds `sWildHeldItemBrokerShopItems[ITEMS_COUNT + 1]`, which is currently about 1.6 KiB of EWRAM (`src/wild_held_item_broker.c:13`). This is not the battle heap and does not look like a direct capture overflow, but it is a post-fork memory-footprint increase worth tracking because the original failure mode involved memory headroom.

Recommended fix: keep this on the watch list if EWRAM headroom becomes tight, but do not treat it as a primary capture corruption suspect without more evidence.

Current memory-budget check from `pokeemerald.elf` after the hardening pass:

```text
EWRAM link usage: 229608 B / 262144 B (87.59%), 32536 B free
020000a8 00000004 B gBattleStruct
02014cf0 0001c300 B gHeap
020378e8 0000067c b sWildHeldItemBrokerShopItems
020378e6 00000001 b sWildHeldItemBrokerShopItemsBuilt
```

`gBattleStruct` is the EWRAM pointer symbol; the battle struct itself remains heap-backed.

## Unlikely Culprits

### Current Shared Power pool and queue capacities look bounded

The previous `ABILITIES_COUNT`-sized order arrays are gone. `SHARED_POWER_POOL_MAX` is now `PARTY_SIZE` (`include/battle_shared_power.h:8`). `SharedPower_AddToPool` validates ability IDs and caps both unique and all-source order arrays before writing (`src/battle_shared_power.c:186`, `src/battle_shared_power.c:192`, `src/battle_shared_power.c:203`). `SharedPower_BuildSwitchInQueue` writes from `sharedPowerPoolAllCount`, which is also capped at `PARTY_SIZE` (`src/battle_shared_power.c:415`).

This area should still get assertions, but the original oversized-array memory pressure does appear fixed.

### The caught-Pokemon copy path does not directly copy `gBattleMons.ability`

The catch and give flow sets ball, some Heal/Friend Ball fields, and then gives the Pokemon from party storage (`src/battle_script_commands.c:13951`, `src/battle_script_commands.c:14151`). `GetBattlerMon` returns `gEnemyParty[index]` or `gPlayerParty[index]`, not `gBattleMons` (`include/battle.h:1225`). `GiveMonToPlayer` copies that Pokemon struct (`src/pokemon.c:3312`).

This makes the leaked Shared Power ability override less likely to permanently alter the caught Pokemon's actual ability. It can still affect battle logic that runs before the encounter fully exits.

### The in-battle summary path no longer creates a separate summary manager

The current summary loader uses battle-owned `gMonSpritesGfxPtr` while in battle (`src/pokemon_summary_screen.c:4487`). That makes the old "summary screen allocates competing battle sprite resources" diagnosis less likely for the current report.

The out-of-battle manager use-after-free remains a separate smoking gun because it is triggered by the new existence probe after the manager has been destroyed.

### Wild held-item broker array writes are bounded

The broker's large static array is a memory-footprint concern, but the append path is constrained by valid item IDs and the array has an extra sentinel slot. I did not find evidence that this shop list writes past its own buffer.

## Suggested Fix Order

1. Fixed: restore the Shared Power switch-in override before yielding from a pooled switch-in activation, with focused coverage that the battler's native ability survives the script yield.
2. Fixed: null the global MonSpritesGfxManager pointer on destroy, with focused lifecycle coverage that destroyed managers cannot be reused.
3. Fixed: widen Shared Power case ID fields to `u16` and add end-turn multi-phase iterator coverage.
4. Fixed: add defensive bounds guards to the Shared Power bitset and suppression helpers.
5. Recorded: re-check EWRAM and heap headroom after those fixes, treating the wild held-item broker as a secondary memory-footprint item rather than a likely capture bug.

## Completed Hardening Follow-up

- `sharedPowerMoveEndCaseId` and `sharedPowerEndTurnCaseId` are now `u16` so composed end-turn phase IDs are not truncated.
- A regression now covers one battler processing multiple Shared Power end-turn phases in the same turn, proving iterator state resets independently per phase.
- `SharedPower_ClearBattleState` now restores active popup ability overrides before zeroing Shared Power fields.
- Shared Power bitset helpers and `IsAbilitySuppressedFor` now guard `ability == ABILITY_NONE || ability >= ABILITIES_COUNT`.
- Memory-budget diagnostics were recorded using `arm-none-eabi-nm -S --size-sort pokeemerald.elf` for `gHeap`, `gBattleStruct`, and `sWildHeldItemBrokerShopItems`.
