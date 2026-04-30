# Build a Canonical Mightyena Intimidate Switch Automation Route

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This repository stores the ExecPlan protocol in `.agent/PLANS.md`. Maintain this file according to that protocol: milestones must be completed in order, evidence must be recorded in this document, and execution should continue until the route works end to end or a true blocker requires escalation.

Execution note: Milestones 5 and later are superseded by the governed Scala implementation plan in `../ouroboros-ide/logs/plans/20260430_mightyena_switch_scala_route_execplan.md`. Python commands below are retained only as historical context for earlier spike evidence and must not be extended for new canonical route work.

## Purpose / Big Picture

The goal is to turn the existing automation spike into a reusable Scala gameplay route that proves a concrete Shared Power behavior in the running game. After this plan succeeds, one governed Scala headless command will start from a clean build, obtain two Pokemon through named route checkpoints, evolve a caught slot-0 Poochyena into a lead Mightyena by using Rare Candies from the player PC, enter a battle, switch from Mightyena to the second Pokemon, and prove that Intimidate appears again on switch-in.

This matters because the previous Mightyena proof was intentionally non-canonical: it used guarded automation commands to create Poochyena and directly mutate it into Mightyena. That proved the probe and savestate plumbing, but it did not build the route we want to reuse for future gameplay tests. This plan replaces the ad-hoc proof with a small route tree whose checkpoints can be shared by later objectives, such as catching, evolving, party ordering, and battle-menu navigation.

The human-visible proof is a Scala route artifact with `ok=true`, `target=MIGHTYENA_SWITCH_INTIMIDATE`, party facts showing slot 0 as `SPECIES_MIGHTYENA` with `ABILITY_INTIMIDATE`, party facts showing a second usable Pokemon, and battle event facts showing that `ABILITY_INTIMIDATE` fired after switching to the second Pokemon. The run should also write ignored savestate/screenshot artifacts so a human can inspect the final battle state. The Python file `tools/mgba/mgba_lua_spike.py` is frozen as reference-only debugging material and must not receive new canonical route logic.

## Progress

- [x] (2026-04-29 22:42 -04:00) Created this ExecPlan after reading `.agent/PLANS.md`, the current automation probe, the mGBA Lua host, the player PC Rare Candy initialization, and existing Shared Power Intimidate tests.
- [x] (2026-04-29 22:49 -04:00) Milestone 1 revalidated the headless probe baseline: the builder image produced `pokeemerald.gba` and `pokeemerald.sym`, and agent-5 ran `probe-smoke` successfully with probe magic `0x41505242`, version `1`, size `296`, route stage `13`, and zero party members.
- [x] (2026-04-29 22:53 -04:00) Milestone 2 added generic observability for PC Rare Candies, selected player battle-control fields, and ability-popup events. `probe-smoke` passed with probe version `2`, size `360`, `pcRareCandyCount=999`, `pcRareCandySlot=0`, `pcUsedItemSlots=1`, and `abilityPopupSequence=0` outside battle.
- [x] (2026-04-29 23:03 -04:00) Milestone 3 implemented and verified `pc-rare-candy-smoke`: the route reaches the bedroom PC, waits on probe-recorded PC menu states, withdraws one Rare Candy through the real item-storage flow, and exits back to semantic field movement readiness with PC Rare Candy `999 -> 998` and bag Rare Candy `0 -> 1`.
- [x] (2026-04-30 05:08 -04:00) Milestone 4 story subcheckpoint passed: `story-pokeballs-smoke` withdrew 25 PC Rare Candies, used 6 on the starter through Start Menu -> Bag -> Party, won the rival path, returned to Birch's lab, and ended with `bagPokeBallCount=5`, `bagRareCandyCount=19`, `pcRareCandyCount=974`, level-11 `SPECIES_MUDKIP`, and field movement ready in Birch's lab.
- [x] (2026-04-30 05:38 -04:00) Milestone 4 passed: `poochyena-capture-smoke` bought 15 Poke Balls at Oldale Mart through the normal shop flow, entered Route 101 grass, ran from non-target encounters, and ended with party `[SPECIES_MUDKIP, SPECIES_POOCHYENA]`; the Poochyena was level 3 with `abilityNum=0`, `ability=50`, and the bag still had 17 Poke Balls.
- [x] (2026-04-30 06:08 -04:00) Corrected implementation target: the Python host is now documented as reference-only, and remaining route implementation must move to the governed Scala automation in `../ouroboros-ide`.
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

