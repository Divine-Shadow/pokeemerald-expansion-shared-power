#ifndef GUARD_WILD_HELD_ITEM_BROKER_H
#define GUARD_WILD_HELD_ITEM_BROKER_H

#include "global.h"

struct Pokemon;

bool32 WildHeldItemBroker_IsEligibleMove(u16 move);
bool32 WildHeldItemBroker_IsEligibleAbility(u16 ability);
bool32 WildHeldItemBroker_IsEligibleMon(struct Pokemon *mon);
const u16 *WildHeldItemBroker_GetShopItems(void);
u16 WildHeldItemBroker_GetShopItemCount(void);
bool8 Script_IsWildHeldItemBrokerEligiblePartyMon(void);
void OpenWildHeldItemBrokerShop(void);

#endif // GUARD_WILD_HELD_ITEM_BROKER_H
