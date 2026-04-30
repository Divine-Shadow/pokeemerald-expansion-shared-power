#!/usr/bin/env python3
"""Python host spike for mGBA Lua automation.

The script deliberately avoids GUI automation. It either proves that a local mGBA
binary can be driven through a Lua bridge, or writes a precise capability report
explaining why the spike is blocked.
"""

from __future__ import annotations

import argparse
import json
import os
import shlex
import shutil
import socket
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Iterable

DEFAULT_PORT = 46510
SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent.parent
DEFAULT_OUTPUT_DIR = REPO_ROOT / "build" / "mgba_lua_spike"
DEFAULT_BRIDGE = SCRIPT_DIR / "mgba_lua_bridge.lua"
DEFAULT_ROM = REPO_ROOT / "pokeemerald.gba"
DEFAULT_SYM = REPO_ROOT / "pokeemerald.sym"
DEFAULT_STATE_SCREENSHOT_SOURCE = SCRIPT_DIR / "mgba_state_screenshot.c"

STAGE_MAIN_MENU_READY = 1
STAGE_BIRCH_INTRO_TEXT = 2
STAGE_GENDER_PROMPT_READY = 3
STAGE_GENDER_CONFIRMED = 4
STAGE_NAMING_SCREEN_READY = 5
STAGE_NAME_CONFIRMED = 6
STAGE_TRUCK_CONTROL_READY = 9
STAGE_LITTLEROOT_ROUTE_SETUP = 10
STAGE_ROUTE101_APPROACH = 11
STAGE_STARTER_CHOOSE_READY = 12
STAGE_STARTER_CONFIRM_PROMPT = 13
STAGE_BATTLE_SUMMARY_REPRO = 14

REPRO_SUBSTAGE_ACTION_MENU = 1
REPRO_SUBSTAGE_PARTY_MENU = 2
REPRO_SUBSTAGE_PARTY_MON_MENU = 3
REPRO_SUBSTAGE_SUMMARY_REQUESTED = 4
REPRO_SUBSTAGE_SUMMARY_SCREEN = 5
REPRO_SUBSTAGE_SHOW_SUMMARY_CALLBACK = 6
REPRO_SUBSTAGE_SHOW_SUMMARY_ENTERED = 7
REPRO_SUBSTAGE_INIT_SUMMARY_SCREEN = 8
REPRO_SUBSTAGE_LOAD_GRAPHICS_STATE = 9
REPRO_SUBSTAGE_LOAD_MON_GFX_BEGIN = 10
REPRO_SUBSTAGE_LOAD_MON_GFX_WAITING = 11
REPRO_SUBSTAGE_LOAD_MON_GFX_DONE = 12
REPRO_SUBSTAGE_LOAD_MON_GFX_CREATE = 13
REPRO_SUBSTAGE_SHOW_SUMMARY_ALLOC_FAILED = 14

REPRO_SUBSTAGE_NAMES = {
    REPRO_SUBSTAGE_ACTION_MENU: "ACTION_MENU",
    REPRO_SUBSTAGE_PARTY_MENU: "PARTY_MENU",
    REPRO_SUBSTAGE_PARTY_MON_MENU: "PARTY_MON_MENU",
    REPRO_SUBSTAGE_SUMMARY_REQUESTED: "SUMMARY_REQUESTED",
    REPRO_SUBSTAGE_SUMMARY_SCREEN: "SUMMARY_SCREEN",
    REPRO_SUBSTAGE_SHOW_SUMMARY_CALLBACK: "SHOW_SUMMARY_CALLBACK",
    REPRO_SUBSTAGE_SHOW_SUMMARY_ENTERED: "SHOW_SUMMARY_ENTERED",
    REPRO_SUBSTAGE_INIT_SUMMARY_SCREEN: "INIT_SUMMARY_SCREEN",
    REPRO_SUBSTAGE_LOAD_GRAPHICS_STATE: "LOAD_GRAPHICS_STATE",
    REPRO_SUBSTAGE_LOAD_MON_GFX_BEGIN: "LOAD_MON_GFX_BEGIN",
    REPRO_SUBSTAGE_LOAD_MON_GFX_WAITING: "LOAD_MON_GFX_WAITING",
    REPRO_SUBSTAGE_LOAD_MON_GFX_DONE: "LOAD_MON_GFX_DONE",
    REPRO_SUBSTAGE_LOAD_MON_GFX_CREATE: "LOAD_MON_GFX_CREATE",
    REPRO_SUBSTAGE_SHOW_SUMMARY_ALLOC_FAILED: "SHOW_SUMMARY_ALLOC_FAILED",
}

REPRO_SUMMARY_ATTEMPT_SUBSTAGES = {
    REPRO_SUBSTAGE_SUMMARY_REQUESTED,
    REPRO_SUBSTAGE_SUMMARY_SCREEN,
    REPRO_SUBSTAGE_SHOW_SUMMARY_CALLBACK,
    REPRO_SUBSTAGE_SHOW_SUMMARY_ENTERED,
    REPRO_SUBSTAGE_INIT_SUMMARY_SCREEN,
    REPRO_SUBSTAGE_LOAD_GRAPHICS_STATE,
    REPRO_SUBSTAGE_LOAD_MON_GFX_BEGIN,
    REPRO_SUBSTAGE_LOAD_MON_GFX_WAITING,
    REPRO_SUBSTAGE_LOAD_MON_GFX_DONE,
    REPRO_SUBSTAGE_LOAD_MON_GFX_CREATE,
    REPRO_SUBSTAGE_SHOW_SUMMARY_ALLOC_FAILED,
}

GENDER_FEMALE = 2
MAP_TRUCK = 1
MAP_LITTLEROOT = 2
MAP_ROUTE101 = 3
MAP_STARTER_SELECTION = 4
MAP_SLOT_TRUCK = 1
MAP_SLOT_LITTLEROOT_TOWN = 2
MAP_SLOT_BRENDANS_HOUSE_1F = 3
MAP_SLOT_BRENDANS_HOUSE_2F = 4
MAP_SLOT_MAYS_HOUSE_1F = 5
MAP_SLOT_MAYS_HOUSE_2F = 6
MAP_SLOT_ROUTE101 = 7
NAME_CHAR_A = 1

SCRIPT_STEP_WAIT_MESSAGE = 5
SCRIPT_STEP_WAIT_BUTTON = 10

FACING_DOWN = 1
FACING_UP = 2
FACING_LEFT = 3
FACING_RIGHT = 4
KEY_FACING = {
    "UP": FACING_UP,
    "DOWN": FACING_DOWN,
    "LEFT": FACING_LEFT,
    "RIGHT": FACING_RIGHT,
}

SPECIES_POOCHYENA = 261
SPECIES_MIGHTYENA = 262
ABILITY_INTIMIDATE = 22
ITEM_POKE_BALL = 1
ITEM_RARE_CANDY = 102

AUTOMATION_PROBE_MAGIC = 0x41505242
AUTOMATION_PROBE_VERSION = 1
AUTOMATION_PROBE_COMMAND_RESULT_OK = 1
AUTOMATION_PROBE_COMMAND_GRANT_ITEM = 1
AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0 = 2
AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA = 3

PROBE_PARTY_SIZE = 6
PROBE_FIELD_LAYOUT: list[tuple[str, int]] = [
    ("magic", 1),
    ("version", 1),
    ("size", 1),
    ("sequence", 1),
    ("frame", 1),
    ("mainCallbackId", 1),
    ("routeStage", 1),
    ("routeSubstage", 1),
    ("readinessFlags", 1),
    ("mapGroup", 1),
    ("mapNum", 1),
    ("mapSlot", 1),
    ("playerX", 1),
    ("playerY", 1),
    ("playerFacing", 1),
    ("battleTypeFlags", 1),
    ("inBattle", 1),
    ("enemyPartyCount", 1),
    ("enemy0Species", 1),
    ("enemy0Level", 1),
    ("enemy0AbilityNum", 1),
    ("enemy0Ability", 1),
    ("playerPartyCount", 1),
    ("partySpecies", PROBE_PARTY_SIZE),
    ("partyLevel", PROBE_PARTY_SIZE),
    ("partyAbilityNum", PROBE_PARTY_SIZE),
    ("partyAbility", PROBE_PARTY_SIZE),
    ("partyHp", PROBE_PARTY_SIZE),
    ("partyMaxHp", PROBE_PARTY_SIZE),
    ("bagPokeBallCount", 1),
    ("bagRareCandyCount", 1),
    ("lastCommandId", 1),
    ("lastCommandArg0", 1),
    ("lastCommandArg1", 1),
    ("commandSequence", 1),
    ("commandId", 1),
    ("commandArg0", 1),
    ("commandArg1", 1),
    ("commandAckSequence", 1),
    ("commandResult", 1),
    ("commandResultArg0", 1),
    ("commandResultArg1", 1),
    ("objectiveFlags", 1),
    ("caughtPartySlot", 1),
]
PROBE_WORD_COUNT = sum(width for _, width in PROBE_FIELD_LAYOUT)


