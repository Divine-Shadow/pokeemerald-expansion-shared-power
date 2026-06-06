# Add the Fallarbor Ability Swapper

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md` from the repository root. It is self-contained so a future contributor can resume the work from this file and the current worktree.

## Purpose / Big Picture

After this change, a player can visit a new NPC in the Fallarbor Move Relearner house and spend one Heart Scale to switch a party Pokemon between its two ordinary, non-hidden ability slots. The service previews the exact current ability and target ability before taking payment. It never grants a Hidden Ability and refuses to change a Pokemon that currently has its Hidden Ability.

The behavior is visible in-game by talking to the new NPC near the Move Relearner, choosing an eligible Pokemon such as Breloom with Effect Spore or Poison Heal, confirming the preview, and observing that one Heart Scale is consumed and the Pokemon now has the alternate ordinary ability.

## Progress

- [x] (2026-06-06T19:14Z) Researched the existing Move Relearner map, Ability Capsule item behavior, script specials, Condition Coach service pattern, and player-note conventions.
- [x] (2026-06-06T19:20Z) Implemented script-facing Ability Swapper preview/swap helpers in `src/script_pokemon_util.c` and exposed them through `include/script_pokemon_util.h` and `data/specials.inc`.
- [x] (2026-06-06T19:20Z) Added named result constants for the Ability Swapper.
- [x] (2026-06-06T19:28Z) Added the Ability Swapper NPC object and dialogue to `data/maps/FallarborTown_MoveRelearnersHouse/`.
- [x] (2026-06-06T19:35Z) Added focused unit tests for eligibility, preview buffering, item consumption, and Hidden Ability refusal in `test/ability_swapper.c`.
- [x] (2026-06-06T19:38Z) Updated public and blind-friendly player notes plus `PATCH_NOTES.md`.
- [x] (2026-06-06T19:39Z) Ran targeted Ability Swapper tests, whitespace validation, and a clean Docker ROM build with `NO_MULTIBOOT=1`.

## Surprises & Discoveries

- Observation: `src/party_menu.c` already implements Ability Capsule and Ability Patch behavior. Ability Capsule swaps only between ordinary slots by XORing the current ability number with `1`, and rejects targets outside slot `0` or `1`.
  Evidence: `Task_AbilityCapsule` rejects duplicate ordinary abilities, missing slot 1, and `tAbilityNum > 1` before calling `SetMonData(... MON_DATA_ABILITY_NUM ...)`.
- Observation: A script service can reuse `ChoosePartyMon`, which returns the selected party slot in `VAR_0x8004`.
  Evidence: `data/scripts/condition_coach.inc` calls `special ChoosePartyMon`, then calls `specialvar VAR_RESULT, ConditionCoach_TryApplyStatus`.
- Observation: `data/maps/FallarborTown_MoveRelearnersHouse/map.json` uses `LAYOUT_HOUSE2`, which is 11 by 8 tiles, and currently has only the Move Relearner at `(4,4)`.
  Evidence: `data/layouts/layouts.json` defines `LAYOUT_HOUSE2` width `11`, height `8`.
- Observation: Native validation in this NixOS shell cannot execute the generated Linux tools directly.
  Evidence: `make check TESTS="Ability Swapper"` failed before compiling tests because `tools/mapjson/mapjson` expected a generic Linux dynamic loader.
- Observation: This checkout needs `NO_MULTIBOOT=1` for Docker validation.
  Evidence: Docker validation without `NO_MULTIBOOT=1` reached link time and failed on duplicate multiboot symbols, while the same targeted test and clean ROM build passed with `NO_MULTIBOOT=1`.

## Decision Log

- Decision: Put payment and ability mutation in a C special rather than scripting `removeitem` after a separate mutation step.
  Rationale: This makes the "do not charge unless the swap completes" invariant unit-testable and keeps item consumption coupled to the state change.
  Date/Author: 2026-06-06 / Codex
- Decision: Reuse `ChoosePartyMon` for selection instead of adding a custom party menu.
  Rationale: The existing party chooser already returns the selected slot and matches nearby service patterns such as the Condition Coach.
  Date/Author: 2026-06-06 / Codex
- Decision: Use an existing distinct overworld sprite for the NPC unless a custom art asset is already present and suitable.
  Rationale: The user asked for a new sprite distinct from the Move Relearner; a different existing GBA-style NPC sprite satisfies the visual requirement without adding unreviewed pixel art.
  Date/Author: 2026-06-06 / Codex

## Outcomes & Retrospective

The Ability Swapper implementation is complete. The new Fallarbor NPC uses a distinct Scientist overworld sprite, previews the exact ordinary ability swap, refuses Eggs, Hidden Ability Pokemon, cancelled selections, and species without a distinct alternate ordinary ability, and couples Heart Scale removal to the successful mutation path.

Validation evidence:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make check TESTS="Ability Swapper" NO_MULTIBOOT=1

This targeted run passed all 9 Ability Swapper tests.

    git diff --check

This whitespace check passed with no output.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make NO_MULTIBOOT=1

This clean ROM build completed successfully and generated `pokeemerald.gba`.

## Context and Orientation

The Move Relearner service lives in `data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc`. The map object list is in `data/maps/FallarborTown_MoveRelearnersHouse/map.json`; adding an NPC means adding another object event with a unique local id, a distinct `OBJ_EVENT_GFX_*` value, coordinates, and a script label.

Script specials are C functions callable from event scripts. They are listed in `data/specials.inc`. If a special is declared with `, 1`, it returns a value for `specialvar`. The C service helpers for party Pokemon already live in `src/script_pokemon_util.c`, with prototypes in `include/script_pokemon_util.h`.

Pokemon ability slots are represented by `MON_DATA_ABILITY_NUM`: slot `0` and slot `1` are ordinary abilities, while slot `2` is the Hidden Ability slot. `NUM_NORMAL_ABILITY_SLOTS` is `2`, so any ability number greater than or equal to that is not an ordinary slot. `GetAbilityBySpecies(species, abilityNum)` and `GetSpeciesAbility(species, slot)` read ability definitions. `gAbilitiesInfo[ability].name` provides the display name for dialogue placeholders.

The existing Ability Capsule implementation in `src/party_menu.c` is the main semantic model: the alternate ordinary slot is `currentAbilityNum ^ 1`; the service is invalid when the two ordinary abilities are identical, the second ordinary ability is missing, or the current ability is not ordinary.

Player-facing notes are split by spoiler policy. The broad rule that Heart Scales can change ordinary abilities belongs in `docs/gameplay_changes_public.md`. The exact NPC location in the Move Relearner house belongs in `docs/gameplay_changes_blind.md`. Every change must also prepend a concise entry to `PATCH_NOTES.md` using `commit pending` until committed.

## Plan of Work

Add `include/constants/ability_swapper.h` with named result codes such as `ABILITY_SWAPPER_RESULT_READY`, `ABILITY_SWAPPER_RESULT_SWAPPED`, `ABILITY_SWAPPER_RESULT_NO_HEART_SCALE`, `ABILITY_SWAPPER_RESULT_EGG`, `ABILITY_SWAPPER_RESULT_HIDDEN_ABILITY`, and `ABILITY_SWAPPER_RESULT_NO_ORDINARY_SWAP`.

In `src/script_pokemon_util.c`, include `constants/ability_swapper.h`. Add a static helper that reads `gSpecialVar_0x8004`, validates the selected party Pokemon, determines the alternate ordinary ability slot, and buffers `gStringVar1` with the nickname, `gStringVar2` with the current ability name, and `gStringVar3` with the target ability name when eligible. Add one special for preview and one special for payment plus mutation. The payment special must check `CheckBagHasItem(ITEM_HEART_SCALE, 1)` immediately before mutation, call `Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE)`, set `MON_DATA_ABILITY_NUM`, and then remove exactly one Heart Scale.

In `data/specials.inc`, expose both specials with `, 1` so scripts can branch on result codes. In `include/script_pokemon_util.h`, add prototypes for tests and specials.

In `data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc`, add a new `FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapper` script. It should lock, face the player, explain ordinary ability swapping for one Heart Scale, check that the player has a Pokemon and Heart Scale, call `ChoosePartyMon`, preview the exact change with a yes/no message, and only call the payment special after confirmation. It should return to mon selection after ineligible selections and say a short goodbye on cancellation.

In `data/maps/FallarborTown_MoveRelearnersHouse/map.json`, add a second object event beside the Move Relearner using a distinct existing NPC graphic and a non-overlapping coordinate.

Add `test/ability_swapper.c` with deterministic tests that create party Pokemon, set ability numbers, clear or add Heart Scales, and call the new specials directly. The tests should cover an eligible swap, the reverse eligible swap, no Heart Scale, single ordinary ability species, duplicate/missing target behavior where practical, Egg refusal, Hidden Ability refusal, and no item loss on every refusal.

Update `docs/gameplay_changes_public.md`, `docs/gameplay_changes_blind.md`, and `PATCH_NOTES.md`. Preserve existing dirty changes and prepend new patch notes.

## Concrete Steps

Work from the repository root: `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Use `rg` and `sed` to inspect local conventions. Make edits with `apply_patch`. Run a targeted test first:

    make check TESTS="Ability Swapper"

