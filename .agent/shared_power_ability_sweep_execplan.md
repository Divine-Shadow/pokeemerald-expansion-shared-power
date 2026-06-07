# Complete the Shared Power ability-callsite sweep

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md` in this repository. It is self-contained for the broader Shared Power ability-callsite sweep driven by `docs/design/shared_power/ability_usage_audit.md`.

## Purpose / Big Picture

Shared Power battle behavior should use the correct ability view at each callsite: native current-slot ability for mutation/copy/suppression/form-gate mechanics, effective Shared Power membership for shareable yes/no checks, and ordered or unique iteration for shareable multi-ability effects. At the end of this sweep, every audit row marked migrate is either implemented with focused tests and validation evidence, or explicitly deferred with a documented policy question. A player should see Shared Power mechanics behave consistently without native-only mechanics such as Trace, Skill Swap, Gastro Acid, or form gates accidentally treating the shared pool as a mutable ability slot.

## Progress

- [x] (2026-06-06T21:31Z) Completed the pooled priority modifier tranche in `.agent/shared_power_priority_execplan.md`: Prankster, Gale Wings, and Triage now use effective ability membership and stack additively under Shared Power.
- [x] (2026-06-06T21:48Z) Created this broader sweep ExecPlan before starting the next audit bucket.
- [x] (2026-06-06T22:03Z) Implemented the powder-move Overcoat blocker bucket from the audit's `IsMovePowderBlocked` migrate row.
- [x] (2026-06-06T22:09Z) Ran targeted validation for the powder-move Overcoat bucket and recorded evidence here.
- [x] (2026-06-06T22:31Z) Implemented the move-blocking helper bucket from the audit's `CanAbilityBlockMove` migrate row.
- [x] (2026-06-06T22:40Z) Ran targeted validation for the move-blocking helper bucket and recorded evidence here.
- [x] (2026-06-06T22:51Z) Implemented the Early Bird sleep-decrement bucket from the audit's `battle_util2.c` migrate row, including the matching normal battle attack-canceler path.
- [x] (2026-06-06T22:57Z) Ran targeted validation for the Early Bird sleep-decrement bucket and recorded evidence here.
- [x] (2026-06-06T23:18Z) Implemented the speed-calculation remainder bucket from the audit's `GetBattlerTotalSpeedStatArgs` migrate row.
- [x] (2026-06-06T23:23Z) Ran targeted validation for the speed-calculation remainder bucket and recorded evidence here.
- [x] (2026-06-06T23:35Z) Selected the infatuation/Oblivious script-command bucket from the audit's `BS_TrySetInfatuation` migrate row.
- [x] (2026-06-06T23:45Z) Implemented the infatuation/Oblivious script-command bucket.
- [x] (2026-06-06T23:55Z) Ran targeted validation for the infatuation/Oblivious script-command bucket and recorded evidence here.
- [x] (2026-06-07T00:05Z) Selected the party-status Heal Bell Soundproof bucket from the audit's `Cmd_healpartystatus` migrate row.
- [x] (2026-06-07T00:15Z) Implemented the party-status Heal Bell Soundproof bucket.
- [x] (2026-06-07T00:25Z) Ran targeted validation for the party-status Heal Bell Soundproof bucket and recorded evidence here.
- [x] (2026-06-07T00:40Z) Selected the accuracy-skip No Guard contract bucket from the audit's `CanMoveSkipAccuracyCalc` migrate-contract row.
- [x] (2026-06-07T00:50Z) Implemented the accuracy-skip No Guard contract bucket.
- [x] (2026-06-07T01:00Z) Ran targeted validation for the accuracy-skip No Guard contract bucket and recorded evidence here.
- [x] (2026-06-07T01:08Z) Selected the Terastal Adaptability audit row and began focused regression coverage.
- [x] (2026-06-07T01:16Z) Added focused Terastal Adaptability Shared Power regression coverage.
- [x] (2026-06-07T01:24Z) Migrated `GetTeraMultiplier` Adaptability to active ability membership after the regression exposed the native-only lookup.
- [x] (2026-06-07T01:34Z) Ran targeted validation for the Terastal Adaptability row and recorded evidence here.
- [x] (2026-06-07T01:48Z) Selected the partner base-power aura bucket for Battery, Power Spot, and Steely Spirit from the partner damage auras migrate row.
- [x] (2026-06-07T02:02Z) Implemented the partner base-power aura bucket.
- [x] (2026-06-07T02:28Z) Ran targeted validation for the partner base-power aura bucket and recorded evidence here.
- [x] (2026-06-07T02:40Z) Selected the Symbiosis bucket from the audit's `TrySymbiosis` / `BS_TrySymbiosis` migrate row.
- [x] (2026-06-07T02:50Z) Implemented the Symbiosis bucket.
- [x] (2026-06-07T03:00Z) Ran targeted validation for the Symbiosis bucket and recorded evidence here.
- [x] (2026-06-07T03:12Z) Selected the Sticky Hold item-protection bucket from the audit's move-end move block row, leaving Gorilla Tactics for a separate choice-lock bucket.
- [x] (2026-06-07T03:25Z) Implemented the Sticky Hold item-protection bucket.
- [x] (2026-06-07T03:38Z) Ran targeted validation for the Sticky Hold item-protection bucket and recorded evidence here.
- [x] (2026-06-07T03:45Z) Selected the Gorilla Tactics choice-lock bucket from the audit's move-end move block row.
- [x] (2026-06-07T03:54Z) Implemented the Gorilla Tactics choice-lock bucket.
- [x] (2026-06-07T04:05Z) Ran targeted validation for the Gorilla Tactics choice-lock bucket and recorded evidence here.
- [x] (2026-06-07T04:14Z) Selected the Flower Gift partner aura bucket from the remaining partner damage auras row.
- [x] (2026-06-07T04:24Z) Implemented the Flower Gift partner aura bucket.
- [x] (2026-06-07T04:35Z) Ran targeted validation for the Flower Gift partner aura bucket and recorded evidence here.
- [x] (2026-06-07T04:42Z) Selected the Victory Star partner accuracy aura bucket from the remaining partner aura and accuracy-math rows.
- [x] (2026-06-07T04:50Z) Implemented the Victory Star partner accuracy aura bucket.
- [x] (2026-06-07T04:58Z) Ran targeted validation for the Victory Star partner accuracy aura bucket and recorded evidence here.
- [x] (2026-06-07T05:08Z) Selected the attack-canceler Magic Bounce ownership bucket from the audit's `Cmd_attackcanceler` migrate row.
- [x] (2026-06-07T05:16Z) Implemented the attack-canceler Magic Bounce ownership bucket.
- [x] (2026-06-07T05:24Z) Corrected pooled Magic Bounce popup attribution after the first targeted validation showed the reflection owner lacked a Magic Bounce popup.
- [x] (2026-06-07T05:31Z) Ran targeted validation for the attack-canceler Magic Bounce ownership bucket and recorded evidence here.
- [x] (2026-06-07T05:43Z) Selected the live `AccuracyCheck` accuracy-math bucket from the audit's accuracy command migrate row.
- [x] (2026-06-07T05:51Z) Implemented the live `AccuracyCheck` accuracy-math bucket.
- [x] (2026-06-07T06:06Z) Ran targeted validation for the live `AccuracyCheck` accuracy-math bucket and recorded evidence here.
- [x] (2026-06-07T06:18Z) Selected the `SetMoveEffect` Flame Burst Magic Guard bucket from the audit's move effect application row.
- [x] (2026-06-07T06:25Z) Implemented the `SetMoveEffect` Flame Burst Magic Guard bucket.
- [x] (2026-06-07T06:33Z) Ran targeted validation for the `SetMoveEffect` Flame Burst Magic Guard bucket and recorded evidence here.
- [x] (2026-06-07T06:45Z) Selected the AI hazard setup ability-awareness bucket from the audit's AI known-ability row, scoped away from broad hidden-information rewrites.
- [x] (2026-06-07T06:52Z) Implemented the AI hazard setup ability-awareness bucket.
- [x] (2026-06-07T07:18Z) Ran targeted validation for the AI hazard setup ability-awareness bucket and recorded evidence here.
- [x] (2026-06-07T07:32Z) Selected the `SetMoveEffect` flinch Inner Focus bucket, leaving broader status helpers, contact callbacks, and recoil/item backlash for separate ownership decisions.
- [x] (2026-06-07T07:40Z) Implemented the `SetMoveEffect` flinch Inner Focus bucket.
- [x] (2026-06-07T07:46Z) Ran targeted validation for the `SetMoveEffect` flinch Inner Focus bucket and recorded evidence here.
- [x] (2026-06-07T07:58Z) Selected the held-item backlash Magic Guard bucket for Rocky Helmet, Jaboca Berry, and Rowap Berry.
- [x] (2026-06-07T08:08Z) Implemented the held-item backlash Magic Guard bucket.
- [x] (2026-06-07T08:14Z) Ran targeted validation for the held-item backlash Magic Guard bucket and recorded evidence here.
- [x] (2026-06-07T08:24Z) Selected the half-HP user-loss Magic Guard bucket for `EFFECT_MAX_HP_50_RECOIL` moves such as Steel Beam and Mind Blown.
- [x] (2026-06-07T08:31Z) Implemented the half-HP user-loss Magic Guard bucket.
- [x] (2026-06-07T08:36Z) Ran targeted validation for the half-HP user-loss Magic Guard bucket and recorded evidence here.
- [x] (2026-06-07T08:46Z) Selected the Spiky Shield Magic Guard bucket for attacker-side contact protect backlash.
- [x] (2026-06-07T08:52Z) Implemented the Spiky Shield Magic Guard bucket.
- [x] (2026-06-07T08:57Z) Ran targeted validation for the Spiky Shield Magic Guard bucket and recorded evidence here.
- [x] (2026-06-07T09:06Z) Selected the holder-side item damage Magic Guard bucket for Black Sludge and Sticky Barb.
- [x] (2026-06-06T23:45Z) Implemented the holder-side item damage Magic Guard bucket.
- [x] (2026-06-06T23:45Z) Ran targeted validation for the holder-side item damage Magic Guard bucket and recorded evidence here.
- [x] (2026-06-06T23:48Z) Selected the entry-hazard damage Magic Guard bucket for Spikes, Stealth Rock, and G-Max Steelsurge damage.
- [x] (2026-06-06T23:52Z) Implemented the entry-hazard damage Magic Guard bucket.
- [x] (2026-06-06T23:55Z) Ran targeted validation for the entry-hazard damage Magic Guard bucket and recorded evidence here.
- [x] (2026-06-06T23:52Z) Selected the Powder self-damage Magic Guard bucket.
- [x] (2026-06-06T23:52Z) Implemented the Powder self-damage Magic Guard bucket.
- [x] (2026-06-06T23:52Z) Ran targeted validation for the Powder self-damage Magic Guard bucket and recorded evidence here.
- [x] (2026-06-06T23:52Z) Selected the Taunt Oblivious target-protection bucket.
- [x] (2026-06-06T23:52Z) Implemented the Taunt Oblivious target-protection bucket.
- [x] (2026-06-06T23:52Z) Ran targeted validation for the Taunt Oblivious target-protection bucket and recorded evidence here.
- [x] (2026-06-06T23:59Z) Selected the Pressure PP deduction bucket.
- [x] (2026-06-06T23:59Z) Implemented the Pressure PP deduction bucket.
- [x] (2026-06-06T23:59Z) Ran targeted validation for the Pressure PP deduction bucket and recorded evidence here.
- [x] (2026-06-06T23:59Z) Selected the generic `jumpifblockedbysoundproof` script-command bucket.
- [x] (2026-06-06T23:59Z) Implemented the generic `jumpifblockedbysoundproof` script-command bucket.
- [x] (2026-06-06T23:59Z) Ran targeted validation for the generic `jumpifblockedbysoundproof` script-command bucket and recorded evidence here.
- [x] (2026-06-07T00:05Z) Selected the Perish Song Soundproof setup bucket.
- [x] (2026-06-07T00:05Z) Implemented the Perish Song Soundproof setup bucket.
- [x] (2026-06-07T00:05Z) Ran targeted validation for the Perish Song Soundproof setup bucket and recorded evidence here.
- [x] (2026-06-07T00:05Z) Selected the Gluttony berry-threshold bucket.
- [x] (2026-06-07T00:05Z) Implemented the Gluttony berry-threshold bucket.
- [x] (2026-06-07T00:05Z) Ran targeted validation for the Gluttony berry-threshold bucket and recorded evidence here.
- [x] (2026-06-07T00:10Z) Selected the stat-boosting Ripen berry bucket.
- [x] (2026-06-07T00:10Z) Implemented the stat-boosting Ripen berry bucket.
- [x] (2026-06-07T00:10Z) Ran targeted validation for the stat-boosting Ripen berry bucket and recorded evidence here.
- [x] (2026-06-07T00:13Z) Selected the standard HP-restoring Ripen berry bucket.
- [x] (2026-06-07T00:13Z) Implemented the standard HP-restoring Ripen berry bucket.
- [x] (2026-06-07T00:13Z) Ran targeted validation for the standard HP-restoring Ripen berry bucket and recorded evidence here.
- [x] (2026-06-07T00:30Z) Selected the remaining Ripen berry-effect bucket for confusion berries, Enigma Berry, Leppa Berry, and Jaboca/Rowap Berry damage.
- [x] (2026-06-07T00:42Z) Implemented the remaining Ripen berry-effect bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:05Z) Ran targeted validation for the remaining Ripen berry-effect bucket and recorded evidence here.
- [x] (2026-06-07T01:12Z) Selected the Heal Pulse Mega Launcher bucket from the remaining pulse-move native attacker check.
- [x] (2026-06-07T01:18Z) Implemented the Heal Pulse Mega Launcher bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:25Z) Ran targeted validation for the Heal Pulse Mega Launcher bucket and recorded evidence here.
- [x] (2026-06-07T01:32Z) Selected the Cheek Pouch holder berry-effect bucket from the remaining native berry-consumer check.
- [x] (2026-06-07T01:40Z) Implemented the Cheek Pouch holder berry-effect bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:48Z) Ran targeted validation for the Cheek Pouch holder berry-effect bucket and recorded evidence here.
- [x] (2026-06-07T01:55Z) Selected the direct drain move Liquid Ooze bucket for absorb-style moves and Strength Sap, leaving Leech Seed end-turn ownership separate.
- [x] (2026-06-07T02:05Z) Implemented the direct drain move Liquid Ooze bucket and added focused Shared Power coverage.
- [x] (2026-06-07T02:18Z) Ran targeted validation for the direct drain move Liquid Ooze bucket and recorded evidence here.
- [x] (2026-06-07T02:25Z) Selected the Guard Dog Red Card forced-switch prevention bucket.
- [x] (2026-06-07T02:32Z) Implemented the Guard Dog Red Card forced-switch prevention bucket and added focused Shared Power coverage.
- [x] (2026-06-07T02:40Z) Ran targeted validation for the Guard Dog Red Card bucket and recorded evidence here.
- [x] (2026-06-07T02:48Z) Selected the Cute Charm Oblivious contact-infatuation bucket.
- [x] (2026-06-07T02:55Z) Implemented the Cute Charm Oblivious contact-infatuation bucket and added focused Shared Power coverage.
- [x] (2026-06-07T03:04Z) Ran targeted validation for the Cute Charm Oblivious bucket and recorded evidence here.
- [x] (2026-06-07T00:59Z) Selected the move-end Mirror Armor contact-reflection bucket for Gooey and Tangling Hair.
- [x] (2026-06-07T00:59Z) Implemented the move-end Mirror Armor contact-reflection bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:06Z) Ran targeted validation for the move-end Mirror Armor contact-reflection bucket and recorded evidence here.
- [x] (2026-06-07T01:06Z) Ran `git diff --check` after the move-end Mirror Armor contact-reflection bucket; no issues reported.
- [x] (2026-06-07T01:18Z) Selected the Own Tempo standard-confusion bucket, leaving Sky Drop release/cancel confusion for separate hardcoded-state review.
- [x] (2026-06-07T01:25Z) Implemented the Own Tempo standard-confusion bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:33Z) Ran targeted validation for the Own Tempo standard-confusion bucket and recorded evidence here.
- [x] (2026-06-07T01:34Z) Ran `git diff --check` after the Own Tempo standard-confusion bucket; no issues reported.
- [x] (2026-06-07T01:13Z) Selected the Toxic Chain Knock Off berry-cure bucket, leaving broader Poison Touch/Toxic Chain callback ownership separate.
- [x] (2026-06-07T01:13Z) Implemented the Toxic Chain Knock Off berry-cure bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:13Z) Ran targeted validation for the Toxic Chain Knock Off berry-cure bucket and recorded evidence here.
- [x] (2026-06-07T01:13Z) Ran `git diff --check` after the Toxic Chain Knock Off berry-cure bucket; no issues reported.
- [x] (2026-06-07T01:13Z) Selected the Poké Flute active Soundproof bucket, keeping inactive party wake-up native/species-based.
- [x] (2026-06-07T01:13Z) Implemented the Poké Flute active Soundproof bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:13Z) Ran targeted validation for the Poké Flute active Soundproof bucket and recorded evidence here.
- [x] (2026-06-07T01:13Z) Ran `git diff --check` after the Poké Flute active Soundproof bucket; no issues reported.
- [x] (2026-06-07T01:25Z) Selected the Unseen Fist protect-bypass bucket, preserving the existing contact-helper contract.
- [x] (2026-06-07T01:25Z) Implemented the Unseen Fist protect-bypass bucket and added focused Shared Power coverage.
- [x] (2026-06-07T01:25Z) Ran targeted validation for the Unseen Fist protect-bypass bucket and recorded evidence here.
- [x] (2026-06-07T01:25Z) Ran `git diff --check` after the Unseen Fist protect-bypass bucket; no issues reported.
- [x] (2026-06-07T01:32Z) Selected the Lightning Rod and Storm Drain move-redirection bucket, scoped to active target exclusion, redirector search, and redirect attribution flags.
- [x] (2026-06-07T01:32Z) Implemented the Lightning Rod and Storm Drain move-redirection bucket.
- [x] (2026-06-07T01:32Z) Added focused Shared Power and disabled-path coverage for Lightning Rod and Storm Drain redirection.
- [x] (2026-06-07T01:32Z) Extended the bucket to live direct absorption callsites after validation showed selected pooled targets were no longer redirected but still did not absorb.
- [x] (2026-06-07T01:32Z) Ran targeted validation for the Lightning Rod and Storm Drain redirection/absorption bucket and recorded evidence here.
- [x] (2026-06-07T01:32Z) Ran `git diff --check` after the Lightning Rod and Storm Drain bucket; no issues reported.
- [x] (2026-06-07T01:47Z) Selected the Infiltrator bypass bucket for Substitute and Mist stat-loss protection, scoped as attacker-owned active bypass behavior.
- [x] (2026-06-07T01:47Z) Implemented the Infiltrator bypass bucket.
- [x] (2026-06-07T01:47Z) Added focused Shared Power and disabled-path coverage for Infiltrator bypassing Substitute and Mist.
- [x] (2026-06-07T01:47Z) Ran targeted validation for the Infiltrator bypass bucket and recorded evidence here.
- [x] (2026-06-07T01:47Z) Ran `git diff --check` after the Infiltrator bypass bucket; no issues reported.
- [x] (2026-06-07T01:53Z) Selected the Klutz item-suppression bucket, scoped to live held-effect suppression, Fling, Poltergeist, and AI item-enabled checks.
- [x] (2026-06-07T01:53Z) Implemented the Klutz item-suppression bucket.
- [x] (2026-06-07T01:53Z) Added focused Shared Power and disabled-path coverage for Klutz suppressing Leftovers, Fling, and Poltergeist.
- [x] (2026-06-07T01:53Z) Ran targeted validation for the Klutz item-suppression bucket and recorded evidence here.
- [x] (2026-06-07T01:53Z) Ran `git diff --check` after the Klutz item-suppression bucket; no issues reported.
- [x] (2026-06-07T02:02Z) Deferred the Run Away/trapping row pending a Shared Power trapping policy decision for Shadow Tag self-exemption and trap-source attribution.
- [x] (2026-06-07T02:02Z) Ran `git diff --check` after the Run Away/trapping defer bucket; no issues reported.
- [x] (2026-06-07T02:05Z) Deferred the player effectiveness preview row pending the broader `DamageContext` and dynamic move-type policy contract.
- [x] (2026-06-07T02:05Z) Ran `git diff --check` after the player effectiveness preview defer bucket; no issues reported.
- [x] (2026-06-07T02:07Z) Reconciled the audit decision column for previously implemented Shared Power buckets so remaining migrate rows are easier to identify.
- [x] (2026-06-07T02:07Z) Ran `git diff --check` after the audit decision reconciliation; no issues reported.
- [x] (2026-06-07T02:10Z) Selected the AI Wonder Guard switch-recognition bucket from the remaining AI known-ability migration row.
- [x] (2026-06-07T02:10Z) Implemented the AI Wonder Guard switch-recognition bucket and added focused Shared Power enabled/off coverage.
- [x] (2026-06-07T02:10Z) Ran targeted validation for the AI Wonder Guard switch-recognition bucket and recorded evidence here.
- [x] (2026-06-07T02:15Z) Selected the AI secondary-damage prediction bucket for active Magic Guard and Poison Heal membership.
- [x] (2026-06-07T02:15Z) Implemented the AI secondary-damage prediction bucket and added focused Shared Power enabled/off helper coverage.
- [x] (2026-06-07T02:15Z) Ran targeted validation for the AI secondary-damage prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:20Z) Selected the AI guaranteed-flinch prediction bucket for active Inner Focus and Shield Dust membership.
- [x] (2026-06-07T02:20Z) Implemented the AI guaranteed-flinch prediction bucket and added focused Shared Power enabled/off helper coverage.
- [x] (2026-06-07T02:20Z) Ran targeted validation for the AI guaranteed-flinch prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:20Z) Ran `git diff --check` after the AI guaranteed-flinch prediction bucket; no issues reported.
- [x] (2026-06-07T02:25Z) Selected the AI Heal Bell Soundproof prediction bucket for active battler Soundproof membership.
- [x] (2026-06-07T02:25Z) Implemented the AI Heal Bell Soundproof prediction bucket and added focused Shared Power enabled/off move-choice coverage.
- [x] (2026-06-07T02:25Z) Ran targeted validation for the AI Heal Bell Soundproof prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:25Z) Ran `git diff --check` after the AI Heal Bell Soundproof prediction bucket; no issues reported.
- [x] (2026-06-07T02:30Z) Selected the AI Comatose sleep-state prediction bucket for active battler membership.
- [x] (2026-06-07T02:30Z) Implemented the AI Comatose sleep-state prediction bucket and added focused Shared Power enabled/off helper coverage.
- [x] (2026-06-07T02:30Z) Ran targeted validation for the AI Comatose sleep-state prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:30Z) Ran `git diff --check` after the AI Comatose sleep-state prediction bucket; no issues reported.
- [x] (2026-06-07T02:35Z) Selected the AI powder-move Overcoat prediction bucket for active target membership.
- [x] (2026-06-07T02:35Z) Implemented the AI powder-move Overcoat prediction bucket and added focused Shared Power enabled/off move-choice coverage.
- [x] (2026-06-07T02:35Z) Ran targeted validation for the AI powder-move Overcoat prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:35Z) Ran `git diff --check` after the AI powder-move Overcoat prediction bucket; no issues reported.
- [x] (2026-06-07T02:40Z) Selected the AI Perish Song Soundproof prediction bucket for active battler membership.
- [x] (2026-06-07T02:40Z) Implemented the AI Perish Song Soundproof prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T02:40Z) Ran targeted validation for the AI Perish Song Soundproof prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:40Z) Ran `git diff --check` after the AI Perish Song Soundproof prediction bucket; no issues reported.
- [x] (2026-06-07T02:45Z) Selected the AI Roar Soundproof prediction bucket for active target membership.
- [x] (2026-06-07T02:45Z) Implemented the AI Roar Soundproof prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T02:45Z) Ran targeted validation for the AI Roar Soundproof prediction bucket and recorded evidence here.
- [x] (2026-06-07T02:45Z) Ran `git diff --check` after the AI Roar Soundproof prediction bucket; no issues reported.
- [x] (2026-06-07T12:20Z) Selected the AI Wonder Guard bad-move scoring bucket for active target membership.
- [x] (2026-06-07T12:25Z) Implemented the AI Wonder Guard bad-move scoring bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T12:40Z) Ran targeted validation for the AI Wonder Guard bad-move scoring bucket and recorded evidence here.
- [x] (2026-06-07T12:42Z) Ran `git diff --check` after the AI Wonder Guard bad-move scoring bucket; no issues reported.
- [x] (2026-06-07T13:05Z) Selected the AI Electric-absorption self-benefit prediction bucket for Ion Deluge and Electrify scoring.
- [x] (2026-06-07T13:12Z) Implemented the AI Electric-absorption self-benefit prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T13:28Z) Ran targeted validation for the AI Electric-absorption self-benefit prediction bucket and recorded evidence here.
- [x] (2026-06-07T13:30Z) Ran `git diff --check` after the AI Electric-absorption self-benefit prediction bucket; no issues reported.
- [x] (2026-06-07T13:45Z) Selected the AI stat-loss blocker prediction bucket for active Clear Body-style and specific stat-loss blockers, scoped away from broad status-helper policy.
- [x] (2026-06-07T13:52Z) Implemented the AI stat-loss blocker prediction bucket and added focused Shared Power enabled/off Spicy Extract coverage.
- [x] (2026-06-07T14:05Z) Ran targeted validation for the AI stat-loss blocker prediction bucket and recorded evidence here.
- [x] (2026-06-07T14:07Z) Ran `git diff --check` after the AI stat-loss blocker prediction bucket; no issues reported.
- [x] (2026-06-07T14:20Z) Selected the AI stat-drop-punish prediction bucket for active Contrary, Competitive, and Defiant viability scoring.
- [x] (2026-06-07T14:24Z) Implemented the AI stat-drop-punish prediction bucket and added focused Shared Power enabled/off Defiant scoring coverage.
- [x] (2026-06-07T14:42Z) Ran targeted validation for the AI stat-drop-punish prediction bucket and recorded evidence here.
- [x] (2026-06-07T14:43Z) Ran `git diff --check` after the AI stat-drop-punish prediction bucket; no issues reported.
- [x] (2026-06-07T15:05Z) Selected the AI Roar Suction Cups prediction bucket for active target phazing protection, scoped away from live Dragon Tail Suction Cups policy.
- [x] (2026-06-07T15:14Z) Implemented the AI Roar Suction Cups prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T15:24Z) Ran targeted validation for the AI Roar Suction Cups prediction bucket and recorded evidence here.
- [x] (2026-06-07T15:25Z) Ran `git diff --check` after the AI Roar Suction Cups prediction bucket; no issues reported.
- [x] (2026-06-07T15:40Z) Selected the AI Strength Sap Contrary prediction bucket for active target Contrary checks, scoped away from broader stat-up and status-helper policy.
- [x] (2026-06-07T15:48Z) Implemented the AI Strength Sap Contrary prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T15:55Z) Ran targeted validation for the AI Strength Sap Contrary prediction bucket and recorded evidence here.
- [x] (2026-06-07T15:56Z) Ran `git diff --check` after the AI Strength Sap Contrary prediction bucket; no issues reported.
- [x] (2026-06-07T16:05Z) Selected the AI multi-hit Rocky Helmet Magic Guard prediction bucket, mirroring the live held-item backlash Magic Guard migration.
- [x] (2026-06-07T16:12Z) Implemented the AI multi-hit Rocky Helmet Magic Guard prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T16:18Z) Ran targeted validation for the AI multi-hit Rocky Helmet Magic Guard prediction bucket and recorded evidence here.
- [x] (2026-06-07T16:19Z) Ran `git diff --check` after the AI multi-hit Rocky Helmet Magic Guard prediction bucket; no issues reported.
- [x] (2026-06-07T16:30Z) Selected the AI Rest fast-recovery prediction bucket for active Shed Skin, Early Bird, and rain Hydration membership.
- [x] (2026-06-07T16:38Z) Implemented the AI Rest fast-recovery prediction bucket and added focused Shared Power enabled/off coverage.
- [x] (2026-06-07T16:45Z) Ran targeted validation for the AI Rest fast-recovery prediction bucket and recorded evidence here.
- [x] (2026-06-07T16:46Z) Ran `git diff --check` after the AI Rest fast-recovery prediction bucket; no issues reported.
- [x] (2026-06-07T16:55Z) Selected the AI crit-setup prediction bucket for Focus Energy and Laser Focus with active Super Luck and Sniper membership.
- [x] (2026-06-07T17:02Z) Implemented the AI crit-setup prediction bucket and added focused Shared Power enabled/off coverage.
- [x] (2026-06-07T17:08Z) Ran targeted validation for the AI crit-setup prediction bucket and recorded evidence here.
- [x] (2026-06-07T17:09Z) Ran `git diff --check` after the AI crit-setup prediction bucket; no issues reported.
- [x] (2026-06-07T17:18Z) Selected the AI crash-recoil Magic Guard prediction bucket for low-accuracy `EFFECT_RECOIL_IF_MISS` moves.
- [x] (2026-06-07T17:25Z) Implemented the AI crash-recoil Magic Guard prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T17:31Z) Ran targeted validation for the AI crash-recoil Magic Guard prediction bucket and recorded evidence here.
- [x] (2026-06-07T17:32Z) Ran `git diff --check` after the AI crash-recoil Magic Guard prediction bucket; no issues reported.
- [x] (2026-06-07T17:40Z) Selected the AI Foresight Scrappy/Mind's Eye prediction bucket for active attacker membership.
- [x] (2026-06-07T17:46Z) Implemented the AI Foresight Scrappy/Mind's Eye prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T17:50Z) Ran targeted validation for the AI Foresight Scrappy/Mind's Eye prediction bucket and recorded evidence here.
- [x] (2026-06-07T17:51Z) Ran `git diff --check` after the AI Foresight Scrappy/Mind's Eye prediction bucket; no issues reported.
- [x] (2026-06-07T03:05Z) Selected the AI weather/terrain benefit prediction bucket, scoped to shareable active ability heuristics while keeping native-only form/species-style weather abilities native.
- [x] (2026-06-07T03:12Z) Implemented the AI weather/terrain benefit prediction bucket and added focused Shared Power enabled/off helper coverage for Rain and Electric Terrain.
- [x] (2026-06-07T03:20Z) Ran targeted validation for the AI weather/terrain benefit prediction bucket and recorded evidence here.
- [x] (2026-06-07T03:20Z) Ran `git diff --check` after the AI weather/terrain benefit prediction bucket; no issues reported.
- [x] (2026-06-07T03:28Z) Selected the AI ally absorption HP-aware prediction bucket for single-target partner healing via Volt Absorb, Earth Eater, Water Absorb, and Dry Skin.
- [x] (2026-06-07T03:35Z) Implemented the AI ally absorption HP-aware prediction bucket and added focused Shared Power enabled/off score coverage for pooled Volt Absorb.
- [x] (2026-06-07T03:45Z) Ran targeted validation for the AI ally absorption HP-aware prediction bucket and recorded evidence here.
- [x] (2026-06-07T03:45Z) Ran `git diff --check` after the AI ally absorption HP-aware prediction bucket; no issues reported.
- [x] (2026-06-07T03:52Z) Selected the AI Sticky Hold item-loss prediction bucket, scoped to active target item protection in Knock Off, theft, Bug Bite, Incinerate, and `CanKnockOffItem` checks.
- [x] (2026-06-07T04:02Z) Implemented the AI Sticky Hold item-loss prediction bucket and added focused Shared Power enabled/off helper coverage.
- [x] (2026-06-07T04:10Z) Ran targeted validation for the AI Sticky Hold item-loss prediction bucket and recorded evidence here.
- [x] (2026-06-07T04:10Z) Ran `git diff --check` after the AI Sticky Hold item-loss prediction bucket; no issues reported.
- [x] (2026-06-07T04:18Z) Selected the AI direct-drain Liquid Ooze prediction bucket for `EFFECT_ABSORB`, leaving Leech Seed prediction under the existing end-turn ownership clarification.
- [x] (2026-06-07T04:24Z) Implemented the AI direct-drain Liquid Ooze prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T04:32Z) Ran targeted validation for the AI direct-drain Liquid Ooze prediction bucket and recorded evidence here.
- [x] (2026-06-07T04:32Z) Ran `git diff --check` after the AI direct-drain Liquid Ooze prediction bucket; no issues reported.
- [x] (2026-06-07T04:40Z) Selected the AI Substitute/Shed Tail Infiltrator prediction bucket, following the already-migrated live Infiltrator bypass policy.
- [x] (2026-06-07T04:46Z) Implemented the AI Substitute/Shed Tail Infiltrator prediction bucket and added focused Shared Power enabled/off Substitute score coverage.
- [x] (2026-06-07T04:54Z) Ran targeted validation for the AI Substitute/Shed Tail Infiltrator prediction bucket and recorded evidence here.
- [x] (2026-06-07T04:54Z) Ran `git diff --check` after the AI Substitute/Shed Tail Infiltrator prediction bucket; no issues reported.
- [x] (2026-06-07T05:02Z) Selected the AI Magnitude Levitate prediction bucket, scoped to the bad-move heuristic for Ground immunity already handled by active ability checks in live battle.
- [x] (2026-06-07T05:08Z) Implemented the AI Magnitude Levitate prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T05:16Z) Ran targeted validation for the AI Magnitude Levitate prediction bucket and recorded evidence here.
- [x] (2026-06-07T05:16Z) Ran `git diff --check` after the AI Magnitude Levitate prediction bucket; no issues reported.
- [x] (2026-06-07T18:20Z) Selected the AI Lock-On and Laser Focus prediction bucket for already-live No Guard and crit-blocker ability semantics.
- [x] (2026-06-07T18:28Z) Implemented the AI Lock-On and Laser Focus prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T18:45Z) Ran targeted validation for the AI Lock-On and Laser Focus prediction bucket and recorded evidence here.
- [x] (2026-06-07T18:50Z) Ran `git diff --check` after the AI Lock-On and Laser Focus prediction bucket; no issues reported.
- [x] (2026-06-07T19:22Z) Selected the AI Black Sludge Trick Magic Guard prediction bucket, scoped to matching live active Magic Guard holder-damage prevention.
- [x] (2026-06-07T19:30Z) Implemented the AI Black Sludge Trick Magic Guard prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T19:42Z) Ran targeted validation for the AI Black Sludge Trick Magic Guard prediction bucket and recorded evidence here.
- [x] (2026-06-07T19:45Z) Ran `git diff --check` after the AI Black Sludge Trick Magic Guard prediction bucket; no issues reported.
- [x] (2026-06-07T20:05Z) Selected the AI Prankster Dark-target prediction bucket, scoped to the bad-move check matching live active Prankster priority blocking.
- [x] (2026-06-07T20:12Z) Implemented the AI Prankster Dark-target prediction bucket and added focused Shared Power enabled/off score coverage.
- [x] (2026-06-07T20:25Z) Ran targeted validation for the AI Prankster Dark-target prediction bucket and recorded evidence here.
- [x] (2026-06-07T20:28Z) Ran `git diff --check` after the AI Prankster Dark-target prediction bucket; no issues reported.
- [ ] Continue through the remaining migrate rows, one coherent bucket at a time.
- [ ] Perform a final audit pass that marks each migrate row implemented or explicitly deferred with rationale.

## Surprises & Discoveries

- Observation: The priority tranche was complete, but the broader sweep remains open.
  Evidence: `docs/design/shared_power/ability_usage_audit.md` still marks multiple rows as `Migrate`, `Migrate contract`, or `Clarify, then migrate`.

- Observation: `IsMovePowderBlocked` is a small next bucket because the audit names the exact native-only Overcoat check and attribution issue.
  Evidence: The row says `IsMovePowderBlocked` accepts `battlerDef` but records Overcoat against `gBattlerTarget`; the current code also checks `GetBattlerAbility(battlerDef) == ABILITY_OVERCOAT`.

- Observation: The battle script needed an Overcoat-specific entry rather than a global `jumpifability` policy change.
  Evidence: `BattleScript_PowderMoveNoEffect` branches among Grass-type immunity, Overcoat popup, and Safety Goggles text; Shared Power pooled Overcoat needs the Overcoat branch even when the protected battler's native ability is not Overcoat.

- Observation: `CanAbilityBlockMove` has two separate migration needs: effective ability membership for target-owned blockers such as Soundproof, Bulletproof, and Good as Gold, and correct attribution when an ally-owned priority blocker such as Dazzling protects its partner.
  Evidence: The helper receives `abilityDef`, switches on that one ability, later fetches `GetBattlerAbility(partnerDef)`, and then records `RecordAbilityBattle(battlerDef, abilityDef)` even after setting `battlerAbility = partnerDef`.

- Observation: Early Bird sleep decrement exists in both the normal attack-canceler path and the Battle Palace status-escape path.
  Evidence: `src/battle_util.c` uses `IsAbilityAndRecord(..., ABILITY_EARLY_BIRD)` in `CancellerAsleepOrFrozen`, while `src/battle_util2.c` uses `GetBattlerAbility(battler) == ABILITY_EARLY_BIRD` in `BattlePalace_TryEscapeStatus`.

- Observation: The battle test DSL rejects adjacent `NOT` assertions and requires a `NONE_OF` block for multiple forbidden events.
  Evidence: `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Early Bird"` marked the off-path test invalid with `'NOT x; NOT y;', did you mean 'NONE_OF { x; y; }'?`.

