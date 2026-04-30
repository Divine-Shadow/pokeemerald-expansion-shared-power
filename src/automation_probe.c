#include "global.h"
#include "automation_probe.h"
#include "automation_beacon.h"
#include "battle.h"
#include "battle_controllers.h"
#include "event_object_movement.h"
#include "field_message_box.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "item.h"
#include "main.h"
#include "palette.h"
#include "pokemon.h"
#include "script.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/species.h"

#if AUTOMATION_PROBE

#define AUTOMATION_PROBE_OBJECTIVE_POOCHYENA_SLOT0  (1 << 0)
#define AUTOMATION_PROBE_OBJECTIVE_MIGHTYENA_INTIMIDATE (1 << 1)

struct AutomationProbe gAutomationProbe;

static void AutomationProbe_SetCommandResult(u32 result, u32 resultArg0, u32 resultArg1)
{
    gAutomationProbe.lastCommandId = gAutomationProbe.commandId;
    gAutomationProbe.lastCommandArg0 = gAutomationProbe.commandArg0;
    gAutomationProbe.lastCommandArg1 = gAutomationProbe.commandArg1;
    gAutomationProbe.commandAckSequence = gAutomationProbe.commandSequence;
    gAutomationProbe.commandResult = result;
    gAutomationProbe.commandResultArg0 = resultArg0;
    gAutomationProbe.commandResultArg1 = resultArg1;
}

static void AutomationProbe_FillPartySlot(u32 slot)
{
    struct Pokemon *mon = &gPlayerParty[slot];
    u32 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG, NULL);

    gAutomationProbe.partySpecies[slot] = species;
    if (species == SPECIES_NONE || species == SPECIES_EGG)
    {
        gAutomationProbe.partyLevel[slot] = 0;
        gAutomationProbe.partyAbilityNum[slot] = 0;
        gAutomationProbe.partyAbility[slot] = 0;
        gAutomationProbe.partyHp[slot] = 0;
        gAutomationProbe.partyMaxHp[slot] = 0;
        return;
    }

    gAutomationProbe.partyLevel[slot] = GetMonData(mon, MON_DATA_LEVEL, NULL);
    gAutomationProbe.partyAbilityNum[slot] = GetMonData(mon, MON_DATA_ABILITY_NUM, NULL);
    gAutomationProbe.partyAbility[slot] = GetMonAbility(mon);
    gAutomationProbe.partyHp[slot] = GetMonData(mon, MON_DATA_HP, NULL);
    gAutomationProbe.partyMaxHp[slot] = GetMonData(mon, MON_DATA_MAX_HP, NULL);
}

static void AutomationProbe_FillEnemySlot0(void)
{
    u32 species;

    gAutomationProbe.enemyPartyCount = CalculateEnemyPartyCount();
    if (gAutomationProbe.enemyPartyCount == 0)
    {
        gAutomationProbe.enemy0Species = SPECIES_NONE;
        gAutomationProbe.enemy0Level = 0;
        gAutomationProbe.enemy0AbilityNum = 0;
        gAutomationProbe.enemy0Ability = ABILITY_NONE;
        return;
    }

    species = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES_OR_EGG, NULL);
    gAutomationProbe.enemy0Species = species;
    if (species == SPECIES_NONE || species == SPECIES_EGG)
    {
        gAutomationProbe.enemy0Level = 0;
        gAutomationProbe.enemy0AbilityNum = 0;
        gAutomationProbe.enemy0Ability = ABILITY_NONE;
        return;
    }

    gAutomationProbe.enemy0Level = GetMonData(&gEnemyParty[0], MON_DATA_LEVEL, NULL);
    gAutomationProbe.enemy0AbilityNum = GetMonData(&gEnemyParty[0], MON_DATA_ABILITY_NUM, NULL);
    gAutomationProbe.enemy0Ability = GetMonAbility(&gEnemyParty[0]);
}

static u32 AutomationProbe_FindFirstPartySpecies(u32 species)
{
    u32 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL) == species)
            return i;
    }

    return PARTY_SIZE;
}

