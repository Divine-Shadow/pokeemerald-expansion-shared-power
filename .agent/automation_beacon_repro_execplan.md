# Build a Beacon-Gated mGBA Reproduction Pipeline

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan must be maintained according to `.agent/PLANS.md` in this repository. Any contributor implementing this plan must complete milestones in order, record evidence in this file before moving to the next milestone, and continue milestone-to-milestone until the full behavior is implemented and verified or a true blocker requires escalation.

## Purpose / Big Picture

Manual reproduction through the early game is too expensive for bugs whose trigger point is first Pokemon selection. The current mGBA automation attempts to drive the user interface with timings and screenshots, but it is fragile under speed-up, desktop focus changes, and small timing drift. After this plan is implemented, an agent can start from a clean ROM boot, read a tiny machine-readable "automation beacon" from the game screen, send inputs only when the game reports it is ready, and produce proof that it reached the desired route state.

The first full proof target is the truck scene with a female player named `A`, because that is a compact setup goal that previously exposed automation drift. The next target is the first Pokemon selection screen, where the current bug reproduction experiment begins. A human can see the system working by running the mGBA automation script and inspecting the generated log: it should show ordered beacon states ending first at `TRUCK_CONTROL_READY` with `gender=FEMALE` and `name=A`, then at `STARTER_CHOOSE_READY`.

## Progress

- [x] (2026-04-26 21:56 -04:00) Read `.agent/PLANS.md` and incorporated its requirements for self-contained, milestone-gated execution.
- [x] (2026-04-26 21:56 -04:00) Identified the main repository touchpoints: `src/main_menu.c` for main menu, Birch speech, gender, and name flow; `data/maps/InsideOfTruck/scripts.inc` for male/female truck setup; `src/starter_choose.c` for first Pokemon selection; `tools/mgba/` for Windows mGBA automation helpers.
- [x] (2026-04-26 21:56 -04:00) Defined the initial beacon protocol and the milestone chain in this ExecPlan.
- [ ] Milestone 1: Add a debug-only static beacon and prove external pixel sampling can read it.
- [ ] Milestone 2: Make the beacon dynamic across early title/main-menu states.
- [ ] Milestone 3: Build a reusable external beacon sampler that locates the game viewport by scanning for the beacon anchor.
- [ ] Milestone 4: Prove one state-gated input transition from main menu into new game.
- [ ] Milestone 5: Drive Birch intro to the gender prompt using beacon states instead of timing loops.
- [ ] Milestone 6: Select female and verify it from in-game state.
- [ ] Milestone 7: Enter exactly the player name `A` and verify the saved name from in-game state.
- [ ] Milestone 8: Reach the truck and verify female player/name/map state with screenshot evidence.
- [ ] Milestone 9: Continue from truck to first Pokemon selection and verify starter selection readiness.
- [ ] Milestone 10: Run the integrated path at normal speed and speed-up, then record reliability evidence.

## Surprises & Discoveries

- Observation: Windows-side `rg.exe` from the Codex bundle may fail with "Access is denied" in this UNC workspace, but WSL-side `rg` is available in the NixOS instance.
  Evidence: PowerShell search fell back to `Select-String`; the operator confirmed WSL has `rg`.

- Observation: `src/main_menu.c` owns several early-game states that matter to this pipeline, including `Task_DisplayMainMenu`, `Task_NewGameBirchSpeech_ChooseGender`, `Task_NewGameBirchSpeech_StartNamingScreen`, and `CB2_NewGameBirchSpeech_ReturnFromNamingScreen`.
  Evidence: repository search found the player gender assignment in `Task_NewGameBirchSpeech_ChooseGender` and the return-from-naming setup in `CB2_NewGameBirchSpeech_ReturnFromNamingScreen`.

- Observation: `data/maps/InsideOfTruck/scripts.inc` already has explicit male and female intro setup paths.
  Evidence: `InsideOfTruck_EventScript_SetIntroFlagsFemale` sets `VAR_LITTLEROOT_INTRO_STATE` to `2`, sets May-house flags, and sets the dynamic warp to the female house coordinates.

