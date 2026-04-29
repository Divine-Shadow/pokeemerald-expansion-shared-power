# Patch Notes

- Tools/Automation: Add starter-confirm mode with semantic menuReady acceptance (commit pending).
- Docs/Automation: Record completed v2 semantic beacon proofs and Scala timeout blocker (commit pending).
- Tools/Automation: Add backward-compatible v2 semantic beacon rows and use movementReady in the headless route (commit pending).
- Docs/Automation: Add incremental beacon v2 ExecPlan and Scala/devkit validation milestones (commit pending).
- Docs/Automation: Record default-build validation and final automation ROM rebuild evidence (commit pending).
- Docs/Automation: Record the completed headless starter-selection proof and migration recommendation (commit pending).
- Tools/Automation: Treat Route 101 script waits as beacon input-ready and clear keys after route entry (commit pending).
- Tools/Automation: Exit the player house by pressing down from the door warp tile instead of stepping laterally across it (commit pending).
- Tools/Automation: Let Route 101 entry acknowledge the Littleroot trigger dialogue while continuing toward the target stage (commit pending).
- Tools/Automation: Replace fixed rival-house counts with map-slot and coordinate-gated headless route waypoints (commit pending).
- Tools/Automation: Encode high coordinate nibbles in the beacon map row so Route 101 positions remain distinguishable (commit pending).
- Tools/Automation: Add a beacon map-slot row so the headless route can distinguish early interiors from town and Route 101 (commit pending).
- Tools/Automation: Count only actual movement keypresses while waiting through TV-report stair transitions (commit pending).
- Tools/Automation: Stop passive waiting after entering the TV report script so the route can acknowledge it (commit pending).
- Tools/Automation: Press into the bedroom stair warp before advancing TV report dialogue (commit pending).
- Tools/Automation: Navigate from the post-clock bedroom position into the stair warp using beacon coordinates (commit pending).
- Tools/Automation: Correct nav-based clock coordinates and direction constants for the headless route (commit pending).
- Tools/Automation: Add beacon navigation proof fields and use them to face the bedroom clock deterministically (commit pending).
- Tools/Automation: Keep the player facing May's bedroom clock before interacting in the headless starter route (commit pending).
- Tools/Automation: Hold emulator movement taps long enough for headless overworld tile transitions (commit pending).
- Tools/Automation: Add a headless Python starter-selection route mode with beacon-gated movement helpers (commit pending).
- Docs/Automation: Document the script-derived truck-to-starter route and defer new beacon fields for the first headless attempt (commit pending).
- Docs/Automation: Record fresh headless truck baseline evidence for the starter-selection plan (commit pending).
- Docs/Automation: Add a headless starter-selection ExecPlan with explicit Python continuation and migration gates (commit pending).
- Docs/Automation: Record the passing repeated headless Python/Lua female `A` truck checkpoint proof (commit pending).
- Tools/Automation: Avoid pressing through Birch's boy/girl transition so the route cannot auto-select male (commit pending).
- Tools/Automation: Wait for naming input to become ready after auto-lowercase before confirming the route OK button (commit pending).
- Tools/Automation: Retry input-ready Python route confirmations until the expected beacon transition appears (commit pending).
- Tools/Automation: Extend the Python Lua-spike runner with a host-owned female `A` truck route FSM (commit pending).
- Tools/Automation: Add a pinned mGBA headless build helper and record the passing no-focus Lua bridge smoke test (commit pending).
- Tools/Automation: Make the Lua bridge wait for frames asynchronously and write the beacon fallback with valid OBJ VRAM halfword stores (commit pending).
- Tools/Automation: Add Python and Lua mGBA automation spike with explicit capability reporting (commit pending).
- Docs/Automation: Add retrospective on beacon automation learnings and next architecture ideas (commit pending).
- Tools/Automation: Treat wait-button prompts as input-ready during the truck-exit Littleroot intro as well (commit pending).
- Tools/Automation: Add wait-button script-step proof and readiness for message prompts that require a fresh A press (commit pending).
- Tools/Automation: Press through the bedroom clock intro message before waiting for the wall-clock UI beacon (commit pending).
- Tools/Automation: Approach May's bedroom clock with two Right presses and an Up-facing turn before interacting (commit pending).
- Tools/Automation: Gate the upstairs transition on Littleroot substage 5 instead of a fixed Up count (commit pending).
- Tools/Automation: Use four gated Up presses for the female house stairs before waiting for bedroom setup (commit pending).
- Tools/Automation: Reject non-v1 beacon-shaped false positives in the PowerShell reader (commit pending).
- Tools/Automation: Broaden Littleroot script-step proof and wait-message readiness to in-house setup scripts (commit pending).
- Tools/Automation: Keep Littleroot intro input-ready while the script is waiting for message acknowledgment (commit pending).
- Tools/Automation: Carry the last script command class in Littleroot beacon proof for cutscene wait diagnostics (commit pending).
- Tools/Automation: Add Littleroot Mom object diagnostics to the beacon error field for truck-exit script stalls (commit pending).
- Tools/Automation: Use a bounded beacon-gated Right loop for truck exit and log diagnostic flags/error codes (commit pending).
- Tools/Automation: Treat held player movement as diagnostic-only after truck idle proved it can remain set while ready (commit pending).
- Tools/Automation: Require a settled player object before overworld movement-ready beacon states (commit pending).
- Tools/Automation: Gate Littleroot truck-exit intro input on visible field messages only to avoid premature A presses (commit pending).
- Tools/Automation: Add overworld beacon diagnostics for script, lock, message, fade, and player movement blockers (commit pending).
- Docs/Automation: Record speed-mode starter attempt and current Littleroot moving-in blocker tuple (commit pending).
- Tools/Automation: Exit the truck with exactly two gated Right presses instead of an unbounded move loop (commit pending).
- Tools/Automation: Treat visible field message boxes as overworld input-ready even while scripts own the context (commit pending).
- Docs/Automation: Record live normal-speed truck proof, AHK v2 decision, local ROM staging, and remaining starter/speed acceptance (commit pending).
- Tools/Automation: Tighten live beacon scan bounds around the fixed 2x beacon position to reduce route runtime (commit pending).
- Tools/Automation: Remove the temporary backdrop diagnostic after live beacon rendering was verified (commit pending).
- Tools/Automation: Gate female confirmation on durable `gender=2` proof instead of a transient stage id (commit pending).
- Tools/Automation: Raise the boot beacon read budget to allow the live PowerShell sampler to finish decoding (commit pending).
- Tools/Automation: Force a small 2x mGBA window size instead of preserving a restored oversized window (commit pending).
- Tools/Automation: Prefer fast integer-scale beacon scans and pass a bounded scan window from the mGBA runner (commit pending).
- Tools/Automation: Launch mGBA at 2x scale so beacon pixels can be sampled as integer-scaled cells (commit pending).
- Tools/Automation: Decode non-integer scaled mGBA beacons with a run-length sampler and bounded top-left scan region (commit pending).
- Tools/Automation: Pin the beacon sprite to OBJ palette slot 15 so renderer palette refreshes match the created sprite (commit pending).
- Tools/Automation: Keep beacon sprite tracking zero-initialized so automation builds do not emit discarded `.data` (commit pending).
- Tools/Automation: Create and update the debug beacon through the standard sprite pipeline so live screens load its OAM and tile data (commit pending).
- Tools/Automation: Force OBJ display directly while rendering the automation beacon so queued DISPCNT writes cannot hide it (commit pending).
- Tools/Automation: Stage the rebuilt ROM to local Windows temp before launching mGBA to avoid UNC ROM fallback behavior (commit pending).
- Tools/Automation: Add a late VBlank beacon refresh after sprite/DMA processing to survive screen callback tile overwrites (commit pending).
- Tools/Automation: Render the debug beacon through OAM and palette buffers before screen VBlank callbacks for live visibility (commit pending).
- Tools/Automation: Keep the AutoHotkey v2 mGBA window small, topmost, and active before beacon captures and key sends (commit pending).
- Tools/Automation: Ignore raced WScript termination errors during bounded no-beacon boot scans (commit pending).
- Tools/Automation: Bound no-beacon boot scans so the AHK v2 runner can keep pressing through title screens (commit pending).
- Tools/Automation: Port beacon-gated mGBA repro runner to AutoHotkey v2 and add validate mode (commit pending).
- Tools/Automation: Inline disabled beacon no-ops so default builds avoid hot-path empty calls (commit pending).
- Tools/Automation: Short-circuit beacon scans on anchor pixels before decoding full proof rows (commit pending).
- Tools/Automation: Keep beacon rendering from forcing OBJ 1D mapping while preserving debug overlay output (commit pending).
- Docs/Automation: Record beacon implementation evidence, sampler validation, and remaining live mGBA acceptance work (commit pending).
- Tools/Automation: Add debug automation beacon, beacon sampler, and state-gated mGBA runner for female `A` truck/starter repros (commit pending).
- Docs/Automation: Add ExecPlan for beacon-gated mGBA reproduction from clean boot through female `A` truck proof and first Pokemon selection (commit pending).
- Docs/Automation: Add design doc for deterministic mGBA reproduction pipeline and beacon-gated stage automation strategy; add docs summary link (commit pending).
- Docs/Process: Add repository-adapted `.agent/PLANS.md` with milestone-continuity policy (exec plans continue until success or escalation) (commit pending).
- Tools/AHK: Tune intro flow near gender/name by lowering pre-gender advance, switching gender nudge to `Right`, trimming pre-name presses, and extending post-name truck advance window (commit pending).
- Tools/AHK: Increase bounded dialogue-advance limits to carry the deterministic flow past Birch scenes into gender/name/truck phases without reverting to endless key spam (commit pending).
- Tools/AHK: Revert mGBA key dispatch to foreground `Send` under full-run input block after `ControlSend` failed to advance title/menu state (commit pending).
- Tools/AHK: Retune intro step timings and key cadence to avoid premature title input and reduce over-advance spam during gender/name/truck progression (commit pending).
- Tools/AHK: Capture mGBA client area via `GetClientRect` + `CopyFromScreen` to remove noisy title-bar/frame pixels from checkpoint matching (commit pending).
- Tools/AHK: Make checkpoint compare fail-closed by requiring parsed numeric diff scores and rejecting missing/unparsable references (commit pending).
- Tools/AHK: Harden mGBA intro input isolation with full-run `BlockInput` and PID-targeted `ControlSend` fallback delivery (commit pending).
- Tools/AHK: Switch intro automation to `SendMode Event` and disable per-key input blocking to improve mGBA key consumption reliability (commit 5c8400f03d).
- Tools/AHK: Fix PowerShell capture helper PID argument collision (`$PID`) by renaming to `-TargetPid` and updating caller (commit 5c8400f03d).
- Tools/AHK: Fix checkpoint capture on UNC workspaces by staging captures/comparisons through local temp files before copying into repo paths (commit 5c8400f03d).
- Tools/AHK: Fix AHK v1 command-context interpolation bug for object fields in checkpoint error dialog (`step.id`) (commit 5c8400f03d).
- Tools/AHK: Add working-directory path fallback for ROM/helper resolution so local temp script copies can still target repo artifacts on UNC setups (commit 5c8400f03d).
- Tools/AHK: Fix deterministic intro script startup on AutoHotkey v1 by removing invalid top-level `global` declarations (commit 5c8400f03d).
- Tools/AHK: Rebuild mGBA intro automation into deterministic `map`/`run` workflow with per-step captures, checkpoint image gating, and helper PowerShell capture/compare utilities (commit 5c8400f03d).
- Tools/AHK: Add gender-confirmation fallback to force "No", return to selector, and re-pick GIRL before naming flow (commit 5c8400f03d).
- Tools/AHK: Correct gender-selection input from `Right` to `Down` for Emerald’s BOY/GIRL menu navigation (commit 5c8400f03d).
- Tools/AHK: Strengthen intro automation to bias and force GIRL selection before name/truck progression (commit 5c8400f03d).
- Tools/AHK: Isolate mGBA macro input via targeted window `PostMessage` key events (no global keystroke injection) (commit 5c8400f03d).
- Tools/AHK: Remove held fast-forward key from intro macro to avoid dropped gameplay input during automation (commit 5c8400f03d).
- Tools/AHK: Switch intro macro input back to foreground `Send` after `ControlSend` proved ineffective on mGBA gameplay input (commit 5c8400f03d).
- Tools/AHK: Remove stray invalid line from mGBA intro macro that caused AutoHotkey parse failure at runtime (commit 5c8400f03d).
- Tools/AHK: Align mGBA intro macro with local keymap (`A=X`, `B=Z`) so confirmation inputs register correctly (commit 5c8400f03d).
- Tools/AHK: Route mGBA macro inputs via PID-targeted ControlSend for focus-independent key delivery (commit 5c8400f03d).
- Tools/AHK: Add execution logging to mGBA intro macro (`C:\Temp\mgba_macro_log.txt`) for startup diagnostics (commit 5c8400f03d).
- Tools/AHK: Use PID-based mGBA window targeting in intro macro for reliable activation on startup (commit 5c8400f03d).
- Tools/AHK: Enable mGBA fast-forward hold during intro macro to fit automated execution windows (commit 5c8400f03d).
- Tools/AHK: Port mGBA intro macro to AutoHotkey v1 syntax for local execution compatibility (commit 5c8400f03d).
- Tools/AHK: Add mGBA macro to start a new game as girl named A and reach truck verification flow (commit 5c8400f03d).
- Summary/Battle: Guard summary sprite-manager ownership and null sprite buffers to prevent in-battle summary black-screen hangs (commit 5c8400f03d).
- Summary/Battle: Switch summary isolation to manager A to avoid in-battle black-screen regressions from manager B usage (commit 5c8400f03d).
- Summary/Battle: Isolate in-battle summary sprite loads into a dedicated manager to avoid shared battle gfx buffer corruption (commit 5c8400f03d).
- Tests/Battle: Validate wild Shared Power pooled Intimidate via resulting stat stage instead of message text (commit 5c8400f03d).
- Battle: Force Shared Power flag globally for all battles for testing (commit 7d03446de7).
- Battle: Force Shared Power flag on all trainer battles for testing (commit 7d03446de7).
- Shared Power/AI: Pool move blocking, absorption, and priority checks (commit 489b89fad2).
- Tests/Battle: Add Shared Power off-path smoke checks (commit d1571d10f8).
- Shared Power/AI: Teach AI to respect pooled status immunities (commit ab29e887ff).
- Shared Power: Add pooled end-turn handling and tests (commit a1263835b2).
- Tests/Battle: Add pooled on-hit Shared Power coverage (commit dadd2e433c)
- Tests/Battle: Add Shared Power Phase 4 coverage (commit f74c176eeb)
- Shared Power: Implement pooled move-end ability iteration (commit 8af5c80cf4)
- Shared Power: Refine switch-in helpers and ability iteration APIs (commit 809b3a3973)
- Shared Power: Dedup switch-in abilities per event (commit 3fa00841ff)
- Shared Power/Tools: Fix switch-in override restoration and test container dependency (commit 1d9dcccb65)
- Battle/Docs: Refine Shared Power switch-in scaffolding (commit 2601803e58)
- Battle: Add Shared Power switch-in pooling scaffolding (commit f2c8aafb1e)
- Battle: Add Shared Power native-ability helper + Phase 2 stubs notes (commit f2e1701764)
- Battle/Tests: Add Shared Power effective-ability query layer (commit 4e7b5d7bff)
- Docs: Clarify Phase 0 validation expectations (commit 9c81ec0662)
- Tools/Docs: Add Shared Power docker test wrapper (commit 1e68b16569)
- Docs/Tests: Add logging notes for test runs (commit 66c0135b85)
- Tests/Tools: Add log helper and battle-type DSL setter (commit c73b264fe2)
- Tests/Docs: Align Shared Power tests with current DSL and plan (commit 6b2eb8b405)
- Tests: Fix Shared Power gate test expectations (commit c179ab2730)
- Battle: Add Shared Power scaffolding and gate (commit ca7d6dd2e4)
- Docs: Add Shared Power implementation plan with incremental validation (commit 9ad232e3f6)
- Docs: Add Shared Power landing map for Trait-System extraction (commit 0352e6b2ed)
- Docs: Add Trait-System extraction map for Shared Power (commit 0352e6b2ed)

