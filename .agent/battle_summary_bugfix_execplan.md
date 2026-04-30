# Fix the initial-battle Pokemon summary screen glitch

Status: Complete as of 2026-04-29. The fix is committed in `caee916773`, pushed to `origin/main`, and validated by three headless battle-summary runs that reached a normal Torchic summary screen.

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan must be maintained according to `.agent/PLANS.md`. Milestones are gated and strictly ordered: complete and verify Milestone N before starting Milestone N+1. Do not stop at a completed milestone unless the full plan is complete or a real escalation is required.

## Purpose / Big Picture

The player should be able to pick a starter, enter the first battle, open the party menu, choose the starter, select Summary, and see a normal Pokemon summary screen. The current headless automation proves the route can reach the Summary command, but the final screenshot is not a valid summary screen. It is black or glitched, and the summary-screen beacon is never observed.

After this plan is complete, the headless mGBA route will prove the game reaches a normal summary screen in that initial battle without relying on wall-clock timings. The proof must include a JSON result whose summary fields distinguish route success from bug success, plus a screenshot artifact showing a normal summary screen rather than a black or corrupted frame.

## Progress

- [x] (2026-04-29 19:10-04:00) Read `.agent/PLANS.md` and confirmed the local ExecPlan protocol for self-contained, living, milestone-gated plans.
- [x] (2026-04-29 19:12-04:00) Captured the current repro evidence from `build/mgba_lua_spike/battle-summary-repro-final/run_result.json`.
- [x] (2026-04-29 19:15-04:00) Identified the first code paths and suspect local changes: `src/party_menu.c::CursorCb_Summary`, `src/party_menu.c::CB2_ShowPokemonSummaryScreen`, `src/pokemon_summary_screen.c::ShowPokemonSummaryScreen`, `src/pokemon_summary_screen.c::CB2_InitSummaryScreen`, `src/pokemon_summary_screen.c::LoadGraphics`, and `src/pokemon_summary_screen.c::LoadMonGfxAndSprite`.
- [x] (2026-04-29 19:12-04:00) Milestone 1 complete. `tools/mgba/mgba_lua_spike.py` now distinguishes `routeOk`, `artifactOk`, `summaryValidationOk`, and `bugObserved`. The rebuilt automation ROM reproduced the failure at `build/mgba_lua_spike/battle-summary-debug/m1-current/run_result.json`: `routeOk=true`, `artifactOk=true`, `summaryScreenReached=false`, `summaryValidationOk=false`, `bugObserved=true`, `ok=false`; screenshot `build/mgba_lua_spike/battle-summary-debug/m1-current/battle_summary_attempt.png` is a black/glitched frame with a tiny colored artifact.
- [x] (2026-04-29 19:25-04:00) Milestone 2 complete. Guarded summary-init beacons and runner history were added, including a lower-impact `AutomationBeacon_SetDebugState` path that writes only the VRAM tile read by Lua. The repro at `build/mgba_lua_spike/battle-summary-debug/m2-instrumented/run_result.json` shows `routeOk=true`, `artifactOk=true`, `summaryRequestedObserved=true`, `summaryScreenReached=false`, and the last debug beacon is `LOAD_GRAPHICS_STATE` with `summaryLoadGraphicsState=6` and `summarySwitchCounter=14`; screenshot `build/mgba_lua_spike/battle-summary-debug/m2-instrumented/battle_summary_attempt.png` is still the black/glitched frame.
- [x] (2026-04-29 19:27-04:00) Milestone 3 complete. Created scratch worktree `build/worktrees/battle-summary-origin-master` at `origin/master`, applied the prior automation commits without committing, built it with the builder image, and ran the same route from container 5. Result `build/worktrees/battle-summary-origin-master/build/mgba_lua_spike/battle-summary-base-origin-master/run_result.json` shows `attemptedSummary=true`, `summaryScreenReached=true`, `summaryError=null`, and final substage 5. The screenshot helper failed in that scratch worktree (`failed to build state screenshot helper: []`), but the base gameplay reached the summary-screen beacon.
- [x] (2026-04-29 19:31-04:00) Milestone 4 complete. Hypothesis A passed. The tested change keeps in-battle summary screens on the battle-owned `gMonSpritesGfxPtr` path and avoids creating or preferring a summary-owned manager while `gMain.inBattle` is true. Repro `build/mgba_lua_spike/battle-summary-debug/m4-hyp-a/run_result.json` shows `ok=true`, `routeOk=true`, `artifactOk=true`, `summaryScreenReached=true`, and `bugObserved=false`; screenshot `build/mgba_lua_spike/battle-summary-debug/m4-hyp-a/battle_summary_attempt.png` is a normal Torchic summary screen.
- [x] (2026-04-29 19:32-04:00) Milestone 5 complete. The passing Hypothesis A change was kept as the production fix in `src/pokemon_summary_screen.c`: non-battle summaries may create a summary manager, but in-battle summaries use existing battle sprite graphics and the standard `SetMultiuseSpriteTemplateToPokemon` path. Temporary summary-init probes are retained only behind `AUTOMATION_BEACON` because they provide useful future diagnostics and use the low-impact VRAM debug-state path.
- [x] (2026-04-29 19:48-04:00) Milestone 6 complete. `git diff --check` passed, Python and Lua syntax checks passed in container 5, the automation ROM built with the builder image, and the fixed battle-summary route passed 3/3 in headless mGBA. The three result JSONs are `build/mgba_lua_spike/battle-summary-fixed/run-1/run_result.json`, `build/mgba_lua_spike/battle-summary-fixed/run-2/run_result.json`, and `build/mgba_lua_spike/battle-summary-fixed/run-3/run_result.json`; each records `ok=true`, `routeOk=true`, `artifactOk=true`, `summaryScreenReached=true`, and `bugObserved=false`. Screenshots `build/mgba_lua_spike/battle-summary-fixed/run-1/battle_summary_attempt.png`, `build/mgba_lua_spike/battle-summary-fixed/run-2/battle_summary_attempt.png`, and `build/mgba_lua_spike/battle-summary-fixed/run-3/battle_summary_attempt.png` are normal Torchic summary screens. The non-automation ROM build passed with `make NO_MULTIBOOT=1`. `make check NO_MULTIBOOT=1` ran to completion but failed with the repository's broad existing-style battle-suite failures: 108 failed, 18 known failing, 777 TODO, 3067 passed, 3970 total.

