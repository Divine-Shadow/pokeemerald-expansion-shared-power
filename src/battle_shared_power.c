#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "battle_util.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "config/shared_power.h"

#define SHARED_POWER_POOL_ABILITY_BITS 9
#define SHARED_POWER_POOL_ABILITY_MASK ((1 << SHARED_POWER_POOL_ABILITY_BITS) - 1)
#define SHARED_POWER_POOL_SOURCE_SHIFT SHARED_POWER_POOL_ABILITY_BITS

STATIC_ASSERT(ABILITIES_COUNT <= (1 << SHARED_POWER_POOL_ABILITY_BITS), SharedPowerAbilityBitsTooSmall);
STATIC_ASSERT(PARTY_SIZE <= (1 << (16 - SHARED_POWER_POOL_ABILITY_BITS)), SharedPowerPartyBitsTooSmall);

static bool32 SharedPower_IsAbilityInPool(u8 trainerIdx, u16 ability)
{
    u32 byteIdx = ability >> 3;
    u32 bitMask = 1u << (ability & 7);

    return (gBattleStruct->sharedPowerPoolBits[trainerIdx][byteIdx] & bitMask) != 0;
}

static void SharedPower_SetAbilityInPool(u8 trainerIdx, u16 ability)
{
    u32 byteIdx = ability >> 3;
    u32 bitMask = 1u << (ability & 7);

    gBattleStruct->sharedPowerPoolBits[trainerIdx][byteIdx] |= bitMask;
}

static bool32 SharedPower_IsSwitchInAbilityDone(u32 battler, u16 ability)
{
    u32 byteIdx = ability >> 3;
    u32 bitMask = 1u << (ability & 7);

    return (gBattleStruct->sharedPowerSwitchInDone[battler][byteIdx] & bitMask) != 0;
}

static void SharedPower_SetSwitchInAbilityDone(u32 battler, u16 ability)
{
    u32 byteIdx = ability >> 3;
    u32 bitMask = 1u << (ability & 7);

    gBattleStruct->sharedPowerSwitchInDone[battler][byteIdx] |= bitMask;
}

static void SharedPower_RestoreOriginalAbility(u32 battler)
{
    if (!gBattleStruct->sharedPowerPopupOverrideActive[battler])
        return;

    if (gBattleStruct->sharedPowerPopupOriginalPartyIndex[battler] == gBattlerPartyIndexes[battler])
        gBattleMons[battler].ability = gBattleStruct->sharedPowerPopupOriginalAbility[battler];
    gBattleStruct->sharedPowerPopupOverrideActive[battler] = FALSE;
    gBattleStruct->sharedPowerPopupOriginalAbility[battler] = ABILITY_NONE;
    gBattleStruct->sharedPowerPopupOriginalPartyIndex[battler] = PARTY_SIZE;
}

static void SharedPower_PushAbilityOverride(u32 battler, u16 ability)
{
    gBattleStruct->sharedPowerPopupOriginalAbility[battler] = gBattleMons[battler].ability;
    gBattleStruct->sharedPowerPopupOriginalPartyIndex[battler] = gBattlerPartyIndexes[battler];
    gBattleStruct->sharedPowerPopupOverrideActive[battler] = TRUE;
    gBattleMons[battler].ability = ability;
}

static void SharedPower_PopAbilityOverride(u32 battler)
{
    SharedPower_RestoreOriginalAbility(battler);
}

static u16 SharedPower_EncodePoolAllEntry(u16 ability, u8 sourcePartyIndex)
{
    return (u16)((sourcePartyIndex << SHARED_POWER_POOL_SOURCE_SHIFT) | (ability & SHARED_POWER_POOL_ABILITY_MASK));
}

static u16 SharedPower_DecodePoolAllAbility(u16 entry)
{
    return entry & SHARED_POWER_POOL_ABILITY_MASK;
}

static bool32 SharedPower_IsBattlerEligibleForPool(u32 battler)
{
    return gBattleMons[battler].hp != 0 && gBattleMons[battler].species != SPECIES_NONE;
}

static bool32 SharedPower_CanBreakThroughAbility(u32 battlerAtk, u32 battlerDef, u32 attackerAbility, u32 targetAbility, bool32 hasAbilityShield, bool32 ignoreMoldBreaker)
{
    if (hasAbilityShield || ignoreMoldBreaker)
        return FALSE;

    return ((IsMoldBreakerTypeAbility(battlerAtk, attackerAbility) || MoveIgnoresTargetAbility(gCurrentMove))
         && battlerDef != battlerAtk
         && gAbilitiesInfo[targetAbility].breakable
         && gBattlerByTurnOrder[gCurrentTurnActionNumber] == battlerAtk
         && gActionsByTurnOrder[gCurrentTurnActionNumber] == B_ACTION_USE_MOVE
         && gCurrentTurnActionNumber < gBattlersCount);
}

