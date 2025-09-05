# Phased Implementation Plan

This plan prioritizes de‑risking core mechanics while building reproducibility, testability, ergonomics, and transparency. It aligns with the product map while sequencing changes to preserve vanilla parity by default.

## Principles
- Feature flags: gate mechanics under `include/config/*.h`; default off; unit and scenario tests enable them explicitly.
- Determinism: seed RNG and force AI choices in tests; minimize reliance on variable damage rolls.
- Vertical slices: implement minimal, safe subsets first; expand only with tests.
- Single source of truth: validate trainer/team data before usage.
- CI first: every phase adds tests; `make check` must stay green.

## Phase 0: Repro & Test Bed
- Docker parity: lock dev image tag/digest; documented bind‑mount workflow and scripts.
- CI baseline: workflow runs `make -j$(nproc) NO_MULTIBOOT=1` and `make check`; uploads `pokeemerald.gba`, `.map`, `.sym` as artifacts.
- RNG hooks: expose `test_set_seed(uint32_t)` and a PRNG override available in tests only.
- Smoke tests: boot and execute a scripted battle; assert stable structured log signature.
- Exit: identical `rom.sha1` on CI for NO_MULTIBOOT build; smoke tests pass.

## Phase 1: Shared Power (Vertical Slice)
- Flag: `include/config/shared_power.h` → `CONFIG_SHARED_POWER` compile flag, with debug runtime toggle for dev builds.
- Minimal scope first: propagate a curated subset (e.g., Intimidate‑style side effects; one or two passive auras) with explicit conflict resolution.
- State model: side‑scoped `shared_power_state` with `active_abilities` bitset and provenance.
- Tests: vanilla parity when off; expected behavior when on for Doubles switch‑ins, KO/slot changes, suppression (Gastro Acid/Mummy), Mold Breaker, Trace/Baton Pass edges.
- Exit: all shared power tests pass; off‑path remains bit‑exact to vanilla smoke tests.

## Phase 2: Trainer Data Pipeline
- Validation tests: traverse `src/data/trainers.h` asserting legal species/items/moves, assets present, level caps, and ruleset constraints.
- Fixtures: helpers to quickly declare named trainer teams for scenario tests.
- (Optional later) Codegen: CSV/JSON→C under `tools/` with deterministic ordering; defer until validator is stable.
- Exit: validator covers committed trainers; at least three boss fixtures have scenario tests.

## Phase 3: Modern Mechanics Toggles
- Config surface: `include/config/battle_modern.h` with granular flags like `CONFIG_DYNAMIC_SPEED`, `CONFIG_WEATHER_DURATION_RULES`, `CONFIG_ITEM_BEHAVIOR`.
- One‑flag‑at‑a‑time: add parity tests (off) and behavior tests (on) with minimal deterministic scenarios.
- CI matrix: jobs for key flag combinations to catch cross‑flag regressions.
- Exit: flags ship default‑off; CI green in all matrix cells.

## Phase 4: Deterministic Battle Harness
- Scenario DSL: macros/utilities under `test/battle/` to set field, parties, moves, items, abilities; step turns with forced choices.
- Structured logging: capture events/messages/damage buckets; compare to concise expectations to reduce brittleness.
- Exit: harness used by Phases 1–3; new tests are concise and parallelizable.

## Phase 5: Ergonomics & Dev Experience
- Debug toggles/UI: runtime toggles for flags in dev builds; HUD of active shared abilities.
- Dev scripts: extend `dev_scripts/` to run filtered test suites and extract failing artifacts/diffs.
- Docs: quickstart for tests, flags, and scenarios; document shared power conflict resolution.
- Exit: new contributors can add a battle test in under 10 minutes.

## Phase 6: Content Rollout & Balancing Loop
- Product map alignment: integrate mandatory boss teams in planned order; each boss gets validator pass and 2–3 deterministic scenario tests.
- Difficulty gates: assert level caps, availability windows, and learnset constraints per milestone.
- Exit: first arc of bosses is test‑covered; gate merges on validator + scenario suite.

## Transparency & Safety Nets
- Patch notes: update `PATCH_NOTES.md` per change; summarize flag changes.
- Config docs: each flag documented in `include/config/*` and `docs/` with references to example tests.
- Symbols/map: keep `.map` and `.sym` artifacts; short guide for debugging test failures using them.

## Risk Mitigations
- Shared power ambiguity: start with a small, documented subset; expand only with tests; maintain a “known unsafe” list.
- Nondeterminism: seed RNG; force AI choices; prefer checks on event sequences over exact damage rolls.
- Regression risk: comprehensive parity tests for default‑off mode; CI matrix coverage.
- Data drift: trainer validator blocks merges on missing assets or illegal combos.

