#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "test/test.h"
#include "constants/battle.h"
#include "constants/condition_coach.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/species.h"

static void SetConditionCoachBadgeCount(u8 badgeCount)
{
    u8 i;

    for (i = 0; i < NUM_BADGES; i++)
        FlagClear(gBadgeFlags[i]);

    for (i = 0; i < badgeCount; i++)
        FlagSet(gBadgeFlags[i]);
}

static void CreateConditionCoachTestMon(u16 species)
{
    u32 abilityNum = 0;

    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&gPlayerParty[0], MON_DATA_ABILITY_NUM, &abilityNum);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = CONDITION_COACH_CHOICE_BURN;
    gSpecialVar_0x8006 = CONDITION_COACH_HINT_NONE;
    SetConditionCoachBadgeCount(NUM_BADGES);
}

static u16 TryConditionCoachChoice(u16 choice)
{
    gSpecialVar_0x8005 = choice;
    return ConditionCoach_TryApplyStatus();
}

TEST("Condition Coach applies burn")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_BURN);
}

TEST("Condition Coach requires two badges for burn")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetConditionCoachBadgeCount(1);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_LOCKED);
    EXPECT_EQ(gSpecialVar_0x8006, 2);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);

    SetConditionCoachBadgeCount(2);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_BURN);
}

TEST("Condition Coach applies regular poison")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetConditionCoachBadgeCount(0);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_POISON), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_POISON);
}

TEST("Condition Coach applies paralysis")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_PARALYSIS), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_PARALYSIS);
}

TEST("Condition Coach requires four badges for paralysis")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetConditionCoachBadgeCount(3);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_PARALYSIS), CONDITION_COACH_RESULT_LOCKED);
    EXPECT_EQ(gSpecialVar_0x8006, 4);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);

    SetConditionCoachBadgeCount(4);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_PARALYSIS), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_PARALYSIS);
}

TEST("Condition Coach applies one-turn Rest-wake sleep")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_REST_WAKE), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_SLEEP_TURN(1));
    EXPECT_EQ(gSpecialVar_0x8006, CONDITION_COACH_HINT_REST_WAKE);
}

TEST("Condition Coach requires six badges for Rest-wake sleep")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetConditionCoachBadgeCount(5);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_REST_WAKE), CONDITION_COACH_RESULT_LOCKED);
    EXPECT_EQ(gSpecialVar_0x8006, 6);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);

    SetConditionCoachBadgeCount(6);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_REST_WAKE), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_SLEEP_TURN(1));
}

TEST("Condition Coach clears status")
{
    u32 status = STATUS1_BURN;

    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetConditionCoachBadgeCount(0);
    SetMonData(&gPlayerParty[0], MON_DATA_STATUS, &status);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_CLEAR), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);
    EXPECT_EQ(gSpecialVar_0x8006, CONDITION_COACH_HINT_CLEAR);
}

TEST("Condition Coach reports already-clear Pokemon")
{
    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_CLEAR), CONDITION_COACH_RESULT_ALREADY_CLEAR);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);
}

TEST("Condition Coach does not overwrite existing status")
{
    u32 status = STATUS1_BURN;

    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetMonData(&gPlayerParty[0], MON_DATA_STATUS, &status);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_POISON), CONDITION_COACH_RESULT_ALREADY_STATUS);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_BURN);
}

TEST("Condition Coach rejects Eggs")
{
    u32 isEgg = TRUE;

    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], SPECIES_PICHU, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&gPlayerParty[0], MON_DATA_IS_EGG, &isEgg);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = 0;

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_EGG);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);
}

TEST("Condition Coach rejects fainted Pokemon")
{
    u32 hp = 0;

    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetMonData(&gPlayerParty[0], MON_DATA_HP, &hp);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_FAINTED);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_STATUS), STATUS1_NONE);
}

TEST("Condition Coach warns when a held berry will cure the selected condition")
{
    u32 item = ITEM_RAWST_BERRY;

    CreateConditionCoachTestMon(SPECIES_WOBBUFFET);
    SetMonData(&gPlayerParty[0], MON_DATA_HELD_ITEM, &item);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(gSpecialVar_0x8006, CONDITION_COACH_HINT_CURING_ITEM);
}

TEST("Condition Coach gives Guts tailored advice")
{
    CreateConditionCoachTestMon(SPECIES_MACHOP);

    EXPECT_EQ(TryConditionCoachChoice(CONDITION_COACH_CHOICE_BURN), CONDITION_COACH_RESULT_APPLIED);
    EXPECT_EQ(gSpecialVar_0x8006, CONDITION_COACH_HINT_GUTS);
}
