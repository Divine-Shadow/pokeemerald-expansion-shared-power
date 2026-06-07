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
| `AB_NATIVE_ONLY_FORM_OR_SPECIES_GATE` | Stay except explicit shareable cases | Species/form identity checks that stay native-only unless the ability is explicitly allowed to share. |
| `AB_AI_CACHED_PREDICTION` | Clarify | AI known/predicted ability state, not necessarily live Shared Power truth. |

## Decision Rules

- Mark **stay** when the code initializes, mutates, copies, swaps, suppresses, stores, restores, or displays the native ability slot.
- Mark **migrate** when the code asks whether a battler benefits from, is protected by, or is blocked by an ability under current battle rules.
- Mark **iterate** when multiple effective abilities can participate in one calculation or phase.
- Mark **clarify** when source identity, affected battler, popup battler, holder semantics, duplicate behavior, AI knowledge, or form/species policy changes the answer.
- For hardcoded globals, classify by intended roles: ability source, active battler, attacker, target, effect battler, damaged battler, popup battler.

## Clarified Decisions

- Partner auras are ally- or side-owned passive modifiers that affect another active Pokemon, such as Battery, Power Spot, Steely Spirit, Flower Gift, Friend Guard, and Victory Star.
- Partner blockers are ally- or side-owned prevention effects, such as Dazzling, Queenly Majesty, Armor Tail, and partner Magic Bounce-style protection. Migrated blockers need both the triggering ability and the blocking battler for popup and record attribution.
- Native mutation/copy effects stay native-slot based. Trace, Role Play, Skill Swap, Doodle, Entrainment, and similar effects copy, suppress, or overwrite one native ability, not every ability in the Shared Power pool.
- Form/species-gated transformation or shield mechanics are native-only by default. Ice Face, Disguise, Battle Bond, Gulp Missile, and similar gates should not be activated by donated abilities. Imposter is explicitly fine to share.
- Damage-context ambiguity is about whether `DamageContext.abilityAtk/abilityDef` are authoritative selected abilities or whether a helper may re-query live effective abilities. This includes Mold Breaker and Unaware-style paths: if the context says target abilities are ignored or a specific ability was selected, a nested live `HasActiveAbility` call can accidentally reintroduce a different pooled ability.
- AI should reason over the full current Shared Power pool, not only one native or revealed ability.
- Pooled priority modifiers should stack additively. A single selected priority ability is not enough for Prankster, Triage, Gale Wings, and similar priority deltas under Shared Power.

## Inventory

