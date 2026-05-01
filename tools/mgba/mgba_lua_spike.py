#!/usr/bin/env python3
"""Reference-only Python host spike for mGBA Lua automation.

Do not extend this file for canonical gameplay route automation.

This script is retained only as a historical/debugging reference for the mGBA
Lua bridge and the automation probe ABI. New route FSM work belongs in the
Scala automation implementation in the ouroboros repository, where route logic
can be governed, typed, and driven by probe/menu/event flags instead of timing
assumptions.
"""

from __future__ import annotations

import argparse
import faulthandler
import json
import os
import signal
import shlex
import shutil
import socket
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any, Callable, Iterable

DEFAULT_PORT = 46510
SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent.parent
DEFAULT_OUTPUT_DIR = REPO_ROOT / "build" / "mgba_lua_spike"
DEFAULT_BRIDGE = SCRIPT_DIR / "mgba_lua_bridge.lua"
DEFAULT_ROM = REPO_ROOT / "pokeemerald.gba"
DEFAULT_SYM = REPO_ROOT / "pokeemerald.sym"
DEFAULT_STATE_SCREENSHOT_SOURCE = SCRIPT_DIR / "mgba_state_screenshot.c"

try:
    faulthandler.register(signal.SIGUSR1, all_threads=True)
except (AttributeError, RuntimeError, ValueError):
    pass

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
STARTER_SELECTION_TREECKO = 0
STARTER_SELECTION_TORCHIC = 1
STARTER_SELECTION_MUDKIP = 2
DEFAULT_STORY_STARTER_SELECTION = STARTER_SELECTION_MUDKIP
STORY_PC_RARE_CANDY_WITHDRAW_COUNT = 25
STORY_STARTER_RARE_CANDY_COUNT = 6

MAP_GROUP_OUTSIDE = 0
MAP_GROUP_LITTLEROOT_BUILDINGS = 1
MAP_GROUP_OLDALE_BUILDINGS = 2
MAP_NUM_LITTLEROOT_TOWN = 9
MAP_NUM_OLDALE_TOWN = 10
MAP_NUM_ROUTE101 = 16
MAP_NUM_ROUTE103 = 18
MAP_NUM_BIRCH_LAB = 4
MAP_NUM_OLDALE_MART = 4

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
ITEM_MASTER_BALL = 4
ITEM_RARE_CANDY = 102

PC_MENU_NONE = 0
PC_MENU_TOP = 1
PC_MENU_ITEM_STORAGE = 2
PC_MENU_ITEM_LIST = 3
PC_MENU_QUANTITY = 4
PC_MENU_WITHDRAW_MESSAGE = 5

FIELD_READY_MAX_STALE_FRAMES = 8

BATTLE_MENU_NONE = 0
BATTLE_MENU_ACTION = 1
BATTLE_MENU_MOVE = 2

PARTY_MENU_NONE = 0
PARTY_MENU_CHOOSE = 1
PARTY_MENU_ACTION = 2

MENU_ACTION_POKEMON = 1
MENU_ACTION_BAG = 2
START_MENU_NONE = 0
START_MENU_READY = 1

POCKET_ITEMS = 0
POCKET_POKE_BALLS = 1
BAG_MENU_NONE = 0
BAG_MENU_ITEM_LIST = 1
BAG_MENU_CONTEXT = 2

SCRIPT_MENU_NONE = 0
SCRIPT_MENU_YESNO = 1
SCRIPT_MENU_MULTICHOICE = 2
SCRIPT_MENU_MULTICHOICE_GRID = 3
SCRIPT_MENU_RESULT_PENDING = 0xFF

SHOP_MENU_NONE = 0
SHOP_MENU_TOP = 1
SHOP_MENU_BUY_LIST = 2
SHOP_MENU_QUANTITY = 3
SHOP_MENU_CONFIRM = 4
SHOP_MENU_MESSAGE = 5

AUTOMATION_PROBE_MAGIC = 0x41505242
AUTOMATION_PROBE_VERSION = 7
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
    ("frontX", 1),
    ("frontY", 1),
    ("fieldInputReady", 1),
    ("fieldMovementReady", 1),
    ("fieldTextReady", 1),
    ("fieldScriptEnabled", 1),
    ("fieldControlsLocked", 1),
    ("fieldPlayerMoving", 1),
    ("fieldPaletteFadeActive", 1),
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
    ("pcRareCandyCount", 1),
    ("pcRareCandySlot", 1),
    ("pcMasterBallCount", 1),
    ("pcMasterBallSlot", 1),
    ("pcUsedItemSlots", 1),
    ("pcMenuState", 1),
    ("pcMenuCursor", 1),
    ("pcItemCursor", 1),
    ("pcItemId", 1),
    ("pcItemQuantity", 1),
    ("bagPokeBallCount", 1),
    ("bagRareCandyCount", 1),
    ("bagMasterBallCount", 1),
    ("playerBattleBattler", 1),
    ("playerBattlePartyIndex", 1),
    ("playerBattleActionCursor", 1),
    ("playerBattleMoveCursor", 1),
    ("playerBattleController", 1),
    ("battleControllerExecFlags", 1),
    ("battleMenuState", 1),
    ("battleMenuFrame", 1),
    ("battleMenuCursor", 1),
    ("battleMenuArg0", 1),
    ("partyMenuState", 1),
    ("partyMenuFrame", 1),
    ("partyMenuType", 1),
    ("partyMenuAction", 1),
    ("partyMenuCursor", 1),
    ("partyMenuCursor2", 1),
    ("startMenuState", 1),
    ("startMenuFrame", 1),
    ("startMenuCursor", 1),
    ("startMenuAction", 1),
    ("startMenuCount", 1),
    ("bagMenuState", 1),
    ("bagMenuFrame", 1),
    ("bagMenuLocation", 1),
    ("bagMenuPocket", 1),
    ("bagMenuCursor", 1),
    ("bagMenuItemId", 1),
    ("bagMenuItemQuantity", 1),
    ("bagMenuContextCursor", 1),
    ("scriptMenuState", 1),
    ("scriptMenuFrame", 1),
    ("scriptMenuCursor", 1),
    ("scriptMenuResult", 1),
    ("scriptMenuId", 1),
    ("shopMenuState", 1),
    ("shopMenuFrame", 1),
    ("shopMenuCursor", 1),
    ("shopMenuItemId", 1),
    ("shopMenuItemQuantity", 1),
    ("shopMenuItemPrice", 1),
    ("shopMenuMoney", 1),
    ("abilityPopupSequence", 1),
    ("abilityPopupFrame", 1),
    ("abilityPopupBattler", 1),
    ("abilityPopupSide", 1),
    ("abilityPopupPosition", 1),
    ("abilityPopupPartyIndex", 1),
    ("abilityPopupAbility", 1),
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


