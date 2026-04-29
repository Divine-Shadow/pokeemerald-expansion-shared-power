# Incremental Beacon v2 and Governed Harness Plan

This ExecPlan is a living document. The sections `Progress`, `Surprises & Discoveries`, `Decision Log`, and `Outcomes & Retrospective` must be kept up to date as work proceeds.

This plan must be maintained according to `.agent/PLANS.md` in this repository. A contributor implementing this plan must proceed milestone-to-milestone until either the incremental beacon v2 harness reaches its acceptance target or a blocker is documented with enough evidence to sync with the operator.

## Purpose / Big Picture

The current automation beacon and Python/Lua headless mGBA spike prove that a clean ROM boot can reach starter selection without taking desktop focus. The next goal is to make that proof maintainable as a generalized debugging tool: a governed agent should be able to build a beacon-enabled ROM, copy it into the Ouroboros devkit container, run an emulator-owned route, and diagnose progress using semantic beacon fields rather than fragile route-specific timing.

After this plan succeeds, the project will have a backward-compatible beacon extension that exposes explicit readiness and interaction facts such as movement readiness, text readiness, menu readiness, script wait kind, and interactable-ahead classification. The Python/Lua proof will consume those fields, and the latest Scala harness in the Ouroboros devkit can be used as an optional typed validation lane from `devkit-ouro8-dev-agent-8`. The observable outcome is a clean headless run that reaches a selected milestone using the new semantic fields, with JSON artifacts showing both the old v1 tuple and the new v2 semantic tuple at each accepted route state.

## Progress

- [x] (2026-04-28) Authored this ExecPlan after reading `.agent/PLANS.md`, the existing automation beacon plans, the current C beacon implementation, the Lua bridge decoder, the Python route runner, and the seeded Ouroboros devkit state.
- [ ] Milestone 1: Rebaseline the current v1 starter-selection proof from a fresh beacon ROM and copy the exact ROM and scripts into `devkit-ouro8-dev-agent-8`.
- [ ] Milestone 2: Specify and document the v2 extension rows while preserving the existing v1 row contract.
- [ ] Milestone 3: Implement the smallest v2 beacon producer and decoder slice for readiness semantics, then validate it at the already-passing starter-selection route.
- [ ] Milestone 4: Replace at least one route wait in the Python host with a v2 semantic gate and prove the route still reaches starter selection.
- [ ] Milestone 5: Exercise the latest Scala harness in agent 8 as a read-only or adapter-level validation lane, without requiring a full migration before the beacon protocol is stable.
- [ ] Milestone 6: Choose the next point-of-interest exit condition, implement only the beacon fields required for that point, and run repeated clean headless proofs.

## Surprises & Discoveries

- Observation: The existing beacon already has a v1 header row, proof row, navigation row, and map-extension row encoded in one 8x8 OBJ tile.
  Evidence: `src/automation_beacon.c` writes rows 0 through 3, and `tools/mgba/mgba_lua_bridge.lua` decodes those rows from OBJ VRAM at tile 1023.

- Observation: The current proof is strong enough for starter selection, but the host route is now too large to be the long-term place for domain invariants.
  Evidence: `tools/mgba/mgba_lua_spike.py` contains the whole clean-boot route, while `tools/mgba/mgba_lua_spike_notes.md` explicitly lists preserved v2 semantic fields for a durable typed harness.

- Observation: Agent 8 is available for validation and already has the required emulator tools and seeded proof directory, but the mounted Ouroboros worktree has unrelated dirty Scala files.
  Evidence: `docker exec devkit-ouro8-dev-agent-8 ...` found `mgba-headless`, `lua`, `python3`, and `.devkit/mgba-poc/pokeemerald.gba`; `git status` inside the container showed dirty `tools/submit-to-ci` Scala files unrelated to the emulator proof.

## Decision Log

- Decision: Keep beacon v2 backward-compatible by leaving rows 0 through 3 unchanged and adding semantic extension rows instead of reassigning existing v1 fields.
  Rationale: The current PowerShell reader, Lua bridge, Python route, and documented starter-selection proof all depend on the v1 rows. Backward compatibility lets each consumer migrate incrementally.
  Date/Author: 2026-04-28 / Codex

