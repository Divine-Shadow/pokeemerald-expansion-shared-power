# Implement the Highlander Charm Toggle

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md`. It is self-contained so a contributor can restart from only this file and the current worktree.

## Purpose / Big Picture

The Highlander Charm is an opt-in Key Item for players using a no-duplicates encounter rule. When the player uses the charm from the Key Items pocket, it toggles a saved ON/OFF state. While ON, normal random wild encounters are chosen only from evolutionary families the player has not caught before; while OFF, encounters behave like vanilla. This lets a player intentionally deplete common encounter families and then force remaining legal encounters without repeatedly rolling duplicates.

The visible proof is a focused test suite where the saved toggle controls activation, caught families disappear from normal encounter pools only when enabled, remaining slots keep their original relative odds, and a depleted normal pool produces no battle plus the clear message `No unfamiliar POKéMON seem to be nearby.`

## Progress

- [x] (2026-06-04T21:05Z) Created the first Highlander Charm ExecPlan and recorded the initial passive-item scope, constraints, verification plan, and public player-note expectation.
- [x] (2026-06-04T21:18Z) Inspected current item, encounter, Pokedex, evolution-family, message, and test APIs before editing implementation files.
- [x] (2026-06-04T22:35Z) Completed and validated the first passive Items-pocket implementation, including proportional filtering, empty-pool signal behavior, and focused tests.
- [x] (2026-06-04T23:04Z) Reopened the plan after the UX decision changed from passive bag presence to a toggleable Key Item with saved ON/OFF state.
- [x] (2026-06-04T23:09Z) Converted item data, starting availability, and item-use plumbing from passive Items-pocket behavior to a non-consumable Key Item that toggles saved state.
- [x] (2026-06-04T23:09Z) Preserved the existing encounter filtering behavior while changing `IsHighlanderCharmActive` to read the saved toggle rather than bag presence.
- [x] (2026-06-04T23:09Z) Updated tests to cover default OFF state, explicit ON/OFF toggling, persistence through the save struct, and unchanged filtering/empty-pool behavior.
- [x] (2026-06-04T23:09Z) Updated player-facing docs, patch notes, and this ExecPlan for the toggleable Key Item behavior; final validation evidence remains pending.
- [x] (2026-06-05T02:08Z) Fixed Sweet Scent depleted-pool handling so the Highlander empty-pool signal is started from land and water Sweet Scent paths.
- [x] (2026-06-05T02:08Z) Ran targeted Highlander tests, item text validation, a normal ROM build, final full-suite validation, and a contract audit.

## Surprises & Discoveries

- Observation: The existing implementation already filters normal land, water, Rock Smash, Sweet Scent, and fishing random encounters while leaving roamers, outbreaks, Battle Pike/Pyramid replacement generation, scripted/static encounters, gifts, and trainer battles outside the Highlander path.
  Evidence: `src/wild_encounter.c` applies `WILD_CHECK_HIGHLANDER` only to regular `TryGenerateWildMon` calls in the normal map encounter flow, Rock Smash, Sweet Scent, and fishing. Roamer and mass outbreak setup happen before those filtered calls.

- Observation: The previous passive design put `ITEM_HIGHLANDER_CHARM` in `POCKET_ITEMS` and seeded one copy in the player's starting PC storage. That is now intentionally obsolete because Key Items cannot use PC storage as a natural ON/OFF mechanism.
  Evidence: The prior implementation used `POCKET_ITEMS` and `sNewGamePCItems`; the current worktree now uses `POCKET_KEY_ITEMS` in `src/data/items.h`, removes the PC seed from `src/player_pc.c`, and adds the item with `AddBagItem` during `NewGameInitData` in `src/new_game.c`.

- Observation: A close local toggle pattern already exists for Gen 6 Exp. Share behavior.
  Evidence: `src/item_use.c` has `ItemUseOutOfBattle_ExpShare`, which flips persistent state, plays distinct ON/OFF sounds, and reports the new state through either bag-menu or registered-key-item message paths.

- Observation: `SaveBlock3` is the least invasive save location for a new boolean toggle because it is already an expansion save block with optional feature state and enough capacity for additional bytes.
  Evidence: `include/global.h` defines `struct SaveBlock3`; `src/save.c` asserts it fits in SaveBlock3 sectors; `src/new_game.c` initializes existing SaveBlock3 fields for new games.

- Observation: New-game bag seeding can safely add the Highlander Charm immediately after `ClearBag`.
  Evidence: `src/item.c` implements `ClearBag` by zeroing `gSaveBlock1Ptr->bag`, and bag pockets point into that saved bag storage. `src/new_game.c` now calls `AddBagItem(ITEM_HIGHLANDER_CHARM, 1)` immediately after `ClearBag`.

- Observation: The saved toggle is serialized by the existing SaveBlock3 save machinery.
  Evidence: `include/global.h` stores `highlanderCharmActive` inside `struct SaveBlock3`; `src/save.c` copies all of `gSaveblock3` through `CopyFromSaveBlock3`/`CopyToSaveBlock3` and the `SaveBlock3FreeSpace` static assert still passes in the normal build.

- Observation: Sweet Scent needed an explicit empty-pool signal hop after the toggle migration audit.
  Evidence: `src/wild_encounter.c` now calls `TryStartHighlanderCharmEmptyEncounterScript()` after depleted Highlander generation in both land and water Sweet Scent paths.

## Decision Log

- Decision: Supersede the passive bag-presence design with a toggleable Key Item.
  Rationale: User feedback and the latest objective prefer an explicit `Use` action that turns the ruleset modifier ON/OFF; this is clearer than teaching players to move an item between bag and PC.
  Date/Author: 2026-06-04 / Codex.

- Decision: Store the Highlander Charm ON/OFF state in `gSaveBlock3Ptr`.
  Rationale: The state must persist across save/load, does not belong to map vars or event flags, and is project-specific feature state similar to other expansion data in SaveBlock3.
  Date/Author: 2026-06-04 / Codex.

- Decision: Keep the existing evolutionary-family filtering and proportional slot chooser.
  Rationale: The mechanic already satisfies the requested no-duplicates behavior; the scope change is activation UX and persistence, not encounter math.
  Date/Author: 2026-06-04 / Codex.

- Decision: Give the player the Key Item in the starting bag, OFF by default.
  Rationale: The previous passive implementation put a copy in the PC so the mechanic was available without being forced on. A Key Item cannot naturally use PC storage, so starting in the Key Items pocket while saved OFF preserves immediate availability and vanilla default behavior.
  Date/Author: 2026-06-04 / Codex.

## Outcomes & Retrospective

Implemented the Highlander Charm as a toggleable Key Item. The worktree has a saved `highlanderCharmActive` field in `SaveBlock3`, a Key Item use function that toggles it, new-game bag seeding with the toggle OFF, encounter activation based on saved state, and updated tests/docs.

The Highlander-specific tests and normal ROM build pass. The broad full-suite check still fails in unrelated existing suites: Limber, Drought, Booster Energy, AI switching, Condition Coach, and Protosynthesis. No Highlander-specific failure appears in the broad-suite summary. The final audit found and fixed one Highlander signal gap in Sweet Scent before the final targeted/build validation was rerun.

## Context and Orientation

This repository is a `pokeemerald-expansion`-based C project. Item definitions live in `src/data/items.h`, item constants live in `include/constants/items.h`, common item behavior constants live in `include/constants/item.h`, item-use declarations live in `include/item_use.h`, and item-use functions live in `src/item_use.c`. Random wild encounter logic lives in `src/wild_encounter.c` with public declarations in `include/wild_encounter.h`.

A wild encounter table contains multiple slots. A slot is one possible species and level range entry. The Highlander Charm must remove illegal slots before a species is rolled. Preserving relative weights means that if the original legal remaining slots represented 10 percent Poochyena and 40 percent Lotad after Rattata was removed, Poochyena should become 20 percent and Lotad should become 80 percent among the remaining legal choices. The implementation must not roll a removed slot and then reject the encounter because that would recreate the duplicate-roll problem.

The Pokedex caught state is the intended caught-history source. It records whether a species has been caught before and is not the same as the current party, the current PC boxes, or the seen state. Evolutionary-family filtering means that catching one member of a family should remove every species in that family from future Highlander-filtered random encounters.

Repel and Lure already operate in `src/wild_encounter.c`. Repel checks a generated wild Pokemon's level after rolling a slot; Lure modifies encounter rate and can mirror selected slots. Highlander Charm should compose with these mechanics without changing their semantics.

The current worktree already contains the previous passive implementation. The necessary migration is to keep the encounter-side helpers and empty-pool signal path, then replace activation plumbing and tests so saved toggle state controls the mechanic.

## Plan of Work

First, change the save model by adding a `bool8 highlanderCharmActive` field to `struct SaveBlock3` in `include/global.h` and initializing it to `FALSE` in `src/new_game.c`. This field is the source of truth for whether Highlander filtering is active.

Second, add public helper functions for toggling and setting the charm state. `IsHighlanderCharmActive` in `src/wild_encounter.c` should return `gSaveBlock3Ptr->highlanderCharmActive`, not `CheckBagHasItem`. Tests may use a test-only setter if needed, but normal game code should change the state through item use.

Third, convert `ITEM_HIGHLANDER_CHARM` in `src/data/items.h` to a Key Item. Set `importance = 1`, `pocket = POCKET_KEY_ITEMS`, keep it non-consumable, and point `.fieldUseFunc` at a new `ItemUseOutOfBattle_HighlanderCharm`. Add that declaration to `include/item_use.h`.

Fourth, implement `ItemUseOutOfBattle_HighlanderCharm` in `src/item_use.c` using the local Exp. Share toggle pattern. It should flip the saved state, play an ON/OFF sound, and display concise text for both bag-menu use and registered-key-item use. Add `gText_HighlanderCharmOn` and `gText_HighlanderCharmOff` to `src/strings.c` with declarations in `include/strings.h`.

Fifth, move starting availability from PC storage to the starting bag. Remove `ITEM_HIGHLANDER_CHARM` from `sNewGamePCItems` in `src/player_pc.c`, and add it during new game initialization after bag pockets are initialized. If the local startup order makes direct bag insertion unsafe in `NewGameInitData`, use the closest existing new-game bag seeding pattern.

Sixth, update the tests in `test/wild_encounter.c`. Replace the passive bag/PC activation test with default OFF, state setter/toggle semantics, and bag-presence independence. Keep the existing family filtering, proportional weighting, empty pool, and Old Rod tests because they validate the core encounter behavior. Add a direct save-struct persistence-style assertion by setting `gSaveBlock3Ptr->highlanderCharmActive`, calling `IsHighlanderCharmActive`, and confirming the value survives normal state reads without a bag item.

Finally, update `docs/gameplay_changes_public.md`, `docs/gameplay_changes_blind.md` if necessary, and `PATCH_NOTES.md`. Run targeted tests and a normal build. If full `make check` still fails because of previously recorded unrelated failures, capture that as residual risk rather than treating it as Highlander failure.

## Concrete Steps

Run commands from the repository root `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Useful research commands:

    rg -n "Highlander|HIGHLANDER|ItemUseOutOfBattle_ExpShare|SaveBlock3|NewGameInitData" include src test data docs PATCH_NOTES.md
    rg -n "ChooseWildMonIndex|TryGenerateWildMon|SweetScentWildEncounter|FishingWildEncounter|RockSmashWildEncounter" src/wild_encounter.c include/wild_encounter.h test/wild_encounter.c

