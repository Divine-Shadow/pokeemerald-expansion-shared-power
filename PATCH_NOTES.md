# Patch Notes

- Docs/Automation: Add ExecPlan for beacon-gated mGBA reproduction from clean boot through female `A` truck proof and first Pokemon selection (commit pending).
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
