# Implement Radiant Charm as a shiny encounter toggle

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md`. It is self-contained so a future contributor can finish the feature from this file and the current repository alone.

## Purpose / Big Picture

The Radiant Charm lets a player opt into a cosmetic shiny mode for encounters without changing which Pokemon they are allowed to meet. A new excited kid in Littleroot Town gives the player the Radiant Charm once. The Key Item starts off, can be toggled from the bag, and while on it makes eligible normal wild and scripted/static wild Pokemon shiny only after the encounter has already passed normal rules such as repel, Highlander Charm filtering, and Extinction Charm suppression.

This is not a shiny odds redesign. It is a persistent mode implemented behind a small helper API so it can later become "next encounter only" or limited-use without rewriting every encounter caller.

## Progress

- [x] (2026-06-06T19:17Z) Read the repository gameplay updater skill, `.agent/PLANS.md`, current charm code, save layout, item use code, wild/static encounter creation, Littleroot map scripts, flag ranges, text tests, and player notes.
- [x] (2026-06-06T19:17Z) Decided the implementation shape: add a standalone Radiant Charm module with saved toggle state and one central shiny-application helper.
- [x] (2026-06-06T19:22Z) Added item constant, save field, item data, toggle text, and bag use behavior for `Radiant Charm`.
- [x] (2026-06-06T19:22Z) Hooked Radiant Charm into normal wild and scripted/static wild creation after each enemy Pokemon has been created.
- [x] (2026-06-06T19:22Z) Added a Littleroot Town kid object with one-time gift dialogue guarded by `FLAG_RECEIVED_RADIANT_CHARM`.
- [x] (2026-06-06T19:22Z) Added focused tests for toggle persistence, off/on shiny behavior, static encounters, gift exclusion, and interaction with Highlander/Extinction blocking.
- [x] (2026-06-06T19:22Z) Updated public and blind-friendly gameplay documentation plus `PATCH_NOTES.md`.
- [x] (2026-06-06T19:39Z) Ran targeted Radiant tests, text fit checks, Highlander/Extinction regression checks, a ROM build, and recorded verification evidence.

## Surprises & Discoveries

- Observation: The repository already has an official `ITEM_SHINY_CHARM`, so this feature must use a separate `ITEM_RADIANT_CHARM` constant and avoid changing the existing shiny-roll mechanic.
  Evidence: `include/constants/items.h` defines `ITEM_SHINY_CHARM 691`; `src/pokemon.c` uses it during regular `CreateBoxMon` shiny generation.
- Observation: Normal wild generation has a clean post-validation hook point.
  Evidence: `src/wild_encounter.c` chooses species/slot/level, applies repel and charm suppression, then calls `CreateWildMon(...)` only after the encounter is valid.
- Observation: Static wild scripts generally route through `CreateScriptedWildMon` and `CreateScriptedDoubleWildMon`, with `CreateEnemyEventMon` as another enemy-party event creation path.
  Evidence: `src/scrcmd.c` handles `setwildbattle` by calling the scripted wild helpers, and `data/specials.inc` exposes `CreateEnemyEventMon`.
- Observation: Gift Pokemon explicitly set shininess through `ScriptGiveMonParameterized`; Radiant Charm should not be called from that path.
  Evidence: `src/script_pokemon_util.c` passes `isShiny` to `SetMonData(&mon, MON_DATA_IS_SHINY, &isShiny)` before giving the mon to the player.
- Observation: Direct local test runs are blocked in this NixOS environment by dynamically linked repo tools, so validation must use the Docker builder.
  Evidence: local `make check TESTS="Radiant Charm"` failed before compiling Radiant code with `Could not start dynamically linked executable: tools/scaninc/scaninc` and the NixOS stub-ld guidance.
- Observation: Docker validation needs `NO_MULTIBOOT=1` in this workspace.
  Evidence: a Docker test run without `NO_MULTIBOOT=1` reached link and failed with duplicate multiboot symbols between `data/multiboot_stubs.o` and multiboot blobs; reruns with `NO_MULTIBOOT=1` passed.

## Decision Log

- Decision: Implement `Radiant Charm` as a new key item rather than reusing the existing Shiny Charm.
  Rationale: The existing Shiny Charm changes odds; the requested feature is a persistent cosmetic guaranteed-shiny mode and should remain independently tunable.
  Date/Author: 2026-06-06 / Codex
- Decision: Store `radiantCharmActive` in `SaveBlock3` next to the Highlander and Extinction charm toggles.
  Rationale: Those charms are saved toggleable Key Items, and matching their save location makes the state persist while keeping related mode flags together.
  Date/Author: 2026-06-06 / Codex
- Decision: Add `ShouldRadiantCharmForceShiny()` and `ApplyRadiantCharmToEncounterMon(struct Pokemon *mon)` as the central behavior seam.
  Rationale: Callers do not need to know whether the future model is persistent, next-encounter, or limited-use. They only ask whether this encounter mon should be forced shiny.
  Date/Author: 2026-06-06 / Codex
- Decision: Hook after Pokemon creation, not during slot or personality selection.
  Rationale: Setting `MON_DATA_IS_SHINY` preserves species, level, personality, OT ID, nature, IVs, held item, encounter slot, location, and all upstream legality decisions.
  Date/Author: 2026-06-06 / Codex
- Decision: Use a saved `FLAG_RECEIVED_RADIANT_CHARM` from an unused system flag slot for the Littleroot gift.
  Rationale: The gift must not duplicate across sessions. Temporary and special flags are not durable save-state flags, while an unused system flag is already part of saved event state.
  Date/Author: 2026-06-06 / Codex

## Outcomes & Retrospective

Radiant Charm is implemented and verified. The item is a saved toggleable Key Item named `Radiant Charm`; the new Littleroot kid gives it once after a successful `giveitem`; normal wild and scripted/static wild encounter creation calls the central Radiant helper only after the encounter Pokemon exists; and gifts remain outside the helper path.

Validation evidence from `/home/bayesartre/dev/pokeemerald-expansion-shared-power`:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make check TESTS="Radiant Charm" NO_MULTIBOOT=1

Passed 8/8 tests: saved toggle independence, disabled helper behavior, normal wild shiny forcing without species/level changes, scripted static shiny forcing, double static shiny forcing, Highlander blocking, Extinction blocking, and gift exclusion.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make check TESTS="test/text.c" NO_MULTIBOOT=1

Passed 37/37 text fit tests, including item names and item descriptions.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make check TESTS="test/wild_encounter.c" NO_MULTIBOOT=1

Passed 6/6 Highlander/wild encounter regression tests.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make check TESTS="test/boundary_charm.c" NO_MULTIBOOT=1

Passed 8/8 Extinction Charm regression tests.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1

Built `pokeemerald.gba` successfully. Final memory report: EWRAM 230200 B / 256 KB, IWRAM 28705 B / 32 KB, ROM 24707204 B / 32 MB.

## Context and Orientation

Item constants live in `include/constants/items.h`; item data lives in `src/data/items.h`; out-of-battle Key Item use lives in `src/item_use.c` and is declared in `include/item_use.h`. Player-facing toggle strings live in `src/strings.c` and are declared in `include/strings.h`.

Saved mode state for recent charms lives in `struct SaveBlock3` in `include/global.h`. Highlander Charm exposes its toggle from `include/wild_encounter.h` because the behavior is implemented in `src/wild_encounter.c`. Extinction Charm has its own `include/boundary_charm.h` and `src/boundary_charm.c` module. Radiant Charm should follow the standalone module pattern because its behavior is shared by wild and static creation.

Normal wild Pokemon are created in `src/wild_encounter.c`. The important function is `CreateWildMon(u16 species, u8 level)`, which writes to `gEnemyParty[0]`. Static wild Pokemon created by event scripts are handled in `src/script_pokemon_util.c` by `CreateScriptedWildMon(...)` and `CreateScriptedDoubleWildMon(...)`. A second enemy event helper, `CreateEnemyEventMon`, lives in `src/pokemon.c` and also writes to `gEnemyParty[0]`.

Shiny state can be changed without changing personality or OT ID by setting `MON_DATA_IS_SHINY`. Existing tests in `test/pokemon.c` prove that `SetMonData(..., MON_DATA_IS_SHINY, ...)` flips shininess independently from PID and OTID.

Littleroot Town objects live in `data/maps/LittlerootTown/map.json`; local dialogue and scripts live in `data/maps/LittlerootTown/scripts.inc`. One-time gifts usually check a `FLAG_RECEIVED_*` flag, call `giveitem`, and set the flag after success.

Player-facing notes are split by audience. The broad Radiant Charm rule belongs in `docs/gameplay_changes_public.md`. The exact Littleroot NPC availability belongs in `docs/gameplay_changes_blind.md` because item gift locations are discovery details.

## Plan of Work

First, add the small Radiant Charm module. Create `include/radiant_charm.h` and `src/radiant_charm.c`. The header declares `IsRadiantCharmActive`, `SetRadiantCharmActive`, `ToggleRadiantCharmActive`, `ShouldRadiantCharmForceShiny`, and `ApplyRadiantCharmToEncounterMon`. The source reads and writes `gSaveBlock3Ptr->radiantCharmActive`; `ApplyRadiantCharmToEncounterMon` sets `MON_DATA_IS_SHINY` to true only when `ShouldRadiantCharmForceShiny()` returns true.

Second, add item plumbing. Add `ITEM_RADIANT_CHARM` after `ITEM_BOUNDARY_CHARM` and increment `ITEMS_COUNT`. Add `radiantCharmActive` to `SaveBlock3`. Add item use declaration and implementation that mirrors Highlander/Extinction toggle behavior. Add toggle strings. Add item data using the Catching Charm icon palette and a short description that fits text tests. Do not add it to new-game starting items because the NPC gives it.

Third, hook encounter creation. In `CreateWildMon`, call `ApplyRadiantCharmToEncounterMon(&gEnemyParty[0])` after both the Cute Charm and normal creation paths. In `CreateScriptedWildMon`, call it after held item assignment. In `CreateScriptedDoubleWildMon`, call it for both `gEnemyParty[0]` and `gEnemyParty[1]` after each mon's held item assignment. In `CreateEnemyEventMon`, call it after held item assignment. Do not call it from trainer generation, gift Pokemon, egg creation, Battle Tower/factory generation, or generic party creation.

Fourth, add the Littleroot NPC. Add a new object to `data/maps/LittlerootTown/map.json` with a child sprite such as `OBJ_EVENT_GFX_LITTLE_BOY`, a safe town coordinate, and `LittlerootTown_EventScript_RadiantCharmKid`. Add `FLAG_RECEIVED_RADIANT_CHARM` in `include/constants/flags.h` using an unused saved system flag. Add a script that checks the flag, talks excitedly about shiny Pokemon, gives `ITEM_RADIANT_CHARM`, sets the flag after successful gift, and has repeat dialogue after the gift.

Fifth, add tests. Prefer a dedicated `test/radiant_charm.c`. Tests must prove the saved toggle is independent of bag possession, off mode does not force a non-shiny mon shiny, on mode forces a normal wild mon shiny without changing species or level, on mode forces scripted/static wild mons shiny, Radiant does not bypass Highlander empty-pool filtering, Radiant does not bypass Extinction claimed-location suppression, and Radiant does not affect `ScriptGiveMon` gifts. Text tests should cover item name/description fit.

Sixth, update docs and patch notes. Public docs describe the mechanic without giving the exact NPC location. Blind-friendly docs mention the Littleroot Town kid. `PATCH_NOTES.md` gets concise top entries with `commit pending` until the final commit hash exists.

## Concrete Steps

Run all commands from `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

