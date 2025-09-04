# Shared Power — Game Mode Explainer

Shared Power is a special battle mode where teammates accumulate and share abilities across the battle.

## Core Rules

- When a Pokémon is sent out, its ability is added to a shared pool for its owner.
- The pool contains no duplicates and resets at the end of the battle.
- Every Pokémon a trainer uses in that battle benefits from all abilities currently in that trainer’s pool.
- All ability categories apply: switch-in effects (e.g., Intimidate, Drizzle), continuous modifiers (e.g., Tough Claws, Magic Guard), redirection/absorption (e.g., Lightning Rod, Storm Drain), and on-faint effects (e.g., Aftermath).

## Scope

- Battle-only: overworld features like wild encounter modifiers are unaffected.
- Per-trainer by default: partners from different trainers do not share unless configured. See the [Per-Side Pool Variant](./per_side_pool.md).

## Examples

- Lead with Pelipper (Drizzle): rain starts. Later, switch to Barraskewda — it also benefits from Drizzle (rain already up) and any other abilities accumulated.
- Send out Luxray (Intimidate), then Drednaw (Strong Jaw): subsequent teammates have both Intimidate (on switch-in) and Strong Jaw (for biting moves).
- Use a partner with Lightning Rod: later switch to a special attacker on the same trainer — their Electric moves can be redirected for boosts if the mode variant allows side-wide sharing.

## Variants

- Per-Trainer (default): each trainer maintains a separate pool.
- Per-Side (optional): one pool per side; allies always share. See [variant details](./per_side_pool.md).

## Enabling

- Intended to be toggled via a battle-type flag (e.g., `BATTLE_TYPE_SHARED_POWER`). See [Config & Tests](./config_tests.md).

For implementation details, start at the [design overview](./README.md).