- Observation: The agent-5 devkit container has the required mGBA runtime but not the ARM cross-compiler.
  Evidence: `docker exec devkit-ouro8-dev-agent-5 ... make ... rom syms` failed with `arm-none-eabi-gcc: command not found`; `docker run ... pokeemerald-expansion:builder make ... rom syms` succeeded and wrote the ROM and symbols consumed by the agent-5 `probe-smoke` run.

- Observation: Overworld beacon `scriptWaitKind` could stay at `WAIT_BUTTON` after a PC script ended, causing false `textReady=1` and preventing semantic `movementReady=1`.
  Evidence: The first tightened `pc-rare-candy-smoke` exit wait saw `flags=0`, `inputReady=1`, `textReady=1`, `movementReady=0`, and `scriptWaitKind=10` forever after PC exit. Clearing stale script-step readiness when no script and no field message are active made the rerun exit with `movementReady=1`, `textReady=0`, and `scriptWaitKind=0`.

- Observation: Route 101's rescue Zigzagoon is only removed from the active map instance; its hide flag is not set in either this branch, `origin/main`, `origin/master`, or upstream `pret/pokeemerald`.
  Evidence: `Route101_EventScript_BirchsBag` calls `removeobject LOCALID_ROUTE101_ZIGZAGOON`, then warps to Birch's lab without `setflag FLAG_HIDE_ROUTE_101_ZIGZAGOON`; returning to Route 101 reloads the object at `(10,13)`. The route now uses the Birch-cleared x=9 lane rather than forcing the blocked x=10 tile.

- Observation: Beacon raw `flags` can report script/controls bits without the message-box bit while semantic `textReady=1` and `scriptWaitKind=WAIT_BUTTON` still prove that pressing A is the correct progress action.
  Evidence: A `story-pokeballs-smoke` run timed out during `rival house stairs approach` with `flags=3`, `inputReady=1`, `textReady=1`, and `scriptWaitKind=10`. The movement waiter now treats semantic text readiness as actionable instead of relying only on `flags & 4`.

- Observation: Coordinate-to-coordinate movement is not a pathfinder; its greedy X-first behavior can repeatedly press into terrain even when a valid nearby route exists.
  Evidence: After returning to Route 101 from Littleroot, the story route tried to move from `(10,19)` to `(9,14)` and repeatedly pressed left into a blocked tile. A collision-grid inspection of `data/layouts/Route101/map.bin` produced reusable northbound waypoints `(10,14) -> (7,14) -> (7,10) -> (13,10) -> (13,9) -> (15,9) -> (15,2) -> (11,2) -> (11,0) -> (10,0)` and return waypoints `(10,5) -> (13,5) -> (13,6) -> (15,6) -> (15,12) -> (7,12) -> (7,17) -> (10,17) -> (10,19)`.

- Observation: Walking through Route 101 grass before receiving Poke Balls can enter incidental wild battles, and battle intro text is not represented by field text readiness.
  Evidence: The collision-grid Route 101 path reached `(13,10)` and entered a wild `SPECIES_WURMPLE` battle with `inBattle=1`, `fieldTextReady=0`, and stale `battleMenuState=0`. A later run reached a wild `SPECIES_POOCHYENA` action menu where `battleMenuState=ACTION` but `battleMenuFrame` was not a heartbeat, so freshness rejection prevented running. The incidental run-away helper now owns the full loop: advance battle text, navigate the visible action menu to Run without stale-frame rejection, press A, and wait for field readiness after battle exit.