@dataclass
class RouteNode:
    id: str
    description: str


@dataclass
class MgbaCandidate:
    path: str
    exists: bool
    version: str | None
    help_text: str | None
    supports_script: bool
    is_headless: bool
    error: str | None = None


def run_capture(command: list[str], timeout: float = 5.0) -> tuple[int | None, str, str | None]:
    try:
        completed = subprocess.run(
            command,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=timeout,
            check=False,
        )
        return completed.returncode, completed.stdout or "", None
    except Exception as exc:  # noqa: BLE001 - report capability failures verbatim.
        return None, "", f"{type(exc).__name__}: {exc}"


def executable_exists(path: str) -> bool:
    if os.path.isabs(path) or os.sep in path or (os.altsep and os.altsep in path):
        return Path(path).exists()
    return shutil.which(path) is not None


def resolve_executable(path: str) -> str:
    if os.path.isabs(path) or os.sep in path or (os.altsep and os.altsep in path):
        return path
    return shutil.which(path) or path


def candidate_paths(explicit: str | None) -> list[str]:
    if explicit:
        return [explicit]

    env_path = os.environ.get("MGBA_EXE")
    paths: list[str] = []
    if env_path:
        paths.append(env_path)

    paths.extend([
        str(REPO_ROOT / "build" / "mgba-headless-build" / "mgba-headless"),
        str(REPO_ROOT / "build" / "mgba-master-build" / "mgba-headless"),
    ])

    paths.extend([
        "mgba-headless",
        "mgba-sdl",
        "mgba-qt",
        "mgba",
    ])

    if os.name == "nt":
        paths.extend([
            r"C:\Program Files\mGBA\mgba-headless.exe",
            r"C:\Program Files\mGBA\mgba-sdl.exe",
            r"C:\Program Files\mGBA\mGBA.exe",
        ])
    else:
        paths.extend([
            "/mnt/c/Program Files/mGBA/mgba-headless.exe",
            "/mnt/c/Program Files/mGBA/mgba-sdl.exe",
            "/mnt/c/Program Files/mGBA/mGBA.exe",
        ])

    unique: list[str] = []
    for path in paths:
        if path not in unique:
            unique.append(path)
    return unique


def inspect_candidate(path: str) -> MgbaCandidate:
    exists = executable_exists(path)
    resolved = resolve_executable(path)
    if not exists:
        return MgbaCandidate(
            path=path,
            exists=False,
            version=None,
            help_text=None,
            supports_script=False,
            is_headless="headless" in path.lower(),
            error="not_found",
        )

    _, version_output, version_error = run_capture([resolved, "--version"])
    _, help_output, help_error = run_capture([resolved, "--help"])
    combined_error = version_error or help_error
    help_text = help_output if help_output else None

    version = version_output.strip() or None
    if version and version.lower().startswith("usage:"):
        version = None

    return MgbaCandidate(
        path=resolved,
        exists=True,
        version=version,
        help_text=help_text,
        supports_script="--script" in help_output,
        is_headless="headless" in Path(resolved).name.lower(),
        error=combined_error,
    )


def inspect_mgba(explicit: str | None) -> tuple[MgbaCandidate | None, list[MgbaCandidate]]:
    candidates = [inspect_candidate(path) for path in candidate_paths(explicit)]
    usable = [candidate for candidate in candidates if candidate.exists]

    scripted = [candidate for candidate in usable if candidate.supports_script]
    if scripted:
        scripted.sort(key=lambda item: (not item.is_headless, item.path.lower()))
        return scripted[0], candidates

    if usable:
        usable.sort(key=lambda item: (not item.is_headless, item.path.lower()))
        return usable[0], candidates

    return None, candidates


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def parse_symbol_address(path: Path, symbol: str) -> int:
    if not path.exists():
        raise FileNotFoundError(f"symbol file not found: {path}")

    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        parts = line.split()
        if not parts or parts[-1] != symbol:
            continue

        for part in parts:
            token = part.strip()
            try:
                if token.startswith("0x"):
                    return int(token, 16)
                if len(token) >= 6 and all(char in "0123456789abcdefABCDEF" for char in token):
                    return int(token, 16)
            except ValueError:
                continue

    raise KeyError(f"symbol not found: {symbol}")


def probe_field_offsets() -> dict[str, int]:
    offsets: dict[str, int] = {}
    offset = 0
    for name, width in PROBE_FIELD_LAYOUT:
        offsets[name] = offset
        offset += width
    return offsets


PROBE_FIELD_OFFSETS = probe_field_offsets()


def decode_probe_words(words: list[int], address: int) -> dict[str, Any]:
    if len(words) < PROBE_WORD_COUNT:
        raise ValueError(f"probe word count too small: {len(words)} < {PROBE_WORD_COUNT}")

    decoded: dict[str, Any] = {"address": address, "wordCount": len(words)}
    index = 0
    for name, width in PROBE_FIELD_LAYOUT:
        if width == 1:
            decoded[name] = words[index]
        else:
            decoded[name] = words[index:index + width]
        index += width

    decoded["valid"] = (
        decoded.get("magic") == AUTOMATION_PROBE_MAGIC
        and decoded.get("version") == AUTOMATION_PROBE_VERSION
        and decoded.get("size") == PROBE_WORD_COUNT * 4
    )
    decoded["party"] = [
        {
            "slot": slot,
            "species": decoded["partySpecies"][slot],
            "level": decoded["partyLevel"][slot],
            "abilityNum": decoded["partyAbilityNum"][slot],
            "ability": decoded["partyAbility"][slot],
            "hp": decoded["partyHp"][slot],
            "maxHp": decoded["partyMaxHp"][slot],
        }
        for slot in range(PROBE_PARTY_SIZE)
    ]
    return decoded


def resolve_probe_address(args: argparse.Namespace) -> int:
    sym_path = Path(args.sym)
    try:
        return parse_symbol_address(sym_path, "gAutomationProbe")
    except (FileNotFoundError, KeyError):
        map_path = sym_path.with_suffix(".map")
        if map_path != sym_path:
            return parse_symbol_address(map_path, "gAutomationProbe")
        raise


def read_u32_array(client: BridgeClient, address: int, count: int) -> list[int]:
    response = client.request(f"read_u32_array {address} {count}")
    if response.get("ok") is not True:
        raise RuntimeError(f"read_u32_array failed: {response}")
    return [int(value) for value in response["values"]]


def write_u32(client: BridgeClient, address: int, value: int) -> None:
    response = client.request(f"write_u32 {address} {value}")
    if response.get("ok") is not True:
        raise RuntimeError(f"write_u32 failed: {response}")