- Observation: The audit row for speed calculation still has native comparisons for Surge Surfer, Slow Start, Protosynthesis, Quark Drive, and Unburden, but some of those abilities also rely on activation state set elsewhere.
  Evidence: `GetBattlerTotalSpeedStatArgs` still compares the supplied `ability` against those ability constants; `slowStartTimer`, `boosterEnergyActivated`, and `unburdenActive` are maintained by separate switch-in/item/field code.

- Observation: The repo's Alolan Raichu species constant is `SPECIES_RAICHU_ALOLA`, not `SPECIES_RAICHU_ALOLAN`.
  Evidence: The first `TESTS="Shared Power: Surge Surfer"` validation failed to compile until the test used the species constant defined in `include/constants/species.h`.

- Observation: The infatuation migrate row points at `BS_TrySetInfatuation`, but two nearby script commands also gate infatuation-family effects on native Oblivious.
  Evidence: `Cmd_tryinfatuating` handles Attract, `Cmd_jumpifcaptivateaffected` handles Captivate, and `BS_TrySetInfatuation` handles side-wide infatuation such as G-Max Cuddle; all used direct native ability checks before this bucket.

- Observation: Heal Bell's Soundproof behavior splits active battlers from inactive party members.
  Evidence: `Cmd_healpartystatus` directly clears active attacker and partner status before looping party slots; inactive slots use `GetAbilityBySpecies` or test-forced party ability rather than a battler ability.