To inspect relevant files:

    rg -n "ITEM_HIGHLANDER_CHARM|ITEM_BOUNDARY_CHARM|CreateWildMon|CreateScriptedWildMon|CreateEnemyEventMon" include src data test docs

After implementation, run targeted tests:

    make check TESTS="Radiant Charm"
    make check TESTS="test/text.c"

Then run the normal ROM build:

    make -j"$(nproc)"

If the local toolchain is unavailable, use the Docker builder command from `AGENTS.md`:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)"

## Validation and Acceptance

The feature is accepted only when current evidence proves all of these behaviors:

Radiant Charm is a Key Item named exactly `Radiant Charm`, is obtainable once from the new Littleroot kid, and can be toggled on/off from the Key Items pocket. Its toggle starts off and persists in save data.

With Radiant off, calling the central helper on a known non-shiny Pokemon leaves it non-shiny. With Radiant on, normal wild creation and scripted/static wild creation produce shiny enemy Pokemon after species and level have already been selected.

Highlander Charm and Extinction Charm still block encounters before Radiant can make anything shiny. A caught-only Highlander pool still produces no battle. A claimed Extinction location still produces no battle.

Gift Pokemon and eggs are not made shiny by Radiant Charm. Trainer/facility generated parties are not touched by the new helper unless they already use the normal wild/static wild creation functions named in this plan.

