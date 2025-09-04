# Damage Calculation & Continuous Effects

[Index](./README.md) · Related: [API](./api.md), [Events](./events.md), [Suppression & Edge Cases](./suppression_edge_cases.md)

Centralize Shared Power inside existing damage/path helpers to avoid touching every callsite.

## Offensive Modifiers

Apply if any effective ability on the attacker matches (stack where the base game stacks, otherwise apply once):

- Contact/punch/projectile boosts: Tough Claws, Iron Fist, Mega Launcher, Strong Jaw.
- Type/power modifiers: Adaptability, Technician, Analytic, Sniper, Sheer Force, Stakeout, Hustle, Gorilla Tactics, Guts/Toxic Boost/Flare Boost.
- Multi-hit: Parental Bond (consider scripting cadence/lockouts carefully).

## Defensive Modifiers

- Damage reduction: Filter/Solid Rock/Prism Armor, Multiscale, Fur Coat, Thick Fat.
- Immunities/absorptions/redirects: Levitate, Flash Fire, Water Absorb, Volt Absorb, Dry Skin, Sap Sipper, Lightning Rod, Storm Drain, Soundproof, Bulletproof.
- Ignore checks: Mold Breaker/Teravolt/Turboblaze must ignore breakable abilities across the entire target’s effective set.

## Implementation Strategy

- Replace equality checks like `ctx.abilityAtk == ABILITY_*` with `ForEachEffectiveAbility(battlerAtk, cb)` where a single ability changes power/calculation.
- For block/absorb/redirect decisions, update consolidators (`CanAbilityBlockMove`, `CanAbilityAbsorbMove`, redirect logic) to iterate over effective abilities and short-circuit on match.
- Maintain `gLastUsedAbility` where a specific ability needs to be surfaced in UI/logs when its effect applies.

## Stacking & Precedence

- Follow the engine’s existing stacking rules; Shared Power does not introduce new stacking beyond enabling multiple abilities to be present.
- If multiple effective abilities compete, preserve the engine’s order-of-operations.

See also: [Events & Dispatchers](./events.md), [Suppression & Edge Cases](./suppression_edge_cases.md).