- Observation: `CanMoveSkipAccuracyCalc` is called only from the live battle-script accuracy command and the AI move-accuracy calculator.
  Evidence: `rg -n "CanMoveSkipAccuracyCalc\\(" src include test` finds callers in `src/battle_script_commands.c` with `RUN_SCRIPT` and `src/battle_ai_main.c` with `AI_CHECK`.

- Observation: The Terastal Adaptability migrate row is already implemented in production code.
  Evidence: `GetSameTypeAttackBonusModifier` in `src/battle_util.c` computes `hasAdaptability = HasActiveAbility(ctx->battlerAtk, ABILITY_ADAPTABILITY)`, and that helper is used for normal and Terastal same-type attack bonus calculations.

- Observation: Terastal damage applies a separate Tera multiplier after the normal damage roll instead of using `GetSameTypeAttackBonusModifier`.
  Evidence: `ApplyModifiersAfterDmgRoll` calls `GetTeraMultiplier` when `GetActiveGimmick(ctx->battlerAtk) == GIMMICK_TERA`; the first Shared Power Tera Adaptability regression failed with identical damage for Hyper Cutter and Adaptability until `GetTeraMultiplier` was inspected.

- Observation: Partner damage auras are split across different damage phases.
  Evidence: `CalcMoveBasePowerAfterModifiers` has native partner checks for Battery, Power Spot, and Steely Spirit; `CalcAttackStat` and `CalcDefenseStat` have native partner checks for Flower Gift; `GetDefenderPartnerAbilitiesModifier` already uses `HasActiveAbility` for Friend Guard.

- Observation: The battle test DSL rejects adding a second explicit move to a Pokemon whose `Moves(...)` list was already fixed.
  Evidence: The first `TESTS="Shared Power: pooled Battery"` validation failed as invalid with `Missing explicit Celebrate` until Wobbuffet's explicit move list included both Water Gun and Celebrate.

- Observation: A bench-only ability source is not enough for a Shared Power test unless the ability has already entered the pool.
  Evidence: The first `TESTS="Shared Power: pooled Symbiosis"` validation failed because Oranguru was never active, so the active Clefairy donor did not have pooled Symbiosis.

- Observation: The AI hazard Magic Bounce regression needs an active doubles partner, not a switched-out singles source.
  Evidence: Single-battle scoring after switching away from Espeon did not see pooled Magic Bounce for Wobbuffet, while the existing live Magic Bounce regression models the protection as an active partner contribution in doubles.

- Observation: Overlapping Docker `make check` runs can wedge the mGBA test runner exactly as the repo guidance warns.
  Evidence: An accidentally parallel disabled-path AI run hung at `mgba-rom-test-hydra` and had to be killed before rerunning that filter alone.

- Observation: Flinch volatile state is not a stable post-turn assertion for the disabled Inner Focus off-path.
  Evidence: The first `TESTS="Shared Power off: partner Inner Focus does not prevent Fake Out flinch"` run showed the flinch transcript but failed the post-turn `EXPECT(playerLeft->volatiles.flinched)` because the volatile was cleared by the time `THEN` executed.

- Observation: AI weather/terrain benefit prediction includes both shareable active ability heuristics and native-only weather/form-style heuristics.
  Evidence: `DoesAbilityBenefitFromWeather` includes native Forecast and Ice Face, so the Shared Power adapter preserves native benefit checks first and only adds pooled recognition for shareable benefit abilities such as Swift Swim and Surge Surfer.

- Observation: AI ally-targeting prediction has a narrow single-target absorption path separate from spread move ally-safety logic.
  Evidence: `AI_HPAware` directly checks `gAiLogicData->abilities[BATTLE_PARTNER(battlerAtk)]` for Volt Absorb, Earth Eater, Dry Skin, and Water Absorb when the AI is already targeting its partner.

- Observation: AI Sticky Hold item-loss prediction still has native-only checks after the live Sticky Hold bucket.
  Evidence: `CanKnockOffItem`, Knock Off/Trick/Corrosive Gas scoring, Thief scoring, Bug Bite scoring, and Incinerate scoring compare the target's cached native ability to `ABILITY_STICKY_HOLD`.

- Observation: AI Liquid Ooze prediction has both direct-drain and Leech Seed callsites.
  Evidence: `EFFECT_ABSORB` in `AI_CheckBadMove` compares the target ability directly, while Leech Seed callsites also mention Liquid Ooze but remain tied to the audit's end-turn Leech Seed source/target clarification.

- Observation: AI Substitute/Shed Tail scoring has a native-only Infiltrator shortcut even though live Substitute bypass is already active-aware.
  Evidence: `EFFECT_SUBSTITUTE` and `EFFECT_SHED_TAIL` in `AI_CheckBadMove` compare `aiData->abilities[battlerDef] == ABILITY_INFILTRATOR`.

- Observation: AI Magnitude bad-move scoring still has a native-only Levitate shortcut.
  Evidence: `EFFECT_MAGNITUDE` in `AI_CheckBadMove` compares `aiData->abilities[battlerDef] == ABILITY_LEVITATE`.

- Observation: There is no dedicated native Magnitude-plus-Levitate battle guardrail in the current test tree.
  Evidence: `test/battle/ability/levitate.c` and `test/battle/move_effect/magnitude.c` are TODO-only files, so the AI Magnitude bucket relies on the focused Shared Power enabled/off scoring checks plus the existing broader Ground-immunity coverage.

- Observation: `IsMoveMakingContact` cannot be used as a simple move-only assumption in Shared Power tests.
  Evidence: The first `TESTS="Shared Power: pooled Magic Guard prevents held item backlash"` compile failed because `IsMoveMakingContact` requires attacker, defender, ability, held-effect, and move arguments. The test now uses move-category assumptions for Scratch and Water Gun instead.

- Observation: Powder's Magic Guard prevention can still emit the user HP bar with no HP loss.
  Evidence: The first `TESTS="Shared Power: pooled Magic Guard prevents Powder self-damage"` validation matched an `HP_BAR(playerLeft)` even though the final HP stayed full; existing native Powder coverage also asserts the HP outcome rather than forbidding the user's HP bar.

- Observation: The test DSL rejects forcing a taunted battler to choose a blocked status move in a later turn.
  Evidence: The first `TESTS="Shared Power off: partner Oblivious does not block Taunt"` validation errored with `Illegal battle record` on the forced Spore choice. The off-path now proves Taunt application from the first-turn transcript and taunt timer instead.

- Observation: Perish Song's Soundproof blocked branch prints the block message directly without an ability popup.
  Evidence: The first `TESTS="Shared Power: pooled Soundproof prevents Perish Song timers"` validation failed on an unmatched `ABILITY_POPUP(playerLeft, ABILITY_SOUNDPROOF)`, while the direct `Wobbuffet's Soundproof blocks Perish Song!` message and timer assertions pass.

- Observation: The remaining direct Ripen checks all live in item-consumption effect code that already has nearby active-aware Ripen migrations.
  Evidence: `HealConfuseBerry`, `TrySetEnigmaBerry`, `ItemRestorePp`, and the Jaboca/Rowap branches still compare `GetBattlerAbility(battler) == ABILITY_RIPEN`, while stat-boosting and standard HP-restoring berry paths already use `HasActiveAbility` plus `IsAbilityAndRecord`.

- Observation: Figy Berry test setup must start above the activation threshold or the berry is consumed before the scripted attack.
  Evidence: The first `TESTS="Shared Power: pooled Ripen doubles remaining healing berries"` validation failed before the Dragon Rage scene because the holder started at 70/300 HP, already below the 1/4 threshold. Raising the start to 100/300 made the post-hit threshold crossing deterministic.

- Observation: Leppa Berry restores after the move spends its last PP, so final PP is the restored amount rather than start PP plus restored amount.
  Evidence: The first `TESTS="Shared Power: pooled Ripen doubles Leppa Berry PP restoration"` validation failed with `EXPECT_EQ(20, 21)`; the correct doubled result from 0 PP is 20.

- Observation: Cheek Pouch uses the shared ability-heal string, not an item-heal style sentence.
  Evidence: The first `TESTS="Shared Power: pooled Cheek Pouch heals after berry consumption"` validation failed on `Wobbuffet restored a little HP using its Cheek Pouch!`; the actual script string is `Wobbuffet's Cheek Pouch restored its HP a little!`.

- Observation: Volatile bit-fields cannot be compared with `EXPECT_EQ`.
  Evidence: The first `TESTS="Shared Power: pooled Oblivious blocks Cute Charm"` validation failed to compile with `'typeof' applied to a bit-field` for `playerLeft->volatiles.infatuation`; using `EXPECT(!playerLeft->volatiles.infatuation)` fixed the assertion.

- Observation: The Mirror Armor branch in Gooey/Tangling Hair only changes behavior when the contact attacker is already at minimum Speed.
  Evidence: The move-end target callback already runs when `CompareStat(gBattlerAttacker, STAT_SPEED, MIN_STAT_STAGE, CMP_GREATER_THAN)` is true; the Mirror Armor allowance matters for the minimum-stage reflection case.

- Observation: The Mirror Armor contact-reflection regression needs doubles setup to prove pooled behavior without losing the attacker's minimum Speed stage.
  Evidence: The passing enabled test lowers the native Shadow Tag attacker to minimum Speed before switching its partner to Corviknight; the matching Shared Power-off test keeps the same partner Mirror Armor inert.

- Observation: Own Tempo confusion prevention has standard move/item paths and separate Sky Drop hardcoded-state paths.
  Evidence: Standard confusion uses `CanBeConfused`, `BattleScript_EffectConfuse`/Swagger/Flatter `jumpifability`, and Berserk Gene's helper/script; Sky Drop release/cancel also checks Own Tempo but is tied to `gBattleStruct->skyDropTargets` and remains in the hardcoded target-state queue.

- Observation: Confusion volatile fields are bit-fields and need boolean assertions in tests.
  Evidence: The first `TESTS="Shared Power: pooled Own Tempo prevents confusion from moves"` validation failed to compile with `'typeof' applied to a bit-field` for `player->volatiles.confusionTurns`; using `EXPECT(!player->volatiles.confusionTurns)` avoids the macro limitation.

- Observation: Toxic Chain's Knock Off cure-berry interaction is separate from the general move-end Toxic Chain callback.
  Evidence: `TryMoveEndAttackerAbilityEffect` can already dispatch pooled Toxic Chain, but the Pecha/Lum cure before Knock Off removal still checks `GetBattlerAbility(gBattlerAttacker) == ABILITY_TOXIC_CHAIN` inside holder item-effect handling.

- Observation: Poké Flute has separate active-battler and inactive-party Soundproof checks.
  Evidence: `BS_CheckPokeFlute` loops over active battlers with `GetBattlerAbility(i) != ABILITY_SOUNDPROOF`, then `UpdatePokeFlutePartyStatus` checks inactive party members by species/party ability; only the active loop should use Shared Power membership.

- Observation: Poké Flute item-name text is brittle in scene assertions.
  Evidence: The first `TESTS="Shared Power: pooled Soundproof blocks Poke Flute from waking an active teammate"` run failed on the exact `1 played the Poké Flute...` message while the intended behavior is better proven by the absence of the wake-up message and the sleeping status assertion.

- Observation: Poké Flute's party-status update also visits active party slots.
  Evidence: After the active loop used `HasActiveAbility`, the enabled Poké Flute regression still printed the wake-up message because `UpdatePokeFlutePartyStatus` saw the active Wobbuffet's native Shadow Tag in the player party and cleared sleep through the party bitmask update.

- Observation: Unseen Fist protect bypass is a narrow active-attacker membership check.

- Observation: Lightning Rod and Storm Drain redirection already use active field/side searches, but still use native target exclusion and native redirector ability checks.
  Evidence: `HandleMoveTargetRedirection` tests `ability != ABILITY_LIGHTNING_ROD/STORM_DRAIN` for the current target and then loops with `GetBattlerAbility(battler)`, while `GetBattleMoveTarget` uses `IsAbilityOnField` / `IsAbilityOnOpposingSide` but suppresses redirection only when the randomly selected target's native ability matches.

- Observation: Redirection migration also needs live direct absorption resolution for selected pooled targets.
  Evidence: After the first redirection-only implementation, `TESTS="Shared Power: pooled Lightning Rod keeps Electric moves on the selected active target"` no longer redirected to the native Lightning Rod source but still failed to show the selected target's Lightning Rod popup until live callsites selected pooled Lightning Rod / Storm Drain before calling `CanAbilityAbsorbMove`.

- Observation: Infiltrator has two native-only attacker bypass checks in this scope.
  Evidence: `DoesSubstituteBlockMove` uses `IsAbilityAndRecord(battlerAtk, GetBattlerAbility(battlerAtk), ABILITY_INFILTRATOR)`, while `ChangeStatBuffs` bypasses Mist only when `GetBattlerAbility(gBattlerAttacker) == ABILITY_INFILTRATOR`.

