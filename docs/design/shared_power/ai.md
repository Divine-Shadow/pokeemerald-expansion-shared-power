# AI Integration

Keep the AI coherent by exposing effective abilities with minimal changes.

## Short-Term Approach

- Mirror `HasActiveAbility` in AI context: derive `abilitiesEffective[battler]` from the battle state (native + trainer pool after suppression/eligibility) at the start of scoring.
- Update AI helpers that query abilities (`AI_IsAbilityOnSide`, absorb/block checks) to consult the effective set.

## Longer-Term Improvements

- Fold effective ability checks into AI consolidators (e.g., prediction of redirection/absorptions, susceptibility to statuses).
- Incorporate shared abilities into switch/pivot decisions (e.g., value of bringing in a mon to grow the pool).

## Testing

- Add deterministic tests covering AI move choice when Shared Power changes immunity/redirect or damage-boost expectations.

See also: [State Model](./state.md), [Damage Calculation](./damage_calc.md).

