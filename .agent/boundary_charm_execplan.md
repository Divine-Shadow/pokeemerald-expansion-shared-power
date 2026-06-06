# Implement the Extinction Charm

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md`. It is self-contained so a contributor can restart from only this file and the current worktree.

## Purpose / Big Picture

The Extinction Charm is a toggleable Key Item for players using a one-capture-per-location rule. A location means the same map section the game writes into a Pokemon's caught-at entry, such as Route 102, Granite Cave, or Route 127 Underwater. When the charm is ON, normal random wild encounters should not start in a location that already has a captured Pokemon recorded. When the charm is OFF, normal encounters should behave like vanilla.

This lets a player use the game itself to enforce a nuzlocke-style "one capture per caught-at location" rule without preventing strategic choices. Static encounters still count once caught, but this plan does not require blocking a static before battle; a player can catch a normal Pokemon in that location first and then decline or skip the static by their own rules.

## Progress

- [x] (2026-06-05T02:44Z) Read repository gameplay guidance, `.agent/PLANS.md`, current Highlander Charm worktree state, item constants, caught-location storage, wild encounter paths, Sweet Scent/Rock Smash signal hooks, and the battle catch handoff.
- [x] (2026-06-05T02:44Z) Created this ExecPlan with the item name, save model, encounter scope, capture-claim hook, and validation expectations.
- [x] (2026-06-05T03:01Z) Added the Extinction Charm item, saved toggle, saved claimed-location bitset, and new-game initialization.
- [x] (2026-06-05T03:01Z) Recorded claims after successful captures and used the claim set to suppress normal random encounters while the charm is ON.
- [x] (2026-06-05T03:01Z) Added focused tests, player-facing docs, and patch notes.
- [x] (2026-06-05T03:01Z) Ran validation, recorded evidence here, and audited every contract requirement.

## Surprises & Discoveries

- Observation: Pokemon caught-at locations are map sections, not raw map ids.
  Evidence: `src/pokemon.c` writes `GetCurrentRegionMapSectionId()` into `MON_DATA_MET_LOCATION` when creating Pokemon, and `GetCurrentRegionMapSectionId()` reads the current map header's `regionMapSectionId`.

- Observation: `MAPSEC_COUNT` and `MAPSEC_NONE` exist in the generated region-map section constants.
  Evidence: `include/constants/region_map_sections.h` ends with `MAPSEC_NONE` and `MAPSEC_COUNT`, plus special met locations `METLOC_SPECIAL_EGG`, `METLOC_IN_GAME_TRADE`, and `METLOC_FATEFUL_ENCOUNTER` outside the normal map-section range.

- Observation: Successful captures are handed to the player in one shared battle script command.
  Evidence: `data/battle_scripts_2.s` calls `givecaughtmon` after a successful ball throw, and `src/battle_script_commands.c` implements it in `Cmd_givecaughtmon` by obtaining the caught battler's `struct Pokemon` and passing it to `GiveMonToPlayer`.

- Observation: Existing Highlander Charm work already provides patterns for toggleable Key Items and normal encounter suppression.
  Evidence: `src/item_use.c` has `ItemUseOutOfBattle_HighlanderCharm`, `src/wild_encounter.c` has `TryStartHighlanderCharmEmptyEncounterScript`, and `data/scripts/field_move_scripts.inc` already handles a special Rock Smash no-encounter result.

## Decision Log

- Decision: Name the item `Extinction Charm`.
  Rationale: The rule is about location boundaries rather than species or routes only. The name is short enough for item UI and avoids implying only numbered routes count.
  Date/Author: 2026-06-05 / Codex.

- Decision: Store location claims as a `SaveBlock3` bitset indexed by map section id.
  Rationale: `SaveBlock3` is already used for project-specific saved feature state, and a bitset over `MAPSEC_COUNT` is compact while matching the caught-at location identity.
  Date/Author: 2026-06-05 / Codex.

- Decision: Record claims after successful captures regardless of whether the Extinction Charm is currently ON.
  Rationale: The rule is based on captured history, not item activation history. Turning the item on later should still respect locations already claimed by earlier captures.
  Date/Author: 2026-06-05 / Codex.

- Decision: Suppress only normal random encounter paths while the charm is ON.
  Rationale: The contract excludes roamers, outbreaks, gifts, trainer battles, and scripted/static encounters unless a simple optional static blocker appears. Capturing a static still claims its location because the shared capture hook records it after the battle succeeds.
  Date/Author: 2026-06-05 / Codex.

## Outcomes & Retrospective

Implementation is complete and targeted validation passed. The work now has a saved item toggle, persistent location-claim bitset, capture bookkeeping from the shared successful-catch command, normal encounter suppression for land, water, fishing, Rock Smash, and Sweet Scent, a player-facing suppression message, focused tests, and gameplay notes.

The broad test suite still has unrelated project failures in Condition Coach, Drought, AI switching, Protosynthesis, and Limber tests. Every Extinction Charm test passed in both the targeted run and the broad run.

## Context and Orientation

This repository is a `pokeemerald-expansion` C project. Item constants live in `include/constants/items.h`, item data lives in `src/data/items.h`, item-use declarations live in `include/item_use.h`, and item-use functions live in `src/item_use.c`. Save data structs live in `include/global.h`, new-game initialization lives in `src/new_game.c`, and save serialization lives in `src/save.c`.

The game's caught-at value is `MON_DATA_MET_LOCATION`. For ordinary Pokemon creation, `src/pokemon.c` sets it to `GetCurrentRegionMapSectionId()`, and `src/overworld.c` implements that helper by reading the current map header's `regionMapSectionId`. A map section is the user-facing location value used by map names and caught-location display; multiple map ids can share one map section. That is why this feature must not key claims by raw map group/map number.

Normal random encounters are generated in `src/wild_encounter.c`. The relevant paths are walking grass/cave/land encounters, surfing water encounters, Rock Smash encounters, Sweet Scent encounters, and fishing encounters. Roamers and mass outbreaks are selected before the regular land/water random generation path, so leaving those calls outside the Extinction Charm check keeps them unaffected. Static and scripted wild encounters start through battle setup functions rather than the regular random encounter roll, so they are not blocked by this plan unless a later optional hook is deliberately added.

## Plan of Work

First, add a small `boundary_charm` feature module with helpers to read and write saved state. The module should expose whether the charm is active, set/toggle helpers for item use and tests, helpers to clear and claim map sections, a helper to ask whether a map section is claimed, a helper to ask whether the current location suppresses normal encounters, and a helper to start the clear signal script when suppression happens. The saved state should be a boolean active flag plus a bitset in `struct SaveBlock3`.

Second, add `ITEM_BOUNDARY_CHARM` after `ITEM_HIGHLANDER_CHARM` and increment `ITEMS_COUNT`. Define the item in `src/data/items.h` as an important Key Item with a concise description and a field-use function. Give it to new games in the Key Items pocket and reset the active state and claim bitset to empty in `src/new_game.c`. Use Highlander Charm's item icon and palette unless a better existing icon is clearly available.

Third, add item-use text and the field-use function. The function should follow the Highlander/Exp. Share toggle pattern: toggle saved state, play an ON/OFF sound, and display a message for either bag use or registered-key-item use.

Fourth, record claims after captures. Add a call in `Cmd_givecaughtmon` after `GiveMonToPlayer(caughtMon)` has succeeded or attempted to store the Pokemon. The helper should read `MON_DATA_MET_LOCATION` from the caught Pokemon and ignore `MAPSEC_NONE` and special met locations outside the `MAPSEC_COUNT` range. This catches regular wild captures and static captures because both pass through the successful catch script.

Fifth, suppress normal random encounters while the charm is ON and the current map section is claimed. Add a `WILD_CHECK_BOUNDARY` flag to the normal `TryGenerateWildMon` call sites for land, water, and Rock Smash, and add a fishing-specific check before `TryGenerateFishingWildMon` chooses a fish. Do not add this flag to roamer, mass outbreak, Battle Pike/Pyramid replacement, static, gift, or trainer paths. When suppression happens, set an Extinction Charm suppressed state and route walking/Sweet Scent/fishing through a script message; for Rock Smash, return a new `WILD_ENCOUNTER_RESULT_BOUNDARY_CLAIMED` so the field move script can show the same message.

Sixth, add focused tests. Tests should prove the saved toggle is independent of bag presence, OFF state preserves normal generation even in claimed locations, captured Pokemon claim their met location, claimed locations suppress regular generation while ON, unclaimed locations still generate, static-like captured Pokemon claim their location through the same capture-claim helper, shared map sections collapse to the same claim bit, and excluded paths are unaffected by using helper-level tests where full battle setup is not practical.

Finally, update `docs/gameplay_changes_public.md`, `docs/gameplay_changes_blind.md` if needed, and `PATCH_NOTES.md`. Run targeted tests, item description validation, a normal ROM build, and a broad check if practical. Record exact outputs in this plan.

## Concrete Steps

Run commands from the repository root `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Useful research commands:

    rg -n "MON_DATA_MET_LOCATION|GetCurrentRegionMapSectionId|Cmd_givecaughtmon|TryGenerateWildMon|FishingWildEncounter|SweetScentWildEncounter|RockSmashWildEncounter" src include test data
    rg -n "Highlander|HIGHLANDER|ItemUseOutOfBattle_HighlanderCharm|SaveBlock3|ITEM_HIGHLANDER_CHARM" include src test data docs PATCH_NOTES.md