- Observation: Klutz item behavior is centralized enough for a live bucket plus one AI adapter callsite.
  Evidence: `GetBattlerHoldEffectInternal`, `CanFling`, and `BS_CheckPoltergeist` check native Klutz directly, while `IsBattlerItemEnabled` in AI checks `gBattleMons[battler].ability == ABILITY_KLUTZ` instead of the existing `AI_HasActiveAbility` adapter.
  Evidence: `IsBattlerProtected` currently checks `GetBattlerAbility(battlerAtk) == ABILITY_UNSEEN_FIST` before asking whether the selected move makes contact under Unseen Fist; no native slot mutation or form/species gate is involved.

- Observation: The battle test DSL rejects adjacent forbidden scene assertions.
  Evidence: The first `TESTS="Shared Power off: partner Unseen Fist does not bypass Protect"` validation was invalid with `'NOT x; NOT y;', did you mean 'NONE_OF { x; y; }'?`; the disabled-path test now groups the forbidden Scratch animation and HP bar.

- Observation: The Run Away/trapping row is not policy-clear enough for a silent migration.
  Evidence: `IsAbilityPreventingEscape` currently returns only a battler index after native Shadow Tag, Arena Trap, or Magnet Pull checks, while `IsRunningFromBattleImpossible` displays `gBattleMons[i - 1].ability`. A pooled trapping migration would need to decide donated trap participation, Shadow Tag's Gen4+ self-exemption basis, and popup/history attribution for a pooled trapper whose native slot is not the trapping ability.

- Observation: The player effectiveness preview row is mostly gated by the same unresolved damage-context and dynamic-move-type contracts as live damage calculation.
  Evidence: `CheckTypeEffectiveness` seeds `DamageContext.abilityAtk/Def` with native abilities, but `CalcTypeEffectivenessMultiplierInternal` already uses live `HasActiveAbility` for Scrappy, Mind's Eye, Levitate, Wonder Guard, and Telepathy. The remaining preview mismatch risk is whether preview should use live or selected ability context for dynamic type conversion and future context-sensitive ability decisions.

- Observation: Switch AI's Wonder Guard escape path was a narrow AI full-pool migration candidate.
  Evidence: `ShouldSwitchIfWonderGuard` is singles-only and previously gated on `gAiLogicData->abilities[opposingBattler] == ABILITY_WONDER_GUARD`; the migrated path can query `HasActiveAbility(opposingBattler, ABILITY_WONDER_GUARD)` under Shared Power without changing broad `gAiLogicData->abilities[]` storage or hidden-information policy.

- Observation: AI secondary-damage prediction has active-battler ability checks that can migrate independently of switch-in candidate scoring.
  Evidence: `GetBattlerSecondaryDamage` predicted poison, weather, trap, curse, nightmare, and Leech Seed damage for active battlers and only skipped all damage on native `gAiLogicData->abilities[battlerId] == ABILITY_MAGIC_GUARD`; `GetPoisonDamage` similarly skipped poison damage only on native Poison Heal. The bucket changes only active-battler prediction through `AI_HasActiveAbility` and leaves inactive switch-in candidate status/weather scoring native.

- Observation: AI guaranteed-flinch prediction had fallen behind the nearby flinch-scoring helper.
  Evidence: `ShouldTryToFlinch` already checked active Shared Power membership for defender-side Shield Dust and Inner Focus, while `IsFlinchGuaranteed` still read only `gAiLogicData->abilities[battlerDef]` for those same defender protections.

- Observation: AI Heal Bell prediction splits active battler Soundproof checks from inactive party Soundproof checks.
  Evidence: `AnyPartyMemberStatused` checks the active user and partner via `gAiLogicData->abilities[]`, but inactive party members via `GetMonAbility(&party[i])`; Shared Power only grants active battlers effective side abilities, so the bucket can migrate active checks and leave inactive party slots native-only.

- Observation: AI sleep-state prediction for Comatose is an active battler membership check.
  Evidence: status-prevention AI already uses `AI_HasActiveAbility(battlerDef, ABILITY_COMATOSE)`, and live move-effect checks treat active Comatose as sleep-like in selected contexts; `AI_IsBattlerAsleepOrComatose` was the matching AI helper still reading only `gAiLogicData->abilities[battlerId]`.

- Observation: AI powder prediction should use the same active Overcoat membership as live powder blocking.
  Evidence: live attack-canceling uses `HasActiveAbility(battlerDef, ABILITY_OVERCOAT)` for powder moves under Shared Power, but AI bad-move and Rage Powder scoring passed only `aiData->abilities[battlerDef]` into `IsAffectedByPowder`.

- Observation: AI Perish Song prediction has active Soundproof checks that match already-migrated live Soundproof timers.
  Evidence: live Shared Power tests cover pooled Soundproof preventing Perish Song timers, while the AI bad-move score still compared user, partner, and foe `aiData->abilities[]` directly to `ABILITY_SOUNDPROOF`.

- Observation: AI Roar prediction has a sound-blocker check that can migrate independently of phazing immunities.
  Evidence: `EFFECT_ROAR` scoring checked `IsSoundMove(move) && aiData->abilities[battlerDef] == ABILITY_SOUNDPROOF` before considering stat-clear score; live sound move blocking already treats active pooled Soundproof as authoritative, while Suction Cups remains a separate native/cached phazing check outside this bucket.

- Observation: AI Roar prediction still has native-only Suction Cups checks beside already-migrated active Soundproof checks.
  Evidence: `src/battle_ai_main.c` has native `aiData->abilities[battlerDef] == ABILITY_SUCTION_CUPS` checks in both bad-move and viability Roar scoring. Live Dragon Tail Suction Cups still uses `GetBattlerAbility(gBattlerTarget)`, so this bucket is scoped to AI Roar-style phazing prediction only and leaves live hit-switch-target policy for a later hardcoded-state review.

- Observation: AI Strength Sap prediction has one native Contrary shortcut before the already-active stat-loss helper.
  Evidence: `src/battle_ai_main.c` checks `aiData->abilities[battlerDef] == ABILITY_CONTRARY` in the `EFFECT_STRENGTH_SAP` bad-move branch, then falls through to `CanLowerStat`, which already uses active ability membership for Contrary and stat-loss blockers. The bucket can migrate that one shortcut without changing live Strength Sap Liquid Ooze behavior or the broader status/stat helper contract.

- Observation: AI multi-hit contact scoring still penalizes Rocky Helmet contact when Magic Guard is pooled onto the attacker.
  Evidence: `src/battle_ai_main.c` checks `aiData->abilities[battlerAtk] != ABILITY_MAGIC_GUARD` before applying the multi-hit Rocky Helmet score penalty. Live Rocky Helmet, Jaboca Berry, and Rowap Berry backlash already use active Magic Guard membership on the attacker, so this AI bucket can migrate the matching Rocky Helmet heuristic without changing contact-helper policy or target-held item ownership.

- Observation: AI Rest scoring still treats fast recovery as native-only for Shed Skin, Early Bird, and Hydration.
  Evidence: `src/battle_ai_main.c` adds an extra Rest recovery score for cure items, Sleep Talk/Snore, or native `ABILITY_SHED_SKIN`, `ABILITY_EARLY_BIRD`, and rain `ABILITY_HYDRATION`. Live end-turn Shed Skin/Hydration and sleep decrement Early Bird are already active-aware, so this AI bucket can migrate only the matching Rest prediction checks without changing sleep-clause eligibility or weather/item policy.

- Observation: AI Focus Energy and Laser Focus setup scoring still treats crit payoff abilities as native-only.
  Evidence: `src/battle_ai_main.c` adds a crit-setup score for native `ABILITY_SUPER_LUCK` or `ABILITY_SNIPER`, even though AI crit damage prediction already uses `AI_HasActiveAbility` for Super Luck and live Sniper damage is a passive modifier. This bucket can migrate only the attacker-owned setup heuristic without changing guaranteed-crit prevention or damage-context policy.

- Observation: AI crash-recoil prediction still penalizes low-accuracy miss-recoil moves when Magic Guard is pooled onto the attacker.
  Evidence: `src/battle_ai_main.c` checks `aiData->abilities[battlerAtk] != ABILITY_MAGIC_GUARD` before applying the low-accuracy `EFFECT_RECOIL_IF_MISS` score penalty. Live crash recoil is user-side indirect damage, and Magic Guard has already migrated across nearby recoil/item-damage paths, so this bucket can migrate the matching attacker-owned prediction check.

- Observation: AI Foresight prediction still treats Scrappy and Mind's Eye as native-only attacker abilities.
  Evidence: `src/battle_ai_main.c` skips Foresight setup when native `aiData->abilities[battlerAtk]` is `ABILITY_SCRAPPY` or `ABILITY_MINDS_EYE`, but live Normal/Fighting into Ghost type-effectiveness already queries active membership for both abilities. This bucket can migrate only the Foresight usefulness heuristic without changing the type-effectiveness or Foresight volatile contracts.

## Decision Log

- Decision: Defer the Run Away/trapping row instead of migrating Shadow Tag, Arena Trap, and Magnet Pull trapping in this sweep pass.
  Rationale: The audit row is labeled `Clarify`, and the live code cannot preserve correct attribution without a policy decision. A migration would need to define whether donated trapping abilities prevent escape, whether donated Shadow Tag on the trapped battler satisfies the Gen4+ self-exemption rule, and which battler/ability should be displayed and recorded when the trap source is pooled rather than native.
  Date/Author: 2026-06-07 / Codex

- Decision: Defer the player effectiveness preview row until the broader damage-context contract is settled.
  Rationale: The current preview already calls the same active-aware type-effectiveness helper for the abilities presently hardcoded inside that helper. Changing only the native `DamageContext.abilityAtk/Def` seeds would not change those paths, while forcing live ability re-queries or dynamic move-type sharing would preempt unresolved policy for `DamageContext`, Mold Breaker-style suppression, and Ate/Liquid Voice/Normalize ordering.
  Date/Author: 2026-06-07 / Codex

- Decision: Keep the completed priority tranche as its own finished plan and use this document as the umbrella tracker for future buckets.
  Rationale: The priority tranche already has detailed implementation and validation evidence. Duplicating every detail here would make the broader sweep harder to scan.
  Date/Author: 2026-06-06 / Codex

- Decision: Use powder-move Overcoat blocking as the next bucket.
  Rationale: It is explicitly marked migrate, has a clear target-state/attribution bug, and can be fixed without changing broad script `jumpifability` policy.
  Date/Author: 2026-06-06 / Codex

- Decision: Do not make `jumpifability` Shared Power-aware as part of the powder bucket.
  Rationale: `jumpifability` appears in many scripts, including native-only and clarify-only contexts. The powder bucket should instead route the known Overcoat result to an Overcoat-specific script path.
  Date/Author: 2026-06-06 / Codex

- Decision: Keep `AI_CHECK` behavior in `CanAbilityBlockMove` on explicit supplied abilities, and migrate live `RUN_SCRIPT` / trigger checks through effective ability membership.
  Rationale: `AI_CanAbilityBlockMove` is already a Shared Power-aware AI adapter. Changing lower-level AI checks to use live `HasActiveAbility` would broaden AI hidden-information semantics beyond this bucket.
  Date/Author: 2026-06-06 / Codex

- Decision: Scope the infatuation/Oblivious bucket to script-command target gates for Attract, Captivate, and side-wide infatuation, and leave Cute Charm's contact callback Oblivious check to the move-end callback clarification queue.
  Rationale: The direct script-command gates all answer the same target-membership question and can preserve existing popup behavior. Cute Charm executes as a pooled target-owned callback after contact and also mixes attacker/target globals, so it belongs with the audited callback ownership work.
  Date/Author: 2026-06-06 / Codex

- Decision: Scope the party-status Heal Bell Soundproof bucket to active battler Soundproof checks and leave inactive party slots native-only.
  Rationale: Shared Power only grants active battlers effective side abilities. Inactive party slots are not active Shared Power recipients, and the existing loop intentionally reads party ability data for non-battlers.
  Date/Author: 2026-06-07 / Codex

- Decision: Split the accuracy work and first migrate only the `CanMoveSkipAccuracyCalc` No Guard contract.
  Rationale: The helper has a crisp contract bug: it accepts attacker and defender ability parameters for AI but ignores them, and live recording always attributes No Guard to the attacker. The broader `AccuracyCheck` row includes Dragon Darts target mutation and full accuracy-modifier context, so it should remain open for a later bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate `GetTeraMultiplier` for Adaptability rather than treating the Terastal row as a no-op.
  Rationale: `GetSameTypeAttackBonusModifier` was already active-aware, but Terastal damage bypasses it and calls `GetTeraMultiplier`, which still used native `GetBattlerAbility`. The focused regression exposed this as a real remaining migrate row.
  Date/Author: 2026-06-07 / Codex

- Decision: Split partner damage auras and first migrate the base-power partner auras Battery, Power Spot, and Steely Spirit.
  Rationale: These three share one native-only partner switch in `CalcMoveBasePowerAfterModifiers` and do not require weather/form or accuracy policy. Flower Gift depends on Cherrim form/weather ownership and Victory Star belongs partly to the accuracy-math row, so they remain open for later focused buckets.
  Date/Author: 2026-06-07 / Codex

- Decision: Use Symbiosis as the next bucket.
  Rationale: The audit marks `TrySymbiosis` / `BS_TrySymbiosis` migrate, and the owner model is narrow: the active partner remains the item donor and popup battler, while effective membership decides whether that donor has Symbiosis under Shared Power.
  Date/Author: 2026-06-07 / Codex

- Decision: Split the move-end move block row and first migrate Sticky Hold item protection.
  Rationale: Sticky Hold is a target-owned item protection ability with clear popup and record attribution, while Gorilla Tactics involves move-choice restriction and choiced-move state in selection helpers and should be validated as its own choice-lock bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Group the remaining Ripen berry-effect paths into one bucket.
  Rationale: Confusion berries, Enigma Berry, Leppa Berry, and Jaboca/Rowap Berry damage share the same holder-owned item-effect semantics: the consuming or triggering holder remains the battler to record, and effective Shared Power membership decides whether Ripen doubles the effect. This keeps item ownership native while migrating the shareable modifier.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate the Heal Pulse Mega Launcher boost as a narrow pulse-move support bucket.
  Rationale: `BS_TryHealPulse` asks whether the active attacker benefits from Mega Launcher when using a pulse move. It does not mutate abilities, depend on form/species gates, or need source iteration; active membership on the attacker should decide the 75% healing boost under Shared Power while Floral Healing's terrain branch remains unchanged.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Cheek Pouch as a holder-owned berry-consumption effect.
  Rationale: `TryCheekPouch` runs after the holder consumes its own berry and asks whether that active holder has Cheek Pouch. Active membership should trigger the extra heal under Shared Power, while the consumed item, `ateBerry` state, Heal Block check, damage restore behavior, and popup battler remain the berry consumer.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Liquid Ooze only for direct drain moves in this bucket.
  Rationale: Absorb-style move-end healing and Strength Sap both directly drain from `gBattlerTarget`, whose active Liquid Ooze should turn the attacker's healing into damage and own the popup. End-turn Leech Seed has separate seeded-victim/receiver ownership questions in the hardcoded target-state queue, so it remains outside this bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Guard Dog protection from Red Card forced switching.
  Rationale: Red Card is a holder-owned item effect, but the ability question is whether the active attacker can be forced out. Existing phazing logic already treats Guard Dog as active membership on the protected target, so the Red Card attacker check should match without changing Red Card consumption, item messages, Commander, Dynamax, or replacement selection.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate the Oblivious blocker inside Cute Charm's contact callback.
  Rationale: The move-end target callback already owns Cute Charm through the damaged target and its existing popup script. The native-only check is the contact attacker's Oblivious protection, which should use active membership just like direct infatuation-family gates, while Aroma Veil side protection and contact-avoidance policy remain unchanged.
  Date/Author: 2026-06-07 / Codex

- Decision: Scope Gorilla Tactics to choice-lock behavior and Knock Off lock preservation.
  Rationale: Gorilla Tactics' damage boost already participates in existing effective ability iteration, but the choice-lock setup, move selection limitation, and Knock Off lock preservation still read native ability slots.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Flower Gift's partner stat aura without migrating Flower Gift form changes.
  Rationale: The partner aura has a clear owner: the active partner Cherrim must already be in Sunshine form and weather-affected by sun, while effective membership decides whether that active partner has Flower Gift. The transformation rules remain native/species-gated and are not part of this bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate only Victory Star's partner accuracy aura inside `GetTotalAccuracy`.
  Rationale: The audit marks partner auras for migration and separately flags the broader accuracy math contract. The isolated native partner Victory Star lookup can use active ally membership without changing supplied attacker/defender ability semantics or Dragon Darts target handling.
  Date/Author: 2026-06-07 / Codex

- Decision: Scope the attack-canceler row to Magic Bounce ownership.
  Rationale: The block/absorb helper work has already been migrated, while `Cmd_attackcanceler` still has native Magic Bounce checks for the target and target partner. Magic Bounce has clear popup ownership through `gBattlerAbility`, and this can be fixed without changing status-helper, contact, or accuracy contracts.
  Date/Author: 2026-06-07 / Codex

