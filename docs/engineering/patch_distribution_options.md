# Patch Distribution Options

Research date: June 6, 2026.

This report covers patch files that can turn a clean base game file into this project's built `pokeemerald.gba`. It also separates those player-facing binary patches from source patches for developers, because the creation and application tools are different.

## Recommendation

Use **BPS** as the default player-facing release patch format.

- Create tool: **Floating IPS / Flips**.
- Apply tool to document for players: **Floating IPS / Flips**.
- Browser fallback: **RomPatcher.js** or a RomPatcher.js-based site.
- Optional emulator-side application: **RetroArch softpatching** with a matching `.bps` sidecar, when the selected GBA core supports softpatching.

BPS is the best fit because it is ROM-hack-native, supports expanded files, and stores CRC32 checksums for the source, target, and patch. That catches the most common support problem: a player applying the patch to the wrong clean ROM or wrong base build.

Keep **xdelta3** as the automation fallback when a command-line-only pipeline matters more than ROM-hack convention. Keep **UPS** only if the target audience specifically expects GBA `.ups` patches. Avoid **IPS** for new releases unless the output is known to stay within legacy IPS limits and you accept that IPS does not verify the input file.

## Base File Decision

Before creating any player-facing patch, choose and publish the exact base:

- `Vanilla Emerald Version`: a `.gba` built from an unmodified `master` branch of pret's `pokeemerald`.
- `Base Expansion Version`: a `.gba` built from an unmodified `master` or `upcoming` branch of `pokeemerald-expansion`.

Those terms already exist in [scope.md](../team_procedures/scope.md). For a custom game built from this repository, a **Base Expansion Version** is usually the more practical patch base because the delta is smaller and support questions can be tied to a specific expansion commit. A patch against clean retail Emerald is friendlier to players only if the final project is intended to be distributed like a conventional ROM hack and the build can be reproduced from that base.

For every release, publish:

- Base file name and source branch/commit or retail revision.
- Base SHA-1 and CRC32.
- Patched output SHA-1 and CRC32.
- Patch format and apply tool.
- Whether saves are compatible with earlier builds.

## Options

| Format | Best use | Create tool | Apply tool to document | Strengths | Risks |
| --- | --- | --- | --- | --- | --- |
| BPS (`.bps`) | Default player release | Floating IPS / Flips | Floating IPS / Flips, RomPatcher.js, MultiPatch, RetroArch softpatching | Checks source and output, supports expansion, usually small patches | Players need a BPS-capable patcher |
| UPS (`.ups`) | GBA audience that expects UPS | NUPS, RomPatcher.js | NUPS, RomPatcher.js, MultiPatch | Familiar in older GBA hack workflows; includes input/output integrity checking in common implementations | Older tooling; not the best new default now that BPS is common |
| IPS (`.ips`) | Legacy compatibility only | Floating IPS / Flips, Lunar IPS, MultiPatch | Floating IPS / Flips, Lunar IPS, RomPatcher.js, MultiPatch, RetroArch softpatching | Very widely recognized | No source-file verification; 24-bit offsets make it unsuitable for expanded targets beyond roughly 16 MiB |
| xdelta / VCDIFF (`.xdelta`, `.vcdiff`) | Command-line automation or large binary deltas | xdelta3 | xdelta3, Delta Patcher, RomPatcher.js, MultiPatch | Strong CLI workflow; VCDIFF is a standardized binary-delta format; xdelta3 checksums by default unless disabled | Less familiar to GBA ROM-hack players than BPS/UPS |
| PPF (`.ppf`) | Disc-image or legacy N64-style workflows | PPF Studio, MultiPatch | PPF-O-Matic, applyppf, MultiPatch, RomPatcher.js | Handles large binary images and has legacy tool support | Not a good fit for a GBA release; associated mostly with PlayStation/large-image patching |
| Git patch (`.patch`) | Developer/source distribution | `git format-patch`, `git diff --binary` | `git am`, `git apply` | Human-reviewable, preserves source edits, can be applied before building | Not usable by players who only have a ROM; may conflict as the codebase moves |