Validation commands to run after implementation:

    make -j"$(nproc)" TESTS="Highlander Charm" check
    make -j"$(nproc)" TESTS="Item descriptions fit" check
    make -j"$(nproc)" NO_MULTIBOOT=1

If the local host cannot run generated tools, use the existing Docker path documented in `AGENTS.md`:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Highlander Charm" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Item descriptions fit" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check

## Validation and Acceptance

Acceptance requires evidence for every requested behavior. With the saved Highlander Charm state OFF, normal encounter selection must behave as before even if the Key Item is in the bag. With the saved state ON and a family marked caught in the Pokedex caught state, every slot in that family must be excluded before the slot roll. When remaining slots have original weights such as 10 percent and 40 percent after a 50 percent caught family is removed, the resulting legal odds must be 20 percent and 80 percent. When all normal random slots are illegal, the game must suppress the encounter and show a clear signal message instead of silently failing or falling back to duplicates.

The feature is not accepted if it filters roamers, outbreaks, statics, gifts, scripted encounters, trainer battles, or current party/box membership. It is also not accepted if it relies on seen state, exact species only, or after-the-fact duplicate rejection. The feature is not accepted if the toggle is only in RAM and cannot be saved, or if the charm defaults ON for new games.

## Idempotence and Recovery

The planned edits are additive and local to save state, item data, item-use messaging, encounter activation, tests, player notes, and patch notes. Re-running tests is safe. If a test or build fails, inspect the failing file and update this plan's `Surprises & Discoveries` and `Decision Log` before changing course. Do not use destructive git commands; preserve unrelated worktree changes if any appear.