- Decision: Scope the accuracy command row to live battle accuracy math.
  Rationale: The No Guard skip helper already has a focused contract migration, while `AccuracyCheck` still passed native attacker and defender abilities into `GetTotalAccuracy` for real targets. A live-only wrapper lets the battle script use active Shared Power membership for accuracy and evasion abilities without changing the explicit supplied-ability API used by AI calculations. Dragon Darts target mutation remains in the command, and broader type-effectiveness `DamageContext` semantics remain a separate clarified contract.
  Date/Author: 2026-06-07 / Codex

- Decision: Treat live Victory Star accuracy as a unique Shared Power modifier in the live accuracy wrapper.
  Rationale: The attacker and ally can both have effective Victory Star through the same shared pool. Counting both the attacker modifier and ally aura would double-count one pooled ability source, so the live wrapper only applies the ally aura when the attacker does not already have active Victory Star. The explicit supplied-ability `GetTotalAccuracy` API keeps its existing native behavior.
  Date/Author: 2026-06-07 / Codex

- Decision: Scope the `SetMoveEffect` row to Flame Burst partner splash Magic Guard first.
  Rationale: `SetMoveEffect` mixes many mechanics. The Flame Burst branch is a clear active-battler yes/no protection check on the damaged partner, and currently reads the partner's native ability slot. Status helper policy, Inner Focus flinch prevention, Parental Bond recoil, and native mutation/form-gated mechanics require separate ownership decisions and are not changed in this bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Start the AI known-ability row with hazard setup ability-awareness checks rather than replacing global AI known-ability cache semantics.
  Rationale: The audit says AI should reason over the current Shared Power pool, but broad hidden-information policy should remain explicit. `AI_ShouldSetUpHazards` already makes live tactical yes/no checks for Magic Bounce and Shield Dust, and can use `AI_HasActiveAbility` so the target's effective Shared Power membership is considered without changing prediction, species guessing, or the one-ability `gAiLogicData->abilities[]` cache. This intentionally avoids side-wide partner checks when Shared Power is disabled.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate only the `SetMoveEffect` flinch prevention gate for Inner Focus in this bucket.
  Rationale: Flinch prevention is a clear target-owned protection check: the effect battler is either flinched or protected. The existing branch already attributes the prevention popup and battle record to `gEffectBattler` with `ABILITY_INNER_FOCUS`, so active membership can decide the protection without changing status helper policy, contact callback owner rules, or recoil/item backlash handling.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Magic Guard protection from held-item backlash for Rocky Helmet, Jaboca Berry, and Rowap Berry as one item-effect bucket.
  Rationale: These three target-held items all damage the attacker as indirect backlash and currently check the attacker's native ability slot for Magic Guard. The owner is unambiguous: active Magic Guard on the attacker prevents the backlash damage, while contact avoidance, item effect records, target-held item ownership, and Ripen's target-side berry modifier stay unchanged.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate only the `EFFECT_MAX_HP_50_RECOIL` Magic Guard gate for half-HP user-loss moves in this bucket.
  Rationale: Steel Beam and Mind Blown use a clear attacker-owned Magic Guard protection check before applying half-max-HP loss, and existing native tests establish Magic Guard prevents that loss. This bucket does not change regular recoil, crash recoil, Chloroblast/Rock Head behavior, or evolution tracking policy.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate only Spiky Shield's Magic Guard suppression check in the protect-like move-end bucket.
  Rationale: Spiky Shield's secondary effect is direct HP loss on the contacting attacker, so active Magic Guard on that attacker should suppress it under Shared Power. The sibling protect-like branches involve status helper policy or stat-drop attribution, so Baneful Bunker, Burning Bulwark, King's Shield, Silk Trap, and Obstruct remain separate.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Magic Guard protection from holder-side damaging item effects for Black Sludge and Sticky Barb.
  Rationale: These item effects hurt their holder directly and currently check only the holder's native Magic Guard. Active Magic Guard on the item holder should suppress that damage under Shared Power, while Poison-type Black Sludge healing, Toxic Orb, Flame Orb, and Sticky Barb transfer behavior remain outside this bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Magic Guard protection from damaging entry hazards for Spikes, Stealth Rock, and G-Max Steelsurge.
  Rationale: These hazard paths directly apply indirect HP loss to the switching battler and currently check only that battler's native Magic Guard. Active Magic Guard on the switching battler should suppress the damage under Shared Power. Toxic Spikes status application, Poison-type Toxic Spikes absorption, Sticky Web stat drops, and broader status-helper policy remain outside this bucket.
  Date/Author: 2026-06-06 / Codex

- Decision: Migrate Magic Guard protection from Powder self-damage.
  Rationale: Powder's fire-move self damage is a direct indirect-damage prevention check on the active attacker and currently checks only the attacker's native Magic Guard. Active Magic Guard on the attacker should suppress that damage under Shared Power. Paralysis immobility, regular recoil/crash recoil, Gulp Missile, and broader status-helper policy remain outside this bucket.
  Date/Author: 2026-06-06 / Codex

- Decision: Migrate the direct Taunt Oblivious target gate.
  Rationale: Taunt prevention is a target-owned yes/no protection check that already records and displays Oblivious against the protected target. Active Oblivious on the target should block Taunt under Shared Power, while broader status-helper policy, Encore/Torment behavior, and non-Taunt volatile rules remain outside this bucket.
  Date/Author: 2026-06-06 / Codex

- Decision: Migrate Pressure PP deduction.
  Rationale: Pressure PP deduction asks whether the target or each opposing active battler has Pressure for the move being used. Active Pressure membership should drive that PP cost under Shared Power. For spread or force-Pressure moves, each alive opposing battler with effective Pressure contributes one extra PP just as multiple native Pressure users already stack.
  Date/Author: 2026-06-06 / Codex

- Decision: Migrate the generic `jumpifblockedbysoundproof` script command.
  Rationale: This command is a direct sound-move immunity gate for a script-selected active battler and already records/displays Soundproof against that battler. Active Soundproof membership should decide the block under Shared Power. Broader Perish Song setup, Poke Flute, Uproar wake policy, and inactive-party Soundproof checks remain separate.
  Date/Author: 2026-06-06 / Codex

- Decision: Migrate Perish Song's Soundproof setup exclusion.
  Rationale: Perish Song setup loops over active battlers and decides whether each receives a perish timer. That active-battler Soundproof exclusion should use active membership under Shared Power. The later popup loop is already covered by `jumpifblockedbysoundproof`; Uproar, Poke Flute, inactive-party Soundproof, Commander, and Prankster-block policy remain outside this bucket.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Gluttony's berry-consumption HP threshold.
  Rationale: `HasEnoughHpToEatBerry` is the common holder-owned threshold helper for pinch berry activation. Active Gluttony on the holder should allow eligible berries to trigger at half HP under Shared Power. Ripen's effect doubling, Harvest, Cud Chew, and item-consumption ownership remain separate buckets.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Ripen doubling for stat-boosting berries.
  Rationale: Stat pinch berries, random stat berries, and hit-triggered Kee/Maranga-style berries are holder-owned item effects whose boost amount is doubled by Ripen. Active Ripen on the holder should double those stat boosts under Shared Power. HP/PP restoration berries, Enigma/confusion berries, and other berry ownership cases remain separate buckets.
  Date/Author: 2026-06-07 / Codex

- Decision: Migrate Ripen doubling for standard HP-restoring berries in `ItemHealHp`.
  Rationale: Oran/Sitrus-style HP restoration is a holder-owned berry effect whose healing amount is doubled by Ripen. Active Ripen on the holder should double that healing under Shared Power. Confusion berries, Enigma Berry, Leppa/PP restoration, and damaging berries remain separate buckets.
  Date/Author: 2026-06-07 / Codex

## Outcomes & Retrospective

No umbrella outcome has been recorded yet. This plan remains active until the broader sweep is complete or the remaining rows are explicitly deferred with evidence-backed rationale.

The powder-move Overcoat bucket is complete. `IsMovePowderBlocked` now checks active Overcoat membership for the protected battler, records Overcoat against that battler, and routes pooled Overcoat blocks to an Overcoat popup script path. Grass-type powder immunity and Safety Goggles remain separate paths.

The move-blocking helper bucket is complete. `CanAbilityBlockMove` now separates blocker selection from script side effects, uses effective live Shared Power membership for target-owned blockers such as Soundproof, Bulletproof, and Good as Gold, preserves ally-owned priority blocker popup ownership, and records the selected blocker ability against the popup battler.

The Early Bird sleep-decrement bucket is complete. Normal move cancellation and Battle Palace status escape now use active Early Bird membership, so pooled Early Bird shortens sleep duration in Shared Power without changing disabled/off-path sleep behavior.

The speed-calculation remainder bucket is complete. `GetBattlerTotalSpeedStatArgs` now uses active ability membership for Surge Surfer, Slow Start, Protosynthesis, Quark Drive, and Unburden while preserving the existing activation-state gates for Electric Terrain, Slow Start timers, Booster Energy state, and Unburden state.

The infatuation/Oblivious script-command bucket is complete. Attract, Captivate, and side-wide infatuation script gates now check active Oblivious membership so Shared Power can protect active teammates from those direct infatuation-family effects. Attract and Captivate still attribute the blocking popup and battle record to Oblivious on the protected target, and the side-wide loop preserves its existing silent per-target fail behavior.

The party-status Heal Bell Soundproof bucket is complete. Active attacker and active partner Soundproof checks now use active ability membership, while inactive party slots remain native ability checks. When active Soundproof blocks a cure, battle history records `ABILITY_SOUNDPROOF` against the protected battler.

The accuracy-skip No Guard contract bucket is complete. `CanMoveSkipAccuracyCalc` now honors supplied ability arguments for `AI_CHECK`, uses live active ability membership for `RUN_SCRIPT`, and records No Guard against the attacker or defender whose active ability membership caused the skip. The broader `AccuracyCheck` command row remains open for Dragon Darts/global-target and full accuracy-modifier context work.

The holder-side item damage Magic Guard bucket is complete. Black Sludge damage on non-Poison holders and Sticky Barb end-turn holder damage now use active Magic Guard membership, so pooled Magic Guard suppresses those indirect item damage paths under Shared Power. Poison-type Black Sludge healing, Toxic Orb, Flame Orb, and Sticky Barb transfer behavior remain unchanged. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents holder item damage"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent holder item damage"` on 2026-06-06.

The entry-hazard damage Magic Guard bucket is complete. Spikes, Stealth Rock, and G-Max Steelsurge switch-in damage now use active Magic Guard membership, so pooled Magic Guard suppresses those hazard damage paths under Shared Power. Toxic Spikes status application and Sticky Web stat drops remain unchanged. Focused regression coverage validates Spikes and Stealth Rock with pooled Magic Guard and Shared Power disabled; G-Max Steelsurge shares the migrated switch-in damage branch and remains covered by existing Steelsurge setup tests. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents damaging entry hazards"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent damaging entry hazards"` on 2026-06-06.

The Powder self-damage Magic Guard bucket is complete. Powder's blocked-fire-move self damage now uses active Magic Guard membership, so pooled Magic Guard suppresses the HP loss under Shared Power while the blocked move behavior remains unchanged. The enabled regression asserts that Wobbuffet's native Shadow Tag receives pooled Magic Guard from active partner Clefable and keeps full HP; the disabled regression confirms the same partner does not prevent the 25% HP loss outside Shared Power. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents Powder self-damage"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent Powder self-damage"` on 2026-06-06.

The Taunt Oblivious target-protection bucket is complete. `Cmd_settaunt` now uses active Oblivious membership for the protected target, so pooled Oblivious blocks Taunt under Shared Power while preserving the target-side popup and battle-record attribution. The enabled regression asserts the Oblivious popup on the protected target and confirms the target can still use Spore; the disabled regression confirms partner Oblivious does not block Taunt outside Shared Power. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Oblivious blocks Taunt"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Oblivious does not block Taunt"` on 2026-06-06.

The Pressure PP deduction bucket is complete. `Cmd_ppreduce` now uses active Pressure membership for both selected-target PP deduction and spread or force-Pressure move deduction across all opposing active battlers. The enabled regression confirms pooled Pressure adds PP cost for a single-target move aimed at a non-Pressure native target, and makes both opposing active battlers contribute to spread-move PP cost under Shared Power. The disabled regression confirms partner Pressure does not affect its ally outside Shared Power. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Pressure increases PP deduction"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Pressure does not increase PP deduction for its ally"` on 2026-06-06.

The generic `jumpifblockedbysoundproof` script-command bucket is complete. The command now uses active Soundproof membership for the script-selected battler, preserving existing popup and battle-record attribution to that battler. The focused Howl regression confirms pooled Soundproof from the active user blocks the partner-side Attack boost on a native Shadow Tag partner and displays the partner's Soundproof popup; the disabled regression confirms Howl boosts the partner when Shared Power is off. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof blocks Howl on an active partner"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: user Soundproof does not block Howl on its partner"` on 2026-06-06.

The Perish Song Soundproof setup bucket is complete. `Cmd_trysetperishsong` now uses active Soundproof membership when deciding which active battlers receive perish timers. The later script display path was already active-aware through `jumpifblockedbysoundproof`. The enabled regression confirms pooled Soundproof prevents a native Shadow Tag battler from receiving a Perish Song timer, and the disabled regression confirms partner Soundproof does not prevent the timer when Shared Power is off. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof prevents Perish Song timers"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof does not prevent Perish Song timers"` on 2026-06-07.

The Gluttony berry-threshold bucket is complete. `HasEnoughHpToEatBerry` now uses active Gluttony membership for eligible berry thresholds while preserving the existing battle-history record of Gluttony as the threshold ability. The enabled regression confirms pooled Gluttony lets a native Shadow Tag Liechi Berry holder consume at exactly half HP, and the disabled regression confirms partner Gluttony does not trigger that early outside Shared Power. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Gluttony triggers pinch berries at half HP"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Gluttony does not trigger pinch berries at half HP"` on 2026-06-07.

The stat-boosting Ripen berry bucket is complete. `StatRaiseBerry`, `RandomStatRaiseBerry`, and `DamagedStatBoostBerryEffect` now use active Ripen membership for stat boost doubling while preserving the existing native Contrary handling in the random-stat helper. The enabled regression confirms pooled Ripen doubles a native Shadow Tag holder's Liechi Berry and Kee Berry boosts; the disabled regression confirms partner Ripen does not double those boosts when Shared Power is off. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles stat-boosting berries"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double stat-boosting berries"` on 2026-06-07.

The standard HP-restoring Ripen berry bucket is complete. `ItemHealHp` now uses active Ripen membership for Oran/Sitrus-style berry healing while preserving the existing holder attribution. The enabled regression confirms pooled Ripen doubles Sitrus Berry healing on a native Shadow Tag holder, and the disabled regression confirms partner Ripen does not double that healing when Shared Power is off. Validation passed with `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles HP-restoring berries"` and `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double HP-restoring berries"` on 2026-06-07.

The remaining Ripen berry-effect bucket is complete. `HealConfuseBerry`, `TrySetEnigmaBerry`, `ItemRestorePp`, and Jaboca/Rowap Berry damage now use active Ripen membership through the holder-owned item-effect path, so pooled Ripen doubles those berry effects while keeping item ownership on the holder. `jumpifability` now treats Ripen as an active-membership check for the existing Ripen item-script popup branches without changing generic ability-script policy. Focused regressions cover pooled and disabled/off-path behavior for Figy/Enigma healing, Leppa PP restoration, and Jaboca/Rowap damage.

The Heal Pulse Mega Launcher bucket is complete. `BS_TryHealPulse` now uses active Mega Launcher membership for the attacker when deciding whether Heal Pulse restores 75% HP, while preserving the existing max-HP failure and Floral Healing terrain branches. Focused regressions cover pooled Mega Launcher from an active partner, Shared Power disabled behavior, and existing native Mega Launcher behavior.

The Cheek Pouch holder berry-effect bucket is complete. `TryCheekPouch` now uses active Cheek Pouch membership for the berry consumer, records `ABILITY_CHEEK_POUCH`, and keeps the berry consumer as the popup and healing battler. The consumed item, `ateBerry` state, Heal Block gate, Symbiosis ordering, and damage-restore behavior remain unchanged.

The direct drain move Liquid Ooze bucket is complete. Absorb-style move-end healing now checks active Liquid Ooze membership on the drained target, records `ABILITY_LIQUID_OOZE` for pooled popups, and Strength Sap's existing Liquid Ooze script branches now resolve through active membership. Leech Seed end-turn Liquid Ooze behavior remains covered by the separate end-turn ownership clarification queue.

The Guard Dog Red Card bucket is complete. Red Card now checks active Guard Dog membership on the attacker before deciding whether the item can force that attacker out. The Red Card holder, item animation/message, item consumption, Commander and Dynamax exceptions, and replacement selection remain unchanged.

The Cute Charm Oblivious contact-infatuation bucket is complete. The Cute Charm move-end target callback now checks active Oblivious membership on the contact attacker before applying infatuation, while Cute Charm ownership, popup attribution, contact avoidance, and Aroma Veil side protection remain unchanged.

The Terastal Adaptability row is complete. `GetTeraMultiplier` now uses active Adaptability membership, so Terastal same-type attack bonus multipliers can benefit from pooled Shared Power Adaptability.