- Observation: `src/starter_choose.c` has a compact state machine for the first Pokemon selection screen.
  Evidence: `CB2_ChooseStarter` creates `Task_StarterChoose`, `Task_HandleStarterChooseInput` updates `tStarterSelection`, and `Task_AskConfirmStarter` opens the confirmation prompt.

## Decision Log

- Decision: Use a tiny on-screen automation beacon instead of classifier-style screenshot matching for primary pass/fail decisions.
  Rationale: The game can expose exact state through pixels, which keeps automation tied to game state instead of timing, frame rate, host load, or fragile visual recognition.
  Date/Author: 2026-04-26 / Codex

- Decision: Treat saves as optional accelerators, not canonical fixtures, for this pipeline.
  Rationale: Save files may be contaminated across builds. The durable path is fixed RNG plus deterministic progression from clean boot, with later save exports allowed only as convenience artifacts.
  Date/Author: 2026-04-26 / Codex

- Decision: Make the truck scene the first full end-to-end proof target, then extend to first Pokemon selection.
  Rationale: The truck target proves the historically flaky setup details, especially female avatar selection and exact name entry, before the route expands into overworld movement and starter selection.
  Date/Author: 2026-04-26 / Codex

- Decision: Gate every automation input on beacon states, and make screenshots supporting evidence rather than the primary oracle.
  Rationale: Screenshots are still valuable to humans, but exact beacon tuples give scripts a deterministic reason to proceed or stop.
  Date/Author: 2026-04-26 / Codex

## Outcomes & Retrospective

This section is intentionally empty at plan creation. After each major milestone, add a short entry comparing the observed behavior against the purpose of this plan. At completion, summarize whether the system can reliably reach female `A` in the truck and first Pokemon selection at both normal speed and speed-up.

## Context and Orientation

This repository builds a Game Boy Advance ROM. The main code is C under `src/`, headers live under `include/`, script data lives under `data/`, and the generated ROM is `pokeemerald.gba`. Build commands are run from the repository root.

An "automation beacon" in this plan means a small debug-only visual marker drawn by the game itself into the GBA video output. External automation captures the mGBA window, scans for the beacon, decodes a few pixel colors into stage values, and sends the next input only when the expected stage is visible. This is not a user-facing feature. It is a test instrument.

The early game flow relevant to this plan is:

Start mGBA and boot the ROM. Press through title and main menu into new game. `src/main_menu.c` runs the new-game Birch speech. During that flow, the game asks for gender and then opens the naming screen. After the name is confirmed, the game transitions into the real game and enters the moving truck map. `data/maps/InsideOfTruck/scripts.inc` sets different flags depending on player gender. Later, overworld movement and scripts lead to Route 101, where `ChooseStarter` in `src/battle_setup.c` calls `CB2_ChooseStarter` in `src/starter_choose.c`.

The existing files in `tools/mgba/` are useful starting points but should be treated as prototypes. `tools/mgba/new_girl_a_truck.ahk` is an AutoHotkey v1 script that currently attempts a timing/checkpoint path to the truck. `tools/mgba/mgba_capture_window.ps1` captures the mGBA client area. `tools/mgba/mgba_compare_capture.ps1` computes image difference scores. This plan replaces timing and image-diff gates with explicit beacon reads.

## Beacon Protocol v1

The beacon should be enabled only for automation builds. Add a Makefile option named `AUTOMATION_BEACON`, defaulting to `0`, and pass it to C as `-DAUTOMATION_BEACON=$(AUTOMATION_BEACON)`. A normal `make` build should not show or update the beacon. An automation build should use:

    make AUTOMATION_BEACON=1 DEBUG=1

The beacon should be a single 8x8 4bpp object sprite pinned to the top-left of the GBA screen. A "4bpp object sprite" is a normal GBA sprite whose pixels are four-bit palette indices. Index `0` is transparent, so encoded values must use visible indices `1` through `15`. The decoded value is `paletteIndex - 1`, giving values `0` through `14`. If a future field needs a value above `14`, encode it as two base-15 digits in two pixels.

