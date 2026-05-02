# Journey NPCs

Journey NPCs are recurring optional characters whose teams, dialogue, rewards, and mechanical lessons change across the game. Use this folder as the planning source before placing them on maps.

## Record Structure

Each NPC record should cover:

- Concept: the short player-facing identity.
- Player impact: what the NPC teaches, unlocks, or changes in routing.
- Journey beats: early, middle, late, and optional postgame states.
- Battle identity: team-building rule, AI needs, and expected counterplay.
- Rewards: item or system unlocks, including economy risk.
- Dialogue beats: intro, defeat, and post-battle tone for each meeting.
- Asset needs: overworld sprite, trainer front sprite, palette notes, and snapshot proof paths.
- Placement status: candidate maps, gating, rematch state, and unresolved questions.

## Execution Plans

Plan A is the content pass. It creates or updates the NPC record, classifies player-visible information, and records open placement questions without editing maps.

Plan B is the integration pass. It reserves trainer ids, registers assets, proves sprite rendering in a ROM snapshot, and only then moves to map placement and battle scripting.

Plan C is the mechanics pass for NPCs that need custom AI, battle effects, or reward logic. Keep this separate from placement unless the mechanic is already proven.

## Asset Proof Notes

For overworld sprites, register both the PNG and an explicit graphics rule when the sheet uses animation-frame metatiles. Barry's 16x32 overworld sheet uses `-mwidth 2 -mheight 4` so each facing frame is emitted contiguously for `overworld_ascending_frames`.

Snapshot proofs can live under ignored `build/journey_npc_sprites/<npc>/` paths. Keep the source record linked to the latest proof path so later placement work starts from already-verified sprite ids.

## Spoiler Policy

Journey NPC existence and broad reward categories can be public. Exact teams, locations, rematch timing, hidden rewards, and dialogue should stay blind-friendly until intentionally surfaced.
