#ifndef GUARD_BATTLE_SHARED_POWER_H
#define GUARD_BATTLE_SHARED_POWER_H

#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"

#define SHARED_POWER_POOL_MAX ABILITIES_COUNT
#define SHARED_POWER_POOL_BYTES ((ABILITIES_COUNT + 7) / 8)
#define SHARED_POWER_TRAINER_POOL_COUNT MAX_BATTLERS_COUNT

bool32 SharedPower_IsEnabled(void);
u8 SharedPower_GetTrainerIndex(u8 battler);
bool32 SharedPower_AddToPool(u8 trainerIdx, u16 ability);
bool32 SharedPower_TrainerHasAbility(u8 trainerIdx, u16 ability);
void SharedPower_ClearBattleState(void);
void SharedPower_ResetSwitchInQueue(u32 battler);
bool32 IsAbilitySuppressedFor(u32 battler, u16 ability, bool32 ignoreMoldBreaker, bool32 noAbilityShield);
bool32 SharedPower_IsEligibleFor(u32 battler, u16 ability);
bool32 HasActiveAbility(u32 battler, u16 ability);
bool32 ForEachEffectiveAbility(u32 battler, bool32 (*cb)(u16 ability));
bool32 SharedPower_TrySwitchInAbilities(u32 battler);

#endif // GUARD_BATTLE_SHARED_POWER_H