def wait_for_probe_condition(
    client: BridgeClient,
    probe_address: int,
    predicate: Callable[[dict[str, Any]], bool],
    timeout: float,
    label: str,
    poll_frames: int = 5,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        last_probe = read_probe(client, probe_address)
        validate_probe(last_probe)
        if predicate(last_probe):
            return last_probe
        client.request(f"run_frames {poll_frames}")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def probe_frame_delta(probe: dict[str, Any], frame_key: str) -> int:
    return (int(probe.get("frame", 0)) - int(probe.get(frame_key, 0))) & 0xFFFFFFFF


def probe_state_is_fresh(probe: dict[str, Any], frame_key: str, max_stale_frames: int = FIELD_READY_MAX_STALE_FRAMES) -> bool:
    return probe_frame_delta(probe, frame_key) <= max_stale_frames


def probe_on_map(probe: dict[str, Any], map_group: int, map_num: int) -> bool:
    return probe.get("mapGroup") == map_group and probe.get("mapNum") == map_num


def wait_for_probe_field_movement_ready(
    client: BridgeClient,
    probe_address: int,
    map_group: int | None,
    map_num: int | None,
    timeout: float,
    label: str,
    run_incidental_battles: bool = False,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        map_ok = map_group is None or map_num is None or probe_on_map(probe, map_group, map_num)
        if map_ok and probe.get("fieldMovementReady") == 1:
            return probe
        if run_incidental_battles and probe.get("inBattle") == 1:
            run_from_battle(client, probe_address, max(1.0, deadline - time.monotonic()), f"{label} incidental battle")
            idle_polls = 0
            continue
        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
            idle_polls = 0
        else:
            client.request("run_frames 5")
            if map_ok and (probe.get("fieldScriptEnabled") == 1 or probe.get("fieldControlsLocked") == 1):
                idle_polls += 1
                if idle_polls >= 12:
                    tap(client, "A", frames=4, settle_frames=6)
                    idle_polls = 0

    raise RuntimeError(f"{label} timed out: {last_probe}")


def wait_for_script_menu_state(
    client: BridgeClient,
    probe_address: int,
    state: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    return wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: (
            probe.get("scriptMenuState") == state
            and probe_state_is_fresh(probe, "scriptMenuFrame")
            and probe.get("scriptMenuResult") == SCRIPT_MENU_RESULT_PENDING
        ),
        timeout,
        label,
    )


def choose_script_yesno(
    client: BridgeClient,
    probe_address: int,
    want_yes: bool,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    target_cursor = 0 if want_yes else 1
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_script_menu_state(
            client,
            probe_address,
            SCRIPT_MENU_YESNO,
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_probe = probe
        cursor = probe.get("scriptMenuCursor")
        if cursor == target_cursor:
            client.request("run_frames 8")
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        tap(client, "UP" if target_cursor == 0 else "DOWN", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def wait_for_battle_menu_state(
    client: BridgeClient,
    probe_address: int,
    state: int,
    timeout: float,
    label: str,
    allow_post_battle_script: bool = False,
) -> dict[str, Any]:
    return wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: (
            (
                probe.get("inBattle") == 1
                and probe.get("battleMenuState") == state
                and probe_state_is_fresh(probe, "battleMenuFrame")
            )
            or (
                allow_post_battle_script
                and probe.get("inBattle") == 0
                and probe.get("scriptMenuState") != SCRIPT_MENU_NONE
                and probe_state_is_fresh(probe, "scriptMenuFrame", max_stale_frames=30)
            )
        ),
        timeout,
        label,
        poll_frames=2,
    )


def wait_for_battle_action_or_probe_condition(
    client: BridgeClient,
    probe_address: int,
    predicate: Callable[[dict[str, Any]], bool],
    timeout: float,
    label: str,
    allow_post_battle_script: bool = False,
) -> tuple[str, dict[str, Any]]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if predicate(probe):
            return "predicate", probe
        if (
            allow_post_battle_script
            and probe.get("inBattle") == 0
            and probe.get("scriptMenuState") != SCRIPT_MENU_NONE
            and probe_state_is_fresh(probe, "scriptMenuFrame", max_stale_frames=30)
        ):
            append_event(
                client.event_log,
                {
                    "event": "post_battle_script_handoff",
                    "label": label,
                    "probeFrame": probe.get("frame"),
                    "scriptMenuState": probe.get("scriptMenuState"),
                    "scriptMenuResult": probe.get("scriptMenuResult"),
                    "mapGroup": probe.get("mapGroup"),
                    "mapNum": probe.get("mapNum"),
                },
            )
            return "post_battle_script", probe
        if (
            probe.get("inBattle") == 1
            and probe.get("battleMenuState") == BATTLE_MENU_ACTION
            and probe_state_is_fresh(probe, "battleMenuFrame")
        ):
            return "action", probe
        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
            idle_polls = 0
        else:
            client.request("run_frames 5")
            idle_polls += 1
            if idle_polls >= 6:
                tap(client, "A", frames=4, settle_frames=6)
                idle_polls = 0

    raise RuntimeError(f"{label} timed out: {last_probe}")


def set_battle_action_cursor(
    client: BridgeClient,
    probe_address: int,
    target_cursor: int,
    timeout: float,
    label: str,
    allow_post_battle_script: bool = False,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_battle_menu_state(
            client,
            probe_address,
            BATTLE_MENU_ACTION,
            max(1.0, deadline - time.monotonic()),
            label,
            allow_post_battle_script=allow_post_battle_script,
        )
        last_probe = probe
        if probe.get("inBattle") == 0:
            return probe
        cursor = int(probe.get("battleMenuCursor", 0))
        if cursor == target_cursor:
            return probe
        if (cursor & 1) and not (target_cursor & 1):
            tap(client, "LEFT", frames=8, settle_frames=10)
        elif not (cursor & 1) and (target_cursor & 1):
            tap(client, "RIGHT", frames=8, settle_frames=10)
        elif (cursor & 2) and not (target_cursor & 2):
            tap(client, "UP", frames=8, settle_frames=10)
        elif not (cursor & 2) and (target_cursor & 2):
            tap(client, "DOWN", frames=8, settle_frames=10)

    raise RuntimeError(f"{label} cursor timed out: {last_probe}")


def set_battle_move_cursor_zero(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
    label: str,
    allow_post_battle_script: bool = False,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_battle_menu_state(
            client,
            probe_address,
            BATTLE_MENU_MOVE,
            max(1.0, deadline - time.monotonic()),
            label,
            allow_post_battle_script=allow_post_battle_script,
        )
        last_probe = probe
        if probe.get("inBattle") == 0:
            return probe
        cursor = int(probe.get("battleMenuCursor", 0))
        if cursor == 0:
            return probe
        if cursor & 1:
            tap(client, "LEFT", frames=8, settle_frames=10)
        elif cursor & 2:
            tap(client, "UP", frames=8, settle_frames=10)

    raise RuntimeError(f"{label} cursor timed out: {last_probe}")


def choose_first_battle_move(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
    label: str,
    allow_post_battle_script: bool = False,
) -> dict[str, Any]:
    action_probe = set_battle_action_cursor(
        client,
        probe_address,
        0,
        timeout,
        f"{label} action cursor",
        allow_post_battle_script=allow_post_battle_script,
    )
    if action_probe.get("inBattle") == 0:
        return action_probe
    tap(client, "A", frames=8, settle_frames=12)
    move_probe = set_battle_move_cursor_zero(
        client,
        probe_address,
        timeout,
        f"{label} move cursor",
        allow_post_battle_script=allow_post_battle_script,
    )
    if move_probe.get("inBattle") == 0:
        return move_probe
    tap(client, "A", frames=8, settle_frames=18)
    return move_probe


def run_from_battle(client: BridgeClient, probe_address: int, timeout: float, label: str) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if probe.get("inBattle") == 0 and probe.get("fieldMovementReady") == 1:
            return probe

        if probe.get("inBattle") == 1 and probe.get("battleMenuState") == BATTLE_MENU_ACTION:
            cursor = int(probe.get("battleMenuCursor", 0))
            if cursor == 3:
                tap(client, "A", frames=8, settle_frames=18)
            elif cursor & 1:
                tap(client, "DOWN", frames=8, settle_frames=10)
            elif cursor & 2:
                tap(client, "RIGHT", frames=8, settle_frames=10)
            else:
                tap(client, "RIGHT", frames=8, settle_frames=10)
            idle_polls = 0
            continue

        if probe.get("inBattle") == 1:
            client.request("run_frames 5")
            idle_polls += 1
            if idle_polls >= 6:
                tap(client, "A", frames=4, settle_frames=6)
                idle_polls = 0
            continue

        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def win_battle_with_first_move_until(
    client: BridgeClient,
    probe_address: int,
    predicate: Callable[[dict[str, Any]], bool],
    timeout: float,
    label: str,
    max_turns: int = 40,
    allow_post_battle_script: bool = False,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    for turn in range(max_turns):
        state, probe = wait_for_battle_action_or_probe_condition(
            client,
            probe_address,
            predicate,
            max(1.0, deadline - time.monotonic()),
            f"{label} turn {turn + 1}",
            allow_post_battle_script=allow_post_battle_script,
        )
        last_probe = probe
        if state in {"predicate", "post_battle_script"}:
            return probe
        choose_first_battle_move(
            client,
            probe_address,
            max(1.0, deadline - time.monotonic()),
            f"{label} turn {turn + 1}",
            allow_post_battle_script=allow_post_battle_script,
        )

    raise RuntimeError(f"{label} exceeded {max_turns} turns: {last_probe}")


def advance_story_dialogue(
    client: BridgeClient,
    probe_address: int,
    predicate: Callable[[dict[str, Any]], bool],
    timeout: float,
    label: str,
    yesno_choices: list[bool] | None = None,
    default_yesno_choice: bool | None = None,
    run_battles_with_first_move: bool = False,
    stable_frames: int = 0,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    choices = list(yesno_choices or [])
    active_yesno = False
    last_probe: dict[str, Any] | None = None
    idle_polls = 0
    last_yesno_action_frame = 0
    stable_start_frame: int | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if predicate(probe):
            if stable_frames <= 0:
                return probe
            probe_frame = int(probe.get("frame", 0))
            if stable_start_frame is None:
                stable_start_frame = probe_frame
            elif probe_frame - stable_start_frame >= stable_frames:
                return probe
            client.request("run_frames 5")
            continue
        stable_start_frame = None
        if (
            run_battles_with_first_move
            and probe.get("inBattle") == 1
            and probe_state_is_fresh(probe, "battleMenuFrame")
        ):
            menu_state = probe.get("battleMenuState")
            if menu_state not in {BATTLE_MENU_ACTION, BATTLE_MENU_MOVE}:
                client.request("run_frames 3")
                continue
            append_event(
                client.event_log,
                {
                    "event": "story_battle_first_move",
                    "label": label,
                    "probeFrame": probe.get("frame"),
                    "enemy0Species": probe.get("enemy0Species"),
                    "battleMenuState": menu_state,
                    "battleMenuCursor": probe.get("battleMenuCursor"),
                },
            )
            cursor = int(probe.get("battleMenuCursor", 0))
            if menu_state == BATTLE_MENU_ACTION:
                if cursor == 0:
                    tap(client, "A", frames=8, settle_frames=12)
                elif cursor & 1:
                    tap(client, "LEFT", frames=8, settle_frames=10)
                else:
                    tap(client, "UP", frames=8, settle_frames=10)
            elif menu_state == BATTLE_MENU_MOVE:
                if cursor == 0:
                    tap(client, "A", frames=8, settle_frames=18)
                elif cursor & 1:
                    tap(client, "LEFT", frames=8, settle_frames=10)
                else:
                    tap(client, "UP", frames=8, settle_frames=10)
            active_yesno = False
            idle_polls = 0
            continue
        yesno_active = (
            probe.get("scriptMenuState") == SCRIPT_MENU_YESNO
            and probe_state_is_fresh(probe, "scriptMenuFrame")
            and probe.get("scriptMenuResult") == SCRIPT_MENU_RESULT_PENDING
        )
        if yesno_active:
            append_event(
                client.event_log,
                {
                    "event": "script_yesno_seen",
                    "label": label,
                    "probeFrame": probe.get("frame"),
                    "cursor": probe.get("scriptMenuCursor"),
                    "remainingPlannedChoices": len(choices),
                    "hasDefaultChoice": default_yesno_choice is not None,
                },
            )
            used_default_choice = False
            if choices:
                choice = choices.pop(0)
            elif default_yesno_choice is not None:
                choice = default_yesno_choice
                used_default_choice = True
            else:
                raise RuntimeError(f"{label} saw unexpected yes/no prompt: {probe}")
            append_event(
                client.event_log,
                {
                    "event": "script_yesno_choice",
                    "label": label,
                    "choice": choice,
                    "remainingPlannedChoices": len(choices),
                    "usedDefaultChoice": used_default_choice,
                    "probeFrame": probe.get("frame"),
                    "cursor": probe.get("scriptMenuCursor"),
                },
            )
            choose_script_yesno(client, probe_address, choice, max(1.0, deadline - time.monotonic()), f"{label} yesno")
            active_yesno = True
            last_yesno_action_frame = int(probe.get("frame", 0))
            stable_start_frame = None
        elif not yesno_active:
            active_yesno = False
            if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
                tap(client, "A", frames=8, settle_frames=12)
                idle_polls = 0
            else:
                client.request("run_frames 5")
                idle_polls += 1
                if idle_polls >= 12:
                    tap(client, "A", frames=4, settle_frames=6)
                    idle_polls = 0
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def probe_move_to_position(
    client: BridgeClient,
    probe_address: int,
    map_group: int,
    map_num: int,
    target_x: int,
    target_y: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_probe_field_movement_ready(
            client,
            probe_address,
            map_group,
            map_num,
            max(1.0, deadline - time.monotonic()),
            label,
            run_incidental_battles=True,
        )
        last_probe = probe
        player_x = probe.get("playerX")
        player_y = probe.get("playerY")
        if player_x == target_x and player_y == target_y:
            return probe
        if player_x is None or player_y is None:
            raise RuntimeError(f"{label} missing probe position: {probe}")
        if player_x < target_x:
            move_tap(client, "RIGHT")
        elif player_x > target_x:
            move_tap(client, "LEFT")
        elif player_y < target_y:
            move_tap(client, "DOWN")
        elif player_y > target_y:
            move_tap(client, "UP")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def probe_follow_waypoints(
    client: BridgeClient,
    probe_address: int,
    map_group: int,
    map_num: int,
    waypoints: Iterable[tuple[int, int, str]],
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    for target_x, target_y, waypoint_label in waypoints:
        last_probe = probe_move_to_position(
            client,
            probe_address,
            map_group,
            map_num,
            target_x,
            target_y,
            max(1.0, deadline - time.monotonic()),
            f"{label}: {waypoint_label}",
        )

    if last_probe is None:
        return wait_for_probe_field_movement_ready(
            client,
            probe_address,
            map_group,
            map_num,
            max(1.0, deadline - time.monotonic()),
            label,
            run_incidental_battles=True,
        )
    return last_probe


def probe_face_tile(
    client: BridgeClient,
    probe_address: int,
    map_group: int,
    map_num: int,
    key: str,
    front_x: int,
    front_y: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    key = key.upper()
    expected_facing = KEY_FACING[key]
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_probe_field_movement_ready(
            client,
            probe_address,
            map_group,
            map_num,
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_probe = probe
        if (
            probe.get("playerFacing") == expected_facing
            and probe.get("frontX") == front_x
            and probe.get("frontY") == front_y
        ):
            return probe
        move_tap(client, key)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def probe_move_until_map(
    client: BridgeClient,
    probe_address: int,
    key: str,
    target_map_group: int,
    target_map_num: int,
    max_presses: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    presses = 0
    idle_polls = 0

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if probe_on_map(probe, target_map_group, target_map_num):
            return wait_for_probe_field_movement_ready(
                client,
                probe_address,
                target_map_group,
                target_map_num,
                max(1.0, deadline - time.monotonic()),
                label,
                run_incidental_battles=True,
            )
        if (
            probe.get("inBattle") == 1
            and probe.get("battleMenuState") == BATTLE_MENU_ACTION
            and probe_state_is_fresh(probe, "battleMenuFrame")
        ):
            run_from_battle(client, probe_address, max(1.0, deadline - time.monotonic()), f"{label} incidental battle")
            idle_polls = 0
        if probe.get("fieldMovementReady") == 1:
            if presses >= max_presses:
                raise RuntimeError(f"{label} exceeded max presses: {probe}")
            move_tap(client, key)
            presses += 1
            idle_polls = 0
        elif probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
            idle_polls = 0
        else:
            client.request("run_frames 5")
            if probe.get("fieldScriptEnabled") == 1 or probe.get("fieldControlsLocked") == 1:
                idle_polls += 1
                if idle_polls >= 12:
                    tap(client, "A", frames=4, settle_frames=6)
                    idle_polls = 0

    raise RuntimeError(f"{label} timed out: {last_probe}")


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


def choose_battle_party_cursor(client: BridgeClient, timeout: float = 15.0) -> dict[str, Any]:
    criteria = {
        "stageId": STAGE_BATTLE_SUMMARY_REPRO,
        "substageId": REPRO_SUBSTAGE_ACTION_MENU,
        "flags": 2,
    }
    deadline = time.monotonic() + timeout
    last_beacon: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        beacon = client.request("read_beacon")
        last_beacon = beacon
        if semantic_beacon_matches(beacon, criteria, "menuReady", "battle party cursor"):
            append_event(
                client.event_log,
                {
                    "event": "semantic_gate",
                    "label": "battle party cursor",
                    "semanticKey": "menuReady",
                    "beacon": beacon,
                },
            )
            return beacon

        if semantic_beacon_matches(
            beacon,
            {
                "stageId": STAGE_BATTLE_SUMMARY_REPRO,
                "substageId": REPRO_SUBSTAGE_ACTION_MENU,
            },
            "menuReady",
            "battle action menu",
        ):
            tap(client, "DOWN", frames=8, settle_frames=12)
        else:
            client.request("run_frames 10")

    raise RouteTimeout("battle party cursor", last_beacon)


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


def capture_desktop_screenshot(path: Path) -> dict[str, Any]:
    path = path.resolve()
    path.parent.mkdir(parents=True, exist_ok=True)
    try:
        path.unlink()
    except FileNotFoundError:
        pass
    win_path = str(path)
    try:
        converted = subprocess.run(
            ["wslpath", "-w", str(path)],
            check=True,
            capture_output=True,
            text=True,
        )
        win_path = converted.stdout.strip() or win_path
    except Exception:
        pass

    ps_out = win_path.replace("`", "``").replace('"', '`"')
    script = rf"""
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
$out = "{ps_out}"
$bounds = [System.Windows.Forms.SystemInformation]::VirtualScreen
$bmp = New-Object System.Drawing.Bitmap($bounds.Width, $bounds.Height)
$gfx = [System.Drawing.Graphics]::FromImage($bmp)
$gfx.CopyFromScreen($bounds.Left, $bounds.Top, 0, 0, $bounds.Size)
$gfx.Dispose()
$bmp.Save($out, [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()
"""
    try:
        completed = subprocess.run(
            ["powershell.exe", "-NoProfile", "-NonInteractive", "-Command", script],
            capture_output=True,
            text=True,
            timeout=20.0,
        )
        status = png_status(path)
        status.update(
            {
                "method": "powershell-virtual-screen",
                "ok": completed.returncode == 0 and status["validPng"],
                "returnCode": completed.returncode,
                "stdout": completed.stdout,
                "stderr": completed.stderr,
                "windowsPath": win_path,
            }
        )
        return status
    except Exception as exc:  # noqa: BLE001 - preserve screenshot failure in run_result.json.
        status = png_status(path)
        status.update(
            {
                "method": "powershell-virtual-screen",
                "ok": False,
                "error": f"{type(exc).__name__}: {exc}",
                "windowsPath": win_path,
            }
        )
        return status


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
    tap(client, key, frames=16, settle_frames=max(settle_frames, 36))


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
            and (
                (beacon.get("flags", 0) & 4) != 0
                or beacon.get("textReady") == 1
                or beacon.get("scriptWaitKind") == SCRIPT_STEP_WAIT_BUTTON
            )
        ):
            tap(client, "A", frames=8, settle_frames=18)
        else:
            client.request("run_frames 10")

    raise RouteTimeout(label, last_beacon)


def wait_for_semantic_movement_ready_or_advance_text(
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
        if (
            beacon_matches(beacon, criteria)
            and beacon.get("semanticFound")
            and beacon.get("movementReady") == 1
        ):
            return beacon
        if (
            beacon.get("found")
            and beacon.get("stageId") == criteria.get("stageId")
            and beacon.get("inputReady") == 1
            and (beacon.get("flags", 0) & 4) != 0
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


def drive_to_bedroom_setup_ready(client: BridgeClient, truck_timeout: float, setup_timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()
    truck = drive_to_truck(client, truck_timeout)
    if not truck.get("ok"):
        return truck

    deadline = time.monotonic() + setup_timeout

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

    route_phase(client, "bedroom_setup")
    move_until_substage(
        client,
        "UP",
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        5,
        remaining_timeout(deadline, 60.0, "bedroom setup ready", client),
        "bedroom setup ready",
    )
    bedroom = wait_for_beacon(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 5, MAP_LITTLEROOT),
        remaining_timeout(deadline, 60.0, "bedroom ready", client),
        "bedroom ready",
    )

    return {
        "ok": True,
        "target": "BEDROOM_SETUP_READY",
        "beacon": bedroom,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


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


def drive_from_bedroom_setup_to_starter_choose(client: BridgeClient, starter_timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()
    deadline = time.monotonic() + starter_timeout

    route_phase(client, "clock_setup")
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


def drive_to_starter_choose(client: BridgeClient, truck_timeout: float, starter_timeout: float) -> dict[str, Any]:
    route_start = time.monotonic()
    bedroom = drive_to_bedroom_setup_ready(client, truck_timeout, starter_timeout)
    if not bedroom.get("ok"):
        return bedroom

    starter = drive_from_bedroom_setup_to_starter_choose(client, starter_timeout)
    starter["elapsedSeconds"] = round(time.monotonic() - route_start, 3)
    return starter


def choose_starter_selection(
    client: BridgeClient,
    current_beacon: dict[str, Any],
    target_selection: int,
    timeout: float,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    starter = current_beacon

    while int(starter.get("starterSelection", STARTER_SELECTION_TORCHIC)) != target_selection:
        current_selection = int(starter.get("starterSelection", STARTER_SELECTION_TORCHIC))
        if current_selection < target_selection:
            tap(client, "RIGHT", frames=8, settle_frames=12)
            next_selection = current_selection + 1
        else:
            tap(client, "LEFT", frames=8, settle_frames=12)
            next_selection = current_selection - 1
        starter = wait_for_beacon(
            client,
            {
                "stageId": STAGE_STARTER_CHOOSE_READY,
                "mapKind": MAP_STARTER_SELECTION,
                "gender": GENDER_FEMALE,
                "nameLen": 1,
                "nameChar0": NAME_CHAR_A,
                "starterSelection": next_selection,
                "inputReady": 1,
            },
            max(1.0, deadline - time.monotonic()),
            f"starter selection {target_selection}",
        )

    return starter


def confirm_starter_from_choose(
    client: BridgeClient,
    starter: dict[str, Any],
    starter_selection: int = DEFAULT_STORY_STARTER_SELECTION,
    timeout: float = 120.0,
) -> dict[str, Any]:
    route_start = time.monotonic()
    starter_beacon = choose_starter_selection(
        client,
        starter["beacon"],
        starter_selection,
        timeout,
    )
    tap(client, "A")
    confirm = wait_for_semantic_beacon(
        client,
        {
            "stageId": STAGE_STARTER_CONFIRM_PROMPT,
            "mapKind": MAP_STARTER_SELECTION,
            "gender": GENDER_FEMALE,
            "nameLen": 1,
            "nameChar0": NAME_CHAR_A,
            "starterSelection": starter_selection,
            "inputReady": 1,
        },
        "menuReady",
        timeout,
        "starter confirm prompt",
    )

    return {
        "ok": True,
        "target": "STARTER_CONFIRM_PROMPT",
        "starterSelection": starter_selection,
        "starterChooseBeacon": starter_beacon,
        "beacon": confirm,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_to_starter_confirm(
    client: BridgeClient,
    truck_timeout: float,
    starter_timeout: float,
    starter_selection: int = DEFAULT_STORY_STARTER_SELECTION,
) -> dict[str, Any]:
    route_start = time.monotonic()
    starter = drive_to_starter_choose(client, truck_timeout, starter_timeout)
    if not starter["ok"]:
        return starter

    confirm = confirm_starter_from_choose(client, starter, starter_selection, starter_timeout)
    confirm["elapsedSeconds"] = round(time.monotonic() - route_start, 3)
    return confirm


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


def drive_story_pokeballs_smoke(client: BridgeClient, args: argparse.Namespace) -> dict[str, Any]:
    route_start = time.monotonic()
    probe_address = resolve_probe_address(args)
    trace: list[dict[str, Any]] = []

    trace.append(asdict(RouteNode("EarlyGame.BedroomPC", "Reach bedroom setup and withdraw route Rare Candies before setting the clock")))
    bedroom = drive_to_bedroom_setup_ready(client, args.truck_timeout, args.objective_timeout)
    if not bedroom.get("ok"):
        return bedroom

    withdrawal = withdraw_rare_candies_from_bedroom_pc(
        client,
        probe_address,
        STORY_PC_RARE_CANDY_WITHDRAW_COUNT,
        args.objective_timeout,
    )
    trace.append({
        "id": "Inventory.RareCandiesWithdrawnFromPC",
        "accepted": True,
        "quantity": STORY_PC_RARE_CANDY_WITHDRAW_COUNT,
        "beforePcRareCandyCount": withdrawal["beforeProbe"].get("pcRareCandyCount"),
        "afterPcRareCandyCount": withdrawal["afterProbe"].get("pcRareCandyCount"),
        "beforeBagRareCandyCount": withdrawal["beforeProbe"].get("bagRareCandyCount"),
        "afterBagRareCandyCount": withdrawal["afterProbe"].get("bagRareCandyCount"),
    })

    trace.append(asdict(RouteNode("EarlyGame.StarterConfirm", "Resume from bedroom setup and reach the starter confirmation prompt")))
    starter = drive_from_bedroom_setup_to_starter_choose(client, args.starter_timeout)
    if not starter.get("ok"):
        return starter
    confirm = confirm_starter_from_choose(client, starter, DEFAULT_STORY_STARTER_SELECTION, args.starter_timeout)
    if not confirm.get("ok"):
        return confirm

    trace.append(asdict(RouteNode("Story.InitialRescueBattle", "Confirm the starter and win the rescue battle through probe-gated story/battle handling")))
    tap(client, "A", frames=8, settle_frames=24)
    trace.append(asdict(RouteNode("Story.StarterLabAcknowledged", "Decline nickname, agree to see rival, and regain lab field control")))
    lab_ready = advance_story_dialogue(
        client,
        probe_address,
        lambda probe: (
            probe_on_map(probe, MAP_GROUP_LITTLEROOT_BUILDINGS, MAP_NUM_BIRCH_LAB)
            and probe.get("playerPartyCount") == 1
            and probe.get("fieldMovementReady") == 1
        ),
        args.objective_timeout,
        "starter lab acknowledgement",
        yesno_choices=[False, True],
        default_yesno_choice=True,
        run_battles_with_first_move=True,
    )
    lab_ready = advance_story_dialogue(
        client,
        probe_address,
        lambda probe: (
            probe_on_map(probe, MAP_GROUP_LITTLEROOT_BUILDINGS, MAP_NUM_BIRCH_LAB)
            and probe.get("playerPartyCount") == 1
            and probe.get("fieldMovementReady") == 1
        ),
        args.objective_timeout,
        "starter lab acknowledgement settle",
        default_yesno_choice=True,
        stable_frames=90,
    )
    post_rescue = lab_ready

    trace.append(asdict(RouteNode("Inventory.StarterOverleveled", "Use PC-withdrawn Rare Candies on the starter before the Route 103 rival battle")))
    starter_candy_uses = use_rare_candies_on_party_slot(
        client,
        probe_address,
        0,
        STORY_STARTER_RARE_CANDY_COUNT,
        args.objective_timeout,
        "starter Rare Candy",
    )
    starter_leveled_probe = read_probe(client, probe_address)
    validate_probe(starter_leveled_probe)
    trace.append({
        "id": "Inventory.StarterRareCandiesUsed",
        "accepted": starter_leveled_probe.get("partyLevel", [0])[0] >= 5 + STORY_STARTER_RARE_CANDY_COUNT,
        "quantity": STORY_STARTER_RARE_CANDY_COUNT,
        "starterSpecies": starter_leveled_probe.get("partySpecies", [0])[0],
        "starterLevel": starter_leveled_probe.get("partyLevel", [0])[0],
        "bagRareCandyCount": starter_leveled_probe.get("bagRareCandyCount"),
    })

    trace.append(asdict(RouteNode("Story.LeaveLab", "Exit Birch's lab using generic probe coordinates")))
    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_LITTLEROOT_BUILDINGS,
        MAP_NUM_BIRCH_LAB,
        6,
        12,
        args.objective_timeout,
        "Birch lab exit tile",
    )
    town_ready = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        4,
        args.objective_timeout,
        "Littleroot after lab exit",
    )

    trace.append(asdict(RouteNode("Story.Route103Rival", "Walk to Route 103, battle the rival, and regain field control")))
    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        11,
        2,
        args.objective_timeout,
        "Littleroot north route approach",
    )
    route101_ready = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        4,
        args.objective_timeout,
        "Route 101 south entry",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        [
            (10, 14, "south lane"),
            (7, 14, "west below rescue blocker"),
            (7, 10, "west route bend"),
            (13, 10, "east route bend"),
            (13, 9, "north turn"),
            (15, 9, "east grass edge"),
            (15, 2, "north grass edge"),
            (11, 2, "top route bend"),
            (11, 0, "top exit lane"),
            (10, 0, "north exit approach"),
        ],
        args.objective_timeout,
        "Route 101 outbound collision-grid path",
    )
    oldale_ready = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        4,
        args.objective_timeout,
        "Oldale south entry",
    )
    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        10,
        0,
        args.objective_timeout,
        "Oldale north exit approach",
    )
    route103_ready = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE103,
        4,
        args.objective_timeout,
        "Route 103 south entry",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE103,
        [
            (11, 13, "south connection lane"),
            (14, 13, "south east bend"),
            (14, 7, "east north lane"),
            (12, 7, "west turn"),
            (12, 6, "north turn"),
            (5, 6, "west lane"),
            (5, 4, "northwest rival lane"),
            (10, 4, "rival approach"),
        ],
        args.objective_timeout,
        "Route 103 rival collision-grid path",
    )
    probe_face_tile(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE103,
        "UP",
        10,
        3,
        args.objective_timeout,
        "Route 103 rival facing",
    )
    tap(client, "A", frames=8, settle_frames=18)
    route103_after_rival = advance_story_dialogue(
        client,
        probe_address,
        lambda probe: (
            probe_on_map(probe, MAP_GROUP_OUTSIDE, MAP_NUM_ROUTE103)
            and probe.get("fieldMovementReady") == 1
        ),
        args.objective_timeout,
        "Route 103 rival exit",
        run_battles_with_first_move=True,
    )
    post_rival_battle = route103_after_rival

    trace.append(asdict(RouteNode("Story.ReturnForPokedexAndPokeBalls", "Return to Birch's lab and accept the Pokedex/Poke Ball story rewards")))
    oldale_return = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        40,
        args.objective_timeout,
        "Oldale return from Route 103",
    )
    advance_story_dialogue(
        client,
        probe_address,
        lambda probe: (
            probe_on_map(probe, MAP_GROUP_OUTSIDE, MAP_NUM_OLDALE_TOWN)
            and probe.get("fieldMovementReady") == 1
        ),
        args.objective_timeout,
        "Oldale rival return prompt",
    )
    route101_return = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        40,
        args.objective_timeout,
        "Route 101 return from Oldale",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        [
            (10, 5, "north lane"),
            (13, 5, "east bend"),
            (13, 6, "south turn"),
            (15, 6, "east lane"),
            (15, 12, "south grass edge"),
            (7, 12, "west route bend"),
            (7, 17, "south below rescue blocker"),
            (10, 17, "central lower route"),
            (10, 19, "south exit approach"),
        ],
        args.objective_timeout,
        "Route 101 return collision-grid path",
    )
    littleroot_return = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        40,
        args.objective_timeout,
        "Littleroot return from Route 101",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        [
            (10, 17, "north road southbound"),
            (7, 17, "lower lab lane"),
        ],
        args.objective_timeout,
        "Littleroot north-to-lab collision-grid path",
    )
    lab_return = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_LITTLEROOT_BUILDINGS,
        MAP_NUM_BIRCH_LAB,
        4,
        args.objective_timeout,
        "Birch lab return",
    )
    pokeballs_ready = advance_story_dialogue(
        client,
        probe_address,
        lambda probe: (
            probe_on_map(probe, MAP_GROUP_LITTLEROOT_BUILDINGS, MAP_NUM_BIRCH_LAB)
            and probe.get("bagPokeBallCount", 0) >= 5
            and probe.get("fieldMovementReady") == 1
        ),
        args.objective_timeout,
        "Pokedex and Poke Ball reward",
    )

    final_probe = read_probe(client, probe_address)
    validate_probe(final_probe)
    ok = (
        final_probe.get("playerPartyCount") == 1
        and final_probe.get("bagPokeBallCount", 0) >= 5
        and probe_on_map(final_probe, MAP_GROUP_LITTLEROOT_BUILDINGS, MAP_NUM_BIRCH_LAB)
        and final_probe.get("fieldMovementReady") == 1
    )
    trace.append({
        "id": "Story.PokeBallsAcquired",
        "accepted": ok,
        "playerPartyCount": final_probe.get("playerPartyCount"),
        "bagPokeBallCount": final_probe.get("bagPokeBallCount"),
        "mapGroup": final_probe.get("mapGroup"),
        "mapNum": final_probe.get("mapNum"),
        "playerX": final_probe.get("playerX"),
        "playerY": final_probe.get("playerY"),
    })

    return {
        "ok": ok,
        "target": "STORY_POKEBALLS_SMOKE",
        "probeAddress": probe_address,
        "routeTrace": trace,
        "bedroomBeacon": bedroom["beacon"],
        "rareCandyWithdrawal": withdrawal,
        "starterConfirmBeacon": confirm["beacon"],
        "starterRareCandyUses": starter_candy_uses,
        "starterLeveledProbe": starter_leveled_probe,
        "postRescueProbe": post_rescue,
        "labReadyProbe": lab_ready,
        "townReadyProbe": town_ready,
        "route101ReadyProbe": route101_ready,
        "oldaleReadyProbe": oldale_ready,
        "route103ReadyProbe": route103_ready,
        "postRivalBattleProbe": post_rival_battle,
        "route103AfterRivalProbe": route103_after_rival,
        "oldaleReturnProbe": oldale_return,
        "route101ReturnProbe": route101_return,
        "littlerootReturnProbe": littleroot_return,
        "labReturnProbe": lab_return,
        "pokeballsReadyProbe": pokeballs_ready,
        "finalProbe": final_probe,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def drive_poochyena_capture_smoke(client: BridgeClient, args: argparse.Namespace) -> dict[str, Any]:
    route_start = time.monotonic()
    probe_address = resolve_probe_address(args)
    trace: list[dict[str, Any]] = []

    story = drive_story_pokeballs_smoke(client, args)
    if not story.get("ok"):
        return story
    trace.extend(story.get("routeTrace", []))
    trace.append(asdict(RouteNode("Party.CatchPoochyena", "Catch a real Route 101 Poochyena with abilityNum 0 using real Poke Balls")))

    capture = capture_route101_poochyena(client, probe_address, args.objective_timeout)
    trace.extend(capture.get("routeTrace", []))
    final_probe = capture["finalProbe"]
    poochyena = capture["poochyena"]
    ok = (
        capture.get("ok") is True
        and final_probe.get("playerPartyCount", 0) >= 2
        and poochyena.get("species") == SPECIES_POOCHYENA
        and poochyena.get("abilityNum") == 0
    )

    return {
        "ok": ok,
        "target": "POOCHYENA_CAPTURE_SMOKE",
        "probeAddress": probe_address,
        "routeTrace": trace,
        "storyResult": story,
        "captureResult": capture,
        "poochyena": poochyena,
        "finalProbe": final_probe,
        "elapsedSeconds": round(time.monotonic() - route_start, 3),
    }


def wait_for_pc_menu_state(
    client: BridgeClient,
    probe_address: int,
    state: int,
    timeout: float,
    label: str,
    item_id: int | None = None,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if probe.get("pcMenuState") == state and (item_id is None or probe.get("pcItemId") == item_id):
            return probe

        beacon = client.request("read_beacon")
        if beacon.get("found") and beacon.get("inputReady") == 1 and beacon.get("textReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def wait_for_pc_quantity(
    client: BridgeClient,
    probe_address: int,
    quantity: int | None,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    return wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: (
            probe.get("pcMenuState") == PC_MENU_QUANTITY
            and probe.get("pcItemId") == ITEM_RARE_CANDY
            and (quantity is None or probe.get("pcItemQuantity") == quantity)
        ),
        timeout,
        label,
    )


def set_pc_quantity(
    client: BridgeClient,
    probe_address: int,
    quantity: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    probe = wait_for_pc_quantity(client, probe_address, None, max(1.0, deadline - time.monotonic()), label)

    while time.monotonic() < deadline:
        current = int(probe.get("pcItemQuantity", 0))
        if current == quantity:
            return probe
        if current < quantity:
            key = "RIGHT" if current + 10 <= quantity else "UP"
            expected_quantity = current + (10 if key == "RIGHT" else 1)
        else:
            key = "LEFT" if current - 10 >= quantity else "DOWN"
            expected_quantity = current - (10 if key == "LEFT" else 1)
        tap(client, key, frames=8, settle_frames=12)
        probe = wait_for_pc_quantity(
            client,
            probe_address,
            expected_quantity,
            max(1.0, deadline - time.monotonic()),
            label,
        )

    raise RuntimeError(f"{label} quantity timed out: {probe}")


def withdraw_rare_candies_from_bedroom_pc(
    client: BridgeClient,
    probe_address: int,
    quantity: int,
    timeout: float,
) -> dict[str, Any]:
    before = read_probe(client, probe_address)
    validate_probe(before)
    if before.get("pcRareCandyCount", 0) < quantity:
        raise RuntimeError(f"PC Rare Candy count {before.get('pcRareCandyCount')} is below requested {quantity}: {before}")

    move_to_position(
        client,
        8,
        2,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        MAP_LITTLEROOT,
        timeout,
        "bedroom PC approach",
    )
    face_tile(
        client,
        "UP",
        8,
        1,
        STAGE_LITTLEROOT_ROUTE_SETUP,
        5,
        MAP_LITTLEROOT,
        timeout,
        "bedroom PC facing",
    )
    tap(client, "A", frames=8, settle_frames=18)

    top_menu = wait_for_pc_menu_state(client, probe_address, PC_MENU_TOP, timeout, "bedroom PC top menu")
    tap(client, "A", frames=8, settle_frames=12)
    item_storage = wait_for_pc_menu_state(client, probe_address, PC_MENU_ITEM_STORAGE, timeout, "item storage menu")
    tap(client, "A", frames=8, settle_frames=12)
    item_list = wait_for_pc_menu_state(client, probe_address, PC_MENU_ITEM_LIST, timeout, "PC item list", ITEM_RARE_CANDY)
    tap(client, "A", frames=8, settle_frames=12)
    quantity_probe = set_pc_quantity(client, probe_address, quantity, timeout, "Rare Candy quantity prompt")
    tap(client, "A", frames=8, settle_frames=12)
    withdrew = wait_for_pc_menu_state(client, probe_address, PC_MENU_WITHDRAW_MESSAGE, timeout, "Rare Candy withdraw message", ITEM_RARE_CANDY)
    tap(client, "A", frames=8, settle_frames=12)

    after = wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: (
            probe.get("pcRareCandyCount") == before.get("pcRareCandyCount") - quantity
            and probe.get("bagRareCandyCount") == before.get("bagRareCandyCount") + quantity
        ),
        timeout,
        "Rare Candy counts after withdraw",
    )

    wait_for_pc_menu_state(client, probe_address, PC_MENU_ITEM_LIST, timeout, "PC item list after withdraw", ITEM_RARE_CANDY)
    tap(client, "B", frames=8, settle_frames=18)
    wait_for_pc_menu_state(client, probe_address, PC_MENU_ITEM_STORAGE, timeout, "item storage menu after list exit")
    tap(client, "B", frames=8, settle_frames=18)
    wait_for_pc_menu_state(client, probe_address, PC_MENU_TOP, timeout, "PC top menu after item storage exit")
    tap(client, "B", frames=8, settle_frames=18)
    exit_beacon = wait_for_semantic_movement_ready_or_advance_text(
        client,
        movement_ready_criteria(STAGE_LITTLEROOT_ROUTE_SETUP, 5, MAP_LITTLEROOT),
        timeout,
        "field control after PC exit",
    )

    final_probe = read_probe(client, probe_address)
    validate_probe(final_probe)
    return {
        "beforeProbe": before,
        "afterProbe": after,
        "finalProbe": final_probe,
        "exitBeacon": exit_beacon,
        "quantity": quantity,
        "topMenuProbe": top_menu,
        "itemStorageProbe": item_storage,
        "itemListProbe": item_list,
        "quantityProbe": quantity_probe,
        "withdrewProbe": withdrew,
    }


def wait_for_start_menu_ready(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    return wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: probe.get("startMenuState") == START_MENU_READY,
        timeout,
        label,
    )


def choose_start_menu_action(
    client: BridgeClient,
    probe_address: int,
    action: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    visited: set[tuple[int, int]] = set()

    while time.monotonic() < deadline:
        probe = wait_for_start_menu_ready(client, probe_address, max(1.0, deadline - time.monotonic()), label)
        cursor_key = (int(probe.get("startMenuCursor", 0)), int(probe.get("startMenuAction", 0)))
        if probe.get("startMenuAction") == action:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        if cursor_key in visited and len(visited) >= int(probe.get("startMenuCount", 1)):
            raise RuntimeError(f"{label} could not find start menu action {action}: {probe}")
        visited.add(cursor_key)
        tap(client, "DOWN", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out")


def open_bag_from_field(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    field = wait_for_probe_field_movement_ready(
        client,
        probe_address,
        None,
        None,
        timeout,
        f"{label} field ready",
        run_incidental_battles=False,
    )
    tap(client, "START", frames=8, settle_frames=18)
    choose_start_menu_action(client, probe_address, MENU_ACTION_BAG, timeout, f"{label} start bag")
    bag = wait_for_probe_condition(
        client,
        probe_address,
        lambda probe: probe.get("bagMenuState") == BAG_MENU_ITEM_LIST,
        timeout,
        f"{label} bag item list",
    )
    return {"fieldProbe": field, "bagProbe": bag}


def choose_bag_item(
    client: BridgeClient,
    probe_address: int,
    item_id: int,
    pocket: int,
    timeout: float,
    label: str,
    retry_battle_bag: bool = False,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    visited: set[tuple[int, int, int]] = set()
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if retry_battle_bag and probe.get("inBattle") == 0:
            return probe
        if (
            retry_battle_bag
            and probe.get("inBattle") == 1
            and probe.get("battleMenuState") == BATTLE_MENU_ACTION
            and probe.get("battleMenuCursor") == 1
            and probe.get("bagMenuState") != BAG_MENU_ITEM_LIST
        ):
            tap(client, "A", frames=8, settle_frames=18)
            continue
        if not (
            probe.get("bagMenuState") == BAG_MENU_ITEM_LIST
            and probe_state_is_fresh(probe, "bagMenuFrame")
            and probe.get("fieldPaletteFadeActive") == 0
        ):
            client.request("run_frames 5")
            continue
        current_pocket = int(probe.get("bagMenuPocket", 0))
        if current_pocket != pocket:
            tap(client, "RIGHT" if current_pocket < pocket else "LEFT", frames=8, settle_frames=14)
            continue
        if probe.get("bagMenuItemId") == item_id:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        cursor_key = (
            int(probe.get("bagMenuPocket", 0)),
            int(probe.get("bagMenuCursor", 0)),
            int(probe.get("bagMenuItemId", 0)),
        )
        if cursor_key in visited:
            raise RuntimeError(f"{label} could not find bag item {item_id} in pocket {pocket}: {probe}")
        visited.add(cursor_key)
        tap(client, "DOWN", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def choose_bag_context_first_action(
    client: BridgeClient,
    probe_address: int,
    item_id: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if (
            probe.get("bagMenuState") == BAG_MENU_ITEM_LIST
            and probe_state_is_fresh(probe, "bagMenuFrame")
            and probe.get("bagMenuItemId") == item_id
            and probe.get("fieldPaletteFadeActive") == 0
        ):
            tap(client, "A", frames=8, settle_frames=18)
            continue
        if not (
            probe.get("bagMenuState") == BAG_MENU_CONTEXT
            and probe_state_is_fresh(probe, "bagMenuFrame")
            and probe.get("bagMenuItemId") == item_id
        ):
            client.request("run_frames 5")
            continue
        if probe.get("bagMenuContextCursor") == 0:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        tap(client, "UP", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def choose_party_slot(
    client: BridgeClient,
    probe_address: int,
    slot: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout

    while time.monotonic() < deadline:
        probe = wait_for_probe_condition(
            client,
            probe_address,
            lambda probe: probe.get("partyMenuState") == PARTY_MENU_CHOOSE,
            max(1.0, deadline - time.monotonic()),
            label,
        )
        cursor = int(probe.get("partyMenuCursor", 0))
        if cursor == slot:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        tap(client, "DOWN" if cursor < slot else "UP", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out")


def wait_for_rare_candy_completion(
    client: BridgeClient,
    probe_address: int,
    slot: int,
    before_level: int,
    before_candies: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0
    completion_seen = False

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        levels = probe.get("partyLevel", [])
        level = int(levels[slot]) if slot < len(levels) else 0
        completion_seen = completion_seen or (
            level > before_level
            and probe.get("bagRareCandyCount") == before_candies - 1
        )
        if completion_seen and probe.get("fieldMovementReady") == 1:
            return probe
        if completion_seen and probe.get("partyMenuState") in {PARTY_MENU_CHOOSE, PARTY_MENU_ACTION}:
            tap(client, "B", frames=8, settle_frames=18)
            continue
        if completion_seen and probe.get("bagMenuState") in {BAG_MENU_ITEM_LIST, BAG_MENU_CONTEXT}:
            tap(client, "B", frames=8, settle_frames=18)
            continue
        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
            idle_polls = 0
            continue

        client.request("run_frames 5")
        idle_polls += 1
        if idle_polls >= 10:
            tap(client, "A", frames=4, settle_frames=6)
            idle_polls = 0

    raise RuntimeError(f"{label} timed out: {last_probe}")


def use_rare_candy_on_party_slot(
    client: BridgeClient,
    probe_address: int,
    slot: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    before = read_probe(client, probe_address)
    validate_probe(before)
    levels = before.get("partyLevel", [])
    if slot >= len(levels) or int(levels[slot]) <= 0:
        raise RuntimeError(f"{label} cannot use Rare Candy on empty slot {slot}: {before}")
    if before.get("bagRareCandyCount", 0) <= 0:
        raise RuntimeError(f"{label} has no Rare Candies in bag: {before}")

    open_bag = open_bag_from_field(client, probe_address, timeout, label)
    item = choose_bag_item(client, probe_address, ITEM_RARE_CANDY, POCKET_ITEMS, timeout, f"{label} choose Rare Candy")
    context = choose_bag_context_first_action(client, probe_address, ITEM_RARE_CANDY, timeout, f"{label} choose Use")
    party = choose_party_slot(client, probe_address, slot, timeout, f"{label} choose party slot")
    after = wait_for_rare_candy_completion(
        client,
        probe_address,
        slot,
        int(levels[slot]),
        int(before.get("bagRareCandyCount", 0)),
        timeout,
        label,
    )
    return {
        "beforeProbe": before,
        "afterProbe": after,
        "openBagProbe": open_bag["bagProbe"],
        "itemProbe": item,
        "contextProbe": context,
        "partyProbe": party,
    }


def use_rare_candies_on_party_slot(
    client: BridgeClient,
    probe_address: int,
    slot: int,
    count: int,
    timeout: float,
    label: str,
) -> list[dict[str, Any]]:
    results: list[dict[str, Any]] = []
    for index in range(count):
        results.append(use_rare_candy_on_party_slot(client, probe_address, slot, timeout, f"{label} {index + 1}/{count}"))
    return results


def wait_for_shop_menu_state(
    client: BridgeClient,
    probe_address: int,
    state: int,
    timeout: float,
    label: str,
    item_id: int | None = None,
    quantity: int | None = None,
    require_fresh: bool = True,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if (
            probe.get("shopMenuState") == state
            and (not require_fresh or probe_state_is_fresh(probe, "shopMenuFrame"))
            and (item_id is None or probe.get("shopMenuItemId") == item_id)
            and (quantity is None or probe.get("shopMenuItemQuantity") == quantity)
        ):
            return probe
        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
        elif probe.get("shopMenuState") == SHOP_MENU_MESSAGE:
            tap(client, "A", frames=8, settle_frames=12)
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def choose_shop_top_action(
    client: BridgeClient,
    probe_address: int,
    cursor: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_shop_menu_state(
            client,
            probe_address,
            SHOP_MENU_TOP,
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_probe = probe
        current = int(probe.get("shopMenuCursor", 0))
        if current == cursor:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        tap(client, "DOWN" if current < cursor else "UP", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def choose_shop_buy_item(
    client: BridgeClient,
    probe_address: int,
    item_id: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    visited: set[tuple[int, int]] = set()
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = wait_for_shop_menu_state(
            client,
            probe_address,
            SHOP_MENU_BUY_LIST,
            max(1.0, deadline - time.monotonic()),
            label,
        )
        last_probe = probe
        if probe.get("shopMenuItemId") == item_id:
            tap(client, "A", frames=8, settle_frames=18)
            return probe
        cursor_key = (int(probe.get("shopMenuCursor", 0)), int(probe.get("shopMenuItemId", 0)))
        if cursor_key in visited:
            raise RuntimeError(f"{label} could not find shop item {item_id}: {probe}")
        visited.add(cursor_key)
        tap(client, "DOWN", frames=8, settle_frames=12)

    raise RuntimeError(f"{label} timed out: {last_probe}")


def set_shop_quantity(
    client: BridgeClient,
    probe_address: int,
    quantity: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    probe = wait_for_shop_menu_state(
        client,
        probe_address,
        SHOP_MENU_QUANTITY,
        max(1.0, deadline - time.monotonic()),
        label,
        ITEM_POKE_BALL,
    )

    while time.monotonic() < deadline:
        current = int(probe.get("shopMenuItemQuantity", 0))
        if current == quantity:
            return probe
        if current < quantity:
            key = "RIGHT" if current + 10 <= quantity else "UP"
        else:
            key = "LEFT" if current - 10 >= quantity else "DOWN"
        tap(client, key, frames=8, settle_frames=12)
        probe = wait_for_shop_menu_state(
            client,
            probe_address,
            SHOP_MENU_QUANTITY,
            max(1.0, deadline - time.monotonic()),
            label,
            ITEM_POKE_BALL,
            require_fresh=True,
        )

    raise RuntimeError(f"{label} quantity timed out: {probe}")


def wait_for_shop_purchase_complete(
    client: BridgeClient,
    probe_address: int,
    target_poke_balls: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if (
            probe.get("bagPokeBallCount", 0) >= target_poke_balls
            and probe.get("shopMenuState") == SHOP_MENU_BUY_LIST
            and probe_state_is_fresh(probe, "shopMenuFrame")
        ):
            return probe
        if probe.get("shopMenuState") in {SHOP_MENU_CONFIRM, SHOP_MENU_MESSAGE}:
            tap(client, "A", frames=8, settle_frames=18)
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"{label} timed out: {last_probe}")


def buy_oldale_poke_balls_from_lab(
    client: BridgeClient,
    probe_address: int,
    quantity: int,
    timeout: float,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    trace: list[dict[str, Any]] = []
    before = read_probe(client, probe_address)
    validate_probe(before)

    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_LITTLEROOT_BUILDINGS,
        MAP_NUM_BIRCH_LAB,
        6,
        12,
        max(1.0, deadline - time.monotonic()),
        "Birch lab exit tile for Oldale Mart",
    )
    town = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        4,
        max(1.0, deadline - time.monotonic()),
        "Littleroot after lab exit for Oldale Mart",
    )
    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_LITTLEROOT_TOWN,
        11,
        2,
        max(1.0, deadline - time.monotonic()),
        "Littleroot north exit for Oldale Mart",
    )
    route101 = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        4,
        max(1.0, deadline - time.monotonic()),
        "Route 101 entry for Oldale Mart",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        [
            (10, 14, "south lane"),
            (7, 14, "west below rescue blocker"),
            (7, 10, "west route bend"),
            (13, 10, "east route bend"),
            (13, 9, "north turn"),
            (15, 9, "east grass edge"),
            (15, 2, "north grass edge"),
            (11, 2, "top route bend"),
            (11, 0, "top exit lane"),
            (10, 0, "north exit approach"),
        ],
        max(1.0, deadline - time.monotonic()),
        "Route 101 outbound path for Oldale Mart",
    )
    oldale = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        4,
        max(1.0, deadline - time.monotonic()),
        "Oldale south entry for Mart",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        [
            (10, 7, "Oldale central lane"),
            (14, 7, "Oldale Mart door"),
        ],
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart approach",
    )
    mart_entry = probe_move_until_map(
        client,
        probe_address,
        "UP",
        MAP_GROUP_OLDALE_BUILDINGS,
        MAP_NUM_OLDALE_MART,
        4,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart entry",
    )
    probe_move_to_position(
        client,
        probe_address,
        MAP_GROUP_OLDALE_BUILDINGS,
        MAP_NUM_OLDALE_MART,
        3,
        3,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart clerk approach",
    )
    probe_face_tile(
        client,
        probe_address,
        MAP_GROUP_OLDALE_BUILDINGS,
        MAP_NUM_OLDALE_MART,
        "LEFT",
        2,
        3,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart clerk facing",
    )
    tap(client, "A", frames=8, settle_frames=18)

    top = wait_for_shop_menu_state(
        client,
        probe_address,
        SHOP_MENU_TOP,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart top menu",
    )
    choose_shop_top_action(client, probe_address, 0, max(1.0, deadline - time.monotonic()), "Oldale Mart choose Buy")
    item = choose_shop_buy_item(
        client,
        probe_address,
        ITEM_POKE_BALL,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart choose Poke Ball",
    )
    quantity_probe = set_shop_quantity(
        client,
        probe_address,
        quantity,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart Poke Ball quantity",
    )
    tap(client, "A", frames=8, settle_frames=18)
    confirm = wait_for_shop_menu_state(
        client,
        probe_address,
        SHOP_MENU_CONFIRM,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart purchase confirmation",
        ITEM_POKE_BALL,
        quantity,
        require_fresh=False,
    )
    target_poke_balls = int(before.get("bagPokeBallCount", 0)) + quantity
    tap(client, "A", frames=8, settle_frames=18)
    purchased = wait_for_shop_purchase_complete(
        client,
        probe_address,
        target_poke_balls,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart purchase complete",
    )
    tap(client, "B", frames=8, settle_frames=18)
    wait_for_shop_menu_state(
        client,
        probe_address,
        SHOP_MENU_TOP,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart top menu after buy",
    )
    tap(client, "B", frames=8, settle_frames=18)
    mart_ready = wait_for_probe_field_movement_ready(
        client,
        probe_address,
        MAP_GROUP_OLDALE_BUILDINGS,
        MAP_NUM_OLDALE_MART,
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart field control after purchase",
    )
    probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OLDALE_BUILDINGS,
        MAP_NUM_OLDALE_MART,
        [
            (3, 3, "Mart center aisle"),
            (3, 6, "Mart exit lane"),
        ],
        max(1.0, deadline - time.monotonic()),
        "Oldale Mart exit approach",
    )
    outside = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        4,
        max(1.0, deadline - time.monotonic()),
        "Oldale after Mart exit",
    )
    final_probe = read_probe(client, probe_address)
    validate_probe(final_probe)
    ok = final_probe.get("bagPokeBallCount", 0) >= target_poke_balls
    trace.extend([
        {"id": "Shop.LeftLabForOldale", "probe": town},
        {"id": "Shop.Route101Outbound", "probe": route101},
        {"id": "Shop.OldaleReached", "probe": oldale},
        {"id": "Shop.OldaleMartEntered", "probe": mart_entry},
        {
            "id": "Inventory.PokeBallsBoughtAtOldaleMart",
            "accepted": ok,
            "quantity": quantity,
            "beforePokeBallCount": before.get("bagPokeBallCount"),
            "afterPokeBallCount": final_probe.get("bagPokeBallCount"),
            "beforeMoney": top.get("shopMenuMoney"),
            "afterMoney": purchased.get("shopMenuMoney"),
            "unitPrice": item.get("shopMenuItemPrice"),
        },
    ])

    return {
        "ok": ok,
        "target": "OLDALE_POKE_BALL_PURCHASE",
        "routeTrace": trace,
        "beforeProbe": before,
        "topMenuProbe": top,
        "itemProbe": item,
        "quantityProbe": quantity_probe,
        "confirmProbe": confirm,
        "purchasedProbe": purchased,
        "martReadyProbe": mart_ready,
        "outsideProbe": outside,
        "finalProbe": final_probe,
    }


def set_visible_battle_action_cursor(
    client: BridgeClient,
    probe_address: int,
    target_cursor: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        if probe.get("inBattle") == 0:
            return probe
        if probe.get("battleMenuState") != BATTLE_MENU_ACTION:
            client.request("run_frames 5")
            idle_polls += 1
            if idle_polls >= 10:
                tap(client, "A", frames=4, settle_frames=6)
                idle_polls = 0
            continue

        cursor = int(probe.get("battleMenuCursor", 0))
        if cursor == target_cursor:
            return probe
        if (cursor & 1) and not (target_cursor & 1):
            tap(client, "LEFT", frames=8, settle_frames=10)
        elif not (cursor & 1) and (target_cursor & 1):
            tap(client, "RIGHT", frames=8, settle_frames=10)
        elif (cursor & 2) and not (target_cursor & 2):
            tap(client, "UP", frames=8, settle_frames=10)
        elif not (cursor & 2) and (target_cursor & 2):
            tap(client, "DOWN", frames=8, settle_frames=10)
        idle_polls = 0

    raise RuntimeError(f"{label} cursor timed out: {last_probe}")


def wait_for_capture_resolution(
    client: BridgeClient,
    probe_address: int,
    before_party_count: int,
    before_ball_count: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    last_probe: dict[str, Any] | None = None
    idle_polls = 0
    captured_seen = False

    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        last_probe = probe
        captured_seen = captured_seen or int(probe.get("playerPartyCount", 0)) > before_party_count
        if captured_seen:
            if (
                probe.get("scriptMenuState") == SCRIPT_MENU_YESNO
                and probe_state_is_fresh(probe, "scriptMenuFrame")
                and probe.get("scriptMenuResult") == SCRIPT_MENU_RESULT_PENDING
            ):
                choose_script_yesno(client, probe_address, False, max(1.0, deadline - time.monotonic()), f"{label} nickname")
                idle_polls = 0
                continue
            if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
                tap(client, "A", frames=8, settle_frames=12)
                idle_polls = 0
                continue
            if probe.get("inBattle") == 0 and probe.get("fieldMovementReady") == 1:
                return {"outcome": "captured", "probe": probe}

        if (
            not captured_seen
            and probe.get("inBattle") == 1
            and probe.get("battleMenuState") == BATTLE_MENU_ACTION
            and int(probe.get("bagPokeBallCount", before_ball_count)) < before_ball_count
        ):
            return {"outcome": "failed", "probe": probe}

        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
            idle_polls = 0
        else:
            client.request("run_frames 5")
            idle_polls += 1
            if idle_polls >= 10:
                tap(client, "A", frames=4, settle_frames=6)
                idle_polls = 0

    raise RuntimeError(f"{label} timed out: {last_probe}")


def throw_poke_ball_from_battle(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
    label: str,
) -> dict[str, Any]:
    before = read_probe(client, probe_address)
    validate_probe(before)
    if before.get("bagPokeBallCount", 0) <= 0:
        raise RuntimeError(f"{label} has no Poke Balls: {before}")

    action = set_visible_battle_action_cursor(client, probe_address, 1, timeout, f"{label} battle bag action")
    if action.get("inBattle") == 0:
        return {"outcome": "ended", "beforeProbe": before, "afterProbe": action}
    tap(client, "A", frames=8, settle_frames=18)
    item = choose_bag_item(
        client,
        probe_address,
        ITEM_POKE_BALL,
        POCKET_POKE_BALLS,
        timeout,
        f"{label} choose Poke Ball",
        retry_battle_bag=True,
    )
    if item.get("inBattle") == 0:
        return {
            "outcome": "captured" if int(item.get("playerPartyCount", 0)) > int(before.get("playerPartyCount", 0)) else "ended",
            "beforeProbe": before,
            "actionProbe": action,
            "itemProbe": item,
            "afterProbe": item,
        }
    context = choose_bag_context_first_action(client, probe_address, ITEM_POKE_BALL, timeout, f"{label} use Poke Ball")
    resolution = wait_for_capture_resolution(
        client,
        probe_address,
        int(before.get("playerPartyCount", 0)),
        int(before.get("bagPokeBallCount", 0)),
        timeout,
        label,
    )
    return {
        "outcome": resolution["outcome"],
        "beforeProbe": before,
        "actionProbe": action,
        "itemProbe": item,
        "contextProbe": context,
        "afterProbe": resolution["probe"],
    }


def capture_route101_poochyena(
    client: BridgeClient,
    probe_address: int,
    timeout: float,
) -> dict[str, Any]:
    deadline = time.monotonic() + timeout
    trace: list[dict[str, Any]] = []

    purchase = buy_oldale_poke_balls_from_lab(client, probe_address, 15, timeout)
    if not purchase.get("ok"):
        raise RuntimeError(f"Oldale Mart Poke Ball purchase failed: {purchase}")
    trace.extend(purchase.get("routeTrace", []))
    oldale_south = probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_OLDALE_TOWN,
        [
            (10, 7, "Oldale central lane after Mart"),
            (10, 19, "Oldale south exit after Mart"),
        ],
        max(1.0, deadline - time.monotonic()),
        "Oldale south exit after Mart purchase",
    )
    route101 = probe_move_until_map(
        client,
        probe_address,
        "DOWN",
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        40,
        max(1.0, deadline - time.monotonic()),
        "Route 101 capture entry from Oldale",
    )
    grass = probe_follow_waypoints(
        client,
        probe_address,
        MAP_GROUP_OUTSIDE,
        MAP_NUM_ROUTE101,
        [
            (10, 5, "north lane"),
            (13, 5, "east bend"),
            (13, 6, "south turn"),
            (15, 6, "east lane"),
            (15, 9, "east grass edge"),
            (13, 9, "west grass edge"),
            (13, 10, "capture grass lane"),
        ],
        max(1.0, deadline - time.monotonic()),
        "Route 101 capture grass approach from Oldale",
    )
    trace.extend([
        {"id": "Capture.OldalePokeBallsPurchased", "probe": purchase["finalProbe"]},
        {"id": "Capture.OldaleSouthExitReady", "probe": oldale_south},
        {"id": "Capture.Route101Entered", "probe": route101},
        {"id": "Capture.Route101GrassReady", "probe": grass},
    ])

    step_up = True
    capture_attempts: list[dict[str, Any]] = []
    encounters: list[dict[str, Any]] = []
    while time.monotonic() < deadline:
        probe = read_probe(client, probe_address)
        validate_probe(probe)
        if probe.get("inBattle") == 1:
            encounter = {
                "enemy0Species": probe.get("enemy0Species"),
                "enemy0Level": probe.get("enemy0Level"),
                "enemy0AbilityNum": probe.get("enemy0AbilityNum"),
                "enemy0Ability": probe.get("enemy0Ability"),
                "bagPokeBallCount": probe.get("bagPokeBallCount"),
            }
            encounters.append(encounter)
            if probe.get("enemy0Species") == SPECIES_POOCHYENA and probe.get("enemy0AbilityNum") == 0:
                attempt = throw_poke_ball_from_battle(
                    client,
                    probe_address,
                    max(1.0, deadline - time.monotonic()),
                    "Route 101 Poochyena capture",
                )
                capture_attempts.append(attempt)
                if attempt.get("outcome") == "captured":
                    final_probe = attempt["afterProbe"]
                    poochyena = find_party_mon(final_probe, SPECIES_POOCHYENA, ability_num=0)
                    if poochyena is None:
                        raise RuntimeError(f"capture completed but slot-0 ability Poochyena was not observable: {final_probe}")
                    trace.append({
                        "id": "Party.CaughtPoochyenaAbilitySlot0",
                        "accepted": True,
                        "poochyena": poochyena,
                        "encounters": encounters,
                        "captureAttempts": len(capture_attempts),
                    })
                    return {
                        "ok": True,
                        "target": "POOCHYENA_CAPTURE",
                        "routeTrace": trace,
                        "encounters": encounters,
                        "captureAttempts": capture_attempts,
                        "poochyena": poochyena,
                        "finalProbe": final_probe,
                    }
                if attempt["afterProbe"].get("bagPokeBallCount", 0) <= 0:
                    raise RuntimeError(f"ran out of Poke Balls while catching Poochyena: {attempt}")
                continue

            run_probe = run_from_battle(
                client,
                probe_address,
                max(1.0, deadline - time.monotonic()),
                "Route 101 non-target encounter",
            )
            trace.append({"id": "Capture.RanFromNonTarget", "encounter": encounter, "probe": run_probe})
            continue

        if not probe_on_map(probe, MAP_GROUP_OUTSIDE, MAP_NUM_ROUTE101):
            raise RuntimeError(f"capture route left Route 101 unexpectedly: {probe}")
        if probe.get("fieldMovementReady") == 1:
            x = int(probe.get("playerX", 0))
            y = int(probe.get("playerY", 0))
            if (x, y) == (13, 10):
                move_tap(client, "UP")
                step_up = False
            elif (x, y) == (13, 9):
                move_tap(client, "DOWN")
                step_up = True
            else:
                probe_move_to_position(
                    client,
                    probe_address,
                    MAP_GROUP_OUTSIDE,
                    MAP_NUM_ROUTE101,
                    13,
                    10,
                    max(1.0, deadline - time.monotonic()),
                    "Route 101 capture grass reset",
                )
            continue
        if probe.get("fieldInputReady") == 1 and probe.get("fieldTextReady") == 1:
            tap(client, "A", frames=8, settle_frames=12)
        else:
            client.request("run_frames 5")

    raise RuntimeError(f"Route 101 Poochyena capture timed out after encounters={encounters}")


def drive_pc_rare_candy_smoke(client: BridgeClient, args: argparse.Namespace) -> dict[str, Any]:
    route_start = time.monotonic()
    probe_address = resolve_probe_address(args)
    trace: list[dict[str, Any]] = []

    trace.append(asdict(RouteNode("EarlyGame.BedroomPC", "Reach the bedroom PC before setting the clock")))
    bedroom = drive_to_bedroom_setup_ready(client, args.truck_timeout, args.objective_timeout)
    if not bedroom.get("ok"):
        return bedroom

    withdrawal = withdraw_rare_candies_from_bedroom_pc(client, probe_address, 1, args.objective_timeout)
    before = withdrawal["beforeProbe"]
    after = withdrawal["afterProbe"]
    final_probe = withdrawal["finalProbe"]
    exit_beacon = withdrawal["exitBeacon"]

    trace.append({
        "id": "Inventory.PCRareCandyAvailable",
        "accepted": before.get("pcRareCandyCount", 0) > 0,
        "pcRareCandyCount": before.get("pcRareCandyCount"),
        "pcRareCandySlot": before.get("pcRareCandySlot"),
        "bagRareCandyCount": before.get("bagRareCandyCount"),
    })

    trace.append({
        "id": "Inventory.RareCandiesWithdrawnFromPC",
        "accepted": True,
        "beforePcRareCandyCount": before.get("pcRareCandyCount"),
        "afterPcRareCandyCount": after.get("pcRareCandyCount"),
        "beforeBagRareCandyCount": before.get("bagRareCandyCount"),
        "afterBagRareCandyCount": after.get("bagRareCandyCount"),
        "topMenuProbe": withdrawal["topMenuProbe"],
        "itemStorageProbe": withdrawal["itemStorageProbe"],
        "itemListProbe": withdrawal["itemListProbe"],
        "quantityProbe": withdrawal["quantityProbe"],
        "withdrewProbe": withdrawal["withdrewProbe"],
    })
    ok = (
        final_probe.get("pcRareCandyCount") == before.get("pcRareCandyCount") - 1
        and final_probe.get("bagRareCandyCount") == before.get("bagRareCandyCount") + 1
    )

    return {
        "ok": ok,
        "target": "PC_RARE_CANDY_SMOKE",
        "probeAddress": probe_address,
        "routeTrace": trace,
        "bedroomBeacon": bedroom["beacon"],
        "exitBeacon": exit_beacon,
        "beforeProbe": before,
        "afterProbe": after,
        "finalProbe": final_probe,
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
    party_cursor = choose_battle_party_cursor(client)
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
    summary_state_artifact: dict[str, Any] | None = None
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
        summary_state_artifact = save_state_file(client, output_dir / "battle_summary_reached.ss")
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
        "saveState": summary_state_artifact,
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
    if args.mode in {"probe-smoke", "story-pokeballs-smoke", "poochyena-capture-smoke", "poochyena-intimidate", "pc-rare-candy-smoke"} and not report["symExists"] and not Path(args.sym).with_suffix(".map").exists():
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
            elif args.mode == "story-pokeballs-smoke":
                result = drive_story_pokeballs_smoke(client, args)
            elif args.mode == "poochyena-capture-smoke":
                result = drive_poochyena_capture_smoke(client, args)
            elif args.mode == "pc-rare-candy-smoke":
                result = drive_pc_rare_candy_smoke(client, args)
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
        if args.desktop_screenshot:
            try:
                client.request("clear_keys")
            except Exception:
                pass
            if args.desktop_screenshot_delay > 0:
                time.sleep(args.desktop_screenshot_delay)
            result["desktopScreenshot"] = capture_desktop_screenshot(Path(args.desktop_screenshot))
        write_json(output_dir / "run_result.json", result)
        print(json.dumps({"ok": result["ok"], "result": str(output_dir / "run_result.json")}, sort_keys=True))
        if args.keep_open_seconds > 0:
            try:
                client.request("clear_keys")
            except Exception:
                pass
            time.sleep(args.keep_open_seconds)
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
            "story-pokeballs-smoke",
            "poochyena-capture-smoke",
            "pc-rare-candy-smoke",
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
    parser.add_argument("--keep-open-seconds", type=float, default=0.0, help="delay cleanup after writing the run result so live GUI evidence can be captured")
    parser.add_argument("--desktop-screenshot", help="capture the full desktop to this PNG before emulator cleanup")
    parser.add_argument("--desktop-screenshot-delay", type=float, default=1.0, help="seconds to wait before desktop screenshot capture")
    parser.add_argument("--force-script", action="store_true", help="try --script even if --help does not list it")
    parser.add_argument("--require-headless", action="store_true", help="block if the selected binary is not headless")
    return parser.parse_args(argv)


if __name__ == "__main__":
    raise SystemExit(run(parse_args(sys.argv[1:])))
