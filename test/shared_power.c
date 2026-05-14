#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "battle_setup.h"
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

TEST("Wild battle setup enables Shared Power")
{
    u32 wildFlags = BattleSetup_ApplySharedPowerToWildBattleFlags(0);
    u32 doubleWildFlags = BattleSetup_ApplySharedPowerToWildBattleFlags(BATTLE_TYPE_DOUBLE);
    u32 roamerFlags = BattleSetup_ApplySharedPowerToWildBattleFlags(BATTLE_TYPE_ROAMER);

#if CONFIG_SHARED_POWER && !TESTING
    EXPECT_EQ(wildFlags, BATTLE_TYPE_SHARED_POWER);
    EXPECT_EQ(doubleWildFlags, BATTLE_TYPE_DOUBLE | BATTLE_TYPE_SHARED_POWER);
    EXPECT_EQ(roamerFlags, BATTLE_TYPE_ROAMER | BATTLE_TYPE_SHARED_POWER);
#else
    EXPECT_EQ(wildFlags, 0);
    EXPECT_EQ(doubleWildFlags, BATTLE_TYPE_DOUBLE);
    EXPECT_EQ(roamerFlags, BATTLE_TYPE_ROAMER);
#endif
}

TEST("Shared Power pools tag and multi partners by side")
{
    u32 prevBattleTypeFlags = gBattleTypeFlags;
    u8 prevPositions[MAX_BATTLERS_COUNT];

    memcpy(prevPositions, gBattlerPositions, sizeof(prevPositions));
    gBattlerPositions[B_BATTLER_0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[B_BATTLER_1] = B_POSITION_OPPONENT_LEFT;
    gBattlerPositions[B_BATTLER_2] = B_POSITION_PLAYER_RIGHT;
    gBattlerPositions[B_BATTLER_3] = B_POSITION_OPPONENT_RIGHT;

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER;
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_0), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_2), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_1), B_SIDE_OPPONENT);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_3), B_SIDE_OPPONENT);

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER | BATTLE_TYPE_INGAME_PARTNER;
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_0), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_2), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_1), B_BATTLER_1);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_3), B_BATTLER_3);

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER | BATTLE_TYPE_TWO_OPPONENTS;
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_0), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_2), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_1), B_SIDE_OPPONENT);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_3), B_SIDE_OPPONENT);

    gBattleTypeFlags = BATTLE_TYPE_SHARED_POWER | BATTLE_TYPE_MULTI;
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_0), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_2), B_SIDE_PLAYER);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_1), B_SIDE_OPPONENT);
    EXPECT_EQ(SharedPower_GetTrainerIndex(B_BATTLER_3), B_SIDE_OPPONENT);

    memcpy(gBattlerPositions, prevPositions, sizeof(prevPositions));
    gBattleTypeFlags = prevBattleTypeFlags;
}
