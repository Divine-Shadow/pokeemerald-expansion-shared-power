#include "global.h"
#include "pokemon.h"
#include "test/test.h"
#include "wild_held_item_broker.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"

static void ClearMonMoves(struct Pokemon *mon)
{
    u32 i;
    u16 move = MOVE_NONE;

    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonData(mon, MON_DATA_MOVE1 + i, &move);
}

static bool32 ShopItemsContain(u16 item)
{
    const u16 *items = WildHeldItemBroker_GetShopItems();
    u16 i;

    for (i = 0; items[i] != ITEM_NONE; i++)
    {
        if (items[i] == item)
            return TRUE;
    }

    return FALSE;
}

TEST("Wild held item broker accepts Pokemon with item-stealing moves")
{
    u16 move;
    struct Pokemon mon;

    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    PARAMETRIZE { move = MOVE_TRICK; }
    PARAMETRIZE { move = MOVE_SWITCHEROO; }

    CreateMon(&mon, SPECIES_WOBBUFFET, 30, 0, FALSE, 0, OT_ID_PRESET, 0);
    ClearMonMoves(&mon);
    SetMonData(&mon, MON_DATA_MOVE3, &move);

    EXPECT(WildHeldItemBroker_IsEligibleMon(&mon));
}

TEST("Wild held item broker accepts Pokemon with item-stealing abilities")
{
    u16 species;
    u16 ability;
    u8 abilityNum;
    struct Pokemon mon;

    PARAMETRIZE { species = SPECIES_FENNEKIN; ability = ABILITY_MAGICIAN; abilityNum = 2; }
    PARAMETRIZE { species = SPECIES_SNEASEL; ability = ABILITY_PICKPOCKET; abilityNum = 2; }

    CreateMon(&mon, species, 30, 0, FALSE, 0, OT_ID_PRESET, 0);
    ClearMonMoves(&mon);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);

    ASSUME(GetMonAbility(&mon) == ability);
    EXPECT(WildHeldItemBroker_IsEligibleMon(&mon));
}

TEST("Wild held item broker rejects scouting or non-acquiring options")
{
    u16 species;
    u16 move;
    u16 ability;
    u8 abilityNum;
    struct Pokemon mon;

    PARAMETRIZE { species = SPECIES_SENTRET; move = MOVE_NONE; ability = ABILITY_FRISK; abilityNum = 2; }
    PARAMETRIZE { species = SPECIES_ZIGZAGOON; move = MOVE_NONE; ability = ABILITY_PICKUP; abilityNum = 0; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; move = MOVE_KNOCK_OFF; ability = ABILITY_SHADOW_TAG; abilityNum = 0; }

    CreateMon(&mon, species, 30, 0, FALSE, 0, OT_ID_PRESET, 0);
    ClearMonMoves(&mon);
    SetMonData(&mon, MON_DATA_MOVE1, &move);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);

    ASSUME(GetMonAbility(&mon) == ability);
    EXPECT(!WildHeldItemBroker_IsEligibleMon(&mon));
}

TEST("Wild held item broker rejects eggs")
{
    u8 isEgg = TRUE;
    u16 move = MOVE_THIEF;
    struct Pokemon mon;

    CreateMon(&mon, SPECIES_WOBBUFFET, 30, 0, FALSE, 0, OT_ID_PRESET, 0);
    ClearMonMoves(&mon);
    SetMonData(&mon, MON_DATA_MOVE1, &move);
    SetMonData(&mon, MON_DATA_IS_EGG, &isEgg);

    EXPECT(!WildHeldItemBroker_IsEligibleMon(&mon));
}

TEST("Wild held item broker shop contains distinct wild held items")
{
    const u16 *items = WildHeldItemBroker_GetShopItems();
    u16 count = WildHeldItemBroker_GetShopItemCount();
    u16 i, j;

    ASSUME(gSpeciesInfo[SPECIES_PIKACHU].itemRare == ITEM_LIGHT_BALL);
    ASSUME(gSpeciesInfo[SPECIES_ABRA].itemRare == ITEM_TWISTED_SPOON);

    EXPECT_GT(count, 0);
    EXPECT_EQ(items[count], ITEM_NONE);
    EXPECT(ShopItemsContain(ITEM_LIGHT_BALL));
    EXPECT(ShopItemsContain(ITEM_TWISTED_SPOON));

    for (i = 0; i < count; i++)
    {
        EXPECT_NE(items[i], ITEM_NONE);
        EXPECT_LT(items[i], ITEMS_COUNT);

        for (j = i + 1; j < count; j++)
            EXPECT_NE(items[i], items[j]);
    }
}

TEST("Wild held item broker excludes strict competitive items")
{
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].itemCommon == ITEM_LEFTOVERS);

    EXPECT(!ShopItemsContain(ITEM_CHOICE_BAND));
    EXPECT(!ShopItemsContain(ITEM_CHOICE_SPECS));
    EXPECT(!ShopItemsContain(ITEM_CHOICE_SCARF));
    EXPECT(!ShopItemsContain(ITEM_EVIOLITE));
    EXPECT(!ShopItemsContain(ITEM_FOCUS_SASH));
    EXPECT(!ShopItemsContain(ITEM_LEFTOVERS));
    EXPECT(!ShopItemsContain(ITEM_LIFE_ORB));
    EXPECT(!ShopItemsContain(ITEM_ASSAULT_VEST));
}