The partner base-power aura bucket is complete. Battery, Power Spot, and Steely Spirit now check active ability membership on the attacker's alive partner in `CalcMoveBasePowerAfterModifiers`, while Flower Gift and Victory Star remain open for later ownership-specific buckets.

The Symbiosis bucket is complete. `TryTriggerSymbiosis` now uses active ability membership on the active partner item donor, and both Symbiosis script paths set `gLastUsedAbility` to `ABILITY_SYMBIOSIS` so pooled Symbiosis displays the triggering shared ability rather than the donor's native slot.

The Sticky Hold item-protection bucket is complete. Item theft/destruction checks now route target-owned Sticky Hold protection through active ability membership, and visible popup paths record `ABILITY_STICKY_HOLD` against the protected item holder. Gorilla Tactics choice-lock behavior remains open for a separate bucket.

The Gorilla Tactics choice-lock bucket is complete. Choice-lock setup, move-selection limitations, selection scripts, and Knock Off lock preservation now use active ability membership for Gorilla Tactics, while the existing damage boost remains handled by effective ability iteration.

The Flower Gift partner aura bucket is complete. Attack and Sp. Def partner aura checks now use active ability membership on the active Sunshine Cherrim partner, while Flower Gift form-change and Cherrim species/form gates remain native-only.

The Victory Star partner accuracy aura bucket is complete. `GetTotalAccuracy` now uses active ability membership for the attacker's alive ally Victory Star boost, while the supplied attacker and defender ability parameters and the broader `AccuracyCheck` target-state contract remain unchanged.

The attack-canceler Magic Bounce ownership bucket is complete. Direct target and target-partner Magic Bounce checks now use active ability membership, while `gBattlerAbility` remains the battler whose effective Magic Bounce caused the reflection. The branch also sets `gLastUsedAbility` to `ABILITY_MAGIC_BOUNCE` before calling the reflection script so pooled owners display Magic Bounce instead of their native slot.

The live `AccuracyCheck` accuracy-math bucket is complete. `Cmd_accuracycheck` now asks a live accuracy wrapper to calculate hit chance from active Shared Power membership for accuracy and evasion abilities, while `GetTotalAccuracy` remains the explicit supplied-ability API for AI callers. The live wrapper also avoids double-counting pooled Victory Star when both the attacker and its ally receive the same shared ability.

The `SetMoveEffect` Flame Burst Magic Guard bucket is complete. Flame Burst's splash-damaged partner now uses active Magic Guard membership for that indirect-damage protection, without migrating the rest of `SetMoveEffect` in this bucket.

The AI hazard setup ability-awareness bucket is complete. `AI_ShouldSetUpHazards` now asks `AI_HasActiveAbility` for Magic Bounce and Shield Dust checks, so Shared Power-enabled AI scoring can avoid setting hazards into pooled active protection without changing broad known-ability cache semantics or treating a non-shared partner's ability as target protection.

Validation evidence for this bucket:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI:"` passed 5 tests, including `Shared Power AI: avoids Stealth Rock when pooled Magic Bounce`.
- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: AI may set Stealth Rock"` passed 1 test.

The `SetMoveEffect` flinch Inner Focus bucket is complete. `SetMoveEffect` now checks active Inner Focus membership for the effect battler when handling `MOVE_EFFECT_FLINCH`, while preserving the existing popup owner and battle-record attribution to `ABILITY_INNER_FOCUS` on the protected battler. Status helper policy, contact callback owner rules, and recoil/item backlash remain separate buckets.

Validation evidence for this bucket:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Inner Focus prevents Fake Out flinch"` passed 1 test.
- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Inner Focus does not prevent Fake Out flinch"` passed 1 test.

The held-item backlash Magic Guard bucket is complete. Rocky Helmet, Jaboca Berry, and Rowap Berry now use active Magic Guard membership on the attacker to suppress indirect backlash damage when Shared Power grants Magic Guard to an attacker whose native ability differs. Contact avoidance, target-held item records, berry ownership, and Ripen's target-side berry modifier remain unchanged.

Validation evidence for this bucket:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents held item backlash"` passed 1 parameterized test covering Rocky Helmet, Jaboca Berry, and Rowap Berry.
- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent held item backlash"` passed 1 parameterized test covering Rocky Helmet, Jaboca Berry, and Rowap Berry.

The half-HP user-loss Magic Guard bucket is complete. `EFFECT_MAX_HP_50_RECOIL` now uses active Magic Guard membership on the attacker before applying the half-max-HP loss used by moves such as Steel Beam and Mind Blown. Regular recoil, crash recoil, Chloroblast/Rock Head behavior, and recoil evolution tracking policy remain outside this bucket.

Validation evidence for this bucket:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents half-HP user-loss moves"` passed 1 parameterized test covering Steel Beam and Mind Blown.
- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent half-HP user-loss moves"` passed 1 parameterized test covering Steel Beam and Mind Blown.

The Spiky Shield Magic Guard bucket is complete. Spiky Shield's move-end contact backlash now uses active Magic Guard membership on the contacting attacker before applying HP loss. Baneful Bunker, Burning Bulwark, King's Shield, Silk Trap, Obstruct, and crash recoil remain outside this bucket.

Validation evidence for this bucket:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents Spiky Shield backlash"` passed 1 test.
- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent Spiky Shield backlash"` passed 1 test.

The holder-side item damage Magic Guard bucket is selected. The intended behavior is that active Shared Power Magic Guard on a Black Sludge or Sticky Barb holder prevents the holder-side HP loss when the holder's native ability is not Magic Guard. Shared Power disabled/off-path cases should still take the item damage.

## Context and Orientation

The audit file classifies ability callsites by the ability view they should use. Rows marked `Stay` should not be disturbed unless a focused audit proves a bug. Rows marked `Migrate` are implementation targets. Rows marked `Clarify` need source, target, popup, duplicate, or AI-knowledge policy before code changes.

The completed priority tranche changed `GetBattleMovePriority` in `src/battle_main.c` and tests in `test/battle/shared_power.c`. It did not migrate native mutation/copy mechanics or form/species gates.

The powder bucket is centered on `IsMovePowderBlocked` in `src/battle_script_commands.c` and `BattleScript_PowderMoveNoEffect` in `data/battle_scripts_1.s`. The intended behavior is:

- Grass-type powder immunity still blocks without an Overcoat popup.
- Safety Goggles still prints the item protection message.
- Native Overcoat still blocks and shows Overcoat.
- Shared Power Overcoat from an active teammate blocks powder moves and shows Overcoat for the protected battler, even when the protected battler's native ability differs.
- Shared Power disabled/off-path cases do not receive Overcoat protection from a partner.

## Plan of Work

Work bucket-by-bucket. For each migrate bucket, inspect the current callsites, write down the source/target/attribution decision, implement the narrowest helper or script change that satisfies the audit, add focused tests, update public gameplay notes if the behavior is player-planning relevant, update `PATCH_NOTES.md`, run filtered validation, and record evidence in this plan.

For the powder bucket, update the C predicate to use `HasActiveAbility(battlerDef, ABILITY_OVERCOAT)` when checking Overcoat. Set `gBattlerAbility` and `gLastUsedAbility` to the protected battler and Overcoat when that ability caused the block, and record Overcoat against the protected battler. Add a dedicated battle script entry for the Overcoat result so Shared Power Overcoat does not fall through to the Safety Goggles message. Add tests for native Overcoat preservation, Shared Power enabled pooled Overcoat, and Shared Power disabled partner Overcoat.

For the move-blocking helper bucket, split ability-block selection from the side effects of running the battle script. First evaluate the supplied `abilityDef` for compatibility with existing callers. In live Shared Power contexts, if that supplied ability does not block, check the protected battler's effective ability membership for target-owned blockers: Soundproof, Bulletproof, and Good as Gold. For priority side blockers, keep the partner-owner model and select Dazzling, Queenly Majesty, or Armor Tail from the ally's native or effective abilities. When a script runs, set `gLastUsedAbility` to the triggering blocker and record that ability against `gBattlerAbility`, not blindly against `battlerDef`.

For the Early Bird bucket, replace native-only Early Bird checks in both sleep-decrement paths with active ability membership. The normal battle path should still record Early Bird when it contributes the faster decrement. The Battle Palace path should use the same active membership, without adding broader Battle Palace policy changes.

For the speed-calculation remainder bucket, keep the scope to `GetBattlerTotalSpeedStatArgs`: build local effective-membership booleans for Surge Surfer, Slow Start, Protosynthesis, Quark Drive, and Unburden in the same style as Quick Feet and weather speed abilities, then use those booleans in speed math. Do not change booster-energy consumption, Slow Start timer setup, or Unburden activation policy in this bucket.

For the infatuation/Oblivious script-command bucket, replace direct native Oblivious checks in `Cmd_tryinfatuating`, `Cmd_jumpifcaptivateaffected`, and `BS_TrySetInfatuation` with active ability membership for the target. Preserve existing popup and battle-record behavior for Attract and Captivate by continuing to set `gLastUsedAbility` to `ABILITY_OBLIVIOUS` and record Oblivious against the target when the ability blocks. Do not add a new popup to the side-wide `BS_TrySetInfatuation` loop in this bucket; that path currently fails the individual target silently when the target is protected, and this bucket only changes which ability view determines protection.

For the party-status Heal Bell Soundproof bucket, replace active attacker and active doubles-partner Soundproof checks in `Cmd_healpartystatus` with active ability membership. When Soundproof blocks an active battler's cure, record `ABILITY_SOUNDPROOF` against that protected battler rather than recording the native slot ability. Keep inactive party member checks native-only, because inactive party members are not active Shared Power recipients.

For the accuracy-skip No Guard contract bucket, update `CanMoveSkipAccuracyCalc` so `AI_CHECK` honors the supplied `abilityAtk` and `abilityDef` parameters, while `RUN_SCRIPT` uses active ability membership for live Shared Power behavior. When live No Guard causes an accuracy skip, record `ABILITY_NO_GUARD` against the battler whose active ability membership supplied it, preferring the attacker if both attacker and defender have active No Guard. Do not migrate the broader `AccuracyCheck` Dragon Darts/global-target behavior in this bucket.

For the Terastal Adaptability row, update `GetTeraMultiplier` in `src/battle_terastal.c` so its Adaptability check uses active ability membership. Add a focused Shared Power test proving a non-Adaptability native battler receives the higher Adaptability Tera STAB multiplier from a Shared Power party ability, plus a disabled/off-path comparison proving the same party ability does not affect Tera STAB without Shared Power.

For the partner base-power aura bucket, replace the native `GetBattlerAbility(BATTLE_PARTNER(battlerAtk))` switch in `CalcMoveBasePowerAfterModifiers` with active ability membership checks for Battery, Power Spot, and Steely Spirit on the attacker's alive partner. Preserve their existing conditions: Battery only boosts special moves, Power Spot boosts all moves, and Steely Spirit only boosts Steel-type moves. Leave Flower Gift and Victory Star for separate buckets.

For the Symbiosis bucket, replace the native partner Symbiosis gate in `TryTriggerSymbiosis` with active ability membership on the active partner. Keep the existing item donor as `BATTLE_PARTNER(battler)`, keep `gBattlerAbility` / popup ownership on that donor, and set `gLastUsedAbility` to `ABILITY_SYMBIOSIS` when the shared ability triggers. Do not change the item transfer restrictions, gem timing, Fling handling, Cheek Pouch ordering, or inactive-party semantics.

For the Sticky Hold item-protection bucket, migrate direct target Sticky Hold checks in item-loss and item-steal paths to active ability membership on the item holder. Preserve popup and battle-history attribution by setting `gLastUsedAbility` to `ABILITY_STICKY_HOLD`, `gBattlerAbility` to the protected item holder, and recording Sticky Hold against that holder. Keep generic `jumpifability` native-only except for the Sticky-Hold-specific default-battler branch used by item-protection scripts. Leave Gorilla Tactics choiced-move locking and Knock Off's choiced-move reset for a later choice-lock bucket.

For the Gorilla Tactics choice-lock bucket, replace native Gorilla Tactics checks in choice-lock setup, move-selection limitations, selection scripts, and Knock Off choiced-move preservation with active ability membership. Keep the existing physical damage boost on the damage modifier iterator. Do not change AI known-ability or hidden-information behavior in this bucket.

For the Flower Gift partner aura bucket, replace native partner Flower Gift checks in attack and defense stat calculation with active ability membership on the active Cherrim partner. Preserve the existing aura conditions: the partner must be `SPECIES_CHERRIM_SUNSHINE`, weather-affected by sun, and the affected stat must be Attack for physical moves or Sp. Def for special defense. Do not migrate Flower Gift form transformation, suppression-driven reversion, or any Cherrim species/form gates.

For the Victory Star partner accuracy aura bucket, replace the native attacker-partner Victory Star check in `GetTotalAccuracy` with active ability membership on the alive attacker ally. Do not change `GetTotalAccuracy`'s supplied `atkAbility` / `defAbility` contract, Dragon Darts target handling, AI cached ability semantics, or other accuracy modifier ownership in this bucket.

For the attack-canceler Magic Bounce bucket, replace native Magic Bounce checks in `Cmd_attackcanceler` with active ability membership on the protected target or selected target-side partner. Preserve the existing popup owner by keeping `gBattlerAbility` on the battler whose effective Magic Bounce caused the bounce. Do not migrate generic status helper behavior, contact helper behavior, Magic Coat, or broader `jumpifability` policy in this bucket.

For the live `AccuracyCheck` accuracy-math bucket, keep `GetTotalAccuracy` as the explicit supplied-ability API for AI and add a live wrapper that uses active ability membership for attacker accuracy modifiers, defender evasion modifiers, stage-ignore abilities, Wonder Skin, and Micle Berry Ripen. Route `Cmd_accuracycheck` through the live wrapper and use active Skill Link membership for the live multi-hit accuracy skip. Do not change AI hidden-information semantics, Dragon Darts retarget policy, or the broader type-effectiveness `DamageContext` contract in this bucket.

For the `SetMoveEffect` Flame Burst Magic Guard bucket, replace the native Magic Guard check on `BATTLE_PARTNER(gBattlerTarget)` with active ability membership on that splash-damaged partner. Add Shared Power enabled and disabled double-battle tests where the splash-damaged partner's native ability differs from Magic Guard, and keep native Magic Guard Flame Burst behavior covered by existing passive-damage/Retaliate validation.

## Validation and Acceptance

For the umbrella sweep, acceptance requires every `Migrate` row in `docs/design/shared_power/ability_usage_audit.md` to be implemented and validated, or explicitly deferred with a rationale and next policy question. Clarify rows may remain deferred if their policy question is documented.

For the powder bucket, acceptance requires filtered tests proving:

- Native Overcoat still blocks powder moves and shows Overcoat.
- Shared Power pooled Overcoat blocks a powder move when the protected battler's native ability is not Overcoat.
- The same partner Overcoat does not block when Shared Power is disabled.
- Safety Goggles and Grass-type powder immunity are not regressed by the script routing change.

For the move-blocking helper bucket, acceptance requires filtered tests proving:

- A pooled target-owned blocker such as Soundproof blocks a matching move when the protected battler's native ability differs.
- The same partner blocker does not protect the target when Shared Power is disabled.
- Existing native blocker tests for Soundproof or Good as Gold still pass.
- Existing ally priority-blocker tests for Dazzling, Queenly Majesty, and Armor Tail still pass, preserving popup attribution.

For the Early Bird bucket, acceptance requires filtered tests proving:

- A sleeping battler with a non-Early-Bird native ability wakes faster when an active Shared Power teammate contributes Early Bird.
- The same partner Early Bird does not shorten sleep when Shared Power is disabled.
- The existing generic sleep-turn test still passes.

For the speed-calculation remainder bucket, acceptance requires filtered tests proving:

- A non-Surge-Surfer native battler moves first under Electric Terrain when an active Shared Power teammate contributes Surge Surfer.
- The same partner Surge Surfer does not change order when Shared Power is disabled.
- Existing speed behavior for pooled Swift Swim and Quick Feet still passes.

For the infatuation/Oblivious script-command bucket, acceptance requires filtered tests proving:

- A target with a non-Oblivious native ability is protected from Attract when an active Shared Power teammate contributes Oblivious, including the existing Oblivious popup.
- The same partner Oblivious does not block Attract when Shared Power is disabled.
- Existing native Oblivious Attract behavior still passes.

For the party-status Heal Bell Soundproof bucket, acceptance requires filtered tests proving:

- A non-Soundproof active partner remains statused after Heal Bell when an active Shared Power teammate contributes Soundproof.
- The same partner Soundproof does not block the cure when Shared Power is disabled.
- Existing native Heal Bell Soundproof behavior still passes.

For the accuracy-skip No Guard contract bucket, acceptance requires filtered tests proving:

- A non-No-Guard attacker always hits an inaccurate move when the target side contributes pooled No Guard and the defender's native ability differs.
- The same partner No Guard does not bypass accuracy when Shared Power is disabled.
- Existing pooled attacker-side No Guard OHKO behavior still passes.

For the Terastal Adaptability row, acceptance requires filtered tests proving:

- A non-Adaptability native battler gains Adaptability's stronger Tera STAB multiplier from Shared Power.
- The same party Adaptability does not change Tera STAB when Shared Power is disabled.
- Existing native Tera Adaptability tests still pass.

