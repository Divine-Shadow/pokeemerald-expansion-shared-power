# Build a Canonical Mightyena Intimidate Switch Automation Route

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This repository stores the ExecPlan protocol in `.agent/PLANS.md`. Maintain this file according to that protocol: milestones must be completed in order, evidence must be recorded in this document, and execution should continue until the route works end to end or a true blocker requires escalation.

## Purpose / Big Picture

The goal is to turn the existing automation spike into a reusable gameplay route that proves a concrete Shared Power behavior in the running game. After this plan succeeds, one headless command will start from a clean build, obtain two Pokemon through named route checkpoints, evolve a caught slot-0 Poochyena into a lead Mightyena by using Rare Candies from the player PC, enter a battle, switch from Mightyena to the second Pokemon, and prove that Intimidate appears again on switch-in.

This matters because the previous Mightyena proof was intentionally non-canonical: it used guarded automation commands to create Poochyena and directly mutate it into Mightyena. That proved the probe and savestate plumbing, but it did not build the route we want to reuse for future gameplay tests. This plan replaces the ad-hoc proof with a small route tree whose checkpoints can be shared by later objectives, such as catching, evolving, party ordering, and battle-menu navigation.

The human-visible proof is a `run_result.json` under `build/mgba_lua_spike/mightyena-switch-intimidate/` with `ok=true`, `target=MIGHTYENA_SWITCH_INTIMIDATE`, party facts showing slot 0 as `SPECIES_MIGHTYENA` with `ABILITY_INTIMIDATE`, party facts showing a second usable Pokemon, and battle event facts showing that `ABILITY_INTIMIDATE` fired after switching to the second Pokemon. The run should also write an ignored savestate and screenshot so a human can inspect the final battle state.

## Progress

- [x] (2026-04-29 22:42 -04:00) Created this ExecPlan after reading `.agent/PLANS.md`, the current automation probe, the mGBA Lua host, the player PC Rare Candy initialization, and existing Shared Power Intimidate tests.
- [ ] Milestone 1: Revalidate the current headless probe baseline and record the exact build, emulator, and smoke-test evidence in this plan.
- [ ] Milestone 2: Add generic route observability for PC Rare Candies, selected battle UI state, and switch-in ability events without adding objective-specific mutation commands.
- [ ] Milestone 3: Implement and verify the reusable PC Rare Candy withdrawal checkpoint from the bedroom PC.
- [ ] Milestone 4: Implement and verify the reusable two-Pokemon early-game party checkpoint, ending with a starter and a real caught slot-0 Poochyena.
- [ ] Milestone 5: Implement and verify the reusable Poochyena evolution checkpoint by using Rare Candies through the normal item and evolution flow.
- [ ] Milestone 6: Implement and verify the party-order and battle-entry checkpoint with Mightyena as lead and a second Pokemon available to switch into.
- [ ] Milestone 7: Implement and verify the final switch proof where switching from Mightyena to the second Pokemon causes Intimidate to appear again.
- [ ] Milestone 8: Run validation, repeat the final route enough times to prove reliability, update `PATCH_NOTES.md`, and commit all changes.

## Surprises & Discoveries

- Observation: The player PC is already seeded with Rare Candies in this working tree.
  Evidence: `src/player_pc.c` defines `sNewGamePCItems` with `{ ITEM_RARE_CANDY, 999 }`, and `NewGameInitPCItems` copies those entries into `gSaveBlock1Ptr->pcItems` for a new game.

- Observation: The existing Lua/Python proof can already decode party species, levels, ability slots, resolved abilities, and bag counts from `gAutomationProbe`.
  Evidence: `include/automation_probe.h` exposes `partySpecies`, `partyLevel`, `partyAbilityNum`, `partyAbility`, `bagPokeBallCount`, and `bagRareCandyCount`; `tools/mgba/mgba_lua_spike.py` decodes those fields in `PROBE_FIELD_LAYOUT`.

- Observation: The current Mightyena proof is not acceptable as the final route because it bypasses the evolution path.
  Evidence: `src/automation_probe.c` contains `AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0` and `AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA`; `drive_poochyena_intimidate` in `tools/mgba/mgba_lua_spike.py` uses those commands and records the work as deferred canonical work.

- Observation: Battle tests already express the core mechanic that the final automation route should observe in-game.
  Evidence: `test/battle/shared_power.c` contains `SINGLE_BATTLE_TEST("Shared Power: Intimidate is fully shared with a switched in pokemon", s16 damage)` and other switch-in Intimidate tests that assert `ABILITY_POPUP(..., ABILITY_INTIMIDATE)` after switching.