## Surprises & Discoveries

- Observation: The latest repro result marks `ok: true`, but that currently means the route and artifact capture completed. It does not mean the summary screen is valid.
  Evidence: `build/mgba_lua_spike/battle-summary-repro-final/run_result.json` has `attemptedSummary: true`, `summaryScreenReached: false`, `summaryError.error: timeout`, and `finalBeacon.substageId: 4`, which is `SUMMARY_REQUESTED`.

- Observation: The summary-screen beacon never fires in the current failure.
  Evidence: `tools/mgba/mgba_lua_spike.py` waits for `REPRO_SUBSTAGE_SUMMARY_SCREEN = 5`, while the final result remains at `substageId: 4` for roughly the whole `summary-timeout` window.

- Observation: The screenshot capture path is working even though the screen is invalid.
  Evidence: `finalScreenArtifact.validPng: true`, `finalScreenArtifact.size: 302`, and `finalScreenArtifact.method: savestate-helper` in the result JSON.

- Observation: Rendering the bad savestate with the helper prints repeated mGBA illegal-opcode messages, which is evidence of a corrupted or invalid execution state after Summary is selected, not merely a delayed UI transition.
  Evidence: `finalScreenArtifact.stdout` contains many `GBA: Illegal opcode: 0000ea00` lines.

- Observation: Recent local code in `src/pokemon_summary_screen.c` creates or uses a summary sprite manager even during battle, and it is a high-value suspect but not yet a proven cause.
  Evidence: `git blame` attributes the isolated sprite-manager block in `ShowPokemonSummaryScreen` and fallback manager logic in `LoadMonGfxAndSprite` to local commit `7371596cfe checkpoint: fixed-rng`.

- Observation: Container 5 has the mGBA, Python, and Lua runtime needed for the route, but it does not have `arm-none-eabi-*` on `PATH` for ROM compilation.
  Evidence: `docker exec devkit-ouro8-dev-agent-5 ... make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1` failed with `arm-none-eabi-gcc: command not found`; `docker run --rm ... pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1` succeeded and produced `pokeemerald.gba`.

- Observation: Summary initialization enters `LoadGraphics()` but does not get past state 6, which calls `DecompressGraphics()`.
  Evidence: `summaryBeaconHistory` in `build/mgba_lua_spike/battle-summary-debug/m2-instrumented/run_result.json` records `SUMMARY_REQUESTED`, then `LOAD_GRAPHICS_STATE` with `summaryLoadGraphicsState=6`, and the last beacon remains state 6 with `summarySwitchCounter=14` until timeout.

- Observation: The state-6 finding is not solely caused by repeated sprite/palette allocation from debug beacons.
  Evidence: After adding `AutomationBeacon_SetDebugState`, summary-init probes write only the VRAM tile read by Lua during summary initialization; the rerun still stalls at `LOAD_GRAPHICS_STATE` state 6.

