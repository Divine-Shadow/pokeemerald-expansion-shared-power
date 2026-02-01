# Shared Power Implementation Plan (Incremental Validation)

This plan turns the Shared Power design into phased, testable steps. Each phase is meant to ship safely with Shared Power disabled by default and validate behavior with focused tests.

## Related Docs

- Design overview: `docs/design/shared_power/README.md`
- Scope: `docs/design/shared_power/scope.md`
- State model: `docs/design/shared_power/state.md`
- Core API: `docs/design/shared_power/api.md`
- Switch-in flow: `docs/design/shared_power/acquisition_switchin.md`
- Events/dispatchers: `docs/design/shared_power/events.md`
- Damage calc: `docs/design/shared_power/damage_calc.md`
- Suppression/edge cases: `docs/design/shared_power/suppression_edge_cases.md`
- AI: `docs/design/shared_power/ai.md`
- Config/tests: `docs/design/shared_power/config_tests.md`
- Implementation map: `docs/design/shared_power/implementation_map.md`
- Trait-System extraction map: `docs/design/shared_power/trait_system_extraction_map.md`

## Phase 0 — Scaffolding and gating

Goal: establish compile/runtime gates with no behavioral changes when off.

Implementation
- Add `include/battle_shared_power.h` and `src/battle_shared_power.c` with `SharedPower_IsEnabled()` stub.
- Add config flag (e.g., `CONFIG_SHARED_POWER`) and battle-type flag (e.g., `BATTLE_TYPE_SHARED_POWER`).
- Ensure Shared Power codepaths are compiled but gated.
- Test scaffolding: keep `test/battle/shared_power.c` compiling by adding result fields for `captureDamage`, using valid DSL `Ability(...)` calls, and providing a temporary test-only flag alias while battle-type flags are wired into the DSL.

Suggested tests
- Unit test: Shared Power is off by default.
- Battle test: flag on/off toggles `SharedPower_IsEnabled()` without affecting vanilla flow when off.

Validation
- `make check` passes with Shared Power off.

## Phase 1 — Effective ability query layer

Goal: centralize “effective ability” checks (native + pool) with suppression/eligibility hooks.

Implementation
- Implement `HasActiveAbility` and `ForEachEffectiveAbility` in `src/battle_shared_power.c`.
- Add `SharedPower_IsEligibleFor` and suppression helpers per `docs/design/shared_power/api.md`.
- Update `IsAbilityOnSide`, `IsAbilityOnField`, `IsAbilityOnFieldExcept` to use `HasActiveAbility`.

Suggested tests
- Field/side queries reflect pooled abilities when mode on.
- Suppression (Gastro Acid, Neutralizing Gas, Ability Shield, Mold Breaker) hides pooled abilities.
- Vanilla behavior unchanged when mode off.

Validation
- Shared Power tests pass with flag on.
- Existing ability tests pass with flag off.

## Phase 2 — Switch-in acquisition + popup ordering

Goal: add pool acquisition on switch-in and deterministic switch-in activation order.

Implementation
- Add pool tracking to `struct BattleStruct` per `docs/design/shared_power/state.md`.
- Add pending switch-in queue per battler; enqueue pooled abilities on switch-in.
- Add per-ability latch for switch-in to avoid double triggers.
- Add/extend ability popup stack to preserve ability order.

Suggested tests
- Intimidate/Drizzle are shared and trigger in order after switch-in.
- Duplicate abilities trigger once.
- Popups show pooled abilities in deterministic order.

Validation
- Tests for switch-in behavior in singles and doubles.

## Phase 3 — Damage and continuous modifiers

Goal: make passive and damage modifiers read the effective ability set.

Implementation
- Route damage calc and continuous modifiers through `ForEachEffectiveAbility`.
- Update consolidated helpers in `src/battle_util.c`.

Suggested tests
- Adaptability/Technician/Tough Claws apply when pooled.
- Duplicates don’t stack beyond existing rules.

Validation
- Damage ratios match expectations with pooled abilities on.

## Phase 4 — Move-end / on-hit / on-faint effects

Goal: pooled abilities trigger for on-hit and on-faint effects.

Implementation
- Replace raw ability checks with `HasActiveAbility` in move-end/on-hit sections.
- Ensure `gLastUsedAbility` + popup stack are set correctly for scripts.

Suggested tests
- Rough Skin/Iron Barbs/Aftermath/Innards Out trigger from pool.
- Suppression disables pooled triggers.

Validation
- Scene tests confirm messages and HP bars.

## Phase 5 — End-turn effects

Goal: pooled abilities apply for end-turn recoveries/penalties.

Implementation
- Update end-turn handlers in `src/battle_end_turn.c` to use effective abilities.
- Add per-ability end-turn latch to avoid double triggers.

Suggested tests
- Rain Dish/Dry Skin/Ice Body apply from pool.
- Magic Guard/Overcoat prevent damage as expected.

Validation
- Weather/status end-turn tests pass.

## Phase 6 — AI integration

Goal: AI sees pooled abilities for immunity/redirect/damage predictions.

Implementation
- Add AI helper (e.g., `AI_HasActiveAbility`).
- Update AI consolidators to consult effective abilities.

Suggested tests
- AI avoids ineffective moves when pooled immunities are present.
- AI chooses stronger moves when pooled damage boosts apply.

Validation
- Deterministic AI tests pass with Shared Power on.

## Phase 7 — Regression and parity

Goal: verify Shared Power off-path stays bit-accurate.

Implementation
- Add parity tests / smoke tests for standard battles with Shared Power off.
- Ensure no config or battle flags change vanilla output.

Suggested tests
- Existing suite unchanged with Shared Power off.
- A small smoke test suite for vanilla battles.

Validation
- `make check` clean with Shared Power disabled.
