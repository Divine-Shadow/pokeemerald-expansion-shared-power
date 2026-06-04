# Implement the Highlander Charm

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This document is maintained according to `.agent/PLANS.md`. It is self-contained so a contributor can restart from only this file and the current worktree.

## Purpose / Big Picture

The Highlander Charm is a passive opt-in item for players using a no-duplicates encounter rule. When the charm is in the player's bag, random wild encounters should be chosen only from evolutionary families the player has not caught before. This lets a player deliberately deplete common encounter families and then force the remaining legal encounters without spending time rolling duplicates. The visible proof is a focused test suite where caught families disappear from normal encounter pools, remaining slots keep their original relative odds, and a depleted pool produces no battle plus a clear message.

## Progress

- [x] (2026-06-04T21:05Z) Created this ExecPlan and recorded the initial scope, constraints, verification plan, and public player-note expectation.
- [x] (2026-06-04T21:18Z) Inspected current item, encounter, Pokedex, evolution-family, message, and test APIs before editing implementation files.
- [x] (2026-06-04T21:34Z) Added the Highlander Charm item, starting PC copy, and passive bag-presence detection.
- [x] (2026-06-04T21:41Z) Added encounter-pool filtering that excludes caught evolutionary families, preserves remaining relative slot weights, and reports an empty legal pool.
- [x] (2026-06-04T21:48Z) Applied filtering to normal land, water, fishing, Rock Smash, and Sweet Scent random encounters while leaving roamers, outbreaks, scripted/static encounters, gifts, and trainer battles unchanged.
- [x] (2026-06-04T21:55Z) Added player-facing public and blind-friendly notes plus required patch notes.
- [x] (2026-06-04T21:53Z) Added focused automated tests for inactive behavior, family filtering, proportional reweighting, empty-pool suppression, and an Old Rod path.
- [x] (2026-06-04T22:35Z) Ran targeted tests, item text validation, broad-suite validation, and a normal non-test build.

## Surprises & Discoveries

- Observation: Existing charm items are key items, but the Highlander Charm must be a regular Items-pocket item so PC storage can disable it without special cases.
  Evidence: `src/item_menu.c` blocks PC storage for important/key items; `src/data/items.h` now defines `ITEM_HIGHLANDER_CHARM` with `POCKET_ITEMS` and no importance flag.

- Observation: Lure changes slot selection by mirroring the selected index 20 percent of the time.
  Evidence: Existing `ChooseWildMonIndex_*` functions mirror land, water, rock, and rod slot indices when `LURE_STEP_COUNT != 0`.

- Observation: Fishing previously always started a battle after a successful bite.
  Evidence: `FishingWildEncounter` returned `void`; it now returns `bool8` so an empty Highlander-filtered pool can suppress the battle and show the signal script.

## Decision Log

- Decision: Implement the charm as a passive item active only when present in the bag.
  Rationale: The user wants storage in the PC to opt out without step counters or active-use overlap with Repel.
  Date/Author: 2026-06-04 / Codex.

- Decision: Treat the mechanic as public player-facing knowledge.
  Rationale: It is a global item mechanic that affects route planning before a run; exact route encounter data is not being changed.
  Date/Author: 2026-06-04 / Codex.

- Decision: Use evolutionary-family caught history rather than exact species, seen state, or current storage membership.
  Rationale: This matches common no-duplicates clause play and prevents releasing or boxing from reopening a family.
  Date/Author: 2026-06-04 / Codex.

- Decision: Put a single Highlander Charm in the player's starting PC storage.
  Rationale: The item is opt-in and PC storage is the agreed disable path, so starting in PC gives immediate availability without forcing the passive on.
  Date/Author: 2026-06-04 / Codex.

- Decision: Reuse the Catching Charm icon and palette.
  Rationale: It keeps this feature scoped to mechanics, text, and tests rather than adding new graphics.
  Date/Author: 2026-06-04 / Codex.