- Observation: Upstream `origin/master` plus the prior route automation reaches the summary screen in the same initial-battle route.
  Evidence: `build/worktrees/battle-summary-origin-master/build/mgba_lua_spike/battle-summary-base-origin-master/run_result.json` records `attemptedSummary=true`, `summaryScreenReached=true`, `summaryError=null`, and `finalBeacon.substageId=5`.

- Observation: The scratch base screenshot helper failed, but that is separate from the gameplay comparison.
  Evidence: The base result's `screenArtifact.error` is `RuntimeError: failed to build state screenshot helper: []`, while the final beacon still proves `SUMMARY_SCREEN`.

- Observation: Avoiding a summary-owned sprite manager in battle fixes the state-6 summary initialization stall.
  Evidence: `build/mgba_lua_spike/battle-summary-debug/m4-hyp-a/run_result.json` records `ok=true`, `summaryScreenReached=true`, and `summaryBeaconHistory` transitions from `SUMMARY_REQUESTED` directly to `SUMMARY_SCREEN`.

- Observation: The full `make check NO_MULTIBOOT=1` suite is not green on this branch/environment, but its failures are broad battle DSL expectations unrelated to the summary-screen graphics path.
  Evidence: The completed check reported 108 failed tests, 18 known failing tests, 777 TODO tests, 3067 passed tests, and 3970 total tests, with representative failures around ability popups, spread-move damage, weather damage, shared power, and pursuit behavior.

## Decision Log

- Decision: Start by making the repro result semantics explicit before changing game logic.
  Rationale: The current runner's `ok: true` can be misread as summary-screen success. The plan needs a stable pass/fail contract before experiments begin.
  Date/Author: 2026-04-29 / Codex

- Decision: Instrument summary initialization before attempting a fix.
  Rationale: The existing evidence only says that `SUMMARY_REQUESTED` was reached and `SUMMARY_SCREEN` was not. The next useful fact is where the engine stops between selecting Summary and entering `MainCB2` for the summary screen.
  Date/Author: 2026-04-29 / Codex

- Decision: Use scratch `git worktree` directories under `build/worktrees/` for base-source comparisons and hypothesis patches.
  Rationale: This avoids destructive operations in the main working tree, respects the user's permission to compare against base source, and keeps experiments repeatable without resetting the active branch.
  Date/Author: 2026-04-29 / Codex

- Decision: Test hypotheses as one isolated diff at a time.
  Rationale: The failure could be sprite-resource related, callback sequencing related, mode related, or automation related. Combining changes would make the result hard to trust.
  Date/Author: 2026-04-29 / Codex

- Decision: Use `pokeemerald-expansion:builder` for ROM builds and container 5 for mGBA/Python repro execution.
  Rationale: The planned container-5 build command is not valid in the current environment because the ARM toolchain is absent there, while the existing builder image compiles the ROM successfully. Keeping mGBA execution in container 5 preserves the proven runtime environment.
  Date/Author: 2026-04-29 / Codex

- Decision: Treat the bug as branch-local after Milestone 3.
  Rationale: The upstream scratch worktree reaches the summary-screen beacon with the same route. The remaining hypotheses should focus on local changes between `origin/master` and this branch, especially summary-screen allocation/layout and battle summary graphics changes.
  Date/Author: 2026-04-29 / Codex

- Decision: Promote Hypothesis A and skip lower-priority hypotheses B through E.
  Rationale: Hypothesis A produced the required normal summary screen with a minimal, root-cause-shaped change. Continuing to alter summary mode, stubbing sprites, or changing callback sequencing would add risk without improving confidence.
  Date/Author: 2026-04-29 / Codex

- Decision: Do not block this localized summary-screen fix on the broad failing `make check` baseline.
  Rationale: The targeted route passes repeatedly in headless mGBA, both automation and non-automation ROM builds pass, and the `make check` failures are distributed across unrelated battle mechanics rather than the touched summary or automation files.
  Date/Author: 2026-04-29 / Codex

## Outcomes & Retrospective

The implementation fixes the initial-battle summary-screen glitch by preserving battle-owned Pokemon sprite graphics while the summary screen is opened from battle. The branch-local summary manager changes were valid for non-battle summaries but unsafe in battle: creating or preferring a summary-owned manager while `gMain.inBattle` is true left summary initialization stalled in `LoadGraphics()` state 6 and produced black/glitched screenshot artifacts. The durable rule is now that non-battle summaries may create and use the summary manager, while in-battle summaries use `gMonSpritesGfxPtr` and the standard battle-compatible `SetMultiuseSpriteTemplateToPokemon` path.

