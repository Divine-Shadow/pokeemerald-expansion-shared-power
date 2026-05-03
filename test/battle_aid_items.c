#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/species.h"

static void InitBattleAidItemTestState(void)
{
    u32 i;

    ASSUME(B_FLAG_NO_BATTLE_AID_ITEMS != 0);

    FlagClear(B_FLAG_NO_BATTLE_AID_ITEMS);
#if B_FLAG_NO_CATCHING != 0
    FlagClear(B_FLAG_NO_CATCHING);
#endif
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();

    gBattleTypeFlags = 0;
    gBattlersCount = 2;
    gBattlerInMenuId = B_POSITION_PLAYER_LEFT;
    gAbsentBattlerFlags = 0;
    gSideStatuses[B_SIDE_PLAYER] = 0;
    gSideStatuses[B_SIDE_OPPONENT] = 0;
    gPartyMenu.slotId = 0;

    memset(gBattleMons, 0, sizeof(gBattleMons));
    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        gBattlerPositions[i] = i;

    gBattleMons[B_POSITION_PLAYER_LEFT].hp = 1;
    gBattleMons[B_POSITION_PLAYER_LEFT].maxHP = 1;
    gBattleMons[B_POSITION_OPPONENT_LEFT].hp = 1;
    gBattleMons[B_POSITION_OPPONENT_LEFT].maxHP = 1;

    for (i = 0; i < NUM_BATTLE_STATS; i++)
        gBattleMons[B_POSITION_PLAYER_LEFT].statStages[i] = DEFAULT_STAT_STAGE;
}

static struct Pokemon CreateDamagedBattleAidItemTestMon(void)
{
    u32 hp;
    struct Pokemon mon;

    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, FALSE, 0, OT_ID_PRESET, 0);
    hp = GetMonData(&mon, MON_DATA_MAX_HP) - 1;
    SetMonData(&mon, MON_DATA_HP, &hp);

    return mon;
}

static void CleanupBattleAidItemTestState(void)
{
    FlagClear(B_FLAG_NO_BATTLE_AID_ITEMS);
#if B_FLAG_NO_CATCHING != 0
    FlagClear(B_FLAG_NO_CATCHING);
#endif
}

TEST("No combat aid flag blocks X items in battle")
{
    bool32 cannotUse;

    ASSUME(GetItemBattleUsage(ITEM_X_ATTACK) == EFFECT_ITEM_INCREASE_STAT);

    InitBattleAidItemTestState();
    FlagSet(B_FLAG_NO_BATTLE_AID_ITEMS);
    cannotUse = CannotUseItemsInBattle(ITEM_X_ATTACK, NULL);
    CleanupBattleAidItemTestState();

    EXPECT(cannotUse);
}

TEST("No combat aid flag blocks medicine in battle")
{
    bool32 cannotUse;
    struct Pokemon mon;

    ASSUME(GetItemBattleUsage(ITEM_POTION) == EFFECT_ITEM_RESTORE_HP);

    InitBattleAidItemTestState();
    mon = CreateDamagedBattleAidItemTestMon();
    FlagSet(B_FLAG_NO_BATTLE_AID_ITEMS);
    cannotUse = CannotUseItemsInBattle(ITEM_POTION, &mon);
    CleanupBattleAidItemTestState();

    EXPECT(cannotUse);
}

TEST("No combat aid flag allows Poke Balls through normal catching rules")
{
    bool32 cannotUse;

    ASSUME(GetItemBattleUsage(ITEM_POKE_BALL) == EFFECT_ITEM_THROW_BALL);

    InitBattleAidItemTestState();
    FlagSet(B_FLAG_NO_BATTLE_AID_ITEMS);
    cannotUse = CannotUseItemsInBattle(ITEM_POKE_BALL, NULL);
    CleanupBattleAidItemTestState();

    EXPECT(!cannotUse);
}