- Decision: Treat Lua as the emulator IO and decode bridge, Python as the temporary route host, and Scala as an optional validation or migration lane until the beacon protocol stabilizes.
  Rationale: The current Python/Lua path already proves emulator control. Moving protocol semantics into Scala before the fields are stable would risk designing typed wrappers around unstable facts.
  Date/Author: 2026-04-28 / Codex

- Decision: Use `devkit-ouro8-dev-agent-8` for container validation when Scala or governed-harness checks are needed.
  Rationale: The operator explicitly offered agent 8, and it already has `mgba-headless`, Lua, Python, and the `.devkit/mgba-poc/` seed path.
  Date/Author: 2026-04-28 / Codex

- Decision: Make starter selection the baseline acceptance for the first v2 readiness fields, then select a later point of interest only after one semantic wait replaces a v1/timing gate successfully.
  Rationale: Starter selection is already deterministic, so it isolates beacon protocol risk from route-discovery risk.
  Date/Author: 2026-04-28 / Codex

## Outcomes & Retrospective

This section is intentionally sparse at plan creation. At each major milestone, update it with the exact result, what changed, what remains, and whether the plan should continue in Python/Lua or sync with the operator about Scala migration.

Initial outcome: the plan now defines an ordered path from the current v1 proof to semantic v2 beacon fields and optional Scala validation in the Ouroboros devkit.

## Context and Orientation

This repository is `/home/bayesartre/dev/pokeemerald-expansion-shared-power`. It builds a Game Boy Advance ROM named `pokeemerald.gba`. The automation build uses `AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1` so the ROM draws a debug-only machine-readable sprite called the automation beacon. A beacon is an 8x8 4bpp object sprite whose pixel palette indices encode small integers. The Lua bridge reads the beacon directly from OBJ VRAM inside mGBA, so the route does not need Windows screenshots or desktop focus.

The current beacon producer is `src/automation_beacon.c` with declarations in `include/automation_beacon.h`. The current consumers are `tools/mgba/mgba_lua_bridge.lua`, which exposes `read_beacon` over a localhost socket, and `tools/mgba/mgba_lua_spike.py`, which launches `mgba-headless`, sends key inputs, waits for emulator frames, and writes JSON artifacts. `tools/mgba/mgba_read_beacon.ps1` and `tools/mgba/run_beacon_repro.ahk` are the Windows GUI reader and runner; they should remain compatible but are not the primary path for this plan.

The current v1 rows are:

- Row 0 is the header: anchor values `14,13`, protocol version, stage id, substage id, flags, pulse, and checksum.
- Row 1 is route proof: gender, name length, first name character, map kind, starter selection, input-ready flag, error code, and reserved.
- Row 2 is navigation proof: anchor values `12,11`, low player coordinates, facing, low front-tile coordinates, and checksum.
- Row 3 is map extension proof: anchor values `10,9`, map slot, high coordinate nibbles, and checksum.

