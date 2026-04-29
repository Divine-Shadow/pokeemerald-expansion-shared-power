#ifndef GUARD_AUTOMATION_BEACON_H
#define GUARD_AUTOMATION_BEACON_H

#define AUTOMATION_BEACON_PROTOCOL_VERSION 1
#define AUTOMATION_BEACON_VALUE_MAX 14

enum AutomationBeaconStage
{
    AUTOMATION_BEACON_STAGE_INACTIVE,
    AUTOMATION_BEACON_STAGE_MAIN_MENU_READY,
    AUTOMATION_BEACON_STAGE_BIRCH_INTRO_TEXT,
    AUTOMATION_BEACON_STAGE_GENDER_PROMPT_READY,
    AUTOMATION_BEACON_STAGE_GENDER_CONFIRMED,
    AUTOMATION_BEACON_STAGE_NAMING_SCREEN_READY,
    AUTOMATION_BEACON_STAGE_NAME_CONFIRMED,
    AUTOMATION_BEACON_STAGE_POST_NAME_BIRCH_FLOW,
    AUTOMATION_BEACON_STAGE_TRUCK_ENTERED,
    AUTOMATION_BEACON_STAGE_TRUCK_CONTROL_READY,
    AUTOMATION_BEACON_STAGE_LITTLEROOT_ROUTE_SETUP,
    AUTOMATION_BEACON_STAGE_ROUTE101_APPROACH,
    AUTOMATION_BEACON_STAGE_STARTER_CHOOSE_READY,
    AUTOMATION_BEACON_STAGE_STARTER_CONFIRM_PROMPT,
    AUTOMATION_BEACON_STAGE_ERROR,
};

#define AUTOMATION_BEACON_STAGE_BATTLE_SUMMARY_REPRO AUTOMATION_BEACON_STAGE_ERROR

enum AutomationBeaconBattleSummaryReproSubstage
{
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_INACTIVE,
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_ACTION_MENU,
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_PARTY_MENU,
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_PARTY_MON_MENU,
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SUMMARY_REQUESTED,
    AUTOMATION_BEACON_BATTLE_SUMMARY_REPRO_SUMMARY_SCREEN,
};

enum AutomationBeaconGender
{
    AUTOMATION_BEACON_GENDER_UNKNOWN,
    AUTOMATION_BEACON_GENDER_MALE,
    AUTOMATION_BEACON_GENDER_FEMALE,
};

enum AutomationBeaconMapKind
{
    AUTOMATION_BEACON_MAP_UNKNOWN,
    AUTOMATION_BEACON_MAP_TRUCK,
    AUTOMATION_BEACON_MAP_LITTLEROOT,
    AUTOMATION_BEACON_MAP_ROUTE101,
    AUTOMATION_BEACON_MAP_STARTER_SELECTION,
};

enum AutomationBeaconMapSlot
{
    AUTOMATION_BEACON_MAP_SLOT_UNKNOWN,
    AUTOMATION_BEACON_MAP_SLOT_TRUCK,
    AUTOMATION_BEACON_MAP_SLOT_LITTLEROOT_TOWN,
    AUTOMATION_BEACON_MAP_SLOT_BRENDANS_HOUSE_1F,
    AUTOMATION_BEACON_MAP_SLOT_BRENDANS_HOUSE_2F,
    AUTOMATION_BEACON_MAP_SLOT_MAYS_HOUSE_1F,
    AUTOMATION_BEACON_MAP_SLOT_MAYS_HOUSE_2F,
    AUTOMATION_BEACON_MAP_SLOT_ROUTE101,
};

#define AUTOMATION_BEACON_STARTER_NA 14

