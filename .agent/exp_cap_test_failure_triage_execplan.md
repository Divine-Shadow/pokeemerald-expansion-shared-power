# Isolate hard-cap test fallout from battle regressions

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan follows `.agent/PLANS.md`. It is self-contained so a contributor can resume from this file without earlier conversation context.

## Purpose / Big Picture

The branch intentionally enables hard badge-based experience caps and Rare Candy caps. The full headless test run now reports 116 failures, but only a small subset is expected fallout from those settings: generic experience tests were written as if there were no active level cap. This plan preserves the new gameplay settings, updates tests so normal EXP behavior is tested under an explicit no-cap fixture, adds direct cap-focused coverage, and keeps the much larger battle-engine failure bucket separate for regression triage.

After this work, a reviewer can run the EXP tests and see the cap-related failures pass without treating unrelated ability, spread-move, held-item, and AI failures as part of the cap change.

## Progress

- [x] (2026-05-02T21:18Z) Read `.agent/PLANS.md`, `test/battle/exp.c`, `include/test/battle.h`, `test/test_runner_battle.c`, `src/caps.c`, and the saved full-check failure list at `/tmp/pokeemerald-failures-98b624f91c.tsv`.
- [x] (2026-05-02T21:18Z) Classified the 116 failures into three cap-related EXP tests and 113 broader battle-regression-looking failures.
- [x] (2026-05-02T21:22Z) Added support for multiple `FLAG_SET` calls in a single battle test so tests can set all badge flags without leaking state between parameters.
- [x] (2026-05-02T21:22Z) Updated generic EXP battle tests to set all badge and champion flags when their purpose is uncapped EXP math rather than cap behavior.
- [x] (2026-05-02T21:23Z) Added focused tests for the active hard level cap and EXP Candy clamp behavior.
- [x] (2026-05-02T21:26Z) Ran targeted EXP validation: `test/battle/exp.c` passed 7/7, and `test/pokemon.c` passed 26/27 with the one existing known failing learnset test.
- [x] (2026-05-02T21:30Z) Ran the broader check and recorded evidence that the non-cap failures remain a separate bucket: full `make check` still fails, but the three settings-related EXP failure names are gone and there are no new unique failure names.
- [x] (2026-05-02T21:31Z) Updated patch notes, committed the implementation as `f2857a1bff`, and prepared the branch for push.

## Surprises & Discoveries

- Observation: `GetCurrentLevelCap()` returns 15 when `B_LEVEL_CAP_TYPE` is `LEVEL_CAP_FLAG_LIST` and no badge flags are set.
  Evidence: `src/caps.c` maps the first unset badge flag, `FLAG_BADGE01_GET`, to level 15.
- Observation: The battle test helper `FLAG_SET(flagId)` currently records only one flag, but a max-level-cap fixture needs all eight badge flags plus `FLAG_IS_CHAMPION`.
  Evidence: `test/test_runner_battle.c:SetFlagForTest` rejects a second flag with `INVALID_IF(DATA.flagId != 0, "FLAG can only be set once per test")`.
- Observation: The saved full-check failure list contains exactly three generic EXP failures attributable to the hard cap: held Exp Share, large EXP gains, and higher-leveled opponent EXP scaling.
  Evidence: `/tmp/pokeemerald-failures-98b624f91c.tsv` lists those three names alongside 113 failures in unrelated battle mechanics.
- Observation: EXP Candy item effects can update a level 14 Pokemon to level 15 immediately while still clamping stored EXP to the level 15 cap threshold.
  Evidence: The first `test/pokemon.c` targeted run failed with `EXPECT_EQ(14, 15)` until the assertion was corrected to accept reaching, but not exceeding, the current cap.
- Observation: The full suite remains red, but not because of the cap settings.
  Evidence: `/tmp/pokeemerald-check-exp-cap-triage.log` reports 111 failures, 21 known failing tests, one assumption failure, 799 TODOs, and 3117 passes; `rg` shows the hard-cap and generic EXP tests passing, and sorted unique failure comparison shows no new unique failure names.

## Decision Log

- Decision: Keep `B_EXP_CAP_TYPE = EXP_CAP_HARD`, `B_LEVEL_CAP_TYPE = LEVEL_CAP_FLAG_LIST`, and `B_RARE_CANDY_CAP = TRUE`.
  Rationale: These are the requested gameplay settings. The tests should state when they need a max-cap fixture rather than weakening the setting.
  Date/Author: 2026-05-02 / Codex
