# Implementation Map

[Index](./README.md) · Related: [API](./api.md), [Events](./events.md), [Config & Tests](./config_tests.md)

High-leverage, minimal changes with file-level pointers.

## New Module

- `src/battle_shared_power.c` (new): implement helpers from [Core API](./api.md). Add a header under `include/`.

## State

- `include/battle.h` / `src/battle_main.c`:
  - Add per-trainer pool state and per-battler pending switch-in queue to `struct BattleStruct`.
  - Initialize/clear alongside other battle runtime fields.

## Field/Side Queries

- `src/battle_util.c`:
  - Update `IsAbilityOnField`, `IsAbilityOnSide`, `IsAbilityOnFieldExcept`, and `IsAbilityPreventingEscape` to use `HasActiveAbility`.

## Switch-In Flow

- `src/battle_script_commands.c` (`DoSwitchInEffectsForBattler`, `DoSwitchInAbilities`):
  - Add entering mon’s ability to trainer pool.
  - Enqueue and drain pooled abilities via `AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, ...)` (see [Acquisition & Switch-In Flow](./acquisition_switchin.md)).

## Move-End / On-Hit

- `src/battle_util.c`, `src/battle_script_commands.c`:
  - Replace direct ability equality gates with `HasActiveAbility` for on-hit/move-end sections (Rough Skin/Iron Barbs, Gooey/Tangling Hair, Color Change, Aftermath/Innards Out, etc.).

## End of Turn

- `src/battle_end_turn.c`:
  - Update weather/status tick handlers to consult `HasActiveAbility` (Dry Skin, Rain Dish, Ice Body, Magic Guard, Overcoat, etc.).

## Damage Calculation

- `src/battle_util.c`:
  - Offensive modifiers and immunity/absorb/redirect consolidators should iterate effective abilities (see [Damage Calculation](./damage_calc.md)).

## AI

- `src/battle_ai_util.c` and related:
  - Provide effective ability visibility; update consolidators to consult the effective set where they currently read abilities.

See also: [Events & Dispatchers](./events.md), [Suppression & Edge Cases](./suppression_edge_cases.md).