Item name, item description, and toggle messages pass text fit tests. A normal ROM build completes.

## Idempotence and Recovery

These edits are additive and safe to re-run. If item constants fail to compile, check that `ITEMS_COUNT` is exactly one greater than the highest real item ID and that `ITEM_FIELD_ARROW` still equals `ITEMS_COUNT`. If map scripts fail to assemble, check that the new `FLAG_RECEIVED_RADIANT_CHARM`, script label, and item constant names are visible to the script assembler.

Do not revert unrelated dirty files in the worktree. Before finalizing, review `git diff --stat` and `git diff` for touched Radiant files plus `PATCH_NOTES.md`, docs, and the ExecPlan.

## Artifacts and Notes

Initial worktree before Radiant edits included unrelated modifications to `PATCH_NOTES.md`, `docs/gameplay_changes_public.md`, `src/data/pokemon/level_up_learnsets/gen_9.h`, `src/data/pokemon/teachable_learnsets.h`, and `tools/learnset_helpers/porymoves_files/sv.json`. Those must be preserved.

## Interfaces and Dependencies

At the end of implementation these interfaces must exist:

    include/radiant_charm.h
    bool32 IsRadiantCharmActive(void);
    void SetRadiantCharmActive(bool32 active);
    void ToggleRadiantCharmActive(void);
    bool32 ShouldRadiantCharmForceShiny(void);
    void ApplyRadiantCharmToEncounterMon(struct Pokemon *mon);

The helper implementation must use:

    bool32 isShiny = TRUE;
    SetMonData(mon, MON_DATA_IS_SHINY, &isShiny);

The item use function must exist:

    void ItemUseOutOfBattle_RadiantCharm(u8 taskId);

The new flag must exist:

    #define FLAG_RECEIVED_RADIANT_CHARM ...

The new item must exist:

    #define ITEM_RADIANT_CHARM ...

Revision Note (2026-06-06): Initial Radiant Charm execution plan created after reading current charm, encounter, item, map, flag, and test patterns.

Revision Note (2026-06-06T19:22Z): Updated progress after implementing item plumbing, encounter hooks, Littleroot gift script, focused tests, gameplay docs, and patch notes. Validation remains pending.

Revision Note (2026-06-06T19:39Z): Recorded final validation evidence and environment caveats. Radiant Charm meets the acceptance criteria in this plan.