- Observation: Route 103 has the same limitation as Route 101: the shortest geometric path from the south connection to the rival is not a legal walking path.
  Evidence: After exiting Oldale north, the route reached Route 103 at `(11,21)`. A collision-grid inspection of `data/layouts/Route103/map.bin` produced reusable rival-approach waypoints `(11,13) -> (14,13) -> (14,7) -> (12,7) -> (12,6) -> (5,6) -> (5,4) -> (10,4)`. The rival exit script later deposits the player back at Route 103's south connection around `(11,21)`, so the return leg should use the connection directly rather than replaying north-route waypoints.

- Observation: The default starter selection is a fragile branch for the canonical story checkpoint.
  Evidence: The default middle-ball route selected `SPECIES_TORCHIC`, reached the Route 103 rival battle against level-5 `SPECIES_MUDKIP`, used the first damaging move, lost the battle, and then warped to May's house 1F at map `(1,2)` with the starter healed. This explains the later apparent navigation stall: the route was no longer in Littleroot Town outside Birch's lab.

- Observation: Starter matchup alone is not a robust invariant for the Route 103 rival battle.
  Evidence: A follow-up run selected `SPECIES_MUDKIP` explicitly, reached the rival battle against level-5 `SPECIES_TREECKO`, repeatedly selected the first move, lost, and warped back to May's house 1F. The route therefore needs to use the canonical PC Rare Candy resource and normal item UI to overlevel the starter before the rival battle, which also advances the later Poochyena evolution milestone.

- Observation: Menu and script readiness can appear one layer deeper than the first probe fact that identifies the screen.
  Evidence: The Rare Candy item list was visible with `bagMenuState=ITEM_LIST` and `bagMenuItemId=ITEM_RARE_CANDY`, but the first A press during bag startup was ignored until the bag accepted input. Later, after the Route 103 rival fight, Oldale script control could lock movement without setting `fieldTextReady`. The host now retries the item-list A press while the desired item remains selected and lets target-map movement waits conservatively advance script-locked prompts.

- Observation: Littleroot also needs explicit waypoints when returning from the north connection.
  Evidence: After returning from Route 101, the player reached Littleroot at `(10,0)` and the greedy coordinate mover repeatedly faced left because it tried to correct X before walking south. The first audited path reached `(10,14)`, but the west step from there was blocked. The second path stepped onto the lab warp tile `(7,16)` before the outdoor waypoint waiter could observe completion. The route now stops on the lower lab lane `(10,17) -> (7,17)`, then lets `probe_move_until_map(UP, Birch Lab)` own the warp.

- Observation: The five story Poke Balls are not a reliable capture resource for the canonical Poochyena checkpoint.
  Evidence: `poochyena-capture-smoke` observed a target level-2 `SPECIES_POOCHYENA` with `enemy0AbilityNum=0` but exhausted all five story Poke Balls and failed with `RuntimeError: ran out of Poke Balls while catching Poochyena`. The route now adds an Oldale Mart purchase checkpoint using normal shop UI and probe-visible shop state before entering Route 101 grass.

- Observation: The Oldale Mart clerk must be approached from the accessible customer side of the counter, not from the clerk's object coordinate.
  Evidence: The first shop run entered Oldale Mart and reached `(2,5)`, then repeatedly pressed up into blocked counter tiles while trying to reach `(2,3)`. The Mart layout marks `(2,3)`, `(2,4)`, and `(1,4)` as collision tiles; the route now stands on `(3,3)` and faces left toward the counter tile `(2,3)`.

- Observation: Leaving Oldale Mart also needs explicit town waypoints; straight down from the Mart door is blocked before the south connection.
  Evidence: After the first successful purchase, the probe showed `bagPokeBallCount=20`, `shopMenuMoney=300`, and the player stuck in Oldale at `(14,13)` after exhausting the straight-down map-transition presses. The route now moves from the Mart door back to the central lane `(10,7) -> (10,19)` before stepping south to Route 101.

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

