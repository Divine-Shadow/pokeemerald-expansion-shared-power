# State Model

[Index](./README.md) · Related: [API](./api.md), [Acquisition & Switch-In](./acquisition_switchin.md)

Store Shared Power state inside the battle runtime to keep it ephemeral and localized.

## Data Structures

- `BattleStruct` additions (see `include/battle.h`):
  - `trainer_pools`: array indexed by logical trainer index; each element is a bitset of `ABILITIES_COUNT` (dedupe fast path) representing ability membership.
  - `pending_switchin_abilities[battler]`: small queue or bitset for abilities to activate on that battler’s switch-in.

## Trainer Identity

Define a helper that maps a battler to a trainer index consistent with multi battles:

- `SharedPower_GetTrainerIndex(u8 battler)`:
  - Same-trainer partners share an index (`IsPartnerMonFromSameTrainer(battler)` true).
  - Multi battles split allies/opponents into distinct indices per seat when they are from different trainers.

## Initialization & Reset

- Clear all pools and pending queues when `gBattleStruct` is created.
- Clear on battle end (no persistence to save data).

See also: [Core API](./api.md), [Acquisition & Switch-In Flow](./acquisition_switchin.md).
