# Reach Starter Selection with Headless mGBA Before Migration

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan must be maintained according to `.agent/PLANS.md` in this repository. A contributor implementing this plan must proceed milestone-to-milestone until either the headless starter-selection proof succeeds, or a blocker is documented with enough evidence to sync with the operator about migrating the host FSM to a stricter language.

## Purpose / Big Picture

The current Python/Lua mGBA spike can boot a clean automation ROM, select a female player, enter the name `A`, and stop at truck control without taking Windows desktop focus. The next useful proof is first Pokemon selection. After this plan succeeds, an agent can run one headless command and prove the ROM reached `STARTER_CHOOSE_READY` from clean boot, with no GUI focus hijack and with artifacts that explain every accepted route state.

The scope is intentionally narrow. The goal is not to build a general navigation framework in Python. The goal is to learn whether the existing Python host plus Lua mGBA bridge can carry one deterministic clean-boot route through truck exit, Littleroot moving-in, clock setup, Route 101, and Birch's starter bag. If the route needs a richer typed FSM, general navigation model, or stronger invariant governance before starter selection is practical, this plan requires a blocker report and an operator sync before migration.

## Progress

- [x] (2026-04-27) Authored this ExecPlan after the headless female `A` truck checkpoint passed three consecutive runs under `mgba-headless`.
- [x] (2026-04-27 16:44 -04:00) Revalidated the current headless truck baseline from a freshly built `AUTOMATION_BEACON=1` ROM. `build/mgba_lua_spike_truck_baseline/run_result.json` ended at `TRUCK_CONTROL_READY` with `stageId=9`, `gender=2`, `nameLen=1`, `nameChar0=1`, `mapKind=1`, and `inputReady=1`.
- [x] (2026-04-27 16:47 -04:00) Inspected and documented the exact in-game route from truck control to first Pokemon selection using repository scripts and callbacks. The route table is in `Artifacts and Notes`.
- [x] (2026-04-27 16:47 -04:00) Decided to attempt the first Python `starter` mode with existing beacon fields only. `stageId`, `substageId`, `starterSelection`, `inputReady`, and `errorCode` cover the known intro, clock, Route 101, and starter-selection gates.
- [x] (2026-04-27 17:21 -04:00) Extended `tools/mgba/mgba_lua_spike.py` with `--mode starter`, map-slot and coordinate-gated movement helpers, Route 101 handling, and a `drive_to_starter_choose` route that reuses the passing truck route.
- [x] (2026-04-27 17:22 -04:00) Added beacon extension fields for map slot and high coordinate nibbles after fixed-route movement proved `mapKind=2` and low nibble coordinates were not enough to distinguish early interiors and Route 101 positions.
- [x] (2026-04-27 17:22 -04:00) Verified one clean headless starter run. `build/mgba_lua_spike_starter/run_result.json` ended at `STARTER_CHOOSE_READY` with `stageId=12`, `mapKind=4`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=1`.
- [x] (2026-04-27 17:23 -04:00) Verified three consecutive clean headless starter runs. `build/mgba_lua_spike_starter_1`, `_2`, and `_3` all ended at `STARTER_CHOOSE_READY` with the accepted female `A` starter-selection tuple.

## Surprises & Discoveries

- Observation: Headless mGBA is available through the repository-local upstream build, not the installed Windows mGBA release.
  Evidence: `tools/mgba/mgba_lua_spike_notes.md` records that the Windows and Nix release binaries do not advertise `--script`, while the pinned upstream `mgba-headless` build does.

- Observation: The current Python route already exposed two input-boundary hazards that are easy to miss with timing scripts.
  Evidence: `.agent/automation_beacon_repro_execplan.md` records that Birch's boy/girl text can auto-transition into a male selection if `A` is held too long, and that the naming screen disables input during auto-lowercase page swap after entering `A`.

- Observation: The previous GUI/AHK starter attempt consistently reached Littleroot moving-in state `2` and then stalled.
  Evidence: `.agent/automation_beacon_repro_execplan.md` records a speed-mode failure with `stageId=10`, `substageId=2`, `mapKind=2`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=0`.