The automation runner now reports route success separately from summary-screen success. That distinction was necessary because the original repro could return `ok=true` even when the final screenshot was invalid. The retained `AUTOMATION_BEACON` summary-init probes are intentionally guarded and low-impact, and they provide useful future regression evidence without affecting default builds.

The fixed route passed three consecutive headless mGBA runs and produced normal Torchic summary screenshots. A return-path assertion from the summary screen back to party or battle remains a follow-up because adding it would expand the runner state machine beyond the bugfix's root cause. It should be added before relying on this route as a full menu lifecycle regression test.

## Context and Orientation

This repository builds a Game Boy Advance ROM. The relevant output is `pokeemerald.gba` at the repository root after a successful build. The project uses C source files under `src/`, headers under `include/`, generated artifacts under `build/`, and automation tools under `tools/mgba/`.

An automation beacon is a small piece of debug state exposed by the ROM so an external script can tell where the game is. The beacon code lives in `include/automation_beacon.h` and `src/automation_beacon.c`. The mGBA bridge reads the beacon from emulator memory or VRAM so the Python runner can press buttons only when the game is ready. This matters because the route must work at high emulator speed and with dropped frames, where fixed sleeps are unreliable.

mGBA is the emulator used for headless testing. Headless means it runs without a normal GUI window. In this environment, mGBA's direct Lua screenshot APIs are not sufficient, so the current tooling saves a state through Lua and converts that state to a PNG using `tools/mgba/mgba_state_screenshot.c`. The artifact can be valid as a PNG while still showing a bad in-game screen.

The current battle-summary route is implemented in `tools/mgba/mgba_lua_spike.py`. In `battle-summary` mode it drives a female `A` save from a clean boot through starter selection into the first battle. It opens the battle action menu, selects Pokemon, opens the party menu, chooses the starter, selects Summary, then waits for a summary-screen beacon. Lua support for input and savestate capture lives in `tools/mgba/mgba_lua_bridge.lua`.

The player-facing menu path is in `src/party_menu.c`. `CursorCb_Summary` handles the Summary action from the party Pokemon action menu. `CB2_ShowPokemonSummaryScreen` calls `UpdatePartyToBattleOrder()` when `gPartyMenu.menuType == PARTY_MENU_TYPE_IN_BATTLE`, then calls `ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gPlayerParty, gPartyMenu.slotId, gPlayerPartyCount - 1, CB2_ReturnToPartyMenuFromSummaryScreen)`.

The summary screen implementation is in `src/pokemon_summary_screen.c`. `ShowPokemonSummaryScreen` allocates and initializes summary state, then calls `SetMainCallback2(CB2_InitSummaryScreen)`. `CB2_InitSummaryScreen` repeatedly calls `LoadGraphics()` until graphics loading is complete. `LoadGraphics()` advances `gMain.state` through numbered initialization cases, and only after the final case does it call `SetMainCallback2(MainCB2)`. The current `SUMMARY_SCREEN` beacon is emitted in `MainCB2`, so it will not fire if the game crashes or stalls during `CB2_InitSummaryScreen` or `LoadGraphics()`.

The first high-value suspect area is sprite setup inside `src/pokemon_summary_screen.c`. A Pokemon sprite manager is a structure that owns sprite graphics memory. Battle code and summary-screen code both need Pokemon sprites, and using the wrong manager or freeing it at the wrong time can corrupt graphics or control flow. Recent local code tries to ensure the summary screen has an isolated sprite manager even in battle and changes `LoadMonGfxAndSprite` to load from that manager or fall back to battle sprite memory. This must be tested, not assumed.

## Milestone 1: Reproduce the current failure with unambiguous result semantics

First, rerun the current repro exactly enough to prove the failure is stable on the current HEAD. The executor must use the container that has the mGBA and Python environment, not the host if the host lacks those tools. The known container name is `devkit-ouro8-dev-agent-5`, and the expected repository path inside it is `/workspaces/dev/pokeemerald-expansion-shared-power`.

If the container is not running, start or scale the devkit environment in the same way previous automation work did. Do not run ouroboros or Scala code on the host for this plan. This plan is about the C ROM repository and the mGBA automation route.

Run from the repository root on the host:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"'

Then run the repro into a fresh output directory and unique port:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; rm -rf build/mgba_lua_spike/battle-summary-debug/m1-current; python3 tools/mgba/mgba_lua_spike.py --mode battle-summary --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/battle-summary-debug/m1-current --port 46630 --connect-timeout 20 --starter-timeout 600 --battle-timeout 300 --summary-timeout 45'

