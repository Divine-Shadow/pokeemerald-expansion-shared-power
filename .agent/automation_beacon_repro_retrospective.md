# Automation Beacon Repro Retrospective

Date: 2026-04-27
Author: Codex

## Summary

The debug-only on-screen beacon proved useful: it converted early-game mGBA automation from timing and image matching into explicit game-state polling. It successfully reached and proved the female player named `A` truck milestone, and it made later route failures diagnosable with exact tuples rather than screenshots alone.

The main shortfall is that the runner still treats navigation as keypress sequences. That abstraction is too weak for fast mode and for maps where turning, movement, warps, field messages, and interactions share the same input keys. The latest route work reached the bedroom but confused the TV and clock because the beacon only proved "bedroom input-ready", not player position, facing, or interactable target.

## What Worked

- Beacon-gated state transitions were materially better than fixed sleeps or screenshot classifiers.
- A visible, decoded proof tuple made failures actionable: stage, substage, gender, name, map kind, script wait class, and input readiness gave concrete evidence.
- Local ROM staging and fixed 2x mGBA sizing reduced Windows/UNC and pixel-sampling instability.
- Strict protocol validation in the reader was necessary; checksum alone allowed false positives.
- Keeping screenshots as human evidence while using beacon JSON as the oracle was the right split.

## What Did Not Work

- `inputReady` was overloaded. Movement, text, interaction, menu, and script waits are different readiness domains.
- Fixed movement counts were brittle. Fast mode exposed turn-vs-step ambiguity and map-transition timing quickly.
- AutoHotkey grew beyond its useful role. Window focus, subprocess management, routing, retries, JSON parsing, and logging are too much for a monolithic AHK script.
- The v1 beacon proof row became an ad hoc diagnostic channel. Reusing `starterSelection` and `errorCode` for script/movement diagnostics helped short-term debugging but is not a clean contract.
- I added some diagnostics reactively. Player position, facing, movement completion, and interactable-ahead would have shortened the iteration loop substantially.

## Design Lessons

- A route runner should wait for semantic preconditions, not inferred readiness. Example: "player is at `(3,2)`, facing north, and interactable-ahead is clock" is the correct gate for pressing `A` at the clock.
- Beacon fields should say what kind of input is safe and why. `textReady`, `movementReady`, `menuReady`, and `interactReady` are better than one global `inputReady`.
- Movement helpers should be bounded state-machine operations such as `MoveTo(tile)`, `Face(direction)`, `Interact(target)`, and `AdvanceText()`, not "press Right twice".
- The emulator input bridge should be thin. Route orchestration, retry policy, structured logs, and beacon decoding should live in a general-purpose language.
- The automation surface needs map-local semantics. For early-game routing, `MayHouse2F`, `playerX`, `playerY`, `facing`, and `interactableAhead` are more useful than broad `Littleroot` substages alone.

## Recommended Next Iteration

- Define a beacon v2 or debug extension with explicit fields for `mapLocalKind`, `playerX`, `playerY`, `facing`, `movementReady`, `textReady`, `scriptWaitKind`, and `interactableAhead`.
- Move orchestration into Python, C#, or another general-purpose language. Keep AutoHotkey only as a minimal input/window adapter if needed.
- Replace fixed route fragments with semantic route steps:
  - wait for player pose,
  - move until position changes or target pose is reached,
  - interact only when `interactableAhead` matches the expected object,
  - advance text only when `textReady` or `scriptWaitKind` says a fresh button press is needed.
- Keep the PowerShell reader or port it behind a library interface, but make protocol validation strict and keep captures reproducible.
- Treat screenshots as audit artifacts, not control flow.

## Current Practical Guidance

If continuing from the current implementation, avoid more fixed-count movement fixes. The next useful code change should be beacon-side position/facing/interactable proof, then route-side semantic helpers that consume those fields.