- Observation: The old `mapKind` proof was too coarse for deterministic overworld movement after the clock flow.
  Evidence: Both Littleroot town and the early Littleroot interiors report `mapKind=2`; the successful route required a new `mapSlot` beacon extension to distinguish town, May's house, Brendan's house, and Route 101 without image recognition.

- Observation: Low-nibble coordinate proof was insufficient on Route 101.
  Evidence: Route 101 local Y positions exceed 14, so the first attempt could not distinguish `y=4` from `y=19`; the beacon map row now carries high coordinate nibbles and the Lua bridge reconstructs full local coordinates.

- Observation: Door and script trigger tiles are action-specific, not just coordinate targets.
  Evidence: Exiting May's 1F from the door tile required pressing `DOWN`; stepping laterally across that tile did not warp and left the route inside the house.

- Observation: Route 101 rescue prompts needed the same script-wait input readiness as Littleroot setup scripts.
  Evidence: A stuck run reached Route 101 at `stageId=11`, `substageId=1`, `mapSlot=7`, `playerX=11`, `playerY=19`, with `flags=3`, `inputReady=0`, and `errorCode=4` while the host passively waited. Treating Route 101 wait-message and wait-button script steps as input-ready unblocked the route.

- Observation: The fixed-route Python spike succeeded, but it is now past the plan's soft governance threshold.
  Evidence: `tools/mgba/mgba_lua_spike.py` is 1,279 lines after the starter-selection route. That is acceptable as a completed proof artifact, but it is not a good foundation for a broader generalized debugger without migrating the host FSM.

## Decision Log

- Decision: Attempt starter selection in the Python/Lua spike before migrating.
  Rationale: Reaching starter selection proves the emulator-owned approach across overworld scripts and movement, which is the next real uncertainty. The spike already handles boot, menu, gender, naming, and truck control without GUI focus.
  Date/Author: 2026-04-27 / Codex

- Decision: Treat Python as the temporary host FSM owner and Lua as a narrow emulator IO bridge.
  Rationale: Python is sufficient for fast iteration and artifact generation. Lua should remain small and expose only key input, frame advancement, screenshots, and beacon reads. This keeps migration easier because the future typed host can replace Python without replacing emulator IO.
  Date/Author: 2026-04-27 / Codex

- Decision: Stop for migration sync if route progress requires general navigation, not just a fixed deterministic route.
  Rationale: A general route planner or minimap model benefits from typed state, explicit invariants, and stronger effect boundaries. Building that inside a throwaway Python spike would create the exact governance debt the migration is meant to avoid.
  Date/Author: 2026-04-27 / Codex

- Decision: Do not add new C beacon diagnostics before the first headless `starter` attempt.
  Rationale: The route can be expressed as a fixed script-derived sequence over existing proof fields. The old Littleroot moving-in ambiguity is now diagnosable through `substageId`, `starterSelection` as script-step proof, `inputReady`, and `errorCode`; adding player position fields before seeing a headless failure would increase protocol scope without evidence.
  Date/Author: 2026-04-27 / Codex

- Decision: Add narrow beacon extension fields after the first route attempt exposed insufficient navigation state.
  Rationale: Fixed route waypoints still need reliable map and coordinate identity. A map-slot row and coordinate high nibbles are smaller and more reviewable than screenshot-based minimap detection or generalized path planning.
  Date/Author: 2026-04-27 / Codex

- Decision: Treat this plan as complete after starter-selection proof, and recommend migration before expanding automation scope.
  Rationale: The proof objective passed in Python, but the host file now exceeds the soft size threshold. Further goals such as starter confirmation, arbitrary points of interest, minimap concepts, or reusable route planning should move the FSM/invariant layer into the typed host architecture while keeping Lua as the emulator IO bridge.
  Date/Author: 2026-04-27 / Codex

## Outcomes & Retrospective

Outcome: Success. The Python/Lua headless spike reaches first Pokemon selection from a clean ROM boot without Windows GUI focus or OS input injection.

Final single-run command:

    rm -rf build/mgba_lua_spike_starter
    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode starter --require-headless --output-dir build/mgba_lua_spike_starter --connect-timeout 20 --starter-timeout 600'
    {"ok": true, "result": "build/mgba_lua_spike_starter/run_result.json"}