Update `tools/mgba/mgba_lua_spike.py` if necessary so the result JSON separates these meanings:

- `routeOk`: the automation reached and selected the Summary command.
- `artifactOk`: a PNG artifact was created and can be decoded as a PNG.
- `summaryScreenReached`: the summary-screen beacon was observed.
- `bugObserved`: Summary was requested but a normal summary screen was not reached within the timeout, or the screenshot classifier identifies a black/glitched screen.
- `ok`: the overall command status. For a bugfix validation command, this should only be true when the intended assertion passes. If the command is intentionally a repro command, then the JSON must make that explicit with a field such as `target: BATTLE_SUMMARY_BUG_REPRO`.

The acceptance for this milestone is not a fix. Acceptance is that the current broken behavior is recorded with clear semantics. The result must show `routeOk: true`, `summaryScreenReached: false`, and `bugObserved: true`, or equivalent names with the same meanings documented in the JSON. The PNG must exist, and the plan must record its path.

If this milestone requires editing the Python runner, keep the edit small and commit it only after it is validated with `python3 -m py_compile tools/mgba/mgba_lua_spike.py` inside container 5 and `git diff --check` on the host.

## Milestone 2: Add summary-initialization instrumentation

Next, add beacons or logged debug fields that narrow the failure from "after Summary was requested" to a specific summary initialization step. Keep this instrumentation behind `AUTOMATION_BEACON` so default builds are not affected.

In `include/automation_beacon.h`, extend the battle-summary repro substage enum with additional substages. Use names that describe engine state, not hypotheses. Suggested names are:

    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SHOW_SUMMARY_CALLBACK
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SHOW_SUMMARY_ENTERED
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_INIT_SUMMARY_SCREEN
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_LOAD_GRAPHICS_STATE
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_LOAD_MON_GFX_BEGIN
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_LOAD_MON_GFX_WAITING
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_LOAD_MON_GFX_DONE
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SUMMARY_SCREEN

Preserve the existing numeric meanings when feasible so old artifacts remain understandable. If renumbering is unavoidable, update `tools/mgba/mgba_lua_spike.py` in the same commit.

In `src/party_menu.c`, emit a beacon at the beginning of `CB2_ShowPokemonSummaryScreen`. Include enough debug data in existing beacon fields to tell whether the party menu type is `PARTY_MENU_TYPE_IN_BATTLE`, which slot is selected, and whether the menu is about to call `SUMMARY_MODE_LOCK_MOVES` or `SUMMARY_MODE_NORMAL`.

In `src/pokemon_summary_screen.c`, emit beacons in these locations:

- At the start of `ShowPokemonSummaryScreen`, after allocation succeeds and before `SetMainCallback2(CB2_InitSummaryScreen)`.
- At the start of `CB2_InitSummaryScreen`.
- During `LoadGraphics()`, with the current `gMain.state` encoded in a beacon field.
- In `LoadMonGfxAndSprite`, before sprite work starts, when it returns `SPRITE_NONE`, and when it returns a real sprite id.
- Around allocation or manager checks that can fail: `sMonSummaryScreen == NULL`, `MonSpritesGfxManager_GetSpritePtr(...) == NULL`, `gMain.inBattle`, and `gMonSpritesGfxPtr == NULL`.

Do not add large logging systems unless beacons cannot carry the required facts. Prefer small fields that the existing Python reader already decodes, such as substage, flags, routeErrorCode, or reserved bytes. If a field is repurposed for debug, document it in this plan and in comments near the instrumentation.

Update `tools/mgba/mgba_lua_spike.py` so `battle-summary` mode captures the last beacon history around the failure, not just the final beacon. It should write a compact list of recent substages and frames into `run_result.json`, enough to answer "what was the last observed summary init step?" without replaying the whole run.

Run the same build and repro commands as Milestone 1, writing to `build/mgba_lua_spike/battle-summary-debug/m2-instrumented`. Acceptance for this milestone is a result JSON that identifies the last successful summary initialization state. Examples of acceptable findings are: "stalled at `LOAD_GRAPHICS_STATE` case 17 before `LoadMonGfxAndSprite` returned", "crashed immediately after entering `LoadMonGfxAndSprite`", or "never entered `CB2_ShowPokemonSummaryScreen` despite Summary being requested".

## Milestone 3: Compare against base source in scratch worktrees

This milestone determines whether the failure is inherited from upstream/base code, introduced by this branch, or introduced by the automation itself. Do not reset or checkout the main working tree. Create scratch worktrees under `build/worktrees/`, which is an ignored build area.

Start by creating a current-HEAD control worktree if a clean comparison is useful:

    mkdir -p build/worktrees
    git worktree add --detach build/worktrees/battle-summary-current HEAD

