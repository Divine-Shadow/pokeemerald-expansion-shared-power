# Events & Dispatchers

Leverage centralized dispatchers to apply Shared Power with minimal edits.

## Field/Side Ability Queries

Update these helpers to consult effective abilities:

- `IsAbilityOnField`, `IsAbilityOnSide`, `IsAbilityOnFieldExcept` → check `HasActiveAbility` instead of raw `GetBattlerAbility(...) == ability`.
- Escape prevention (`IsAbilityPreventingEscape`) → inspect targets’ effective abilities.

## Switch-In

Handled as described in [Acquisition & Switch-In Flow](./acquisition_switchin.md).

## Move End / On-Hit

Sections in `battle_util.c` and `battle_script_commands.c` that run on-hit/move-end ability effects (e.g., Rough Skin/Iron Barbs, Gooey/Tangling Hair, Color Change, Aftermath/Innards Out) should:

- Use `HasActiveAbility(battler, ABILITY_*)` for both attacker and defender gating.
- When a script requires `gLastUsedAbility`, temporarily set it to the matching ability before calling the script so popups/messages are correct.

## End of Turn

`src/battle_end_turn.c` already calls ability effects by weather/poison/burn/etc. Modify gates to `HasActiveAbility` where appropriate:

- Weather recovery/penalty: Dry Skin, Rain Dish, Ice Body, Magic Guard gating.
- Status/volatiles interactions: Magic Guard, Overcoat, etc.

## On-Going Auras & Field Modifiers

Checks for global abilities (Aura Break, Ruin abilities, Lightning Rod/Storm Drain redirection) should rely on field/side queries updated to effective abilities.

See also: [Damage Calculation](./damage_calc.md), [Suppression & Edge Cases](./suppression_edge_cases.md).

