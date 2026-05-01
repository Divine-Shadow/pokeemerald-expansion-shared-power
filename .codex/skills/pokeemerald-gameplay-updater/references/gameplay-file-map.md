# Gameplay File Map

Use this map to start investigation. Confirm local references with `rg` before editing because expansion data may be generated or split by generation.

## Pokemon Data

- Species constants: `include/constants/species.h`
- Species config and enabled generations/forms: `include/config/species_enabled.h`, `include/config/pokemon.h`
- Main species records: `src/data/pokemon/species_info.h`
- Generation family records: `src/data/pokemon/species_info/gen_*_families.h`
- Forms and form changes: `src/data/pokemon/form_species_tables.h`, `src/data/pokemon/form_change_tables.h`, `src/data/pokemon/form_change_table_pointers.h`
- Evolution methods live on species records through `.evolutions = EVOLUTION(...)`; runtime evolution logic is in `src/pokemon.c`
- Dex text and ordering: `src/data/pokemon/species_info/shared_dex_text.h`, `src/data/pokemon/pokedex_orders.h`

Player-note default: stats, typing, abilities, forms, learnsets, and evolution methods are public.

## Moves, Abilities, and Battle Mechanics

- Move constants: `include/constants/moves.h`
- Move data: `src/data/moves_info.h`
- Move effects: `include/constants/battle_move_effects.h`, `src/data/battle_move_effects.h`
- Ability constants and data: `include/constants/abilities.h`, `src/data/abilities.h`
- Battle scripts: `data/battle_scripts_1.s`, `data/battle_scripts_2.s`
- Battle logic: `src/battle_script_commands.c`, `src/battle_util.c`, `src/battle_main.c`, `src/battle_controller_*.c`
- AI behavior: `src/battle_ai_main.c`, `src/battle_ai_util.c`, `src/battle_ai_script_commands.c`, `src/battle_ai_switch_items.c`
- Animations: `data/battle_anim_scripts.s`, `src/battle_anim_*.c`, `src/data/battle_anim.h`
- Shared Power mechanics: `src/battle_shared_power.c`, `include/battle_shared_power.h`

Player-note default: move, ability, and global battle-rule changes are public. Exact enemy usage of those mechanics is blind-friendly.

## Learnsets and Teachable Moves

- Level-up learnsets: `src/data/pokemon/level_up_learnsets/gen_*.h`
- Teachable learnsets: `src/data/pokemon/teachable_learnsets.h`
- Egg moves: `src/data/pokemon/egg_moves.h`
- TM/HM constants: `include/constants/tms_hms.h`
- Move tutor scripts and text: `data/scripts/move_tutors.inc`, `data/text/move_tutors.inc`

Player-note default: move access changes are public. Tutor NPC location or event gating is blind-friendly unless the user wants a public availability chart.

## Trainers

- Source trainer data: `src/data/trainers.party`
- Generated trainer C output: `src/data/trainers.h` and related generated data. Do not edit generated trainer output directly.
- Trainer constants: `include/constants/trainers.h`, `include/constants/trainer_types.h`
- Trainer pools: `src/data/battle_pool_rules.h`, `src/trainer_pools.c`, `include/trainer_pools.h`
- Trainer battle scripts: `data/scripts/trainer_battle.inc`, map-local `data/maps/*/scripts.inc`
- Trainer text: `data/text/trainers.inc` and map-local script text
- Trainer tutorials: `docs/tutorials/how_to_trainer_party_pool.md`, `docs/tutorials/how_to_new_trainer_slide.md`

Player-note default: trainer sets, gym rosters, rematches, party pools, and added trainer placement are blind-friendly.

## Encounters

- Wild encounter data: `src/data/wild_encounters.json`
- Wild encounter logic: `src/wild_encounter.c`, `include/wild_encounter.h`
- Encounter constants: `include/constants/wild_encounter.h`
- Time-of-day encounter guidance: `docs/tutorials/how_to_time_of_day_encounters.md`

Player-note default: broad availability rules are public; exact route/cave/rod placements are blind-friendly when intended for discovery.

## Items, Shops, and Rewards

- Item constants: `include/constants/items.h`, `include/constants/item.h`
- Item config and effects: `include/config/item.h`, `include/constants/item_effects.h`, `src/data/items.h`, `src/data/pokemon/item_effects.h`
- Item graphics: `src/data/item_icon_table.h`, `src/data/graphics/items.h`
- Common item scripts/text: `data/scripts/item_ball_scripts.inc`, `data/scripts/obtain_item.inc`, `data/text/obtain_item.inc`, `data/text/mart_clerk.inc`
- Map-local item balls, hidden items, gifts, rewards, and shops: `data/maps/<MapName>/map.json`, `data/maps/<MapName>/scripts.inc`

Player-note default: item mechanics are public; exact locations, shop inventory changes, gifts, and rewards are blind-friendly.

## Maps, Events, Dialogue, and Story Flow

- Map list include: `data/maps.s`
- Map objects, warps, signs, coordinates: `data/maps/<MapName>/map.json`
- Map scripts and local text: `data/maps/<MapName>/scripts.inc`
- Shared scripts: `data/scripts/*.inc`, `data/event_scripts.s`
- Shared text: `data/text/*.inc`
- Script command table: `data/script_cmd_table.inc`
- Map constants: `include/constants/maps.h`, `include/constants/map_scripts.h`

Player-note default: new dialogue, optional branches, story events, object placement, puzzle changes, and surprise map content are blind-friendly.

## Tests

- Core test framework: `include/test/test.h`, `test/test_runner.c`
- Pokemon tests: `test/pokemon.c`, `test/species.c`
- Battle tests: `test/battle/`
- Trainer test helpers: `test/battle/trainer_control.c`, `test/battle/trainer_control.h`

Add focused tests for mechanics and battle outcomes. Data-only trainer, map, encounter, or dialogue edits may be validated by build unless they change shared logic or generated parser behavior.
