# Find the real Mudkip in-battle summary animation root cause

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md` from the repository root. It continues the 2026-05-01 Mudkip summary-screen investigation, but it does not accept "skip the animation" as a final answer unless the root cause proves that animation is intentionally unsupported in this context. The goal is to understand why Mudkip's summary can black-screen or reset when opened from the initial Zigzagoon battle and to produce the smallest defensible fix.

## Purpose / Big Picture

The player should be able to choose Mudkip, enter the initial Zigzagoon battle, open Pokemon, select Mudkip, choose Summary, and see Mudkip's normal Pokemon summary screen without a black screen, reset, or crash. An earlier working tree could produce a passing desktop screenshot by suppressing in-battle summary animation, but that was only a stabilizing workaround. This plan will determine whether the real issue is summary sprite ownership, affine animation state, OAM matrix state, task lifetime, or a branch-local regression. At the end, the repository should either keep animation enabled with a validated fix or have a precise, evidence-backed reason for any remaining animation guard.

## Progress

- [x] (2026-05-01 04:17-04:00) Read `.agent/PLANS.md` and confirmed the required living ExecPlan workflow.
- [x] (2026-05-01 04:17-04:00) Recorded current working-tree context: branch `main`, dirty files `PATCH_NOTES.md`, `src/pokemon_summary_screen.c`, `tools/mgba/mgba_lua_spike.py`, and prior GUI evidence ExecPlan.
- [x] (2026-05-01 04:17-04:00) Identified `84f5aa6dae62da212efa2e8de11585bad189a031` as the merge base between current `HEAD` and `origin/master`; `origin/main` currently equals `HEAD`.
- [x] (2026-05-01 04:22-04:00) Removed the in-battle summary animation suppression in the main worktree by resetting the summary sprite's animation gate field to `0`.
- [x] (2026-05-01 04:22-04:00) Added guarded animation-path instrumentation around `SpriteCB_Pokemon`, `PokemonSummaryDoMonAnimation`, `StartMonSummaryAnimation`, `HandleStartAffineAnim`, and `Anim_CircularStretchTwice`, plus Python decoding for the new animation-state beacon.
- [x] (2026-05-01 04:23-04:00) Built the active worktree with `AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1`; the ROM and symbols were generated successfully.
- [x] (2026-05-01 04:28-04:00) Ran the Mudkip GUI route with animation enabled in the active worktree. The route selected Mudkip and requested Summary, then reset to the title sequence before the summary main callback beacon or any animation-path beacon was observed.
- [x] (2026-05-01 04:38-04:00) Ran the `origin/master` ROM through the current Mudkip route; it reached Mudkip's summary with animation enabled.
- [x] (2026-05-01 04:45-04:00) Ran the `caee916773` ROM through the current Mudkip route; it selected Mudkip and hung during summary graphics state `19`, where the caught-ball sprite is created.
- [x] (2026-05-01 04:46-04:00) Determined that the initial fork-point fallback is unnecessary because `origin/master` passes the Mudkip summary route.
- [x] (2026-05-01 04:58-04:00) Reduced Shared Power's per-trainer ability-order capacity from `ABILITIES_COUNT` to `PARTY_SIZE` and re-tested the current build with animation enabled; Mudkip's in-battle summary reached successfully.
- [x] (2026-05-01 05:02-04:00) Removed the temporary animation-path automation beacons after the memory reduction produced a passing route.
- [x] (2026-05-01 05:07-04:00) Rebuilt after removing temporary animation instrumentation and captured a clean passing live GUI Mudkip summary route.
- [x] (2026-05-01 16:15-04:00) Re-ran the latest build through the live WSLg SDL mGBA route using semantic Lua beacons, captured the full desktop at `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`, and confirmed Mudkip's summary with animation enabled.

## Surprises & Discoveries

- Observation: The prior live GUI route proved that suppressing in-battle summary animation can make Mudkip's summary visible, but this does not prove animation is the root cause.
  Evidence: `build/mgba_gui_evidence/mudkip_gui_live_route_freeze_desktop/run_result.json` records `summaryScreenReached=true` and `bugObserved=false`, while the code path did not call `PokemonSummaryDoMonAnimation` for the summary sprite in battle.

- Observation: Mudkip's summary animation uses affine sprite animation.
  Evidence: `src/data/pokemon/species_info/gen_3_families.h` gives `SPECIES_MUDKIP` `.frontAnimId = ANIM_CIRCULAR_STRETCH_TWICE`; `src/pokemon_animation.c::Anim_CircularStretchTwice` calls `HandleStartAffineAnim` and `HandleSetAffineData`.

- Observation: The current branch point against `origin/master` is `84f5aa6dae62da212efa2e8de11585bad189a031`.
  Evidence: `git merge-base HEAD origin/master` returned that hash on 2026-05-01.

- Observation: The automation beacon protocol is limited to values `0..14`, so substage `14` is now reused as a compact animation-state channel during the investigation.
  Evidence: `include/automation_beacon.h` defines `AUTOMATION_BEACON_VALUE_MAX 14` and `AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_ANIMATION_STATE`.

- Observation: With animation re-enabled, the current branch reset before the first observed summary main-loop or animation beacon.
  Evidence: `build/mgba_gui_evidence/mudkip_anim_root_cause_current/run_result.json` has `ok=false`, `routeOk=true`, `bugObserved=true`; `summaryBeaconHistory` reaches `LOAD_GRAPHICS_STATE` with `summaryLoadGraphicsState=25`, then falls back to stage `0`. The desktop screenshot `build/mgba_gui_evidence/mudkip_anim_root_cause_current_desktop.png` shows mGBA back at the Pokemon title screen.

- Observation: `origin/master` is not affected by the Mudkip in-battle summary bug.
  Evidence: `build/mgba_gui_evidence/mudkip_origin_master_current_runner/run_result.json` has `ok=true`, `summaryScreenReached=true`, `bugObserved=false`, and the screenshot shows Mudkip's summary.

- Observation: The earlier `caee916773` summary-manager fix can select Mudkip but stalls at `LoadGraphics` state `19`, the `CreateCaughtBallSprite` step.
  Evidence: `build/mgba_gui_evidence/mudkip_caee_current_runner/run_result.json` has `summaryLoadGraphicsState=19`, `summaryScreenReached=false`, and the screenshot is a mostly blank lavender screen.

- Observation: Shared Power stores per-trainer ability order arrays at `ABILITIES_COUNT` capacity even though a trainer can contribute at most `PARTY_SIZE` sources.
  Evidence: `include/battle.h` stores `sharedPowerPoolOrder`, `sharedPowerPoolAllOrder`, and `sharedPowerSwitchInAbilities` using `SHARED_POWER_POOL_MAX`; before this fix `include/battle_shared_power.h` set that max to `ABILITIES_COUNT`.

- Observation: Reducing `SHARED_POWER_POOL_MAX` to `PARTY_SIZE` makes the current Mudkip route pass with normal summary animation enabled.
  Evidence: `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_current/run_result.json` reports `ok=true`, `summaryScreenReached=true`, `summaryValidationOk=true`, and `bugObserved=false`; the desktop screenshot `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_current_desktop.png` shows Mudkip's summary in mGBA.

- Observation: The final accepted GUI proof uses semantic beacons, not timing replay.
  Evidence: `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json` records `ok=true`, `summaryScreenReached=true`, `summaryValidationOk=true`, `bugObserved=false`, and a `desktopScreenshot.ok=true` artifact at `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`.

- Observation: The timing-based replay thread is intentionally discarded.
  Evidence: A replay attempt drifted into a wrong visual state including the male player sprite; it is not referenced as proof in the outcome, and the accepted route keeps control through route beacons from clean boot to summary screen.

## Decision Log

- Decision: Treat animation suppression as a temporary workaround, not a completed root-cause fix.
  Rationale: Upstream summary screens normally animate Pokemon portraits, and the user's question correctly points out that disabling animation is not a sufficient explanation.
  Date/Author: 2026-05-01 / Codex.

- Decision: Use scratch worktrees under `build/worktrees/` for baseline comparisons.
  Rationale: The main worktree is dirty with current investigation changes and user-visible evidence files; scratch worktrees let us compare `origin/master` and the fork point without destructive checkout or reverting unrelated work.
  Date/Author: 2026-05-01 / Codex.

- Decision: Compare `origin/master` first, then the fork point only if `origin/master` has Mudkip issues.
  Rationale: The user explicitly requested this order, and `origin/master` is the closest upstream baseline likely to contain compatible current summary/animation code.
  Date/Author: 2026-05-01 / Codex.

- Decision: Keep summary animation enabled and fix the Shared Power battle-state footprint instead of retaining the animation-skip workaround.
  Rationale: `origin/master` passed with animation, the current branch passed after the Shared Power buffer shrink, and the earlier failures occurred before or around summary resource creation rather than inside Mudkip's affine animation callback.
  Date/Author: 2026-05-01 / Codex.

## Outcomes & Retrospective

The root cause is branch-local Shared Power battle-state memory pressure, not Mudkip's summary animation itself. `origin/master` reaches Mudkip's in-battle summary with animation enabled, while `caee916773` stalls at summary graphics state `19` (`CreateCaughtBallSprite`) and the current pre-fix branch resets after summary graphics state `25`. Shrinking Shared Power's per-trainer ability-order buffers from `ABILITIES_COUNT` to `PARTY_SIZE` restores enough battle-time resource headroom for the summary screen and Mudkip's normal affine portrait animation to run.

The final code keeps in-battle summary animation enabled. Temporary animation-path beacons were removed after the passing memory-fix run; the final validation was performed on a clean rebuilt ROM through a live WSLg SDL mGBA process. The accepted desktop screenshot is `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png`, and the matching route result is `build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json`.

## Context and Orientation

This repository builds a Game Boy Advance ROM named `pokeemerald.gba`. The relevant player flow starts at a new game, chooses Mudkip from Professor Birch's bag, enters the initial Zigzagoon battle, opens the battle action menu, selects Pokemon, selects Mudkip, and chooses Summary. The summary screen implementation is in `src/pokemon_summary_screen.c`; the general Pokemon animation code is in `src/pokemon_animation.c`; species animation data for Mudkip is in `src/data/pokemon/species_info/gen_3_families.h`.

An OAM matrix is a Game Boy Advance hardware sprite transformation slot used for affine effects such as scaling and rotation. In this codebase, `src/sprite.c::AllocOamMatrix`, `src/sprite.c::FreeOamMatrix`, and `src/pokemon_animation.c::HandleStartAffineAnim` manage those slots. Mudkip's front animation uses `ANIM_CIRCULAR_STRETCH_TWICE`, which enters this affine path.

The current automation route is in `tools/mgba/mgba_lua_spike.py`. It can run a live SDL mGBA process with Lua scripting and can capture both an emulator framebuffer PNG and a full desktop PNG through Windows PowerShell. The final user-facing evidence should be a desktop screenshot of a running mGBA window, not only a framebuffer PNG.

Build artifacts and screenshots go under `build/`, which is ignored by git. Scratch comparison worktrees also belong under `build/worktrees/`.

## Plan of Work

First, remove the temporary in-battle animation suppression from `src/pokemon_summary_screen.c` so the failing path is live again. Keep the battle-owned opponent sprite slot, because prior evidence showed the static summary-owned buffer can render black even when beacons report success.

Second, add narrow instrumentation behind `AUTOMATION_BEACON` around the animation path. The instrumentation should record enough state to distinguish bad sprite template data, bad affine matrix allocation, wrong `sDontFlip`/`data[1]`, stale `data[2]`, task creation failure, and reset before callback completion. Prefer adding small beacon substages and compact fields rather than general logging.

Third, rebuild with `AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1`, run the Mudkip GUI route using `tools/mgba/mgba_lua_spike.py`, and inspect both `run_result.json` and the captured PNGs.

Fourth, compare against `origin/master` in `build/worktrees/mudkip-summary-origin-master`. Apply only the minimum automation files and guarded beacon hooks needed to run the same Mudkip route. If that build and route pass with animation enabled, the bug is branch-local after `84f5aa6dae`. If it fails, repeat from the fork point worktree `build/worktrees/mudkip-summary-fork-point`.

Fifth, promote the smallest fix supported by evidence. If animation can be safely enabled, remove the in-battle suppression and keep the fixed animation path. If the evidence proves a specific battle-owned resource cannot be animated during summary, keep the narrowest guard and document exactly why it is required.

## Concrete Steps

All commands run from `/home/bayesartre/dev/pokeemerald-expansion-shared-power` unless a scratch worktree path is explicitly named.

Build the active worktree:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms

Run the live GUI Mudkip route:

    rm -rf build/mgba_gui_evidence/mudkip_anim_root_cause_current
    nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode battle-summary --force-script --mgba build/mgba-sdl-build/sdl/mgba --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_gui_evidence/mudkip_anim_root_cause_current --port 46730 --connect-timeout 20 --truck-timeout 300 --starter-timeout 600 --battle-timeout 300 --summary-timeout 60 --desktop-screenshot build/mgba_gui_evidence/mudkip_anim_root_cause_current_desktop.png --desktop-screenshot-delay 2'

Create the `origin/master` worktree:

    mkdir -p build/worktrees
    git worktree add --detach build/worktrees/mudkip-summary-origin-master origin/master

If needed, create the fork-point worktree:

    git worktree add --detach build/worktrees/mudkip-summary-fork-point 84f5aa6dae62da212efa2e8de11585bad189a031

## Validation and Acceptance

Acceptance requires an evidence-backed answer to why disabling animation appeared necessary. A successful final state must include a rebuilt ROM and a live GUI mGBA route that reaches Mudkip's summary from the initial Zigzagoon battle without black screen or reset. The final response must include the key result JSON path and the desktop screenshot path.

If the final fix keeps animation enabled, the evidence should show `summaryScreenReached=true`, `bugObserved=false`, and a desktop screenshot with the summary screen. If the final fix keeps any animation guard, the evidence must identify the exact unsafe animation subpath and why a broader animation-enabled fix is not correct.

## Idempotence and Recovery

All generated evidence under `build/mgba_gui_evidence/` can be deleted and recreated. Worktrees under `build/worktrees/` can be removed with `git worktree remove --force <path>` if they become stale. Do not use destructive checkout or reset commands in the main worktree. Do not kill unrelated user mGBA processes; only terminate task-launched emulator processes if they remain after a failed automation run.

## Artifacts and Notes

Expected current-run artifacts:

    build/mgba_gui_evidence/mudkip_anim_root_cause_current/run_result.json
    build/mgba_gui_evidence/mudkip_anim_root_cause_current/battle_summary_attempt.png
    build/mgba_gui_evidence/mudkip_anim_root_cause_current_desktop.png

Current passing memory-fix artifacts:

    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_current/run_result.json
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_current/battle_summary_attempt.png
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_current_desktop.png

Final accepted beacon GUI artifacts:

    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/run_result.json
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/battle_summary_attempt.png
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui/battle_summary_reached.ss
    build/mgba_gui_evidence/mudkip_shared_power_pool_shrink_beacon_gui_desktop.png

## Interfaces and Dependencies

The core interfaces under investigation are:

`src/pokemon_summary_screen.c::LoadMonGfxAndSprite`: loads the Pokemon portrait graphics and sets `gMultiuseSpriteTemplate`.

`src/pokemon_summary_screen.c::CreateMonSprite`: creates the summary-screen Pokemon sprite and initializes sprite `data` fields.

`src/pokemon_summary_screen.c::SpriteCB_Pokemon`: calls `PokemonSummaryDoMonAnimation` unless its gate field `data[2]` is set to `1`.

`src/pokemon.c::PokemonSummaryDoMonAnimation`: starts species front animation for summary sprites and creates a delayed animation task when needed.

`src/pokemon_animation.c::StartMonSummaryAnimation` and affine helpers: install the species animation callback and manipulate OAM affine matrix state.

`tools/mgba/mgba_lua_spike.py`: drives the route and captures evidence.

Revision Note (2026-05-01): Initial root-cause plan created after the user challenged the animation-suppression workaround. The plan makes baseline comparison and animation-path instrumentation mandatory before accepting any final fix.