enum AutomationBeaconScriptStep
{
    AUTOMATION_BEACON_SCRIPT_STEP_NONE,
    AUTOMATION_BEACON_SCRIPT_STEP_APPLY_PLAYER,
    AUTOMATION_BEACON_SCRIPT_STEP_APPLY_OBJECT,
    AUTOMATION_BEACON_SCRIPT_STEP_WAIT_MOVEMENT,
    AUTOMATION_BEACON_SCRIPT_STEP_MESSAGE,
    AUTOMATION_BEACON_SCRIPT_STEP_WAIT_MESSAGE,
    AUTOMATION_BEACON_SCRIPT_STEP_CLOSE_MESSAGE,
    AUTOMATION_BEACON_SCRIPT_STEP_OPEN_DOOR,
    AUTOMATION_BEACON_SCRIPT_STEP_CLOSE_DOOR,
    AUTOMATION_BEACON_SCRIPT_STEP_WAIT_DOOR,
    AUTOMATION_BEACON_SCRIPT_STEP_WAIT_BUTTON,
};

#if AUTOMATION_BEACON
void AutomationBeacon_SetStage(u8 stageId, u8 substageId, u8 flags);
void AutomationBeacon_SetProof(u8 gender, u8 nameLen, u8 nameChar0, u8 mapKind, u8 starterSelection, bool8 inputReady);
void AutomationBeacon_SetErrorCode(u8 errorCode);
void AutomationBeacon_SetReadiness(bool8 movementReady, bool8 textReady, bool8 menuReady, bool8 interactReady);
void AutomationBeacon_SetInteractionProof(u8 scriptWaitKind, u8 interactableAhead, u8 routeErrorCode);
void AutomationBeacon_SetNavProof(
    u8 playerX,
    u8 playerY,
    u8 playerFacing,
    u8 frontX,
    u8 frontY,
    u8 mapSlot,
    u8 playerXHi,
    u8 playerYHi,
    u8 frontXHi,
    u8 frontYHi);
void AutomationBeacon_SetScriptStep(u8 scriptStep);
u8 AutomationBeacon_GetScriptStep(void);
void AutomationBeacon_Render(void);
void AutomationBeacon_RenderLate(void);
#else
static inline void AutomationBeacon_SetStage(u8 stageId, u8 substageId, u8 flags)
{
    (void)stageId;
    (void)substageId;
    (void)flags;
}

static inline void AutomationBeacon_SetProof(u8 gender, u8 nameLen, u8 nameChar0, u8 mapKind, u8 starterSelection, bool8 inputReady)
{
    (void)gender;
    (void)nameLen;
    (void)nameChar0;
    (void)mapKind;
    (void)starterSelection;
    (void)inputReady;
}

static inline void AutomationBeacon_SetErrorCode(u8 errorCode)
{
    (void)errorCode;
}

static inline void AutomationBeacon_SetReadiness(bool8 movementReady, bool8 textReady, bool8 menuReady, bool8 interactReady)
{
    (void)movementReady;
    (void)textReady;
    (void)menuReady;
    (void)interactReady;
}

static inline void AutomationBeacon_SetInteractionProof(u8 scriptWaitKind, u8 interactableAhead, u8 routeErrorCode)
{
    (void)scriptWaitKind;
    (void)interactableAhead;
    (void)routeErrorCode;
}

static inline void AutomationBeacon_SetNavProof(
    u8 playerX,
    u8 playerY,
    u8 playerFacing,
    u8 frontX,
    u8 frontY,
    u8 mapSlot,
    u8 playerXHi,
    u8 playerYHi,
    u8 frontXHi,
    u8 frontYHi)
{
    (void)playerX;
    (void)playerY;
    (void)playerFacing;
    (void)frontX;
    (void)frontY;
    (void)mapSlot;
    (void)playerXHi;
    (void)playerYHi;
    (void)frontXHi;
    (void)frontYHi;
}

static inline void AutomationBeacon_SetScriptStep(u8 scriptStep)
{
    (void)scriptStep;
}

static inline u8 AutomationBeacon_GetScriptStep(void)
{
    return AUTOMATION_BEACON_SCRIPT_STEP_NONE;
}

static inline void AutomationBeacon_Render(void)
{
}

static inline void AutomationBeacon_RenderLate(void)
{
}
#endif

#endif // GUARD_AUTOMATION_BEACON_H
