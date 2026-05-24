# Shared Power Ability Usage Audit

[Index](./README.md) - Related: [Core API](./api.md), [Events](./events.md), [Damage Calc](./damage_calc.md), [AI](./ai.md)

This audit classifies battle-core ability usage by intent so future Shared Power fixes can decide whether each callsite should stay native-only, migrate to effective ability queries, or wait for a design decision.

## Core Finding

`GetBattlerAbility()` is still a native/current-slot query with suppression and breakthrough rules. It does not mean "all abilities active through Shared Power." Shared Power behavior is represented by `HasActiveAbility()`, `ForEachEffectiveAbility*()`, and explicit Shared Power event iterators.

The risky callsites are not just `GetBattlerAbility(...) == ABILITY_*`. They also include cached `abilityAtk`/`abilityDef` parameters, direct `gBattleMons[].ability` reads, popup attribution through `gLastUsedAbility`, AI cached ability arrays, and helpers that accept battler arguments but still read `gBattlerTarget`, `gEffectBattler`, or `gBattlerAttacker`.

## Classification Labels

| Label | Default decision | Meaning |
| --- | --- | --- |
| `AB_NATIVE_SUPPRESSED_SINGLE` | Stay or clarify | Needs the battler's own current ability slot after suppression rules. |
| `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate or stay if already using helper | A yes/no ability check should include Shared Power pool membership. |
| `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Stay if already iterator-based | Multiple effective abilities may modify one calculation, but duplicate sources should not stack. |
| `AB_EFFECTIVE_SHARED_ITERATION_EVENT` | Stay or clarify | Ability events may trigger through ordered Shared Power dispatch. |
| `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Helper receives explicit ability parameters; callers must define whether those parameters are native, predicted, or selected effective abilities. |
| `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify | Helper mixes explicit arguments with global attacker/target/effect state. |
| `AB_POPUP_ATTRIBUTION` | Stay or clarify | UI/message identity, usually through `gLastUsedAbility`, `gBattlerAbility`, and popup override state. |
| `AB_RECORD_ATTRIBUTION` | Clarify | Battle history or AI knowledge recording; the recorded ability may differ from native holder. |
| `AB_MUTATION_NATIVE_SLOT` | Stay | Ability copy, overwrite, swap, or suppress mechanics that mutate native battle state. |
| `AB_NATIVE_ONLY_FORM_OR_SPECIES_GATE` | Stay or clarify | Species/form identity checks that should not automatically use donated abilities. |
| `AB_AI_CACHED_PREDICTION` | Clarify | AI known/predicted ability state, not necessarily live Shared Power truth. |

## Decision Rules

- Mark **stay** when the code initializes, mutates, copies, swaps, suppresses, stores, restores, or displays the native ability slot.
- Mark **migrate** when the code asks whether a battler benefits from, is protected by, or is blocked by an ability under current battle rules.
- Mark **iterate** when multiple effective abilities can participate in one calculation or phase.
- Mark **clarify** when source identity, affected battler, popup battler, holder semantics, duplicate behavior, AI knowledge, or form/species policy changes the answer.
- For hardcoded globals, classify by intended roles: ability source, active battler, attacker, target, effect battler, damaged battler, popup battler.

## Inventory

