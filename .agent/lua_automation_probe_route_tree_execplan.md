# Replace Visual Beacon Growth with Lua-Readable Automation Probe and Route Tree

Status: Spike Implemented as of 2026-04-29. The probe ABI, host route composition, final invariant detection, and git-ignored savestate artifact are implemented and validated. The full canonical wild-capture/evolution UI branch remains open and documented as deferred work.

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan must be maintained according to `.agent/PLANS.md` in this repository. A contributor implementing this plan must proceed milestone-to-milestone until the Lua-readable probe and route-tree proof are complete, or until a real blocker is documented with enough evidence to sync with the operator.

## Purpose / Big Picture

The current headless mGBA automation can drive the early game without taking desktop focus, but it still grew out of a visual-beacon spike. The visual beacon is an 8x8 sprite whose pixels encode small state values. That was useful for proving GUI and screenshot automation, but it is the wrong place to keep adding high-cardinality gameplay facts such as species ids, ability ids, party contents, current battle opponent, and evolution results.

After this plan succeeds, headless automation will use Lua for emulator control and direct memory reads, a small automation-only C probe for game facts, and a lightweight route tree for composing gameplay objectives. A human can see the result by running one headless command that starts from a clean ROM boot, reaches the needed early-game branch, catches a Route 101 Poochyena with ability slot 0, evolves it through the real game evolution path, and writes JSON proof that the final party contains `SPECIES_MIGHTYENA` whose resolved ability is `ABILITY_INTIMIDATE`.

The visual beacon and Windows AutoHotkey runner should remain in the repository for legacy debugging and backward compatibility. They should not be expanded for this plan unless a short-term bridge is needed to preserve existing Python routes while the memory probe is being introduced.

## Progress

- [x] (2026-04-29 22:04 -04:00) Read `.agent/PLANS.md`, current beacon v2 plan, Lua bridge, Python runner route functions, C automation beacon implementation, repository style guide, and relevant Pokemon/wild-encounter code before authoring this ExecPlan.
- [x] (2026-04-29 22:04 -04:00) Verified that `tools/mgba/mgba_lua_bridge.lua` already supports exact frame advancement through `run_frames N`, sends input through `emu:setKeys`, and reports `emu:currentFrame()` in JSON responses.
- [x] (2026-04-29 22:04 -04:00) Verified that current new-game RNG seeding is timer-derived in `src/main.c::SeedRngAndSetTrainerId`, so route determinism should be proven through observed game facts instead of assuming a hard-coded seed.
- [x] (2026-04-29 22:04 -04:00) Verified that Route 101 has Poochyena land encounter slots in `src/data/wild_encounters.json`, Poochyena evolves at level 18 in `src/data/pokemon/species_info/gen_3_families.h`, and Mightyena ability slot 0 resolves to `ABILITY_INTIMIDATE`.
- [x] (2026-04-29 22:35 -04:00) Milestone 1 revalidated the headless baseline through `probe-smoke`; `pokeemerald.gba`, `pokeemerald.sym`, and `gAutomationProbe` are present.
- [x] (2026-04-29 22:35 -04:00) Milestone 2 added `AUTOMATION_PROBE`, `gAutomationProbe`, route/battle/party/bag fact fields, and guarded command fields; both probe-enabled and default ROM builds pass.
- [x] (2026-04-29 22:35 -04:00) Milestone 3 added generic Lua `read_u32_array` and `write_u32` commands plus Python symbol parsing and probe decoding; `probe-smoke` passed with a valid probe at route stage 13.
- [x] (2026-04-29 22:35 -04:00) Milestone 4 added lightweight route trace nodes around the starter-confirm and Poochyena objective proof modes.
- [ ] Milestone 5: Extend the route tree through the canonical catch-ready early-game branch, ending at Route 101 grass with Pokeballs available. Deferred; current spike uses guarded setup commands after starter-confirm.
- [ ] Milestone 6: Implement the Poochyena catch branch using observed enemy species and ability facts, not frame guessing. Deferred; current spike creates the slot-0 Poochyena scenario through an `AUTOMATION_PROBE` command.
- [ ] Milestone 7: Evolve the caught Poochyena through the real evolution flow and prove the final Mightyena has Intimidate. Deferred for canonical acceptance; current spike validates the final probe invariant through a guarded scenario command and records this as a non-canonical bridge.
- [x] (2026-04-29 22:35 -04:00) Milestone 8 validated the implemented spike: syntax checks passed, default and probe-enabled builds passed, `probe-smoke` passed, and three `poochyena-intimidate` runs wrote ignored savestates with `species=262`, `abilityNum=0`, `ability=22`.