Final single-run tuple:

    target=STARTER_CHOOSE_READY
    elapsedSeconds=6.285
    stageId=12
    mapKind=4
    gender=2
    nameLen=1
    nameChar0=1
    inputReady=1
    frame=12213

Repeated clean-run evidence:

    build/mgba_lua_spike_starter_1: target=STARTER_CHOOSE_READY elapsedSeconds=6.007 frame=12257 stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1
    build/mgba_lua_spike_starter_2: target=STARTER_CHOOSE_READY elapsedSeconds=5.982 frame=12195 stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1
    build/mgba_lua_spike_starter_3: target=STARTER_CHOOSE_READY elapsedSeconds=6.164 frame=12250 stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1

Recommendation: Do not expand the Python host into a generalized automation/debugging framework. Keep the Lua bridge as the emulator-owned IO layer, keep the Python route as a proof and regression harness, and migrate the host FSM/invariant layer before adding broader route planning, minimap/interactable modeling, or more points of interest.

## Context and Orientation

This repository builds a Game Boy Advance ROM. The project root is `/home/bayesartre/dev/pokeemerald-expansion-shared-power`. The generated ROM is `pokeemerald.gba`. Automation builds use `AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1`, which enables a small debug-only visual beacon inside the game. The beacon is a machine-readable sprite whose decoded fields describe the current route state.

The existing broad beacon plan is `.agent/automation_beacon_repro_execplan.md`. It explains the v1 beacon protocol, existing AutoHotkey runner, and prior GUI evidence. This plan builds on that checked-in file and on the newer Python/Lua spike recorded in `tools/mgba/mgba_lua_spike_notes.md`.

The current headless spike has two main files. `tools/mgba/mgba_lua_bridge.lua` runs inside mGBA and exposes a localhost line protocol. It supports `read_beacon`, `tap`, `set_keys`, `clear_keys`, `run_frames`, and `screenshot`. It reads the beacon directly from OBJ VRAM, so it does not need a GUI window capture. `tools/mgba/mgba_lua_spike.py` is the Python host. It finds a script-capable mGBA binary, launches `mgba-headless` with the Lua bridge, connects over localhost, sends inputs, waits for emulator frames, and writes JSON artifacts.

Important stage ids from the v1 beacon are:

- `1` is `MAIN_MENU_READY`.
- `2` is Birch intro text.
- `3` is gender prompt ready.
- `5` is naming screen ready.
- `6` is name confirmed.
- `9` is truck control ready.
- `10` is Littleroot setup in progress.
- `11` is Route 101 approach.
- `12` is starter choose ready.
- `13` is starter confirmation prompt.