Then create an upstream/base worktree. The available upstream branch in this repository is `origin/master`; `origin/main` is this repo's remote main and may already include local snapshot history. Fetch first if network is available:

    git fetch origin
    git worktree add --detach build/worktrees/battle-summary-origin-master origin/master

If `origin/master` cannot build because it lacks local project configuration or shared-power changes, record that exact blocker in `Surprises & Discoveries`, then use the closest safe base that builds. Do not hide this decision.

Apply only the minimum automation needed to build and run the battle-summary route in the base worktree. Prefer copying the current automation files and guarded beacon code rather than cherry-picking unrelated gameplay commits. If cherry-picking is cleaner, start with the automation commits in this order and stop if conflicts become too broad:

    aad6f7fb01 Add automation beacon headless tooling and plans
    5ad583570b Add semantic automation beacon v2 proof
    2091e87632 Add battle summary automation repro

If those commits include unrelated behavior, instead copy these files from the main working tree and manually apply the smallest needed C hooks:

- `include/automation_beacon.h`
- `src/automation_beacon.c`
- `tools/mgba/mgba_lua_spike.py`
- `tools/mgba/mgba_lua_bridge.lua`
- `tools/mgba/mgba_state_screenshot.c`
- the `AUTOMATION_BEACON` beacon calls in menu and summary code needed by the route

Build the base worktree with automation enabled and run the same repro command from inside container 5, changing the container path to the mounted worktree path if it is available there. If container 5 only mounts the main repository, copy the scratch worktree into a mounted scratch directory or bind it through the devkit mechanism used for this project. Record the exact path used.

Acceptance for this milestone is a clear comparison:

- Base source plus minimal automation reaches a normal summary screen.
- Base source plus minimal automation reproduces the same black/glitched screen.
- Base source cannot be validly compared, with the exact build or route blocker documented.

This result determines how to prioritize the next milestone. If base passes, branch-local gameplay or summary changes are likely. If base fails, the fix may need to be upstream-style engine repair rather than reverting a local change.

## Milestone 4: Test isolated hypotheses until one passes

Use the instrumented result from Milestone 2 and the base comparison from Milestone 3 to choose hypotheses. Each hypothesis must be tested as a single isolated diff in a scratch worktree or temporary branch. Do not combine hypotheses in one run unless a previous run proved one change is required and still insufficient.

Hypothesis A: summary sprite-manager changes are corrupting or starving the summary screen during battle. Test this by reverting only the local summary sprite-manager changes in `src/pokemon_summary_screen.c` or by forcing the initial-battle summary path to use the battle sprite memory path that upstream uses. The important code is the manager creation in `ShowPokemonSummaryScreen` and the manager/fallback choice in `LoadMonGfxAndSprite`. Acceptance for this hypothesis is `summaryScreenReached: true` plus a normal summary screenshot.

Hypothesis B: `SUMMARY_MODE_LOCK_MOVES` is wrong for this path in the initial battle. Test this by changing only the in-battle call in `CB2_ShowPokemonSummaryScreen` to `SUMMARY_MODE_NORMAL`, or to the mode used by base source if Milestone 3 identifies a different expected call. Acceptance is the same: the route reaches a normal summary screen, and pressing B returns safely to party or battle.

Hypothesis C: sprite or graphics resource loading fails before the summary UI can finish initialization. Test this by stubbing only the Pokemon portrait sprite creation in `LoadMonGfxAndSprite`, such as returning a placeholder sprite id or bypassing species sprite decompression, while leaving the rest of summary graphics intact. If the summary UI appears without the portrait, the root cause is resource-specific rather than menu or callback sequencing.

Hypothesis D: callback sequencing frees or reorders party/battle resources before summary initialization can use them. Test this by isolating changes around `Task_ClosePartyMenu`, `UpdatePartyToBattleOrder`, `FreeAllWindowBuffers`, and return callbacks. Use instrumentation to prove whether the crash occurs before or after `SetMainCallback2(CB2_InitSummaryScreen)`. Do not ship a broad delay-based fix; this is a diagnostic path.

Hypothesis E: automation or beacon writes perturb state. Test this only if the first four hypotheses do not explain the failure. Run a build with the minimum beacons needed for route gating and no extra summary probes, or replay from a savestate with manual Lua inputs after Summary is selected. If the screen works only when probes are removed, reduce beacon writes or move them away from fragile graphics initialization.

For each hypothesis, record in this plan:

- The exact files changed.
- The exact command used to build.
- The exact command used to run the repro.
- The result JSON path.
- Whether `summaryScreenReached` became true.
- The screenshot path and whether it is normal, black, or glitched.
- The decision to promote or discard the hypothesis.

