# Trait-System Extraction Map for Shared Power

This document lists exact functions/blocks from the `Trait-System` branch in `https://github.com/bassforte123/pokeemerald-complete` that are worth copying/adapting for Shared Power. Line references are from the `Trait-System` branch at `trait-system/Trait-System` (fetched locally).

## 1) Effective-ability query helpers (core pattern)

### Trait/ability scan helpers
- `include/battle_main.h:138-146` — `SearchTraits(...)` helper used everywhere to ask “does this battler have ability X” across a trait set.
- `include/battle_ai_main.h:125-138` — AI equivalents: `AI_BATTLER_HAS_TRAIT`, `AI_STORE_BATTLER_TRAITS`, and `AISearchTraits(...)`.
- `include/battle_util.h:73-82` — `STORE_BATTLER_TRAITS` / `STORE_BATTLER_TRAITS_IGNORE_MOLDBREAKER` macros.

**Shared Power adaptation:**
- Replace these with `SharedPower_ForEachEffectiveAbility`/`SharedPower_HasActiveAbility` (as per design docs), then use in the same callsites (field/side queries, damage calc, move-end, end-turn, AI).

### Trait resolution entry points
- `include/battle_util.h:458-465` — Declarations for `GetBattlerTrait`, `BattlerHasTrait`, `PushTraitStack`, etc.
- `src/battle_util.c:11541-11638` — `GetBattlerTrait`, `BattlerHasInnate`, `BattlerHasTrait`, `BattlerHasTraitPlain`.

**Shared Power adaptation:**
- Implement a `SharedPower_GetEffectiveAbilityList(...)` or `SharedPower_ForEachEffectiveAbility(...)` that centralizes eligibility + suppression checks, similar in spirit to `GetBattlerTrait` + `SearchTraits`.

## 2) Field/side/field-except helpers

- `src/battle_util.c:6023-6061` — `IsAbilityOnSide`, `IsAbilityOnOpposingSide`, `IsAbilityOnField`, `IsAbilityOnFieldExcept` rewritten to call `BattlerHasTrait`.

**Shared Power adaptation:**
- Redirect these helpers to `HasActiveAbility` so any existing “field/side ability” checks automatically reflect Shared Power.

## 3) Multi-activation latches (per-ability)

- `include/battle.h:220-221` — `switchInTraitDone[MAX_MON_TRAITS]` and `endTurnTraitDone[MAX_MON_TRAITS]` added to `struct SpecialStatus`.
- `src/battle_util.c:279-285` — `CommonSwitchInAbilities(...)` sets the latch and pushes a popup.
- `src/battle_util.c:4140-4639` — `AbilityBattleEffects` switch-in section uses `switchInTraitDone` checks per ability.
- `src/battle_util.c:4641-4700` (and onward) — `AbilityBattleEffects` end-turn section uses `endTurnTraitDone` per ability.

**Shared Power adaptation:**
- Add per-ability latch state (bitset or array) per battler for switch-in and end-turn. Use this to avoid double-triggering pooled abilities when multiple abilities are in play.

## 4) Ability popup stack for ordered multi-ability UI

### Data + storage
- `src/battle_main.c:175` — `gTraitStack[MAX_BATTLERS_COUNT * MAX_MON_TRAITS][2]` global storage.
- `src/battle_util.c:11640-11702` — `PushTraitStack`, `PullTraitStackBattler`, `PullTraitStackAbility`, `PopTraitStack`.

### Consumption in scripts
- `src/battle_script_commands.c:17490-17504` — `BS_ShowAbilityPopup` pops from the stack and shows popups in order.

**Shared Power adaptation:**
- Reuse the stack approach to queue Shared Power popups so pooled abilities appear in deterministic order without clobbering `gLastUsedAbility`.

## 5) Script-level “has ability” checks

- `src/battle_script_commands.c:4425-4467` — `Cmd_jumpifability` updated to:
  - Use `BattlerHasTrait` for specific battlers.
  - Use `IsAbilityOnSide` / `IsAbilityOnOpposingSide` for side checks.
  - Push ability to popup stack when matched.

**Shared Power adaptation:**
- Ensure any script command equivalent to `jumpifability` consults the effective-ability set and queues popups. This is a key hook for Shared Power to integrate cleanly with existing battle scripts.

## 6) Ordered switch-in chain (multi triggers)

- `src/battle_util.c:4140-4639` — `AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN)` uses a long ordered `if` chain instead of a mutually exclusive switch, enabling multiple abilities to fire in sequence.