Validation commands to run after implementation:

    make -j"$(nproc)" TESTS="Extinction Charm" check
    make -j"$(nproc)" TESTS="Item descriptions fit" check
    make -j"$(nproc)" NO_MULTIBOOT=1

If the local host cannot run generated tools, use Docker:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Extinction Charm" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Item descriptions fit" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1

## Validation and Acceptance

Acceptance requires evidence for every requested behavior. With the saved Extinction Charm state OFF, claimed locations must still produce normal random encounters. With the saved state ON, a claimed current map section must suppress normal land, water, fishing, Rock Smash, and Sweet Scent encounters with a clear message instead of silently failing or starting a duplicate encounter. An unclaimed current map section must still allow normal encounters. A captured Pokemon must permanently claim its `MON_DATA_MET_LOCATION`, including a static Pokemon after it is caught. Two maps that share the same map-section caught-at value must share one claim.

The feature is not accepted if it keys claims by raw map id, if the item defaults ON, if claims are lost across save/load, if it blocks roamers/outbreaks/gifts/trainers/scripted or static wild battles before capture, or if it depends on current party/box membership rather than persistent capture history.

## Idempotence and Recovery

The planned edits are additive and local to save state, a new feature module, item data, item-use messaging, capture bookkeeping, encounter suppression, scripts, tests, player notes, and patch notes. Re-running tests and builds is safe. The worktree already contains uncommitted Highlander Charm changes; do not revert them. If a validation failure appears outside Extinction Charm or Highlander-adjacent code, record it as residual risk only after verifying it reproduces independently or is already present in the broad-suite summary.

