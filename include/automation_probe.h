#ifndef GUARD_AUTOMATION_PROBE_H
#define GUARD_AUTOMATION_PROBE_H

#define AUTOMATION_PROBE_MAGIC 0x41505242
#define AUTOMATION_PROBE_VERSION 1
#define AUTOMATION_PROBE_PARTY_SIZE 6

enum AutomationProbeReadinessFlags
{
    AUTOMATION_PROBE_READY_SAVEBLOCK = 1 << 0,
    AUTOMATION_PROBE_READY_PLAYER_AVATAR = 1 << 1,
    AUTOMATION_PROBE_READY_IN_BATTLE = 1 << 2,
    AUTOMATION_PROBE_READY_PLAYER_PARTY = 1 << 3,
    AUTOMATION_PROBE_READY_POKE_BALL = 1 << 4,
    AUTOMATION_PROBE_READY_RARE_CANDY = 1 << 5,
};

enum AutomationProbeCommand
{
    AUTOMATION_PROBE_COMMAND_NONE,
    AUTOMATION_PROBE_COMMAND_GRANT_ITEM,
    AUTOMATION_PROBE_COMMAND_CREATE_POOCHYENA_SLOT0,
    AUTOMATION_PROBE_COMMAND_PROMOTE_POOCHYENA_TO_MIGHTYENA,
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
    u32 bagPokeBallCount;
    u32 bagRareCandyCount;
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
#else
static inline void AutomationProbe_Update(void)
{
}
#endif

#endif // GUARD_AUTOMATION_PROBE_H
