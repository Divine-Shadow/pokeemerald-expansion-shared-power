# Core API

[Index](./README.md) · Related: [State](./state.md), [Events](./events.md), [Damage Calc](./damage_calc.md)

The API exposes a virtual view of “effective abilities” (native + pool) without changing how `gBattleMons[battler].ability` is stored.

## Feature Gate

- `bool32 SharedPower_IsEnabled(void)`

## Pool Operations

- `u8 SharedPower_GetTrainerIndex(u8 battler)` — resolve battler → trainer index.
- `bool32 SharedPower_AddToPool(u8 trainerIdx, u16 ability, u8 sourcePartyIndex)` — insert (dedup), returns true if newly added; always records a source entry even when duplicates exist.
- `bool32 SharedPower_TrainerHasAbility(u8 trainerIdx, u16 ability)` — membership test.

## Effective Ability Queries

- `bool32 IsAbilitySuppressedFor(u8 battler, u16 ability, bool32 ignoreMoldBreaker, bool32 noAbilityShield)` — generalized suppression (Gastro Acid, Neutralizing Gas, ability flags, Ability Shield, Mold Breaker).
- `bool32 SharedPower_IsEligibleFor(u8 battler, u16 ability)` — denylist for form/species-locked or nonsensical abilities when borrowed (default true).
- `bool32 HasActiveAbility(u8 battler, u16 ability)` — true if the battler’s native ability equals `ability` (and not suppressed) OR the pool contains `ability` and it’s not suppressed and is eligible for this battler.
- `bool32 ForEachEffectiveAbility(u8 battler, bool32 (*cb)(u16 ability))` — iterate native + pool (deduped), stop when callback returns true.

## Switch-In Queue

- Append abilities from the trainer pool to `pending_switchin_abilities[battler]` and drain them through the regular `AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, ...)` path.

See also: [Events & Dispatchers](./events.md), [Damage Calculation](./damage_calc.md).