The current important stage ids are `1` for main menu ready, `3` for gender prompt ready, `5` for naming screen ready, `9` for truck control ready, `10` for Littleroot setup, `11` for Route 101 approach, `12` for starter choose ready, and `13` for starter confirmation prompt. The current starter-selection proof accepts `stageId=12`, `mapKind=4`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=1`.

The sibling Ouroboros repository is `/home/bayesartre/dev/ouroboros-ide`. Its devkit stack mounts the repository into containers under `/workspaces/dev/ouroboros-ide`. Agent 8 is the container named `devkit-ouro8-dev-agent-8`. It can run `mgba-headless`, Lua, and Python against files under `.devkit/mgba-poc/`. If the Scala harness is tested, run it in that container so the host NixOS environment does not need to support the Ouroboros build.

## Plan of Work

Milestone 1 revalidates the known baseline before changing beacon semantics. Build a fresh automation ROM from this repository, run Python and Lua syntax checks, and run the existing `starter` mode once. Copy the fresh `pokeemerald.gba`, `tools/mgba/mgba_lua_spike.py`, and `tools/mgba/mgba_lua_bridge.lua` into `/home/bayesartre/dev/ouroboros-ide/.devkit/mgba-poc/`, then run the same proof inside `devkit-ouro8-dev-agent-8`. This milestone is complete when both local or repository-context proof and container proof write `run_result.json` ending at `STARTER_CHOOSE_READY` with the accepted v1 tuple. If the local host cannot run the proof but agent 8 can, record that and continue using agent 8 as the validation surface.

Milestone 2 defines the v2 semantic extension rows. Add a short section to `tools/mgba/mgba_lua_spike_notes.md` and this ExecPlan describing each new field in plain language, its numeric range, and which game fact produces it. Keep rows 0 through 3 unchanged. Prefer row 4 for readiness facts and row 5 for interaction facts. A good first row 4 shape is anchor `8,7`, protocol extension version, `movementReady`, `textReady`, `menuReady`, `interactReady`, and checksum. A good first row 5 shape is anchor `6,5`, `scriptWaitKind`, `interactableAhead`, `routeErrorCode`, two reserved fields, and checksum. Do not implement every aspirational field if there is no producer yet; a field may be reserved with value `0` until a milestone needs it. This milestone is complete when the field contract is written clearly enough that a reader can update both the C producer and Lua decoder without guessing.

Milestone 3 implements the smallest producer and decoder slice. Extend `include/automation_beacon.h` with one or two setter functions for semantic readiness, such as `AutomationBeacon_SetReadiness(...)` and `AutomationBeacon_SetInteractionProof(...)`, and no-op inline versions when `AUTOMATION_BEACON` is disabled. Extend `src/automation_beacon.c` so `WriteBeaconTile` writes rows 4 and 5 in the same checksum style as rows 2 and 3. Extend `tools/mgba/mgba_lua_bridge.lua` so `read_beacon` returns `semanticFound`, `movementReady`, `textReady`, `menuReady`, `interactReady`, `scriptWaitKind`, `interactableAhead`, and `routeErrorCode`. Keep all existing JSON keys unchanged. This milestone is complete when `docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"` succeeds and a starter run still reaches `STARTER_CHOOSE_READY` while the final beacon JSON includes `semanticFound=true`.

Milestone 4 replaces one fragile host wait with a v2 semantic gate. Pick a route wait that currently infers readiness from v1 `inputReady` plus route-specific flags. The preferred first candidate is a dialogue or script wait around Littleroot or Route 101 because the previous failures were caused by ambiguity between waiting, pressing `A`, and moving. Update `tools/mgba/mgba_lua_spike.py` so that one helper checks the new `textReady`, `movementReady`, or `scriptWaitKind` field before sending input. Do not rewrite the route wholesale. This milestone is complete when three clean headless starter runs pass and their `events.ndjson` files show at least one accepted transition using the new semantic field.

Milestone 5 tries the latest Scala harness without making it the blocking implementation path. In `devkit-ouro8-dev-agent-8`, refresh the mounted Ouroboros worktree only if it is safe to do so; if it has unrelated dirty files, record them and do not overwrite them. Copy the fresh ROM and scripts into `.devkit/mgba-poc/` and run the available Scala build or harness command if discoverable from repository docs or scripts. The goal is to answer whether Scala can parse the same beacon JSON and dispatch an emulator interaction through the Lua bridge. If the Scala build is blocked by unrelated dirty files, governance lints, or missing command documentation, record the exact blocker and continue with Python/Lua for beacon protocol validation. This milestone is complete when there is either a passing Scala smoke result or a documented blocker with enough command output for the operator to decide whether to clean or migrate.

Milestone 6 chooses and proves the next point of interest. The default next target is starter confirmation prompt, `stageId=13`, because it is one screen beyond the current proof and does not require general overworld navigation. If starter confirmation is too shallow to exercise the new fields, choose the first post-starter state that requires a battle/menu/text boundary, but document why. Add only the beacon fields needed for that target. Run at least three clean headless attempts from boot and record final tuples, output directories, and any failures. This milestone is complete when repeated runs reach the selected target using the v2 semantic fields, or when the plan records a migration blocker showing that route control has become too complex for the Python host.