- Decision: Extend battle test `FLAG_SET` support instead of directly calling `FlagSet` in EXP tests.
  Rationale: Direct flag calls can leak event flags after a failure. The existing test helper already owns cleanup, so expanding it keeps the fixture safe and reusable.
  Date/Author: 2026-05-02 / Codex
- Decision: Treat broad ability, spread-move, held-item, and AI failures as a separate regression bucket unless focused validation ties them to cap settings.
  Rationale: Their names and mechanics do not depend on EXP gain, Rare Candy use, or badge cap state.
  Date/Author: 2026-05-02 / Codex

## Outcomes & Retrospective

Validation achieved the expected cap outcome. `test/battle/exp.c` passes under hard badge caps after setting an explicit max-cap fixture for generic EXP tests. `test/pokemon.c` confirms that level increments stop at the current cap and EXP Candies clamp stored EXP to that cap. Full `make check` still fails in the broad battle bucket, but the cap-related failures are removed and no new unique failure names appear compared with the saved pre-fix failure list.

## Context and Orientation

The cap settings live in `include/config/caps.h`. `B_EXP_CAP_TYPE` controls whether Pokemon at or above the current cap can gain battle EXP. `B_LEVEL_CAP_TYPE` tells the game how to calculate the cap. With `LEVEL_CAP_FLAG_LIST`, `src/caps.c:GetCurrentLevelCap` reads badge flags in order and returns the level associated with the first unset flag. Because fresh tests start without badge flags, the current cap is level 15.

Generic EXP battle tests live in `test/battle/exp.c`. They use the battle-test DSL from `include/test/battle.h`, where `GIVEN` creates parties and event flags, `WHEN` chooses actions, `SCENE` checks player-visible output, and `FINALLY` compares captured values across parameters. These tests should verify normal EXP math, so they need to set all badge flags and the champion flag to make `GetCurrentLevelCap()` return `MAX_LEVEL`.

The battle-test runner implementation lives in `test/test_runner_battle.c`. Its `FLAG_SET(flagId)` macro calls `SetFlagForTest`, which currently allows one recorded flag per test and clears it in `ClearFlagAfterTest`. This plan changes the implementation to record a small array of flags and clear them before each parameter and after each test.

The broader regression bucket comes from a saved full headless run, `/tmp/pokeemerald-check-98b624f91c.log`, with extracted names in `/tmp/pokeemerald-failures-98b624f91c.tsv`. Those failures are dominated by ability event ordering, spread-move targeting, held item timing, and AI behavior.

## Plan of Work

First, update the battle-test helper so repeated `FLAG_SET` calls are legal. In `include/test/battle.h`, replace the single `flagId` field with a bounded flag list and update the nearby DSL comment to explain that multiple flags are supported and cleaned up automatically. In `test/test_runner_battle.c`, call `ClearFlagAfterTest` before resetting `DATA` for each battle parameter, append each flag in `SetFlagForTest`, and clear every recorded flag in `ClearFlagAfterTest`.

Second, update `test/battle/exp.c`. Add a small helper that sets `FLAG_BADGE01_GET` through `FLAG_BADGE08_GET` and `FLAG_IS_CHAMPION`. Call that helper from generic EXP tests whose assertions assume normal uncapped growth. Add one battle test proving that a Pokemon already at the current badge cap receives no battle EXP under hard caps.

Third, add focused Pokemon-level tests in `test/pokemon.c` for cap behavior that is not easily visible through the battle DSL. These tests should clear level-cap flags before inspecting results, avoid assertions before cleanup, and verify that level increments stop at the cap and EXP Candies clamp stored EXP to the current cap under hard caps.

Finally, run targeted validation with Docker, then run either the full headless check or a focused sample from the regression bucket. Record commands and outcomes here, update `PATCH_NOTES.md`, commit, and push.

## Concrete Steps

