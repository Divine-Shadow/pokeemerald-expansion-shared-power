# Implement Shared Power pooled priority modifiers

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md` in this repository. It is self-contained for the selected tranche: implementing Shared Power priority modifiers for Prankster, Gale Wings, and Triage.

## Purpose / Big Picture

Shared Power lets active Pokemon benefit from abilities supplied by their side's current pool. The audit in `docs/design/shared_power/ability_usage_audit.md` says priority-changing abilities should use the effective ability set and stack additively. After this change, a slow Pokemon with a non-priority native ability can move earlier when a teammate contributes Prankster, Gale Wings, or Triage, and combined priority modifiers such as Prankster plus Triage can add together. A human can see this working through battle tests that assert turn-order messages.

## Progress

- [x] (2026-06-06T20:58Z) Selected pooled priority modifiers as the first audit tranche because the audit explicitly says to migrate and stack them, and the behavior is observable through deterministic turn order.
- [x] (2026-06-06T20:58Z) Inspected `src/battle_main.c`, `src/battle_ai_util.c`, `src/battle_util.c`, `test/battle/ability/prankster.c`, `test/battle/ability/gale_wings.c`, and `test/battle/ability/triage.c`.
- [x] (2026-06-06T20:58Z) Implemented live battle priority calculation so Shared Power checks active Prankster, Gale Wings, and Triage independently and adds every applicable modifier.
- [x] (2026-06-06T20:58Z) Aligned AI priority queries through the shared live `GetBattleMovePriority` helper; broader AI known-ability policy remains unchanged.
- [x] (2026-06-06T20:58Z) Added focused battle tests for pooled Prankster, pooled Gale Wings, additive Prankster plus Triage, and a Shared Power disabled Prankster off-path.
- [x] (2026-06-06T20:58Z) Updated public gameplay notes because pooled priority is a public battle rule.
- [x] (2026-06-06T21:20Z) Adjusted the new double-battle tests to provide explicit targets for damaging moves after the runner rejected untargeted `Scratch`.
- [x] (2026-06-06T21:31Z) Ran focused Docker validation for the new Shared Power priority tests plus native Prankster and Gale Wings sanity checks.

## Surprises & Discoveries

- Observation: `GetBattleMovePriority` currently accepts a single ability and uses an `else if` chain, so even if a caller selected one Shared Power ability, Prankster, Gale Wings, and Triage cannot stack.
  Evidence: `src/battle_main.c` checks Gale Wings, then `else if` Prankster, then later `else if` Triage.

- Observation: AI already has a Shared Power-aware helper, `AI_GetPriorityAbility`, but it returns only one ability. That mirrors the live single-ability limitation and is suitable for replacement within this tranche.
  Evidence: `src/battle_ai_util.c` returns `ABILITY_GALE_WINGS`, `ABILITY_PRANKSTER`, or `ABILITY_TRIAGE`, then calls `GetBattleMovePriority`.

- Observation: The double-battle test DSL requires explicit targets for damaging moves in these scenarios.
  Evidence: Docker validation with `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Prankster"` marked both matching tests invalid with `Scratch requires explicit target`.

- Observation: Parallel `make check` invocations can race on the shared `pokeemerald-test.elf` and patched test-runner argument.
  Evidence: A parallel off-path and Gale Wings validation attempt reported the Gale Wings case from the off-path filter; rerunning the off-path filter alone produced the correct off-path test result.

## Decision Log

- Decision: Implement the first tranche as pooled priority modifiers, not blockers, partner auras, or status helpers.
  Rationale: The audit already resolved the key policy question: pooled priority should stack additively. This avoids native mutation/copy/form-gate behavior and gives direct testable battle output.
  Date/Author: 2026-06-06 / Codex

- Decision: Treat order tiebreakers such as Stall, Mycelium Might, Quick Draw, and Custap Berry as out of scope.
  Rationale: The audit explicitly separates them from additive priority stages and marks them for separate clarification.
  Date/Author: 2026-06-06 / Codex

- Decision: Keep `AI_GetPriorityAbility` in place and rely on `GetBattleMovePriority` to apply the additive Shared Power live calculation.
  Rationale: This preserves existing non-Shared-Power AI call structure and avoids broad AI known-ability policy changes while making AI priority estimates use the same Shared Power additive result.
  Date/Author: 2026-06-06 / Codex