## Concrete Steps

From `/home/bayesartre/dev/pokeemerald-expansion-shared-power`, inspect current state and avoid losing existing dirty work:

    git status --short --branch
    git diff --stat

Build the automation ROM. Prefer Docker because this repository documents it as the reproducible agent path:

    docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder make AUTOMATION_BEACON=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)"

Run syntax checks for the host proof files:

    python3 -m py_compile tools/mgba/mgba_lua_spike.py
    lua -v
    luac -p tools/mgba/mgba_lua_bridge.lua

Run the current starter proof from this repository if `mgba-headless` is available locally:

    python3 tools/mgba/mgba_lua_spike.py --mode starter --require-headless --rom pokeemerald.gba --bridge tools/mgba/mgba_lua_bridge.lua --output-dir build/mgba_lua_spike_v2_baseline --connect-timeout 20 --starter-timeout 600

Seed agent 8. This path is ignored in the Ouroboros repository and can be deleted and recreated:

    mkdir -p /home/bayesartre/dev/ouroboros-ide/.devkit/mgba-poc
    cp pokeemerald.gba /home/bayesartre/dev/ouroboros-ide/.devkit/mgba-poc/pokeemerald.gba
    cp tools/mgba/mgba_lua_spike.py /home/bayesartre/dev/ouroboros-ide/.devkit/mgba-poc/mgba_lua_spike.py
    cp tools/mgba/mgba_lua_bridge.lua /home/bayesartre/dev/ouroboros-ide/.devkit/mgba-poc/mgba_lua_bridge.lua

Run the proof inside agent 8:

    docker exec devkit-ouro8-dev-agent-8 bash -lc 'cd /workspaces/dev/ouroboros-ide && rm -rf .devkit/mgba-poc/runs/v2-baseline && python3 .devkit/mgba-poc/mgba_lua_spike.py --mode starter --require-headless --mgba /usr/local/bin/mgba-headless --rom .devkit/mgba-poc/pokeemerald.gba --bridge .devkit/mgba-poc/mgba_lua_bridge.lua --output-dir .devkit/mgba-poc/runs/v2-baseline --connect-timeout 20 --starter-timeout 600'

After implementing v2 rows, inspect the final result JSON. A successful final beacon should still include the v1 proof and should additionally include semantic fields:

    python3 - <<'PY'
    import json
    from pathlib import Path
    result = json.loads(Path('build/mgba_lua_spike_v2_baseline/run_result.json').read_text())
    beacon = result['beacon']
    print(result['ok'], result['target'])
    print(beacon['stageId'], beacon['mapKind'], beacon['gender'], beacon['nameLen'], beacon['nameChar0'], beacon['inputReady'])
    print(beacon.get('semanticFound'), beacon.get('movementReady'), beacon.get('textReady'), beacon.get('menuReady'), beacon.get('interactReady'))
    PY

When trying Scala in agent 8, first discover the current command rather than guessing:

    docker exec devkit-ouro8-dev-agent-8 bash -lc 'cd /workspaces/dev/ouroboros-ide && git status --short --branch && find . -maxdepth 4 -type f \( -name "*.scala" -o -name "*.sbt" -o -name "README.md" \) | grep -Ei "mgba|emulator|beacon|automation|devkit" | sort | sed -n "1,120p"'

If a Scala command is documented, run it in agent 8 and record exact output in this plan. If no command is documented, record that as a discovery and do not invent a long-lived Scala API in this repository.

## Validation and Acceptance

The plan is accepted only when the implementation demonstrates working behavior, not just compilation.

Milestone 1 acceptance requires a fresh run ending at `STARTER_CHOOSE_READY` with `stageId=12`, `mapKind=4`, `gender=2`, `nameLen=1`, `nameChar0=1`, and `inputReady=1`, plus the same proof inside `devkit-ouro8-dev-agent-8` or a documented container-specific blocker.

