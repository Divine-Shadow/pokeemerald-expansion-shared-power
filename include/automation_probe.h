#ifndef GUARD_AUTOMATION_PROBE_H
#define GUARD_AUTOMATION_PROBE_H

#define AUTOMATION_PROBE_MAGIC 0x41505242
#define AUTOMATION_PROBE_VERSION 7
#define AUTOMATION_PROBE_PARTY_SIZE 6

enum AutomationProbeReadinessFlags
{
    AUTOMATION_PROBE_READY_SAVEBLOCK = 1 << 0,
    AUTOMATION_PROBE_READY_PLAYER_AVATAR = 1 << 1,
    AUTOMATION_PROBE_READY_IN_BATTLE = 1 << 2,
    AUTOMATION_PROBE_READY_PLAYER_PARTY = 1 << 3,
    AUTOMATION_PROBE_READY_POKE_BALL = 1 << 4,
    AUTOMATION_PROBE_READY_RARE_CANDY = 1 << 5,
    AUTOMATION_PROBE_READY_MASTER_BALL = 1 << 6,
};

enum AutomationProbeCommand
{
    AUTOMATION_PROBE_COMMAND_NONE,
    AUTOMATION_PROBE_COMMAND_GRANT_ITEM,
    AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0,
    AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA,
    AUTOMATION_PROBE_COMMAND_SHOW_BARRY_SPRITE_PREVIEW,
};

enum AutomationProbeCommandResult
{
    AUTOMATION_PROBE_COMMAND_RESULT_NONE,
    AUTOMATION_PROBE_COMMAND_RESULT_OK,
    AUTOMATION_PROBE_COMMAND_RESULT_ERROR,
    AUTOMATION_PROBE_COMMAND_RESULT_BAD_ARGUMENT,
    AUTOMATION_PROBE_COMMAND_RESULT_NO_SPACE,
    AUTOMATION_PROBE_COMMAND_RESULT_NO_MON,
    AUTOMATION_PROBE_COMMAND_RESULT_WRONG_SPECIES,
};

enum AutomationProbePCMenuState
{
    AUTOMATION_PROBE_PC_MENU_NONE,
    AUTOMATION_PROBE_PC_MENU_TOP,
    AUTOMATION_PROBE_PC_MENU_ITEM_STORAGE,
    AUTOMATION_PROBE_PC_MENU_ITEM_LIST,
    AUTOMATION_PROBE_PC_MENU_QUANTITY,
    AUTOMATION_PROBE_PC_MENU_WITHDRAW_MESSAGE,
};

enum AutomationProbeBattleMenuState
{
    AUTOMATION_PROBE_BATTLE_MENU_NONE,
    AUTOMATION_PROBE_BATTLE_MENU_ACTION,
    AUTOMATION_PROBE_BATTLE_MENU_MOVE,
};

enum AutomationProbePartyMenuState
{
    AUTOMATION_PROBE_PARTY_MENU_NONE,
    AUTOMATION_PROBE_PARTY_MENU_CHOOSE,
    AUTOMATION_PROBE_PARTY_MENU_ACTION,
};

enum AutomationProbeStartMenuState
{
    AUTOMATION_PROBE_START_MENU_NONE,
    AUTOMATION_PROBE_START_MENU_READY,
};

enum AutomationProbeBagMenuState
{
    AUTOMATION_PROBE_BAG_MENU_NONE,
    AUTOMATION_PROBE_BAG_MENU_ITEM_LIST,
    AUTOMATION_PROBE_BAG_MENU_CONTEXT,
};

enum AutomationProbeScriptMenuState
{
    AUTOMATION_PROBE_SCRIPT_MENU_NONE,
    AUTOMATION_PROBE_SCRIPT_MENU_YESNO,
    AUTOMATION_PROBE_SCRIPT_MENU_MULTICHOICE,
    AUTOMATION_PROBE_SCRIPT_MENU_MULTICHOICE_GRID,
};

enum AutomationProbeShopMenuState
{
    AUTOMATION_PROBE_SHOP_MENU_NONE,
    AUTOMATION_PROBE_SHOP_MENU_TOP,
    AUTOMATION_PROBE_SHOP_MENU_BUY_LIST,
    AUTOMATION_PROBE_SHOP_MENU_QUANTITY,
    AUTOMATION_PROBE_SHOP_MENU_CONFIRM,
    AUTOMATION_PROBE_SHOP_MENU_MESSAGE,
};

