# Capture GUI evidence for Mudkip's initial-battle summary screen

Status: Superseded and completed as of 2026-05-01. The valid GUI proof is the beacon-driven WSLg SDL mGBA route at `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`; earlier timing replay and savestate-load attempts are retained only as discarded troubleshooting history.

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md` from the repository root. It is intentionally scoped to the user-visible regression evidence requested on 2026-05-01: a desktop screenshot with a GUI mGBA instance showing Mudkip's Pokemon summary, reached from the initial Zigzagoon battle on the latest local build.

## Purpose / Big Picture

The player reports that the latest build still black-screens or crashes when opening Mudkip's information during the first battle. Previous automation validated the same flow with Torchic, so this plan focuses on reproducing the exact Mudkip path in a visible GUI emulator and preserving evidence that a human can inspect. The immediate success condition is a screenshot of the user's desktop showing a running mGBA window on Mudkip's summary screen after selecting Mudkip in the initial Zigzagoon battle.

If the GUI path still crashes before the screenshot can be captured, this plan becomes the record of where the failure occurs and what fallback automation was used to isolate it. A code fix is allowed only after the evidence route is established or the crash blocks that route.

## Progress

- [x] (2026-05-01 02:52-04:00) Created this focused regression ExecPlan after finding `.agent/battle_summary_bugfix_execplan.md` was already marked complete for the older Torchic validation path.
- [x] (2026-05-01 02:55-04:00) Built the latest ROM and symbols with the Docker builder using `AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1`.
- [x] (2026-05-01 02:55-04:00) Found Windows GUI mGBA at `C:\Program Files\mGBA\mGBA.exe` and AutoHotkey at `C:\ProgramData\chocolatey\bin\AutoHotkey.exe`; chose GUI mGBA as the primary route.
- [x] (2026-05-01 03:02-04:00) Drove the game from a clean boot through Mudkip selection into the initial Zigzagoon battle with the Python/Lua fallback route; the route saved `build/mgba_gui_evidence/mudkip_headless_route/battle_summary_attempt.ss`.
- [x] (2026-05-01 03:08-04:00) Loaded that state into GUI mGBA and reproduced the bad user-visible result: the GUI window showed a black/battle-background state instead of Mudkip's summary.
- [x] (2026-05-01 03:18-04:00) Reverted the current static in-battle summary portrait buffer back to the battle-owned sprite manager path identified as passing in the prior battle-summary ExecPlan.
- [x] (2026-05-01 03:24-04:00) Rebuilt the ROM and symbols after removing the static in-battle summary buffer; the first player-slot variant reached the summary beacon but still produced a black framebuffer screenshot.
- [x] (2026-05-01 03:32-04:00) Restored the battle-owned opponent sprite slot and verified the headless route reached `SUMMARY_SCREEN`; the savestate helper could not render under Nix because of the known `stub-ld` dynamic-linker issue.
- [x] (2026-05-01 03:37-04:00) Ran the live SDL mGBA route and captured a normal framebuffer summary at `build/mgba_gui_evidence/mudkip_gui_live_route_desktop_capture/battle_summary_attempt.png`, but the first desktop-capture hook failed because PowerShell did not inherit the WSL output-path environment variable.
- [x] (2026-05-01 03:43-04:00) Re-ran the live SDL route after fixing desktop capture and reproduced a reset/crash after Summary was requested, isolating the remaining instability to the in-battle summary sprite animation path rather than the action-menu route or desktop capture.
- [x] (2026-05-01 03:49-04:00) Kept the battle-owned opponent sprite slot and skipped the Pokemon summary animation only while `gMain.inBattle`; the live SDL route passed with `summaryScreenReached=true` and `bugObserved=false`, producing a temporary workaround screenshot at `build/mgba_gui_evidence/mudkip_summary_desktop.png`.
- [x] (2026-05-01 15:02-04:00) Reclassified the animation skip as a temporary workaround after root-cause work showed `origin/master` can animate Mudkip summaries normally.
- [x] (2026-05-01 15:07-04:00) Reduced Shared Power's ordered ability buffers from `ABILITIES_COUNT` to `PARTY_SIZE`, rebuilt, and restored normal in-battle summary animation.
- [x] (2026-05-01 16:15-04:00) Captured the final accepted evidence with the repo-local WSLg SDL mGBA build using Lua beacons, not timing replay: `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`.

## Surprises & Discoveries

- Observation: The existing battle-summary plan is not sufficient for this report because it validates Torchic, not Mudkip.
  Evidence: `.agent/battle_summary_bugfix_execplan.md` says it is complete and its validation screenshots are normal Torchic summary screens.

- Observation: This environment has Windows mGBA available, but no Linux `mgba`, `mgba-qt`, or common Linux screenshot utilities in `PATH`.
  Evidence: `find /mnt/c/Program Files ...` found `/mnt/c/Program Files/mGBA/mGBA.exe`, while `command -v mgba mgba-qt ...` produced no Linux GUI emulator path.

- Observation: The fallback route on the latest build reached the summary-screen beacon for Mudkip but the GUI-loaded state did not show the summary screen.
  Evidence: `build/mgba_gui_evidence/mudkip_headless_route/run_result.json` recorded `summaryScreenReached=true`, while `build/mgba_gui_evidence/mudkip_summary_client_local_after_wait.png` showed the battle background rather than Mudkip's summary.

- Observation: The current static in-battle summary portrait buffer matches the later black-screen experiments, not the earlier visible Mudkip summary experiments.
  Evidence: The existing `build/mgba-battle-summary-static-buffer-mudkip-screenshot/battle_summary_attempt.png` artifact is black, while `build/mgba-battle-summary-final-mudkip-screenshot/battle_summary_attempt.png` shows a Mudkip summary screen.

- Observation: Loading transient battle-summary savestates into a separate GUI frontend is not reliable enough for the requested evidence.
  Evidence: Windows GUI mGBA loads of `build/mgba_gui_evidence/mudkip_headless_route/battle_summary_attempt.ss` and `build/mgba_gui_evidence/mudkip_headless_route_player_slot/battle_summary_attempt.ss` showed black, battle-background, or intro states rather than a stable summary screen.

- Observation: The static summary-owned sprite buffer reaches the summary beacon but can still render as a black frame.
  Evidence: `build/mgba_gui_evidence/mudkip_gui_live_route_debug_full/battle_summary_attempt.png` and older `build/mgba-battle-summary-static-buffer-mudkip-screenshot/battle_summary_attempt.png` are black despite `summaryScreenReached=true`.

- Observation: Restoring the battle-owned opponent sprite slot makes Mudkip render, but allowing the normal summary animation during an in-battle summary can still reset or crash on some live GUI runs.
  Evidence: `build/mgba_gui_evidence/mudkip_gui_live_route_desktop_capture/battle_summary_attempt.png` rendered normally, while the same code path in `build/mgba_gui_evidence/mudkip_gui_live_route_desktop_capture2/run_result.json` reset to stage 0 after `LOAD_GRAPHICS_STATE` state 25 and captured an intro-frame artifact.

- Observation: Suppressing the summary sprite animation only for in-battle summaries avoids the reset while preserving the visible first-frame portrait and summary UI.
  Evidence: `build/mgba_gui_evidence/mudkip_gui_live_route_freeze_desktop/run_result.json` records `ok=true`, `summaryScreenReached=true`, and `bugObserved=false`; its framebuffer artifact and desktop screenshot both show Mudkip's summary.

- Observation: That animation suppression was not the real fix.
  Evidence: After shrinking Shared Power's ordered battle buffers, `src/pokemon_summary_screen.c::CreateMonSprite` again leaves `data[2] = 0`, and `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json` records `ok=true`, `summaryScreenReached=true`, `summaryValidationOk=true`, and `bugObserved=false`.

- Observation: Timing-based GUI replay is not accepted evidence for this route.
  Evidence: A discarded replay attempt drifted into a wrong visual state including the male player sprite, while the accepted route used semantic beacons through `tools/mgba/mgba_lua_bridge.lua` and a live WSLg SDL mGBA process.

- Observation: Loading transient headless savestates into a separate Windows GUI mGBA process is useful for debugging but not reliable enough as proof.
  Evidence: Windows mGBA 0.10.4 did not accept the Lua script bridge on the command line and did not consistently load the battle-summary state, while the WSLg SDL mGBA run drove the state live and captured the desktop before emulator cleanup.

- Observation: The in-run desktop screenshot route must pass the Windows path directly to PowerShell rather than through a WSL environment variable.
  Evidence: The first capture attempt produced a valid stale file but PowerShell reported `$out` was null; after embedding the converted `wslpath -w` result in the PowerShell script, `desktopScreenshot.ok=true`.

## Decision Log

- Decision: Create a new regression evidence plan rather than reopening the completed battle-summary bugfix plan.
  Rationale: The new user report is specific to Mudkip on the latest build, and the acceptance artifact is a GUI desktop screenshot rather than the earlier headless Torchic proof.
  Date/Author: 2026-05-01 / Codex.

- Decision: Prefer GUI mGBA for the primary route and keep the Python spike as a fallback.
  Rationale: The user explicitly permitted GUI mGBA and required a desktop screenshot with a running mGBA instance; headless automation alone cannot satisfy that artifact requirement.
  Date/Author: 2026-05-01 / Codex.

- Decision: Remove the static in-battle summary portrait buffer and return Mudkip summary portrait loading to the battle-owned `gMonSpritesGfxPtr` path.
  Rationale: The static-buffer path reproduced the black GUI result, while the prior battle-summary plan already isolated the battle-owned sprite manager path as the root-cause-shaped passing behavior.
  Date/Author: 2026-05-01 / Codex.

- Decision: Use the battle-owned opponent sprite slot, not the player slot or a static summary-owned buffer, for in-battle summary portrait graphics.
  Rationale: The opponent-slot path matches the upstream and previously passing route; the player-slot and static-buffer variants could reach beacons while still producing black frames or route instability.
  Date/Author: 2026-05-01 / Codex.

- Decision: Do not keep Pokemon summary animation disabled as the final fix.
  Rationale: Baseline comparison and the Shared Power buffer shrink showed the reset was caused by battle-state memory pressure, not by Mudkip's animation path being inherently unsafe.
  Date/Author: 2026-05-01 / Codex.

- Decision: Capture the final artifact from the live WSLg SDL mGBA process with a PowerShell full-desktop screenshot before emulator cleanup.
  Rationale: The user requested a desktop screenshot with a running mGBA instance, and semantic-beacon control of the live process is the reliable proof path for this transient battle-summary state.
  Date/Author: 2026-05-01 / Codex.

## Outcomes & Retrospective

The accepted GUI evidence was captured from a live WSLg SDL mGBA run launched by the automation route and controlled by semantic Lua beacons. The desktop screenshot is `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`; it shows the desktop, the still-running mGBA window, and Mudkip's summary screen from the initial Zigzagoon battle. The matching route result is `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json`, which records `ok=true`, `routeOk=true`, `artifactOk=true`, `summaryScreenReached=true`, `summaryValidationOk=true`, and `bugObserved=false`.

The code fix keeps in-battle summaries on the battle-owned opponent sprite graphics path and keeps normal Pokemon summary animation enabled. The root-cause fix is the Shared Power battle-state memory reduction in `include/battle_shared_power.h`; the earlier static sprite-buffer and animation-suppression attempts were removed. The runner retained the useful retrying battle-action cursor helper, desktop screenshot hook, and savestate artifact output, but savestate loading into a separate GUI process is not treated as validation evidence.

## Context and Orientation

This repository builds a Game Boy Advance ROM named `pokeemerald.gba`. The initial battle begins after the player chooses a starter from Professor Birch's bag on Route 101; choosing Mudkip enters a battle against Zigzagoon. From the battle action menu, the player can open the Pokemon party menu, select Mudkip, and choose Summary. The desired screen is the Pokemon summary screen implemented mostly in `src/pokemon_summary_screen.c`, entered from party menu code in `src/party_menu.c`.

The prior battle-summary debugging work lives in `.agent/battle_summary_bugfix_execplan.md` and `tools/mgba/mgba_lua_spike.py`. The Python spike can drive the route headlessly and can be extended if GUI input cannot be stabilized, but the final evidence must still be a desktop screenshot of GUI mGBA.

Generated outputs belong under `build/`, which is ignored by git. The intended output directory for this work is `build/mgba_gui_evidence/`.

## Plan of Work

First, confirm the repository state and build the current ROM with automation symbols. The build command should use the repository Docker image if the local toolchain is not the known-good path. The ROM to launch is `pokeemerald.gba`.

Second, inspect available GUI and screenshot tools. The GUI path should launch mGBA with the current ROM on the active desktop display, then drive the input sequence either through a local GUI automation tool or through the emulator's script bridge if GUI mGBA accepts the same Lua bridge. If GUI control is unreliable, use the Python spike to reach a savestate immediately before or on the Mudkip summary screen, then load that state into GUI mGBA for the required desktop screenshot.

Third, capture the desktop. The screenshot must show the mGBA window itself, not just a framebuffer PNG exported by mGBA. Store it under `build/mgba_gui_evidence/` and record the path here.

If the game crashes or black-screens before the screenshot can be captured, preserve the best failure evidence, record the exact step, and then investigate the code path in `src/pokemon_summary_screen.c` and `src/party_menu.c` before rerunning.

## Concrete Steps

Run these commands from `/home/bayesartre/dev/pokeemerald-expansion-shared-power` unless another working directory is explicitly named.

Confirm status:

    git status -sb

Build current ROM and symbols:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms

Create the evidence directory:

    mkdir -p build/mgba_gui_evidence

Then launch GUI mGBA on `pokeemerald.gba`, drive to the Mudkip summary screen from the initial battle, and capture the desktop. The exact GUI launch and screenshot commands will be added after inspecting what is installed on this machine.

The GUI tools found are:

    C:\Program Files\mGBA\mGBA.exe
    C:\ProgramData\chocolatey\bin\AutoHotkey.exe

## Validation and Acceptance

Acceptance requires:

The ROM used is the latest local build from this repository, not an older artifact.

The mGBA window is still running when the desktop screenshot is taken.

The game state shown in the screenshot is Mudkip's summary screen reached from the initial Zigzagoon battle. It is acceptable if the screenshot also shows surrounding desktop UI.

The screenshot path is recorded in this plan and in the final response.

If a crash prevents the screenshot, acceptance changes to a clear escalation record: the visible failure screenshot path, the route step that caused the crash, and the next code investigation target.

## Idempotence and Recovery

Build artifacts and screenshots under `build/` can be deleted and regenerated. GUI emulator processes can be closed and relaunched. If a stale mGBA process remains, identify it with `pgrep -af mgba` and terminate only that emulator process for this task.

Do not revert user changes. If source edits become necessary, update `PATCH_NOTES.md` first with a top entry using `(commit pending)`, keep the change narrowly scoped, rebuild, and rerun the evidence path.

## Artifacts and Notes

Planned artifacts:

    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png
    build/mgba_gui_evidence/route_notes.txt

Actual artifacts:

    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/battle_summary_attempt.png
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/battle_summary_reached.ss
    build/mgba_gui_evidence/mudkip_gui_live_route_freeze_desktop/run_result.json
    build/mgba_gui_evidence/mudkip_gui_live_route_freeze_desktop/battle_summary_attempt.png
    build/mgba_gui_evidence/mudkip_gui_live_route_desktop_capture2/run_result.json
    build/mgba_gui_evidence/mudkip_gui_live_route_desktop_capture2/battle_summary_attempt.png

## Interfaces and Dependencies

The important gameplay interfaces are:

`src/party_menu.c`: battle party menu entry and Summary action selection.

`src/pokemon_summary_screen.c`: summary-screen setup, graphics allocation, sprite loading, and return callbacks.

`tools/mgba/mgba_lua_spike.py`: deterministic mGBA automation fallback for reaching the initial battle and summary request.

`tools/mgba/mgba_lua_bridge.lua`: socket bridge used by headless mGBA automation and potentially by GUI mGBA if the installed GUI build supports Lua scripts.

Revision Note (2026-05-01): Initial plan created for the Mudkip-specific GUI evidence request. The plan deliberately separates this regression from the completed Torchic validation plan and records the user's screenshot requirement as the primary acceptance condition.