The target final proof for this plan is `stageId=12`, `mapKind=4`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=1`. This means the route reached the first Pokemon selection screen, the player is female, the name is exactly `A`, the map context is starter selection, and the game is ready for input.

The relevant game code is spread across these files:

- `src/main_menu.c` drives clean boot into Birch intro, gender selection, naming, and the transition into the truck.
- `src/naming_screen.c` drives player name entry and emits the naming-screen beacon.
- `src/overworld.c` emits truck, Littleroot, and Route 101 route beacons from `AutomationBeacon_UpdateOverworld`.
- `src/scrcmd.c` tracks script command classes through `AutomationBeacon_SetScriptStep`.
- `src/wallclock.c` emits beacon states while the wall-clock screen owns control.
- `src/starter_choose.c` emits `STARTER_CHOOSE_READY` and starter-confirmation beacon states.
- `data/maps/InsideOfTruck/scripts.inc`, the Littleroot map scripts, and the Route 101 scripts define the actual early-game movement and event sequence.

## Migration Gate

Continue in Python only while the route remains a single deterministic path and each next action can be chosen from a small beacon tuple. Python is acceptable if the work is limited to adding route helpers, adding one `--mode starter`, refining local beacon diagnostics, and recording artifacts.

Stop and sync with the operator about migration if any of these conditions become true:

- The same beacon tuple repeats after two targeted instrumentation attempts and still does not explain whether to wait, press `A`, or move.
- Reaching starter selection requires a general minimap, path planner, or object-interaction discovery system instead of a fixed route.
- The host FSM needs cross-screen invariant enforcement that is hard to review in Python, such as typed transitions between menu, text, movement, script, and cutscene domains.
- The Python route implementation becomes large enough that route correctness depends on implicit mutable state rather than explicit beacon proofs. As a practical threshold, if `tools/mgba/mgba_lua_spike.py` would exceed roughly 900 lines or require multiple route-state classes, stop and recommend migration.
- The Lua bridge needs capabilities beyond emulator-owned input, frame stepping, screenshots, and memory/beacon reads.
- Three clean headless attempts fail at different route points without a stable beacon diagnosis. That indicates nondeterminism or insufficient state modeling, not just a missing tap.

When stopping for migration sync, do not leave only a timeout. Update this ExecPlan with a blocker report containing the command run, output directory, last beacon tuple, event log path, any screenshot path, exact attempted fixes, and a clear recommendation: continue in Python with a named small change, or migrate the host FSM before continuing.

## Plan of Work

Milestone 1 revalidates the known baseline. Build a fresh automation ROM, run Python syntax validation, run Lua syntax validation, and run the current `truck` mode once. This proves the starting point is not stale. Record the final `TRUCK_CONTROL_READY` beacon tuple in `Progress` and `Artifacts and Notes` before editing route logic.

Milestone 2 maps the remaining route through code. Read the map scripts and C callbacks that control truck exit, Littleroot moving-in, wall-clock setup, Route 101, Birch's help event, and starter selection. Do not change route code during this milestone except to add documentation to this ExecPlan. The output of this milestone is a short route table in `Artifacts and Notes` that names each expected beacon stage, the next action, and the code location that justifies it. If the current code does not expose enough state to make this table, record that as the first candidate for beacon instrumentation.

Milestone 3 adds only the minimum beacon diagnostics needed to unblock the known Littleroot moving-in ambiguity. Prefer existing v1 fields first: `stageId`, `substageId`, `flags`, `starterSelection`, `inputReady`, and `errorCode`. If that is insufficient, add extension rows to the beacon tile while preserving the first two v1 rows. Update `tools/mgba/mgba_lua_bridge.lua` to decode any new fields, and update `tools/mgba/mgba_read_beacon.ps1` only if the Windows GUI runner still needs the same diagnostics. Candidate fields are player local X/Y, player facing, movement-ready, text-ready, script-wait kind, and interactable-ahead. Add no more fields than the route actually uses.

Milestone 4 extends the Python host route. Add a `starter` choice to `--mode`, a `--starter-timeout` argument, constants for stages `10`, `11`, `12`, and map kinds `2`, `3`, `4`, and a `drive_to_starter_choose` function. This function should call `drive_to_truck` first and then continue from truck control. Keep Lua as a bridge only. If movement needs holding a direction across frames, use the existing `set_keys` and `clear_keys` commands through a Python helper; do not add OS input injection.

Milestone 5 proves truck exit and Littleroot moving-in. Drive out of the truck using beacon-gated movement, then handle Littleroot setup only when the beacon says the relevant script/text/movement state is ready. The acceptance for this milestone is not starter selection; it is a stable later Littleroot or wall-clock beacon that proves the old `stageId=10`, `substageId=2` blocker has been passed. If this milestone cannot pass without a generalized navigation model, write the migration blocker report and stop.

Milestone 6 proves wall-clock and bedroom flow. Use the existing `src/wallclock.c` beacons for substages `6` and `7` to avoid timing sleeps. If movement through the player's house requires precise position or facing, add the smallest navigation proof fields in Milestone 3 before attempting this milestone. The acceptance is returning to overworld control after the clock flow with female `A` proof still intact.

Milestone 7 proves Route 101 and starter-choice entry. Continue from Littleroot to Route 101 using beacon-gated movement and script/text readiness. Stop when `src/starter_choose.c` emits `STARTER_CHOOSE_READY`. The first acceptance target is `stageId=12`, `mapKind=4`, `inputReady=1`, with starter selection at any valid value. Do not attempt starter confirmation until this basic screen-entry proof is reliable.

Milestone 8 validates repeated clean runs. Run three separate headless `starter` attempts from clean boot using separate output directories. Each must end at `STARTER_CHOOSE_READY` with female `A` proof. Record each result path and tuple in this ExecPlan. If one run fails, inspect the event log, add a precise `Surprises & Discoveries` entry, and either fix a small route issue or trigger the migration gate.

## Concrete Steps

Start from the repository root:

    cd /home/bayesartre/dev/pokeemerald-expansion-shared-power

Use WSL-side search and file reads for repository investigation:

    rg -n "VAR_LITTLEROOT_INTRO_STATE|ChooseStarter|CB2_ChooseStarter|WallClock|MAP_ROUTE101|InsideOfTruck" src data include

Baseline validation for Milestone 1:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"
    nix-shell -p python3 --run 'python3 -m py_compile tools/mgba/mgba_lua_spike.py'
    nix-shell -p lua --run 'luac -p tools/mgba/mgba_lua_bridge.lua'
    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode truck --require-headless --output-dir build/mgba_lua_spike_truck_baseline --connect-timeout 20 --truck-timeout 360'

Expected truck result:

    {"ok": true, "result": "build/mgba_lua_spike_truck_baseline/run_result.json"}

The `run_result.json` should contain a beacon with `stageId=9`, `gender=2`, `nameLen=1`, `nameChar0=1`, `mapKind=1`, and `inputReady=1`.

After implementing `starter` mode, the single-run command should be:

    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode starter --require-headless --output-dir build/mgba_lua_spike_starter --connect-timeout 20 --starter-timeout 600'

Expected starter result:

    {"ok": true, "result": "build/mgba_lua_spike_starter/run_result.json"}

The `run_result.json` should contain `target=STARTER_CHOOSE_READY` and a beacon with `stageId=12`, `mapKind=4`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=1`.