## Artifacts and Notes

Initial evidence for this resumed toggle work:

    git status --short
    <clean output before new edits>

    date -u +%Y-%m-%dT%H:%MZ
    2026-06-04T23:04Z

The prior passive implementation's final validation evidence remains useful for encounter math but not for activation UX. It passed Highlander-specific tests and a normal Docker ROM build with `NO_MULTIBOOT=1`; broad `make check` had unrelated pre-existing failures documented in the previous version of this plan.

Final validation evidence for the toggle work:

    make -j"$(nproc)" TESTS="Highlander Charm" check
    Failed before compile on this NixOS host because existing generated Linux tools could not run under the host loader.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Highlander Charm" NO_MULTIBOOT=1 check
    6 passed / 6 total.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Item descriptions fit" NO_MULTIBOOT=1 check
    1 passed / 1 total.

    git diff --check
    Passed with no output.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1
    Passed. Memory usage: EWRAM 230168 B / 256 KB (87.80%); IWRAM 28705 B / 32 KB (87.60%); ROM 24703864 B / 32 MB (73.62%).

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check
    Failed with 7 unrelated existing failures, 18 known failing, 3 assumptions failed, 792 TODO, 3310 passed, 4130 total. Failed tests were Limber prevents paralysis from Thunder Wave; Drought 5-turn and Heat Rock 8-turn sun setup; Booster Energy after harsh sunlight ends; AI smart switching 1v1 matchup; Condition Coach rejects Eggs; Protosynthesis popup once during sunny day.