## Outcomes & Retrospective

The pooled priority tranche is implemented and validated. Under Shared Power, Prankster, Gale Wings, and Triage now check active effective ability membership and contribute independently, so applicable modifiers stack. Non-Shared-Power behavior still uses the supplied single ability path. The player-facing public note was updated because this is a planning-relevant battle rule. No blind-friendly note was needed because the change does not reveal locations, trainers, scripts, or other discovery-sensitive content.

## Context and Orientation

Shared Power is implemented as a battle-layer view of active abilities. A Pokemon still has one native ability stored in `gBattleMons[battler].ability`, but Shared Power helpers such as `HasActiveAbility` and `ForEachEffectiveAbilityUnique` expose the native ability plus abilities in that battler's Shared Power pool. The pool is seeded from active battlers and guarded by eligibility and suppression checks in `src/battle_shared_power.c`.

Move priority decides turn order before Speed. In `src/battle_main.c`, `GetBattleMovePriority(u32 battler, u32 ability, u32 move)` starts with the move's base priority and then applies ability modifiers. Current code checks only one `ability` argument and uses an `else if` chain for Gale Wings, Prankster, Grassy Glide, and Triage. The audit requires the ability modifiers to use effective ability membership under Shared Power and to add together when more than one modifier applies. Grassy Glide is terrain-based, not an ability, and remains a separate single increment.

Prankster gives status moves +1 priority and sets `gProtectStructs[battler].pranksterElevated` so Dark-type targets can block those moves in Gen 7+. Gale Wings gives +1 to Flying-type moves when the generation/configuration and HP condition allow it. Triage gives healing moves +3 priority. These three are the only ability priority modifiers in this tranche.

AI currently queries priority through `AI_GetBattleMovePriority` in `src/battle_ai_util.c`. It uses `AI_GetPriorityAbility` to select one active priority ability under Shared Power before calling `GetBattleMovePriority`. This tranche should keep the AI change narrow: make AI priority use the same additive result for currently active Shared Power abilities, but do not redesign AI known-ability/prediction policy.

Battle tests live under `test/battle/ability/`. Existing Prankster and Gale Wings tests show local turn-order assertions with `MESSAGE(...)`. `test/battle/ability/triage.c` currently has only a TODO placeholder, so this tranche may add the first concrete Triage tests there or use a new Shared Power test file if that better matches nearby conventions.

## Plan of Work

First, adjust live battle priority calculation in `src/battle_main.c`. Keep `GetBattleMovePriority` as the public API because many callers already use it. Add a small internal calculation path that, when Shared Power is enabled, evaluates Prankster, Gale Wings, and Triage through `HasActiveAbility` or `ForEachEffectiveAbilityUnique` and adds every matching modifier. Preserve current non-Shared Power behavior by applying the single supplied `ability` argument when Shared Power is disabled. Preserve `pranksterElevated` whenever Prankster contributes priority.

Second, adjust `src/battle_ai_util.c` only enough that `AI_GetBattleMovePriority` receives the additive Shared Power priority result. If the live helper already handles Shared Power internally, simplify `AI_GetBattleMovePriority` to pass the AI's native/cached ability for the non-Shared Power path and rely on the live helper for Shared Power. Do not change unrelated AI ability knowledge tables.

Third, add battle tests. At minimum, add a double battle where the acting Pokemon's native ability is not Prankster but its partner contributes Prankster, and the acting Pokemon's status move goes before a faster opponent. Add a stacking test where a healing status move receives both Prankster and Triage and beats a faster +3 priority move such as Upper Hand, proving additive +4 rather than single selected +3. Add a Shared Power disabled/off-path test where the same bench or partner ability does not alter priority when Shared Power is disabled or the donor is not active, whichever is practical in the repository's DSL.

Fourth, update player-facing notes if the implementation changes public mechanics in a way not already described. This is a battle-system rule, so public docs are appropriate if no existing note covers pooled priority. Keep blind-friendly notes untouched unless a discovery-sensitive placement or trainer change is introduced, which this tranche should not do.

Fifth, run targeted tests. Prefer a filtered `make check TESTS="..."` for the changed ability tests, or the existing Shared Power wrapper if it maps cleanly to these cases. Record the exact command and a concise success/failure transcript in this ExecPlan.