- Decision: When Highlander filtering is active, choose the slot through a filtered weighted chooser before the final level/Repel/Keen Eye checks.
  Rationale: This prevents duplicate-slot rolls and preserves the remaining slot proportions; Repel still checks the generated level afterward.
  Date/Author: 2026-06-04 / Codex.

## Outcomes & Retrospective

Implemented the Highlander Charm as a regular Items-pocket passive item, seeded one copy in the player's starting PC storage, and added a bag-only active check. When active, normal land, water, Rock Smash, Sweet Scent, and fishing random encounters choose from uncaught evolutionary-family slots before rolling the weighted slot. Empty filtered pools suppress the encounter and show `No unfamiliar POKéMON seem to be nearby.`

Focused tests now cover bag-vs-PC activation, evolutionary-family filtering from Pokedex caught state, proportional land-table reweighting, empty filtered pools, and an Old Rod filtered path. The item description text also passes the existing bag/shop fit test.

Broad `make check` still has unrelated pre-existing failures outside this change area. The isolated `Condition Coach rejects Eggs` test fails by itself with `EXPECT_EQ(7, 2)`, so it is not caused by Highlander tests or encounter state. The filtered full-suite rerun also reported unrelated Drought/Limber/Protosynthesis/AI failures. Highlander-specific tests passed after the final item-description edit, and a normal non-test ROM build passed with `NO_MULTIBOOT=1`.

## Context and Orientation

This repository is a `pokeemerald-expansion`-based C project. Item definitions live in `src/data/items.h`, item constants live in `include/constants/items.h`, common item constants live in `include/constants/item.h`, and item use functions live in `src/item_use.c`. Random wild encounter logic lives in `src/wild_encounter.c` with public declarations in `include/wild_encounter.h`.

A wild encounter table contains multiple slots. A slot is one possible species and level range entry. The Highlander Charm must remove illegal slots before a species is rolled. Preserving relative weights means that if the original legal remaining slots represented 10 percent Poochyena and 40 percent Lotad after Rattata was removed, Poochyena should become 20 percent and Lotad should become 80 percent among the remaining legal choices. The implementation should not roll a removed slot and then reject the encounter because that would recreate the duplicate-roll problem.

The Pokedex caught state is the intended caught-history source. It records whether a species has been caught before and is not the same as the current party, the current PC boxes, or the seen state. Evolutionary-family filtering means that catching one member of a family should remove every species in that family from future Highlander-filtered random encounters.

Repel and Lure already operate in `src/wild_encounter.c`. Repel checks a generated wild Pokemon's level after rolling a slot; Lure modifies encounter rate. Highlander Charm should compose with these mechanics without changing their semantics.

Player-facing broad mechanics belong in `docs/gameplay_changes_public.md`. Blind-friendly discovery notes belong in `docs/gameplay_changes_blind.md` when exact placements or surprise content changes. This feature is a public rule, not a placement change.

## Plan of Work

First, inspect the current item ID layout, item icon conventions, Pokedex caught helpers, evolution-family helpers, wild encounter slot selection functions, and nearby tests. Record any implementation-shaping findings in this plan before editing shared logic.

Second, add `ITEM_HIGHLANDER_CHARM` in the item constants and define its item data in `src/data/items.h`. Use an existing charm or passive field item style if one exists; otherwise use a regular item with no active field-use function and a concise description explaining that it filters already-caught wild Pokemon families. Use existing item graphics if adding a unique icon is outside scope.

Third, add helper logic in the encounter module or a closely related module to detect whether the item is active by checking the player's bag only. PC item storage must not activate it. Add a helper to decide whether a wild species is legal by checking whether any member of its evolutionary family is marked caught.

Fourth, change the random slot selection flow so Highlander Charm filtering happens before choosing the final slot. The helper should collect legal candidate slots, choose among them with the same relative slot weights as the existing table, and report when no candidate exists. Existing ability-biased selection for types such as Static and Magnet Pull must be inspected before deciding where the filter enters, because it should not reintroduce duplicate rolls or flatten valid odds.

