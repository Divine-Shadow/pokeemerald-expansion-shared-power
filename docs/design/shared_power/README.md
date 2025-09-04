# Shared Power Mode — Design Overview

This folder documents the proposed “Shared Power” battle mode, scoped to the battle engine and designed for minimal, maintainable integration.

- Each time a Pokémon is sent out, its ability is added to its trainer’s pool.
- Pools are per-trainer (not global), contain no duplicates, and reset after the battle.
- All Pokémon a trainer uses during the fight benefit from every ability currently in that trainer’s pool.
- Applies to all ability categories: switch-in, continuous, and on-faint/after-effects.

Use the sections below for focused details; files reference each other where relevant.

- [Mode Scope](./scope.md)
- [State Model](./state.md)
- [Core API](./api.md)
- [Acquisition & Switch-In Flow](./acquisition_switchin.md)
- [Events & Dispatchers](./events.md)
- [Damage Calculation & Continuous Effects](./damage_calc.md)
- [Suppression, Eligibility & Edge Cases](./suppression_edge_cases.md)
- [AI Integration](./ai.md)
- [Activation Order & UX](./activation_ux.md)
- [Config, Feature Flag & Tests](./config_tests.md)
- [Implementation Map (touch points)](./implementation_map.md)
- [Variant: Per-Side Ability Pool](./per_side_pool.md)
- [Game Mode Explainer](./mode_explainer.md)

## Design Goals

- Minimal surface area: extend existing centralized ability dispatch (switch-in, move-end, end-turn, damage calc) rather than duplicating logic.
- Maintainability: introduce a small, testable layer for “effective abilities” (native + pooled), keep data local to `BattleStruct`.
- Ergonomics: express intent with helpers like `HasActiveAbility`/`ForEachEffectiveAbility` and keep scripts/popups intact.

## Quick Start

- Read [State Model](./state.md) and [Core API](./api.md) first.
- Then follow [Acquisition & Switch-In Flow](./acquisition_switchin.md) for entry sequencing.
- For behavior coverage, see [Events & Dispatchers](./events.md) and [Damage Calculation](./damage_calc.md).
- For special cases and rule interactions, see [Suppression & Edge Cases](./suppression_edge_cases.md).