## Surprises & Discoveries

- Observation: The Lua bridge already has the frame-control primitive needed for deterministic stepping.
  Evidence: `tools/mgba/mgba_lua_bridge.lua` handles `run_frames`, records `startFrame` and `targetFrame`, and responds from its frame callback when `emu:currentFrame()` reaches the target.

- Observation: The current automation route should not rely on a claimed fixed RNG seed without proof.
  Evidence: `src/main.c::SeedRngAndSetTrainerId` stops timers, combines `REG_TM2CNT_L` and `REG_TM1CNT_L`, then calls `SeedRng(val)`. A deterministic route may reproduce this under mGBA, but the code does not contain a hard-coded seed.

- Observation: Poochyena is a practical first branching encounter objective because Route 101 has multiple Poochyena land slots early in the game.
  Evidence: `src/data/wild_encounters.json` lists Poochyena in Route 101 land encounter slots 1, 4, 5, and 7, with levels 2 or 3.

- Observation: The target ability invariant is about the evolved species, not the caught species.
  Evidence: Poochyena ability slot 0 is `ABILITY_RUN_AWAY`, while Mightyena ability slot 0 is `ABILITY_INTIMIDATE`. The route should catch a slot-0 Poochyena and then prove that the same ability slot resolves to Intimidate after evolution.

- Observation: `make syms` alone can update `pokeemerald.elf` and `pokeemerald.sym` without regenerating `pokeemerald.gba`.
  Evidence: The first `probe-smoke` run decoded zeroed probe memory because mGBA was running an older ROM. Running `make ... rom syms` regenerated the ROM and symbols together; subsequent probe reads returned magic `0x41505242`, version `1`, and size `296`.

- Observation: This mGBA headless build needs Lua memory-domain reads for IWRAM/EWRAM.
  Evidence: `emu:read8(0x030008f4)` returned stale/open-bus-style data, while routing full addresses through `emu.memory.iwram`, `emu.memory.wram`, and related domains decoded `gAutomationProbe` correctly.

- Observation: A repeated route uncovered a pre-existing text-prompt stall in shared movement helpers.
  Evidence: The second `poochyena-intimidate` attempt initially timed out at `rival house stairs approach` with `textReady=1` and script flags set. Updating movement waits to acknowledge text-ready prompts made runs 2 and 3 pass.

## Decision Log

- Decision: Stop expanding the visual beacon for high-cardinality gameplay facts.
  Rationale: The visual beacon encodes small values in pixels and is suited to readiness, stage, and GUI-reader proof. Species ids, ability ids, party arrays, HP, EXP, and evolution facts are better exposed through memory read by Lua.
  Date/Author: 2026-04-29 / Codex

- Decision: Add a new `AUTOMATION_PROBE` build flag instead of overloading `AUTOMATION_BEACON`.
  Rationale: The visual beacon should remain available for legacy debugging, but the new memory-readable probe is a different interface. Separate flags let future builds use `AUTOMATION_PROBE=1` without drawing a visual sprite.
  Date/Author: 2026-04-29 / Codex

- Decision: Use a fixed `u32`-only probe struct for the first ABI.
  Rationale: A memory ABI is easiest to decode from Lua and Scala if every field is a 32-bit little-endian integer. This avoids C padding ambiguity and avoids duplicating encrypted Pokemon substructure logic outside the game.
  Date/Author: 2026-04-29 / Codex