Work from `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Use these commands for validation:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 TESTS="test/battle/exp.c" check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 TESTS="test/pokemon.c" check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check

If full `make check` still fails, compare the remaining EXP-related names against `/tmp/pokeemerald-failures-98b624f91c.tsv`. The accepted result for this plan is that the three cap-related EXP failures disappear; unrelated battle failures can remain but must be reported as a separate regression bucket.

Actual validation run:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 TESTS="test/battle/exp.c" check
    Result: passed 7/7 tests.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 TESTS="test/pokemon.c" check
    Result: passed 26/27 tests with one existing known failing learnset test.

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check > /tmp/pokeemerald-check-exp-cap-triage.log 2>&1
    Result: failed in the broader battle bucket. Summary: 111 failed, 21 known failing, 1 assumptions failed, 799 TODO, 3117 passed, 4049 total.

    perl -ne 'if (/^\[\d+\] (.*): \e\[31m(FAIL|ERROR)\e\[0m$/) { print "$2\t$1\n" }' /tmp/pokeemerald-check-exp-cap-triage.log > /tmp/pokeemerald-failures-exp-cap-triage.tsv
    rg -n "Exp Share|Large exp gains|Higher leveled|Hard level cap|EXP Candies|test/battle/exp.c|test/pokemon.c" /tmp/pokeemerald-check-exp-cap-triage.log /tmp/pokeemerald-failures-exp-cap-triage.tsv
    Result: the three prior EXP failure names no longer appear as failures; the new hard-cap and EXP Candy tests pass.

## Validation and Acceptance

The plan is accepted when the targeted EXP tests pass under the active hard-cap settings and the full or focused broader run shows no new cap-related failures. The test runner helper change is accepted when multiple `FLAG_SET` calls in one `GIVEN` block do not invalidate the test and do not leak flags into later parameters.

The cap-specific behavior is accepted when a battle test captures zero gained EXP for a Pokemon at the current cap, and a Pokemon test shows EXP Candy cannot push stored EXP beyond the current cap while `B_EXP_CAP_TYPE` is `EXP_CAP_HARD`.

## Idempotence and Recovery

The edits are additive and safe to re-run. Re-running `make check` rebuilds changed objects. If a validation command fails because of known broader battle regressions, preserve the log and inspect whether any `test/battle/exp.c` or `test/pokemon.c` cap tests failed. Do not change the requested cap config to make tests pass.

If the battle-test helper change causes widespread invalid tests, revert only the helper edits made by this plan and use a narrower helper local to EXP tests, but document that decision here before proceeding.

## Artifacts and Notes

Initial failure evidence from the saved run:

    Tests FAILED: 116
    KNOWN_FAILING: 22
    ASSUMPTIONS_FAILED: 1
    TO_DO: 787
    PASSED: 3107
    TOTAL: 4033

Settings-related failure names extracted from `/tmp/pokeemerald-failures-98b624f91c.tsv`:

    Exp Share(held) gives Experience to mons which did not participate in battle 2/2
    Large exp gains are supported 3/3
    Higher leveled Pokemon give more exp 2/2

Post-fix comparison evidence:

    comm -23 /tmp/fail-before-u.sorted /tmp/fail-after-u.sorted
    Before-only cap names:
    Exp Share(held) gives Experience to mons which did not participate in battle 2/2
    Higher leveled Pokemon give more exp 2/2
    Large exp gains are supported 3/3

    comm -13 /tmp/fail-before-u.sorted /tmp/fail-after-u.sorted
    Result: no after-only unique failure names.

## Interfaces and Dependencies

The implementation uses these existing interfaces:

`FLAG_SET(flagId)` from `include/test/battle.h` records event flags for battle tests. It must support at least nine flags for the max-level-cap fixture.

`GetCurrentLevelCap()` from `src/caps.c` returns 15 with no badge flags and `MAX_LEVEL` after all badge and champion flags are set.

`EXPERIENCE_BAR(player, captureGainedExp: &results[i].exp)` in `test/battle/exp.c` captures EXP changes from the player-visible battle EXP bar. Tests that expect no EXP at the hard cap should use `NOT EXPERIENCE_BAR(player)`.

`TryIncrementMonLevel(struct Pokemon *mon)` and `ExecuteTableBasedItemEffect(struct Pokemon *mon, u16 item, u8 partyIndex, u8 moveIndex)` from `include/pokemon.h` are used for direct Pokemon-level cap tests.

Revision Note (2026-05-02): Initial ExecPlan created to isolate hard-cap test fallout from unrelated battle regression failures and guide implementation through validation.

Revision Note (2026-05-02): Recorded implementation and validation evidence after the targeted EXP and Pokemon cap tests passed and the broad suite confirmed remaining failures are unrelated to the cap settings.

Revision Note (2026-05-02): Recorded the implementation commit hash and completed progress state before publishing the branch.
