# Config, Feature Flag & Tests

[Index](./README.md) · Related: [Implementation Map](./implementation_map.md), [Events](./events.md)

## Feature Flag

- Introduce `BATTLE_TYPE_SHARED_POWER` (or similar) as a battle-type bitflag.
- Toggle at battle creation; guard Shared Power logic with `SharedPower_IsEnabled()`.

## Configuration

- Keep config surface minimal; Shared Power is a discrete battle mode, not a per-ability toggle.
- Document activation in any user-facing docs that list battle types/modes.

## Test Plan

Create deterministic tests under `test/battle/...` that mirror engine areas:

- Pool growth & reset: adding on switch-in, no duplicates, cleared on battle end; separate pools for distinct trainers in multi battles.
- Switch-in effects: Intimidate, Drizzle/Drought/Snow Warning/Surges, Frisk, Hospitality, Ruin abilities fire for non-native holders.
- Continuous modifiers: Tough Claws/Iron Fist/Adaptability/Technician/Sheer Force apply from pool.
- Immunity/absorb/redirect: Levitate, Flash Fire, Water/Volt Absorb, Lightning Rod, Storm Drain, Sap Sipper.
- On-faint: Aftermath, Innards Out from pool.
- Suppression interactions: Neutralizing Gas, Gastro Acid, Ability Shield, Mold Breaker ignoring defense.
- AI behavior: ensure expected move choice in scenarios with Shared Power immunity/redirect and damage boosts.

See also: [State Model](./state.md), [Events & Dispatchers](./events.md).