## Decision Log

- Decision: Use one ExecPlan with ordered, independently verifiable milestones instead of several unrelated plans.
  Rationale: The requested outcome depends on a single route tree: PC inventory, early-game party setup, catch, evolution, party ordering, battle entry, and switch proof. Splitting those into unrelated documents would encourage exactly the ad-hoc fragments this task is meant to avoid.
  Date/Author: 2026-04-29 / Codex

- Decision: Treat the bedroom PC Rare Candy withdrawal as a canonical route checkpoint, not as a direct bag-grant command.
  Rationale: The user stated that the PC should already have the required Rare Candies. The plan should verify that fact and use the normal PC item flow so later routes can reuse inventory acquisition without weakening the gameplay proof.
  Date/Author: 2026-04-29 / Codex

- Decision: Add observability fields for generic facts such as PC item quantities and switch-in ability events, but do not add new objective-specific commands that create or promote Pokemon for final acceptance.
  Rationale: Probe fields are acceptable because they tell the host what the game is doing. Direct mutation commands are useful for spikes but would make the final route prove the command rather than the game path.
  Date/Author: 2026-04-29 / Codex

- Decision: The final route may keep old guarded commands in the code only for legacy spike modes, but the new `mightyena-switch-intimidate` mode must not call them.
  Rationale: Removing the legacy spike in the same change is unnecessary risk. The important invariant is that the new acceptance path uses real route checkpoints and records enough trace data to audit that it did so.
  Date/Author: 2026-04-29 / Codex

## Outcomes & Retrospective

This plan is newly authored. No implementation outcome has been produced yet. The expected end state is an end-to-end automation route that demonstrates Mightyena-derived Intimidate firing again when the player switches to a second Pokemon, with evidence in JSON, savestate, screenshot, targeted tests, and committed source changes.

## Context and Orientation

This repository is a Pokemon Emerald decompilation-based project. The game code is C under `src/`, public headers are under `include/`, gameplay data is under `data/`, tests are under `test/`, and automation host scripts are under `tools/mgba/`. The final ROM is `pokeemerald.gba`, and `pokeemerald.sym` or `pokeemerald.map` provides symbol addresses for host automation.

The current automation stack has three layers. The first layer is mGBA, the emulator. The second layer is `tools/mgba/mgba_lua_bridge.lua`, a Lua script run by mGBA that accepts socket commands from the host and can press buttons, advance frames, and read or write emulator memory. The third layer is `tools/mgba/mgba_lua_spike.py`, a Python host that launches mGBA, talks to the Lua bridge, reads game facts, and drives named route modes.

An automation probe is a small C struct compiled only when `AUTOMATION_PROBE=1`. It lives in `include/automation_probe.h` and `src/automation_probe.c` as `gAutomationProbe`. The host finds `gAutomationProbe` in `pokeemerald.sym` or `pokeemerald.map`, then reads it through Lua. The probe should expose stable facts such as current map, player coordinates, party species, bag counts, PC item counts, and battle events. A probe fact is different from a command: a fact observes game state; a command mutates game state. This route should prefer facts and normal input over mutation commands.

A route checkpoint is a named, observable state that future objectives can reuse. Examples for this plan are `Inventory.RareCandiesWithdrawnFromPC`, `Party.CaughtPoochyenaSlot0`, `Party.PoochyenaEvolvedByRareCandy`, `Party.MightyenaLeadWithSecond`, and `Battle.SwitchToSecondTriggeredIntimidate`. Each checkpoint should have a predicate that proves it is reached, actions that advance toward it, invariants that must remain true, and a trace entry in `run_result.json`.

Poochyena and Mightyena matter because of their ability slots. A slot-0 Poochyena resolves to its Poochyena ability before evolution, but after evolving into Mightyena the same ability slot resolves to `ABILITY_INTIMIDATE`. The route must therefore catch or otherwise obtain a real Poochyena with `abilityNum=0`, evolve that Pokemon through the game evolution flow, and prove the resulting Mightyena has `ability=ABILITY_INTIMIDATE`. Do not directly set the species to Mightyena in the final route.

The target battle behavior is Shared Power behavior. Shared Power is a battle mode in this project where abilities can be shared across the party. Existing tests in `test/battle/shared_power.c` prove that Intimidate can be shared and can appear on a switched-in Pokemon. The route in this plan is not a replacement for those tests; it is an emulator-level proof that the gameplay automation can reach a relevant state and observe the same kind of behavior in the running ROM.

