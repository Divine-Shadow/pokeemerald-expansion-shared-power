#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "constants/abilities.h"
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

TEST("HasActiveAbility respects suppression for native abilities")
{
    u32 prevBattleTypeFlags = gBattleTypeFlags;
    u32 prevBattlersCount = gBattlersCount;
    u32 prevBattlerAttacker = gBattlerAttacker;
    u16 prevCurrentMove = gCurrentMove;
    struct BattlePokemon prevMon = gBattleMons[0];

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER;
    gBattlersCount = 1;
    gBattlerAttacker = 0;
    gCurrentMove = MOVE_NONE;

    memset(&gBattleMons[0], 0, sizeof(gBattleMons[0]));
    gBattleMons[0].hp = 1;
    gBattleMons[0].ability = ABILITY_DAMP;

    EXPECT(HasActiveAbility(0, ABILITY_DAMP));

    gBattleMons[0].volatiles.gastroAcid = TRUE;
    EXPECT(!HasActiveAbility(0, ABILITY_DAMP));

    gBattleMons[0] = prevMon;
    gCurrentMove = prevCurrentMove;
    gBattlerAttacker = prevBattlerAttacker;
    gBattlersCount = prevBattlersCount;
    gBattleTypeFlags = prevBattleTypeFlags;
}
