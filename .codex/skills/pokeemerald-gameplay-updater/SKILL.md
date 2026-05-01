---
name: pokeemerald-gameplay-updater
description: Update gameplay content in this pokeemerald-expansion repo, including Pokemon, moves, abilities, learnsets, evolutions, trainers, maps, items, encounters, dialogue, events, and related tests, while maintaining spoiler-aware public and blind-friendly player change lists.
---

# Pokeemerald Gameplay Updater

Use this skill for gameplay-content work in this repository. Keep technical implementation, public player knowledge, and blind-playthrough discovery notes separated.

## Core workflow

1. Load [references/gameplay-file-map.md](references/gameplay-file-map.md) before editing unfamiliar gameplay areas.
2. Classify each player-visible change before or while implementing it:
   - Public: players should know before planning a run.
   - Blind-friendly: players should discover naturally during play.
   - Mixed: split the public rule or mechanic from the spoiler-sensitive placement, team, script, or reveal.
3. Search for existing player-facing gameplay note files before creating new ones:
   `rg -n "Public Gameplay|Blind|Spoiler|gameplay changes|player-facing" docs .codex PATCH_NOTES.md`
4. If no better repo convention exists, maintain these files:
   - `docs/gameplay_changes_public.md`
   - `docs/gameplay_changes_blind.md`
5. Keep `PATCH_NOTES.md` current at the top of the file after every code, data, or docs change, using the repository's required `- Area: short description (commit <short-hash>).` format.
6. Run the narrowest useful validation first, then broader validation when shared mechanics, generated data, or battle behavior changed. Prefer `make check` for risky battle/gameplay logic and `make` for generated data or ROM integration.

## Spoiler policy

Treat these as public unless the user explicitly says otherwise:

- Pokemon stat, type, ability-slot, form, learnset, tutor, TM/HM, egg-move, or evolution-method changes.
- Move type, category, power, accuracy, PP, priority, flags, effect, targeting, or animation-relevant behavior changes.
- Ability list membership, ability mechanics, item mechanics, global config, rule toggles, and battle-system behavior.
- Broad availability rules that affect teambuilding before a run.

Treat these as blind-friendly by default:

- Trainer teams, gym sets, Elite Four/champion sets, rival teams, battle rematches, trainer AI surprises, and trainer party pools.
- Added, removed, or moved trainers and NPCs.
- Item-ball, hidden-item, gift-item, shop-inventory, reward, and pickup-location changes.
- Map scripts, story events, optional events, puzzle states, signposts, dialogue branches, and new dialogue.
- Exact wild encounter placements when the change is meant to reward exploration rather than pre-run planning.

For ambiguous changes, default to blind-friendly if the information answers "where is it or who has it?" Default to public if it answers "what are the rules or what can this Pokemon/move/item do?"

## Player-note writing

Write player notes in player terms, not implementation terms. Prefer "Strength is now a Rock-type move with 75 power" over constants or filenames.

Public entries should be concise and useful before a run. Avoid exact trainer rosters, item coordinates, event flags, and surprise dialogue in public notes.

Pokemon move-access, learnset, tutor, TM/HM, and egg-move changes must be explicit in the public/spoiler-free notes. Prefer a small table or structured list so players can see who got what, for example:

| Pokemon | Move | Access | Notes |
| --- | --- | --- | --- |
| Yanmega | Hurricane | Level 57 | Added level-up move |

Blind-friendly entries may be explicit, but keep them in the spoiler-protected file. Make the first lines of that file clearly warn that it contains discovery spoilers.

When a single change has both aspects, write two entries. Example: a gym leader now uses the shared-power rule publicly, but the exact gym team belongs in blind-friendly notes.

## Implementation habits

- Use `rg` and `rg --files` to locate constants, generated files, and references before editing.
- Prefer source inputs over generated outputs. For example, edit `src/data/trainers.party`, not generated `src/data/trainers.h`.
- Follow existing data style and keep diffs focused.
- Add or update tests when mechanics, battle behavior, parser behavior, or generated contracts change.
- In final responses, mention which player-facing notes were updated and whether any blind-friendly details were intentionally kept out of the public summary.
