# Add Condition Coaches to Pokemon Centers

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md` from the repository root. Any contributor should be able to resume from this file alone.

## Purpose / Big Picture

Players should be able to prepare intentional status-based strategies without hunting for wild encounters before every important battle. After this change, a recurring Condition Coach NPC appears in each 1F Pokemon Center and can safely apply burn, poison, paralysis, a one-turn Rest-style sleep wakeup, or clear status from a selected non-egg, conscious party Pokemon. The NPC also frames the service narratively as controlled competitive conditioning rather than harm, and gives tailored advice when the selected Pokemon has a status-synergy ability or a curing held item.

The visible proof is that each Pokemon Center 1F map contains the new custom coach sprite, the shared service script opens a party selection flow, and focused tests show the C/script status helper applies the intended status bytes.

## Progress

- [x] (2026-05-03T04:30Z) Read repository instructions, the gameplay updater skill, `docs/design/journey_npcs`, Pokemon Center map conventions, script menu conventions, object-event sprite registration, and existing debug status scripts.
- [x] (2026-05-03T04:30Z) Decided the service should be a shared common script plus a C helper rather than duplicated per-map scripts.
- [x] (2026-05-03T04:42Z) Created the Condition Coach design record under `docs/design/journey_npcs`.
- [x] (2026-05-03T04:42Z) Added constants and C helpers for status choices, status application, validation, and tailored hint classification.
- [x] (2026-05-03T04:42Z) Added the shared Condition Coach script and included it in `data/event_scripts.s`.
- [x] (2026-05-03T04:42Z) Created and registered a custom `OBJ_EVENT_GFX_CONDITION_COACH` overworld sprite.
- [x] (2026-05-03T04:46Z) Placed one Condition Coach object event in every Pokemon Center 1F map.
- [x] (2026-05-03T04:48Z) Added public and blind-friendly gameplay notes.
- [x] (2026-05-03T04:51Z) Added focused tests for the status helper.
- [x] (2026-05-03T05:02Z) Fixed compile blockers in the test Egg setup and object graphics info declaration.
- [x] (2026-05-03T05:03Z) Ran focused tests for the status helper; all 11 Condition Coach tests passed.
- [x] (2026-05-03T05:13Z) Built an automation/probe ROM and captured mGBA sprite-preview snapshots for the coach.
- [x] (2026-05-03T05:18Z) Verified all 16 Pokemon Center 1F maps contain the coach object graphics id and shared script.
- [x] (2026-05-03T05:18Z) Ran hygiene checks: `git diff --check` and `python3 -m py_compile tools/mgba/mgba_lua_spike.py` passed.
- [x] (2026-05-03T05:20Z) Built the default ROM with Docker to validate map data, scripts, and generated sprite assets.
- [x] (2026-05-03T05:23Z) Moved the save-effect request to the actual status write path, then reran focused tests and the default ROM build.
- [x] (2026-05-03T05:24Z) Updated this ExecPlan with validation evidence and final outcome.

## Surprises & Discoveries

- Observation: The repo already has a debug-only status script using `ChoosePartyMon` plus `setstatus1`, so selecting a party Pokemon from field script is proven.
  Evidence: `data/scripts/debug.inc` has `Debug_EventScript_InflictStatus1` with `special ChoosePartyMon`, `waitstate`, and `setstatus1 STATUS1_BURN, VAR_0x8004`.
- Observation: Sleep counters are raw status bits. `STATUS1_SLEEP_TURN(1)` is enough to simulate a Pokemon waking as soon as it tries to act.
  Evidence: `src/battle_util.c` decrements the sleep counter before move execution and clears sleep when the counter reaches zero.
- Observation: Barry already established the custom sprite insertion pattern this task can copy.
  Evidence: `OBJ_EVENT_GFX_BERRY_BARRY` is registered through `include/constants/event_objects.h`, `src/data/object_events/object_event_graphics.h`, `src/data/object_events/object_event_pic_tables.h`, `src/data/object_events/object_event_graphics_info.h`, `src/data/object_events/object_event_graphics_info_pointers.h`, and the palette table in `src/event_object_movement.c`.
- Observation: Pacifidlog already has a service NPC at `(12, 8)`, so the coach cannot use the otherwise common lower-right tile there.
  Evidence: `data/maps/PacifidlogTown_PokemonCenter_1F/map.json` uses `(12, 8)` for `PacifidlogTown_PokemonCenter_1F_EventScript_ExplosionTutor`; the coach is placed at `(11, 8)` on that map.
- Observation: The first sprite snapshot run rendered the coach as blank or fragmented because the new sheet lacked the object-event metatile conversion rule.
  Evidence: `condition_coach_overworld_south.png` initially contained no visible coach, while adding `graphics/object_events/pics/people/condition_coach.4bpp` to `graphics_file_rules.mk` with `-mwidth 2 -mheight 4`, forcing regeneration, and rebuilding produced complete south/north/west/east snapshots.

## Decision Log

- Decision: Implement the coach as a common NPC service available in every 1F Pokemon Center.
  Rationale: The user asked for an NPC in each Pokemon Center, and one shared script keeps behavior consistent while allowing map placement to stay simple.
  Date/Author: 2026-05-03, Codex.
- Decision: Include one-turn Rest-style sleep in the MVP as "Rest-wake sleep" rather than deferring it.
  Rationale: The battle engine already supports a one-turn sleep counter, so this is low implementation risk and directly matches the requested advanced option.
  Date/Author: 2026-05-03, Codex.
- Decision: Do not overwrite an existing major status. The coach asks the player to clear status first.
  Rationale: Major status conditions are mutually exclusive in battle, and an explicit clear option makes the service predictable and avoids accidental replacement.
  Date/Author: 2026-05-03, Codex.
- Decision: Use a custom recolored overworld sprite registered as its own object graphics id.
  Rationale: The user asked to use custom sprite powers, and a distinct id lets future map work identify this service without borrowing stock NPC ids.
  Date/Author: 2026-05-03, Codex.
- Decision: Use `(12, 8)` facing up for most Pokemon Centers and `(11, 8)` facing up in Pacifidlog.
  Rationale: The lower-right service position is away from doors, stairs, and nurses; Pacifidlog already uses `(12, 8)`, and `(11, 8)` is a proven adjacent service tile in other centers.
  Date/Author: 2026-05-03, Codex.

## Outcomes & Retrospective

Complete. The MVP adds a recurring Condition Coach to every 1F Pokemon Center, using a shared script and a C helper for safe status application. The service can apply burn, regular poison, paralysis, one-turn Rest-wake sleep, or clear an existing status from a conscious non-Egg party Pokemon. Success dialogue is tailored by held curing berries and status-synergy abilities.

Validation evidence:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check TESTS="Condition Coach"

Result: 11 passed, 11 total.

This focused test command was rerun after the final save-effect cleanup with the same 11 passed, 11 total result.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power && python3 -m py_compile tools/mgba/mgba_lua_spike.py'

Result: passed.

This default build was rerun after the final save-effect cleanup and passed.

    git diff --check

Result: passed.

    rg -l 'OBJ_EVENT_GFX_CONDITION_COACH' data/maps/*PokemonCenter_1F/map.json | wc -l
    rg -n 'OBJ_EVENT_GFX_CONDITION_COACH|Common_EventScript_ConditionCoach' data/maps/*PokemonCenter_1F/map.json | wc -l

Result: 16 maps and 32 total graphics/script matches.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms

Result: passed. The mGBA journey NPC sprite-preview run wrote `build/journey_npc_sprites/condition_coach/run_result.json` with `ok: true` and valid PNG snapshots for the combined preview plus south, north, west, and east facings.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1

Result: passed.

## Context and Orientation

Pokemon Center maps live under `data/maps/<MapName>/map.json`; each 1F Pokemon Center currently has a Nurse object at `(7, 2)` and normal NPC object events in the same `object_events` array. The common nurse script is in `data/scripts/pkmn_center_nurse.inc`, and map-local nurse scripts call it from each Pokemon Center script file.

The script engine can call C functions through `special` or `specialvar`; the function list is defined in `data/specials.inc`. Script commands and constants are available to event scripts through `data/event_scripts.s`, which includes many `include/constants/*.h` headers and later includes shared script files such as `data/scripts/pkmn_center_nurse.inc`.

Object-event graphics are registered in several C data files. `include/constants/event_objects.h` assigns numeric graphics ids and palette tags. `src/data/object_events/object_event_graphics.h` declares image and palette data. `src/data/object_events/object_event_pic_tables.h` maps an image sheet to animation frames. `src/data/object_events/object_event_graphics_info.h` describes sprite size, palette slot, shadow, and animation table. `src/data/object_events/object_event_graphics_info_pointers.h` maps the id to the graphics info. `src/event_object_movement.c` registers custom palettes so they can load in the overworld.

The player-facing note policy from `.codex/skills/pokeemerald-gameplay-updater/SKILL.md` treats the broad availability of a new preparation service as public, while exact NPC placement and dialogue are blind-friendly. This feature therefore updates both `docs/gameplay_changes_public.md` and `docs/gameplay_changes_blind.md`.

## Plan of Work

First, create `docs/design/journey_npcs/condition_coach.md` to define the character concept, player impact, service rules, narrative beats, tailored text categories, and asset needs. This keeps the recurring NPC documented alongside Barry and gives future journey/service NPCs a template.

Second, add `include/constants/condition_coach.h` with small enums for menu choices, result codes, and hint categories. Add declarations to `include/script_pokemon_util.h`, then implement the helper in `src/script_pokemon_util.c`. The helper reads the selected party slot from `gSpecialVar_0x8004` and the requested status choice from `gSpecialVar_0x8005`, validates the target, writes the status byte with `SetMonData`, records save effects with `Script_RequestEffects`, and stores a hint enum in `gSpecialVar_0x8006`.

Third, add `data/scripts/condition_coach.inc` and include it from `data/event_scripts.s`. The script should lock, face the player, explain the service, present a dynamic menu with burn, poison, paralysis, Rest-wake sleep, clear status, and exit, then call `ChoosePartyMon`. It should branch on result codes for cancel, egg, fainted, already statused, already clear, and successful application. On success, it should print the appropriate tailored text based on `VAR_0x8006`.

Fourth, create `graphics/object_events/pics/people/condition_coach.png` and `graphics/object_events/palettes/condition_coach.pal`, then register `OBJ_EVENT_GFX_CONDITION_COACH`. The sprite should read as a clinical sports trainer or battle-prep coach: visually tied to Pokemon Centers, but distinct from the Nurse.

Fifth, add a stationary Condition Coach object event to every map matching `data/maps/*PokemonCenter_1F/map.json`. The script should be `Common_EventScript_ConditionCoach`, the graphics id should be `OBJ_EVENT_GFX_CONDITION_COACH`, and placement should avoid doors, stairs, nurses, and existing NPCs. If a center already uses the lower-right area heavily, choose a nearby safe floor tile instead of forcing a single coordinate.

Sixth, add tests. Focus on the C helper rather than the interactive party menu, because the interactive party menu is not available in isolated tests. Tests should create party Pokemon, set `gSpecialVar_0x8004` and `gSpecialVar_0x8005`, call `ConditionCoach_TryApplyStatus`, and assert the resulting status byte and result code. Include burn, poison, paralysis, Rest-wake sleep, clear status, already-status rejection, egg rejection, and fainted rejection.

Seventh, update `PATCH_NOTES.md`, `docs/gameplay_changes_public.md`, and `docs/gameplay_changes_blind.md`. Then run focused tests with Docker, followed by a ROM build with `NO_MULTIBOOT=1`.

## Concrete Steps

Run all commands from `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Use `rg` and `sed` to inspect nearby examples before editing. Use `apply_patch` for source, script, docs, and JSON edits. Generated or binary image processing may be performed through the existing Docker builder image when shell tools are not enough.

Focused validation command:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check TESTS="Condition Coach"

Build validation command:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1

If the focused test filter does not match because the test runner uses file or suite names differently, run the nearest useful test file or the full `make check` command and record the actual command here.

## Validation and Acceptance

The feature is accepted when focused tests pass for the helper, the ROM build succeeds, all 16 Pokemon Center 1F maps contain an object event using `OBJ_EVENT_GFX_CONDITION_COACH` and `Common_EventScript_ConditionCoach`, and the public/blind notes accurately describe the new service without leaking exact dialogue unnecessarily.

Expected focused test evidence should include passing tests for burn, poison, paralysis, one-turn sleep, clear status, existing-status rejection, egg rejection, and fainted rejection.

Expected build evidence should end with a successful `pokeemerald.gba` build and no map-script, object graphics, palette, or text assembly errors.

## Idempotence and Recovery

All source edits are additive or narrowly scoped. Re-running the Docker build regenerates `.4bpp` and `.gbapal` outputs from PNG and PAL sources. If a map placement collides or fails to build, inspect the affected `map.json`, move only that coach object to a nearby free floor tile, and rebuild. If the status helper fails tests, keep the script unchanged and fix the helper first because every map uses the same service.

## Artifacts and Notes

Latest sprite proof artifacts:

    build/journey_npc_sprites/condition_coach/run_result.json
    build/journey_npc_sprites/condition_coach/condition_coach_all.png
    build/journey_npc_sprites/condition_coach/condition_coach_overworld_south.png
    build/journey_npc_sprites/condition_coach/condition_coach_overworld_north.png
    build/journey_npc_sprites/condition_coach/condition_coach_overworld_west.png
    build/journey_npc_sprites/condition_coach/condition_coach_overworld_east.png

Build artifacts such as `pokeemerald.gba`, `.sym`, screenshots, and generated logs must remain uncommitted.

## Interfaces and Dependencies

The final implementation must expose these C/script interfaces:

    include/constants/condition_coach.h
    enum ConditionCoachChoice
    enum ConditionCoachResult
    enum ConditionCoachHint

    include/script_pokemon_util.h
    u16 ConditionCoach_TryApplyStatus(void);

    data/scripts/condition_coach.inc
    Common_EventScript_ConditionCoach::

The helper uses `gSpecialVar_0x8004` for the selected party slot, `gSpecialVar_0x8005` for the requested condition choice, and writes `gSpecialVar_0x8006` with the hint category. It returns a `ConditionCoachResult` through `specialvar VAR_RESULT`.

Revision Note (2026-05-03): Initial plan created after repository inspection. The design uses a shared service script and a custom overworld sprite because those match the existing Pokemon Center and Barry sprite patterns.