static bool32 SharedPower_HasNativeAbilityActive(u32 battler, u16 ability)
{
    return gBattleMons[battler].ability == ability
        && !IsAbilitySuppressedFor(battler, ability, FALSE, FALSE);
}

bool32 SharedPower_IsEnabled(void)
{
#if CONFIG_SHARED_POWER
    return (gBattleTypeFlags & BATTLE_TYPE_SHARED_POWER) != 0;
#else
    return FALSE;
#endif
}

u8 SharedPower_GetTrainerIndex(u8 battler)
{
    if (IsPartnerMonFromSameTrainer(battler))
        return GetBattlerSide(battler);

    return battler;
}

struct SharedPowerPoolAddResult SharedPower_AddToPool(u8 trainerIdx, u16 ability, u8 sourcePartyIndex)
{
    struct SharedPowerPoolAddResult result = {0};
    u16 count;
    u16 allCount;

    if (ability == ABILITY_NONE || ability >= ABILITIES_COUNT)
        return result;

    allCount = gBattleStruct->sharedPowerPoolAllCount[trainerIdx];
    if (allCount < SHARED_POWER_POOL_MAX)
    {
        gBattleStruct->sharedPowerPoolAllOrder[trainerIdx][allCount] = SharedPower_EncodePoolAllEntry(ability, sourcePartyIndex);
        gBattleStruct->sharedPowerPoolAllCount[trainerIdx] = allCount + 1;
        result.addedAll = TRUE;
    }

    if (SharedPower_IsAbilityInPool(trainerIdx, ability))
        return result;

    count = gBattleStruct->sharedPowerPoolCount[trainerIdx];
    if (count >= SHARED_POWER_POOL_MAX)
        return result;

    gBattleStruct->sharedPowerPoolOrder[trainerIdx][count] = ability;
    gBattleStruct->sharedPowerPoolCount[trainerIdx] = count + 1;
    SharedPower_SetAbilityInPool(trainerIdx, ability);
    result.addedUnique = TRUE;
    return result;
}

bool32 SharedPower_TrainerHasAbility(u8 trainerIdx, u16 ability)
{
    if (ability == ABILITY_NONE || ability >= ABILITIES_COUNT)
        return FALSE;

    return SharedPower_IsAbilityInPool(trainerIdx, ability);
}

void SharedPower_ClearBattleState(void)
{
    if (gBattleStruct == NULL)
        return;

    memset(gBattleStruct->sharedPowerPoolCount, 0, sizeof(gBattleStruct->sharedPowerPoolCount));
    memset(gBattleStruct->sharedPowerPoolAllCount, 0, sizeof(gBattleStruct->sharedPowerPoolAllCount));
    memset(gBattleStruct->sharedPowerSwitchInCount, 0, sizeof(gBattleStruct->sharedPowerSwitchInCount));
    memset(gBattleStruct->sharedPowerSwitchInIndex, 0, sizeof(gBattleStruct->sharedPowerSwitchInIndex));
    memset(gBattleStruct->sharedPowerPoolBits, 0, sizeof(gBattleStruct->sharedPowerPoolBits));
    memset(gBattleStruct->sharedPowerSwitchInDone, 0, sizeof(gBattleStruct->sharedPowerSwitchInDone));
    memset(gBattleStruct->sharedPowerPoolOrder, 0, sizeof(gBattleStruct->sharedPowerPoolOrder));
    memset(gBattleStruct->sharedPowerPoolAllOrder, 0, sizeof(gBattleStruct->sharedPowerPoolAllOrder));
    memset(gBattleStruct->sharedPowerSwitchInAbilities, 0, sizeof(gBattleStruct->sharedPowerSwitchInAbilities));
    memset(gBattleStruct->sharedPowerSwitchInQueued, 0, sizeof(gBattleStruct->sharedPowerSwitchInQueued));
    memset(gBattleStruct->sharedPowerPopupAbility, 0, sizeof(gBattleStruct->sharedPowerPopupAbility));
    memset(gBattleStruct->sharedPowerPopupActive, 0, sizeof(gBattleStruct->sharedPowerPopupActive));
    memset(gBattleStruct->sharedPowerPopupOriginalAbility, 0, sizeof(gBattleStruct->sharedPowerPopupOriginalAbility));
    memset(gBattleStruct->sharedPowerPopupOverrideActive, 0, sizeof(gBattleStruct->sharedPowerPopupOverrideActive));
    for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        gBattleStruct->sharedPowerPopupOriginalPartyIndex[battler] = PARTY_SIZE;
}