The first row of the 8x8 sprite is the stable machine-readable header:

Pixel 0 is anchor value `14`.
Pixel 1 is anchor value `13`.
Pixel 2 is protocol version, initially `1`.
Pixel 3 is `stageId`.
Pixel 4 is `substageId`.
Pixel 5 is `flags`.
Pixel 6 is `pulse`.
Pixel 7 is `checksum`.

The checksum is `(version + stageId + substageId + flags + pulse) % 15`. External scripts must reject a beacon whose first two decoded pixels are not `14, 13` or whose checksum does not match.

The second row carries route-specific proof fields:

Pixel 0 is `gender`, with `0` for unknown, `1` for male, and `2` for female.
Pixel 1 is `nameLen`, with `0` through `7` allowed for player names.
Pixel 2 is `nameChar0`, with `0` for unknown, `1` for `A`, `2` for `B`, through `14` for `N`. This v1 pipeline only needs to prove `A`.
Pixel 3 is `mapKind`, with `0` unknown, `1` truck, `2` Littleroot, `3` Route 101, and `4` starter selection.
Pixel 4 is `starterSelection`, with `0`, `1`, or `2` matching `src/starter_choose.c`'s `tStarterSelection`, and `14` for not applicable.
Pixel 5 is `inputReady`, with `0` false and `1` true.
Pixel 6 is `errorCode`, with `0` for none.
Pixel 7 is reserved and should be `0`.

Use these stage ids:

`0` means inactive or unknown. `1` means main menu ready. `2` means Birch intro text is active. `3` means gender prompt ready. `4` means gender confirmed. `5` means naming screen ready. `6` means name confirmed. `7` means post-name Birch flow. `8` means truck entered. `9` means truck control ready. `10` means Littleroot route setup in progress. `11` means Route 101 approach. `12` means starter choose ready. `13` means starter confirmation prompt. `14` means automation error.

## Plan of Work

Milestone 1 adds the smallest possible debug-only beacon. Create `include/automation_beacon.h` and `src/automation_beacon.c`. Add the source to the existing build by relying on the Makefile's `src/*.c` wildcard. Add `AUTOMATION_BEACON ?= 0` to `Makefile` near the other build flags and append `-DAUTOMATION_BEACON=$(AUTOMATION_BEACON)` to `CPPFLAGS`. The implementation should compile to no-ops when `AUTOMATION_BEACON` is `0`.

The beacon implementation should expose:

    void AutomationBeacon_SetStage(u8 stageId, u8 substageId, u8 flags);
    void AutomationBeacon_SetProof(u8 gender, u8 nameLen, u8 nameChar0, u8 mapKind, u8 starterSelection, bool8 inputReady);
    void AutomationBeacon_Render(void);

Use names and formatting consistent with `docs/STYLEGUIDE.md`: functions in PascalCase, locals in camelCase, globals prefixed with `g`, and static file-scope data prefixed with `s`.

Milestone 2 wires the beacon into early-game callbacks. Add calls in `src/main_menu.c` so `Task_DisplayMainMenu` reports `MAIN_MENU_READY`, the Birch speech task reports text and gender states, the name flow reports naming states, and the return-from-naming code reports `NAME_CONFIRMED` with player gender and name proof. Add calls in `src/starter_choose.c` so first Pokemon selection reports `STARTER_CHOOSE_READY`, selected starter id, and confirmation-prompt state. Add calls for the truck proof either in a safe overworld callback or in the truck script path after inspecting the current overworld callback structure; do not guess at a map hook without reading the relevant field code first.

Milestone 3 replaces image-diff checkpointing with a beacon sampler. Add `tools/mgba/mgba_read_beacon.ps1`. It should capture the mGBA client area, scan pixels for the two-anchor sequence, map the following colors back to protocol values, validate the checksum, and print a single-line JSON object. A successful read should look like:

    {"stageId":9,"substageId":0,"gender":2,"nameLen":1,"nameChar0":1,"mapKind":1,"inputReady":1}