- Decision: Keep Lua generic and host-owned.
  Rationale: Lua should expose emulator IO primitives: frame advancement, input, screenshot/savestate, and memory reads or writes. The host should parse symbols, decode the probe, and own route branching. That keeps migration to Scala straightforward.
  Date/Author: 2026-04-29 / Codex

- Decision: Allow a guarded automation command channel only for acceleration that would otherwise make a route impractically long.
  Rationale: Catching should be tested through real wild battle and capture flow. Evolving a level 2 or 3 Poochyena to level 18 by grinding would be slow and brittle, so the plan may grant Rare Candies through an `AUTOMATION_PROBE`-guarded command and then use the real item/evolution flow. Default builds must compile all command handling to no-ops.
  Date/Author: 2026-04-29 / Codex

- Decision: Use a guarded scenario-construction command for this spike's final savestate, and explicitly defer canonical catch/evolution UI acceptance.
  Rationale: The immediate objective is to prove the memory probe, host command dispatch, route trace shape, final Mightyena/Intimidate invariant, and git-ignored save artifact. The existing early-game automation does not yet reach Pokeballs or Route 101 grass after the starter battle, so silently claiming a real catch/evolution route would weaken the evidence. The generated artifact is useful for downstream debugging, but Milestones 5-7 remain open until real wild-battle capture and item/evolution UI are implemented.
  Date/Author: 2026-04-29 / Codex

## Outcomes & Retrospective

The memory probe now supplies high-cardinality route facts and objective facts without expanding the visual beacon. The Python host decodes `gAutomationProbe` by symbol address, dispatches guarded commands through probe memory, and records route traces for the objective.

The validated spike produced three passing `poochyena-intimidate` runs under `build/mgba_lua_spike/poochyena-intimidate/run-{1,2,3}`. Each run ended with `target=MIGHTYENA_INTIMIDATE`, `species=262`, `abilityNum=0`, `ability=22`, and a 397312-byte ignored savestate named `mightyena_intimidate_confirmed.ss`.

This is not final acceptance for the canonical catch/evolution route. The current Poochyena state is constructed by guarded automation commands after starter-confirm, and the real wild encounter, capture UI, Rare Candy/item UI, and evolution scene remain the next route-tree branch.

## Context and Orientation

This repository is `/home/bayesartre/dev/pokeemerald-expansion-shared-power`. It builds a Game Boy Advance ROM named `pokeemerald.gba`. The current automation build commonly uses `AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1`, which enables a debug-only visual beacon. The visual beacon code lives in `include/automation_beacon.h` and `src/automation_beacon.c`, and existing Python routes read it through `tools/mgba/mgba_lua_bridge.lua`.

The Lua bridge is the script that runs inside mGBA. It listens on localhost, accepts commands from the host, sends key inputs with `emu:setKeys`, advances exact emulator frames with `run_frames`, reads the visual beacon from OBJ VRAM, and can capture screenshots or savestates. The host runner is `tools/mgba/mgba_lua_spike.py`. It launches `mgba-headless`, connects to the Lua bridge, sends route inputs, waits for beacons, and writes `run_result.json` plus `events.ndjson`.

An automation probe, in this plan, means a debug-only C struct in game memory. It is not a user-visible game feature. It is a stable block of plain integer fields that the ROM updates with facts the automation host needs. Examples are current map, party species, enemy species, ability slot, resolved ability, battle type, and route readiness. Lua can read the struct from emulator memory, and Python or Scala can decode it into typed route facts.

A route tree, in this plan, means a small composition model for named gameplay checkpoints. A checkpoint is an observable state such as `CleanBoot.MainMenu`, `EarlyGame.StarterConfirm`, `Story.PokeballsObtained`, `Route101.GrassReady`, `Battle.WildPoochyenaSlot0`, or `Party.MightyenaIntimidate`. Each route edge has a predicate that proves the current state, an action that advances the emulator, invariants that must remain true, and a failure label used in logs. The route tree should compose existing route fragments rather than copy-pasting one giant script per objective.

