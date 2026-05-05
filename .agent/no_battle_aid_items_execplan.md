# Add a Battle Flag for No Combat Aid Items

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md` from the repository root. A future contributor should be able to restart from only this file and the working tree.

## Purpose / Big Picture

The game already has battle flags that can disable all Bag use or disable catching. This change adds a narrower rule: scripts can set one runtime flag to block combat aid items such as X items, Guard Spec, Dire Hit, healing items, status cures, revives, PP restorers, Escape items, and the battle-use Poke Flute while still allowing Poke Balls to be used under the normal catch rules. This lets challenge battles or route segments prevent mid-battle item help without breaking capture gameplay.

The visible proof is a testable battle-item legality check: with the new flag set, `ITEM_X_ATTACK` and `ITEM_POTION` report unusable in battle, while `ITEM_POKE_BALL` remains legal unless an existing catching restriction such as `B_FLAG_NO_CATCHING` applies.

## Progress

- [x] (2026-05-03T05:29Z) Investigated existing battle item gates in `src/battle_main.c`, `src/item_use.c`, `src/item_menu.c`, `src/party_menu.c`, `include/config/battle.h`, and `include/constants/flags.h`.
- [x] (2026-05-03T05:29Z) Chose a new runtime config flag rather than reusing `B_FLAG_NO_BAG_USE`, because `B_FLAG_NO_BAG_USE` blocks the whole item action before the item kind is known.
- [x] (2026-05-03T05:29Z) Wrote this ExecPlan and added a pending `PATCH_NOTES.md` entry.
- [x] (2026-05-03T05:29Z) Add the runtime flag constant and config macro.
- [x] (2026-05-03T05:29Z) Update the item legality gate so non-ball in-battle item effects are blocked when the flag is set.
- [x] (2026-05-03T05:29Z) Reset and expose the flag consistently with nearby battle flags.
- [x] (2026-05-03T05:29Z) Add focused tests for X item, medicine, and Poke Ball behavior.
- [x] (2026-05-03T05:29Z) Update player-facing public gameplay notes.
- [x] (2026-05-03T05:38Z) Focused validation passed with `make -j"$(nproc)" check TESTS="*combat aid" NO_MULTIBOOT=1`.
- [x] (2026-05-03T05:54Z) Broad validation passed with `make -j"$(nproc)" check NO_MULTIBOOT=1`: 3329 passed, 17 known failing, 827 TODO, 4173 total.
- [x] (2026-05-03T05:55Z) ROM build passed with `make -j"$(nproc)" NO_MULTIBOOT=1`.
- [x] (2026-05-03T05:55Z) Whitespace validation passed with `git diff --check`.

## Surprises & Discoveries

- Observation: The existing `B_FLAG_NO_BAG_USE` check occurs in `src/battle_main.c` while choosing the item action, before `src/item_use.c` knows whether the chosen item is a Poke Ball.
  Evidence: `B_ACTION_USE_ITEM` branches to `BattleScript_ActionSelectionItemsCantBeUsed` when `FlagGet(B_FLAG_NO_BAG_USE)` is true.
- Observation: `CannotUseItemsInBattle` currently reads a selected Pokemon's HP before switching on item battle effect, even though bag-menu items like X Attack and Poke Balls call it with `mon == NULL`.
  Evidence: `src/item_use.c` computes `u16 hp = GetMonData(mon, MON_DATA_HP);` before the `switch (battleUsage)`, while `ItemUseInBattle_BagMenu` calls `CannotUseItemsInBattle(gSpecialVar_ItemId, NULL)`.
- Observation: In this repository's test runner, a plain `TESTS` value is a prefix match. To match text in the middle of test names, prefix the filter with `*`.
  Evidence: `TESTS="combat aid"` produced `No tests found`, while `TESTS="*combat aid"` ran the new tests.
- Observation: This environment needs `NO_MULTIBOOT=1` for test linking after generated multiboot objects are present.
  Evidence: `make check TESTS="combat aid"` failed with duplicate `gMultiBootProgram_*` symbols; adding `NO_MULTIBOOT=1` avoided the duplicate multiboot link.

## Decision Log

- Decision: Add `FLAG_NO_BATTLE_AID_ITEMS` by renaming unused flag `0x264`, and set `B_FLAG_NO_BATTLE_AID_ITEMS` to that flag in `include/config/battle.h`.
  Rationale: The battle config comments explicitly recommend renaming unused flags when enabling flag-backed battle rules. A real save flag lets map scripts and debug tools toggle the rule at runtime.
  Date/Author: 2026-05-03 / Codex.
- Decision: Define “combat aid item” as any item with a nonzero battle-use effect except `EFFECT_ITEM_THROW_BALL`.
  Rationale: The item data already classifies battle behavior through `GetItemBattleUsage`. This keeps Poke Balls exempt while covering X items, Dire Hit, Guard Spec, healing, status cures, revives, PP restorers, Escape, Max Mushrooms, and battle-use Poke Flute without maintaining a fragile item-id allowlist.
  Date/Author: 2026-05-03 / Codex.
- Decision: Keep `B_FLAG_NO_CATCHING` responsible for Poke Ball prohibition.
  Rationale: Catching has its own rules and failure text. The new flag should allow balls through to the existing catching gate, not duplicate or override it.
  Date/Author: 2026-05-03 / Codex.

## Outcomes & Retrospective

Implemented a runtime no-combat-aid-items rule. The new flag blocks battle-use aid effects through `CannotUseItemsInBattle` while letting Poke Balls proceed to the existing catching checks. The implementation compiles in the production ROM, is available from the debug flag menu, is documented in public gameplay notes, and has focused tests for X item, medicine, and Poke Ball behavior.

One incidental robustness fix was required: `CannotUseItemsInBattle` now reads selected Pokemon HP only inside item-effect cases that need a selected Pokemon, because bag-menu-only battle items call the function with `mon == NULL`.

## Context and Orientation

This repository is a C-based pokeemerald-expansion project. Battle rule toggles live in `include/config/battle.h`; many of those toggles are compile-time constants set to `0` unless a real save flag is assigned. Save flag constants live in `include/constants/flags.h`.

The Bag and battle engine use two different item gates. `src/battle_main.c` handles the high-level battle action chosen by a battler. Its `B_ACTION_USE_ITEM` branch can reject all items globally with `B_FLAG_NO_BAG_USE`, but this is too broad because it also rejects Poke Balls. Once the Bag menu knows which item was selected, `src/item_use.c` calls `CannotUseItemsInBattle(u16 itemId, struct Pokemon *mon)` to decide whether that specific item can be used in the current battle context. Party-menu medicines and PP items also route into this function from `src/party_menu.c`.

In this plan, “battle-use effect” means the `battleUsage` field in `src/data/items.h`, read through `GetItemBattleUsage(itemId)`. Poke Balls use `EFFECT_ITEM_THROW_BALL`. Other combat aid effects include `EFFECT_ITEM_INCREASE_STAT`, `EFFECT_ITEM_SET_MIST`, `EFFECT_ITEM_SET_FOCUS_ENERGY`, `EFFECT_ITEM_ESCAPE`, `EFFECT_ITEM_INCREASE_ALL_STATS`, `EFFECT_ITEM_RESTORE_HP`, `EFFECT_ITEM_CURE_STATUS`, `EFFECT_ITEM_HEAL_AND_CURE_STATUS`, `EFFECT_ITEM_REVIVE`, `EFFECT_ITEM_RESTORE_PP`, and `EFFECT_ITEM_USE_POKE_FLUTE`.

Player-facing documentation is split by the repo-local gameplay updater skill. Rule toggles and battle-system behavior are public, so the public note file is `docs/gameplay_changes_public.md`. Exact map placements or story beats would be blind-friendly, but this change does not place the flag anywhere.

## Plan of Work

First, add a descriptive save flag in `include/constants/flags.h` and wire `B_FLAG_NO_BATTLE_AID_ITEMS` in `include/config/battle.h` near `B_FLAG_NO_BAG_USE` and `B_FLAG_NO_CATCHING`.

Second, update `src/item_use.c`. Add a small helper that returns true only when `B_FLAG_NO_BATTLE_AID_ITEMS` is configured and set. In `CannotUseItemsInBattle`, after reading `battleUsage` but before item-specific checks, return unusable when that helper is true and `battleUsage != EFFECT_ITEM_THROW_BALL`. Use the same generic failed item text as other item legality failures by expanding `gText_WontHaveEffect` into `gStringVar4`. Move the selected Pokemon HP read into only the switch cases that need a non-null Pokemon so bag-menu items do not dereference `NULL`.

Third, integrate the flag with nearby battle infrastructure. In `src/overworld.c`, clear it in `Overworld_ResetBattleFlagsAndVars` alongside `B_FLAG_NO_BAG_USE` and `B_FLAG_NO_CATCHING`. In `src/debug.c`, add a debug toggle entry next to Bag Use OFF and Catching OFF so testers can flip the rule manually.

Fourth, add focused unit tests in a new `test/battle_aid_items.c` file. The tests should call `CannotUseItemsInBattle` directly because this is the menu legality gate being changed, not the battle script that executes a previously selected item. They should prove that the new flag blocks an X item with no selected Pokemon, blocks a medicine on a selected Pokemon, and does not block a Poke Ball when the existing catching rules allow it.

Fifth, update `docs/gameplay_changes_public.md` to describe the new challenge-rule capability without exposing any future placement details. Update `PATCH_NOTES.md` at the top for each changed area as required by repo policy.

## Concrete Steps

Run all commands from `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