Fifth, thread the empty-pool result through land, water, fishing, Rock Smash, and Sweet Scent. Normal no-encounter results should remain silent, but Highlander-depleted pools should set up a clear message so the player understands why no battle occurred. Roamers and outbreaks must continue using their existing paths without Highlander filtering.

Sixth, add focused tests. Tests should not depend on natural randomness where possible. If the repository already has wild encounter unit tests, extend them; otherwise add a small test-only seam in the encounter code that validates candidate-slot selection deterministically. Tests must cover inactive behavior when the charm is absent, family exclusion after a caught related species, proportional reweighting, strict empty-pool suppression, and at least one special path such as fishing, Rock Smash, or Sweet Scent.

Finally, update `docs/gameplay_changes_public.md` and `PATCH_NOTES.md`, run targeted tests, run broader validation, and update this plan with evidence and retrospective notes.

## Concrete Steps

Run commands from the repository root `/home/bayesartre/dev/pokeemerald-expansion-shared-power`.

Research commands:

    rg -n "ITEM_.*CHARM|Shiny Charm|Oval Charm|CheckBagHasItem|GetSetPokedexFlag|FLAG_GET_CAUGHT|evolution" include src test
    rg -n "ChooseWildMonIndex|TryGenerateWildMon|SweetScentWildEncounter|FishingWildEncounter|RockSmashWildEncounter" src/wild_encounter.c test include

Validation commands run:

    make -j"$(nproc)" TESTS="Highlander Charm" check
    docker build -t pokeemerald-expansion:builder .
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Highlander Charm" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" TESTS="Item descriptions fit" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1 check
    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make -j"$(nproc)" NO_MULTIBOOT=1

Local `make check` was attempted first and failed before compilation because this NixOS host cannot run existing dynamically linked generated tools. The Docker path was used for validation. The full `make check` does not currently pass because of unrelated failures described in Outcomes & Retrospective.

## Validation and Acceptance

Acceptance requires evidence for every requested behavior. With the Highlander Charm absent from the bag, random encounter selection must behave as before. With the charm present and a family marked caught in the Pokedex caught state, every slot in that family must be excluded before the slot roll. When remaining slots have original weights such as 10 percent and 40 percent after a 50 percent caught family is removed, the resulting legal odds must be 20 percent and 80 percent. When all slots are illegal, the game must suppress the encounter and show a clear signal message instead of silently failing or falling back to duplicates.

The feature is not accepted if it filters roamers, outbreaks, statics, gifts, scripted encounters, trainer battles, or current party/box membership. It is also not accepted if it relies on seen state, exact species only, or after-the-fact duplicate rejection.

## Idempotence and Recovery

The planned edits are additive and local to item data, encounter logic, messages, tests, and player notes. Re-running tests is safe. If a test or build fails, inspect the failing file and update this plan's `Surprises & Discoveries` and `Decision Log` before changing course. Do not use destructive git commands; preserve unrelated worktree changes if any appear.

## Artifacts and Notes

Initial evidence:

    git status --short
    <clean output>

    git rev-parse --short HEAD
    aba5867665

## Interfaces and Dependencies

The implementation should expose only small helper interfaces needed by tests and encounter generation. Existing code style uses `PascalCase` for functions, `camelCase` for local variables, and four-space indentation in C. Tests use the repository's `TEST("Suite/Case")` style and assertions from `include/test/test.h`.

Likely dependencies to inspect and reuse are `CheckBagHasItem` for bag-only item presence, Pokedex caught flag helpers for caught history, and existing wild encounter slot selection helpers in `src/wild_encounter.c` for preserving vanilla odds.

Revision Note (2026-06-04): Initial ExecPlan created from the user's OCVA scope so implementation can proceed end to end with living progress and verification evidence.