Milestone 4 adds a new AutoHotkey v1 runner, `tools/mgba/run_beacon_repro.ahk`. This runner should launch mGBA, call `mgba_read_beacon.ps1` until the expected state appears, send the next input, and write logs under `build/mgba_beacon/`. It should not use long fixed sleeps except for short debounce intervals after sending a key. Each wait should have a timeout and should capture a failure screenshot before exiting.

Milestones 5 through 8 implement the truck proof. The runner should wait for `MAIN_MENU_READY`, enter new game, drive Birch text only when `inputReady=1`, select female at `GENDER_PROMPT_READY`, verify `gender=2`, open the naming screen, enter exactly `A`, verify `nameLen=1` and `nameChar0=1`, continue through post-name text, and stop at `TRUCK_CONTROL_READY` with `mapKind=1`. The final artifact for this section is `build/mgba_beacon/truck_female_a.png` plus a log showing every accepted beacon transition.

Milestone 9 extends the same system from the truck to first Pokemon selection. Do not encode route progress as timing. Add route beacons around the truck exit, Littleroot setup, and Route 101 transition after reading the relevant field and script code. The final artifact is `build/mgba_beacon/starter_choose_ready.png` plus a log line where `stageId=12`, `mapKind=4`, and `inputReady=1`.

Milestone 10 validates the integrated run at normal speed and speed-up. Speed-up means mGBA is running faster than real time. The runner should still work because it waits for beacon states rather than elapsed seconds. Record at least three normal-speed successful runs and three speed-up successful runs in this ExecPlan.

## Concrete Steps

Start from the repository root:

    cd /home/bayesartre/dev/pokeemerald-expansion-shared-power

Use WSL-side search when possible:

    rg -n "Task_NewGameBirchSpeech_ChooseGender|CB2_ChooseStarter|InsideOfTruck" src data include

Implement Milestone 1 with these file changes:

Add `AUTOMATION_BEACON ?= 0` near the other top-level Makefile feature flags. Add `-DAUTOMATION_BEACON=$(AUTOMATION_BEACON)` to `CPPFLAGS`.

Create `include/automation_beacon.h` with the public functions and stage constants. Create `src/automation_beacon.c` with no-op definitions when `AUTOMATION_BEACON` is `0`, and with sprite-backed rendering when `AUTOMATION_BEACON` is `1`.

Build both normal and automation variants:

    make
    make AUTOMATION_BEACON=1 DEBUG=1

The normal build should complete without showing a beacon when launched. The automation build should show the static anchor in mGBA and allow the sampler to find it.

Implement Milestone 3 with this command-line contract:

    powershell -NoProfile -ExecutionPolicy Bypass -File tools/mgba/mgba_read_beacon.ps1 -TargetPid <mgba-pid>

Expected success output:

    {"found":true,"stageId":1,"protocol":1,"checksumOk":true}

Expected failure output:

    {"found":false,"error":"beacon anchor not found"}

When running AHK locally on Windows, copy the script to `C:\Temp` before launch if UNC execution is unreliable, but keep the canonical script in `tools/mgba/`.

## Validation and Acceptance

Milestone 1 is accepted when `make AUTOMATION_BEACON=1 DEBUG=1` builds successfully and `mgba_read_beacon.ps1` can find the static anchor in an mGBA screenshot. Record the exact build command, whether it passed, and the beacon tuple in `Progress` or `Artifacts and Notes`.

Milestone 2 is accepted when the beacon changes from `MAIN_MENU_READY` to a Birch intro state during a manual new-game flow. A screenshot alone is not enough; include the decoded JSON tuple.

Milestone 3 is accepted when the sampler finds the beacon after the mGBA window is moved or resized. This proves the sampler is finding the game viewport by scanning for the anchor rather than trusting fixed window coordinates.

Milestone 4 is accepted when the runner waits for `MAIN_MENU_READY`, sends one input, and observes the next expected state. Include a short log excerpt.

Milestone 8 is accepted when the runner produces `build/mgba_beacon/truck_female_a.png` and the log contains proof equivalent to:

    stage=TRUCK_CONTROL_READY gender=FEMALE nameLen=1 nameChar0=A mapKind=TRUCK inputReady=1

