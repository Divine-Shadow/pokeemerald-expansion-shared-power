---
name: pokeemerald-scala-emulator-runner
description: Build this pokeemerald repo's automation ROM, hand its ROM/symbol/Lua bridge artifacts to the Ouroboros Scala mGBA runner, and run Scala-side emulator proof targets inside the agent-5 Docker container.
---

# Pokeemerald Scala Emulator Runner

Use this skill when a task needs the Scala/Ouroboros emulator runner to execute against the current pokeemerald workspace, especially for save-state generation, route proofs, or automation probe validation.

## Repositories and ownership

- Pokeemerald source repo: `/home/bayesartre/dev/pokeemerald-expansion-shared-power`
- Ouroboros Scala runner repo: `/home/bayesartre/dev/ouroboros-ide-mightyena-scala`
- Ouro container for Scala commands: `devkit-ouro8-dev-agent-5`
- Ouro in-container workdir: `/workspaces/dev/ouroboros-ide-mightyena-scala`
- Ignored handoff directory in Ouro: `.devkit/mgba-poc/`

Run pokeemerald builds from the pokeemerald repo, normally through the `pokeemerald-expansion:builder` Docker image. Run Ouro/Scala build and route commands inside `devkit-ouro8-dev-agent-5`; do not run host `sbt` directly.

## Build and handoff workflow

From the pokeemerald repo:

```bash
docker run --rm -u "$(id -u):$(id -g)" -v "$PWD:/workspace" -w /workspace pokeemerald-expansion:builder \
  make AUTOMATION_BEACON=1 AUTOMATION_PROBE=1 DEBUG=1 NO_MULTIBOOT=1 -j"$(nproc)" rom syms

mkdir -p /home/bayesartre/dev/ouroboros-ide-mightyena-scala/.devkit/mgba-poc
cp pokeemerald.gba pokeemerald.sym tools/mgba/mgba_lua_bridge.lua \
  /home/bayesartre/dev/ouroboros-ide-mightyena-scala/.devkit/mgba-poc/
```

Use `rom syms` together. `make syms` alone can leave the ROM and symbols out of sync.

## Scala runner targets

The Scala entry point is:

```text
com.crib.bills.ouroboros.environments.local.emulator.mgba.ProofMain
```

Common `OUROBOROS_EMULATOR_TARGET` values:

- `STARTER_CHOOSE_READY`
- `PC_RARE_CANDY_SMOKE`
- `POOCHYENA_EVOLUTION_SMOKE`
- `PARTY_ORDER_SMOKE`
- `SECOND_CAPTURE_PARTY_ORDER_SMOKE`
- `FIRST_GYM_REACH_SMOKE`
- `ROXANNE_READY_SAVE_STATE`

Use `ROXANNE_READY_SAVE_STATE` when the user asks for a pre-gym Mightyena/Intimidate state or a save-state before Roxanne.

## Run a Scala proof

Template:

```bash
docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/ouroboros-ide-mightyena-scala && rm -rf build/<output-dir> && OUROBOROS_EMULATOR_TARGET=<TARGET> OUROBOROS_EMULATOR_RUNS=1 OUROBOROS_EMULATOR_OUTPUT=build/<output-dir> OUROBOROS_EMULATOR_ROM=.devkit/mgba-poc/pokeemerald.gba OUROBOROS_EMULATOR_LUA=.devkit/mgba-poc/mgba_lua_bridge.lua OUROBOROS_EMULATOR_SYM=.devkit/mgba-poc/pokeemerald.sym OUROBOROS_EMULATOR_PORT=<unused-port> OUROBOROS_EMULATOR_PROOF_TIMEOUT_SECONDS=<seconds> scripts/subagent/sbt-governed --summary-lines 220 --failure-lines 600 -- ";project local-environments;Test / runMain com.crib.bills.ouroboros.environments.local.emulator.mgba.ProofMain"'
```

For the Roxanne-ready save state:

```bash
docker exec devkit-ouro8-dev-agent-5 bash -lc 'cd /workspaces/dev/ouroboros-ide-mightyena-scala && rm -rf build/mgba-scala-proof-roxanne-ready && OUROBOROS_EMULATOR_TARGET=ROXANNE_READY_SAVE_STATE OUROBOROS_EMULATOR_RUNS=1 OUROBOROS_EMULATOR_OUTPUT=build/mgba-scala-proof-roxanne-ready OUROBOROS_EMULATOR_ROM=.devkit/mgba-poc/pokeemerald.gba OUROBOROS_EMULATOR_LUA=.devkit/mgba-poc/mgba_lua_bridge.lua OUROBOROS_EMULATOR_SYM=.devkit/mgba-poc/pokeemerald.sym OUROBOROS_EMULATOR_PORT=46680 OUROBOROS_EMULATOR_PROOF_TIMEOUT_SECONDS=3600 scripts/subagent/sbt-governed --summary-lines 220 --failure-lines 600 -- ";project local-environments;Test / runMain com.crib.bills.ouroboros.environments.local.emulator.mgba.ProofMain"'
```

Expected artifacts are under the configured output directory. For `ROXANNE_READY_SAVE_STATE`, report:

- `build/<output-dir>/run-1/run_result.json`
- `build/<output-dir>/run-1/events.ndjson`
- `build/<output-dir>/run-1/roxanne_ready.ss0`

## Validation and hygiene

- Check `run_result.json` for `"ok": true` and the expected target.
- Verify party/probe fields when the user asks for a specific gameplay state.
- Keep generated ROMs, symbols, logs, screenshots, and savestates uncommitted.
- If this repo is changed by adding or updating the skill, add a top entry to `PATCH_NOTES.md`.