For repeated validation, use separate output directories:

    for i in 1 2 3; do
      out="build/mgba_lua_spike_starter_$i"
      nix-shell -p python3 --run "python3 tools/mgba/mgba_lua_spike.py --mode starter --require-headless --output-dir $out --connect-timeout 20 --starter-timeout 600" || exit 1
      cat "$out/run_result.json"
    done

If a run fails, do not blindly increase timeouts. Read the event log and final result:

    cat build/mgba_lua_spike_starter/run_result.json
    tail -n 200 build/mgba_lua_spike_starter/events.ndjson

## Validation and Acceptance

The plan is complete only when three clean headless runs from clean boot reach starter selection. Each run must start from `pokeemerald.gba`, use `mgba-headless`, use the Lua bridge for emulator input, and avoid Windows GUI focus or OS input injection.

The final accepted beacon tuple for each successful run must include:

    stageId=12
    mapKind=4
    gender=2
    nameLen=1
    nameChar0=1
    inputReady=1

Also run static validation before declaring success:

    nix-shell -p python3 --run 'python3 -m py_compile tools/mgba/mgba_lua_spike.py'
    nix-shell -p lua --run 'luac -p tools/mgba/mgba_lua_bridge.lua'
    git diff --check

If beacon C code changes, rebuild the automation ROM:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"

If default-build behavior could be affected by beacon call sites or macros, also run:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make NO_MULTIBOOT=1 -j"$(nproc)"

## Idempotence and Recovery

All source changes should be additive and safe to rerun. Headless route runs write artifacts under `build/` and can be repeated with new output directories. Do not delete user files or reset the repository. If a stale output directory is confusing, create a new one instead of relying on cleanup.

If the ROM does not expose a beacon, rebuild with `AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1`. If `mgba-headless` is missing, use `tools/mgba/build_mgba_headless.sh` as documented in `tools/mgba/mgba_lua_spike_notes.md`. If network access or dependency fetching is needed for that build in a restricted environment, request operator approval instead of trying to bypass restrictions.

If a route run times out, preserve `run_result.json`, `events.ndjson`, `mgba_stdout.log`, and `mgba_stderr.log`. If a visual screenshot would help, use the Lua bridge `screenshot` command or add a Python helper that calls it into the current output directory. Record the blocker before changing route logic.

## Artifacts and Notes

Important output paths:

    build/mgba_lua_spike_truck_baseline/run_result.json
    build/mgba_lua_spike_starter/run_result.json
    build/mgba_lua_spike_starter/events.ndjson
    build/mgba_lua_spike_starter_1/run_result.json
    build/mgba_lua_spike_starter_2/run_result.json
    build/mgba_lua_spike_starter_3/run_result.json