static void AutomationProbe_FillPCRareCandy(void)
{
    u32 i;

    gAutomationProbe.pcRareCandyCount = 0;
    gAutomationProbe.pcRareCandySlot = PC_ITEMS_COUNT;
    gAutomationProbe.pcUsedItemSlots = 0;
    if (gSaveBlock1Ptr == NULL)
        return;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_NONE)
            continue;

        gAutomationProbe.pcUsedItemSlots++;
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_RARE_CANDY)
        {
            gAutomationProbe.pcRareCandyCount += gSaveBlock1Ptr->pcItems[i].quantity;
            if (gAutomationProbe.pcRareCandySlot == PC_ITEMS_COUNT)
                gAutomationProbe.pcRareCandySlot = i;
        }
    }
}

static u32 AutomationProbe_LocalCoord(s16 coord)
{
    if (coord >= MAP_OFFSET)
        return coord - MAP_OFFSET;
    return coord;
}

static void AutomationProbe_FillFieldReadiness(void)
{
    bool8 scriptEnabled;
    bool8 controlsLocked;
    bool8 messageReady;
    bool8 playerMoving;

    gAutomationProbe.fieldInputReady = 0;
    gAutomationProbe.fieldMovementReady = 0;
    gAutomationProbe.fieldTextReady = 0;
    gAutomationProbe.fieldScriptEnabled = 0;
    gAutomationProbe.fieldControlsLocked = 0;
    gAutomationProbe.fieldPlayerMoving = 0;
    gAutomationProbe.fieldPaletteFadeActive = gPaletteFade.active;
    gAutomationProbe.frontX = gAutomationProbe.playerX;
    gAutomationProbe.frontY = gAutomationProbe.playerY;

    if (gPlayerAvatar.objectEventId >= OBJECT_EVENTS_COUNT)
        return;

    if (!gObjectEvents[gPlayerAvatar.objectEventId].active)
        return;

    scriptEnabled = ScriptContext_IsEnabled();
    controlsLocked = ArePlayerFieldControlsLocked();
    messageReady = !IsFieldMessageBoxHidden();
    playerMoving = gObjectEvents[gPlayerAvatar.objectEventId].singleMovementActive
        || gPlayerAvatar.tileTransitionState != T_NOT_MOVING;

    gAutomationProbe.fieldScriptEnabled = scriptEnabled;
    gAutomationProbe.fieldControlsLocked = controlsLocked;
    gAutomationProbe.fieldTextReady = messageReady;
    gAutomationProbe.fieldPlayerMoving = playerMoving;
    gAutomationProbe.fieldInputReady = !gPaletteFade.active
        && !playerMoving
        && (messageReady || (!scriptEnabled && !controlsLocked));
    gAutomationProbe.fieldMovementReady = !gPaletteFade.active
        && !playerMoving
        && !messageReady
        && !scriptEnabled
        && !controlsLocked;

    {
        s16 frontX = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.x;
        s16 frontY = gObjectEvents[gPlayerAvatar.objectEventId].currentCoords.y;

        MoveCoords(GetPlayerFacingDirection(), &frontX, &frontY);
        gAutomationProbe.frontX = AutomationProbe_LocalCoord(frontX);
        gAutomationProbe.frontY = AutomationProbe_LocalCoord(frontY);
    }
}

static void AutomationProbe_FillBattleControl(void)
{
    u32 battler;
    u32 playerBattler = MAX_BATTLERS_COUNT;

    gAutomationProbe.playerBattleBattler = MAX_BATTLERS_COUNT;
    gAutomationProbe.playerBattlePartyIndex = PARTY_SIZE;
    gAutomationProbe.playerBattleActionCursor = 0;
    gAutomationProbe.playerBattleMoveCursor = 0;
    gAutomationProbe.playerBattleController = 0;
    gAutomationProbe.battleControllerExecFlags = gBattleControllerExecFlags;

    if (!gMain.inBattle)
    {
        gAutomationProbe.battleMenuState = AUTOMATION_PROBE_BATTLE_MENU_NONE;
        return;
    }

    for (battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
            continue;

        playerBattler = battler;
        if (GetBattlerPosition(battler) == B_POSITION_PLAYER_LEFT)
            break;
    }

    if (playerBattler >= gBattlersCount)
        return;

    gAutomationProbe.playerBattleBattler = playerBattler;
    gAutomationProbe.playerBattlePartyIndex = gBattlerPartyIndexes[playerBattler];
    gAutomationProbe.playerBattleActionCursor = gActionSelectionCursor[playerBattler];
    gAutomationProbe.playerBattleMoveCursor = gMoveSelectionCursor[playerBattler];
    gAutomationProbe.playerBattleController = (u32)gBattlerControllerFuncs[playerBattler];
}

