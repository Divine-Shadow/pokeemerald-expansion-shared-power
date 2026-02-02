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
- Test logging helper: `docs/engineering/test_logging.md`

## Phase 0 — Scaffolding and gating

Goal: establish compile/runtime gates with no behavioral changes when off.

Implementation
- Add `include/battle_shared_power.h` and `src/battle_shared_power.c` with `SharedPower_IsEnabled()` stub.
- Add config flag (e.g., `CONFIG_SHARED_POWER`) and battle-type flag (e.g., `BATTLE_TYPE_SHARED_POWER`).
- Ensure Shared Power codepaths are compiled but gated.
- Test scaffolding: keep `test/battle/shared_power.c` compiling by adding result fields for `captureDamage`, using valid DSL `Ability(...)` calls, and use a test DSL helper to set `BATTLE_TYPE_SHARED_POWER`.

Suggested tests
- Unit test: Shared Power is off by default.
- Battle test: flag on/off toggles `SharedPower_IsEnabled()` without affecting vanilla flow when off.

Validation
- Shared Power tests compile and run; failing Shared Power tests are explicitly grandfathered until later phases.
- No general pass for failing tests outside Shared Power.

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
- Track pooled sources (party index) in the pool while switch-in processing dedupes per ability for that event.
- Explicitly scope ability overrides during pooled switch-in effects to prevent leaking overrides across script pauses.
- Provide unique vs all-sources effective ability iterators for Phase 3+.

Friction observed
- Switch-in scripts rely on a single `switchInAbilityDone` latch and `gLastUsedAbility`, so pooled abilities are skipped or show wrong popups.
- Tests fail with unmatched ability popups because the switch-in path assumes one ability per battler.
- Restoring a shared-power ability override after a switch could overwrite the incoming mon’s native ability if the battler index changed species.

Resolution path
- Add per-ability switch-in latches (bitset) to prevent duplicates without blocking other pooled abilities.
- Ensure `gLastUsedAbility` reflects the pooled ability when invoking `AbilityBattleEffects`.
- (If needed) add a popup queue so `BS_ShowAbilityPopup` can display pooled abilities deterministically.
- Track the original party index for each popup override; only restore the original ability when the battler still has the same party index.

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
- Stat-change reactions (e.g. Defiant/Competitive/Inner Focus) should respect pooled abilities.

Suggested tests
- Adaptability/Technician/Tough Claws apply when pooled.
- Duplicates don’t stack beyond existing rules.
- Defiant/Competitive/Inner Focus interact correctly with pooled Intimidate.

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
- Off-path smoke tests confirm pooled abilities do not apply without `BATTLE_TYPE_SHARED_POWER` (e.g. no pooled Rain Dish or Intimidate).