Milestone 1 evidence captured on 2026-04-27:

    nix-shell -p python3 --run 'python3 -m py_compile tools/mgba/mgba_lua_spike.py'
    # passed

    nix-shell -p lua --run 'luac -p tools/mgba/mgba_lua_bridge.lua'
    # passed

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"
    Memory region         Used Size  Region Size  %age Used
               EWRAM:      227948 B       256 KB     86.96%
               IWRAM:       28821 B        32 KB     87.95%
                 ROM:    24734532 B        32 MB     73.71%

    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode truck --require-headless --output-dir build/mgba_lua_spike_truck_baseline --connect-timeout 20 --truck-timeout 360'
    {"ok": true, "result": "build/mgba_lua_spike_truck_baseline/run_result.json"}
    final tuple: stageId=9 gender=2 nameLen=1 nameChar0=1 mapKind=1 inputReady=1

Final validation evidence captured on 2026-04-27:

    nix-shell -p python3 --run 'python3 -m py_compile tools/mgba/mgba_lua_spike.py'
    # passed

    nix-shell -p lua --run 'luac -p tools/mgba/mgba_lua_bridge.lua'
    # passed

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"
    Memory region         Used Size  Region Size  %age Used
               EWRAM:      227948 B       256 KB     86.96%
               IWRAM:       28829 B        32 KB     87.98%
                 ROM:    24735356 B        32 MB     73.72%

    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode starter --require-headless --output-dir build/mgba_lua_spike_starter --connect-timeout 20 --starter-timeout 600'
    {"ok": true, "result": "build/mgba_lua_spike_starter/run_result.json"}
    final tuple: target=STARTER_CHOOSE_READY stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1 frame=12213 elapsedSeconds=6.285

    Repeated clean runs:
    build/mgba_lua_spike_starter_1: target=STARTER_CHOOSE_READY stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1 frame=12257 elapsedSeconds=6.007
    build/mgba_lua_spike_starter_2: target=STARTER_CHOOSE_READY stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1 frame=12195 elapsedSeconds=5.982
    build/mgba_lua_spike_starter_3: target=STARTER_CHOOSE_READY stageId=12 mapKind=4 gender=2 nameLen=1 nameChar0=1 inputReady=1 frame=12250 elapsedSeconds=6.164

    git diff --check
    # passed

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make NO_MULTIBOOT=1 -j"$(nproc)"
    Memory region         Used Size  Region Size  %age Used
               EWRAM:      227940 B       256 KB     86.95%
               IWRAM:       28705 B        32 KB     87.60%
                 ROM:    24667124 B        32 MB     73.51%

    The default build overwrote `pokeemerald.gba`, so the automation build was run again afterward to leave the workspace ROM compatible with the headless beacon route:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"
    Memory region         Used Size  Region Size  %age Used
               EWRAM:      227948 B       256 KB     86.96%
               IWRAM:       28829 B        32 KB     87.98%
                 ROM:    24735356 B        32 MB     73.72%

Important implementation files:

    tools/mgba/mgba_lua_spike.py
    tools/mgba/mgba_lua_bridge.lua
    tools/mgba/mgba_lua_spike_notes.md
    include/automation_beacon.h
    src/automation_beacon.c
    src/overworld.c
    src/scrcmd.c
    src/wallclock.c
    src/starter_choose.c
    PATCH_NOTES.md