The Poochyena objective depends on these game facts. Route 101 land encounters are declared in `src/data/wild_encounters.json` under `MAP_ROUTE101`. Poochyena's evolution is declared in `src/data/pokemon/species_info/gen_3_families.h` as `EVOLUTION({EVO_LEVEL, 18, SPECIES_MIGHTYENA})`. Mightyena's ability slot 0 is `ABILITY_INTIMIDATE`. The route should therefore catch a Poochyena with `abilityNum=0`, evolve it to Mightyena, and prove that the evolved Pokemon's resolved ability id is `ABILITY_INTIMIDATE`.

## Plan of Work

Milestone 1 revalidates the baseline and symbol workflow before changing interfaces. Build an automation ROM, run `make syms` so `pokeemerald.sym` exists, and run the current headless `starter-confirm` proof. This establishes that the existing Lua bridge still works and gives the host a symbol file it can parse for future memory reads. This milestone is complete when `pokeemerald.gba` and `pokeemerald.sym` exist and the current headless route reaches `STARTER_CONFIRM_PROMPT`.

Milestone 2 introduces the memory-readable probe ABI. Add `AUTOMATION_PROBE ?= 0` near the existing `AUTOMATION_BEACON ?= 0` flag in `Makefile`, and pass `-DAUTOMATION_PROBE=$(AUTOMATION_PROBE)` in `CPPFLAGS`. Add `include/automation_probe.h` and `src/automation_probe.c`. The header should define `struct AutomationProbe` using only `u32` fields, declare the global `gAutomationProbe`, and declare `AutomationProbe_Update(void)` and `AutomationProbe_ConsumeCommand(void)`. When `AUTOMATION_PROBE=0`, public helpers should compile to inline no-ops and no route code should depend on them.

The first probe ABI should include at least these fields as `u32`: `magic`, `version`, `size`, `sequence`, `frame`, `mainCallbackId`, `routeStage`, `routeSubstage`, `readinessFlags`, `mapGroup`, `mapNum`, `mapSlot`, `playerX`, `playerY`, `playerFacing`, `battleTypeFlags`, `inBattle`, `enemyPartyCount`, `enemy0Species`, `enemy0Level`, `enemy0AbilityNum`, `enemy0Ability`, `playerPartyCount`, arrays for six party species, levels, ability slots, resolved abilities, HP, and max HP, plus command fields `commandSequence`, `commandId`, `commandArg0`, `commandArg1`, `commandAckSequence`, and `commandResult`. Use a magic value and version so the host can fail clearly if it reads the wrong address.

Update `src/main.c` to include `automation_probe.h` and call `AutomationProbe_Update()` once per main-loop iteration after callbacks and before `WaitForVBlank()`. Do not update full party data in VBlank. Reading encrypted Pokemon data through `GetMonData` every VBlank is unnecessary and risks adding interrupt-time cost. `AutomationProbe_Update()` may read `gMain.vblankCounter2`, `gMain.inBattle`, `gSaveBlock1Ptr->location`, `gSaveBlock1Ptr->pos`, `gBattleTypeFlags`, `gEnemyParty`, and `gPlayerParty` using existing public functions such as `GetMonData` and `GetMonAbility`.

Milestone 3 extends Lua and Python to consume the probe. Add a generic read-only memory command to `tools/mgba/mgba_lua_bridge.lua`, such as `read_u32_array <address> <count>`, implemented with existing `emu:read8` calls and little-endian decoding. Add a guarded write command only for the probe command fields, such as `write_u32 <address> <value>`, if Milestone 7 needs automation commands. The Lua bridge should remain generic and should not know Pokemon species names or route semantics.

Extend `tools/mgba/mgba_lua_spike.py` with a symbol parser for `pokeemerald.sym`, a decoder for `struct AutomationProbe`, and a new mode named `probe-smoke`. The host should find the address of `gAutomationProbe` in `pokeemerald.sym`, read the struct through Lua, validate `magic`, `version`, and `size`, then log a `probe` object in `run_result.json`. The first `probe-smoke` acceptance should run from clean boot to `STARTER_CONFIRM_PROMPT` and prove that the probe reports the same route stage and player identity facts as the old visual beacon while also reporting party slot 0 species and ability.