- Decision: Record ability-popup events at `CreateAbilityPopUp` rather than in Intimidate-specific battle logic.
  Rationale: The final route needs to know when Intimidate visibly appears, but the probe should remain generic. Recording every ability popup with ability id, battler, side, position, party index, frame, and sequence avoids coupling the probe to one ability while giving the host a reliable monotonic event counter.
  Date/Author: 2026-04-29 / Codex

- Decision: Add PC menu state as probe data instead of driving the PC flow from fixed frame sleeps.
  Rationale: The PC flow uses normal game menus with no existing semantic beacon. Exposing the current PC menu state, cursor, item id, and quantity lets the host press buttons only after the game has reached the expected menu state, preserving the route's flag/event-driven shape.
  Date/Author: 2026-04-29 / Codex

- Decision: Keep Route 101 navigation as explicit audited waypoints rather than modifying the base map script to hide the rescue Zigzagoon.
  Rationale: The persistent object matches upstream decomp data, and the object has a valid route-around lane. Waypoints make the automation robust without changing game content while still documenting the surprising object-state behavior for later review.
  Date/Author: 2026-04-30 / Codex

- Decision: Select Mudkip explicitly for the early story route instead of inheriting the starter menu default.
  Rationale: The final objective does not depend on starter identity, and the observed default Torchic branch can lose the mandatory rival battle before Poke Balls. Making starter choice an explicit beacon-gated route input removes a fragile default without adding mutation commands or weakening the canonical gameplay path.
  Date/Author: 2026-04-30 / Codex

- Decision: Fold normal Rare Candy item use into the route tree now rather than treating rival-battle wins as matchup-dependent.
  Rationale: The PC Rare Candy supply is already part of the accepted canonical path and the item-use UI is needed again for Poochyena evolution. Withdrawing candies early and using them on the starter before Route 103 is a stronger route invariant than selecting a different starter and hoping the deterministic battle remains favorable after incidental encounters.
  Date/Author: 2026-04-30 / Codex

- Decision: Buy additional Poke Balls at Oldale Mart through the normal shop flow before attempting the Route 101 Poochyena capture.
  Rationale: Retrying capture with only the five story reward balls creates a low-resource stochastic checkpoint. The shop is already unlocked after the Pokedex/Poke Ball story reward, and a probe-visible shop state keeps the route event-driven without adding item-grant commands.
  Date/Author: 2026-04-30 / Codex

- Decision: Freeze `tools/mgba/mgba_lua_spike.py` as reference-only and move canonical route logic into the governed Scala automation implementation.
  Rationale: The Python spike was useful for discovery, but the target route needs typed route state, governed artifacts, and explicit invariants. The Python file may remain useful for comparing bridge/probe behavior, but extending it would continue the wrong implementation path.
  Date/Author: 2026-04-30 / Codex

## Outcomes & Retrospective

This plan is newly authored. No implementation outcome has been produced yet. The expected end state is an end-to-end automation route that demonstrates Mightyena-derived Intimidate firing again when the player switches to a second Pokemon, with evidence in JSON, savestate, screenshot, targeted tests, and committed source changes.

## Context and Orientation

This repository is a Pokemon Emerald decompilation-based project. The game code is C under `src/`, public headers are under `include/`, gameplay data is under `data/`, tests are under `test/`, and automation host scripts are under `tools/mgba/`. The final ROM is `pokeemerald.gba`, and `pokeemerald.sym` or `pokeemerald.map` provides symbol addresses for host automation.

The current automation stack has three layers. The first layer is mGBA, the emulator. The second layer is `tools/mgba/mgba_lua_bridge.lua`, a Lua script run by mGBA that accepts socket commands from the host and can press buttons, advance frames, and read or write emulator memory. The canonical third layer is now the Scala automation implementation in `../ouroboros-ide`; it should launch or connect to mGBA, talk to the Lua bridge, read game facts, and drive named route modes. The older `tools/mgba/mgba_lua_spike.py` Python host is reference-only and should not receive new route FSM logic.

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

