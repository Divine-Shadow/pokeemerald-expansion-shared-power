#include "global.h"
#include "battle.h"
#include "battle_shared_power.h"
#include "battle_util.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "config/shared_power.h"

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
    return GetBattlerSide(battler);
}

bool32 SharedPower_AddToPool(u8 trainerIdx, u16 ability)
{
    (void)trainerIdx;
    (void)ability;
    // Phase 2: replace with per-trainer pool insertion.
    return FALSE;
}

bool32 SharedPower_TrainerHasAbility(u8 trainerIdx, u16 ability)
{
    (void)trainerIdx;
    (void)ability;
    // Phase 2: replace with per-trainer pool membership test.
    return FALSE;
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
    // Phase 2: replace with eligibility denylist checks.
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

bool32 ForEachEffectiveAbility(u32 battler, bool32 (*cb)(u16 ability))
{
    u16 ability;

    if (!IsBattlerAlive(battler))
        return FALSE;

    ability = gBattleMons[battler].ability;
    if (ability != ABILITY_NONE
     && SharedPower_HasNativeAbilityActive(battler, ability)
     && cb(ability))
        return TRUE;

    if (!SharedPower_IsEnabled())
        return FALSE;

    return FALSE;
}