Milestone 9 is accepted when the runner reaches first Pokemon selection and the log contains proof equivalent to:

    stage=STARTER_CHOOSE_READY mapKind=STARTER_SELECTION inputReady=1 starterSelection=1

Milestone 10 is accepted when the normal-speed and speed-up runs both complete repeatedly without changing key timings. If any run fails, record the failed beacon tuple, screenshot path, and recovery decision in this ExecPlan before retrying.

## Idempotence and Recovery

All source changes should be additive and safe to rebuild. Running `make AUTOMATION_BEACON=1 DEBUG=1` repeatedly should update build artifacts but should not alter source files. Running the mGBA automation scripts repeatedly should write fresh logs and screenshots under `build/mgba_beacon/`; scripts should create that directory if missing.

If a milestone fails because the beacon is not visible, first verify that the ROM was built with `AUTOMATION_BEACON=1`. If the beacon appears visually but the sampler cannot find it, save the screenshot and update `Surprises & Discoveries` with the observed colors and location before changing the sampler.

If the runner sends an input at the wrong time, do not add a long sleep as the primary fix. Instead, add or refine a beacon state so the runner can tell when the game is ready.

If a future build changes early-game scripts enough to invalidate route assumptions, keep the beacon protocol stable and update only the stage-producing hooks and route runner. Record the change in `Decision Log`.

## Artifacts and Notes

Important output paths for implementers:

    build/mgba_beacon/run.log
    build/mgba_beacon/failure.png
    build/mgba_beacon/truck_female_a.png
    build/mgba_beacon/starter_choose_ready.png

Important implementation files:

    Makefile
    include/automation_beacon.h
    src/automation_beacon.c
    src/main_menu.c
    src/starter_choose.c
    data/maps/InsideOfTruck/scripts.inc
    tools/mgba/mgba_read_beacon.ps1
    tools/mgba/run_beacon_repro.ahk

When this plan is executed, add concise evidence snippets here. Keep them short and focused on proof, for example:

    make AUTOMATION_BEACON=1 DEBUG=1
    ...
    gba-fixed: success

    mgba_read_beacon.ps1 output:
    {"found":true,"protocol":1,"stageId":9,"gender":2,"nameLen":1,"nameChar0":1,"mapKind":1,"inputReady":1}

## Interfaces and Dependencies

`AUTOMATION_BEACON` is the build-time switch. It must default to `0` so normal builds are unaffected. When `AUTOMATION_BEACON=0`, every public beacon function must compile as an empty inline or empty function so callers can remain simple.

`include/automation_beacon.h` should define the protocol constants, stage ids, and function declarations. Keep constants small enough for the v1 base-15 pixel encoding.

`src/automation_beacon.c` owns the sprite tile, palette, pulse counter, checksum, and render logic. It should not own game-specific decisions like "female was selected"; callers in `src/main_menu.c` and `src/starter_choose.c` provide those values.

`tools/mgba/mgba_read_beacon.ps1` owns screenshot capture, anchor scan, color decoding, checksum validation, and JSON output. It should not send input.

`tools/mgba/run_beacon_repro.ahk` owns mGBA launch, waiting for beacon states, sending inputs, timeout handling, and artifact paths. It should not make decisions from screenshots other than parsed beacon data.

Use AutoHotkey v1 syntax for `.ahk` scripts because the local executable is AutoHotkey v1. Use PowerShell with `System.Drawing` for Windows screenshot and pixel reads unless a later decision log entry records a better local dependency.

The first implementation should use these route assertions:

`gender=2` proves the selected player gender is female. `nameLen=1` and `nameChar0=1` prove the name is exactly `A` for this pipeline. `mapKind=1` and `stageId=9` prove the truck scene is reached and ready. `stageId=12` and `mapKind=4` prove first Pokemon selection is ready.

## Revision Notes

Revision Note (2026-04-26): Initial ExecPlan for replacing timing-driven mGBA early-game automation with a debug-only visual beacon, external beacon sampler, staged AutoHotkey runner, truck verification target, and first Pokemon selection target.
