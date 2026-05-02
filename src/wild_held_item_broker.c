#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "shop.h"
#include "wild_held_item_broker.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"

static EWRAM_DATA u16 sWildHeldItemBrokerShopItems[ITEMS_COUNT + 1] = {0};
static EWRAM_DATA u16 sWildHeldItemBrokerShopItemCount = 0;
static EWRAM_DATA bool8 sWildHeldItemBrokerShopItemsBuilt = FALSE;

static void WildHeldItemBroker_BuildShopItems(void);
static bool8 WildHeldItemBroker_TryAppendShopItem(u16 item);
static bool8 WildHeldItemBroker_HasShopItem(u16 item);

bool32 WildHeldItemBroker_IsEligibleMove(u16 move)
{
    switch (move)
    {
    case MOVE_THIEF:
    case MOVE_COVET:
    case MOVE_TRICK:
    case MOVE_SWITCHEROO:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 WildHeldItemBroker_IsEligibleAbility(u16 ability)
{
    switch (ability)
    {
    case ABILITY_MAGICIAN:
    case ABILITY_PICKPOCKET:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 WildHeldItemBroker_IsEligibleMon(struct Pokemon *mon)
{
    u32 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);

    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    if (WildHeldItemBroker_IsEligibleAbility(GetMonAbility(mon)))
        return TRUE;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (WildHeldItemBroker_IsEligibleMove(GetMonData(mon, MON_DATA_MOVE1 + i)))
            return TRUE;
    }

    return FALSE;
}

const u16 *WildHeldItemBroker_GetShopItems(void)
{
    WildHeldItemBroker_BuildShopItems();
    return sWildHeldItemBrokerShopItems;
}

u16 WildHeldItemBroker_GetShopItemCount(void)
{
    WildHeldItemBroker_BuildShopItems();
    return sWildHeldItemBrokerShopItemCount;
}

bool8 Script_IsWildHeldItemBrokerEligiblePartyMon(void)
{
    if (gSpecialVar_0x8004 >= PARTY_SIZE)
        return FALSE;

    return WildHeldItemBroker_IsEligibleMon(&gPlayerParty[gSpecialVar_0x8004]);
}

void OpenWildHeldItemBrokerShop(void)
{
    CreateFreePokemartMenu(WildHeldItemBroker_GetShopItems());
    ScriptContext_Stop();
}

static void WildHeldItemBroker_BuildShopItems(void)
{
    u16 species;

    if (sWildHeldItemBrokerShopItemsBuilt)
        return;

    sWildHeldItemBrokerShopItemCount = 0;

    for (species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        WildHeldItemBroker_TryAppendShopItem(gSpeciesInfo[species].itemCommon);
        WildHeldItemBroker_TryAppendShopItem(gSpeciesInfo[species].itemRare);
    }

    sWildHeldItemBrokerShopItems[sWildHeldItemBrokerShopItemCount] = ITEM_NONE;
    sWildHeldItemBrokerShopItemsBuilt = TRUE;
}

static bool8 WildHeldItemBroker_TryAppendShopItem(u16 item)
{
    if (item == ITEM_NONE || item >= ITEMS_COUNT || WildHeldItemBroker_HasShopItem(item))
        return FALSE;

    sWildHeldItemBrokerShopItems[sWildHeldItemBrokerShopItemCount++] = item;
    return TRUE;
}

static bool8 WildHeldItemBroker_HasShopItem(u16 item)
{
    u16 i;

    for (i = 0; i < sWildHeldItemBrokerShopItemCount; i++)
    {
        if (sWildHeldItemBrokerShopItems[i] == item)
            return TRUE;
    }

    return FALSE;
}
