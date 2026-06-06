#!/usr/bin/env python3
"""Static NPC accessibility checker for pokeemerald map JSON.

The checker answers one narrow question: can a land player reach a tile from
which an object-event NPC can be talked to? It models ordinary adjacent
interaction and counter interaction using the repo's compiled map.bin collision
data and metatile behavior attributes.
"""

from __future__ import annotations

import argparse
import json
import re
import struct
import subprocess
import sys
from collections import deque
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


MAPGRID_COLLISION_MASK = 0x0C00
MAPGRID_ELEVATION_MASK = 0xF000
MAPGRID_METATILE_ID_MASK = 0x03FF
NUM_METATILES_IN_PRIMARY = 512
METATILE_ATTR_BEHAVIOR_MASK = 0x00FF

DIRS = (
    ("east", 1, 0),
    ("west", -1, 0),
    ("south", 0, 1),
    ("north", 0, -1),
)

SKIP_TARGET_GRAPHICS_EXACT = {
    "OBJ_EVENT_GFX_INVISIBLE",
    "OBJ_EVENT_GFX_CABLE_CAR",
    "OBJ_EVENT_GFX_MOVING_BOX",
    "OBJ_EVENT_GFX_SS_TIDAL",
}

SKIP_TARGET_GRAPHICS_PREFIXES = (
    "OBJ_EVENT_GFX_ITEM_BALL",
    "OBJ_EVENT_GFX_CUTTABLE_TREE",
    "OBJ_EVENT_GFX_BREAKABLE_ROCK",
    "OBJ_EVENT_GFX_PUSHABLE_BOULDER",
    "OBJ_EVENT_GFX_BERRY_TREE",
)

SKIP_TARGET_MOVEMENTS = {
    "MOVEMENT_TYPE_BERRY_TREE_GROWTH",
}


@dataclass(frozen=True)
class StandTile:
    kind: str
    direction: str
    player: tuple[int, int]
    through: tuple[int, int] | None = None


@dataclass(frozen=True)
class Finding:
    map_name: str
    map_path: Path
    object_id: str
    graphics_id: str
    coords: tuple[int, int]
    reason: str


@dataclass(frozen=True)
class UnsupportedMap:
    map_name: str
    map_path: Path
    reason: str


class RepoData:
    def __init__(self, root: Path):
        self.root = root
        self.layouts = self._load_layouts()
        self.mb_counter = self._load_metatile_behavior("MB_COUNTER")
        self.tileset_attr_paths = self._load_tileset_attr_paths()
        self.attr_cache: dict[Path, list[int]] = {}

    def _load_layouts(self) -> dict[str, dict]:
        path = self.root / "data/layouts/layouts.json"
        with path.open(encoding="utf-8") as f:
            data = json.load(f)
        return {layout["id"]: layout for layout in data["layouts"]}

    def _load_metatile_behavior(self, name: str) -> int:
        path = self.root / "include/constants/metatile_behaviors.h"
        text = path.read_text(encoding="utf-8")
        text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
        text = "\n".join(line.split("//", 1)[0] for line in text.splitlines())
        body = text.split("enum {", 1)[1].split("};", 1)[0]

        value = 0
        for raw in body.split(","):
            raw = raw.strip()
            if not raw:
                continue
            if "=" in raw:
                enum_name, enum_value = [part.strip() for part in raw.split("=", 1)]
                value = int(enum_value, 0)
            else:
                enum_name = raw
            if enum_name == name:
                return value
            value += 1

        raise ValueError(f"Could not find {name} in {path}")

    def _load_tileset_attr_paths(self) -> dict[str, Path]:
        metatiles_path = self.root / "src/data/tilesets/metatiles.h"
        headers_path = self.root / "src/data/tilesets/headers.h"
        metatiles = metatiles_path.read_text(encoding="utf-8")
        headers = headers_path.read_text(encoding="utf-8")

        attr_paths = {
            match.group(1): self.root / match.group(2)
            for match in re.finditer(
                r'const u16 (gMetatileAttributes_\w+)\[\] = INCBIN_U16\("([^"]+)"\);',
                metatiles,
            )
        }
        tileset_attrs = {
            match.group(1): match.group(2)
            for match in re.finditer(
                r"const struct Tileset (gTileset_\w+)\s*=\s*\{.*?"
                r"\.metatileAttributes\s*=\s*(gMetatileAttributes_\w+),",
                headers,
                re.S,
            )
        }

        missing = sorted(set(tileset_attrs.values()) - set(attr_paths))
        if missing:
            raise ValueError(f"Missing metatile attribute paths: {', '.join(missing)}")

        return {tileset: attr_paths[attr] for tileset, attr in tileset_attrs.items()}

    def metatile_attrs(self, tileset: str) -> list[int]:
        path = self.tileset_attr_paths[tileset]
        if path not in self.attr_cache:
            raw = path.read_bytes()
            self.attr_cache[path] = list(struct.unpack(f"<{len(raw) // 2}H", raw))
        return self.attr_cache[path]


