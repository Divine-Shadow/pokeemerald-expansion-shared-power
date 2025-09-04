# Suppression, Eligibility & Edge Cases

Shared Power must respect existing suppression/override rules and avoid nonsensical borrowed abilities.

## Suppression Rules

- Neutralizing Gas: suppresses all other abilities unless the battler is the source (per base rules).
- Gastro Acid: suppresses abilities on that battler.
- Ability Shield: protects a battler’s active abilities (native and borrowed) from suppression; integrate via `IsAbilitySuppressedFor`.
- Mold Breaker (and clones): ignore target’s breakable abilities across the entire effective set.

## Eligibility Filter

Some abilities should not apply when borrowed (species/form-bound or meaningless). Examples include Comatose, Zen Mode, Stance Change, Shields Down, Ice Face, Disguise, Forecast (context dependent), etc.

- Implement `SharedPower_IsEligibleFor(battler, ability)` with a small denylist; default to allowed.
- Start conservative and expand based on testing.

## Duplicates & Stacking

- Pool is a set; no duplicate membership.
- If the battler’s native ability is already in the pool, do not double-apply effects; equality and dedupe via `ForEachEffectiveAbility` handle this naturally.

## UI/Popups

- When invoking a non-native pooled ability’s script, set `gLastUsedAbility` to the pooled ability so popups/messages remain accurate.

See also: [Core API](./api.md), [Events & Dispatchers](./events.md).