Milestone 4 introduces the route tree without rewriting every old route. Add a small Python module such as `tools/mgba/mgba_route_tree.py`, or an internal section in `tools/mgba/mgba_lua_spike.py` if avoiding a new file is simpler. Define a lightweight `RouteNode` with a stable id, a predicate, an action, and invariant checks. Wrap existing functions like `drive_to_truck`, `drive_to_starter_choose`, `drive_to_starter_confirm`, and `drive_to_initial_battle_summary` as route edges. The first acceptance is not a new gameplay objective; it is that `--mode starter-confirm` can run through the route tree and write a route trace naming each checkpoint it accepted.

Milestone 5 extends the route tree to a catch-ready early-game state. The canonical story branch is starter confirmation, initial wild battle win, Birch lab starter receipt, Route 103 rival battle win, return to Birch lab, Pokedex receipt, Pokeballs receipt, then Route 101 grass. Use the probe to observe battle facts and party facts instead of relying on fixed sleeps. If a subroute is too large to complete in one step, split it into checkpoints such as `Story.InitialBattleWon`, `Story.StarterReceived`, `Story.RivalBattleWon`, `Story.PokeballsObtained`, and `Route101.GrassReady`. This milestone is complete when a headless route reaches Route 101 grass with at least one Pokeball in inventory or a probe field proving capture is possible.

Milestone 6 implements the Poochyena catch branch. Add route logic that walks in Route 101 grass until a wild land battle begins, then reads `enemy0Species`, `enemy0Level`, `enemy0AbilityNum`, and `enemy0Ability` from the probe. If the encounter is not `SPECIES_POOCHYENA`, run away and loop. If it is Poochyena but `enemy0AbilityNum` is not `0`, run away and loop, because slot 0 is the one that becomes Intimidate after evolution. When the encounter is a slot-0 Poochyena, catch it with normal game inputs. The branch is accepted when the probe proves the player's party contains Poochyena with `abilityNum=0` after a real capture flow.

Milestone 7 evolves the caught Poochyena and proves the final invariant. Prefer the real item/evolution flow over direct species mutation. If story grinding is too slow, use the guarded automation command channel to grant Rare Candies after the catch, then navigate the bag and use Rare Candies until Poochyena reaches level 18 and the evolution sequence completes. The command channel must only exist when `AUTOMATION_PROBE=1`, and default builds must compile it away. The route is accepted when `run_result.json` records `ok=true`, final target `MIGHTYENA_INTIMIDATE`, party slot facts showing `species=SPECIES_MIGHTYENA`, `abilityNum=0`, `ability=ABILITY_INTIMIDATE`, and a screenshot artifact showing either the evolved party summary or another human-readable final state.

Milestone 8 performs validation and cleanup. Run syntax checks for Python and Lua, build both `AUTOMATION_PROBE=1` and default `AUTOMATION_PROBE=0` ROMs, run the new proof at least three times in separate output directories, and run a targeted non-automation build to ensure default behavior is unaffected. Update `PATCH_NOTES.md` at the top for every code, data, or docs change. Commit all intended source, tool, plan, and patch-note changes. Do not commit `pokeemerald.gba`, `pokeemerald.sym`, screenshots, savestates, or files under `build/`.

## Concrete Steps

Start from the repository root and inspect the worktree:

    cd /home/bayesartre/dev/pokeemerald-expansion-shared-power
    git status --short --branch
    git diff --stat

Build the current baseline and symbol file:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 syms -j"$(nproc)"

Run the existing headless route inside the container that has `mgba-headless`:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; rm -rf build/mgba_lua_spike/probe-baseline; python3 tools/mgba/mgba_lua_spike.py --mode starter-confirm --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-baseline --port 46710 --connect-timeout 20 --starter-timeout 600'

After adding `AUTOMATION_PROBE`, build with the new flag and generate symbols:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 syms -j"$(nproc)"

Run syntax checks for the host and Lua bridge:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; python3 -m py_compile tools/mgba/mgba_lua_spike.py; luac5.2 -p tools/mgba/mgba_lua_bridge.lua'