Milestone 2 route table captured on 2026-04-27:

    truck control -> press RIGHT until Littleroot setup begins
      beacon: stageId=9, mapKind=1, inputReady=1, then stageId=10
      source: src/overworld.c AutomationBeacon_UpdateOverworld; data/maps/InsideOfTruck/scripts.inc; data/maps/LittlerootTown/scripts.inc

    Littleroot truck exit and moving-in cutscene -> press A only when ready until Mom tells player to set the clock
      beacon: stageId=10, substageId=2 to substageId=4, mapKind=2
      source: data/maps/LittlerootTown/scripts.inc LittlerootTown_EventScript_StepOffTruckFemale; data/scripts/players_house.inc PlayersHouse_1F_EventScript_EnterHouseMovingIn

    player house 1F -> move UP into stair trigger and wait for 2F setup
      beacon: stageId=10, substageId=4 to substageId=5, mapKind=2
      source: data/maps/LittlerootTown_MaysHouse_1F/scripts.inc LittlerootTown_MaysHouse_1F_EventScript_GoUpstairsToSetClock; data/maps/LittlerootTown_MaysHouse_2F/scripts.inc PlayersHouse_2F_EventScript_BlockStairsUntilClockIsSet

    player room -> face/interact with clock, accept stopped-clock text, set clock, confirm YES
      beacon: stageId=10, substageId=5 to wall-clock substageId=6/7, mapKind=2, inputReady=1
      source: data/scripts/players_house.inc PlayersHouse_2F_EventScript_WallClock; src/wallclock.c Task_SetClock_HandleInput and Task_SetClock_HandleConfirmInput

    after clock -> press through Mom's room text and TV report until told to meet the neighbor
      beacon: stageId=10, substageId=6 to substageId=7, mapKind=2
      source: data/scripts/players_house.inc PlayersHouse_2F_EventScript_MomComesUpstairsFemale and PlayersHouse_1F_EventScript_PetalburgGymReportFemale

    Littleroot town -> enter Brendan's house, press through rival mom, enter 2F, inspect rival Poke Ball, return outside
      beacon: stageId=10, substageId=7, mapKind=2; rival state changes are not separately beaconed
      source: data/maps/LittlerootTown_BrendansHouse_1F/scripts.inc LittlerootTown_BrendansHouse_1F_EventScript_YoureNewNeighbor; data/maps/LittlerootTown_BrendansHouse_2F/scripts.inc LittlerootTown_BrendansHouse_2F_EventScript_MeetBrendan

    Route 101 approach -> walk to Birch rescue trigger, press through help text, walk to bag, press A
      beacon: stageId=11, mapKind=3, then stageId=12
      source: data/maps/Route101/scripts.inc Route101_EventScript_StartBirchRescue and Route101_EventScript_BirchsBag

    starter selection -> stop at first input-ready starter chooser
      beacon: stageId=12, mapKind=4, inputReady=1
      source: src/starter_choose.c CB2_ChooseStarter and Task_HandleStarterChooseInput

When a blocker requires migration sync, add a report here:

    Blocker report:
      command:
      output directory:
      last beacon:
      event log:
      screenshot:
      attempted fixes:
      why this crosses the migration gate:
      recommendation:

## Interfaces and Dependencies

`tools/mgba/mgba_lua_spike.py` should expose a new mode:

    --mode starter
    --starter-timeout <seconds>

The successful JSON result should follow the existing `run_result.json` pattern:

    {
      "ok": true,
      "target": "STARTER_CHOOSE_READY",
      "beacon": {
        "stageId": 12,
        "mapKind": 4,
        "gender": 2,
        "nameLen": 1,
        "nameChar0": 1,
        "inputReady": 1
      },
      "elapsedSeconds": 0.0
    }

Timeout results should remain structured:

    {
      "ok": false,
      "target": "starter",
      "error": "timeout",
      "label": "short human-readable failing wait",
      "lastBeacon": { }
    }

If adding beacon fields, preserve the current first two rows so existing readers keep working. Add new fields as extension fields decoded by name in `tools/mgba/mgba_lua_bridge.lua`; do not overload an existing field unless the plan records why that overload is safe. For C code, follow repository style: functions use PascalCase, locals use camelCase, and file-static variables use the `s` prefix.

Any code, data, or documentation change made while executing this plan must add a top entry to `PATCH_NOTES.md` using this repository's required format:

    - Area: short description (commit pending).

## Revision Notes

Revision Note (2026-04-27): Initial ExecPlan. It narrows the next pre-migration goal to a headless clean-boot route that reaches `STARTER_CHOOSE_READY`, defines the evidence required for success, and defines when to stop and sync about migrating the host FSM before adding broader navigation or typed invariant machinery.

Revision Note (2026-04-27): Completed the plan. The headless Python/Lua spike now reaches `STARTER_CHOOSE_READY` from clean boot in three consecutive runs. The outcome recommends migration before broader automation scope because the Python host has served its proof purpose and crossed the intended complexity threshold.
