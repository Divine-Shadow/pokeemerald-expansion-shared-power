# Acquisition & Switch-In Flow

[Index](./README.md) · Related: [API](./api.md), [Events](./events.md), [Activation & UX](./activation_ux.md)

This file defines when abilities are added to pools and how switch-in effects run for all pooled abilities.

## Acquisition Timing

- On every send-out/switch-in (including replacement after fainting), add the entering mon’s native ability to its trainer’s pool via `SharedPower_AddToPool`, recording the source party index for per-switch-in dedup.
- Membership is independent of current suppression; suppression is applied at activation time per battler.

## Switch-In Effects

Integrate into existing entry flow (`DoSwitchInEffectsForBattler` / `DoSwitchInAbilities`).

1) Add entering mon’s ability to its trainer’s pool.
2) Compute the trainer’s current pool for this battler and enqueue all abilities that:
   - are not native duplicates for this battler, and
   - are eligible and not suppressed for this battler.
3) Drain the queue by invoking `AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battler, ability, 0, 0)` one ability at a time, preserving the normal script/popup flow.

### Latching and Re-entrancy

The engine uses `gSpecialStatuses[battler].switchInAbilityDone` to gate some cases for a single ability per battler. For Shared Power:

- Track per-ability execution for switch-in via a battler-scoped bitset/queue.
- For cases that check the latch, ensure it is scoped to the currently executing ability (e.g., temporarily clear/restore or change the condition to the new bitset).

### Order of Activation

- Use acquisition order by default: abilities fire in the order the trainer’s pool was built.
- Alternative: stable ascending by ability ID for deterministic behavior; decide once and document.

See also: [Events & Dispatchers](./events.md), [Activation Order & UX](./activation_ux.md).