## BPS Release Workflow

Build or obtain the selected clean base ROM, then build this project to create the target ROM:

```sh
make -j"$(nproc)" NO_MULTIBOOT=1
```

Record checksums:

```sh
sha1sum clean-base.gba pokeemerald.gba
```

Record CRC32 too, using a release hasher such as RomPatcher.js, 7-Zip, `rhash --crc32`, or another CRC32-capable tool.

Create the patch with Flips:

```sh
flips --create --bps clean-base.gba pokeemerald.gba shared-power-v1.bps
```

Verify the patch by applying it to a fresh copy of the clean base:

```sh
flips --apply shared-power-v1.bps clean-base.gba verify-shared-power-v1.gba
cmp pokeemerald.gba verify-shared-power-v1.gba
sha1sum verify-shared-power-v1.gba
```

The `cmp` command should print nothing and exit successfully. The verified output hash must match the published patched ROM hash.

Player-facing apply instructions should name the tool:

1. Open Floating IPS / Flips.
2. Select `Apply Patch`.
3. Choose `shared-power-v1.bps`.
4. Choose the clean base ROM whose SHA-1/CRC32 matches the release notes.
5. Save the patched `.gba`.
6. Confirm the patched `.gba` hash if a hasher is available.

## xdelta Automation Workflow

Use this when the release process needs simple shell commands and a GUI fallback can be Delta Patcher or RomPatcher.js:

```sh
xdelta3 -e -s clean-base.gba pokeemerald.gba shared-power-v1.xdelta
xdelta3 -d -s clean-base.gba shared-power-v1.xdelta verify-shared-power-v1.gba
cmp pokeemerald.gba verify-shared-power-v1.gba
```

Do not pass `-n`; that disables xdelta3 checksum handling.

## Source Patch Workflow

Use source patches for contributors, not players:

```sh
git format-patch --stdout BASE_COMMIT..HEAD > shared-power-source.patch
git am shared-power-source.patch
```

For uncommitted work, create a binary-capable diff instead:

```sh
git diff --binary BASE_COMMIT > shared-power-working-tree.patch
git apply --check shared-power-working-tree.patch
git apply shared-power-working-tree.patch
```

After applying a source patch, the recipient still has to build `pokeemerald.gba`.

## Notes On Softpatching

Softpatching is an application method, not a separate release format. RetroArch can auto-apply sidecar patch files such as `.bps`, `.ips`, and `.ups` when the ROM and patch are named consistently and the selected core supports loading from memory. This can be convenient for emulator users, but release notes should still give normal hard-patching instructions with Flips or RomPatcher.js.

## Sources

- [BPS file format specification](https://floating.muncher.se/byuu/bps/bps_spec.html)
- [Floating IPS / Flips README](https://github.com/Alcaro/Flips)
- [Flips command-line usage in source](https://github.com/Alcaro/Flips/blob/master/flips.cpp)
- [SMS Power: How to use BPS and IPS patch files](https://www.smspower.org/Hacks/HowToUseBPSAndIPSPatchFiles)
- [NUPS user guide](https://www.gamebrew.org/wiki/NUPS)
- [IPS file format notes](https://sneslab.net/wiki/IPS_file_format)
- [xdelta3 man page](https://www.mankier.com/1/xdelta3)
- [RFC 3284: VCDIFF format](https://www.rfc-editor.org/rfc/rfc3284.html)
- [Git format-patch documentation](https://git-scm.com/docs/git-format-patch)
- [Git apply documentation](https://www.kernel.org/pub/software/scm/git/docs/git-apply.html)
- [RomPatcher.js README](https://github.com/marcrobledo/RomPatcher.js/)
- [MultiPatch patching tool](https://projects.sappharad.com/multipatch/)
- [Libretro RetroArch softpatching guide](https://docs.libretro.com/guides/softpatching/)