Acceptance for this milestone is at least one isolated hypothesis producing a normal summary screen, or a documented escalation with evidence that all feasible hypotheses failed and why a broader migration or debugger-level path is needed.

## Milestone 5: Implement the smallest durable fix in the main worktree

Only after Milestone 4 identifies a passing hypothesis, apply the smallest fix to the main worktree. Keep the fix in production game code focused on the root cause. Do not leave experimental stubs, broad sleeps, or route-specific hacks in production code.

If the fix changes summary sprite-manager behavior, make the ownership rules explicit in code. A durable rule might be: the summary screen may create a private manager when no battle manager is active, but it must not claim or destroy a battle-owned manager while `gMain.inBattle` is true. This is only an example; implement the rule proven by Milestone 4, not this wording by default.

If the fix changes summary mode, ensure the mode still prevents move switching or editing in battle if that was the purpose of `SUMMARY_MODE_LOCK_MOVES`. If `SUMMARY_MODE_NORMAL` is needed to avoid a crash but permits incorrect battle behavior, add a targeted guard elsewhere rather than accepting a gameplay regression.

If the fix changes callback sequencing, preserve the ability to return from summary to the party menu and then back to battle. The route must not strand the player in a menu or reorder the party incorrectly.

Remove temporary instrumentation that is not necessary for long-term automation. Keep useful battle-summary beacons if they are guarded by `AUTOMATION_BEACON` and serve the regression test. Update `PATCH_NOTES.md` at the top for every code, data, or docs change. Because the final commit hash is not known before commit, use `(commit pending)` while editing and update it only if the repository convention for this branch requires resolved hashes.

Acceptance for this milestone is a minimal diff in the main worktree that builds and makes the repro pass once. The diff must not include ROM files or build artifacts.

## Milestone 6: Validate, document, and commit

Run formatting and lightweight syntax checks first:

    git diff --check
    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; luac5.2 -p tools/mgba/mgba_lua_bridge.lua'

Build the automation ROM:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"'

Run the fixed route at least three times into separate directories and ports:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; for i in 1 2 3; do out="build/mgba_lua_spike/battle-summary-fixed/run-$i"; rm -rf "$out"; python3 tools/mgba/mgba_lua_spike.py --mode battle-summary --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --bridge tools/mgba/mgba_lua_bridge.lua --output-dir "$out" --port "$((46640 + i))" --connect-timeout 20 --starter-timeout 600 --battle-timeout 300 --summary-timeout 45; done'

Each run must meet these conditions:

- Summary was requested from the initial battle party menu.
- `summaryScreenReached` is true.
- The final screenshot is a normal Pokemon summary screen, not black, not a tiny-pixel artifact, and not corrupted.
- The result JSON does not contain illegal-opcode output from the screenshot helper as evidence of the final visible state.
- The route is beacon-gated, not sleep-gated.

Add or run a return-path check if not already part of the runner. The check should press B after the summary screen is reached and prove the game returns to either the party menu or battle action flow without crashing. If implementing that in the Python runner is too large for this fix, record it as a follow-up in `Outcomes & Retrospective`, but do not skip it if the final code touched callbacks or party ordering.

Run a non-automation build if time permits:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make NO_MULTIBOOT=1 -j"$(nproc)"

Run `make check` if the container has the required blobs and test runner support. If it is not feasible, document the blocker and the commands that did pass.

Before committing, inspect the diff:

    git status --short
    git diff --stat
    git diff -- PATCH_NOTES.md .agent/battle_summary_bugfix_execplan.md include/automation_beacon.h src/party_menu.c src/pokemon_summary_screen.c tools/mgba/mgba_lua_spike.py tools/mgba/mgba_lua_bridge.lua

Commit all intended source, tool, documentation, and patch-note changes. Do not commit `pokeemerald.gba`, savestates, screenshots, or files under `build/`.

Acceptance for the full plan is a clean committed state where the final commit includes the fix, updated automation if needed, updated `PATCH_NOTES.md`, and this ExecPlan's `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` sections updated with the actual evidence.

## Validation and Acceptance

The primary acceptance behavior is user-visible: after selecting Summary for the starter from the party menu during the initial battle, the game shows a normal Pokemon summary screen. This must be proven by headless automation and a screenshot artifact.

A passing result JSON must communicate at least these facts:

- The route reached the first battle after starter selection.
- The route opened the party menu from the battle action menu.
- The route selected the Pokemon and chose Summary.
- The summary screen was reached by beacon, not by fixed timing.
- The screenshot artifact is valid and visually a normal summary screen.