## Artifacts and Notes

Initial evidence:

    git status --short
    M .agent/highlander_charm_execplan.md
    M PATCH_NOTES.md
    M docs/gameplay_changes_blind.md
    M docs/gameplay_changes_public.md
    M include/global.h
    M include/item_use.h
    M include/strings.h
    M include/wild_encounter.h
    M src/data/items.h
    M src/item_use.c
    M src/new_game.c
    M src/player_pc.c
    M src/strings.c
    M src/wild_encounter.c
    M test/wild_encounter.c

    date -u +%Y-%m-%dT%H:%MZ
    2026-06-05T02:44Z

## Interfaces and Dependencies

Validation evidence:

    git diff --check
    # passed with no output

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Extinction Charm" NO_MULTIBOOT=1 check
    # passed: 8/8 Extinction Charm tests

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Item descriptions fit" NO_MULTIBOOT=1 check
    # passed: 1/1 item-description layout test

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1
    # passed: built pokeemerald.gba

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check
    # failed with unrelated broad-suite failures:
    # - test/battle/ability/drought.c:22, Drought sets up sun for 5 turns (Gen6+)
    # - test/battle/ai/ai_switching.c:1201, AI_FLAG_SMART_SWITCHING switch choice expectation
    # - test/condition_coach.c:164, Condition Coach rejects Eggs
    # - test/battle/ability/protosynthesis.c:80, Protosynthesis popup once during sunlight
    # - test/battle/ability/limber.c:32, Limber prevents paralysis from Thunder Wave
    # totals: 5 failed, 19 known failing, 3 assumptions failed, 789 TODO, 3288 passed, 4104 total

Contract audit:

- Toggleable Key Item: `ITEM_BOUNDARY_CHARM` starts in the Key Items pocket, is OFF on new games, and toggles saved state through `ItemUseOutOfBattle_BoundaryCharm`.
- One capture per caught-at location: claims are stored in `SaveBlock3` as a bitset keyed by map section, the same value used by `MON_DATA_MET_LOCATION`.
- Captures count persistently: `Cmd_givecaughtmon` claims the caught Pokemon's met location after `GiveMonToPlayer` succeeds, so regular and static captures count after capture.
- Normal encounters only: land, water, fishing, Rock Smash, and Sweet Scent random generation are suppressed while ON in claimed locations; roamer, outbreak, facility, gift, trainer, and scripted/static pre-battle paths are not given the Boundary check.
- OFF behavior: focused tests prove claimed locations still generate normal encounters while the saved toggle is OFF.
- Static behavior: focused tests prove static-like captured Pokemon claim their location, while the implementation deliberately does not block static battles before capture.

The final feature should provide a small public API, likely in `include/boundary_charm.h`:

    bool32 IsBoundaryCharmActive(void);
    void SetBoundaryCharmActive(bool32 active);
    void ToggleBoundaryCharmActive(void);
    void ClearBoundaryCharmClaims(void);
    bool32 IsBoundaryCharmLocationClaimed(u8 mapSec);
    void ClaimBoundaryCharmLocation(u8 mapSec);
    void ClaimBoundaryCharmLocationForMon(struct Pokemon *mon);
    bool32 IsBoundaryCharmCurrentLocationClaimed(void);
    bool32 ShouldSuppressBoundaryCharmEncounter(void);
    bool32 WasBoundaryCharmEncounterSuppressed(void);
    bool32 TryStartBoundaryCharmEncounterSuppressedScript(void);
    void ItemUseOutOfBattle_BoundaryCharm(u8 taskId);

Only `ItemUseOutOfBattle_BoundaryCharm` should be normal gameplay's toggle entry point. Tests may call setters and claim helpers directly. Capture code should call `ClaimBoundaryCharmLocationForMon(caughtMon)` after a successful catch has a real Pokemon object. Wild encounter code should call `ShouldSuppressBoundaryCharmEncounter()` only on normal random encounter paths.

Revision Note (2026-06-05): Initial ExecPlan created from the OCVA scope and current repository evidence. The design deliberately records static captures as location claims after capture while leaving pre-battle static blocking out of the required implementation.