For the partner base-power aura bucket, acceptance requires filtered tests proving:

- A special move from an active non-Battery attacker is boosted when its active non-Battery partner has pooled Battery through Shared Power.
- The same party Battery does not boost the move when Shared Power is disabled.
- Existing or focused native Battery behavior still passes.

For the Symbiosis bucket, acceptance requires filtered tests proving:

- A non-Symbiosis active partner passes its held item when its active Shared Power pool includes Symbiosis.
- The same party Symbiosis does not trigger when Shared Power is disabled.
- Existing native Symbiosis item-passing behavior still passes.

For the Sticky Hold item-protection bucket, acceptance requires filtered tests proving:

- A non-Sticky-Hold item holder is protected from item theft or destruction when its active Shared Power pool includes Sticky Hold.
- The same party Sticky Hold does not protect the item holder when Shared Power is disabled.
- Existing native Sticky Hold item-protection behavior still passes.

For the Gorilla Tactics choice-lock bucket, acceptance requires filtered tests proving:

- A non-Gorilla-Tactics native battler becomes choice locked when its active Shared Power pool includes Gorilla Tactics.
- The same active partner Gorilla Tactics does not choice lock the battler when Shared Power is disabled.
- Existing or focused native Gorilla Tactics behavior still passes.

For the Flower Gift partner aura bucket, acceptance requires filtered tests proving:

- A physical move from an active non-Flower-Gift attacker is boosted when its active Sunshine Cherrim partner has pooled Flower Gift.
- The same Sunshine Cherrim partner does not boost the move when Shared Power is disabled and its native ability differs.
- Existing native Flower Gift Attack and Sp. Def aura behavior still passes.

For the Victory Star partner accuracy aura bucket, acceptance requires filtered tests proving:

- A non-Victory-Star native attacker gets the 1.1x partner accuracy boost when its active ally has pooled Victory Star.
- The same switched-in ally does not boost accuracy when Shared Power is disabled and its native ability differs.
- Existing or focused native Victory Star accuracy behavior still passes.

For the attack-canceler Magic Bounce bucket, acceptance requires filtered tests proving:

- A target with a non-Magic-Bounce native ability bounces a status move when its active Shared Power pool includes Magic Bounce.
- The same partner Magic Bounce does not bounce the target's status move when Shared Power is disabled.
- Existing native Magic Bounce status and field-targeting behavior still passes.

Validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Bounce"` initially failed with an unmatched Magic Bounce popup, exposing that the pooled reflection owner needed `gLastUsedAbility = ABILITY_MAGIC_BOUNCE`.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Bounce"` passed after the popup attribution fix (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Bounce"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Magic Bounce bounces back"` passed (4 tests).

For the live `AccuracyCheck` accuracy-math bucket, acceptance requires filtered tests proving:

- An active teammate with pooled Compound Eyes improves the attacker's live accuracy calculation.
- The same partner Compound Eyes does not improve accuracy when Shared Power is disabled.
- An active teammate with pooled Sand Veil improves the defender's live evasion in sand.
- The same partner Sand Veil does not improve evasion when Shared Power is disabled.
- Existing native Compound Eyes and Sand Veil accuracy behavior still passes.

Validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Compound Eyes"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Compound Eyes"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Sand Veil"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Sand Veil"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Compound Eyes"` passed (2 tests).
- `make check NO_MULTIBOOT=1 TESTS="Sand Veil"` passed (3 tests).
- After adding the live Victory Star duplicate-source guard, `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Victory Star"` passed (1 test), `TESTS="Shared Power off: pooled Victory Star"` passed (1 test), and `TESTS="Victory Star raises"` passed (1 test).
- After the same guard, `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Compound Eyes"` and `TESTS="Shared Power: pooled Sand Veil"` each passed again (1 test each).

For the `SetMoveEffect` Flame Burst Magic Guard bucket, acceptance requires filtered tests proving:

- A splash-damaged active partner with non-Magic-Guard native ability avoids Flame Burst splash damage when its active Shared Power pool includes Magic Guard.
- The same partner Magic Guard does not block splash damage when Shared Power is disabled.
- Existing native Magic Guard passive-damage behavior involving Flame Burst remains intact.

Validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Magic Guard prevents Flame Burst"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent Flame Burst"` passed (1 test).
- `make check NO_MULTIBOOT=1 TESTS="Retaliate works with passive damage"` passed (1 test).
- `git diff --check` passed after removing unrelated trailing spaces from existing Shared Power Adaptability tests in the dirty diff.

## Idempotence and Recovery

Edits should remain local to the current bucket. If a bucket breaks unrelated battle behavior, revert only that bucket's code/test/doc edits and record the failed evidence here. Do not revert unrelated user or agent work.

## Artifacts and Notes

Powder-move Overcoat validation evidence:

- `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Overcoat"`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Overcoat"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Overcoat blocks powder"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Safety Goggles"` in the same Docker command form: 4 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Powder moves are blocked by Grass-type"` in the same Docker command form: 1 passed, 0 failed.

Move-blocking helper validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Psychic Noise is blocked by Soundproof"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Good as Gold"` in the same Docker command form: 4 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Dazzling, Queenly Majesty and Armor Tail"` in the same Docker command form: 4 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Syrup Bomb is prevented by Bulletproof"` in the same Docker command form: 1 passed, 0 failed.

Early Bird sleep-decrement validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Early Bird"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Early Bird"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Sleep prevents the battler from using a move"` in the same Docker command form: 1 passed, 0 failed.

Speed-calculation remainder validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: Surge Surfer"` in the same Docker command form failed to compile because the test used `SPECIES_RAICHU_ALOLAN`; the repo defines `SPECIES_RAICHU_ALOLA`.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: Surge Surfer"` in the same Docker command form after fixing the species constant: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Surge Surfer"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: Quick Feet"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: Swift Swim"` in the same Docker command form: 1 passed, 0 failed.

Infatuation/Oblivious script-command validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Oblivious"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Oblivious"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Oblivious prevents Infatuation"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Oblivious prevents Captivate"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Dynamax: G-Max Cuddle infatuates both opponents"` in the same Docker command form: 1 passed, 0 failed.

Party-status Heal Bell Soundproof validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof blocks Heal Bell"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof does not block Heal Bell"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Heal Bell does not cure Soundproof partners"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Heal Bell cures a Soundproof user"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Heal Bell cures inactive Soundproof"` in the same Docker command form: 1 passed, 0 failed.

Accuracy-skip No Guard contract validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: target-side pooled No Guard"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner No Guard"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: No Guard"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="OHKO moves can hit semi-invulnerable"` in the same Docker command form: 1 passed, 0 failed.

Terastal Adaptability validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Adaptability strengthens Terastal STAB"` in the same Docker command form failed before the code fix: both damage samples were 36, proving `GetTeraMultiplier` still used native ability lookup.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Adaptability strengthens Terastal STAB"` in the same Docker command form after migrating `GetTeraMultiplier`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: party Adaptability"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="(TERA) Terastallizing into a different type with Adaptability"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="(TERA) Terastallizing into the same type with Adaptability"` in the same Docker command form: 1 passed, 0 failed.

Partner base-power aura validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Battery"` in the same Docker command form failed as invalid because the setup turn used Celebrate on a Pokemon with an explicit Water Gun-only move list.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Battery"` in the same Docker command form after declaring Celebrate in the move list and explicit scene transcript: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: party Battery"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Battery increases Sp. Attack"` in the same Docker command form: 1 passed, 0 failed.

Symbiosis validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Symbiosis"` in the same Docker command form failed because the test had a bench-only Symbiosis source that had not entered the Shared Power pool.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Symbiosis"` in the same Docker command form after seeding the pool from an active Symbiosis source and switching to the non-Symbiosis item donor: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: party Symbiosis"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Symbiosis transfers its item"` in the same Docker command form: 2 passed, 0 failed.

Sticky Hold item-protection validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Sticky Hold"` in the same Docker command form failed to compile because `HasStickyHoldItemProtection` was defined after its first use in `SetMoveEffect`.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Sticky Hold"` in the same Docker command form after adding the helper declaration: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Sticky Hold"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Sticky Hold prevents item theft"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Corrosive Gas doesn't destroy the item"` in the same Docker command form: 1 passed, 0 failed, 1 TODO.
- `make check NO_MULTIBOOT=1 TESTS="Knock Off doesn't remove item if it's prevented by Sticky Hold"` in the same Docker command form: 1 passed, 0 failed.

Gorilla Tactics choice-lock validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Gorilla Tactics"` in the same Docker command form failed to compile because the Shared Power tests indexed `choicedMove` with battler object aliases rather than battler indexes.
- The next `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Gorilla Tactics"` in the same Docker command form failed to compile because the native Gorilla Tactics guard had the same battler-index assertion issue.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Gorilla Tactics"` in the same Docker command form after using `GetBattlerAtPosition`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Gorilla Tactics"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Gorilla Tactics locks"` in the same Docker command form: 1 passed, 0 failed.

Flower Gift partner aura validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Flower Gift"` in the same Docker command form failed as invalid because the test used Sunny Day on a Pokemon with an explicit move list that omitted Sunny Day.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Flower Gift"` in the same Docker command form after declaring Sunny Day: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: pooled Flower Gift"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Flower Gift increases"` in the same Docker command form: 2 passed, 0 failed.

Victory Star partner accuracy aura validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Victory Star"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: pooled Victory Star"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Victory Star raises"` in the same Docker command form: 1 passed, 0 failed.

Remaining Ripen berry-effect validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles remaining healing berries"` in the same Docker command form failed on an unmatched Ripen popup assertion; the regression was narrowed to behavior/HP assertions because existing Ripen item coverage does not require every berry script to show a popup.
- The next `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles remaining healing berries"` in the same Docker command form failed because the Figy holder started below the berry activation threshold.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles remaining healing berries"` in the same Docker command form after raising the Figy holder's starting HP: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double remaining healing berries"` in the same Docker command form: 1 passed, 0 failed.
- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles Leppa Berry PP restoration"` in the same Docker command form failed because the test expected 21 PP instead of the correct post-use restored value of 20 PP.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles Leppa Berry PP restoration"` in the same Docker command form after correcting the expected PP: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double Leppa Berry PP restoration"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles Jaboca and Rowap Berry damage"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double Jaboca and Rowap Berry damage"` in the same Docker command form: 1 passed, 0 failed.
- Regression checks after introducing `TryRecordActiveRipen`: `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles stat-boosting berries"` passed; `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Ripen doubles HP-restoring berries"` passed; `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Ripen does not double HP-restoring berries"` passed.
- `rg -n "GetBattlerAbility\\([^)]*\\) == ABILITY_RIPEN|GetBattlerAbility\\([^)]*\\).*ABILITY_RIPEN" src/battle_util.c src/battle_script_commands.c src/battle_main.c`: no remaining matches.
- `git diff --check`: passed.

Heal Pulse Mega Launcher validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Mega Launcher boosts Heal Pulse"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Mega Launcher does not boost Heal Pulse"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Heal Pulse is boosted by Mega Launcher"` in the same Docker command form: 1 passed, 0 failed.
- `rg -n "GetBattlerAbility\\(gBattlerAttacker\\) == ABILITY_MEGA_LAUNCHER|ABILITY_MEGA_LAUNCHER" src/battle_script_commands.c src/battle_util.c src/battle_main.c`: only the active-aware Heal Pulse check and the expected ability switch entry remain.
- `git diff --check`: passed.

Cheek Pouch holder berry-effect validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Cheek Pouch heals after berry consumption"` in the same Docker command form failed on the wrong expected Cheek Pouch heal message text.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Cheek Pouch heals after berry consumption"` in the same Docker command form after correcting the expected message: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Cheek Pouch does not heal after berry consumption"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Cheek Pouch activation doesn't mutate damage"` in the same Docker command form: 1 passed, 0 failed.
- `rg -n "GetBattlerAbility\\([^)]*\\) == ABILITY_CHEEK_POUCH|ABILITY_CHEEK_POUCH" src/battle_script_commands.c src/battle_util.c src/battle_main.c test/battle/shared_power.c`: only active-aware code/tests remain.
- `git diff --check`: passed.

Direct drain move Liquid Ooze validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Liquid Ooze punishes Absorb"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Liquid Ooze does not punish Absorb"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Liquid Ooze punishes Strength Sap"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Liquid Ooze does not punish Strength Sap"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Liquid Ooze causes Absorb users"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Liquid Ooze causes Strength Sap users"` in the same Docker command form: 1 passed, 0 failed.
- `rg -n "GetBattlerAbility\\([^)]*\\) [!=]= ABILITY_LIQUID_OOZE|ABILITY_LIQUID_OOZE" src/battle_script_commands.c src/battle_util.c src/battle_main.c data/battle_scripts_1.s test/battle/shared_power.c`: only active-aware code, Strength Sap script branches, and tests remain for direct drain paths.
- `git diff --check`: initially found unrelated trailing whitespace in existing Shared Power coverage; after removing that space, it passed.

Guard Dog Red Card validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Guard Dog prevents Red Card"` in the same Docker command form failed as invalid because the bench replacement lacked explicit Speed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Guard Dog prevents Red Card"` in the same Docker command form after adding replacement Speed: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Guard Dog does not prevent Red Card"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Red Card activates but fails if the attacker has Guard Dog"` in the same Docker command form: 1 passed, 0 failed.
- `rg -n "GetBattlerAbility\\([^)]*\\) == ABILITY_GUARD_DOG|ABILITY_GUARD_DOG" src/battle_script_commands.c src/battle_util.c src/battle_main.c test/battle/shared_power.c`: Red Card now uses active membership; remaining hits are supplied-ability/native tests and existing active-aware Guard Dog handling.
- `git diff --check`: passed.

Cute Charm Oblivious validation evidence:

- Initial `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Oblivious blocks Cute Charm"` in the same Docker command form failed to compile because `EXPECT_EQ` cannot be used directly on the `volatiles.infatuation` bit-field.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Oblivious blocks Cute Charm"` in the same Docker command form after switching to `EXPECT(!...)`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Oblivious does not block Cute Charm"` in the same Docker command form: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Cute Charm inflicts infatuation on contact"` in the same Docker command form: 1 passed, 0 failed.
- `rg -n "GetBattlerAbility\\([^)]*\\).*ABILITY_OBLIVIOUS|ABILITY_OBLIVIOUS" src/battle_util.c src/battle_script_commands.c test/battle/shared_power.c`: Cute Charm's Oblivious check is now active-aware; remaining hits are existing active-aware checks/tests or unrelated supplied-ability tables.
- `git diff --check`: passed.

Revision Note (2026-06-06): Initial umbrella ExecPlan created for the broader Shared Power ability-callsite sweep and the next powder-move Overcoat bucket.

Revision Note (2026-06-06): Recorded implementation and targeted validation evidence for the powder-move Overcoat bucket.

Revision Note (2026-06-06): Selected the `CanAbilityBlockMove` helper as the next bucket and recorded the live-vs-AI scope decision before code edits.

Revision Note (2026-06-06): Recorded implementation and targeted validation evidence for the move-blocking helper bucket.

Revision Note (2026-06-06): Selected the Early Bird sleep-decrement bucket and recorded the normal battle plus Battle Palace implementation scope before code edits.

Revision Note (2026-06-06): Recorded the Early Bird off-path test DSL validation finding and updated the test to use `NONE_OF`.

Revision Note (2026-06-06): Recorded implementation and targeted validation evidence for the Early Bird sleep-decrement bucket.

Revision Note (2026-06-06): Selected the speed-calculation remainder bucket and recorded the activation-state boundary before code edits.

Revision Note (2026-06-06): Recorded the Surge Surfer test species constant validation finding and updated the tests to use `SPECIES_RAICHU_ALOLA`.

Revision Note (2026-06-06): Recorded implementation and targeted validation evidence for the speed-calculation remainder bucket.

Revision Note (2026-06-06): Selected the infatuation/Oblivious script-command bucket and recorded the boundary between direct target gates and callback ownership work.

Revision Note (2026-06-06): Recorded implementation progress for the infatuation/Oblivious script-command bucket before targeted validation.

Revision Note (2026-06-06): Recorded targeted validation evidence for the infatuation/Oblivious script-command bucket.

Revision Note (2026-06-07): Selected the party-status Heal Bell Soundproof bucket and recorded the active-battler versus inactive-party boundary.

Revision Note (2026-06-07): Recorded implementation progress for the party-status Heal Bell Soundproof bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the party-status Heal Bell Soundproof bucket.

Revision Note (2026-06-07): Selected the accuracy-skip No Guard contract bucket and recorded the boundary that leaves broader `AccuracyCheck` migration open.

Revision Note (2026-06-07): Recorded implementation progress for the accuracy-skip No Guard contract bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the accuracy-skip No Guard contract bucket and noted that broader `AccuracyCheck` migration remains open.

Revision Note (2026-06-07): Initially recorded Terastal Adaptability as production-code complete via the normal STAB helper and selected focused regression coverage.

Revision Note (2026-06-07): Added Terastal Adaptability Shared Power regression coverage and public gameplay note before targeted validation.

Revision Note (2026-06-07): Corrected the Terastal Adaptability plan after the regression exposed that `GetTeraMultiplier` still used native ability lookup, then migrated that Tera-specific helper to active ability membership.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Terastal Adaptability row, including the failed-before evidence that confirmed the migration was necessary.