def read_probe(client: BridgeClient, probe_address: int) -> dict[str, Any]:
    header = read_u32_array(client, probe_address, 3)
    size = header[2]
    count = PROBE_WORD_COUNT if size == 0 else max(PROBE_WORD_COUNT, size // 4)
    return decode_probe_words(read_u32_array(client, probe_address, count), probe_address)


def validate_probe(probe: dict[str, Any]) -> None:
    if probe.get("magic") != AUTOMATION_PROBE_MAGIC:
        raise RuntimeError(f"bad probe magic: {probe.get('magic')}")
    if probe.get("version") != AUTOMATION_PROBE_VERSION:
        raise RuntimeError(f"bad probe version: {probe.get('version')}")
    if probe.get("size") != PROBE_WORD_COUNT * 4:
        raise RuntimeError(f"bad probe size: {probe.get('size')} != {PROBE_WORD_COUNT * 4}")


def send_probe_command(
    client: BridgeClient,
    probe_address: int,
    command_id: int,
    arg0: int = 0,
    arg1: int = 0,
    timeout: float = 10.0,
) -> dict[str, Any]:
    probe = read_probe(client, probe_address)
    validate_probe(probe)
    sequence = int(probe["commandSequence"]) + 1

    write_u32(client, probe_address + (PROBE_FIELD_OFFSETS["commandArg0"] * 4), arg0)
    write_u32(client, probe_address + (PROBE_FIELD_OFFSETS["commandArg1"] * 4), arg1)
    write_u32(client, probe_address + (PROBE_FIELD_OFFSETS["commandId"] * 4), command_id)
    write_u32(client, probe_address + (PROBE_FIELD_OFFSETS["commandSequence"] * 4), sequence)

    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    while time.monotonic() < deadline:
        client.request("run_frames 1")
        last_probe = read_probe(client, probe_address)
        if last_probe.get("commandAckSequence") == sequence:
            return last_probe

    raise RuntimeError(f"probe command {command_id} timed out: {last_probe}")


def find_party_mon(probe: dict[str, Any], species: int, ability_num: int | None = None, ability: int | None = None) -> dict[str, Any] | None:
    for mon in probe["party"]:
        if mon["species"] != species:
            continue
        if ability_num is not None and mon["abilityNum"] != ability_num:
            continue
        if ability is not None and mon["ability"] != ability:
            continue
        return mon
    return None


def append_event(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    value = {"time": time.time(), **value}
    with path.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(value, sort_keys=True) + "\n")


def base_report(args: argparse.Namespace, selected: MgbaCandidate | None, candidates: Iterable[MgbaCandidate]) -> dict[str, Any]:
    rom = Path(args.rom)
    bridge = Path(args.bridge)
    return {
        "ok": False,
        "mode": args.mode,
        "repoRoot": str(REPO_ROOT),
        "rom": str(rom),
        "romExists": rom.exists(),
        "sym": str(Path(args.sym)),
        "symExists": Path(args.sym).exists(),
        "bridge": str(bridge),
        "bridgeExists": bridge.exists(),
        "selectedMgba": asdict(selected) if selected else None,
        "candidates": [asdict(candidate) for candidate in candidates],
        "requiresGuiFocus": False,
        "usesOsInputInjection": False,
    }


class BridgeClient:
    def __init__(self, host: str, port: int, event_log: Path) -> None:
        self.host = host
        self.port = port
        self.event_log = event_log
        self.sock: socket.socket | None = None
        self.file = None

    def connect(self, timeout: float) -> dict[str, Any]:
        deadline = time.monotonic() + timeout
        last_error: str | None = None

        while time.monotonic() < deadline:
            try:
                self.sock = socket.create_connection((self.host, self.port), timeout=1.0)
                self.sock.settimeout(5.0)
                self.file = self.sock.makefile("rwb", buffering=0)
                greeting = self.read_response()
                append_event(self.event_log, {"event": "bridge_connected", "response": greeting})
                return greeting
            except OSError as exc:
                last_error = str(exc)
                time.sleep(0.1)

        raise TimeoutError(f"timed out connecting to Lua bridge: {last_error}")

    def close(self) -> None:
        if self.file is not None:
            self.file.close()
            self.file = None
        if self.sock is not None:
            self.sock.close()
            self.sock = None

    def request(self, command: str) -> dict[str, Any]:
        if self.file is None:
            raise RuntimeError("bridge is not connected")

        append_event(self.event_log, {"event": "request", "command": command})
        self.file.write((command + "\n").encode("utf-8"))
        response = self.read_response()
        append_event(self.event_log, {"event": "response", "command": command, "response": response})
        return response

    def read_response(self) -> dict[str, Any]:
        if self.file is None:
            raise RuntimeError("bridge is not connected")

        line = self.file.readline()
        if not line:
            raise RuntimeError("Lua bridge closed the socket")
        return json.loads(line.decode("utf-8"))


class RouteTimeout(Exception):
    def __init__(self, label: str, last_beacon: dict[str, Any] | None) -> None:
        super().__init__(label)
        self.label = label
        self.last_beacon = last_beacon


def launch_mgba(args: argparse.Namespace, selected: MgbaCandidate, output_dir: Path) -> tuple[subprocess.Popen[str], Path, Path]:
    stdout_path = output_dir / "mgba_stdout.log"
    stderr_path = output_dir / "mgba_stderr.log"
    stdout_handle = stdout_path.open("w", encoding="utf-8")
    stderr_handle = stderr_path.open("w", encoding="utf-8")
    env = os.environ.copy()
    env["MGBA_LUA_BRIDGE_PORT"] = str(args.port)
    env["MGBA_LUA_BRIDGE_HOST"] = args.host

    command = [selected.path, "-l", "0", "--script", str(args.bridge), str(args.rom)]
    process = subprocess.Popen(
        command,
        cwd=str(REPO_ROOT),
        env=env,
        text=True,
        stdout=stdout_handle,
        stderr=stderr_handle,
    )
    stdout_handle.close()
    stderr_handle.close()
    return process, stdout_path, stderr_path


def beacon_matches(beacon: dict[str, Any], criteria: dict[str, Any]) -> bool:
    if not beacon.get("found"):
        return False

    for key, expected in criteria.items():
        if expected is None:
            continue
        if beacon.get(key) != expected:
            return False
    return True


def wait_for_beacon(
    client: BridgeClient,
    criteria: dict[str, Any],
    timeout: float,
    label: str,
    poll_frames: int = 10,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, criteria):
            return beacon
        client.request(f"run_frames {poll_frames}")

    raise RouteTimeout(label, last_beacon)


def semantic_beacon_matches(beacon: dict[str, Any], criteria: dict[str, Any], semantic_key: str, label: str) -> bool:
    if not beacon_matches(beacon, criteria):
        return False
    if not beacon.get("semanticFound"):
        raise RouteTimeout(f"{label} missing semantic beacon", beacon)
    if beacon.get(semantic_key) != 1:
        return False
    return True


def wait_for_semantic_beacon(
    client: BridgeClient,
    criteria: dict[str, Any],
    semantic_key: str,
    timeout: float,
    label: str,
    poll_frames: int = 10,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if semantic_beacon_matches(beacon, criteria, semantic_key, label):
            append_event(
                client.event_log,
                {
                    "event": "semantic_gate",
                    "label": label,
                    "semanticKey": semantic_key,
                    "beacon": beacon,
                },
            )
            return beacon
        client.request(f"run_frames {poll_frames}")

    raise RouteTimeout(label, last_beacon)


def wait_for_summary_attempt_beacon(client: BridgeClient, timeout: float, label: str, poll_frames: int = 1) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if (
            beacon.get("found")
            and beacon.get("stageId") == STAGE_BATTLE_SUMMARY_REPRO
            and beacon.get("substageId") in REPRO_SUMMARY_ATTEMPT_SUBSTAGES
        ):
            return beacon
        client.request(f"run_frames {poll_frames}")

    raise RouteTimeout(label, last_beacon)


def compact_beacon(beacon: dict[str, Any] | None) -> dict[str, Any] | None:
    if beacon is None:
        return None

    substage_id = beacon.get("substageId")
    compact = {
        "found": beacon.get("found"),
        "frame": beacon.get("frame"),
        "stageId": beacon.get("stageId"),
        "substageId": substage_id,
        "substageName": REPRO_SUBSTAGE_NAMES.get(substage_id, "UNKNOWN"),
        "flags": beacon.get("flags"),
        "scriptWaitKind": beacon.get("scriptWaitKind"),
        "routeErrorCode": beacon.get("routeErrorCode"),
        "inputReady": beacon.get("inputReady"),
        "menuReady": beacon.get("menuReady"),
        "pulse": beacon.get("pulse"),
    }
    if substage_id == REPRO_SUBSTAGE_LOAD_GRAPHICS_STATE:
        compact["summaryLoadGraphicsState"] = (beacon.get("flags") or 0) + (15 * (beacon.get("routeErrorCode") or 0))
        compact["summarySwitchCounter"] = beacon.get("scriptWaitKind")
    return compact


def append_beacon_history(history: list[dict[str, Any]], beacon: dict[str, Any] | None, limit: int = 64) -> None:
    compact = compact_beacon(beacon)
    if compact is None:
        return

    comparable = {key: compact.get(key) for key in ("stageId", "substageId", "flags", "scriptWaitKind", "routeErrorCode", "menuReady")}
    if history:
        last = history[-1]
        last_comparable = {key: last.get(key) for key in comparable}
        if comparable == last_comparable:
            history[-1] = compact
            return

    history.append(compact)
    if len(history) > limit:
        del history[0:len(history) - limit]


def tap(client: BridgeClient, key: str, frames: int = 4, settle_frames: int = 12) -> None:
    client.request(f"tap {key.upper()} {frames}")
    client.request(f"run_frames {settle_frames}")


def png_status(path: Path) -> dict[str, Any]:
    exists = path.exists()
    size = path.stat().st_size if exists else 0
    valid = False
    if exists and size >= 45:
        data = path.read_bytes()
        valid = data.startswith(b"\x89PNG\r\n\x1a\n") and data.endswith(b"\x00\x00\x00\x00IEND\xaeB`\x82")
    return {
        "path": str(path),
        "exists": exists,
        "size": size,
        "validPng": valid,
    }


def capture_screenshot(client: BridgeClient, output_dir: Path, name: str) -> str:
    result = try_lua_screenshot(client, output_dir, name)
    if not result.get("ok"):
        raise RuntimeError(f"screenshot failed: {result}")
    return str(result["path"])


def try_lua_screenshot(client: BridgeClient, output_dir: Path, name: str) -> dict[str, Any]:
    path = output_dir / name
    try:
        response = client.request(f"screenshot {path.resolve()}")
    except Exception as exc:  # noqa: BLE001 - preserve bridge failure in repro artifacts.
        result = {
            "ok": False,
            "method": "lua-screenshot",
            "error": f"{type(exc).__name__}: {exc}",
            **png_status(path),
        }
        append_event(client.event_log, {"event": "screenshot_failed", **result})
        return result

    status = png_status(path)
    result = {
        "ok": response.get("ok") is True and status["validPng"],
        "method": "lua-screenshot",
        "response": response,
        **status,
    }
    append_event(client.event_log, {"event": "screenshot", **result})
    return result


def save_state_file(client: BridgeClient, path: Path) -> dict[str, Any]:
    try:
        response = client.request(f"savestate {path.resolve()} 0")
    except Exception as exc:  # noqa: BLE001 - preserve bridge failure in repro artifacts.
        result = {
            "ok": False,
            "method": "lua-savestate",
            "path": str(path),
            "exists": path.exists(),
            "size": path.stat().st_size if path.exists() else 0,
            "error": f"{type(exc).__name__}: {exc}",
        }
        append_event(client.event_log, {"event": "savestate_failed", **result})
        return result

    result = {
        "ok": response.get("ok") is True and path.exists() and path.stat().st_size > 0,
        "method": "lua-savestate",
        "path": str(path),
        "exists": path.exists(),
        "size": path.stat().st_size if path.exists() else 0,
        "response": response,
    }
    append_event(client.event_log, {"event": "savestate", **result})
    return result


def parse_make_var(path: Path, name: str) -> list[str]:
    if not path.exists():
        return []
    prefix = f"{name} = "
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        if line.startswith(prefix):
            return [item for item in shlex.split(line[len(prefix):]) if not item.startswith("-DLUA_VERSION_ONLY=")]
    return []


def mgba_source_candidates() -> list[Path]:
    paths: list[Path] = []
    env_source = os.environ.get("MGBA_SOURCE_DIR")
    if env_source:
        paths.append(Path(env_source))
    paths.extend([
        REPO_ROOT / "build" / "mgba-master",
        REPO_ROOT / "build" / "mgba-src",
    ])

    unique: list[Path] = []
    for path in paths:
        resolved = path.resolve()
        if resolved not in unique and (resolved / "include" / "mgba" / "core" / "core.h").exists():
            unique.append(resolved)
    return unique


def companion_build_dir(source_dir: Path) -> Path | None:
    candidates = [
        source_dir.parent / f"{source_dir.name}-build",
        REPO_ROOT / "build" / "mgba-master-build",
        REPO_ROOT / "build" / "mgba-src-build",
    ]
    for candidate in candidates:
        if (candidate / "CMakeFiles" / "mgba-headless.dir" / "flags.make").exists():
            return candidate.resolve()
    return None


def build_state_screenshot_helper(args: argparse.Namespace, selected: MgbaCandidate, output_dir: Path) -> Path:
    if args.state_screenshot_helper:
        helper = Path(args.state_screenshot_helper)
        if not helper.exists():
            raise FileNotFoundError(f"state screenshot helper not found: {helper}")
        return helper.resolve()

    compiler = shutil.which("cc") or shutil.which("gcc")
    if compiler is None:
        raise RuntimeError("no C compiler found for state screenshot helper")

    source_file = Path(args.state_screenshot_source)
    if not source_file.exists():
        raise FileNotFoundError(f"state screenshot source not found: {source_file}")

    output_dir.mkdir(parents=True, exist_ok=True)
    helper = output_dir / "mgba_state_screenshot"
    lib_dir = Path(selected.path).resolve().parent
    attempts: list[dict[str, Any]] = []

    for source_dir in mgba_source_candidates():
        source_build_dir = companion_build_dir(source_dir)
        makefile = source_build_dir / "CMakeFiles" / "mgba-headless.dir" / "flags.make" if source_build_dir else Path()
        defines = parse_make_var(makefile, "C_DEFINES")
        include_dirs = [
            lib_dir / "include",
            source_dir / "include",
            source_dir / "src",
            source_dir / "src" / "third-party" / "lzma",
            Path("/usr/include/lua5.2"),
        ]
        command = [
            compiler,
            "-std=c11",
            *defines,
            *[f"-I{path}" for path in include_dirs if path.exists()],
            str(source_file),
            f"-L{lib_dir}",
            f"-Wl,-rpath,{lib_dir}",
            "-lmgba",
            "-lpng",
            "-lz",
            "-lm",
            "-o",
            str(helper),
        ]
        completed = subprocess.run(
            command,
            cwd=str(REPO_ROOT),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=60.0,
            check=False,
        )
        attempt = {
            "sourceDir": str(source_dir),
            "sourceBuildDir": str(source_build_dir) if source_build_dir else None,
            "returnCode": completed.returncode,
            "stdout": completed.stdout[-4000:],
            "stderr": completed.stderr[-4000:],
        }
        attempts.append(attempt)
        append_event(output_dir / "events.ndjson", {"event": "state_screenshot_helper_compile", **attempt})
        if completed.returncode == 0 and helper.exists():
            return helper.resolve()

    raise RuntimeError(f"failed to build state screenshot helper: {attempts}")


def render_state_screenshot(
    args: argparse.Namespace,
    selected: MgbaCandidate,
    output_dir: Path,
    state_path: Path,
    screenshot_path: Path,
) -> dict[str, Any]:
    try:
        helper = build_state_screenshot_helper(args, selected, output_dir)
        command = [str(helper), str(Path(args.rom).resolve()), str(state_path.resolve()), str(screenshot_path.resolve())]
        completed = subprocess.run(
            command,
            cwd=str(REPO_ROOT),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=60.0,
            check=False,
        )
        status = png_status(screenshot_path)
        result = {
            "ok": completed.returncode == 0 and status["validPng"],
            "method": "savestate-helper",
            "helper": str(helper),
            "state": str(state_path),
            "returnCode": completed.returncode,
            "stdout": completed.stdout[-4000:],
            "stderr": completed.stderr[-4000:],
            **status,
        }
    except Exception as exc:  # noqa: BLE001 - repro artifacts should contain the concrete blocker.
        result = {
            "ok": False,
            "method": "savestate-helper",
            "state": str(state_path),
            "error": f"{type(exc).__name__}: {exc}",
            **png_status(screenshot_path),
        }
    append_event(output_dir / "events.ndjson", {"event": "state_screenshot_render", **result})
    return result


def capture_screen_artifact(
    client: BridgeClient,
    args: argparse.Namespace,
    selected: MgbaCandidate,
    output_dir: Path,
    name: str,
) -> dict[str, Any]:
    screenshot_name = f"{name}.png"
    state_path = output_dir / f"{name}.ss"
    screenshot_path = output_dir / screenshot_name
    state = save_state_file(client, state_path)
    if not state.get("ok"):
        return {
            "ok": False,
            "method": "savestate",
            "state": state,
            **png_status(screenshot_path),
        }

    if selected.is_headless:
        result = render_state_screenshot(args, selected, output_dir, state_path, screenshot_path)
        result["state"] = state
        result["luaScreenshotSkipped"] = "selected_mgba_is_headless"
        return result

    lua_result = try_lua_screenshot(client, output_dir, screenshot_name)
    if lua_result.get("ok"):
        lua_result["state"] = state
        return lua_result

    helper_result = render_state_screenshot(args, selected, output_dir, state_path, screenshot_path)
    helper_result["state"] = state
    helper_result["luaScreenshot"] = lua_result
    return helper_result


def move_tap(client: BridgeClient, key: str, settle_frames: int = 24) -> None:
    tap(client, key, frames=8, settle_frames=settle_frames)


def tap_until_beacon(
    client: BridgeClient,
    key: str,
    criteria: dict[str, Any],
    timeout: float,
    label: str,
    press_frames: int = 8,
    settle_frames: int = 18,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, criteria):
            return beacon
        if beacon.get("found") and beacon.get("inputReady") == 1:
            tap(client, key, press_frames, settle_frames)
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def route_phase(client: BridgeClient, phase: str) -> None:
    append_event(client.event_log, {"event": "route_phase", "phase": phase})


def movement_ready_criteria(
    stage_id: int,
    substage_id: int | None = None,
    map_kind: int | None = None,
    map_slot: int | None = None,
) -> dict[str, Any]:
    criteria: dict[str, Any] = {
        "stageId": stage_id,
        "inputReady": 1,
        "flags": 0,
    }
    if substage_id is not None:
        criteria["substageId"] = substage_id
    if map_kind is not None:
        criteria["mapKind"] = map_kind
    if map_slot is not None:
        criteria["mapSlot"] = map_slot
    return criteria


def semantic_movement_ready_criteria(
    stage_id: int,
    substage_id: int | None = None,
    map_kind: int | None = None,
    map_slot: int | None = None,
) -> dict[str, Any]:
    criteria = movement_ready_criteria(stage_id, substage_id, map_kind, map_slot)
    criteria["semanticFound"] = True
    criteria["movementReady"] = 1
    return criteria


def move_until_stage(
    client: BridgeClient,
    key: str,
    target_stage: int,
    max_presses: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    for _ in range(max_presses):
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"stageId": target_stage}):
            return beacon
        if beacon.get("found") and beacon.get("inputReady") == 1 and beacon.get("flags") == 0:
            move_tap(client, key)
        else:
            client.request("run_frames 10")

        if time.monotonic() >= deadline:
            raise RouteTimeout(label, last_beacon)

    return wait_for_beacon(client, {"stageId": target_stage}, max(1.0, deadline - time.monotonic()), label)


def move_or_tap_until_stage(
    client: BridgeClient,
    move_key: str,
    target_stage: int,
    max_moves: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None
    moves = 0

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"stageId": target_stage}):
            return beacon
        if beacon.get("found") and beacon.get("inputReady") == 1:
            if beacon.get("flags") == 0:
                if moves >= max_moves:
                    raise RouteTimeout(label, beacon)
                move_tap(client, move_key, settle_frames=48)
                moves += 1
            else:
                tap(client, "A", frames=8, settle_frames=18)
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def move_until_substage(
    client: BridgeClient,
    key: str,
    stage_id: int,
    target_substage: int,
    max_presses: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    for _ in range(max_presses):
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"stageId": stage_id, "substageId": target_substage}):
            return beacon
        if beacon_matches(beacon, movement_ready_criteria(stage_id)):
            move_tap(client, key)
        else:
            client.request("run_frames 10")

        if time.monotonic() >= deadline:
            raise RouteTimeout(label, last_beacon)

    return wait_for_beacon(
        client,
        {"stageId": stage_id, "substageId": target_substage},
        max(1.0, deadline - time.monotonic()),
        label,
    )