The player PC is relevant because this working tree seeds new games with Rare Candies. `src/player_pc.c` defines `sNewGamePCItems` with `ITEM_RARE_CANDY` and quantity `999`. The plan must verify these candies through the running game and withdraw them using the PC item storage flow, rather than silently granting them to the bag.

## Plan of Work

Milestone 1 revalidates the current baseline before changing code. Build an automation-enabled ROM and symbol file, run the existing `probe-smoke` mode, and record the command and a short evidence snippet in this plan. The acceptance for this milestone is that `probe-smoke` reaches `STARTER_CONFIRM_PROMPT`, decodes `gAutomationProbe`, reports a valid magic/version/size, and proves the host can drive mGBA headlessly.

Milestone 2 expands generic observability. Update `include/automation_probe.h`, `src/automation_probe.c`, and `tools/mgba/mgba_lua_spike.py` so the host can see PC Rare Candy count, current battle menu readiness needed for switching, and a monotonic switch-in ability event record. A monotonic event record means a counter that only increases when the event happens; this lets the host distinguish a new Intimidate event from an old one without relying on timings. The ability event fields should be generic, for example last ability id, battler side, battler index, source party slot if available, and an event counter. The milestone is accepted when `probe-smoke` still passes and its JSON includes the new fields with harmless default values outside battle.

Milestone 3 implements the reusable PC Rare Candy withdrawal checkpoint. Extend the route host with a mode such as `pc-rare-candy-smoke` or a route node callable from the final mode. Starting from the early bedroom state, navigate to the bedroom PC, select item storage, withdraw Rare Candies, and exit back to field control. Use the probe to prove that PC Rare Candy count decreased and bag Rare Candy count increased. This milestone must not call `AUTOMATION_PROBE_COMMAND_GRANT_ITEM`. The acceptance evidence is a `run_result.json` trace containing `Inventory.PCRareCandyAvailable` and `Inventory.RareCandiesWithdrawnFromPC` with before-and-after quantities.

Milestone 4 implements the reusable two-Pokemon early-game party checkpoint. Continue the route through starter selection and the initial story path until the player has a starter and access to wild encounters and Poke Balls. Then enter Route 101 grass, inspect wild encounters through the probe, and catch a Poochyena only when `enemy0Species` is `SPECIES_POOCHYENA` and `enemy0AbilityNum` is `0`. If the encounter is wrong, run away and try again. This milestone is accepted when the player party contains two Pokemon: one starter and one real caught slot-0 Poochyena. The trace must show that the route observed a wild Poochyena battle and completed a capture flow; it must not call `AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0`.

Milestone 5 implements the reusable evolution checkpoint. Use Rare Candies from the bag on the caught Poochyena through the normal item and party menu flow until it reaches level 18 and the evolution scene completes. Add route predicates that can wait for evolution completion by observing party species, level, ability slot, resolved ability, and field control returning after the evolution scene. This milestone is accepted when the same party slot that previously contained `SPECIES_POOCHYENA` with `abilityNum=0` now contains `SPECIES_MIGHTYENA`, still has `abilityNum=0`, and resolves to `ABILITY_INTIMIDATE`. The trace must show Rare Candy count decreasing and must not call `AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA`.

Milestone 6 implements party ordering and battle entry. If Mightyena is not already slot 0, use the normal party menu switch flow to place Mightyena in slot 0 and the second Pokemon in slot 1. Then start a battle suitable for observing switch behavior. Prefer a wild battle if it reliably enables Shared Power in this project; if Shared Power requires a specific flag or battle type, inspect the existing setup code and choose the smallest legitimate route or guarded test-only setup that preserves the party and battle menu behavior. This milestone is accepted when the probe reports an active battle, party slot 0 is Mightyena with Intimidate, party slot 1 is a usable second Pokemon, and the battle action menu can be driven without timing sleeps.

Milestone 7 implements the final switch proof. From the battle action menu, choose Pokemon, select the second Pokemon, confirm the switch, and wait for a new switch-in ability event. The event must be detected through the generic ability event counter added in Milestone 2, not by fixed frame delays. The acceptance evidence is `run_result.json` with `ok=true`, `target=MIGHTYENA_SWITCH_INTIMIDATE`, `routeTrace` entries for every prior checkpoint, `switchedFromSlot=0`, `switchedToSlot=1`, and an observed event where the ability id is `ABILITY_INTIMIDATE` after the switch command. Capture a screenshot and savestate in the output directory for human inspection.

