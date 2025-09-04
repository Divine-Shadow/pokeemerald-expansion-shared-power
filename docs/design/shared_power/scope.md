# Mode Scope

Shared Power is a battle-only mode. It must not affect overworld ability checks or encounter systems.

- Feature flag: enable via a dedicated battle-type flag (e.g., `BATTLE_TYPE_SHARED_POWER`).
- Lifetime: per-battle; pools are initialized when battle state is created and cleared at battle end.
- Granularity: per-trainer pools, not globally shared across an entire side when partners are from different trainers.
- Applicability: affects all ability categories — switch-in, continuous, prevention/redirect, on-hit, move-end, end-turn, and on-faint.

See also: [State Model](./state.md), [Events & Dispatchers](./events.md).