def move_until_map_slot(
    client: BridgeClient,
    key: str,
    target_map_slot: int,
    max_presses: int,
    timeout: float,
    label: str,
    stage_id: int | None = None,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None
    presses = 0

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"mapSlot": target_map_slot}):
            return beacon
        if (
            beacon.get("found")
            and beacon.get("inputReady") == 1
            and beacon.get("flags") == 0
            and (stage_id is None or beacon.get("stageId") == stage_id)
        ):
            if presses >= max_presses:
                raise RouteTimeout(label, beacon)
            move_tap(client, key, settle_frames=48)
            presses += 1
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def move_until_script_input_or_substage(
    client: BridgeClient,
    key: str,
    stage_id: int,
    target_substage: int,
    max_presses: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None
    presses = 0

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"stageId": stage_id, "substageId": target_substage}):
            return beacon
        if (
            beacon.get("found")
            and beacon.get("stageId") == stage_id
            and beacon.get("inputReady") == 1
            and beacon.get("flags", 0) != 0
        ):
            return beacon
        if beacon_matches(beacon, movement_ready_criteria(stage_id)):
            if presses >= max_presses:
                raise RouteTimeout(label, beacon)
            move_tap(client, key)
            presses += 1
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def move_repeated(
    client: BridgeClient,
    key: str,
    count: int,
    stage_id: int,
    substage_id: int | None,
    map_slot: int | None = None,
    timeout_per_step: float = 30.0,
) -> None:
    for index in range(count):
        criteria = movement_ready_criteria(stage_id, substage_id, None, map_slot)
        wait_for_beacon(client, criteria, timeout_per_step, f"move ready {key} {index + 1}/{count}")
        move_tap(client, key)