void SharedPower_ResetSwitchInQueue(u32 battler)
{
    SharedPower_RestoreOriginalAbility(battler);
    gBattleStruct->sharedPowerSwitchInCount[battler] = 0;
    gBattleStruct->sharedPowerSwitchInIndex[battler] = 0;
    gBattleStruct->sharedPowerSwitchInQueued[battler] = FALSE;
    memset(gBattleStruct->sharedPowerSwitchInDone[battler], 0, sizeof(gBattleStruct->sharedPowerSwitchInDone[battler]));
    gBattleStruct->sharedPowerPopupAbility[battler] = ABILITY_NONE;
    gBattleStruct->sharedPowerPopupActive[battler] = FALSE;
    gBattleStruct->sharedPowerPopupOriginalAbility[battler] = ABILITY_NONE;
    gBattleStruct->sharedPowerPopupOriginalPartyIndex[battler] = PARTY_SIZE;
    gBattleStruct->sharedPowerPopupOverrideActive[battler] = FALSE;
}

bool32 IsAbilitySuppressedFor(u32 battler, u16 ability, bool32 ignoreMoldBreaker, bool32 noAbilityShield)
{
    bool32 hasAbilityShield = !noAbilityShield && GetBattlerHoldEffectIgnoreAbility(battler, TRUE) == HOLD_EFFECT_ABILITY_SHIELD;
    bool32 abilityCantBeSuppressed = gAbilitiesInfo[ability].cantBeSuppressed;

    if (abilityCantBeSuppressed)
    {
        if (gBattleMons[battler].volatiles.transformed
            && gBattleMons[battler].volatiles.gastroAcid
            && ability == ABILITY_COMATOSE)
            return TRUE;

        if (SharedPower_CanBreakThroughAbility(gBattlerAttacker, battler, gBattleMons[gBattlerAttacker].ability, ability, hasAbilityShield, ignoreMoldBreaker))
            return TRUE;

        return FALSE;
    }

    if (gBattleMons[battler].volatiles.gastroAcid)
        return TRUE;

    if (!hasAbilityShield
     && IsNeutralizingGasOnField()
     && ability != ABILITY_NEUTRALIZING_GAS)
        return TRUE;

    if (SharedPower_CanBreakThroughAbility(gBattlerAttacker, battler, gBattleMons[gBattlerAttacker].ability, ability, hasAbilityShield, ignoreMoldBreaker))
        return TRUE;

    return FALSE;
}

bool32 SharedPower_IsEligibleFor(u32 battler, u16 ability)
{
    (void)battler;
    (void)ability;
    // Phase 3+: replace with eligibility denylist checks.
    return TRUE;
}

bool32 HasActiveAbility(u32 battler, u16 ability)
{
    if (ability == ABILITY_NONE || !IsBattlerAlive(battler))
        return FALSE;

    if (!SharedPower_IsEnabled())
        return GetBattlerAbility(battler) == ability;

    if (SharedPower_HasNativeAbilityActive(battler, ability))
        return TRUE;

    if (!SharedPower_IsEligibleFor(battler, ability))
        return FALSE;

    if (SharedPower_TrainerHasAbility(SharedPower_GetTrainerIndex(battler), ability)
     && !IsAbilitySuppressedFor(battler, ability, FALSE, FALSE))
        return TRUE;

    return FALSE;
}

bool32 ForEachEffectiveAbilityUnique(u32 battler, bool32 (*cb)(u16 ability))
{
    u16 ability;
    u16 index;
    u8 trainerIdx;

    if (!IsBattlerAlive(battler))
        return FALSE;

    ability = gBattleMons[battler].ability;
    if (ability != ABILITY_NONE
     && SharedPower_HasNativeAbilityActive(battler, ability)
     && cb(ability))
        return TRUE;

    if (!SharedPower_IsEnabled())
        return FALSE;

    trainerIdx = SharedPower_GetTrainerIndex(battler);
    for (index = 0; index < gBattleStruct->sharedPowerPoolCount[trainerIdx]; index++)
    {
        ability = gBattleStruct->sharedPowerPoolOrder[trainerIdx][index];
        if (ability == ABILITY_NONE || ability == gBattleMons[battler].ability)
            continue;
        if (!SharedPower_IsEligibleFor(battler, ability))
            continue;
        if (IsAbilitySuppressedFor(battler, ability, FALSE, FALSE))
            continue;
        if (cb(ability))
            return TRUE;
    }

    return FALSE;
}