Milestone 8 performs validation and cleanup. Run syntax checks for the Python and Lua host files, build both a normal ROM and the automation-enabled ROM, run targeted Shared Power tests, run the final route at least three times, update this plan with evidence and retrospective notes, update `PATCH_NOTES.md`, and commit all source and documentation changes. This milestone is accepted when the worktree is clean, the commits contain all changes, and the plan records the validation commands and concise outputs.

## Concrete Steps

Work from the repository root:

    cd /home/bayesartre/dev/pokeemerald-expansion-shared-power

Before editing, check the worktree and record whether unrelated changes exist:

    git status --short --branch

Build the baseline automation ROM and symbols. Use the repository Docker image if local toolchain variables are unavailable. `NO_MULTIBOOT=1` is acceptable when multiboot blobs are not present:

    make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms

Run the current probe smoke route. If working inside the agent container, use the installed headless mGBA path already used by previous automation runs:

    rm -rf build/mgba_lua_spike/probe-smoke
    python3 tools/mgba/mgba_lua_spike.py --mode probe-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-smoke --port 46711 --connect-timeout 20 --starter-timeout 600

After Milestone 2 adds fields, rerun the same command and verify the JSON still includes `ok=true` plus the new fields. The expected high-level output shape is:

    {"ok": true, "result": "build/mgba_lua_spike/probe-smoke/run_result.json"}

After Milestone 3, run the PC inventory checkpoint:

    rm -rf build/mgba_lua_spike/pc-rare-candy-smoke
    python3 tools/mgba/mgba_lua_spike.py --mode pc-rare-candy-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/pc-rare-candy-smoke --port 46712 --connect-timeout 20 --objective-timeout 600

After Milestones 4 and 5, run the evolution checkpoint. The exact mode name may be adjusted during implementation, but the result must prove a caught Poochyena evolved through Rare Candy use:

    rm -rf build/mgba_lua_spike/poochyena-evolution-path
    python3 tools/mgba/mgba_lua_spike.py --mode poochyena-evolution-path --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/poochyena-evolution-path --port 46713 --connect-timeout 20 --objective-timeout 1800

After Milestone 7, run the final route three times on distinct ports and output directories:

    for i in 1 2 3; do out="build/mgba_lua_spike/mightyena-switch-intimidate/run-$i"; rm -rf "$out"; python3 tools/mgba/mgba_lua_spike.py --mode mightyena-switch-intimidate --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir "$out" --port "$((46720 + i))" --connect-timeout 20 --objective-timeout 2400; done

Run source validation before committing:

    python3 -m py_compile tools/mgba/mgba_lua_spike.py
    luac -p tools/mgba/mgba_lua_bridge.lua
    make -j"$(nproc)" NO_MULTIBOOT=1 rom
    make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms
    make check TESTS="Shared Power"

If the local environment lacks a toolchain but Docker is available, use the Docker commands from `AGENTS.md` with the same make targets. If the route is being run from the devkit container, keep all commands inside that container so generated artifacts stay in the mounted worktree and do not contaminate another agent's workspace.

## Validation and Acceptance

The final acceptance is behavioral. A novice should be able to run the final `mightyena-switch-intimidate` mode on a clean automation-enabled ROM and observe that it reaches the gameplay target without manual input, without fixed timing assumptions, and without objective-specific Pokemon mutation commands.

The final `run_result.json` must contain `ok=true` and `target=MIGHTYENA_SWITCH_INTIMIDATE`. It must include route trace entries proving PC Rare Candy withdrawal, two-Pokemon party setup, real slot-0 Poochyena capture, Rare Candy evolution into Mightyena, Mightyena party lead placement, battle entry, and switch to the second Pokemon. It must include party facts showing slot 0 as `SPECIES_MIGHTYENA` with `abilityNum=0` and `ability=ABILITY_INTIMIDATE`, and slot 1 as a non-empty usable Pokemon. It must include a switch-in ability event observed after the switch with ability id `ABILITY_INTIMIDATE`.

The final route must not call `AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0` or `AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA`. It must not rely on sleeps or fixed frame counts to decide that movement, menus, text, evolution, or switch effects are complete. Frame advancement is allowed as the emulator clock, but completion must be recognized by flags, probe fields, route predicates, menu readiness, or event counters.

Targeted engine validation must still pass. At minimum, `make check TESTS="Shared Power"` must pass after any battle-event instrumentation. A normal non-probe ROM build must also pass so the automation-only probe does not leak into release builds.

