# Agents Guide for Design Docs

This guide helps human and AI contributors produce high‑quality, maintainable design documentation that is easy to navigate in mdBook and to reason about programmatically.

## Core Principles

- Keep files focused and reasonably small: aim for ~200 lines per file (soft cap). Split large topics into logical, linkable subfiles.
- Contain designs within a dedicated project folder under `docs/design/<project>/`.
- Provide a central index at the root of the project folder (e.g., `README.md`) with a brief overview, context, and a linked outline of subtopics.
- Aggressively cross‑link related concepts and adjacent docs to reduce duplication and improve discovery.

## Folder Structure

- Location: `docs/design/<project_name>/`
- Required files:
  - `README.md`: overview, context, glossary (if needed), and a table of contents linking to all subdocs.
  - Subdocs: separate files for problem statement, state model, APIs, data flow, sequencing, edge cases, tests, rollout, etc.
- Optional files:
  - `mode_explainer.md` for user‑facing summaries (when applicable).
  - `variants/*.md` or a `per_*.md` file for alternative designs.

## File Size & Splitting Guidance

- Target: ~200 lines; avoid exceeding ~300 lines unless there is strong reason.
- If a doc grows, split by cohesive themes (e.g., `state.md`, `api.md`, `events.md`, `damage_calc.md`).
- Keep each file single‑purpose; move background or expanded rationale to a sibling `rationale.md` if necessary.

## Indexing & Navigation

- Add an entry for the project to `docs/SUMMARY.md` under a “Design” section.
- Within the project folder:
  - Link every subdoc from `README.md`.
  - At the top of each subdoc, include links back to the folder index and to closely related subdocs.
- Use descriptive link text and relative links (e.g., `./api.md`).

## Cross‑Linking Patterns

- Link to:
  - Adjacent subdocs (e.g., from `events.md` to `damage_calc.md` and `api.md`).
  - Shared concepts in other design folders (e.g., AI, testing) when relevant.
  - Source files and directories by path when the design references concrete locations.
- Prefer links over repeating explanations to keep docs terse.

## Content Style

- Lead with outcomes and the minimal context needed to evaluate them.
- Favor lists and short sections; avoid long narrative blocks.
- Use consistent terminology that matches the codebase (function names, file paths).
- Call out tradeoffs and constraints explicitly.

## Change Management

- When adding a new project folder:
  - Create `docs/design/<project>/README.md` with a concise overview and a list of subdocs.
  - Add entries to `docs/SUMMARY.md` so the docs appear in the mdBook sidebar.
- Commit messages should be descriptive (e.g., `docs(<project>): add design skeleton with index and crosslinks`).

## Quality Checklist (for each PR)

- Folder lives under `docs/design/<project>/`.
- Central index exists and links to all subdocs.
- Docs are split into reasonably sized files (≈200 lines each).
- Crosslinks in place within the folder and to closely related topics.
- `docs/SUMMARY.md` updated to include the new design section.

---

See `docs/design/shared_power/` for an example that follows these conventions.