1. Inspect the current worktree before editing:

        git status --short

   Expected result before implementation is either no output or only unrelated pre-existing user changes. Do not revert unrelated files.

2. Apply source, test, docs, and patch note edits with `apply_patch`.

3. Run focused validation:

        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" check TESTS="*combat aid" NO_MULTIBOOT=1

   Expected result: the new tests matching “combat aid” pass.

4. Run broader validation for battle and ROM integration:

        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" check NO_MULTIBOOT=1
        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1
        git diff --check

   Expected result: no test failures, a successful ROM build, and no whitespace errors.

## Validation and Acceptance

Acceptance requires behavior, not just compilation. The focused tests must show that `CannotUseItemsInBattle(ITEM_X_ATTACK, NULL)` and `CannotUseItemsInBattle(ITEM_POTION, &mon)` return `TRUE` when `B_FLAG_NO_BATTLE_AID_ITEMS` is set, and that `CannotUseItemsInBattle(ITEM_POKE_BALL, NULL)` returns `FALSE` in a single wild-battle state when that same flag is set and catching is otherwise allowed.

The full `make check` run should pass so this central item legality change does not regress existing battle mechanics. The ROM build should pass because this adds a save flag/config constant and debug menu entry that must compile in the production build, not only tests.

## Idempotence and Recovery