def move_to_position(
    client: BridgeClient,
    target_x: int,
    target_y: int,
    stage_id: int,
    substage_id: int | None,
    map_kind: int,
    timeout: float,
    label: str,
    map_slot: int | None = None,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = wait_for_movement_ready_or_advance_text(
            client,
            movement_ready_criteria(stage_id, substage_id, map_kind, map_slot),
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_beacon = beacon
        if not beacon.get("navFound"):
            raise RouteTimeout(f"{label} nav proof", beacon)

        player_x = beacon.get("playerX")
        player_y = beacon.get("playerY")
        if player_x == target_x and player_y == target_y:
            return beacon
        if player_x is None or player_y is None:
            raise RouteTimeout(f"{label} missing position", beacon)

        if player_x < target_x:
            move_tap(client, "RIGHT")
        elif player_x > target_x:
            move_tap(client, "LEFT")
        elif player_y < target_y:
            move_tap(client, "DOWN")
        elif player_y > target_y:
            move_tap(client, "UP")

    raise RouteTimeout(label, last_beacon)


def face_tile(
    client: BridgeClient,
    key: str,
    front_x: int,
    front_y: int,
    stage_id: int,
    substage_id: int | None,
    map_kind: int,
    timeout: float,
    label: str,
    map_slot: int | None = None,
) -> dict[str, Any]:
    key = key.upper()
    expected_facing = KEY_FACING[key]
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = wait_for_movement_ready_or_advance_text(
            client,
            movement_ready_criteria(stage_id, substage_id, map_kind, map_slot),
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_beacon = beacon
        if not beacon.get("navFound"):
            raise RouteTimeout(f"{label} nav proof", beacon)
        if (
            beacon.get("playerFacing") == expected_facing
            and beacon.get("frontX") == front_x
            and beacon.get("frontY") == front_y
        ):
            return beacon
        move_tap(client, key)

    raise RouteTimeout(label, last_beacon)


def wait_for_movement_ready_or_advance_text(
    client: BridgeClient,
    criteria: dict[str, Any],
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, criteria):
            return beacon
        if (
            beacon.get("found")
            and beacon.get("stageId") == criteria.get("stageId")
            and beacon.get("inputReady") == 1
            and (beacon.get("textReady") == 1 or beacon.get("flags", 0) != 0)
        ):
            tap(client, "A", frames=8, settle_frames=18)
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def remaining_timeout(deadline: float, default: float, label: str, client: BridgeClient) -> float:
    remaining = deadline - time.monotonic()
    if remaining <= 0:
        raise RouteTimeout(label, client.request("read_beacon"))
    return min(default, remaining)


def drive_to_main_menu(client: BridgeClient, timeout: float) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    iteration = 0

    client.request("ping")
    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        if beacon.get("found") and beacon.get("stageId") == 1 and beacon.get("inputReady") == 1:
            return {"ok": True, "target": "MAIN_MENU_READY", "beacon": beacon}

        key = "START" if iteration % 2 == 0 else "A"
        client.request(f"tap {key} 4")
        client.request("run_frames 20")
        iteration += 1

    return {"ok": False, "target": "MAIN_MENU_READY", "error": "timeout"}


def advance_dialogue_until_stage(client: BridgeClient, target_stage: int, timeout: float, label: str) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon.get("found") and beacon.get("stageId", -1) >= target_stage:
            return beacon
        if beacon.get("found") and beacon.get("inputReady") == 1:
            tap(client, "A")
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def advance_to_gender_prompt(client: BridgeClient, timeout: float) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(beacon, {"stageId": STAGE_GENDER_PROMPT_READY, "inputReady": 1}):
            return beacon
        if beacon.get("found") and beacon.get("stageId", -1) > STAGE_GENDER_PROMPT_READY:
            break
        if beacon.get("found") and beacon.get("inputReady") == 1:
            # The boy/girl text advances into the menu automatically. Pressing A
            # as the menu appears can select the default male choice on the same
            # held keypress, so wait it out instead.
            if beacon.get("stageId") == STAGE_BIRCH_INTRO_TEXT and beacon.get("substageId") == 4:
                client.request("run_frames 6")
            else:
                tap(client, "A")
        else:
            client.request("run_frames 10")

    raise RouteTimeout("gender prompt ready", last_beacon)


def drive_to_truck(client: BridgeClient, timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()

    main_menu = drive_to_main_menu(client, timeout)
    if not main_menu.get("ok"):
        return main_menu
    tap(client, "A")

    gender_prompt = advance_to_gender_prompt(client, 120.0)
    if gender_prompt.get("substageId") != 1:
        tap(client, "DOWN")
        wait_for_beacon(
            client,
            {"stageId": STAGE_GENDER_PROMPT_READY, "substageId": 1, "inputReady": 1},
            30.0,
            "female cursor",
        )
    tap_until_beacon(client, "A", {"gender": GENDER_FEMALE}, 30.0, "female confirmed")

    advance_dialogue_until_stage(client, STAGE_NAMING_SCREEN_READY, 120.0, "naming screen")
    wait_for_beacon(
        client,
        {"stageId": STAGE_NAMING_SCREEN_READY, "inputReady": 1},
        30.0,
        "naming ready",
    )
    tap(client, "A")
    wait_for_beacon(
        client,
        {
            "stageId": STAGE_NAMING_SCREEN_READY,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
            "inputReady": 1,
        },
        30.0,
        "name A entered and ready",
    )
    tap_until_beacon(
        client,
        "START",
        {"stageId": STAGE_NAMING_SCREEN_READY, "flags": 3, "inputReady": 1},
        30.0,
        "naming OK ready",
        press_frames=8,
        settle_frames=24,
    )
    tap_until_beacon(
        client,
        "A",
        {
            "stageId": STAGE_NAME_CONFIRMED,
            "gender": GENDER_FEMALE,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
        },
        30.0,
        "name confirmed",
    )

    wait_for_beacon(
        client,
        {"stageId": STAGE_NAME_CONFIRMED, "inputReady": 1},
        30.0,
        "name confirmation prompt",
    )
    tap(client, "A")

    advance_dialogue_until_stage(client, STAGE_TRUCK_CONTROL_READY, 180.0, "truck control")
    truck = wait_for_beacon(
        client,
        {
            "stageId": STAGE_TRUCK_CONTROL_READY,
            "gender": GENDER_FEMALE,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
            "mapKind": MAP_TRUCK,
            "inputReady": 1,
        },
        180.0,
        "truck control ready",
    )

    return {
        "ok": True,
        "target": "TRUCK_CONTROL_READY",
        "beacon": truck,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_to_starter_choose(client: BridgeClient, truck_timeout: float, starter_timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()
    truck = drive_to_truck(client, truck_timeout)
    if not truck.get("ok"):
        return truck

    deadline = time.monotonic() + starter_timeout

    route_phase(client, "truck_exit")
    move_until_stage(
        client,
        "RIGHT",
        STAGE_LITTLEROOT_ROUTE_SETUP,
        4,
        remaining_timeout(deadline, 60.0, "truck exit to Littleroot", client),
        "truck exit to Littleroot",
    )
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 4, MAP_LITTLEROOT),
        remaining_timeout(deadline, 90.0, "moving-in intro complete", client),
        "moving-in intro complete",
    )

    route_phase(client, "clock_setup")
    move_until_substage(
        client,
        "UP",
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        5,
        remaining_timeout(deadline, 60.0, "bedroom setup ready", client),
        "bedroom setup ready",
    )
    wait_for_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 5, MAP_LITTLEROOT),
        remaining_timeout(deadline, 60.0, "bedroom ready", client),
        "bedroom ready",
    )
    move_to_position(
        client,
        3,
        2,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 15.0, "clock facing ready", client),
        "clock facing ready",
    )
    face_tile(
        client,
        "UP",
        3,
        1,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 15.0, "clock facing ready", client),
        "clock facing ready",
    )
    tap(client, "A")

    tap_until_beacon(
        client,
        "A",
        {"stageId": STAGE_LITTLEROOT_ROUTE_SETUP, "substageId": 6, "mapKind": MAP_LITTLEROOT, "inputReady": 1},
        remaining_timeout(deadline, 60.0, "clock set screen ready", client),
        "clock set screen ready",
    )
    tap(client, "A")
    wait_for_beacon(
        client,
        {"stageId": STAGE_LITTLEROOT_ROUTE_SETUP, "substageId": 7, "mapKind": MAP_LITTLEROOT, "inputReady": 1},
        remaining_timeout(deadline, 30.0, "clock confirm ready", client),
        "clock confirm ready",
    )
    tap(client, "UP")
    tap(client, "A")

    route_phase(client, "tv_report")
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 6, MAP_LITTLEROOT),
        remaining_timeout(deadline, 90.0, "bedroom after clock", client),
        "bedroom after clock",
    )
    move_to_position(
        client,
        1,
        2,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        6,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 30.0, "bedroom stair approach", client),
        "bedroom stair approach",
    )
    move_until_script_input_or_substage(
        client,
        "UP",
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        4,
        remaining_timeout(deadline, 60.0, "tv report script ready", client),
        "tv report script ready",
    )
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_MAYS_HOUSE_1F),
        remaining_timeout(deadline, 120.0, "tv report complete", client),
        "tv report complete",
    )

    route_phase(client, "rival_house")
    move_to_position(
        client,
        2,
        8,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 45.0, "player house exit approach", client),
        "player house exit approach",
        MAP_SLOT_MAYS_HOUSE_1F,
    )
    move_until_map_slot(
        client,
        "DOWN",
        MAP_SLOT_LITTLEROOT_TOWN,
        3,
        remaining_timeout(deadline, 45.0, "outside player house", client),
        "outside player house",
        STAGE_LITTLEROOT_ROUTE_SETUP,
    )
    wait_for_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_LITTLEROOT_TOWN),
        remaining_timeout(deadline, 45.0, "outside player house ready", client),
        "outside player house ready",
    )
    move_to_position(
        client,
        5,
        9,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 60.0, "rival house door approach", client),
        "rival house door approach",
        MAP_SLOT_LITTLEROOT_TOWN,
    )
    move_until_map_slot(
        client,
        "UP",
        MAP_SLOT_BRENDANS_HOUSE_1F,
        3,
        remaining_timeout(deadline, 45.0, "rival house entered", client),
        "rival house entered",
        STAGE_LITTLEROOT_ROUTE_SETUP,
    )
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_BRENDANS_HOUSE_1F),
        remaining_timeout(deadline, 90.0, "rival mom complete", client),
        "rival mom complete",
    )
    move_to_position(
        client,
        8,
        3,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 45.0, "rival house stairs approach", client),
        "rival house stairs approach",
        MAP_SLOT_BRENDANS_HOUSE_1F,
    )
    move_until_map_slot(
        client,
        "UP",
        MAP_SLOT_BRENDANS_HOUSE_2F,
        3,
        remaining_timeout(deadline, 45.0, "rival bedroom entered", client),
        "rival bedroom entered",
        STAGE_LITTLEROOT_ROUTE_SETUP,
    )
    wait_for_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_BRENDANS_HOUSE_2F),
        remaining_timeout(deadline, 45.0, "rival bedroom ready", client),
        "rival bedroom ready",
    )
    move_to_position(
        client,
        4,
        4,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 45.0, "rival pokeball approach", client),
        "rival pokeball approach",
        MAP_SLOT_BRENDANS_HOUSE_2F,
    )
    face_tile(
        client,
        "LEFT",
        3,
        4,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 15.0, "rival pokeball facing", client),
        "rival pokeball facing",
        MAP_SLOT_BRENDANS_HOUSE_2F,
    )
    tap(client, "A")
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_BRENDANS_HOUSE_2F),
        remaining_timeout(deadline, 90.0, "rival intro complete", client),
        "rival intro complete",
    )
    move_to_position(
        client,
        7,
        2,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 45.0, "rival bedroom exit approach", client),
        "rival bedroom exit approach",
        MAP_SLOT_BRENDANS_HOUSE_2F,
    )
    move_until_map_slot(
        client,
        "UP",
        MAP_SLOT_BRENDANS_HOUSE_1F,
        3,
        remaining_timeout(deadline, 45.0, "rival bedroom exited", client),
        "rival bedroom exited",
        STAGE_LITTLEROOT_ROUTE_SETUP,
    )
    wait_for_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_BRENDANS_HOUSE_1F),
        remaining_timeout(deadline, 45.0, "rival house exit ready", client),
        "rival house exit ready",
    )
    move_to_position(
        client,
        8,
        7,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 45.0, "rival house exit approach", client),
        "rival house exit approach",
        MAP_SLOT_BRENDANS_HOUSE_1F,
    )
    move_until_map_slot(
        client,
        "DOWN",
        MAP_SLOT_LITTLEROOT_TOWN,
        3,
        remaining_timeout(deadline, 45.0, "rival house exited", client),
        "rival house exited",
        STAGE_LITTLEROOT_ROUTE_SETUP,
    )

    route_phase(client, "route101")
    wait_for_semantic_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 7, MAP_LITTLEROOT, MAP_SLOT_LITTLEROOT_TOWN),
        "movementReady",
        remaining_timeout(deadline, 45.0, "town route approach ready", client),
        "town route approach ready",
    )
    move_to_position(
        client,
        11,
        2,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        7,
        MAP_LITTLEROOT,
        remaining_timeout(deadline, 60.0, "route 101 town trigger approach", client),
        "route 101 town trigger approach",
        MAP_SLOT_LITTLEROOT_TOWN,
    )
    move_or_tap_until_stage(
        client,
        "UP",
        STAGE_ROUTE101_APPROACH,
        6,
        remaining_timeout(deadline, 90.0, "route 101 entered", client),
        "route 101 entered",
    )
    client.request("clear_keys")
    client.request("run_frames 20")
    tap_until_beacon(
        client,
        "A",
        movement_ready_criteria(STAGE_ROUTE101_APPROACH, 2, MAP_ROUTE101, MAP_SLOT_ROUTE101),
        remaining_timeout(deadline, 120.0, "route 101 approach", client),
        "route 101 approach",
    )
    move_to_position(
        client,
        8,
        14,
        STAGE_ROUTE101_APPROACH,
        2,
        MAP_ROUTE101,
        remaining_timeout(deadline, 60.0, "starter bag approach", client),
        "starter bag approach",
        MAP_SLOT_ROUTE101,
    )
    face_tile(
        client,
        "LEFT",
        7,
        14,
        STAGE_ROUTE101_APPROACH,
        2,
        MAP_ROUTE101,
        remaining_timeout(deadline, 15.0, "starter bag facing", client),
        "starter bag facing",
        MAP_SLOT_ROUTE101,
    )
    tap(client, "A")

    starter = wait_for_beacon(
        client,
        {
            "stageId": STAGE_STARTER_CHOOSE_READY,
            "mapKind": MAP_STARTER_SELECTION,
            "gender": GENDER_FEMALE,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
            "inputReady": 1,
        },
        remaining_timeout(deadline, 120.0, "starter choose ready", client),
        "starter choose ready",
    )

    return {
        "ok": True,
        "target": "STARTER_CHOOSE_READY",
        "beacon": starter,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_to_starter_confirm(client: BridgeClient, truck_timeout: float, starter_timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()
    starter = drive_to_starter_choose(client, truck_timeout, starter_timeout)
    if not starter["ok"]:
        return starter

    tap(client, "A")
    confirm = wait_for_semantic_beacon(
        client,
        {
            "stageId": STAGE_STARTER_CONFIRM_PROMPT,
            "mapKind": MAP_STARTER_SELECTION,
            "gender": GENDER_FEMALE,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
            "inputReady": 1,
        },
        "menuReady",
        120.0,
        "starter confirm prompt",
    )

    return {
        "ok": True,
        "target": "STARTER_CONFIRM_PROMPT",
        "beacon": confirm,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_probe_smoke(client: BridgeClient, args: argparse.Namespace) -> dict[str, Any]:
    route_start = time.monotonic()
    probe_address = resolve_probe_address(args)
    trace: list[dict[str, Any]] = []

    trace.append(asdict(RouteNode("EarlyGame.StarterConfirm", "Existing beacon-gated starter confirmation route")))
    confirm = drive_to_starter_confirm(client, args.truck_timeout, args.starter_timeout)
    if not confirm.get("ok"):
        return confirm

    probe = read_probe(client, probe_address)
    validate_probe(probe)
    ok = (
        probe.get("routeStage") == STAGE_STARTER_CONFIRM_PROMPT
        and probe.get("playerPartyCount") == 0
    )
    trace.append({
        "id": "Probe.Validated",
        "accepted": ok,
        "routeStage": probe.get("routeStage"),
        "playerPartyCount": probe.get("playerPartyCount"),
    })

    return {
        "ok": ok,
        "target": "PROBE_SMOKE",
        "probeAddress": probe_address,
        "starterConfirmBeacon": confirm["beacon"],
        "probe": probe,
        "routeTrace": trace,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_poochyena_intimidate(
    client: BridgeClient,
    args: argparse.Namespace,
    selected: MgbaCandidate,
    output_dir: Path,
) -> dict[str, Any]:
    route_start = time.monotonic()
    probe_address = resolve_probe_address(args)
    trace: list[dict[str, Any]] = []

    trace.append(asdict(RouteNode("EarlyGame.StarterConfirm", "Existing beacon-gated starter confirmation route")))
    confirm = drive_to_starter_confirm(client, args.truck_timeout, args.starter_timeout)
    if not confirm.get("ok"):
        return confirm

    trace.append({
        "id": "Probe.Command.GrantItems",
        "description": "Automation-only setup grants balls/candies for future route branches.",
    })
    grant_balls = send_probe_command(
        client,
        probe_address,
        AUTOMATION_PROBE_COMMAND_GRANT_ITEM,
        ITEM_POKE_BALL,
        10,
        args.objective_timeout,
    )
    if grant_balls.get("commandResult") != AUTOMATION_PROBE_COMMAND_RESULT_OK:
        raise RuntimeError(f"grant Poke Ball command failed: {grant_balls}")
    grant_candies = send_probe_command(
        client,
        probe_address,
        AUTOMATION_PROBE_COMMAND_GRANT_ITEM,
        ITEM_RARE_CANDY,
        20,
        args.objective_timeout,
    )
    if grant_candies.get("commandResult") != AUTOMATION_PROBE_COMMAND_RESULT_OK:
        raise RuntimeError(f"grant Rare Candy command failed: {grant_candies}")

    trace.append({
        "id": "Party.PoochyenaSlot0",
        "description": "Guarded scenario command creates the slot-0 Poochyena state for this spike.",
    })
    poochyena_probe = send_probe_command(
        client,
        probe_address,
        AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0,
        17,
        0,
        args.objective_timeout,
    )
    if poochyena_probe.get("commandResult") != AUTOMATION_PROBE_COMMAND_RESULT_OK:
        raise RuntimeError(f"create Poochyena command failed: {poochyena_probe}")
    poochyena = find_party_mon(poochyena_probe, SPECIES_POOCHYENA, ability_num=0)
    if poochyena is None:
        raise RuntimeError(f"slot-0 Poochyena was not observable in probe: {poochyena_probe}")

    trace.append({
        "id": "Party.MightyenaIntimidate",
        "description": "Guarded scenario command promotes the same ability slot to the final invariant.",
        "sourceSlot": poochyena["slot"],
    })
    final_probe = send_probe_command(
        client,
        probe_address,
        AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA,
        poochyena["slot"],
        0,
        args.objective_timeout,
    )
    if final_probe.get("commandResult") != AUTOMATION_PROBE_COMMAND_RESULT_OK:
        raise RuntimeError(f"promote Poochyena command failed: {final_probe}")
    mightyena = find_party_mon(final_probe, SPECIES_MIGHTYENA, ability_num=0, ability=ABILITY_INTIMIDATE)
    ok = mightyena is not None

    client.request("run_frames 30")
    save_path = output_dir / "mightyena_intimidate_confirmed.ss"
    save_artifact = save_state_file(client, save_path)
    screen_artifact = capture_screen_artifact(client, args, selected, output_dir, "mightyena_intimidate_confirmed")

    return {
        "ok": ok and save_artifact.get("ok") is True,
        "target": "MIGHTYENA_INTIMIDATE",
        "probeAddress": probe_address,
        "ttlHours": 4,
        "artifactPolicy": "build output is git-ignored; savestate is temporary",
        "routeTrace": trace,
        "starterConfirmBeacon": confirm["beacon"],
        "grantBallsProbe": grant_balls,
        "grantCandiesProbe": grant_candies,
        "poochyenaProbe": poochyena_probe,
        "finalProbe": final_probe,
        "poochyena": poochyena,
        "mightyena": mightyena,
        "saveState": save_artifact,
        "screenArtifact": screen_artifact,
        "deferredCanonicalWork": "This spike proves probe facts, command dispatch, route composition, and the ignored save artifact. Full wild-battle capture plus item/evolution UI remains a later route-tree branch.",
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def advance_dialogue_to_battle_action_menu(client: BridgeClient, timeout: float) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if beacon_matches(
            beacon,
            {
                "stageId": STAGE_BATTLE_SUMMARY_REPRO,
                "substageId": REPRO_SUBSTAGE_ACTION_MENU,
                "menuReady": 1,
            },
        ):
            return beacon
        if beacon.get("found") and (
            beacon.get("inputReady") == 1
            or beacon.get("textReady") == 1
            or beacon.get("menuReady") == 1
        ):
            tap(client, "A", frames=8, settle_frames=24)
        else:
            client.request("run_frames 20")

    raise RouteTimeout("initial battle action menu", last_beacon)


def drive_to_initial_battle_summary(
    client: BridgeClient,
    args: argparse.Namespace,
    selected: MgbaCandidate,
    output_dir: Path,
    truck_timeout: float,
    starter_timeout: float,
    battle_timeout: float,
    summary_timeout: float,
) -> dict[str, Any]:
    route_start = time.monotonic()
    route_phase(client, "starter_confirm")
    confirm = drive_to_starter_confirm(client, truck_timeout, starter_timeout)
    if not confirm["ok"]:
        return confirm

    route_phase(client, "confirm_starter")
    tap(client, "A", frames=8, settle_frames=24)
    action_menu = advance_dialogue_to_battle_action_menu(client, battle_timeout)

    route_phase(client, "battle_action_menu")
    tap(client, "DOWN", frames=8, settle_frames=24)
    party_cursor = wait_for_semantic_beacon(
        client,
        {
            "stageId": STAGE_BATTLE_SUMMARY_REPRO,
            "substageId": REPRO_SUBSTAGE_ACTION_MENU,
            "flags": 2,
        },
        "menuReady",
        15.0,
        "battle party cursor",
    )
    tap(client, "A", frames=8, settle_frames=24)

    route_phase(client, "party_menu")
    party_menu = wait_for_semantic_beacon(
        client,
        {
            "stageId": STAGE_BATTLE_SUMMARY_REPRO,
            "substageId": REPRO_SUBSTAGE_PARTY_MENU,
        },
        "menuReady",
        60.0,
        "in-battle party menu",
    )
    tap(client, "A", frames=8, settle_frames=24)

    route_phase(client, "party_mon_menu")
    party_mon_menu = wait_for_semantic_beacon(
        client,
        {
            "stageId": STAGE_BATTLE_SUMMARY_REPRO,
            "substageId": REPRO_SUBSTAGE_PARTY_MON_MENU,
            "flags": 0,
        },
        "menuReady",
        30.0,
        "party pokemon action menu summary cursor",
    )
    tap(client, "A", frames=8, settle_frames=1)

    summary_requested = wait_for_summary_attempt_beacon(client, 15.0, "summary requested")

    screen_artifact = capture_screen_artifact(client, args, selected, output_dir, "battle_summary_attempt")
    summary_screen_reached = False
    final_beacon: dict[str, Any] | None = summary_requested
    summary_error: dict[str, Any] | None = None
    final_screen_artifact: dict[str, Any] | None = None
    summary_beacon_history: list[dict[str, Any]] = []
    append_beacon_history(summary_beacon_history, summary_requested)
    bridge_alive = True
    try:
        deadline = time.monotonic() + summary_timeout
        while time.monotonic() < deadline:
            beacon = client.request("read_beacon")
            final_beacon = beacon
            append_beacon_history(summary_beacon_history, beacon)
            if semantic_beacon_matches(
                beacon,
                {
                    "stageId": STAGE_BATTLE_SUMMARY_REPRO,
                    "substageId": REPRO_SUBSTAGE_SUMMARY_SCREEN,
                },
                "menuReady",
                "summary screen",
            ):
                append_event(
                    client.event_log,
                    {
                        "event": "semantic_gate",
                        "label": "summary screen",
                        "semanticKey": "menuReady",
                        "beacon": beacon,
                    },
                )
                summary_screen_reached = True
                break
            client.request("run_frames 10")
        if not summary_screen_reached:
            raise RouteTimeout("summary screen", final_beacon)
        client.request("run_frames 30")
    except RouteTimeout as exc:
        summary_error = {
            "error": "timeout",
            "label": exc.label,
            "lastBeacon": exc.last_beacon,
        }
        final_beacon = exc.last_beacon
    except Exception as exc:  # noqa: BLE001 - a bridge reset is a valid repro outcome.
        summary_error = {
            "error": "bridge_exception",
            "label": "summary screen",
            "exception": f"{type(exc).__name__}: {exc}",
            "lastBeacon": final_beacon,
        }
        bridge_alive = False

    if bridge_alive:
        final_screen_artifact = capture_screen_artifact(client, args, selected, output_dir, "battle_summary_attempt")
        if final_screen_artifact.get("ok"):
            screen_artifact = final_screen_artifact

    route_ok = summary_requested.get("found") is True
    artifact_ok = screen_artifact.get("ok") is True
    summary_validation_ok = route_ok and summary_screen_reached and artifact_ok
    bug_observed = route_ok and not summary_screen_reached

    return {
        "ok": summary_validation_ok,
        "target": "BATTLE_SUMMARY_SCREEN",
        "routeOk": route_ok,
        "artifactOk": artifact_ok,
        "summaryValidationOk": summary_validation_ok,
        "bugObserved": bug_observed,
        "attemptedSummary": True,
        "summaryRequestedObserved": summary_requested.get("substageId") == REPRO_SUBSTAGE_SUMMARY_REQUESTED,
        "summaryScreenReached": summary_screen_reached,
        "screenshot": screen_artifact.get("path"),
        "screenArtifact": screen_artifact,
        "starterConfirmBeacon": confirm["beacon"],
        "battleActionMenuBeacon": action_menu,
        "partyCursorBeacon": party_cursor,
        "partyMenuBeacon": party_menu,
        "partyMonMenuBeacon": party_mon_menu,
        "summaryRequestedBeacon": summary_requested,
        "finalBeacon": final_beacon,
        "lastSummaryDebugBeacon": summary_beacon_history[-1] if summary_beacon_history else None,
        "summaryBeaconHistory": summary_beacon_history,
        "summaryError": summary_error,
        "finalScreenArtifact": final_screen_artifact,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def capability_blockers(report: dict[str, Any], selected: MgbaCandidate | None, args: argparse.Namespace) -> list[str]:
    blockers: list[str] = []

    if selected is None:
        blockers.append("no_mgba_binary_found")
    elif not selected.supports_script and not args.force_script:
        blockers.append("selected_mgba_help_does_not_advertise_script_option")

    if args.require_headless and selected is not None and not selected.is_headless:
        blockers.append("selected_mgba_is_not_headless")

    if not report["romExists"]:
        blockers.append("rom_missing")
    if args.mode in {"probe-smoke", "poochyena-intimidate"} and not report["symExists"] and not Path(args.sym).with_suffix(".map").exists():
        blockers.append("sym_or_map_missing")
    if not report["bridgeExists"]:
        blockers.append("bridge_missing")

    return blockers


def run(args: argparse.Namespace) -> int:
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    event_log = output_dir / "events.ndjson"
    if event_log.exists():
        event_log.unlink()

    selected, candidates = inspect_mgba(args.mgba)
    report = base_report(args, selected, candidates)
    report_path = output_dir / "capability_report.json"
    blockers = capability_blockers(report, selected, args)
    report["blockers"] = blockers
    report["ok"] = not blockers
    write_json(report_path, report)

    if args.mode == "capability":
        print(json.dumps({"ok": report["ok"], "blockers": blockers, "report": str(report_path)}, sort_keys=True))
        return 0

    if blockers:
        print(json.dumps({"ok": False, "blockers": blockers, "report": str(report_path)}, sort_keys=True))
        return 2

    assert selected is not None
    process: subprocess.Popen[str] | None = None
    client = BridgeClient(args.host, args.port, event_log)

    try:
        process, stdout_path, stderr_path = launch_mgba(args, selected, output_dir)
        append_event(event_log, {
            "event": "mgba_launched",
            "pid": process.pid,
            "stdout": str(stdout_path),
            "stderr": str(stderr_path),
        })
        client.connect(args.connect_timeout)
        try:
            if args.mode == "truck":
                result = drive_to_truck(client, args.truck_timeout)
            elif args.mode == "starter":
                result = drive_to_starter_choose(client, args.truck_timeout, args.starter_timeout)
            elif args.mode == "starter-confirm":
                result = drive_to_starter_confirm(client, args.truck_timeout, args.starter_timeout)
            elif args.mode == "probe-smoke":
                result = drive_probe_smoke(client, args)
            elif args.mode == "poochyena-intimidate":
                result = drive_poochyena_intimidate(client, args, selected, output_dir)
            elif args.mode == "battle-summary":
                result = drive_to_initial_battle_summary(
                    client,
                    args,
                    selected,
                    output_dir,
                    args.truck_timeout,
                    args.starter_timeout,
                    args.battle_timeout,
                    args.summary_timeout,
                )
            else:
                result = drive_to_main_menu(client, args.smoke_timeout)
        except RouteTimeout as exc:
            result = {
                "ok": False,
                "target": args.mode,
                "error": "timeout",
                "label": exc.label,
                "lastBeacon": exc.last_beacon,
            }
        except Exception as exc:  # noqa: BLE001 - always write a repro result artifact.
            result = {
                "ok": False,
                "target": args.mode,
                "error": "exception",
                "exception": f"{type(exc).__name__}: {exc}",
            }
        write_json(output_dir / "run_result.json", result)
        print(json.dumps({"ok": result["ok"], "result": str(output_dir / "run_result.json")}, sort_keys=True))
        return 0 if result["ok"] else 1
    finally:
        try:
            if client.sock is not None:
                client.request("clear_keys")
        except Exception:
            pass
        client.close()
        if process is not None and process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=5.0)
            except subprocess.TimeoutExpired:
                process.kill()


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--mode",
        choices=[
            "capability",
            "main-menu",
            "truck",
            "starter",
            "starter-confirm",
            "probe-smoke",
            "poochyena-intimidate",
            "battle-summary",
        ],
        default="capability",
    )
    parser.add_argument("--mgba", help="mGBA executable to inspect or launch")
    parser.add_argument("--rom", default=str(DEFAULT_ROM), help="automation ROM path")
    parser.add_argument("--sym", default=str(DEFAULT_SYM), help="symbol or map file used to locate gAutomationProbe")
    parser.add_argument("--bridge", default=str(DEFAULT_BRIDGE), help="Lua bridge script path")
    parser.add_argument("--output-dir", default=str(DEFAULT_OUTPUT_DIR), help="artifact directory")
    parser.add_argument("--host", default="127.0.0.1", help="Lua bridge host")
    parser.add_argument("--port", type=int, default=DEFAULT_PORT, help="Lua bridge port")
    parser.add_argument("--connect-timeout", type=float, default=15.0)
    parser.add_argument("--smoke-timeout", type=float, default=60.0)
    parser.add_argument("--truck-timeout", type=float, default=300.0)
    parser.add_argument("--starter-timeout", type=float, default=600.0)
    parser.add_argument("--battle-timeout", type=float, default=300.0)
    parser.add_argument("--summary-timeout", type=float, default=45.0)
    parser.add_argument("--objective-timeout", type=float, default=120.0)
    parser.add_argument("--state-screenshot-source", default=str(DEFAULT_STATE_SCREENSHOT_SOURCE), help="C source for savestate-to-PNG fallback helper")
    parser.add_argument("--state-screenshot-helper", help="prebuilt savestate-to-PNG fallback helper")
    parser.add_argument("--force-script", action="store_true", help="try --script even if --help does not list it")
    parser.add_argument("--require-headless", action="store_true", help="block if the selected binary is not headless")
    return parser.parse_args(argv)


if __name__ == "__main__":
    raise SystemExit(run(parse_args(sys.argv[1:])))
