# Variant: Per-Side Ability Pool

This document explores making the Shared Power ability pool per-side (player side vs. opponent side) instead of per-trainer, and evaluates the impact on the existing plan.

## Summary of Change

- Current: each trainer has an independent pool; allied trainers only share if they are the same trainer (e.g., not MULTI/INGAME_PARTNER/TWO_OPPONENTS).
- Variant: a single pool per battle side (2 pools total). All battlers on the same side share abilities regardless of trainer identity.

## Behavioral Differences

- Multi battles (INGAME_PARTNER, TWO_OPPONENTS, MULTI):
  - Per-trainer: allies from different trainers keep separate pools.
  - Per-side: allies always share; abilities gained by one ally immediately benefit the other.
- Balance/expectations:
  - Per-side is simpler and more synergistic (stronger), but may diverge from modes that emphasize individual trainer identity.
  - Could be gated as a separate sub-mode (e.g., `BATTLE_TYPE_SHARED_POWER_SIDE`).

## State & API Impact

- State simplification:
  - Replace `SharedPower_GetTrainerIndex(battler)` with `SharedPower_GetPoolIndex(battler)` that maps to `GetBattlerSide(battler)` (0/1).
  - Pools array becomes size = number of sides (2) instead of dynamic trainer mapping.
- API changes:
  - `SharedPower_TrainerHasAbility` → rename to `SharedPower_PoolHasAbility` (or keep old name for compatibility and document semantics change).
  - Acquisition & queries otherwise unchanged.
- Pending switch-in queue remains per-battler.

## Integration Points

- Acquisition:
  - Insert entering mon’s native ability into `pool[GetBattlerSide(battler)]`.
- Switch-in activations:
  - Enumerate abilities from the side pool; activation flow unchanged.
- Field/side queries:
  - `IsAbilityOnSide` alignment improves: side-wide checks match side-wide pools.
  - `IsAbilityOnField/Except` remain effective ability checks; unaffected.

## Suppression & Edge Cases

- Suppression is still evaluated per-battler at activation time; sharing a pool does not bypass Neutralizing Gas, Gastro Acid, Ability Shield, or Mold Breaker interactions.
- Eligibility rules remain per-battler.

## AI Impact

- Reasoning simplifies: the AI can treat all allies on its side as having the union of abilities.
- Update AI data derivation to reflect per-side pools rather than trainer-identity mapping.

## Tests Impact

- Update tests that assert separate ally pools in MULTI/partner scenarios:
  - New expectations: abilities from one ally benefit the other immediately on the same side.
- Keep all other tests (switch-in triggers, continuous mods, suppression, on-faint) identical.

## Pros and Cons

- Pros:
  - Simpler state and mapping; fewer corner cases around trainer identity.
  - Natural fit for many “side” queries and doubles synergy.
- Cons:
  - Stronger mode; may deviate from the original design goal of per-trainer identity.
  - Less granularity in multi trainer battles (cannot keep ally pools independent).

## Implementation Delta (from base plan)

- State model: 2 side pools, not N trainer pools.
- API: adjust naming/semantics and the indexer helper.
- Acquisition/activation: replace trainer index with side index; otherwise identical.
- Docs/flagging: consider separate flag (e.g., `BATTLE_TYPE_SHARED_POWER_SIDE`) or a configuration knob (`SHARED_POWER_POOL_KIND = SIDE|TRAINER`).

See also: [State Model](./state.md), [Core API](./api.md), [Acquisition & Switch-In Flow](./acquisition_switchin.md), [Events & Dispatchers](./events.md).