The final response to the operator must include the commit hash, the key commands run, the result JSON paths, and the screenshot paths.

## Idempotence and Recovery

All generated experiment outputs should live under `build/`, which is ignored. It is safe to delete `build/mgba_lua_spike/battle-summary-debug/`, `build/mgba_lua_spike/battle-summary-fixed/`, and `build/worktrees/` after useful evidence has been recorded in this plan.

Do not run `git reset --hard` or `git checkout --` in the main working tree. To discard a failed hypothesis, remove the scratch worktree with `git worktree remove build/worktrees/<name>` after confirming no useful uncommitted work remains there. If a scratch worktree contains useful code, copy the specific patch back intentionally with `git diff` and `git apply`, or commit it on a temporary branch and cherry-pick only the needed commit.

If container 5 is unavailable, document the exact error and either start the devkit environment or use an equivalent container with `/usr/local/bin/mgba-headless`, `python3`, `luac5.2`, and the repository mounted at a known path. Do not silently switch to host execution for the mGBA/Python route if that would change the environment.

If `origin/master` or another base branch cannot be built, record the command and error in `Surprises & Discoveries`, then proceed with the closest buildable comparison. The plan does not require destructive local reverts.

## Artifacts and Notes

Current failing artifact from the pre-plan repro:

    JSON: build/mgba_lua_spike/battle-summary-repro-final/run_result.json
    PNG:  build/mgba_lua_spike/battle-summary-repro-final/battle_summary_attempt.png
    Key fields: attemptedSummary=true, summaryScreenReached=false, summaryError.error=timeout, finalBeacon.stageId=14, finalBeacon.substageId=4
    Screenshot capture: validPng=true, method=savestate-helper, size=302 bytes
    Helper output: repeated "GBA: Illegal opcode: 0000ea00" lines

Current substage names in `include/automation_beacon.h` before this bugfix plan:

    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_INACTIVE
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_ACTION_MENU
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_PARTY_MENU
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_PARTY_MON_MENU
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SUMMARY_REQUESTED
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SUMMARY_SCREEN

Current runner constants in `tools/mgba/mgba_lua_spike.py` before this bugfix plan:

    STAGE_BATTLE_SUMMARY_REPRO = 14
    REPRO_SUBSTAGE_SUMMARY_REQUESTED = 4
    REPRO_SUBSTAGE_SUMMARY_SCREEN = 5

Important current call chain:

    src/party_menu.c::CursorCb_Summary
    src/party_menu.c::CB2_ShowPokemonSummaryScreen
    src/pokemon_summary_screen.c::ShowPokemonSummaryScreen
    src/pokemon_summary_screen.c::CB2_InitSummaryScreen
    src/pokemon_summary_screen.c::LoadGraphics
    src/pokemon_summary_screen.c::LoadMonGfxAndSprite
    src/pokemon_summary_screen.c::MainCB2

## Interfaces and Dependencies

The C code must continue to compile with GNU17 and this repo's warning-as-error settings. Use existing naming style: functions in PascalCase, local variables in camelCase, macros and enum constants in upper snake case. Keep C indentation at 4 spaces.

`include/automation_beacon.h` defines stage and substage constants used by both the C ROM and Python runner. Any new constants added for this plan must be consumed consistently by `tools/mgba/mgba_lua_spike.py`.

`src/automation_beacon.c` is the only place that should understand how beacon rows are encoded for the emulator. If new debug fields are needed, add them in a way that preserves old fields needed by the existing route.

`src/party_menu.c` owns the party menu and the transition from the party Pokemon action menu to the summary screen. It should remain the only place deciding whether the summary request came from battle or field context.

`src/pokemon_summary_screen.c` owns summary screen allocation, graphics loading, sprite loading, page rendering, and return callbacks. A fix to summary graphics or callback setup belongs here unless Milestone 4 proves the root cause is earlier.

`tools/mgba/mgba_lua_spike.py` owns the deterministic route and should report evidence in JSON. It should not encode gameplay fixes. It may gain better assertions, result fields, screenshot classification, or a return-path check.

`tools/mgba/mgba_lua_bridge.lua` owns emulator-side input, beacon reads, and savestate capture. It should only change if Milestone 2 or 6 needs better artifact capture or beacon history.

`PATCH_NOTES.md` must receive a concise top entry after each code, data, or docs change according to `AGENTS.md`.

Revision Note (2026-04-29): Initial plan created from the current headless battle-summary repro. It defines the existing black/glitched screenshot as a bug observation, not a passing summary result, and lays out gated milestones for instrumentation, base comparison, isolated hypothesis testing, implementation, and validation.