Milestone 2 acceptance requires written protocol documentation for every v2 field that will be emitted or decoded in Milestone 3. The documentation must state how unknown or unavailable values are encoded.

Milestone 3 acceptance requires the automation ROM to build, the default beacon-disabled ROM to build, Lua syntax validation to pass, Python syntax validation to pass, and a headless starter run to produce `semanticFound=true` without regressing the v1 tuple.

Milestone 4 acceptance requires at least one route transition to depend on a v2 semantic field and three clean headless starter runs to pass. The event logs must identify the semantic gate by phase or event name so a reviewer can see that the new field was used.

Milestone 5 acceptance requires either a Scala smoke pass inside agent 8 or a blocker entry naming the exact command attempted, dirty files or build failures observed, and whether the blocker affects beacon protocol work.

Milestone 6 acceptance requires three clean headless runs to the selected post-starter-selection point of interest, using separate output directories and recording the final v1 and v2 tuples in this ExecPlan.

## Idempotence and Recovery

The `.devkit/mgba-poc/` directory in the Ouroboros repository is a generated local seed. It is safe to delete and recreate, and it must remain git-ignored. Do not commit ROM files, `__pycache__`, or mGBA run outputs.

The beacon protocol must remain backward-compatible during this plan. If a v2 decoder change breaks old v1 consumers, revert the decoder change and restore rows 0 through 3 before continuing. If an automation run times out, preserve its output directory and record the last beacon tuple from `run_result.json` or `events.ndjson` before retrying.

The current Pokeemerald worktree contains active automation changes. If cleanup is needed, commit them in coherent local commits rather than deleting them. Do not use `git reset --hard` or `git checkout --` to discard work unless the operator explicitly requests it.

Agent 8 may contain unrelated dirty Ouroboros files. Do not rebase, pull, overwrite, or clean that worktree unless the operator explicitly approves or the dirty files are first committed by the responsible owner. The Scala lane is optional until the beacon protocol is stable.

## Artifacts and Notes

Initial local state observed while writing this plan:

    /home/bayesartre/dev/pokeemerald-expansion-shared-power was on main ahead of origin/main by one commit with active automation changes in Makefile, PATCH_NOTES.md, src beacon call sites, tools/mgba scripts, and .agent plans.
    /home/bayesartre/dev/ouroboros-ide was behind origin/main by one commit from the host view.
    devkit-ouro8-dev-agent-8 was running and had /usr/local/bin/mgba-headless, /usr/local/bin/lua, /usr/bin/python3, and .devkit/mgba-poc/pokeemerald.gba.
    Inside agent 8, the mounted Ouroboros worktree had unrelated dirty Scala files under tools/submit-to-ci.

When Milestone 1 starts, replace this note with exact command output and result paths.

## Interfaces and Dependencies

The C interface must remain simple and no-op when `AUTOMATION_BEACON` is `0`. New declarations belong in `include/automation_beacon.h`; implementations belong in `src/automation_beacon.c`. A suitable interface shape is:

    void AutomationBeacon_SetReadiness(bool8 movementReady, bool8 textReady, bool8 menuReady, bool8 interactReady);
    void AutomationBeacon_SetInteractionProof(u8 scriptWaitKind, u8 interactableAhead, u8 routeErrorCode);

The exact names may change if the implementation finds better repository-local terminology, but the interface must keep the same intent: callers should set semantic facts without knowing how OBJ tile rows are encoded.

The Lua bridge must preserve existing `read_beacon` JSON keys. New keys should be additive. The Python host should tolerate missing semantic keys while Milestone 3 is in progress, but after Milestone 4 the selected semantic gate should fail clearly if `semanticFound` is false.

The Scala lane must consume the same JSON contract as Python. It should not read emulator memory directly during this plan; direct memory reads remain isolated in Lua so the host language can change without rewriting mGBA IO.

Revision Note (2026-04-28): Initial ExecPlan. It defines an incremental, backward-compatible path from the current v1 starter-selection proof to semantic v2 beacon fields, Python/Lua validation, and optional Scala harness checks inside `devkit-ouro8-dev-agent-8`.