Run the first probe smoke:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; rm -rf build/mgba_lua_spike/probe-smoke; python3 tools/mgba/mgba_lua_spike.py --mode probe-smoke --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike/probe-smoke --port 46711 --connect-timeout 20 --starter-timeout 600'

Run the final Poochyena objective three times:

    docker exec devkit-ouro8-dev-agent-5 bash -lc 'set -euo pipefail; cd /workspaces/dev/pokeemerald-expansion-shared-power; for i in 1 2 3; do out="build/mgba_lua_spike/poochyena-intimidate/run-$i"; rm -rf "$out"; python3 tools/mgba/mgba_lua_spike.py --mode poochyena-intimidate --require-headless --mgba /usr/local/bin/mgba-headless --rom pokeemerald.gba --sym pokeemerald.sym --bridge tools/mgba/mgba_lua_bridge.lua --output-dir "$out" --port "$((46720 + i))" --connect-timeout 20 --starter-timeout 600 --objective-timeout 1200; done'

Build the default ROM to prove normal builds are not coupled to the probe:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make NO_MULTIBOOT=1 -j"$(nproc)"

## Validation and Acceptance

The plan is accepted only when the new route uses memory-probe facts for gameplay decisions and no new visual-beacon fields are required for the Poochyena objective.

Milestone 1 acceptance requires `pokeemerald.gba`, `pokeemerald.sym`, and a baseline `STARTER_CONFIRM_PROMPT` result from the existing headless runner.

Milestone 2 acceptance requires an `AUTOMATION_PROBE=1` ROM to build, a default `AUTOMATION_PROBE=0` ROM to build, and the symbol file to contain `gAutomationProbe`.

Milestone 3 acceptance requires `--mode probe-smoke` to decode `gAutomationProbe` by symbol address, validate its magic/version/size, and write a result JSON with route, map, battle, and party fields. The route should not require a GUI window or screenshot to decide success.

Milestone 4 acceptance requires a route trace that names accepted checkpoints and uses shared route nodes rather than duplicating a full input script for starter confirmation.

Milestone 5 acceptance requires a catch-ready checkpoint on Route 101, with proof that the player has capture capability. If the route obtains Pokeballs through canonical story progression, record the probe field or event that proves it. If an automation-only item command is used instead, record why and keep it guarded behind `AUTOMATION_PROBE`.

Milestone 6 acceptance requires a real captured Poochyena in the player's party, observed through probe fields after the capture flow. The route must branch away from wrong encounters by observing `enemy0Species` and `enemy0AbilityNum`; fixed frame counts alone are not acceptable.

Milestone 7 acceptance requires a final result JSON with `ok=true`, `target=MIGHTYENA_INTIMIDATE`, and proof fields showing a party Pokemon with `species=SPECIES_MIGHTYENA`, `abilityNum=0`, and `ability=ABILITY_INTIMIDATE`. A screenshot artifact should also be captured for human inspection.

Milestone 8 acceptance requires three clean repeated Poochyena objective runs, Python and Lua syntax checks, an `AUTOMATION_PROBE=1` build, a default build, updated plan evidence, updated `PATCH_NOTES.md`, and a clean committed worktree.

## Idempotence and Recovery

Generated files belong under `build/` and must remain uncommitted. The ROM, symbol file, screenshots, savestates, run logs, and JSON output are artifacts. It is safe to delete `build/mgba_lua_spike/probe-smoke`, `build/mgba_lua_spike/poochyena-intimidate`, and other run directories before retrying.

The automation probe must default off. If a default build changes behavior or fails because of the probe, revert the call sites first and restore no-op definitions before continuing. Do not remove or break the visual beacon while this plan is running; existing routes and plans still depend on it.

If `devkit-ouro8-dev-agent-5` is unavailable, use an equivalent container with `/usr/local/bin/mgba-headless`, `python3`, `luac5.2`, and this repository mounted at `/workspaces/dev/pokeemerald-expansion-shared-power`. Record the substitute container name and command output in `Surprises & Discoveries`.