class MapAccess:
    def __init__(self, repo: RepoData, map_path: Path):
        self.repo = repo
        self.map_path = map_path
        with map_path.open(encoding="utf-8") as f:
            self.map_json = json.load(f)

        self.map_name = self.map_json.get("name", str(map_path))
        self.layout = repo.layouts[self.map_json["layout"]]
        self.width = int(self.layout["width"])
        self.height = int(self.layout["height"])
        self.blocks = self._read_blocks()
        self.primary_attrs = repo.metatile_attrs(self.layout["primary_tileset"])
        self.secondary_attrs = repo.metatile_attrs(self.layout["secondary_tileset"])
        self.objects = self.map_json.get("object_events") or []

    def _read_blocks(self) -> list[int]:
        path = self.repo.root / self.layout["blockdata_filepath"]
        raw = path.read_bytes()
        expected = self.width * self.height
        if len(raw) < expected * 2:
            raise ValueError(f"{path} is too small for {self.width}x{self.height}")
        return list(struct.unpack(f"<{expected}H", raw[: expected * 2]))

    def block(self, x: int, y: int) -> int:
        return self.blocks[y * self.width + x]

    def collision(self, x: int, y: int) -> int:
        return (self.block(x, y) & MAPGRID_COLLISION_MASK) >> 10

    def elevation(self, x: int, y: int) -> int:
        return (self.block(x, y) & MAPGRID_ELEVATION_MASK) >> 12

    def metatile_id(self, x: int, y: int) -> int:
        return self.block(x, y) & MAPGRID_METATILE_ID_MASK

    def behavior(self, x: int, y: int) -> int:
        metatile = self.metatile_id(x, y)
        if metatile < NUM_METATILES_IN_PRIMARY:
            attrs = self.primary_attrs[metatile]
        else:
            attrs = self.secondary_attrs[metatile - NUM_METATILES_IN_PRIMARY]
        return attrs & METATILE_ATTR_BEHAVIOR_MASK

    def in_bounds(self, x: int, y: int) -> bool:
        return 0 <= x < self.width and 0 <= y < self.height

    def active_blockers_for(self, target: dict) -> set[tuple[int, int]]:
        blockers: set[tuple[int, int]] = set()
        for obj in self.objects:
            if "x" not in obj or "y" not in obj:
                continue
            if obj is target or obj.get("flag", "0") == "0":
                blockers.add((int(obj["x"]), int(obj["y"])))
        return blockers

    def passable(self, x: int, y: int, blockers: set[tuple[int, int]]) -> bool:
        return (
            self.in_bounds(x, y)
            and self.collision(x, y) == 0
            and (x, y) not in blockers
        )

    def reachable_tiles(self, blockers: set[tuple[int, int]]) -> set[tuple[int, int]]:
        seeds = self.seed_tiles(blockers)
        seen = set(seeds)
        queue: deque[tuple[int, int]] = deque(seeds)

        while queue:
            x, y = queue.popleft()
            for _, dx, dy in DIRS:
                nx, ny = x + dx, y + dy
                if (nx, ny) not in seen and self.passable(nx, ny, blockers):
                    seen.add((nx, ny))
                    queue.append((nx, ny))

        return seen

    def seed_tiles(self, blockers: set[tuple[int, int]]) -> list[tuple[int, int]]:
        seeds: list[tuple[int, int]] = []

        for warp in self.map_json.get("warp_events") or []:
            x, y = int(warp["x"]), int(warp["y"])
            if self.passable(x, y, blockers):
                seeds.append((x, y))
            for _, dx, dy in DIRS:
                if self.passable(x + dx, y + dy, blockers):
                    seeds.append((x + dx, y + dy))

        if self.map_json.get("connections"):
            for x in range(self.width):
                for y in (0, self.height - 1):
                    if self.passable(x, y, blockers):
                        seeds.append((x, y))
            for y in range(self.height):
                for x in (0, self.width - 1):
                    if self.passable(x, y, blockers):
                        seeds.append((x, y))

        return list(dict.fromkeys(seeds))

    def has_static_entrance_seed(self) -> bool:
        return bool(self.seed_tiles(set()))

    def compatible_elevation(self, stand: tuple[int, int], obj: dict) -> bool:
        stand_elevation = self.elevation(*stand)
        position_elevation = stand_elevation if stand_elevation != 0 else 0
        object_elevation = int(obj.get("elevation", 0))
        return (
            object_elevation == 0
            or position_elevation == 0
            or object_elevation == position_elevation
        )

    def stand_tiles_for(self, obj: dict) -> list[StandTile]:
        blockers = self.active_blockers_for(obj)
        reachable = self.reachable_tiles(blockers)
        ox, oy = int(obj["x"]), int(obj["y"])
        stands: list[StandTile] = []

        for direction, dx, dy in DIRS:
            stand = (ox + dx, oy + dy)
            if stand in reachable and self.compatible_elevation(stand, obj):
                stands.append(StandTile("adjacent", direction, stand))

        if stands:
            return stands

        for direction, dx, dy in DIRS:
            counter = (ox + dx, oy + dy)
            stand = (ox + 2 * dx, oy + 2 * dy)
            if (
                self.in_bounds(*counter)
                and stand in reachable
                and self.behavior(*counter) == self.repo.mb_counter
                and self.compatible_elevation(stand, obj)
            ):
                stands.append(StandTile("counter", direction, stand, counter))

        return stands