## Concrete Steps

Work from `/workspaces/dev/agent-worktrees/agent1/pokeemerald-expansion-shared-power`.

1. Edit `src/battle_main.c` near `GetBattleMovePriority` so Shared Power priority modifiers are additive.
2. Edit `src/battle_ai_util.c` near `AI_GetBattleMovePriority` and `AI_GetPriorityAbility` if the live priority helper needs AI-specific input cleanup.
3. Add tests under `test/battle/ability/prankster.c` and/or `test/battle/ability/triage.c` using existing battle DSL patterns.
4. Update `docs/gameplay_changes_public.md` if pooled priority is not already documented.
5. Update `PATCH_NOTES.md` at the top after each code, test, or docs edit.
6. Run targeted tests from the repository root. Expected form:

        make check TESTS="Prankster|Triage|Gale Wings"

   If the test runner filter does not accept alternation, run the affected substrings one at a time.

## Validation and Acceptance

Acceptance is behavior-based. The change is complete when tests prove all of the following:

- A Pokemon with a non-priority native ability receives Prankster priority from an active Shared Power teammate.
- Prankster plus Triage add together for a healing status move under Shared Power, enough to outrank a +3 priority move from a faster opponent.
- The same priority benefit does not appear in the disabled/off-path test where Shared Power should not apply.
- Existing Prankster and Gale Wings behavior remains intact for native abilities.

The implementation must also pass the targeted test command recorded in this document. If targeted tests fail for an unrelated known suite problem, capture the failure and run the narrowest direct test that proves this tranche.

## Idempotence and Recovery

The edits are additive and local. Re-running tests is safe. If a priority change breaks broad battle behavior, revert only this tranche's edits and keep the ExecPlan updated with the failed evidence and decision. Do not revert unrelated user or agent work in the repository.

## Artifacts and Notes

Validation was run in Docker because the local environment lacks the required ARM toolchain and libraries. The working Docker command form mounted both the worktree and its external gitdir target, and used `NO_MULTIBOOT=1` to avoid unavailable multiboot blobs:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -v "/home/bayesartre/dev/pokeemerald-expansion-shared-power:/home/bayesartre/dev/pokeemerald-expansion-shared-power" -w /workspace pokeemerald-expansion:builder make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Prankster"

Targeted validation evidence:

- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Prankster"`: 2 passed, 0 failed. Covered pooled Prankster and additive Prankster plus Triage.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power: pooled Gale Wings"`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Shared Power off: partner Prankster"`: 1 passed, 0 failed.
- `make check NO_MULTIBOOT=1 TESTS="Prankster increases the priority"`: 1 passed, 2 pre-existing TODO placeholders reported.
- `make check NO_MULTIBOOT=1 TESTS="Gale Wings"`: 3 passed, 0 failed.

## Interfaces and Dependencies

The live battle interface remains:

    s32 GetBattleMovePriority(u32 battler, u32 ability, u32 move);

Callers that supply a native or cached ability for non-Shared Power behavior should continue to work. The Shared Power calculation should use these existing helpers from `include/battle_util.h` and related headers:

    bool32 HasActiveAbility(u32 battler, u16 ability);
    bool32 ForEachEffectiveAbilityUnique(u32 battler, bool32 (*cb)(u16 ability));
    bool32 SharedPower_IsEnabled(void);

Tests should use the existing battle DSL from `include/test/battle.h`, matching nearby examples such as:

    DOUBLE_BATTLE_TEST("Prankster increases the priority of moves by 1")
    {
        GIVEN {
            PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
            OPPONENT(SPECIES_VOLBEAT) { Speed(5); Ability(ABILITY_PRANKSTER); }
        } WHEN {
            TURN { MOVE(opponent, MOVE_CONFUSE_RAY); MOVE(player, MOVE_CELEBRATE); }
        } SCENE {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
    }

Revision Note (2026-06-06): Initial ExecPlan created for the pooled priority tranche selected from the Shared Power ability usage audit.

Revision Note (2026-06-06): Recorded implementation, tests, public gameplay note update, and the narrow AI decision before validation.

Revision Note (2026-06-06): Recorded the invalid double-battle target validation finding and updated the new tests with explicit move targets.

Revision Note (2026-06-06): Recorded final targeted Docker validation evidence and outcome for the pooled priority tranche.
