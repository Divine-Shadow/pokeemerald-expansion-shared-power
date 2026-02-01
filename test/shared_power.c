#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "constants/battle.h"
#include "test/test.h"

TEST("Shared Power is gated by config and battle flag")
{
    u32 prevBattleTypeFlags = gBattleTypeFlags;

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER;
#if CONFIG_SHARED_POWER
    EXPECT(SharedPower_IsEnabled());
#else
    EXPECT(!SharedPower_IsEnabled());
#endif

    gBattleTypeFlags = 0;
    EXPECT(!SharedPower_IsEnabled());

    gBattleTypeFlags = prevBattleTypeFlags;
}