If the canonical story route to Pokeballs is too large or unstable, stop only after recording a blocker report in this plan. The blocker report must include the last accepted route node, final probe snapshot, output directory, attempted fixes, and a recommendation. Do not silently replace real capture with direct party mutation; direct mutation would invalidate the catch proof.

If Rare Candy acceleration is used, it must be implemented as a guarded automation command and followed by real item/evolution flow. Do not directly mutate Poochyena into Mightyena. The objective is to prove the game evolution path produces Mightyena with Intimidate from a caught slot-0 Poochyena.

## Artifacts and Notes

Current verified facts before implementation:

    Lua bridge frame control:
      tools/mgba/mgba_lua_bridge.lua handles `run_frames` and responds after `currentFrame() >= targetFrame`.

    Current RNG seed path:
      src/main.c::SeedRngAndSetTrainerId reads REG_TM2CNT_L and REG_TM1CNT_L, then calls SeedRng(val).

    Route 101 Poochyena availability:
      src/data/wild_encounters.json lists SPECIES_POOCHYENA in MAP_ROUTE101 land encounter slots.

    Evolution and ability target:
      Poochyena evolves at level 18 into SPECIES_MIGHTYENA.
      Mightyena ability slot 0 is ABILITY_INTIMIDATE.

Potential route tree checkpoint ids:

    CleanBoot.MainMenu
    EarlyGame.TruckReady
    EarlyGame.StarterChoose
    EarlyGame.StarterConfirm
    Story.InitialBattleWon
    Story.StarterReceived
    Story.RivalBattleWon
    Story.PokeballsObtained
    Route101.GrassReady
    Battle.WildPoochyenaSlot0
    Party.PoochyenaCaughtSlot0
    Party.MightyenaIntimidate

## Interfaces and Dependencies

Add `AUTOMATION_PROBE ?= 0` to `Makefile` and pass `-DAUTOMATION_PROBE=$(AUTOMATION_PROBE)` in `CPPFLAGS`. Keep `AUTOMATION_BEACON` unchanged.

Add `include/automation_probe.h`. The header should own the ABI constants, command ids, `struct AutomationProbe`, `extern struct AutomationProbe gAutomationProbe`, and no-op inline functions when the flag is off. The struct should use only `u32` fields for the first version.

Add `src/automation_probe.c`. The implementation should own `gAutomationProbe`, `AutomationProbe_Update`, helper functions that fill party slots with `GetMonData` and `GetMonAbility`, and `AutomationProbe_ConsumeCommand` if acceleration commands are needed. The global should be zero-initialized or placed in an existing repository-supported RAM section so it does not trigger the `.data` linker issue previously observed in automation work.

Update `src/main.c` to call `AutomationProbe_Update()` from the main loop outside VBlank. The call should be cheap enough for automation builds and compiled away when `AUTOMATION_PROBE=0`.

Extend `tools/mgba/mgba_lua_bridge.lua` with generic memory read support. The preferred first command is `read_u32_array <address> <count>`, returning JSON with `ok`, `type`, `address`, `count`, and `values`. A write command should only be added if the Rare Candy acceleration command is implemented.

Extend `tools/mgba/mgba_lua_spike.py` with `--sym`, symbol parsing for `pokeemerald.sym`, probe decoding, `--mode probe-smoke`, `--mode poochyena-intimidate`, and an optional `--objective-timeout`. Keep existing modes working.

If adding a route module, use `tools/mgba/mgba_route_tree.py`. It should depend only on the Python standard library and the existing bridge client. The route tree should write route trace events into `events.ndjson` so failures show the last accepted node and the failed edge.

Default builds must not expose automation commands or route probes. The default `make NO_MULTIBOOT=1` build must pass after this plan.

Revision Note (2026-04-29): Initial ExecPlan. It captures the decision to stop growing the visual beacon, introduces a Lua-readable memory probe and lightweight route tree, and uses the Route 101 Poochyena-to-Mightyena Intimidate objective as the first end-to-end gameplay proof.