**Shared Power adaptation:**
- Use the same “ordered if-chain” philosophy for pooled abilities to preserve deterministic order and avoid “one ability per event” assumptions.

## 7) AI visibility of effective abilities

- `include/battle_ai_main.h:125-138` — AI helpers for trait search.
- Many AI callsites use `AI_BATTLER_HAS_TRAIT` or `AISearchTraits` as a centralized ability check.

**Shared Power adaptation:**
- Provide AI equivalents (`AI_HasActiveAbility` / `AI_ForEachEffectiveAbility`) so AI logic and damage predictions incorporate pooled abilities consistently.

## 8) Additional notes

- The Trait System “multi-ability” design is much broader than Shared Power (new UI page, innates in data, test duplication). Only the **centralized query**, **latch**, **popup stack**, **AI helper**, and **field/side checks** are recommended for extraction.
- This map avoids copying any data-formatting or non-battle systems.

## 9) Shared Power landing map (where to adapt in this repo)

This section maps each extracted pattern to likely landing points in **this repo** (based on `docs/design/shared_power/implementation_map.md`). Exact line numbers will vary until implementation lands.

### Effective-ability query helpers
- Trait-System sources:
  - `include/battle_main.h:138-146` (`SearchTraits`)
  - `include/battle_ai_main.h:125-138` (AI helpers)
  - `include/battle_util.h:73-82` (trait macros)
- Shared Power targets:
  - `include/battle_shared_power.h` (new): declare `SharedPower_IsEnabled`, `HasActiveAbility`, `ForEachEffectiveAbility`.
  - `src/battle_shared_power.c` (new): implement effective ability iteration + suppression/eligibility gates.
  - `src/battle_ai_util.c` / `include/battle_ai_util.h`: add `AI_HasActiveAbility` or a shared helper for AI visibility.

### Field/side/field-except helpers
- Trait-System sources: `src/battle_util.c:6023-6061`.
- Shared Power targets:
  - `src/battle_util.c`: update `IsAbilityOnSide`, `IsAbilityOnOpposingSide`, `IsAbilityOnField`, `IsAbilityOnFieldExcept` to call `HasActiveAbility`.
  - `src/battle_ai_util.c`: mirror for AI-side equivalents where they exist.

### Multi-activation latches
- Trait-System sources:
  - `include/battle.h:220-221` (per-ability latches)
  - `src/battle_util.c:279-285` (`CommonSwitchInAbilities`)
  - `src/battle_util.c:4140-4700` (`AbilityBattleEffects` switch-in + end-turn sections)
- Shared Power targets:
  - `include/battle.h`: add per-battler switch-in/end-turn latch structure (bitset or array).
  - `src/battle_script_commands.c`: gate switch-in activation with per-ability latch when draining pooled abilities.
  - `src/battle_end_turn.c`: apply latch in end-turn ability processing (Dry Skin/Rain Dish/etc).

### Ability popup stack
- Trait-System sources:
  - `src/battle_main.c:175` (stack storage)
  - `src/battle_util.c:11640-11702` (Push/Pull/Pop)
  - `src/battle_script_commands.c:17490-17504` (BS_ShowAbilityPopup)
- Shared Power targets:
  - `include/battle.h` + `src/battle_main.c`: add storage for queued popups (reuse or parallel to existing popup flow).
  - `src/battle_util.c`: implement stack helpers (or reuse existing ability popup plumbing if present).
  - `src/battle_script_commands.c`: have popup display read from the queue for pooled abilities.

### Script-level “has ability” checks
- Trait-System sources: `src/battle_script_commands.c:4425-4467` (`Cmd_jumpifability`).
- Shared Power targets:
  - `src/battle_script_commands.c`: update ability script command(s) to consult `HasActiveAbility` and push popup queue.

### Ordered switch-in chain (multi triggers)
- Trait-System sources: `src/battle_util.c:4140-4639` (ordered `if` chain).
- Shared Power targets:
  - `src/battle_script_commands.c`: the shared-power switch-in drain should preserve a deterministic ability order.
  - `src/battle_shared_power.c`: decide/encode ability order (acquisition order or sorted ID).

### AI visibility of effective abilities
- Trait-System sources: `include/battle_ai_main.h:125-138` + AI callsites.
- Shared Power targets:
  - `src/battle_ai_util.c`: wrap or replace existing AI ability checks so AI sees pooled abilities.
  - `include/battle_ai_util.h`: declare new AI helpers.