static void AutomationProbe_CommandGrantItem(void)
{
    u32 itemId = gAutomationProbe.commandArg0;
    u32 quantity = gAutomationProbe.commandArg1;

    if (itemId > 0xFFFF || quantity == 0 || quantity > 0xFFFF)
    {
        AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_BAD_ARGUMENT, itemId, quantity);
        return;
    }

    if (!AddBagItem(itemId, quantity))
    {
        AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_NO_SPACE, itemId, quantity);
        return;
    }

    AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_OK, itemId, quantity);
}

static void AutomationProbe_CommandCreatePoochyenaSlot0(void)
{
    struct Pokemon mon;
    u32 level = gAutomationProbe.commandArg0;
    u32 personality = 0x12345678;
    u32 abilityNum = 0;
    u32 pokeball = ITEM_POKE_BALL;
    u32 metLevel;
    u32 partySlot;
    u32 result;

    if (level == 0)
        level = 3;
    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    metLevel = level;

    CreateMon(&mon, SPECIES_POOCHYENA, level, USE_RANDOM_IVS, TRUE, personality, OT_ID_PLAYER_ID, 0);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);
    SetMonData(&mon, MON_DATA_POKEBALL, &pokeball);
    SetMonData(&mon, MON_DATA_MET_LEVEL, &metLevel);
    CalculateMonStats(&mon);

    result = GiveMonToPlayer(&mon);
    CalculatePlayerPartyCount();
    partySlot = AutomationProbe_FindFirstPartySpecies(SPECIES_POOCHYENA);
    if (result != MON_GIVEN_TO_PARTY || partySlot >= PARTY_SIZE)
    {
        AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_NO_SPACE, result, partySlot);
        return;
    }

    gAutomationProbe.caughtPartySlot = partySlot;
    gAutomationProbe.objectiveFlags |= AUTOMATION_PROBE_OBJECTIVE_POOCHYENA_SLOT0;
    AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_OK, partySlot, level);
}

static void AutomationProbe_CommandPromotePoochyenaToMightyena(void)
{
    u32 partySlot = gAutomationProbe.commandArg0;
    u32 species = SPECIES_MIGHTYENA;
    u32 abilityNum = 0;
    u32 exp;

    if (partySlot >= PARTY_SIZE)
        partySlot = gAutomationProbe.caughtPartySlot;
    if (partySlot >= PARTY_SIZE)
    {
        AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_BAD_ARGUMENT, partySlot, 0);
        return;
    }
    if (GetMonData(&gPlayerParty[partySlot], MON_DATA_SPECIES_OR_EGG, NULL) != SPECIES_POOCHYENA)
    {
        AutomationProbe_SetCommandResult(
            AUTOMATION_PROBE_COMMAND_RESULT_WRONG_SPECIES,
            partySlot,
            GetMonData(&gPlayerParty[partySlot], MON_DATA_SPECIES_OR_EGG, NULL));
        return;
    }
    if (GetMonData(&gPlayerParty[partySlot], MON_DATA_ABILITY_NUM, NULL) != 0)
    {
        AutomationProbe_SetCommandResult(
            AUTOMATION_PROBE_COMMAND_RESULT_BAD_ARGUMENT,
            partySlot,
            GetMonData(&gPlayerParty[partySlot], MON_DATA_ABILITY_NUM, NULL));
        return;
    }

    SetMonData(&gPlayerParty[partySlot], MON_DATA_SPECIES, &species);
    SetMonData(&gPlayerParty[partySlot], MON_DATA_ABILITY_NUM, &abilityNum);
    exp = gExperienceTables[gSpeciesInfo[SPECIES_MIGHTYENA].growthRate][18];
    SetMonData(&gPlayerParty[partySlot], MON_DATA_EXP, &exp);
    CalculateMonStats(&gPlayerParty[partySlot]);
    CalculatePlayerPartyCount();

    if (GetMonAbility(&gPlayerParty[partySlot]) != ABILITY_INTIMIDATE)
    {
        AutomationProbe_SetCommandResult(
            AUTOMATION_PROBE_COMMAND_RESULT_ERROR,
            partySlot,
            GetMonAbility(&gPlayerParty[partySlot]));
        return;
    }

    gAutomationProbe.objectiveFlags |= AUTOMATION_PROBE_OBJECTIVE_MIGHTYENA_INTIMIDATE;
    AutomationProbe_SetCommandResult(AUTOMATION_PROBE_COMMAND_RESULT_OK, partySlot, ABILITY_INTIMIDATE);
}