## Idempotence and Recovery

Generated files belong under `build/` and must remain uncommitted. It is safe to delete `build/mgba_lua_spike/pc-rare-candy-smoke`, `build/mgba_lua_spike/poochyena-evolution-path`, and `build/mgba_lua_spike/mightyena-switch-intimidate` before retrying.

If the PC Rare Candy checkpoint reports that the PC has no Rare Candies, do not silently grant bag candies. First verify whether the ROM was rebuilt from this working tree. If the rebuilt ROM still lacks PC candies, record the finding in `Surprises & Discoveries` and either repair `NewGameInitPCItems` or escalate if the missing candies are intentional.

If real Poochyena capture is blocked by lack of Poke Balls or story progression, do not replace it with direct party creation for final acceptance. Add a smaller route checkpoint that obtains Poke Balls or reaches the required story state, record the change in `Decision Log`, and continue.

If the final battle does not have Shared Power active, inspect the battle setup code and existing tests before adding any test-only override. A guarded override may be acceptable only if it changes battle type while preserving the real party, real evolution, normal battle menu, and switch-in event path. Such a decision must be recorded here with evidence.

If ability event instrumentation changes battle behavior or test output, revert that instrumentation and choose a less invasive observation point. Observability must not alter battle semantics.

## Artifacts and Notes

Important existing evidence before implementation:

    src/player_pc.c: sNewGamePCItems currently seeds ITEM_RARE_CANDY with quantity 999.
    include/automation_probe.h: gAutomationProbe already exposes party species, levels, ability slots, resolved abilities, and bag Rare Candy count.
    tools/mgba/mgba_lua_spike.py: drive_poochyena_intimidate currently calls guarded create/promote commands and labels canonical catch/evolution as deferred work.
    test/battle/shared_power.c: Shared Power tests already assert ABILITY_POPUP(..., ABILITY_INTIMIDATE) after switch-in scenarios.

Record future milestone evidence here as concise transcripts. Each evidence entry should include the command, whether it passed, the output artifact path, and the specific JSON fields or test lines that prove the milestone.

## Interfaces and Dependencies

Edit `include/automation_probe.h` when adding probe fields. Keep fields as `u32` values because the Python decoder reads a flat word layout. Increment `AUTOMATION_PROBE_VERSION` only if the host needs to reject old probe layouts; otherwise update `PROBE_FIELD_LAYOUT` in lockstep with the C struct and validate `size`.

Edit `src/automation_probe.c` to fill the new facts. Use existing repository style: C functions use `PascalCase`, local variables and struct fields use `camelCase`, constants use `UPPER_SNAKE_CASE`, and C indentation is four spaces. Keep probe code under `#if AUTOMATION_PROBE` so normal builds compile it away.

Use existing PC item APIs and data. `src/player_pc.c` owns `NewGameInitPCItems` and the item storage menu flow. `include/player_pc.h` exposes PC menu state, but not all item helpers; if additional item storage helpers are needed, inspect `src/item.c`, `include/item.h`, and `src/player_pc.c` before adding declarations. Prefer observing `gSaveBlock1Ptr->pcItems` through the probe over duplicating UI internals in Python.

Edit `tools/mgba/mgba_lua_spike.py` to add route nodes, predicates, and modes. Keep the existing modes working. If the file becomes too large, introduce a small module under `tools/mgba/` for route composition, but keep the command-line entry point stable for existing users.

Use `tools/mgba/mgba_lua_bridge.lua` only for emulator-level primitives such as key input, frame advancement, memory reads, memory writes for existing guarded probe commands, savestate capture, and screenshot capture. Do not put gameplay route logic in Lua; route decisions belong in Python for this phase.

Potential battle event instrumentation sites must be chosen after reading the battle code that emits ability popups and messages. Existing tests refer to `ABILITY_POPUP` and Intimidate messages, so search for the C code that drives those events before editing. The event fields should record only observations, such as an incrementing counter and last ability id, side, battler, and source party slot. They must not influence battle decisions.

Update `PATCH_NOTES.md` after each code, data, or documentation change. Follow the repository's existing top-entry style and use `commit pending` until the final commit exists.

Revision Note (2026-04-29): Initial ExecPlan. It captures the move from a guarded Mightyena proof to a canonical route tree made of reusable checkpoints for PC Rare Candy withdrawal, real Poochyena capture, Rare Candy evolution, party ordering, battle entry, and switch-time Intimidate observation.