| Area | Callsite | Label | Decision | Rationale |
| --- | --- | --- | --- | --- |
| Shared Power core | [`HasActiveAbility`, `ForEachEffectiveAbility*`](../../../src/battle_shared_power.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Stay | These are the intended abstraction for native plus pool queries. |
| Shared Power core | temporary ability override in popup/switch-in dispatch | `AB_MUTATION_NATIVE_SLOT`, `AB_POPUP_ATTRIBUTION` | Stay | This intentionally shims vanilla scripts; do not replace with generic effective checks. |
| Field/side helpers | [`IsAbilityOnSide`, `IsAbilityOnField`, `IsAbilityOnFieldExcept`](../../../src/battle_util.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Stay | Already centralizes field/side checks through active ability membership. |
| Move blocking | [`CanAbilityBlockMove`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Migrate | Takes explicit abilities but also fetches partner ability and can record the wrong battler/ability when an ally blocks. |
| Move absorption | [`CanAbilityAbsorbMove`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Stay, audit callers | Cleaner explicit-ability helper; callers must pass the selected effective ability and attribution owner. |
| Accuracy skip | [`CanMoveSkipAccuracyCalc`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Migrate contract | It accepts `abilityAtk`/`abilityDef` but ignores them for No Guard. Decide whether to remove params or honor supplied ability context. |
| Accuracy math | [`GetTotalAccuracy`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Mostly uses supplied abilities, but ally Victory Star still uses native partner ability. |
| Type effectiveness | [`CalcTypeEffectivenessMultiplierInternal`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Uses `DamageContext`, but some immunity checks use `HasActiveAbility`; define context-vs-live semantics. |
| Damage calc entry | [`CalculateMoveDamage` / `CalculateMoveDamageVars`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Live calc refreshes native abilities; AI calc expects prefilled context. Audit callers before changing. |
| Damage modifiers | base power, attack, defense, final damage callbacks | `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Stay | Existing unique effective iteration is the right model for passive numeric modifiers. |
| Partner damage auras | Battery, Power Spot, Steely Spirit, Flower Gift, Friend Guard, Victory Star | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate or clarify | Several partner checks still use native partner ability; decide if shared partner auras apply from pooled abilities. |
| Contact helpers | [`IsMoveMakingContact`, `CanBattlerAvoidContactEffects`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Callers sometimes pass `ABILITY_NONE` deliberately; define raw contact vs contact after active ability modifiers. |
| Status helpers | [`CanSetNonVolatileStatus`, `CanBe*`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Callers supply abilities, while helper mutates `battlerDef` for side blockers and uses move globals. |
| Move-end iteration | [`SharedPower_GetNextMoveEndAbility`](../../../src/battle_util.c) | `AB_EFFECTIVE_SHARED_ITERATION_EVENT` | Stay, audit callbacks | Iteration is shared-aware, but callbacks use global target/attacker state. |
| Move-end target callbacks | [`TryMoveEndAbilityEffect`](../../../src/battle_util.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify | Some cases use `battler`, others hardcode `gBattlerTarget` or mutate native ability. |
| Move-end attacker callbacks | [`TryMoveEndAttackerAbilityEffect`](../../../src/battle_util.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify | Poison Touch/Toxic Chain consult target native ability and contact globals while executing pooled attacker abilities. |
| Native mutation effects | Mummy, Lingering Aroma, Wandering Spirit, Trace state | `AB_MUTATION_NATIVE_SLOT` | Stay, clarify shared source | These mutate or store native ability slots; shared donated abilities should not be overwritten blindly. |
| Form/species gates | Cramorant/Gulp Missile, Pecharunt/Poison Puppeteer, form-change tables | `AB_NATIVE_ONLY_FORM_OR_SPECIES_GATE` | Stay or denylist | Species/form identity behavior should not automatically become shareable. |
| Powder check | [`IsMovePowderBlocked`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Migrate | Accepts `battlerDef` but records Overcoat against `gBattlerTarget`; attribution should use the actual defender. |
| Attack canceler | [`Cmd_attackcanceler`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Migrate | Top-level script path into block/absorb helpers; Magic Bounce partner logic uses native ability. |
| Accuracy command | [`AccuracyCheck`](../../../src/battle_script_commands.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Migrate | Recomputes native abilities while iterating real targets and mutates `gBattlerTarget` for Dragon Darts. |
| Move effect application | [`SetMoveEffect`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify, then migrate | Has `battler/effectBattler` args but still relies on global target/attacker in several branches. |
| Stat buffs | [`ChangeStatBuffs`](../../../src/battle_script_commands.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Mostly stay | Recent fixes select active stat-loss prevention abilities; Flower Veil source attribution still deserves review. |
| Move-end move block | Knock Off, Sticky Hold, Gorilla Tactics handling | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Migrate | Current checks/popup attribution can use the target's native ability instead of shared Sticky Hold/Gorilla Tactics. |
| Symbiosis | `TrySymbiosis`, `BS_TrySymbiosis` | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Migrate | Activation and `gLastUsedAbility` should represent shared Symbiosis when pooled. |
| Party status heal | `Cmd_healpartystatus` Soundproof checks | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_RECORD_ATTRIBUTION` | Migrate | Shared Soundproof should block and record as `ABILITY_SOUNDPROOF`, not native holder ability. |
| Infatuation | `BS_TrySetInfatuation` | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate | Direct native Oblivious bypasses suppression/shared membership. |
| Last-used ability command | `BS_SetLastUsedAbility` | `AB_POPUP_ATTRIBUTION` | Clarify | Native target ability is correct only for scripts that explicitly want native identity. |
| Ability mutation commands | Transform, Role Play, Doodle, Gastro Acid, Skill Swap, Worry Seed, Simple Beam, Entrainment, Trace, Receiver | `AB_MUTATION_NATIVE_SLOT` | Stay | These intentionally copy, suppress, or overwrite native ability state. |
| Neutralizing Gas script checks | faint, switch-in, switch-out, mutation paths | `AB_NATIVE_SUPPRESSED_SINGLE` | Clarify policy | Shared Power treats Neutralizing Gas specially; native-only checks may be intentional but need policy text. |
| Ability popups | `BS_ShowAbilityPopup`, `GetDisplayedAbilityForMessage`, `UpdateAbilityPopup` | `AB_POPUP_ATTRIBUTION` | Stay | Central arbitration already considers Shared Power popup state and `gLastUsedAbility`. |
| End-turn iterators | weather, first/third/fourth blocks, ability block | `AB_EFFECTIVE_SHARED_ITERATION_EVENT` | Stay, audit filters | Current filters explicitly select end-turn abilities; review missing entries and duplicate behavior. |
| End-turn passive damage | weather damage, poison, burn, nightmare, curse, leech seed, Yawn | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Mixed | Many use `HasActiveAbility`; Leech Seed/Yawn still need source/target contract clarification. |
| Speed calc | [`GetBattlerTotalSpeedStatArgs`](../../../src/battle_main.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate remainder | Weather speed and Quick Feet are fixed; Surge Surfer, Slow Start, Protosynthesis, Quark Drive, and Unburden remain native comparisons. |
| Priority/order | priority, Stall/Mycelium Might, Quick Draw | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate or clarify | Decide if pooled priority and ordering penalties are intended under Shared Power. |
| Dynamic move type | Ate abilities, Liquid Voice, Normalize | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Clarify | Type conversion can have source/order implications; do not migrate blindly. |
| Run Away/trapping | run checks and displayed trap ability | `AB_RECORD_ATTRIBUTION` | Clarify | Run Away is active-aware; trapping report may still expose native holder ability. |
| Terastal STAB | Adaptability in terastal damage path | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate | If Shared Power grants Adaptability elsewhere, this path should match. |
| Sleep decrement | Early Bird in `battle_util2.c` | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Migrate | Passive duration modifier should include pooled Early Bird if ability is shareable. |
| Player preview | effectiveness preview damage context | `AB_HELPER_SUPPLIED_TARGET_STATE` | Migrate or clarify | UI can diverge if it previews native abilities while battle uses effective ability sets. |
| AI wrappers | `AI_HasActiveAbility`, `AI_CanAbilityBlockMove`, `AI_CanAbilityAbsorbMove`, AI status helpers | `AB_AI_CACHED_PREDICTION` | Stay | These are the current Shared Power-aware AI adapters. |
| AI damage usability | `IsDamageMoveUnusable`, `AI_CalcDamage` | `AB_AI_CACHED_PREDICTION` | Clarify | Some paths use `DamageContext`, others reread `gAiLogicData`; align contracts after live helper decisions. |
| AI known ability | `AI_DecideKnownAbilityForTurn`, switch prediction, switch items | `AB_AI_CACHED_PREDICTION` | Clarify | Decide whether AI knows one revealed native ability or the full Shared Power pool. |

## Hardcoded Target-State Queue

These helpers deserve a per-case owner/target review before behavior changes:

| Helper | Current risk | Required audit question |
| --- | --- | --- |
| `CanAbilityBlockMove` | Partner blocker path can record defender instead of partner. | Who is the blocker and popup owner when ally or pooled ability blocks? |
| `SetMoveEffect` | Takes `battler/effectBattler` but uses `gBattlerTarget` for Mirror Armor and other branches. | Should each move effect key off attacker, target, or effect battler? |
| `TryMoveEndAbilityEffect` | Some abilities use `battler`; others use `gBattlerTarget` or mutate native attacker ability. | Which battler owns the ability and which battler was affected? |
| `TryMoveEndAttackerAbilityEffect` | Uses `gBattlerAttacker`, `gBattlerTarget`, `gLastUsedAbility`, and target native status blockers. | Should pooled attacker abilities use active attacker as source or pooled holder as source? |
| `CanSetNonVolatileStatus` | Mutates `battlerDef` for side blockers and mixes explicit abilities with `gCurrentMove`. | Is this a pure predicate or a script-attribution resolver? |
| `AccuracyCheck` | Iterates targets but mutates global target for special targeting. | Should helper calls receive explicit target state instead of globals? |
| `HandleEndTurnLeechSeed` | `battler` is seeded victim, `gBattlerTarget` is receiver, Liquid Ooze checks seeded victim. | Is Liquid Ooze owned by the drained Pokemon or the healed receiver under Shared Power? |
| `HandleEndTurnYawn` | Mixes native ability and `HasActiveAbility` for sleep prevention. | Should all Yawn sleep blockers route through one active status-prevention helper? |
| `BS_SetLastUsedAbility` | Stores native target ability for later script display. | Which callers require native identity versus active displayed ability? |

## Clarification Queue

| Priority | Topic | Default until clarified |
| --- | --- | --- |
| High | Status helper contract | Do not migrate individual status wrappers until `CanSetNonVolatileStatus` defines source, target, and attribution semantics. |
| High | Partner/side ability ownership | Treat partner auras/blockers as clarify unless the effect has an existing `HasActiveAbility` pattern. |
| High | Popup and record attribution | Use triggering ability constants for migrated behavior; do not record native slot by default. |
| High | Native mutation/copy mechanics | Keep native-only; document whether donated abilities can be copied, suppressed, or overwritten. |
| Medium | Form/species ability gates | Keep native-only or add explicit Shared Power eligibility denylist entries. |
| Medium | Damage context semantics | Decide when `DamageContext.abilityAtk/Def` are authoritative versus live effective ability queries. |
| Medium | AI pool knowledge | Decide if AI sees current Shared Power pools or only one known native ability. |
| Medium | Duplicate source behavior | Keep unique iteration unless the underlying vanilla event intentionally triggers per source. |
| Low | UI previews | Align move previews after live battle semantics are settled. |

## Test Gaps

| Behavior class | Current state | Future regression shape |
| --- | --- | --- |
| Pooled speed/weather | Quick Feet and Swift Swim covered. | Add remaining speed modifiers only after deciding shareability. |
| Pooled priority/order | Not broadly covered. | Tests for Prankster/Triage/Gale Wings and Stall/Mycelium Might order if migrated. |
| Partner auras/blockers | Partial AI/block coverage. | Double battle with pooled partner Battery/Power Spot/Victory Star/Dazzling source differing from target. |
| Status blockers | Some AI/status coverage exists. | Pooled blocker with unrelated native ability plus popup/record assertion. |
| Hardcoded contact callbacks | Some pooled contact abilities covered. | Cases where ability owner, damaged target, and active attacker are all distinct. |
| Recoil/item backlash | Life Orb Magic Guard/Sheer Force covered. | Rocky Helmet, Jaboca/Rowap, crash/recoil, and shared Magic Guard variants. |
| End-turn ownership | Poison Heal/Liquid Ooze popups covered. | Leech Seed and Yawn source/target cases once contracts are decided. |
| Native mutation | Not intended to migrate. | Tests that copied/overwritten abilities operate on native slots and restore Shared Power state. |
| AI predictions | Some shared AI tests exist. | AI avoids pooled immunities/status blockers and chooses moves using pooled damage modifiers. |
| Shared Power off | Existing off-path checks are limited. | Every migrated class should include a Shared Power disabled bench-inert check when practical. |

## Reproducible Search Basis

Use these read-only searches to refresh the inventory:

```sh
rg -n "GetBattlerAbility\\(|GetBattlerAbilityIgnoreMoldBreaker\\(|gBattleMons\\[[^]]+\\]\\.ability" src include
rg -n "abilityAtk|abilityDef|battlerAbility|targetAbility|gLastUsedAbility|gBattlerAbility" src include
rg -n "HasActiveAbility\\(|ForEachEffectiveAbility|IsAbilityOn(Field|Side|FieldExcept)|CanAbility(Block|Absorb)|IsMoveEffectBlockedByTarget" src include
rg -n "gBattlerTarget|gEffectBattler|gBattlerAttacker" src/battle_util.c src/battle_script_commands.c src/battle_end_turn.c
```

Treat search output as a candidate list, not a decision. Final classification must come from the surrounding mechanic and the intended ability owner.