def is_target(obj: dict, include_flagged: bool) -> bool:
    if "x" not in obj or "y" not in obj:
        return False
    if not include_flagged and obj.get("flag", "0") != "0":
        return False

    graphics = obj.get("graphics_id", "")
    if graphics in SKIP_TARGET_GRAPHICS_EXACT:
        return False
    if graphics.startswith(SKIP_TARGET_GRAPHICS_PREFIXES):
        return False
    if obj.get("movement_type") in SKIP_TARGET_MOVEMENTS:
        return False

    return True


def object_id(obj: dict, index: int) -> str:
    return str(obj.get("local_id") or f"object#{index}")


def changed_map_jsons(root: Path) -> list[Path]:
    paths: set[Path] = set()
    commands = (
        ["git", "diff", "--name-only", "--diff-filter=ACMRTUXB", "HEAD", "--", "data/maps"],
        ["git", "ls-files", "--others", "--exclude-standard", "--", "data/maps"],
    )
    for command in commands:
        try:
            result = subprocess.run(
                command,
                cwd=root,
                check=False,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL,
            )
        except OSError:
            continue
        if result.returncode != 0:
            continue
        for raw in result.stdout.splitlines():
            path = Path(raw)
            if path.name == "map.json":
                paths.add(root / path)
    return sorted(paths)


def all_map_jsons(root: Path) -> list[Path]:
    return sorted((root / "data/maps").glob("*/map.json"))


def normalize_map_path(root: Path, raw: str) -> Path:
    path = Path(raw)
    if path.is_dir():
        path = path / "map.json"
    if not path.is_absolute():
        path = root / path
    return path


