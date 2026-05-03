# Shared Power Memory Audit

## Scope

This audit covers post-fork memory and cleanup risks found while comparing the current branch against the trusted upstream merge-base `84f5aa6dae62da212efa2e8de11585bad189a031` (`origin/master`). The focus was Shared Power and nearby battle, capture, and Pokemon summary graphics paths, because prior graphical failures were traced to oversized Shared Power battle-state arrays.

This is a static audit report. It records likely corruption sources and does not apply code fixes.

## Smoking Guns

### Shared Power switch-in ability overrides can leak into `gBattleMons`

`SharedPower_PushAbilityOverride` saves the current battler ability, marks an override active, then writes the pooled ability directly into `gBattleMons[battler].ability` (`src/battle_shared_power.c:76`). `SharedPower_TrySwitchInAbilities` restores an existing override when it enters (`src/battle_shared_power.c:451`), but the successful effect path returns before calling `SharedPower_PopAbilityOverride` (`src/battle_shared_power.c:478`).

That means a switch-in ability that produces a script effect can leave the battler's in-battle ability overwritten by the pooled ability. The leak is especially easy to hit when the effect is the last queued ability, because the function only rewinds `switchInBattlerCounter` if `index < count` (`src/battle_shared_power.c:482`). In that last-ability case, the same battler is not guaranteed to re-enter `SharedPower_TrySwitchInAbilities`, so the entry restore does not run.

Why this matters:

- This is definite missed cleanup in post-fork Shared Power code.
- Later battle and capture-adjacent code that reads `gBattleMons[battler].ability` or `GetBattlerAbility` can see the pooled ability as if it were native.
- The catch path does contain ability-dependent logic, such as Dream Ball checking `GetBattlerAbility(gBattlerTarget) == ABILITY_COMATOSE` (`src/battle_script_commands.c:13918`).
- This does not look like a direct persistent caught-Pokemon struct write: caught mons are pulled from party storage via `GetBattlerMon` (`include/battle.h:1225`) and copied by `GiveMonToPlayer` (`src/pokemon.c:3295`). It is still a clear in-battle state corruption source during the same encounter.

Recommended fix: restore the original ability before every successful `return TRUE` from the Shared Power switch-in loop, after recording any popup metadata needed by the script. Longer term, avoid mutating `gBattleMons[battler].ability` for pooled popup display and pass the displayed ability through explicit Shared Power state instead.

### Pokemon summary manager probing can dereference a freed manager

`DestroyMonSpritesGfxManager` frees the selected manager but does not clear `sMonSpritesGfxManagers[managerId]` (`src/pokemon.c:6564`). That missing null assignment is preexisting, but post-fork summary-screen changes made it dangerous by using `MonSpritesGfxManager_GetSpritePtr` as an out-of-battle existence probe (`src/pokemon_summary_screen.c:1266`).

The close path destroys the manager when the summary screen created it (`src/pokemon_summary_screen.c:1686`). On the next summary open, `MonSpritesGfxManager_GetSpritePtr` loads the still-non-null stale pointer and checks `gfx->active` (`src/pokemon.c:6591`). If the freed block has been reused or partially overwritten, this is a use-after-free read and may return stale sprite pointers.

Why this matters:

- This is a concrete graphical memory hazard exposed by post-fork code.
- It matches the class of earlier graphical failures: stale heap-backed sprite manager data can corrupt summary rendering.
- It can plausibly appear after catching Pokemon if the post-capture flow opens summary, party, Pokedex, evolution, or other graphics-heavy screens after a manager was destroyed.

Recommended fix: make `DestroyMonSpritesGfxManager` set `sMonSpritesGfxManagers[managerId] = NULL` on every destroy path after freeing or invalidating the manager. Then replace the summary screen's sprite-pointer probe with an explicit manager-liveness check, or create the manager unconditionally when the owner knows it needs one.

## High-Risk Findings

### Shared Power move-end and end-turn case IDs are stored in `u8`