| Area | Callsite | Label | Decision | Rationale |
| --- | --- | --- | --- | --- |
| Shared Power core | [`HasActiveAbility`, `ForEachEffectiveAbility*`](../../../src/battle_shared_power.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Stay | These are the intended abstraction for native plus pool queries. |
| Shared Power core | temporary ability override in popup/switch-in dispatch | `AB_MUTATION_NATIVE_SLOT`, `AB_POPUP_ATTRIBUTION` | Stay | This intentionally shims vanilla scripts; do not replace with generic effective checks. |
| Field/side helpers | [`IsAbilityOnSide`, `IsAbilityOnField`, `IsAbilityOnFieldExcept`](../../../src/battle_util.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Stay | Already centralizes field/side checks through active ability membership. |
| Move blocking | [`CanAbilityBlockMove`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Implemented | Live trigger checks use effective ability membership for target and partner blockers while preserving explicit supplied-ability behavior for AI checks. |
| Move absorption | [`CanAbilityAbsorbMove`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Stay, audit callers | Cleaner explicit-ability helper; callers must pass the selected effective ability and attribution owner. |
| Move redirection | Lightning Rod and Storm Drain target redirection in `HandleMoveTargetRedirection` / `GetBattleMoveTarget` | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_RECORD_ATTRIBUTION` | Implemented | Redirection/absorption is an active battle benefit; target exclusion, redirector search, redirect flags, and direct absorption now use active membership and record the triggering ability constant. |
| Screen and Substitute bypass | Infiltrator bypassing Substitute and Mist stat-loss protection | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_RECORD_ATTRIBUTION` | Implemented | Infiltrator is an attacker-owned active bypass for protective battle state; Substitute and Mist bypasses use active membership and record Infiltrator when pooled. |
| Accuracy skip | [`CanMoveSkipAccuracyCalc`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Implemented | The helper now honors supplied No Guard abilities for AI checks and uses active ability membership for live battle-script checks. |
| Accuracy math | [`GetTotalAccuracy`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Mostly uses supplied abilities, but ally Victory Star still uses native partner ability. |
| Type effectiveness | [`CalcTypeEffectivenessMultiplierInternal`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify contract | Uses `DamageContext`, but some immunity checks use `HasActiveAbility`; define when context-selected abilities, Mold Breaker suppression, and live pooled immunities are authoritative. |
| Damage calc entry | [`CalculateMoveDamage` / `CalculateMoveDamageVars`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify contract | Live calc refreshes native abilities; AI calc expects prefilled context. Shared Power damage paths need an explicit rule for context-selected abilities versus iterating the full pool. |
| Damage modifiers | base power, attack, defense, final damage callbacks | `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Stay | Existing unique effective iteration is the right model for passive numeric modifiers. |
| Partner damage auras | Battery, Power Spot, Steely Spirit, Flower Gift, Friend Guard, Victory Star | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Partner base-power, Flower Gift stat, Friend Guard, and Victory Star accuracy auras use active ally/side effective membership while preserving their existing aura conditions. |
| Contact helpers | [`IsMoveMakingContact`, `CanBattlerAvoidContactEffects`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Callers sometimes pass `ABILITY_NONE` deliberately; define raw contact vs contact after active ability modifiers. |
| Protect bypass | Unseen Fist contact move bypass in `IsBattlerProtected` | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | The attacker-owned protection bypass uses active Unseen Fist membership while preserving the existing contact-helper contract. |
| Status helpers | [`CanSetNonVolatileStatus`, `CanBe*`](../../../src/battle_util.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify | Callers supply abilities, while helper mutates `battlerDef` for side blockers and uses move globals. |
| Move-end iteration | [`SharedPower_GetNextMoveEndAbility`](../../../src/battle_util.c) | `AB_EFFECTIVE_SHARED_ITERATION_EVENT` | Stay, audit callbacks | Iteration is shared-aware, but callbacks use global target/attacker state. |
| Move-end target callbacks | [`TryMoveEndAbilityEffect`](../../../src/battle_util.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify | Some cases use `battler`, others hardcode `gBattlerTarget` or mutate native ability. |
| Move-end contact infatuation | Cute Charm blocked by attacker's Oblivious | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Cute Charm remains owned by the damaged target callback, while active Oblivious on the contact attacker blocks the infatuation. |
| Move-end contact stat reflection | Gooey/Tangling Hair allowance for attacker's Mirror Armor | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Gooey and Tangling Hair remain owned by the damaged target callback, while active Mirror Armor on the contact attacker allows and reflects the Speed drop even when the attacker is already at minimum Speed. |
| Move-end attacker callbacks | [`TryMoveEndAttackerAbilityEffect`](../../../src/battle_util.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify | Poison Touch/Toxic Chain consult target native ability and contact globals while executing pooled attacker abilities. |
| Knock Off berry cure | Toxic Chain allowing Pecha/Lum Berry cure before Knock Off removal | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Item ownership stays with the poisoned target, and the attacker-owned Toxic Chain interaction uses the attacker's active ability membership before Knock Off removes the cure berry. |
| Native mutation effects | Mummy, Lingering Aroma, Wandering Spirit, Trace state | `AB_MUTATION_NATIVE_SLOT` | Stay | These mutate or store native ability slots. Trace and similar copy effects should select one native ability, not the full donated pool. |
| Form/species gates | Imposter, Ice Face, Disguise, Battle Bond, Gulp Missile, form-change tables | `AB_NATIVE_ONLY_FORM_OR_SPECIES_GATE` | Mixed | Imposter may share. Ice Face, Disguise, Battle Bond, Gulp Missile, and similar form-locked mechanics stay native-only. |
| Powder check | [`IsMovePowderBlocked`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Implemented | Powder blocking uses active Overcoat membership for the actual defender and routes pooled Overcoat to the Overcoat-specific script branch. |
| Attack canceler | [`Cmd_attackcanceler`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Implemented | The top-level canceler path routes live block/absorb checks through active-aware helpers, and Magic Bounce partner ownership uses effective membership. |
| Accuracy command | [`AccuracyCheck`](../../../src/battle_script_commands.c) | `AB_HELPER_SUPPLIED_TARGET_STATE` | Implemented | The live accuracy command uses active-aware ability membership for hit/skip/math checks while leaving AI supplied-ability semantics intact. |
| Move effect application | [`SetMoveEffect`](../../../src/battle_script_commands.c) | `AB_CLARIFY_HARDCODED_TARGET_STATE` | Clarify, then migrate | Has `battler/effectBattler` args but still relies on global target/attacker in several branches. |
| Standard confusion prevention | `CanBeConfused`, Berserk Gene, and Own Tempo script blockers | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Active Own Tempo blocks standard confusion from moves and Berserk Gene while preserving the protected battler as popup owner; Sky Drop release/cancel confusion remains a separate hardcoded-state review. |
| Stat buffs | [`ChangeStatBuffs`](../../../src/battle_script_commands.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Mostly stay | Recent fixes select active stat-loss prevention abilities; Flower Veil source attribution still deserves review. |
| Move-end move block | Knock Off, Sticky Hold, Gorilla Tactics handling | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Sticky Hold item protection and Gorilla Tactics choice-lock checks use active membership and pooled popup attribution where applicable. |
| Move-end forced switch prevention | Guard Dog blocking Red Card forced switch | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Red Card still belongs to the damaged holder, while active Guard Dog on the attacker prevents that attacker from being forced out. |
| Holder berry effects | Ripen doubling for holder-consumed and holder-triggered berry effects | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Item ownership stays with the holder, while effective Ripen membership doubles eligible berry effects and shows shared Ripen popups when pooled. |
| Holder berry effects | Cheek Pouch after holder consumes a berry | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Item ownership stays with the berry consumer, while effective Cheek Pouch membership triggers the extra heal and shows Cheek Pouch for the holder when pooled. |
| Holder item suppression | Klutz disabling held-item effects, Fling, Poltergeist, and AI item-enabled checks | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_AI_CACHED_PREDICTION` | Implemented | Klutz is an active ability drawback/benefit that disables the holder's item behavior; live checks use active membership, while AI routes through its existing active-ability adapter. |
| Drain move backlash | Liquid Ooze on absorb moves and Strength Sap | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | The drained target owns Liquid Ooze for direct drain moves; active membership turns healing into damage while Leech Seed end-turn ownership remains a separate clarified contract. |
| Symbiosis | `TrySymbiosis`, `BS_TrySymbiosis` | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_POPUP_ATTRIBUTION` | Implemented | Activation and `gLastUsedAbility` represent shared Symbiosis when pooled, while the active partner remains the item donor. |
| Party status heal | `Cmd_healpartystatus` Soundproof checks | `AB_EFFECTIVE_SHARED_MEMBERSHIP`, `AB_RECORD_ATTRIBUTION` | Implemented | Shared Soundproof blocks active battler Heal Bell cures and records as `ABILITY_SOUNDPROOF`, not native holder ability. |
| Party wake-up items | `BS_CheckPokeFlute` active Soundproof checks | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Poké Flute respects active Soundproof for active battlers through Shared Power, while inactive party wake-up remains species/native party ability based. |
| Infatuation | `BS_TrySetInfatuation` | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Infatuation script gates use active Oblivious membership instead of direct native checks. |
| Last-used ability command | `BS_SetLastUsedAbility` | `AB_POPUP_ATTRIBUTION` | Clarify | Native target ability is correct only for scripts that explicitly want native identity. |
| Ability mutation commands | Transform, Role Play, Doodle, Gastro Acid, Skill Swap, Worry Seed, Simple Beam, Entrainment, Trace, Receiver | `AB_MUTATION_NATIVE_SLOT` | Stay | These intentionally copy, suppress, or overwrite native ability state. |
| Neutralizing Gas script checks | faint, switch-in, switch-out, mutation paths | `AB_NATIVE_SUPPRESSED_SINGLE` | Clarify policy | Shared Power treats Neutralizing Gas specially; native-only checks may be intentional but need policy text. |
| Ability popups | `BS_ShowAbilityPopup`, `GetDisplayedAbilityForMessage`, `UpdateAbilityPopup` | `AB_POPUP_ATTRIBUTION` | Stay | Central arbitration already considers Shared Power popup state and `gLastUsedAbility`. |
| End-turn iterators | weather, first/third/fourth blocks, ability block | `AB_EFFECTIVE_SHARED_ITERATION_EVENT` | Stay, audit filters | Current filters explicitly select end-turn abilities; review missing entries and duplicate behavior. |
| End-turn passive damage | weather damage, poison, burn, nightmare, curse, leech seed, Yawn | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Mixed | Many use `HasActiveAbility`; Leech Seed/Yawn still need source/target contract clarification. |
| Speed calc | [`GetBattlerTotalSpeedStatArgs`](../../../src/battle_main.c) | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Weather speed, Quick Feet, Surge Surfer, Slow Start, Protosynthesis, Quark Drive, and Unburden use active ability membership while preserving their existing activation-state gates. |
| Priority modifiers | Prankster, Triage, Gale Wings, similar priority deltas | `AB_EFFECTIVE_SHARED_ITERATION_UNIQUE` | Implemented | Pooled priority modifiers sum matching effective priority modifiers instead of selecting only one ability. |
| Order tiebreakers | Stall, Mycelium Might, Quick Draw, Custap-style first/last flags | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Clarify separately | These do not add priority stages; audit them as within-bracket ordering effects after additive priority is defined. |
| Dynamic move type | Ate abilities, Liquid Voice, Normalize | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Clarify | Type conversion can have source/order implications; do not migrate blindly. |
| Run Away/trapping | run checks and displayed trap ability | `AB_RECORD_ATTRIBUTION` | Clarify / defer | Run Away is active-aware. Do not migrate Shadow Tag, Arena Trap, or Magnet Pull trapping until policy decides whether donated trapping abilities contribute to escape prevention, whether Shadow Tag's Gen4+ self-exemption uses the trapped battler's active membership, and how the trapping source/display ability should be attributed when the native trapper ability differs. |
| Terastal STAB | Adaptability in terastal damage path | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Terastal same-type attack bonus now checks active Adaptability membership, matching the normal STAB path. |
| Sleep decrement | Early Bird in `battle_util2.c` | `AB_EFFECTIVE_SHARED_MEMBERSHIP` | Implemented | Passive sleep duration modifiers include pooled Early Bird in normal and Battle Palace sleep-decrement paths. |
| Player preview | effectiveness preview damage context | `AB_HELPER_SUPPLIED_TARGET_STATE` | Clarify / defer | The preview path already reaches active-aware type-effectiveness checks for abilities such as Levitate, Wonder Guard, Telepathy, Scrappy, and Mind's Eye. Do not migrate further until the damage-context contract decides whether preview should honor selected `DamageContext.abilityAtk/Def`, re-query live effective abilities, and apply dynamic move-type abilities such as Ate effects, Liquid Voice, and Normalize. |
| AI wrappers | `AI_HasActiveAbility`, `AI_CanAbilityBlockMove`, `AI_CanAbilityAbsorbMove`, AI status helpers | `AB_AI_CACHED_PREDICTION` | Stay | These are the current Shared Power-aware AI adapters. |
| AI damage usability | `IsDamageMoveUnusable`, `AI_CalcDamage` | `AB_AI_CACHED_PREDICTION` | Clarify | Some paths use `DamageContext`, others reread `gAiLogicData`; align contracts after live helper decisions. |
| AI known ability | `AI_DecideKnownAbilityForTurn`, switch prediction, switch items | `AB_AI_CACHED_PREDICTION` | In progress | AI should reason over the current Shared Power pool through active-aware adapters. Several scoring and switch paths are migrated, including status blockers, move blockers, Klutz item checks, Sticky Hold item-loss checks, hazard Magic Bounce, Wonder Guard smart switching and bad-move scoring, active secondary-damage prediction for Magic Guard and Poison Heal, direct-drain Liquid Ooze prediction, Substitute/Shed Tail Infiltrator prediction, Magnitude Levitate prediction, Lock-On No Guard prediction, Laser Focus crit-blocker prediction, Black Sludge Trick Magic Guard prediction, Prankster-into-Dark status prediction, guaranteed flinch prediction for Inner Focus and Shield Dust, active Heal Bell Soundproof prediction, active Comatose sleep-state prediction, active Overcoat powder prediction, active Perish Song Soundproof prediction, active Roar Soundproof prediction, active Electric-absorption self-benefit prediction for Ion Deluge and Electrify, active stat-loss blocker prediction for Spicy Extract and stat-drop scoring, active weather/terrain benefit prediction for shareable ability heuristics, and active ally-targeting HP-aware prediction for absorption abilities; remaining direct `gAiLogicData->abilities[]` callsites still need per-mechanic review. |

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
| High | Partner/side ability ownership | Partner auras and blockers should use the relevant ally or side effective pool, with explicit popup and record source attribution. |
| High | Popup and record attribution | Use triggering ability constants for migrated behavior; do not record native slot by default. |
| High | Native mutation/copy mechanics | Keep native-only; donated abilities are not copied, suppressed, or overwritten as a pool. |
| Medium | Form/species ability gates | Share Imposter; keep Ice Face, Disguise, Battle Bond, Gulp Missile, and similar form-locked gates native-only. |
| Medium | Damage context semantics | Define when `DamageContext.abilityAtk/Def`, Mold Breaker suppression, and Unaware-style decisions are authoritative versus live effective ability queries. |
| Medium | Duplicate source behavior | Keep unique iteration unless the underlying vanilla event intentionally triggers per source. |
| Low | UI previews | Align move previews after live battle semantics are settled. |

## Test Gaps

| Behavior class | Current state | Future regression shape |
| --- | --- | --- |
| Pooled speed/weather | Quick Feet and Swift Swim covered. | Add remaining speed modifiers only after deciding shareability. |
| Pooled priority/order | Not broadly covered. | Tests for stacked Prankster/Triage/Gale Wings priority, plus separate Stall/Mycelium Might order if migrated. |
| Partner auras/blockers | Partial AI/block coverage. | Double battle with pooled partner Battery/Power Spot/Victory Star/Dazzling source differing from target and popup owner. |
| Status blockers | Some AI/status coverage exists. | Pooled blocker with unrelated native ability plus popup/record assertion. |
| Hardcoded contact callbacks | Some pooled contact abilities covered. | Cases where ability owner, damaged target, and active attacker are all distinct. |
| Recoil/item backlash | Life Orb Magic Guard/Sheer Force covered. | Rocky Helmet, Jaboca/Rowap, crash/recoil, and shared Magic Guard variants. |
| End-turn ownership | Poison Heal/Liquid Ooze popups covered. | Leech Seed and Yawn source/target cases once contracts are decided. |
| Native mutation | Not intended to migrate. | Tests that copied/overwritten abilities operate on native slots and restore Shared Power state. |
| AI predictions | Some shared AI tests exist. | AI avoids pooled immunities/status blockers and chooses moves using the full current Shared Power pool. |
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
