# mGBA Lua Automation Spike Notes

Date: 2026-04-27

## Purpose

This spike tests whether early-game automation can move from desktop-focused AutoHotkey control to an emulator-owned interface: Python host orchestration over localhost to a Lua script running inside mGBA.

## Local Capability Result

- Windows mGBA found: `/mnt/c/Program Files/mGBA/mgba-sdl.exe` reports `mgba-sdl.exe 0.10.4 (6c2da7b62163fcd6446fc06b9b993fd414a05eaa)`.
- The installed Windows `mGBA.exe` and `mgba-sdl.exe` CLI help do not advertise `--script`.
- The Nix `mgba` package checked via `nix-shell -p mgba` provides `mgba 0.10.5`, but its CLI help also does not advertise `--script` and it does not expose `mgba-headless` in PATH.
- Upstream release tag `0.10.5` does not contain the headless frontend. Current upstream commit `b19b557a78930ede7ee7f5dcbc880f9ff2533ffe` does.
- `tools/mgba/build_mgba_headless.sh` builds a pinned `mgba-headless` with `BUILD_HEADLESS=ON`, `ENABLE_SCRIPTING=ON`, and `USE_LUA=ON` under `build/mgba-headless-build/`.
- WSL has no direct `python3` on PATH, but `nix-shell -p python3 --run 'python3 --version'` works. Windows Python is also available through `/mnt/c/Windows/py.exe -3`.

## Spike Interface

- `tools/mgba/mgba_lua_bridge.lua` is the emulator-side bridge.
- `tools/mgba/mgba_lua_spike.py` is the host-side runner and capability reporter.
- Build the pinned script-capable headless mGBA if needed:

```sh
nix-shell -p git cmake gcc gnumake pkg-config lua libpng zlib libzip sqlite ffmpeg --run tools/mgba/build_mgba_headless.sh
```

- Default capability check:

```sh
nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode capability --require-headless'
```

- No-focus main-menu smoke test:

```sh
nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode main-menu --require-headless'
```

- No-focus female `A` truck checkpoint:

```sh
nix-shell -p python3 --run 'python3 tools/mgba/mgba_lua_spike.py --mode truck --require-headless --connect-timeout 20 --truck-timeout 360'
```

## Passing Proof

The no-focus smoke test passed against `build/mgba-master-build/mgba-headless` after building current upstream with headless scripting enabled. The Python host connected to the Lua bridge, sent key taps through `emu:setKeys`, waited for emulator frames through bridge callbacks, read the beacon from OBJ VRAM, and stopped at `MAIN_MENU_READY`.

Artifacts from the passing run:

- `build/mgba_lua_spike/capability_report.json`
- `build/mgba_lua_spike/events.ndjson`
- `build/mgba_lua_spike/run_result.json`

The passing final beacon tuple was `stageId=1`, `inputReady=1`, `protocol=1`, and `source=vram`.

The no-focus truck checkpoint also passed under the same headless bridge. Three clean runs wrote `build/mgba_lua_spike_truck_1/run_result.json`, `build/mgba_lua_spike_truck_2/run_result.json`, and `build/mgba_lua_spike_truck_3/run_result.json`; each ended at `TRUCK_CONTROL_READY` with `stageId=9`, `gender=2`, `nameLen=1`, `nameChar0=1`, `mapKind=1`, and `inputReady=1`.

Two route-specific timing hazards were captured and fixed in the host FSM:

- Birch's boy/girl text auto-opens the gender menu; holding `A` into that transition can select male before the route handles the prompt.
- Entering `A` can auto-swap the naming keyboard page; the route must wait for naming `inputReady=1` before sending `START` for OK.

## Preserved v2 Beacon Fields

A durable typed harness should add or expose these semantic fields before replacing the spike with Scala or another governed implementation:

- `mapLocalKind`
- `playerX`
- `playerY`
- `facing`
- `movementReady`
- `textReady`
- `menuReady`
- `interactReady`
- `scriptWaitKind`
- `interactableAhead`
- `routeErrorCode`