`struct BattleStruct` stores `sharedPowerMoveEndCaseId` and `sharedPowerEndTurnCaseId` as `u8` arrays (`include/battle.h:802`). The helper functions accept `u32 caseId` and compare that full value against the `u8` storage (`src/battle_util.c:3335`, `src/battle_end_turn.c:143`).

The end-turn callers build case IDs with high-byte phase data, such as `(ENDTURN_WEATHER_DAMAGE << 8) | weather`, `(ENDTURN_FIRST_EVENT_BLOCK << 8) | FIRST_EVENT_BLOCK_ABILITIES`, and `ENDTURN_ABILITIES << 8` (`src/battle_end_turn.c:357`, `src/battle_end_turn.c:719`, `src/battle_end_turn.c:1706`). Storing those values in `u8` truncates the phase byte.

This is unlikely to overflow an array because the iterator still bounds-checks against `sharedPowerPoolCount`, but it can make distinct phases collide. Since the same field controls whether the iterator index resets, a collision can skip, repeat, or resume the wrong pooled ability list.

Recommended fix: change both case ID fields to `u16` or `u32`, then add regression tests where multiple Shared Power end-turn phases happen for the same battler in one turn.

### `SharedPower_ClearBattleState` clears override metadata without restoring

`SharedPower_ClearBattleState` zeroes Shared Power popup and override fields (`src/battle_shared_power.c:222`) but does not first call `SharedPower_RestoreOriginalAbility` for active overrides. The current battle-start use is probably benign because battle mons are being initialized, but the function is unsafe as a general cleanup primitive: if it ever runs while an override is active, it discards the saved original ability and leaves the overwritten `gBattleMons[battler].ability` in place.

Recommended fix: restore all active overrides before clearing the fields, or split battle-start initialization from mid-battle cleanup so the dangerous behavior cannot be reused accidentally.

### Shared Power bitset helpers rely on external ability bounds

The bitset helpers index Shared Power arrays with `ability >> 3` without internal bounds checks (`src/battle_shared_power.c:16`, `src/battle_shared_power.c:32`, `src/battle_shared_power.c:48`). Normal pool insertion validates `ability < ABILITIES_COUNT` (`src/battle_shared_power.c:186`), so this is mostly protected in the intended path.

The risk is defensive: if another bug corrupts an ability value or calls these helpers directly with an invalid ability, the byte index can escape arrays sized to `SHARED_POWER_POOL_BYTES`. `IsAbilitySuppressedFor` also indexes `gAbilitiesInfo[ability]` without validating the ability (`src/battle_shared_power.c:265`).

Recommended fix: add cheap guards or debug assertions in the bitset helpers and `IsAbilitySuppressedFor`. The helpers are small enough that central validation is less fragile than relying on every caller forever.

### Wild held-item broker adds static EWRAM pressure

`src/wild_held_item_broker.c` adds `sWildHeldItemBrokerShopItems[ITEMS_COUNT + 1]`, which is currently about 1.6 KiB of EWRAM (`src/wild_held_item_broker.c:13`). This is not the battle heap and does not look like a direct capture overflow, but it is a post-fork memory-footprint increase worth tracking because the original failure mode involved memory headroom.

Recommended fix: keep this on the watch list if EWRAM headroom becomes tight, but do not treat it as a primary capture corruption suspect without more evidence.

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

1. Fix the Shared Power switch-in override leak and add a focused test that a pooled switch-in activation restores the battler's native ability after the script yields.
2. Fix `DestroyMonSpritesGfxManager` to null the global manager pointer, then add a summary-open/close/reopen regression check if the test harness can cover it.
3. Widen Shared Power case ID fields and add end-turn phase collision coverage.
4. Add defensive bounds guards/assertions to the Shared Power bitset and suppression helpers.
5. Re-check EWRAM and heap headroom after those fixes, treating the wild held-item broker as a secondary memory-footprint item rather than a likely capture bug.