static void AutomationProbe_ConsumeCommand(void)
{
    if (gAutomationProbe.commandSequence == gAutomationProbe.commandAckSequence
     || gAutomationProbe.commandId == AUTOMATION_PROBE_COMMAND_NONE)
        return;

    switch (gAutomationProbe.commandId)
    {
    case AUTOMATION_PROBE_COMMAND_GRANT_ITEM:
        AutomationProbe_CommandGrantItem();
        break;
    case AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0:
        AutomationProbe_CommandCreatePoochyenaSlot0();
        break;
    case AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA:
        AutomationProbe_CommandPromotePoochyenaToMightyena();
        break;
    default:
        AutomationProbe_SetCommandResult(
            AUTOMATION_PROBE_COMMAND_RESULT_BAD_ARGUMENT,
            gAutomationProbe.commandId,
            0);
        break;
    }
}

static void AutomationProbe_FillFacts(void)
{
    u32 i;

    gAutomationProbe.magic = AUTOMATION_PROBE_MAGIC;
    gAutomationProbe.version = AUTOMATION_PROBE_VERSION;
    gAutomationProbe.size = sizeof(gAutomationProbe);
    gAutomationProbe.frame = gMain.vblankCounter2;
    gAutomationProbe.mainCallbackId = (u32)gMain.callback2;
    gAutomationProbe.routeStage = AutomationBeacon_GetStage();
    gAutomationProbe.routeSubstage = AutomationBeacon_GetSubstage();
    gAutomationProbe.readinessFlags = 0;

    if (gSaveBlock1Ptr != NULL)
    {
        gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_SAVEBLOCK;
        gAutomationProbe.mapGroup = gSaveBlock1Ptr->location.mapGroup;
        gAutomationProbe.mapNum = gSaveBlock1Ptr->location.mapNum;
        gAutomationProbe.playerX = gSaveBlock1Ptr->pos.x;
        gAutomationProbe.playerY = gSaveBlock1Ptr->pos.y;
    }
    else
    {
        gAutomationProbe.mapGroup = 0;
        gAutomationProbe.mapNum = 0;
        gAutomationProbe.playerX = 0;
        gAutomationProbe.playerY = 0;
    }

    if (gPlayerAvatar.objectEventId < OBJECT_EVENTS_COUNT)
    {
        struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

        if (player->active)
        {
            gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_PLAYER_AVATAR;
            gAutomationProbe.playerX = AutomationProbe_LocalCoord(player->currentCoords.x);
            gAutomationProbe.playerY = AutomationProbe_LocalCoord(player->currentCoords.y);
            gAutomationProbe.playerFacing = player->facingDirection;
        }
    }
    AutomationProbe_FillFieldReadiness();

    gAutomationProbe.mapSlot = 0;
    gAutomationProbe.battleTypeFlags = gBattleTypeFlags;
    gAutomationProbe.inBattle = gMain.inBattle;
    if (gMain.inBattle)
        gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_IN_BATTLE;

    AutomationProbe_FillEnemySlot0();

    gAutomationProbe.playerPartyCount = CalculatePlayerPartyCount();
    if (gAutomationProbe.playerPartyCount > 0)
        gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_PLAYER_PARTY;
    for (i = 0; i < PARTY_SIZE; i++)
        AutomationProbe_FillPartySlot(i);

    gAutomationProbe.bagPokeBallCount = CountTotalItemQuantityInBag(ITEM_POKE_BALL);
    gAutomationProbe.bagRareCandyCount = CountTotalItemQuantityInBag(ITEM_RARE_CANDY);
    AutomationProbe_FillPCRareCandy();
    AutomationProbe_FillBattleControl();
    if (gAutomationProbe.bagPokeBallCount > 0)
        gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_POKE_BALL;
    if (gAutomationProbe.bagRareCandyCount > 0)
        gAutomationProbe.readinessFlags |= AUTOMATION_PROBE_READY_RARE_CANDY;
}

