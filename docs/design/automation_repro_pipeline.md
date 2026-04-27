# Automation Reproduction Pipeline (mGBA)

## Why this exists

Manual reproduction for some bugs is expensive and slows iteration. We need a repeatable way for agents to reach specific in-game states (starting with first Pokemon selection) and run verification loops without consuming human bandwidth.

## Problem statement

Current UI-driving automation is too fragile:

- It depends on real-time sleeps and key spam.
- It drifts under speed-up and variable host load.
- It is vulnerable to desktop focus/input interference.
- It does not fail with clear stage-level diagnostics.

This makes it costly to maintain and unreliable for CI-like loops.

## Constraints

- Execution environment is mGBA plus external automation tools.
- Save artifacts may be epistemically contaminated across builds; save fixtures are not yet a stable source of truth.
- Fixed RNG is available and should be leveraged for determinism.
- We can modify game code when needed.
- Speed-up support is required.

## Guiding principles

1. Determinism from state, not wall-clock.
2. Stage-gated progression with explicit pass/fail criteria.
3. Minimal inputs between checkpoints.
4. Machine-readable diagnostics for every run.
5. Keep build-coupled assumptions visible and versioned.

## Proposed architecture

### 1) Stage pipeline

Split automation into bounded stages, each with a clear oracle:

- Stage A: boot -> intro complete
- Stage B: intro -> gender selection
- Stage C: gender -> name accepted (`A`)
- Stage D: name accepted -> first Pokemon selection target

Each stage should be independently runnable for debugging and iteration speed.

### 2) Beacon-driven gating (preferred over classifier-style vision)

Add a tiny, debug-configurable on-screen automation beacon in the game framebuffer.

Suggested v1:

- Anchor pixel: constant magic color
- Protocol pixel: beacon version
- State pixels: `stage_id`, `substage_id`, flags
- Optional pulse pixel: toggles on transition/frame window

External automation samples these pixels and advances only when the expected state is present.

### 3) Deterministic run controls

- Fixed RNG seed/path for reproducibility.
- Input cadence based on state changes, not `Sleep` deadlines.
- Support speed-up by treating frame/state transitions as the clock.

### 4) Diagnostics

Per run, emit:

- stage entered/completed
- observed beacon tuple
- inputs sent
- failure reason and stage id
- screenshot on failure/success checkpoints

## Near-term scope

Primary target:

- Reach first Pokemon selection reproducibly from a clean boot path.
- Verify female avatar path and exact name entry (`A`) as required setup.

Out of scope for v1:

- Full-game "perfect run"
- Cross-build save compatibility guarantees

## Success criteria

- High reliability across repeated runs (target: >= 95% stage completion for current route).
- Same script works at normal speed and speed-up.
- Failures are diagnosable from logs without manual replay.

## Migration path for save artifacts

When stage reliability is high, add optional export checkpoints as convenience artifacts. These should be treated as accelerators, not canonical truth, until cross-build trust rules are established.