bool32 ForEachEffectiveAbilityAllSources(u32 battler, bool32 (*cb)(u16 ability))
{
    u16 ability;
    u16 index;
    u16 poolCount;
    u8 trainerIdx;

    if (!IsBattlerAlive(battler))
        return FALSE;

    if (!SharedPower_IsEnabled())
        return FALSE;

    trainerIdx = SharedPower_GetTrainerIndex(battler);
    poolCount = gBattleStruct->sharedPowerPoolAllCount[trainerIdx];
    for (index = 0; index < poolCount; index++)
    {
        ability = SharedPower_DecodePoolAllAbility(gBattleStruct->sharedPowerPoolAllOrder[trainerIdx][index]);
        if (ability == ABILITY_NONE || ability == gBattleMons[battler].ability)
            continue;
        if (!SharedPower_IsEligibleFor(battler, ability))
            continue;
        if (IsAbilitySuppressedFor(battler, ability, FALSE, FALSE))
            continue;
        if (cb(ability))
            return TRUE;
    }

    return FALSE;
}

bool32 ForEachEffectiveAbility(u32 battler, bool32 (*cb)(u16 ability))
{
    return ForEachEffectiveAbilityUnique(battler, cb);
}

static void SharedPower_BuildSwitchInQueue(u32 battler)
{
    u8 trainerIdx = SharedPower_GetTrainerIndex(battler);
    u16 index;
    u16 count = 0;
    u16 ability = gBattleMons[battler].ability;
    u16 poolCount;
    u8 seenAbilities[SHARED_POWER_POOL_BYTES];

    memset(seenAbilities, 0, sizeof(seenAbilities));
    if (SharedPower_IsBattlerEligibleForPool(battler))
    {
        SharedPower_AddToPool(trainerIdx, gBattleMons[battler].ability, gBattlerPartyIndexes[battler]);
    }

    poolCount = gBattleStruct->sharedPowerPoolAllCount[trainerIdx];
    for (index = 0; index < poolCount; index++)
    {
        u32 byteIdx;
        u32 bitMask;

        ability = SharedPower_DecodePoolAllAbility(gBattleStruct->sharedPowerPoolAllOrder[trainerIdx][index]);
        if (ability == ABILITY_NONE)
            continue;
        byteIdx = ability >> 3;
        bitMask = 1u << (ability & 7);
        if (seenAbilities[byteIdx] & bitMask)
            continue;
        seenAbilities[byteIdx] |= bitMask;
        if (!SharedPower_IsEligibleFor(battler, ability))
            continue;
        if (IsAbilitySuppressedFor(battler, ability, FALSE, FALSE))
            continue;
        gBattleStruct->sharedPowerSwitchInAbilities[battler][count++] = ability;
    }

    gBattleStruct->sharedPowerSwitchInCount[battler] = count;
    gBattleStruct->sharedPowerSwitchInIndex[battler] = 0;
    gBattleStruct->sharedPowerSwitchInQueued[battler] = TRUE;
}

bool32 SharedPower_TrySwitchInAbilities(u32 battler)
{
    u16 ability;
    u16 index;
    u16 count;

    if (!SharedPower_IsEnabled())
        return AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battler, 0, 0, 0);

    SharedPower_RestoreOriginalAbility(battler);
    if (!gBattleStruct->sharedPowerSwitchInQueued[battler])
        SharedPower_BuildSwitchInQueue(battler);

    count = gBattleStruct->sharedPowerSwitchInCount[battler];
    index = gBattleStruct->sharedPowerSwitchInIndex[battler];

    while (index < count)
    {
        bool8 prevSwitchInDone = gSpecialStatuses[battler].switchInAbilityDone;
        u32 effect;

        ability = gBattleStruct->sharedPowerSwitchInAbilities[battler][index];
        gBattleStruct->sharedPowerSwitchInIndex[battler] = ++index;

        if (ability == ABILITY_NONE)
            continue;

        if (SharedPower_IsSwitchInAbilityDone(battler, ability))
            continue;

        SharedPower_PushAbilityOverride(battler, ability);
        gBattleStruct->sharedPowerPopupAbility[battler] = ability;
        gBattleStruct->sharedPowerPopupActive[battler] = TRUE;

        gSpecialStatuses[battler].switchInAbilityDone = FALSE;
        effect = AbilityBattleEffects(ABILITYEFFECT_ON_SWITCHIN, battler, ability, 0, 0);
        if (effect)
        {
            SharedPower_SetSwitchInAbilityDone(battler, ability);
            gSpecialStatuses[battler].switchInAbilityDone = prevSwitchInDone;
            return TRUE;
        }
        SharedPower_PopAbilityOverride(battler);
        gBattleStruct->sharedPowerPopupActive[battler] = FALSE;
        gSpecialStatuses[battler].switchInAbilityDone = prevSwitchInDone;
    }

    return FALSE;
}