struct AutomationProbe
{
    u32 magic;
    u32 version;
    u32 size;
    u32 sequence;
    u32 frame;
    u32 mainCallbackId;
    u32 routeStage;
    u32 routeSubstage;
    u32 readinessFlags;
    u32 mapGroup;
    u32 mapNum;
    u32 mapSlot;
    u32 playerX;
    u32 playerY;
    u32 playerFacing;
    u32 frontX;
    u32 frontY;
    u32 fieldInputReady;
    u32 fieldMovementReady;
    u32 fieldTextReady;
    u32 fieldScriptEnabled;
    u32 fieldControlsLocked;
    u32 fieldPlayerMoving;
    u32 fieldPaletteFadeActive;
    u32 battleTypeFlags;
    u32 inBattle;
    u32 enemyPartyCount;
    u32 enemy0Species;
    u32 enemy0Level;
    u32 enemy0AbilityNum;
    u32 enemy0Ability;
    u32 playerPartyCount;
    u32 partySpecies[AUTOMATION_PROBE_PARTY_SIZE];
    u32 partyLevel[AUTOMATION_PROBE_PARTY_SIZE];
    u32 partyAbilityNum[AUTOMATION_PROBE_PARTY_SIZE];
    u32 partyAbility[AUTOMATION_PROBE_PARTY_SIZE];
    u32 partyHp[AUTOMATION_PROBE_PARTY_SIZE];
    u32 partyMaxHp[AUTOMATION_PROBE_PARTY_SIZE];
    u32 pcRareCandyCount;
    u32 pcRareCandySlot;
    u32 pcMasterBallCount;
    u32 pcMasterBallSlot;
    u32 pcUsedItemSlots;
    u32 pcMenuState;
    u32 pcMenuCursor;
    u32 pcItemCursor;
    u32 pcItemId;
    u32 pcItemQuantity;
    u32 bagPokeBallCount;
    u32 bagRareCandyCount;
    u32 bagMasterBallCount;
    u32 playerBattleBattler;
    u32 playerBattlePartyIndex;
    u32 playerBattleActionCursor;
    u32 playerBattleMoveCursor;
    u32 playerBattleController;
    u32 battleControllerExecFlags;
    u32 battleMenuState;
    u32 battleMenuFrame;
    u32 battleMenuCursor;
    u32 battleMenuArg0;
    u32 partyMenuState;
    u32 partyMenuFrame;
    u32 partyMenuType;
    u32 partyMenuAction;
    u32 partyMenuCursor;
    u32 partyMenuCursor2;
    u32 startMenuState;
    u32 startMenuFrame;
    u32 startMenuCursor;
    u32 startMenuAction;
    u32 startMenuCount;
    u32 bagMenuState;
    u32 bagMenuFrame;
    u32 bagMenuLocation;
    u32 bagMenuPocket;
    u32 bagMenuCursor;
    u32 bagMenuItemId;
    u32 bagMenuItemQuantity;
    u32 bagMenuContextCursor;
    u32 scriptMenuState;
    u32 scriptMenuFrame;
    u32 scriptMenuCursor;
    u32 scriptMenuResult;
    u32 scriptMenuId;
    u32 shopMenuState;
    u32 shopMenuFrame;
    u32 shopMenuCursor;
    u32 shopMenuItemId;
    u32 shopMenuItemQuantity;
    u32 shopMenuItemPrice;
    u32 shopMenuMoney;
    u32 abilityPopupSequence;
    u32 abilityPopupFrame;
    u32 abilityPopupBattler;
    u32 abilityPopupSide;
    u32 abilityPopupPosition;
    u32 abilityPopupPartyIndex;
    u32 abilityPopupAbility;
    u32 lastCommandId;
    u32 lastCommandArg0;
    u32 lastCommandArg1;
    u32 commandSequence;
    u32 commandId;
    u32 commandArg0;
    u32 commandArg1;
    u32 commandAckSequence;
    u32 commandResult;
    u32 commandResultArg0;
    u32 commandResultArg1;
    u32 objectiveFlags;
    u32 caughtPartySlot;
};

extern struct AutomationProbe gAutomationProbe;

#if AUTOMATION_PROBE
void AutomationProbe_Update(void);
void AutomationProbe_RecordAbilityPopup(u32 battler, u32 ability);
void AutomationProbe_RecordPCMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity);
void AutomationProbe_RecordBattleMenuState(u32 state, u32 cursor, u32 arg0);
void AutomationProbe_RecordPartyMenuState(u32 state, u32 menuType, u32 action, u32 cursor, u32 cursor2);
void AutomationProbe_RecordStartMenuState(u32 state, u32 cursor, u32 action, u32 count);
void AutomationProbe_RecordBagMenuState(u32 state, u32 location, u32 pocket, u32 cursor, u32 itemId, u32 quantity, u32 contextCursor);
void AutomationProbe_RecordScriptMenuState(u32 state, u32 cursor, u32 result, u32 menuId);
void AutomationProbe_RecordShopMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity, u32 price, u32 money);
#else
static inline void AutomationProbe_Update(void)
{
}

static inline void AutomationProbe_RecordAbilityPopup(u32 battler, u32 ability)
{
}

static inline void AutomationProbe_RecordPCMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity)
{
}

static inline void AutomationProbe_RecordBattleMenuState(u32 state, u32 cursor, u32 arg0)
{
}

static inline void AutomationProbe_RecordPartyMenuState(u32 state, u32 menuType, u32 action, u32 cursor, u32 cursor2)
{
}

static inline void AutomationProbe_RecordStartMenuState(u32 state, u32 cursor, u32 action, u32 count)
{
}

static inline void AutomationProbe_RecordBagMenuState(u32 state, u32 location, u32 pocket, u32 cursor, u32 itemId, u32 quantity, u32 contextCursor)
{
}

static inline void AutomationProbe_RecordScriptMenuState(u32 state, u32 cursor, u32 result, u32 menuId)
{
}

static inline void AutomationProbe_RecordShopMenuState(u32 state, u32 cursor, u32 itemId, u32 quantity, u32 price, u32 money)
{
}
#endif

#endif // GUARD_AUTOMATION_PROBE_H
