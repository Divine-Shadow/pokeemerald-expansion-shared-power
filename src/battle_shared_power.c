#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "constants/battle.h"
#include "config/shared_power.h"

bool32 SharedPower_IsEnabled(void)
{
#if CONFIG_SHARED_POWER
    return (gBattleTypeFlags & BATTLE_TYPE_SHARED_POWER) != 0;
#else
    return FALSE;
#endif
}