Revision Note (2026-06-07): Selected the partner base-power aura bucket for Battery, Power Spot, and Steely Spirit, leaving Flower Gift and Victory Star for later focused ownership work.

Revision Note (2026-06-07): Recorded implementation progress for the partner base-power aura bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the partner base-power aura bucket and the Battery test explicit-move-list finding.

Revision Note (2026-06-07): Selected the Symbiosis bucket and recorded the active-partner donor and popup ownership model before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the Symbiosis bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Symbiosis bucket and the active-source pool seeding correction in its regression test.

Revision Note (2026-06-07): Selected the Sticky Hold item-protection bucket and recorded the boundary that leaves Gorilla Tactics choice-lock behavior for a separate bucket.

Revision Note (2026-06-07): Recorded implementation progress for the Sticky Hold item-protection bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Sticky Hold item-protection bucket, including the helper declaration compile fix.

Revision Note (2026-06-07): Selected the Gorilla Tactics choice-lock bucket and recorded that AI choice reasoning remains outside this bucket.

Revision Note (2026-06-07): Recorded implementation progress for the Gorilla Tactics choice-lock bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Gorilla Tactics choice-lock bucket and the battler-index assertion fixes in its tests.

Revision Note (2026-06-07): Selected the Flower Gift partner aura bucket and recorded that Cherrim form-change mechanics stay native-only.

Revision Note (2026-06-07): Recorded implementation progress for the Flower Gift partner aura bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Flower Gift partner aura bucket and the Sunny Day explicit move-list fix.

Revision Note (2026-06-07): Selected the Victory Star partner accuracy aura bucket while leaving the broader `GetTotalAccuracy` / `AccuracyCheck` contract open.

Revision Note (2026-06-07): Recorded implementation progress for the Victory Star partner accuracy aura bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Victory Star partner accuracy aura bucket.

Revision Note (2026-06-07): Selected the attack-canceler Magic Bounce ownership bucket and recorded its boundary from broader status/contact/accuracy contracts.

Revision Note (2026-06-07): Recorded implementation progress for the attack-canceler Magic Bounce ownership bucket before targeted validation.

Revision Note (2026-06-07): Corrected pooled Magic Bounce popup attribution after targeted validation reported an unmatched Magic Bounce popup.

Revision Note (2026-06-07): Recorded targeted validation evidence for the attack-canceler Magic Bounce ownership bucket.

Revision Note (2026-06-07): Selected and implemented the live `AccuracyCheck` accuracy-math bucket while preserving AI supplied-ability semantics and leaving broader `DamageContext` policy separate.

Revision Note (2026-06-07): Recorded targeted validation evidence for the live `AccuracyCheck` accuracy-math bucket.

Revision Note (2026-06-07): Refined live accuracy math to avoid double-counting pooled Victory Star as both an attacker modifier and ally aura.

Revision Note (2026-06-07): Selected the `SetMoveEffect` Flame Burst Magic Guard bucket and recorded its narrow boundary from broader status helper and native mutation policy.

Revision Note (2026-06-07): Recorded implementation progress for the `SetMoveEffect` Flame Burst Magic Guard bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the `SetMoveEffect` Flame Burst Magic Guard bucket.

Revision Note (2026-06-07): Selected the remaining Ripen berry-effect bucket and recorded its holder-owned item-effect boundary before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the remaining Ripen berry-effect bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the remaining Ripen berry-effect bucket, including the Figy threshold and Leppa post-use PP corrections.

Revision Note (2026-06-07): Selected the Heal Pulse Mega Launcher bucket and recorded its attacker-owned support-move boundary before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the Heal Pulse Mega Launcher bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Heal Pulse Mega Launcher bucket.

Revision Note (2026-06-07): Selected the Cheek Pouch holder berry-effect bucket and recorded its holder-owned attribution boundary before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the Cheek Pouch holder berry-effect bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Cheek Pouch holder berry-effect bucket, including the corrected ability-heal message text.

Revision Note (2026-06-07): Selected the direct drain move Liquid Ooze bucket and recorded the boundary that leaves Leech Seed ownership separate.

Revision Note (2026-06-07): Recorded implementation progress for the direct drain move Liquid Ooze bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the direct drain move Liquid Ooze bucket and the whitespace cleanup needed for `git diff --check`.

Revision Note (2026-06-07): Selected the Guard Dog Red Card bucket and recorded its attacker-owned forced-switch prevention boundary before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the Guard Dog Red Card bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Guard Dog Red Card bucket, including the explicit-Speed test harness correction.

Revision Note (2026-06-07): Selected the Cute Charm Oblivious contact-infatuation bucket and recorded its attacker-protection boundary before code edits.

Revision Note (2026-06-07): Recorded implementation progress for the Cute Charm Oblivious bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Cute Charm Oblivious bucket, including the bit-field assertion correction.

Revision Note (2026-06-07): Selected and implemented the move-end Mirror Armor contact-reflection bucket for Gooey and Tangling Hair, preserving target-owned Gooey/Tangling Hair attribution while making the contact attacker's Mirror Armor active-membership aware.

Revision Note (2026-06-07): Recorded targeted validation evidence for the move-end Mirror Armor contact-reflection bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Mirror Armor reflects Tangling Hair at minimum Speed"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Mirror Armor does not reflect Tangling Hair at minimum Speed"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Mirror Armor reflects Tangling Hair speed drop"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the move-end Mirror Armor contact-reflection bucket.

Revision Note (2026-06-07): Selected the Own Tempo standard-confusion bucket and recorded its boundary from Sky Drop release/cancel state.

Revision Note (2026-06-07): Recorded implementation progress for the Own Tempo standard-confusion bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Own Tempo standard-confusion bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Own Tempo prevents confusion from moves"` initially failed to compile because `player->volatiles.confusionTurns` is a bit-field; after changing the assertion to `EXPECT(!player->volatiles.confusionTurns)`, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Own Tempo does not prevent confusion from moves"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Own Tempo prevents Berserk Gene confusion"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Own Tempo does not prevent Berserk Gene confusion"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Own Tempo prevents confusion from moves by the opponent"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Mold Breaker ignores Own Tempo"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Own Tempo prevents confusion from items"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Own Tempo standard-confusion bucket.

Revision Note (2026-06-07): Selected the Toxic Chain Knock Off berry-cure bucket and recorded its boundary from broader Poison Touch/Toxic Chain callback ownership.

Revision Note (2026-06-07): Recorded implementation progress for the Toxic Chain Knock Off berry-cure bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Toxic Chain Knock Off berry-cure bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Toxic Chain lets Pecha Berry cure before Knock Off"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Toxic Chain does not cure berry before Knock Off"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Toxic Chain makes Lum/Pecha Berry trigger before being knocked off"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Toxic Chain Knock Off berry-cure bucket.

Revision Note (2026-06-07): Selected the Poké Flute active Soundproof bucket and recorded its boundary from inactive party wake-up rules.

Revision Note (2026-06-07): Recorded implementation progress for the Poké Flute active Soundproof bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Poké Flute active Soundproof bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof blocks Poke Flute from waking an active teammate"` initially failed after the active loop migration because active party slots were still woken by the party-status update; after making active party slots use `HasActiveAbility`, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof does not block Poke Flute"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Poké Flute active Soundproof bucket.

Revision Note (2026-06-07): Selected the Unseen Fist protect-bypass bucket and recorded that it preserves the existing contact-helper contract.

Revision Note (2026-06-07): Recorded implementation progress for the Unseen Fist protect-bypass bucket before targeted validation.

Revision Note (2026-06-07): Recorded targeted validation evidence for the Unseen Fist protect-bypass bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Unseen Fist bypasses Protect with contact moves"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Unseen Fist does not bypass Protect"` initially produced an invalid test due adjacent `NOT` assertions; after grouping them in `NONE_OF`, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Unseen Fist bypasses Protect with contact moves"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Unseen Fist protect-bypass bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Lightning Rod keeps Electric moves on the selected active target"` initially failed after the redirection-only implementation because the selected pooled target did not absorb; after adding live absorption ability resolution, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: native Lightning Rod source redirects from the selected target"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Storm Drain keeps Water moves on the selected active target"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: native Storm Drain source redirects from the selected target"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Lightning Rod forces single-target Electric-type moves to target the Pokémon with this Ability."` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Storm Drain forces single-target Water-type moves to target the Pokémon with this Ability."` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Lightning Rod and Storm Drain redirection/absorption bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Infiltrator bypasses Substitute"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Infiltrator does not bypass Substitute"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Infiltrator bypasses Mist stat protection"` initially failed because the post-turn stat-stage pointer assertion read a stale/invalid value despite the fall transcript matching; after switching the test to transcript assertions, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Infiltrator does not bypass Mist stat protection"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Infiltrator bypasses Substitute"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Infiltrator bypasses Mist stat protection"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Infiltrator bypass bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Klutz suppresses held item effects"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Klutz does not suppress held item effects"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Klutz prevents Fling"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Klutz does not prevent Fling"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Klutz makes Poltergeist fail"` initially failed on brittle generic failure text; after switching to the no-hit outcome and forcing the accuracy roll to hit, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Klutz does not make Poltergeist fail"` initially failed on brittle item-reveal text and then on the accuracy roll; after asserting the hit outcome and forcing the roll to hit, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Klutz suppresses held item effects"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Klutz makes Poltergeist fail"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Fling fails for Pokémon with Klutz ability"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the Klutz item-suppression bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the new Wonder Guard switch fixture trapped the AI with Shadow Tag; after changing the switched-in target's native ability to Telepathy, it passed 6/6.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: AI does not switch out for partner Wonder Guard"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="AI_FLAG_SMART_SWITCHING: AI will switch out if it can't deal damage to a mon with Wonder Guard"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Wonder Guard switch-recognition bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed to compile because the new AI test included `battle_ai_util.h` before `test/battle.h`; after matching the existing AI test include order, it passed 8/8.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not prevent secondary damage prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Poison Heal does not prevent poison damage prediction"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI secondary-damage prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially found the new guaranteed-flinch fixture invalid because AI tests require Speed on every battler; after giving the donor explicit Speed, it passed 9/9.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Inner Focus does not prevent guaranteed flinch prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="AI prefers to Fake Out the opponent vulnerable to flinching."` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI guaranteed-flinch prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 10/10 after adding the AI Heal Bell Soundproof prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: AI may use Heal Bell for a non-Soundproof active partner"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="AI chooses moves that cure self or partner"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Heal Bell Soundproof prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 11/11 after adding the AI Comatose sleep-state prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Comatose does not make active battlers asleep"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Sleep Talk works if user has Comatose"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Comatose sleep-state prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the move-choice fixture expected `Celebrate` on a switch turn, then produced invalid double score fixtures; after switching the coverage to direct `AI_IsAffectedByPowder` helper assertions, it passed 12/12.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Overcoat does not block powder prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Overcoat blocks powder and spore moves"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI powder-move Overcoat prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the first-turn switch fixture expected `Celebrate` but AI chose `Perish Song`; after accepting the first-turn Perish Song and preserving the second-turn score assertion, it passed 13/13.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof does not make Perish Song score bad"` initially showed an unrelated score of 93 rather than 100; after asserting the score remains above 90 instead of equal to default, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Soundproof prevents Perish Song timers"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Perish Song Soundproof prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially produced invalid score fixtures for the Roar bucket; after narrowing the coverage to the active Soundproof prediction adapter used by Roar scoring, it passed 14/14.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Soundproof does not block Roar prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Roar switches the target with a random non-fainted replacement"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Roar Soundproof prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 16/16 after adding the AI weather/terrain benefit prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Swift Swim does not make rain beneficial"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Surge Surfer does not make Electric Terrain beneficial"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI weather/terrain benefit prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the pooled Volt Absorb ally-healing score was 91 after an existing HP-aware danger penalty; after asserting the score is above the disabled baseline, it passed 17/17.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Volt Absorb does not score ally healing"` initially showed the disabled-path native baseline is 90; after asserting that exact baseline, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="AI sees corresponding absorbing abilities on partners"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI ally absorption HP-aware prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 18/18 after adding the AI Sticky Hold item-loss prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Sticky Hold does not prevent item-loss prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Sticky Hold prevents item theft"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Sticky Hold item-loss prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 19/19 after adding the AI direct-drain Liquid Ooze prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Liquid Ooze does not affect direct drain prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Liquid Ooze causes Absorb users to lose HP instead of heal"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI direct-drain Liquid Ooze prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 20/20 after adding the AI Substitute/Shed Tail Infiltrator prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Infiltrator does not lower Substitute score"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Infiltrator bypasses Substitute"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Substitute/Shed Tail Infiltrator prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 21/21 after adding the AI Magnitude Levitate prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Levitate does not lower Magnitude score"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Magnitude Levitate prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 23/23 after adding the AI Lock-On and Laser Focus prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner No Guard does not lower Lock-On score"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Battle Armor does not lower Laser Focus score"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Lock-On and Laser Focus prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 24/24 after adding the AI Black Sludge Trick Magic Guard prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not stop Black Sludge Trick scoring"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Black Sludge Trick Magic Guard prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially had one assumption failure because the fixture checked runtime battler type before battle setup; after switching to the Poochyena species type invariant, it passed 25/25.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Prankster does not lower status moves into Dark targets"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Prankster Dark-target prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially had one assumption failure because Seismic Toss is `EFFECT_LEVEL_DAMAGE`, not `EFFECT_FIXED_HP_DAMAGE`, then two invalid double-battle score fixtures because the score harness treated the default-score argument as a move in that target shape; after switching the coverage to a single-battle switch-into-pooled-Wonder-Guard fixture using Dragon Rage, it passed 26/26.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Wonder Guard does not lower fixed damage score"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Wonder Guard bad-move scoring bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially showed the first single-battle Electrify fixture did not predict an incoming move, then the double-battle score fixture needed `AI_FLAG_PREDICT_MOVE`; after adding the prediction flag and matching `PASSES_RANDOMLY` guard, it passed 27/27.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Volt Absorb does not raise Electrify score"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Electric-absorption self-benefit prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Spicy Extract"` passed the existing 9/9 Spicy Extract tests; the filter is prefix-based and did not include the new `Shared Power...` test names.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: Spicy Extract"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Clear Body does not make Spicy Extract beneficial"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI stat-loss blocker prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the first Defiant switch fixture asserted a tie-broken move choice, then passed 29/29 after focusing the coverage on the post-switch score assertion.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Defiant does not block stat-drop score boosts"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI stat-drop-punish prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Suction Cups does not lower Roar score"` initially showed the disabled-path Roar score baseline is 90 rather than `AI_SCORE_DEFAULT`; after asserting the actual neutral baseline, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 30/30 after adding the AI Roar Suction Cups prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Roar Suction Cups prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Contrary does not lower Strength Sap score"` initially showed the disabled-path Strength Sap score baseline is `AI_SCORE_DEFAULT`, not above it; after asserting that exact baseline, it passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 31/31 after adding the AI Strength Sap Contrary prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Strength Sap Contrary prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` initially failed because the pooled Magic Guard multi-hit Rocky Helmet fixture expected the neutral default score while the true unpenalized baseline was 101; after calibrating enabled/off expectations to 101 and 99, it passed 32/32.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: pooled Magic Guard avoids multi-hit Rocky Helmet penalty"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not avoid multi-hit Rocky Helmet penalty"` passed 1/1.

Validation (2026-06-07): `git diff --check` passed with no output after the AI multi-hit Rocky Helmet Magic Guard prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: pooled Shed Skin raises Rest fast-recovery score"` initially exposed the AI doubles score harness's self-target limitation, then passed 1/1 after switching the enabled proof to actual move choice: pooled Shed Skin makes the damaged Rest user choose Rest.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Shed Skin is not active for Rest fast-recovery prediction"` passed 1/1. The disabled move-choice fixture tied Rest and Celebrate at score 100, so the final disabled proof asserts the off-path active-membership invariant instead.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 33/33 after adding the AI Rest fast-recovery prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Rest fast-recovery prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: pooled Super Luck raises Focus Energy score"` initially exposed the AI doubles score harness's self-target limitation, then passed 1/1 after switching the enabled proof to actual move choice: pooled Super Luck makes the Focus Energy user choose Focus Energy.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Super Luck is not active for Focus Energy prediction"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 34/34 after adding the AI crit-setup prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI crit-setup prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: pooled Magic Guard avoids low-accuracy crash-recoil penalty"` initially needed explicit Celebrate moves in the fixed test movesets, then passed 1/1 with calibrated unpenalized Axe Kick score 101.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Magic Guard does not avoid low-accuracy crash-recoil penalty"` passed 1/1 with the disabled-path penalized Axe Kick score 95.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 35/35 after adding the AI crash-recoil Magic Guard prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI crash-recoil Magic Guard prediction bucket.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI: pooled Scrappy lowers Foresight score against Ghost targets"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Scrappy does not lower Foresight score against Ghost targets"` passed 1/1.

Validation (2026-06-07): `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power AI"` passed 36/36 after adding the AI Foresight Scrappy/Mind's Eye prediction bucket.

Validation (2026-06-07): `git diff --check` passed with no output after the AI Foresight Scrappy/Mind's Eye prediction bucket.