## Interfaces and Dependencies

The implementation should expose only small helper interfaces needed by tests and item use. Existing code style uses `PascalCase` for functions, `camelCase` for local variables, and four-space indentation in C. Tests use the repository's `TEST("Suite/Case")` style and assertions from `include/test/test.h`.

Required end-state interfaces are:

    bool32 IsHighlanderCharmActive(void);
    void SetHighlanderCharmActive(bool32 active);
    void ToggleHighlanderCharmActive(void);
    void ItemUseOutOfBattle_HighlanderCharm(u8 taskId);

`IsHighlanderCharmActive` should read saved state. `SetHighlanderCharmActive` and `ToggleHighlanderCharmActive` should update saved state. `ItemUseOutOfBattle_HighlanderCharm` should be the only normal gameplay caller that toggles the state.

Revision Note (2026-06-04): Rewrote this ExecPlan to supersede the completed passive Items-pocket implementation with the current toggleable Key Item objective. The change was necessary because the latest UX decision requires saved ON/OFF state, not bag-versus-PC activation.

Revision Note (2026-06-04): Updated progress, discoveries, and outcomes after implementing the saved Key Item toggle and updating tests/docs. Validation remains intentionally open until targeted tests and build commands run.

Revision Note (2026-06-05): Recorded final validation evidence, broad-suite residual failures, and the Sweet Scent empty-pool signal fix. The contract audit is satisfied by the final worktree and command evidence.