Status: this section is historical for the Python spike through the real Poochyena capture checkpoint. Do not add new Python modes or use these commands as acceptance for Milestones 5 and later. Continue the current implementation from the Scala/Ouroboros ExecPlan instead.

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

Milestone 1 evidence:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms
    Result: passed; ROM used 24738452 bytes, and `pokeemerald.gba` plus `pokeemerald.sym` were regenerated.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 tools/mgba/mgba_lua_spike.py --mode probe-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-smoke --port 46711 --connect-timeout 20 --starter-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/probe-smoke/run_result.json"}`.
    Evidence fields: `target=PROBE_SMOKE`, `elapsedSeconds=8.059`, `probe.magic=0x41505242`, `probe.version=1`, `probe.size=296`, `probe.routeStage=13`, `probe.playerPartyCount=0`.

Milestone 2 evidence:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms
    Result: passed; ROM used 24738916 bytes.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 tools/mgba/mgba_lua_spike.py --mode probe-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-smoke --port 46711 --connect-timeout 20 --starter-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/probe-smoke/run_result.json"}`.
    Evidence fields: `probe.version=2`, `probe.size=360`, `probe.routeStage=13`, `probe.playerPartyCount=0`, `probe.pcRareCandyCount=999`, `probe.pcRareCandySlot=0`, `probe.pcUsedItemSlots=1`, `probe.playerBattleBattler=4`, `probe.battleControllerExecFlags=0`, `probe.abilityPopupSequence=0`, `probe.abilityPopupAbility=0`.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; luac -p tools/mgba/mgba_lua_bridge.lua'
    Result: passed.

Milestone 3 evidence:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms
    Result: passed; ROM used 24739148 bytes.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; python3 tools/mgba/mgba_lua_spike.py --mode pc-rare-candy-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/pc-rare-candy-smoke --port 46712 --connect-timeout 20 --objective-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/pc-rare-candy-smoke/run_result.json"}`.
    Evidence fields: `target=PC_RARE_CANDY_SMOKE`, `elapsedSeconds=3.871`, `beforeProbe.version=3`, `beforeProbe.size=380`, `beforeProbe.pcRareCandyCount=999`, `beforeProbe.bagRareCandyCount=0`, `afterProbe.pcRareCandyCount=998`, `afterProbe.bagRareCandyCount=1`, `exitBeacon.movementReady=1`, `exitBeacon.textReady=0`, `exitBeacon.scriptWaitKind=0`.

Milestone 4 evidence:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; rm -rf build/mgba_lua_spike/probe-smoke-v6; python3 tools/mgba/mgba_lua_spike.py --mode probe-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-smoke-v6 --port 46731 --connect-timeout 20 --starter-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/probe-smoke-v6/run_result.json"}`.
    Evidence fields: `probe.version=6`, `probe.size=556`, `probe.wordCount=139`, `probe.shopMenuState=0`, `probe.playerPartyCount=0`, `probe.routeStage=13`.

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; rm -rf build/mgba_lua_spike/poochyena-capture-smoke; python3 tools/mgba/mgba_lua_spike.py --mode poochyena-capture-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/poochyena-capture-smoke --port 46734 --connect-timeout 20 --objective-timeout 1200 --starter-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/poochyena-capture-smoke/run_result.json"}`.
    Evidence fields: `target=POOCHYENA_CAPTURE_SMOKE`, `elapsedSeconds=43.03`, `finalProbe.playerPartyCount=2`, `finalProbe.partySpecies=[258,261,0,0,0,0]`, `finalProbe.partyLevel=[11,3,0,0,0,0]`, `finalProbe.partyAbilityNum=[0,0,0,0,0,0]`, `finalProbe.partyAbility=[67,50,0,0,0,0]`, `finalProbe.bagPokeBallCount=17`, `routeTrace.Inventory.PokeBallsBoughtAtOldaleMart.quantity=15`, `routeTrace.Inventory.PokeBallsBoughtAtOldaleMart.afterPokeBallCount=20`, `routeTrace.Party.CaughtPoochyenaAbilitySlot0.captureAttempts=4`.