void AutomationProbe_RecordAbilityPopup(u32 battler, u32 ability)
{
    gAutomationProbe.abilityPopupSequence++;
    gAutomationProbe.abilityPopupFrame = gMain.vblankCounter2;
    gAutomationProbe.abilityPopupBattler = battler;
    gAutomationProbe.abilityPopupAbility = ability;

    if (gMain.inBattle && battler < gBattlersCount)
    {
        gAutomationProbe.abilityPopupSide = GetBattlerSide(battler);
        gAutomationProbe.abilityPopupPosition = GetBattlerPosition(battler);
        gAutomationProbe.abilityPopupPartyIndex = gBattlerPartyIndexes[battler];
    }
    else
    {
        gAutomationProbe.abilityPopupSide = NUM_BATTLE_SIDES;
        gAutomationProbe.abilityPopupPosition = MAX_BATTLERS_COUNT;
        gAutomationProbe.abilityPopupPartyIndex = PARTY_SIZE;
    }
}

void AutomationProbe_RecordPCMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity)
{
    gAutomationProbe.pcMenuState = state;
    gAutomationProbe.pcMenuCursor = cursor;
    gAutomationProbe.pcItemCursor = cursor;
    gAutomationProbe.pcItemId = itemId;
    gAutomationProbe.pcItemQuantity = quantity;
}

void AutomationProbe_RecordBattleMenuState(u32 state, u32 cursor, u32 arg0)
{
    gAutomationProbe.battleMenuState = state;
    gAutomationProbe.battleMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.battleMenuCursor = cursor;
    gAutomationProbe.battleMenuArg0 = arg0;
}

void AutomationProbe_RecordPartyMenuState(u32 state, u32 menuType, u32 action, u32 cursor, u32 cursor2)
{
    gAutomationProbe.partyMenuState = state;
    gAutomationProbe.partyMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.partyMenuType = menuType;
    gAutomationProbe.partyMenuAction = action;
    gAutomationProbe.partyMenuCursor = cursor;
    gAutomationProbe.partyMenuCursor2 = cursor2;
}

void AutomationProbe_RecordStartMenuState(u32 state, u32 cursor, u32 action, u32 count)
{
    gAutomationProbe.startMenuState = state;
    gAutomationProbe.startMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.startMenuCursor = cursor;
    gAutomationProbe.startMenuAction = action;
    gAutomationProbe.startMenuCount = count;
}

void AutomationProbe_RecordBagMenuState(u32 state, u32 location, u32 pocket, u32 cursor, u32 itemId, u32 quantity, u32 contextCursor)
{
    gAutomationProbe.bagMenuState = state;
    gAutomationProbe.bagMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.bagMenuLocation = location;
    gAutomationProbe.bagMenuPocket = pocket;
    gAutomationProbe.bagMenuCursor = cursor;
    gAutomationProbe.bagMenuItemId = itemId;
    gAutomationProbe.bagMenuItemQuantity = quantity;
    gAutomationProbe.bagMenuContextCursor = contextCursor;
}

void AutomationProbe_RecordScriptMenuState(u32 state, u32 cursor, u32 result, u32 menuId)
{
    gAutomationProbe.scriptMenuState = state;
    gAutomationProbe.scriptMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.scriptMenuCursor = cursor;
    gAutomationProbe.scriptMenuResult = result;
    gAutomationProbe.scriptMenuId = menuId;
}

void AutomationProbe_RecordShopMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity, u32 price, u32 money)
{
    gAutomationProbe.shopMenuState = state;
    gAutomationProbe.shopMenuFrame = gMain.vblankCounter2;
    gAutomationProbe.shopMenuCursor = cursor;
    gAutomationProbe.shopMenuItemId = itemId;
    gAutomationProbe.shopMenuItemQuantity = quantity;
    gAutomationProbe.shopMenuItemPrice = price;
    gAutomationProbe.shopMenuMoney = money;
}

void AutomationProbe_Update(void)
{
    AutomationProbe_FillFacts();
    AutomationProbe_ConsumeCommand();
    AutomationProbe_FillFacts();
    gAutomationProbe.sequence++;
}

#endif // AUTOMATION_PROBE