This file tracks changes merged into `main` that are not yet included in a versioned release. Whenever you make a change, add a concise entry at the top with a brief description and the commit hash.

Format: `- Area: short description (commit <short-hash>)`

## 2025-09-06

- Docs: Add documentation guidance links in AGENTS files (commit TBD).
- Docs: Align tools AGENTS style rules with STYLEGUIDE (commit TBD).
- Docs: Align AGENTS.md style with STYLEGUIDE; add PATCH_NOTES policy to CONTRIBUTING (commit b1aee6f613).

## 2025-09-05

- Planning: Add phased implementation plan focusing on de-risked core features and testability (commit fdfb4892e6).
- Tooling: Add Docker dev setup; ignore build_artifacts; add multiboot stubs and design outline (commit db001f6794).

## 2025-09-03


- Docs: rename UNRELEASED.md to PATCH_NOTES.md (commit TBD)
- Docs: document test directory guidelines (commit TBD)
- Docs: document audio asset formats and build steps (commit 7bb9f2c6).
- Docs: add data directory guidelines (commit c9bb6915).
- Docs: document test directory guidelines (commit TBD)
- Docs: add tools contribution guidelines (commit c7bd2021ac).


## 2025-09-02

- Docs: add graphics asset guidelines (commit 39bfd2d2).
- Gameplay: Strength is now a Rock-type move (commit c78da96b3a).
- Battle: Route shared power damage modifiers through pooled abilities (commit f8e5436834).
- Battle: Refactor shared power damage helpers and switch-in ordering (commit 4801c865fa).
- Battle: Add shared power stat reaction coverage (commit cd7960fe18).
- Tests: Add shared power flinch/stat interaction coverage (commit 7a5851a102).
- Battle: Centralize stat-drop reactions and guard shared power pool seeding (commit f5c8ce0775).
- Battle: Refactor Intimidate prevention handling (commit 452c9721ef).