If the targeted test command is unavailable or too broad for the environment, run the narrowest supported test command and record the result. Because this changes map script data and specials, run at least:

    make

If shared script or Pokemon helper behavior appears risky, also run:

    make check

## Validation and Acceptance

Acceptance requires current-state evidence for each behavior:

An eligible ordinary-slot Pokemon previews the target ability name before payment. On confirmation, the selected Pokemon's `MON_DATA_ABILITY_NUM` changes to the alternate ordinary slot and `CheckBagHasItem(ITEM_HEART_SCALE, 1)` reflects that exactly one Heart Scale was consumed.

A Pokemon with no alternate ordinary ability, an Egg, an invalid or cancelled party slot, a Pokemon currently on the Hidden Ability slot, or a party with no Heart Scale must not change ability and must not consume a Heart Scale.

The Fallarbor Move Relearner house must contain a second, visually distinct NPC object with its own script. The public gameplay notes must describe the general ability-swap rule without revealing the exact location; the blind-friendly notes must mention the Fallarbor Move Relearner house placement.

## Idempotence and Recovery

The code and data edits are additive and can be reapplied safely by checking for the new constants, specials, script labels, and object event before adding duplicates. If tests fail, inspect the named result code first, then verify `MON_DATA_ABILITY_NUM`, species ability slots, and bag state. Avoid reverting unrelated dirty files; preserve existing pending changes in `PATCH_NOTES.md`, gameplay notes, and learnset files.

## Artifacts and Notes

Current relevant dirty files before this work began:

    M PATCH_NOTES.md
    M docs/gameplay_changes_public.md
    M src/data/pokemon/level_up_learnsets/gen_9.h
    M src/data/pokemon/teachable_learnsets.h
    M tools/learnset_helpers/porymoves_files/sv.json

## Interfaces and Dependencies

New C interfaces expected by the end of the work:

    // include/script_pokemon_util.h
    u16 AbilitySwapper_TryPreview(void);
    u16 AbilitySwapper_TrySwap(void);

New script specials expected by the end of the work:

    def_special AbilitySwapper_TryPreview, 1
    def_special AbilitySwapper_TrySwap, 1

Revision Note (2026-06-06): Initial plan created after repository research showed this feature spans C specials, map script data, tests, player notes, and patch notes.