def collect_map_paths(args: argparse.Namespace) -> list[Path]:
    if args.all:
        paths = all_map_jsons(args.repo_root)
    elif args.changed:
        paths = changed_map_jsons(args.repo_root)
    else:
        paths = [normalize_map_path(args.repo_root, raw) for raw in args.maps]

    return sorted(dict.fromkeys(paths))


def format_stand(stand: StandTile) -> str:
    if stand.kind == "counter":
        return (
            f"{stand.kind}:{stand.direction} "
            f"player={stand.player} counter={stand.through}"
        )
    return f"{stand.kind}:{stand.direction} player={stand.player}"


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(
        description="Verify that active NPC object events have reachable talk tiles.",
    )
    parser.add_argument("maps", nargs="*", help="map.json paths or data/maps/<MapName> dirs")
    parser.add_argument(
        "--repo-root",
        type=Path,
        default=Path.cwd(),
        help="repository root (default: current working directory)",
    )
    parser.add_argument("--all", action="store_true", help="scan every data/maps/*/map.json")
    parser.add_argument(
        "--changed",
        action="store_true",
        help="scan changed and untracked data/maps/*/map.json files",
    )
    parser.add_argument(
        "--include-flagged-targets",
        action="store_true",
        help="also check objects hidden behind event flags; default checks flag 0 objects",
    )
    parser.add_argument(
        "--report-only",
        action="store_true",
        help="print findings but return success even when inaccessible targets are found",
    )
    args = parser.parse_args(argv)
    args.repo_root = args.repo_root.resolve()

    selector_count = int(args.all) + int(args.changed) + bool(args.maps)
    if selector_count != 1:
        parser.error("choose exactly one of explicit maps, --changed, or --all")

    repo = RepoData(args.repo_root)
    map_paths = collect_map_paths(args)
    if not map_paths:
        print("No map JSON files selected.")
        return 0

    findings: list[Finding] = []
    unsupported: list[UnsupportedMap] = []
    checked = 0

    for map_path in map_paths:
        access = MapAccess(repo, map_path)
        rel_map_path = map_path.relative_to(args.repo_root)
        map_checked = 0
        targets = [
            (index, obj)
            for index, obj in enumerate(access.objects)
            if is_target(obj, args.include_flagged_targets)
        ]

        if not targets:
            print(f"SKIP {access.map_name} ({rel_map_path}): no checkable NPC targets")
            continue

        if not access.has_static_entrance_seed():
            unsupported.append(
                UnsupportedMap(
                    map_name=access.map_name,
                    map_path=rel_map_path,
                    reason="no static warp or connection entrance seed",
                )
            )
            continue

        for index, obj in targets:
            map_checked += 1
            checked += 1
            stands = access.stand_tiles_for(obj)
            ident = object_id(obj, index)
            coords = (int(obj["x"]), int(obj["y"]))
            graphics = obj.get("graphics_id", "")

            if stands:
                stands_text = ", ".join(format_stand(stand) for stand in stands)
                print(f"OK {access.map_name} {ident} {graphics} at {coords}: {stands_text}")
            else:
                findings.append(
                    Finding(
                        map_name=access.map_name,
                        map_path=rel_map_path,
                        object_id=ident,
                        graphics_id=graphics,
                        coords=coords,
                        reason="no reachable adjacent or counter talk tile",
                    )
                )

    for finding in findings:
        print(
            "FAIL "
            f"{finding.map_name} {finding.object_id} {finding.graphics_id} "
            f"at {finding.coords} in {finding.map_path}: {finding.reason}",
            file=sys.stderr,
        )

    for skipped in unsupported:
        print(
            "UNSUPPORTED "
            f"{skipped.map_name} ({skipped.map_path}): {skipped.reason}; "
            "use emulator or manual proof",
            file=sys.stderr,
        )

    print(
        f"Checked {checked} NPC target(s) across {len(map_paths)} map(s); "
        f"{len(findings)} inaccessible target(s); "
        f"{len(unsupported)} unsupported map(s)."
    )

    if (findings or unsupported) and not args.report_only:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
