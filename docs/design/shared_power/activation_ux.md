# Activation Order & UX

[Index](./README.md) · Related: [Acquisition & Switch-In](./acquisition_switchin.md), [Events](./events.md)

## Activation Order

- Default: acquisition order — abilities activate in the order they were added to the trainer’s pool.
- Alternative: deterministic ascending ability ID — simpler to reason about but less thematic.

Choose one policy and document it here; the design assumes acquisition order.

## Switch-In Popups

- Invoking `AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battler, ability, 0, 0)` ensures popups and scripts use the current battler and the pooled ability.
- Use a per-battler queue to serialize multiple switch-in activations over frames (consistent with engine flow).

## Messages & Logs

- Set `gLastUsedAbility` to the currently executing pooled ability when running scripts to keep messages accurate.

See also: [Acquisition & Switch-In Flow](./acquisition_switchin.md), [Events & Dispatchers](./events.md).