Milestone 4 story subcheckpoint evidence:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; rm -rf build/mgba_lua_spike/story-pokeballs-smoke; python3 tools/mgba/mgba_lua_spike.py --mode story-pokeballs-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/story-pokeballs-smoke --port 46733 --connect-timeout 20 --objective-timeout 600 --starter-timeout 600'
    Result: passed with `{"ok": true, "result": "build/mgba_lua_spike/story-pokeballs-smoke/run_result.json"}`.
    Evidence fields: `target=STORY_POKEBALLS_SMOKE`, `elapsedSeconds=26.598`, `finalProbe.playerPartyCount=1`, `finalProbe.partySpecies[0]=SPECIES_MUDKIP`, `finalProbe.partyLevel[0]=11`, `finalProbe.bagPokeBallCount=5`, `finalProbe.bagRareCandyCount=19`, `finalProbe.pcRareCandyCount=974`, `finalProbe.map=(1,4)`, `finalProbe.fieldMovementReady=1`.

## Interfaces and Dependencies

Edit `include/automation_probe.h` when adding probe fields. Keep fields as `u32` values because the Python decoder reads a flat word layout. Increment `AUTOMATION_PROBE_VERSION` only if the host needs to reject old probe layouts; otherwise update `PROBE_FIELD_LAYOUT` in lockstep with the C struct and validate `size`.

Edit `src/automation_probe.c` to fill the new facts. Use existing repository style: C functions use `PascalCase`, local variables and struct fields use `camelCase`, constants use `UPPER_SNAKE_CASE`, and C indentation is four spaces. Keep probe code under `#if AUTOMATION_PROBE` so normal builds compile it away.

Use existing PC item APIs and data. `src/player_pc.c` owns `NewGameInitPCItems` and the item storage menu flow. `include/player_pc.h` exposes PC menu state, but not all item helpers; if additional item storage helpers are needed, inspect `src/item.c`, `include/item.h`, and `src/player_pc.c` before adding declarations. Prefer observing `gSaveBlock1Ptr->pcItems` through the probe over duplicating UI internals in Python.

Edit `tools/mgba/mgba_lua_spike.py` to add route nodes, predicates, and modes. Keep the existing modes working. If the file becomes too large, introduce a small module under `tools/mgba/` for route composition, but keep the command-line entry point stable for existing users.

Use `tools/mgba/mgba_lua_bridge.lua` only for emulator-level primitives such as key input, frame advancement, memory reads, memory writes for existing guarded probe commands, savestate capture, and screenshot capture. Do not put gameplay route logic in Lua; route decisions belong in Python for this phase.

Potential battle event instrumentation sites must be chosen after reading the battle code that emits ability popups and messages. Existing tests refer to `ABILITY_POPUP` and Intimidate messages, so search for the C code that drives those events before editing. The event fields should record only observations, such as an incrementing counter and last ability id, side, battler, and source party slot. They must not influence battle decisions.

Update `PATCH_NOTES.md` after each code, data, or documentation change. Follow the repository's existing top-entry style and use `commit pending` until the final commit exists.

Revision Note (2026-04-29): Initial ExecPlan. It captures the move from a guarded Mightyena proof to a canonical route tree made of reusable checkpoints for PC Rare Candy withdrawal, real Poochyena capture, Rare Candy evolution, party ordering, battle entry, and switch-time Intimidate observation.

Revision Note (2026-04-29, Milestone 1): Recorded baseline build and probe-smoke evidence, plus the environment discovery that builds must use the builder image while headless emulator runs use agent-5.

Revision Note (2026-04-29, Milestone 2): Recorded probe ABI version 2 evidence, including PC Rare Candy counts, player battle-control defaults, and ability-popup event defaults.

Revision Note (2026-04-29, Milestone 3): Recorded the deterministic PC Rare Candy smoke route, the added PC menu probe states, and the stale overworld script-step fix required to prove field movement readiness after exiting the PC.
