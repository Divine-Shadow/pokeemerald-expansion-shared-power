#ifndef GUARD_BATTLE_SHARED_POWER_H
#define GUARD_BATTLE_SHARED_POWER_H

#include "global.h"

bool32 SharedPower_IsEnabled(void);
u8 SharedPower_GetTrainerIndex(u8 battler);
bool32 SharedPower_AddToPool(u8 trainerIdx, u16 ability);
bool32 SharedPower_TrainerHasAbility(u8 trainerIdx, u16 ability);
bool32 IsAbilitySuppressedFor(u32 battler, u16 ability, bool32 ignoreMoldBreaker, bool32 noAbilityShield);
bool32 SharedPower_IsEligibleFor(u32 battler, u16 ability);
bool32 HasActiveAbility(u32 battler, u16 ability);
bool32 ForEachEffectiveAbility(u32 battler, bool32 (*cb)(u16 ability));

#endif // GUARD_BATTLE_SHARED_POWER_H
