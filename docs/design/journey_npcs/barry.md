# Barry

## Concept

Barry is a recurring berry specialist. He starts by using berries as a stall crutch, then learns to match berries to timing windows, and eventually builds teams that turn berry consumption into win conditions.

## Player Impact

Barry should make held berries feel like a real team-building axis before competitive berries are broadly purchasable. His rewards give players early access to selected competitive berries, but the player still has to plant, multiply, and test them.

The player lesson should progress from "healing buys time" to "timed item consumption can change a matchup" to "berry abilities can define a whole battle plan."

## Journey Beats

- Early Barry: repeats the same safe berry across most of his team. He is resilient but low pressure.
- Middle Barry: matches berries to specific jobs such as ChestoRest, Lum setup, and type-resist lures.
- Late Barry: uses berry engines such as Harvest, Cheek Pouch, Gluttony, Unburden, Cud Chew, or Symbiosis.
- Final Barry: may justify a custom berry-aware AI flag for Rest + Chesto, Harvest reuse, or pinch-berry setup timing.

## Battle Identity

Early Barry should be beatable by pressure and setup because he delays losses more than he creates wins. Middle Barry should punish careless status, single-coverage attacks, and unplanned setup. Late Barry should be a focused archetype encounter where the player recognizes berry loops and cuts them off.

Candidate late-game sets:

- Rest + Chesto bulky setup
- Harvest + Sitrus or Lum sustain
- Cheek Pouch + Sitrus recovery swing
- Gluttony + Salac/Liechi/Petaya timing
- Unburden after berry consumption
- Symbiosis passing a second berry in doubles, if Barry gets a double-battle beat

## Rewards

Reward pacing should avoid handing out the strongest self-multiplying berries too early.

- Early reward: Chesto, Sitrus, or Lum access.
- Middle reward: a small type-resist berry bundle or one pinch berry.
- Late reward: selected competitive berries such as Salac, Liechi, Petaya, Custap, Kee, or Maranga.

## Dialogue Beats

Early intro: "I brought enough berries for everyone! That means I can't lose, right?"

Early defeat: "Wait... I used all my berries and still lost?"

Early post-battle: "Maybe a berry should do more than delay the problem."

Middle intro: "This time I matched every berry to a plan. Probably."

Middle defeat: "The plan was better. The battling still needs work."

Middle post-battle: "A berry is a promise. The hard part is knowing when it will be kept."

Final intro: "I used to hide behind berries. Now I build around them."

Final defeat: "Perfect harvest. Imperfect trainer."

Final post-battle: "Take these. Plant them, test them, lose with them, then win with them."

## Asset Direction

Barry should read as practical rather than mascot-like: green field clothes, berry-red pouch accents, and a compact trainer pose that suggests a prepared gardener or breeder. The pilot asset can be a recolored, repo-native trainer sprite and overworld sprite as long as the palette clearly distinguishes him from stock NPCs.

Current planned asset ids:

- Trainer pic: `TRAINER_PIC_BERRY_BARRY`
- Object event gfx: `OBJ_EVENT_GFX_BERRY_BARRY`

## Sprite Proof

Barry has an automation-only sprite preview command that renders a combined sheet plus individual full-frame snapshots for the trainer front and each overworld facing.

Latest local proof artifacts:

- `build/journey_npc_sprites/barry/barry_all.png`
- `build/journey_npc_sprites/barry/barry_trainer_front.png`
- `build/journey_npc_sprites/barry/barry_overworld_south.png`
- `build/journey_npc_sprites/barry/barry_overworld_north.png`
- `build/journey_npc_sprites/barry/barry_overworld_west.png`
- `build/journey_npc_sprites/barry/barry_overworld_east.png`

## Placement Status

Barry is intentionally unplaced for the first sprite/documentation spike. Placement should be decided after the sprites and trainer id are proven, so the first map edit can also choose the right reward and progression gate.