The edits are additive and can be reapplied or adjusted safely. If a test command fails because the Docker image is missing, build it with:

        docker build -t pokeemerald-expansion:builder .

If focused tests fail, inspect only the changed item legality tests and `src/item_use.c` first. If broad tests fail outside the new test filter, categorize whether the failure is caused by the new flag defaulting on, by missing cleanup after tests, or by unrelated existing repo failures. The test helper should clear any flags it sets before returning.

Do not use destructive git commands such as `git reset --hard` or `git checkout --` while recovering. Preserve unrelated user work.

## Artifacts and Notes

Initial investigation found these existing anchors:

        include/config/battle.h: B_FLAG_NO_BAG_USE and B_FLAG_NO_CATCHING are adjacent battle flags.
        include/constants/flags.h: FLAG_UNUSED_0x264 is available for a descriptive runtime flag.
        src/item_use.c: CannotUseItemsInBattle is the item-specific battle legality gate.
        src/overworld.c: Overworld_ResetBattleFlagsAndVars clears configured battle flags after whiteout.
        src/debug.c: debug toggles already exist for Bag Use OFF and Catching OFF.

Validation transcripts will be added here after commands run.

Focused test evidence:

        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" check TESTS="*combat aid" NO_MULTIBOOT=1
        [02] No combat aid flag blocks X items in battle: PASS
        [00] No combat aid flag allows Poke Balls through normal catching rules: PASS
        [01] No combat aid flag blocks medicine in battle: PASS
        Tests PASSED: 3
        Tests TOTAL: 3

Broad validation evidence:

        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" check NO_MULTIBOOT=1
        Tests KNOWN_FAILING: 17
        Tests TO_DO: 827
        Tests PASSED: 3329
        Tests TOTAL: 4173

ROM build evidence:

        docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1
        Memory region         Used Size  Region Size  %age Used
        EWRAM:      229608 B       256 KB     87.59%
        IWRAM:       28705 B        32 KB     87.60%
        ROM:      24691060 B        32 MB     73.59%

Whitespace evidence:

        git diff --check
        no output

## Interfaces and Dependencies

At the end of the work these interfaces must exist:

    // include/constants/flags.h
    #define FLAG_NO_BATTLE_AID_ITEMS 0x264

    // include/config/battle.h
    #define B_FLAG_NO_BATTLE_AID_ITEMS FLAG_NO_BATTLE_AID_ITEMS

    // src/item_use.c
    bool32 CannotUseItemsInBattle(u16 itemId, struct Pokemon *mon);

`CannotUseItemsInBattle` remains the public item-use legality function declared in `include/item_use.h`. The new helper in `src/item_use.c` can stay file-static unless another subsystem needs it later. Tests should include `item_use.h`, `event_data.h`, `pokemon.h`, `battle.h`, `test/test.h`, `constants/battle.h`, `constants/item_effects.h`, `constants/items.h`, and `constants/species.h` as needed.

Revision Note (2026-05-03): Initial plan written after investigating the existing battle item gates. The selected implementation is a runtime flag that blocks every battle-use item effect except Poke Balls through the central item legality function.
