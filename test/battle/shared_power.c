#include "global.h"
#include "pokedex.h"
#include "test/battle.h"

//test that an active ability is shared
SINGLE_BATTLE_TEST("Shared Power: Intimidate is fully shared with a switched in pokemon", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_SPLASH); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        if (ability == ABILITY_INTIMIDATE)
        {
            ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("The opposing Arbok's Intimidate cuts Wobbuffet's Attack!");
			
            ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("The opposing Wobbuffet's Intimidate cuts Wobbuffet's Attack!");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Active ability only triggers once with two copies", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        if (ability == ABILITY_INTIMIDATE)
        {
            ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("The opposing Arbok's Intimidate cuts Wobbuffet's Attack!");
			
            ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("The opposing Wobbuffet's Intimidate cuts Wobbuffet's Attack!");
			NONE_OF{
				ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
				ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
				MESSAGE("The opposing Wobbuffet's Intimidate cuts Wobbuffet's Attack!");
			}
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Defiant reacts to pooled Intimidate")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_BRAVIARY) { Ability(ABILITY_DEFIANT); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(7); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(3); }
    } WHEN {
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Competitive reacts to pooled Intimidate")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_MILOTIC) { Ability(ABILITY_COMPETITIVE); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(7); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(3); }
    } WHEN {
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Native ability is restored after final pooled switch-in effect")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_RUN_AWAY); Speed(20); }
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(10); }
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_RUN_AWAY); Speed(30); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(5); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_LT(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->ability, ABILITY_RUN_AWAY);
        EXPECT_EQ(GetBattlerAbility(B_POSITION_PLAYER_LEFT), ABILITY_RUN_AWAY);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Inner Focus prevents pooled Intimidate")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(7); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(3); }
    } WHEN {
        TURN { SWITCH(player, 1); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Clear Body prevents Intimidate in doubles")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_ESPATHRA) { Speed(5); }
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); Speed(5); }
    } WHEN {
        TURN { ; }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
        ABILITY_POPUP(opponentLeft, ABILITY_CLEAR_BODY);
        MESSAGE("The opposing Espathra's Clear Body prevents stat loss!");
        NONE_OF {
            MESSAGE("Mightyena's Intimidate cuts the opposing Espathra's Attack!");
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Run Away lets teammates flee")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_RATTATA) { Ability(ABILITY_RUN_AWAY); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        u32 fleeType;

        EXPECT_EQ(IsRunningFromBattleImpossible(B_POSITION_PLAYER_LEFT), BATTLE_RUN_SUCCESS);
        EXPECT(TryRunFromBattle(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(gBattleOutcome, B_OUTCOME_RAN);
        fleeType = gProtectStructs[B_POSITION_PLAYER_LEFT].fleeType;
        EXPECT_EQ(fleeType, FLEE_ABILITY);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Shield Dust blocks secondary effects when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_ROCK_TOMB, MOVE_EFFECT_SPD_MINUS_1, 100) == TRUE);
        PLAYER(SPECIES_VIVILLON) { Ability(ABILITY_SHIELD_DUST); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_RUN_AWAY); Speed(10); }
        OPPONENT(SPECIES_NOSEPASS) { Moves(MOVE_ROCK_TOMB); Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_ROCK_TOMB); }
    } SCENE {
        MESSAGE("The opposing Nosepass used Rock Tomb!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, opponent);
        HP_BAR(player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Sturdy prevents lethal damage when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_GEODUDE) { Ability(ABILITY_STURDY); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(100); HP(100); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player, hp: 1);
        ABILITY_POPUP(player, ABILITY_STURDY);
        MESSAGE("Wobbuffet endured the hit using Sturdy!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Sturdy popup is not replaced by the defender's native Clear Body")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); }
        PLAYER(SPECIES_WYNAUT) { Speed(30); }
        OPPONENT(SPECIES_SKARMORY) { Ability(ABILITY_CLEAR_BODY); MaxHP(1); HP(1); Speed(20); }
        OPPONENT(SPECIES_GEODUDE) { Ability(ABILITY_STURDY); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Scratch!");
        ABILITY_POPUP(opponentLeft, ABILITY_STURDY);
        MESSAGE("The opposing Skarmory endured the hit using Sturdy!");
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_CLEAR_BODY);
        }
    }
}

SINGLE_BATTLE_TEST("Shared Power: Sturdy prevents OHKO moves when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_FISSURE) == EFFECT_OHKO);
        PLAYER(SPECIES_GEODUDE) { Ability(ABILITY_STURDY); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Fissure!");
        ABILITY_POPUP(player, ABILITY_STURDY);
        MESSAGE("Wobbuffet was protected by Sturdy!");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shared Power: No Guard makes pooled Horn Drill always hit")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_HORN_DRILL) == EFFECT_OHKO);
        ASSUME(GetMoveAccuracy(MOVE_HORN_DRILL) == 30);
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Speed(20); }
        PLAYER(SPECIES_SEAKING) { Moves(MOVE_HORN_DRILL); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HORN_DRILL, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Seaking used Horn Drill!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HORN_DRILL, player);
        HP_BAR(opponent, hp: 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: target-side pooled No Guard makes inaccurate moves always hit")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) < 100);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_INNER_FOCUS); Speed(50); }
        OPPONENT(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft, WITH_RNG(RNG_ACCURACY, FALSE));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner No Guard does not make inaccurate moves always hit")
{
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) < 100);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_INNER_FOCUS); Speed(50); }
        OPPONENT(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft, WITH_RNG(RNG_ACCURACY, FALSE));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Zap Cannon!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        MESSAGE("Wobbuffet's attack missed!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Compound Eyes raises an active teammate's accuracy")
{
    PASSES_RANDOMLY(65, 100, RNG_ACCURACY);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON, MOVE_CELEBRATE); Speed(100); }
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_COMPOUND_EYES); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Compound Eyes does not raise an active teammate's accuracy")
{
    PASSES_RANDOMLY(50, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON, MOVE_CELEBRATE); Speed(100); }
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_COMPOUND_EYES); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Sand Veil raises an active teammate's evasion in sand")
{
    PASSES_RANDOMLY(4, 5, RNG_ACCURACY);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveAccuracy(MOVE_POUND) == 100);
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        PLAYER(SPECIES_CACNEA) { Ability(ABILITY_SAND_VEIL); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_SANDSTORM, MOVE_POUND, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SANDSTORM);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_POUND, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, opponentLeft);
        HP_BAR(playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Sand Veil does not raise an active teammate's evasion in sand")
{
    PASSES_RANDOMLY(5, 5, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_POUND) == 100);
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        PLAYER(SPECIES_CACNEA) { Ability(ABILITY_SAND_VEIL); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_SANDSTORM, MOVE_POUND, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SANDSTORM);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_POUND, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, opponentLeft);
        HP_BAR(playerLeft);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Levitate grants Ground immunity when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_KOFFING) { Ability(ABILITY_LEVITATE); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Battle Armor blocks critical hits when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_ARMALDO) { Ability(ABILITY_BATTLE_ARMOR); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SCRATCH, criticalHit: TRUE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: Quick Feet ignores paralysis Speed drop when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        WITH_CONFIG(GEN_CONFIG_PARALYSIS_SPEED, GEN_7);
        ASSUME(GetMoveNonVolatileStatus(MOVE_THUNDER_WAVE) == MOVE_EFFECT_PARALYSIS);
        PLAYER(SPECIES_POOCHYENA) { Ability(ABILITY_QUICK_FEET); Speed(70); }
        PLAYER(SPECIES_MUDKIP) { Speed(50); }
        OPPONENT(SPECIES_NOSEPASS) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_THUNDER_WAVE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(player, paralysis: TRUE);
        MESSAGE("Mudkip used Tackle!");
        MESSAGE("The opposing Nosepass used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: Swift Swim doubles Speed in rain when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_SEAKING) { Ability(ABILITY_SWIFT_SWIM); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(199); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Surge Surfer doubles Speed on Electric Terrain when pooled")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_RAICHU_ALOLA) { Ability(ABILITY_SURGE_SURFER); Speed(60); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Surge Surfer does not double Speed")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ALOLA) { Ability(ABILITY_SURGE_SURFER); Speed(60); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Prankster increases status move priority")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); Speed(1); }
        PLAYER(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Clefairy used Celebrate!");
        MESSAGE("The opposing Wobbuffet used Celebrate!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Prankster does not increase status move priority")
{
    GIVEN {
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); Speed(1); }
        PLAYER(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Celebrate!");
        MESSAGE("Clefairy used Celebrate!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Gale Wings increases Flying move priority")
{
    GIVEN {
        ASSUME(GetMoveType(MOVE_AERIAL_ACE) == TYPE_FLYING);
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); HP(100); MaxHP(100); Speed(1); }
        PLAYER(SPECIES_TALONFLAME) { Ability(ABILITY_GALE_WINGS); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_AERIAL_ACE, target: opponentLeft);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Clefairy used Aerial Ace!");
        MESSAGE("The opposing Wobbuffet used Celebrate!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Prankster and Triage stack priority")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_COMFEY) { Ability(ABILITY_TRIAGE); HP(50); MaxHP(100); Speed(1); }
        PLAYER(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_RECOVER);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            MOVE(opponentLeft, MOVE_UPPER_HAND, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Comfey used Recover!");
        MESSAGE("The opposing Wobbuffet used Upper Hand!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Overcoat blocks powder moves")
{
    GIVEN {
        ASSUME(IsPowderMove(MOVE_SPORE));
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
        OPPONENT(SPECIES_PINECO) { Ability(ABILITY_OVERCOAT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPORE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_OVERCOAT);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, playerLeft);
        MESSAGE("It doesn't affect the opposing Abra…");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Overcoat does not block powder moves")
{
    GIVEN {
        ASSUME(IsPowderMove(MOVE_SPORE));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
        OPPONENT(SPECIES_PINECO) { Ability(ABILITY_OVERCOAT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPORE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NOT ABILITY_POPUP(opponentLeft, ABILITY_OVERCOAT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Soundproof blocks sound moves")
{
    GIVEN {
        ASSUME(IsSoundMove(MOVE_PSYCHIC_NOISE));
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
        OPPONENT(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC_NOISE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_SOUNDPROOF);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_NOISE, playerLeft);
        MESSAGE("The opposing Abra's Soundproof blocks Psychic Noise!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Soundproof does not block sound moves")
{
    GIVEN {
        ASSUME(IsSoundMove(MOVE_PSYCHIC_NOISE));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); }
        OPPONENT(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC_NOISE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NOT ABILITY_POPUP(opponentLeft, ABILITY_SOUNDPROOF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_NOISE, playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Early Bird shortens sleep duration")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_SLEEP_TURN(2)); Speed(100); }
        PLAYER(SPECIES_HOUNDOUR) { Ability(ABILITY_EARLY_BIRD); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet woke up!");
        STATUS_ICON(playerLeft, none: TRUE);
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Early Bird does not shorten sleep duration")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_SLEEP_TURN(2)); Speed(100); }
        PLAYER(SPECIES_HOUNDOUR) { Ability(ABILITY_EARLY_BIRD); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet is fast asleep.");
        NONE_OF {
            MESSAGE("Wobbuffet woke up!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        }
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Oblivious blocks Attract")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
        PLAYER(SPECIES_NIDOQUEEN) { Gender(MON_FEMALE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_INNER_FOCUS); Gender(MON_MALE); Speed(50); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ATTRACT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_OBLIVIOUS);
        MESSAGE("It doesn't affect the opposing Nidoking…");
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, opponentLeft);
            MESSAGE("The opposing Nidoking fell in love!");
        }
    } THEN {
        EXPECT(!(opponentLeft->volatiles.infatuation));
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Oblivious does not block Attract")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
        PLAYER(SPECIES_NIDOQUEEN) { Gender(MON_FEMALE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_INNER_FOCUS); Gender(MON_MALE); Speed(50); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ATTRACT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, playerLeft);
        MESSAGE("The opposing Nidoking fell in love!");
    } THEN {
        EXPECT(opponentLeft->volatiles.infatuation);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Oblivious blocks Cute Charm")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Gender(MON_MALE); Speed(100); }
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
        OPPONENT(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); Gender(MON_FEMALE); Speed(20); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft, WITH_RNG(RNG_CUTE_CHARM, TRUE));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        HP_BAR(opponentLeft);
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_CUTE_CHARM);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, playerLeft);
            MESSAGE("The opposing Clefairy's Cute Charm infatuated Wobbuffet!");
        }
    } THEN {
        EXPECT(!playerLeft->volatiles.infatuation);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Oblivious does not block Cute Charm")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Gender(MON_MALE); Speed(100); }
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
        OPPONENT(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); Gender(MON_FEMALE); Speed(20); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft, WITH_RNG(RNG_CUTE_CHARM, TRUE));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(opponentLeft, ABILITY_CUTE_CHARM);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, playerLeft);
        MESSAGE("The opposing Clefairy's Cute Charm infatuated Wobbuffet!");
    } THEN {
        EXPECT(playerLeft->volatiles.infatuation);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Oblivious blocks Taunt")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
        ASSUME(B_OBLIVIOUS_TAUNT >= GEN_6);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_INNER_FOCUS); Speed(50); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_SPORE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_OBLIVIOUS);
        MESSAGE("It doesn't affect the opposing Nidoking…");
        NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, playerLeft); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, opponentLeft);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, playerLeft);
    } THEN {
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].tauntTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Oblivious does not block Taunt")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
        ASSUME(B_OBLIVIOUS_TAUNT >= GEN_6);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_NIDOKING) { Ability(ABILITY_INNER_FOCUS); Speed(50); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, playerLeft);
        MESSAGE("The opposing Nidoking fell for the taunt!");
    } THEN {
        EXPECT_GT(gDisableStructs[B_POSITION_OPPONENT_LEFT].tauntTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Pressure increases PP deduction")
{
    bool32 spreadMove;

    PARAMETRIZE { spreadMove = FALSE; }
    PARAMETRIZE { spreadMove = TRUE; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveTarget(MOVE_SWIFT) == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_POUND, 35}, {MOVE_SWIFT, 20}, {MOVE_CELEBRATE, 10}); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_ABSOL) { Ability(ABILITY_PRESSURE); Speed(1); }
    } WHEN {
        TURN {
            if (spreadMove)
                MOVE(playerLeft, MOVE_SWIFT);
            else
                MOVE(playerLeft, MOVE_POUND, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        if (spreadMove)
            EXPECT_EQ(playerLeft->pp[1], 20 - 3);
        else
            EXPECT_EQ(playerLeft->pp[0], 35 - 2);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Pressure does not increase PP deduction for its ally")
{
    bool32 spreadMove;

    PARAMETRIZE { spreadMove = FALSE; }
    PARAMETRIZE { spreadMove = TRUE; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_SWIFT) == MOVE_TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { MovesWithPP({MOVE_POUND, 35}, {MOVE_SWIFT, 20}, {MOVE_CELEBRATE, 10}); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_ABSOL) { Ability(ABILITY_PRESSURE); Speed(1); }
    } WHEN {
        TURN {
            if (spreadMove)
                MOVE(playerLeft, MOVE_SWIFT);
            else
                MOVE(playerLeft, MOVE_POUND, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        if (spreadMove)
            EXPECT_EQ(playerLeft->pp[1], 20 - 2);
        else
            EXPECT_EQ(playerLeft->pp[0], 35 - 1);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Soundproof blocks Howl on an active partner")
{
    s16 damage[2];

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_HOWL) == EFFECT_ATTACK_UP_USER_ALLY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_HOWL);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentLeft, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HOWL, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        ABILITY_POPUP(playerRight, ABILITY_SOUNDPROOF);
        MESSAGE("Wobbuffet's Soundproof blocks Howl!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
            MESSAGE("Wobbuffet's Attack rose!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentLeft, captureDamage: &damage[1]);
    } THEN {
        EXPECT_EQ(damage[0], damage[1]);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: user Soundproof does not block Howl on its partner")
{
    s16 damage[2];

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HOWL) == EFFECT_ATTACK_UP_USER_ALLY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_HOWL);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentLeft, captureDamage: &damage[0]);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HOWL, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
        MESSAGE("Wobbuffet's Attack rose!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerRight);
        HP_BAR(opponentLeft, captureDamage: &damage[1]);
    } THEN {
        EXPECT_GT(damage[1], damage[0]);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Soundproof prevents Perish Song timers")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_PERISH_SONG) == EFFECT_PERISH_SONG);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(20); }
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_PERISH_SONG);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PERISH_SONG, opponentLeft);
        MESSAGE("Wobbuffet's Soundproof blocks Perish Song!");
    } THEN {
        EXPECT(!playerLeft->volatiles.perishSong);
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].perishSongTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Soundproof does not prevent Perish Song timers")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_PERISH_SONG) == EFFECT_PERISH_SONG);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(20); }
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_PERISH_SONG);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PERISH_SONG, opponentLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_SOUNDPROOF);
            MESSAGE("Wobbuffet's Soundproof blocks Perish Song!");
        }
    } THEN {
        EXPECT(playerLeft->volatiles.perishSong);
        EXPECT_GT(gDisableStructs[B_POSITION_PLAYER_LEFT].perishSongTimer, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Gluttony triggers pinch berries at half HP")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(80); HP(80); Item(ITEM_LIECHI_BERRY); Speed(20); }
        PLAYER(SPECIES_BELLSPROUT) { Ability(ABILITY_GLUTTONY); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Using Liechi Berry, the Attack of Wobbuffet rose!");
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Gluttony does not trigger pinch berries at half HP")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(80); HP(80); Item(ITEM_LIECHI_BERRY); Speed(20); }
        PLAYER(SPECIES_BELLSPROUT) { Ability(ABILITY_GLUTTONY); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
            MESSAGE("Using Liechi Berry, the Attack of Wobbuffet rose!");
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->item, ITEM_LIECHI_BERRY);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Ripen doubles stat-boosting berries")
{
    u16 item;
    u16 move;
    u16 stat;

    PARAMETRIZE { item = ITEM_LIECHI_BERRY; move = MOVE_DRAGON_RAGE; stat = STAT_ATK; }
    PARAMETRIZE { item = ITEM_KEE_BERRY; move = MOVE_SCRATCH; stat = STAT_DEF; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
        ASSUME(gItemsInfo[ITEM_KEE_BERRY].holdEffect == HOLD_EFFECT_KEE_BERRY);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(160); HP(80); Item(item); Speed(20); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_SCRATCH, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, move, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        if (item == ITEM_LIECHI_BERRY)
            MESSAGE("Using Liechi Berry, the Attack of Wobbuffet sharply rose!");
        else
            MESSAGE("Using Kee Berry, the Defense of Wobbuffet sharply rose!");
    } THEN {
        EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Ripen does not double stat-boosting berries")
{
    u16 item;
    u16 move;
    u16 stat;

    PARAMETRIZE { item = ITEM_LIECHI_BERRY; move = MOVE_DRAGON_RAGE; stat = STAT_ATK; }
    PARAMETRIZE { item = ITEM_KEE_BERRY; move = MOVE_SCRATCH; stat = STAT_DEF; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
        ASSUME(gItemsInfo[ITEM_KEE_BERRY].holdEffect == HOLD_EFFECT_KEE_BERRY);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(160); HP(80); Item(item); Speed(20); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_SCRATCH, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, move, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        if (item == ITEM_LIECHI_BERRY)
            MESSAGE("Using Liechi Berry, the Attack of Wobbuffet rose!");
        else
            MESSAGE("Using Kee Berry, the Defense of Wobbuffet rose!");
    } THEN {
        EXPECT_EQ(playerLeft->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Ripen doubles HP-restoring berries")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffectParam == 25);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(80); Item(ITEM_SITRUS_BERRY); Speed(20); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        HP_BAR(playerLeft, damage: 40);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored its health using its Sitrus Berry!");
        HP_BAR(playerLeft, damage: -50);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 90);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Ripen does not double HP-restoring berries")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffectParam == 25);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(80); Item(ITEM_SITRUS_BERRY); Speed(20); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        HP_BAR(playerLeft, damage: 40);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored its health using its Sitrus Berry!");
        HP_BAR(playerLeft, damage: -25);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 65);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Cheek Pouch heals after berry consumption")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffectParam == 25);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(120); HP(80); Item(ITEM_SITRUS_BERRY); Speed(20); }
        PLAYER(SPECIES_GREEDENT) { Ability(ABILITY_CHEEK_POUCH); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        HP_BAR(playerLeft, damage: 40);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored its health using its Sitrus Berry!");
        HP_BAR(playerLeft, damage: -30);
        ABILITY_POPUP(playerLeft, ABILITY_CHEEK_POUCH);
        MESSAGE("Wobbuffet's Cheek Pouch restored its HP a little!");
        HP_BAR(playerLeft, damage: -40);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 110);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Cheek Pouch does not heal after berry consumption")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffectParam == 25);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(120); HP(80); Item(ITEM_SITRUS_BERRY); Speed(20); }
        PLAYER(SPECIES_GREEDENT) { Ability(ABILITY_CHEEK_POUCH); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_DRAGON_RAGE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponentLeft);
        HP_BAR(playerLeft, damage: 40);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored its health using its Sitrus Berry!");
        HP_BAR(playerLeft, damage: -30);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_CHEEK_POUCH);
            MESSAGE("Wobbuffet's Cheek Pouch restored its HP a little!");
            HP_BAR(playerLeft, damage: -40);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 70);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Ripen doubles remaining healing berries")
{
    u16 item;
    u16 move;
    u16 maxHp;
    u16 hp;
    s16 expectedHeal;
    u16 expectedHp;

    PARAMETRIZE { item = ITEM_FIGY_BERRY; move = MOVE_DRAGON_RAGE; maxHp = 300; hp = 100; expectedHeal = 200; expectedHp = 260; }
    PARAMETRIZE { item = ITEM_ENIGMA_BERRY; move = MOVE_BITE; maxHp = 100; hp = 2; expectedHeal = 50; expectedHp = 51; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_FIGY_BERRY].holdEffect == HOLD_EFFECT_CONFUSE_SPICY);
        ASSUME(gItemsInfo[ITEM_FIGY_BERRY].holdEffectParam == 3);
        ASSUME(gItemsInfo[ITEM_ENIGMA_BERRY].holdEffect == HOLD_EFFECT_ENIGMA_BERRY);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); MaxHP(maxHp); HP(hp); Item(item); Speed(20); Nature(NATURE_HARDY); Moves(MOVE_ENDURE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_BITE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            if (item == ITEM_ENIGMA_BERRY)
                MOVE(playerLeft, MOVE_ENDURE);
            else
                MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, move, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        if (item == ITEM_ENIGMA_BERRY)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ENDURE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        if (item == ITEM_FIGY_BERRY)
            MESSAGE("Wynaut restored its health using its Figy Berry!");
        else
            MESSAGE("Wynaut restored its health using its Enigma Berry!");
        HP_BAR(playerLeft, damage: -expectedHeal);
    } THEN {
        EXPECT_EQ(playerLeft->hp, expectedHp);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Ripen does not double remaining healing berries")
{
    u16 item;
    u16 move;
    u16 maxHp;
    u16 hp;
    s16 expectedHeal;
    u16 expectedHp;

    PARAMETRIZE { item = ITEM_FIGY_BERRY; move = MOVE_DRAGON_RAGE; maxHp = 300; hp = 100; expectedHeal = 100; expectedHp = 160; }
    PARAMETRIZE { item = ITEM_ENIGMA_BERRY; move = MOVE_BITE; maxHp = 100; hp = 2; expectedHeal = 25; expectedHp = 26; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_FIGY_BERRY].holdEffect == HOLD_EFFECT_CONFUSE_SPICY);
        ASSUME(gItemsInfo[ITEM_FIGY_BERRY].holdEffectParam == 3);
        ASSUME(gItemsInfo[ITEM_ENIGMA_BERRY].holdEffect == HOLD_EFFECT_ENIGMA_BERRY);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WYNAUT) { Ability(ABILITY_SHADOW_TAG); MaxHP(maxHp); HP(hp); Item(item); Speed(20); Nature(NATURE_HARDY); Moves(MOVE_ENDURE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_BITE, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            if (item == ITEM_ENIGMA_BERRY)
                MOVE(playerLeft, MOVE_ENDURE);
            else
                MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, move, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        if (item == ITEM_ENIGMA_BERRY)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ENDURE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        if (item == ITEM_FIGY_BERRY)
            MESSAGE("Wynaut restored its health using its Figy Berry!");
        else
            MESSAGE("Wynaut restored its health using its Enigma Berry!");
        HP_BAR(playerLeft, damage: -expectedHeal);
    } THEN {
        EXPECT_EQ(playerLeft->hp, expectedHp);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Ripen doubles Leppa Berry PP restoration")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_LEPPA_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_LEPPA_BERRY].holdEffectParam == 10);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_LEPPA_BERRY); Speed(20); MovesWithPP({MOVE_CELEBRATE, 1}, {MOVE_SPLASH, 40}); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored PP to its move Celebrate using its Leppa Berry!");
    } THEN {
        EXPECT_EQ(playerLeft->pp[0], 20);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Ripen does not double Leppa Berry PP restoration")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LEPPA_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PP);
        ASSUME(gItemsInfo[ITEM_LEPPA_BERRY].holdEffectParam == 10);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_LEPPA_BERRY); Speed(20); MovesWithPP({MOVE_CELEBRATE, 1}, {MOVE_SPLASH, 40}); }
        PLAYER(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet restored PP to its move Celebrate using its Leppa Berry!");
    } THEN {
        EXPECT_EQ(playerLeft->pp[0], 10);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Mega Launcher boosts Heal Pulse")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_HEAL_PULSE) == EFFECT_HEAL_PULSE);
        ASSUME(IsPulseMove(MOVE_HEAL_PULSE));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(1); Speed(20); }
        PLAYER(SPECIES_CLAWITZER) { Ability(ABILITY_MEGA_LAUNCHER); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_PULSE, playerLeft);
        HP_BAR(playerLeft, damage: -75);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 76);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Mega Launcher does not boost Heal Pulse")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HEAL_PULSE) == EFFECT_HEAL_PULSE);
        ASSUME(IsPulseMove(MOVE_HEAL_PULSE));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(1); Speed(20); }
        PLAYER(SPECIES_CLAWITZER) { Ability(ABILITY_MEGA_LAUNCHER); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_PULSE, playerLeft);
        HP_BAR(playerLeft, damage: -50);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 51);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Guard Dog prevents Red Card from forcing out the attacker")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); Speed(20); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(100); }
        OPPONENT(SPECIES_OKIDOGI) { Ability(ABILITY_GUARD_DOG); Speed(10); }
        OPPONENT(SPECIES_UNOWN) { Speed(5); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet held up its Red Card against the opposing Wobbuffet!");
        NOT MESSAGE("The opposing Unown was dragged out!");
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_WOBBUFFET);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Guard Dog does not prevent Red Card from forcing out the attacker")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); Speed(20); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(100); }
        OPPONENT(SPECIES_OKIDOGI) { Ability(ABILITY_GUARD_DOG); Speed(10); }
        OPPONENT(SPECIES_UNOWN) { Speed(5); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet held up its Red Card against the opposing Wobbuffet!");
        MESSAGE("The opposing Unown was dragged out!");
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_UNOWN);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Soundproof blocks Heal Bell from curing an active partner")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(IsSoundMove(MOVE_HEAL_BELL));
        WITH_CONFIG(GEN_CONFIG_HEAL_BELL_SOUNDPROOF, GEN_6);
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_POISON); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAL_BELL, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, playerLeft);
        MESSAGE("Wobbuffet was hurt by its poisoning!");
    } THEN {
        EXPECT(playerRight->status1 & STATUS1_POISON);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Soundproof does not block Heal Bell from curing an active partner")
{
    GIVEN {
        ASSUME(IsSoundMove(MOVE_HEAL_BELL));
        WITH_CONFIG(GEN_CONFIG_HEAL_BELL_SOUNDPROOF, GEN_6);
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_POISON); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HEAL_BELL, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, playerLeft);
        NOT MESSAGE("Wobbuffet was hurt by its poisoning!");
    } THEN {
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Soundproof blocks Poke Flute from waking an active teammate")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetItemBattleUsage(ITEM_POKE_FLUTE) == EFFECT_ITEM_USE_POKE_FLUTE);
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_SLEEP_TURN(2)); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { USE_ITEM(player, ITEM_POKE_FLUTE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { MESSAGE("The Pokémon hearing the flute awoke!"); }
    } THEN {
        EXPECT(player->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Soundproof does not block Poke Flute")
{
    GIVEN {
        ASSUME(GetItemBattleUsage(ITEM_POKE_FLUTE) == EFFECT_ITEM_USE_POKE_FLUTE);
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Status1(STATUS1_SLEEP_TURN(2)); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { USE_ITEM(player, ITEM_POKE_FLUTE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The Pokémon hearing the flute awoke!");
    } THEN {
        EXPECT(!(player->status1 & STATUS1_SLEEP));
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Unseen Fist bypasses Protect with contact moves")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_PROTECT) == EFFECT_PROTECT);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_URSHIFU_RAPID_STRIKE) { Ability(ABILITY_UNSEEN_FIST); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Unseen Fist does not bypass Protect")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_PROTECT) == EFFECT_PROTECT);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_URSHIFU_RAPID_STRIKE) { Ability(ABILITY_UNSEEN_FIST); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
        }
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Lightning Rod keeps Electric moves on the selected active target")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(B_REDIRECT_ABILITY_IMMUNITY >= GEN_5);
        ASSUME(GetMoveType(MOVE_THUNDERBOLT) == TYPE_ELECTRIC);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(90); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_RAICHU) { Ability(ABILITY_LIGHTNING_ROD); Speed(20); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            SWITCH(opponentLeft, 2);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_LIGHTNING_ROD);
        };
        ABILITY_POPUP(opponentRight, ABILITY_LIGHTNING_ROD);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
        MESSAGE("The opposing Wobbuffet's Sp. Atk rose!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: native Lightning Rod source redirects from the selected target")
{
    GIVEN {
        ASSUME(B_REDIRECT_ABILITY_IMMUNITY >= GEN_5);
        ASSUME(GetMoveType(MOVE_THUNDERBOLT) == TYPE_ELECTRIC);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(90); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_RAICHU) { Ability(ABILITY_LIGHTNING_ROD); Speed(20); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            SWITCH(opponentLeft, 2);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponentRight, ABILITY_LIGHTNING_ROD);
        };
        ABILITY_POPUP(opponentLeft, ABILITY_LIGHTNING_ROD);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        MESSAGE("The opposing Raichu's Sp. Atk rose!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Storm Drain keeps Water moves on the selected active target")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(B_REDIRECT_ABILITY_IMMUNITY >= GEN_5);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(90); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_GASTRODON_EAST) { Ability(ABILITY_STORM_DRAIN); Speed(20); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            SWITCH(opponentLeft, 2);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_STORM_DRAIN);
        };
        ABILITY_POPUP(opponentRight, ABILITY_STORM_DRAIN);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);
        MESSAGE("The opposing Wobbuffet's Sp. Atk rose!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: native Storm Drain source redirects from the selected target")
{
    GIVEN {
        ASSUME(B_REDIRECT_ABILITY_IMMUNITY >= GEN_5);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(90); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(10); }
        OPPONENT(SPECIES_GASTRODON_EAST) { Ability(ABILITY_STORM_DRAIN); Speed(20); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            SWITCH(opponentLeft, 2);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponentRight, ABILITY_STORM_DRAIN);
        };
        ABILITY_POPUP(opponentLeft, ABILITY_STORM_DRAIN);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        MESSAGE("The opposing Gastrodon's Sp. Atk rose!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Infiltrator bypasses Substitute")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_SUBSTITUTE) == EFFECT_SUBSTITUTE);
        ASSUME(!MoveIgnoresSubstitute(MOVE_SCRATCH));
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(300); HP(300); Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_LT(opponent->hp, 225);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Infiltrator does not bypass Substitute")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SUBSTITUTE) == EFFECT_SUBSTITUTE);
        ASSUME(!MoveIgnoresSubstitute(MOVE_SCRATCH));
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(300); HP(300); Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 225);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Infiltrator bypasses Mist stat protection")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_MIST) == EFFECT_MIST);
        ASSUME(GetMoveEffect(MOVE_GROWL) == EFFECT_ATTACK_DOWN);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_MIST); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet's Attack fell!");
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Infiltrator does not bypass Mist stat protection")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_MIST) == EFFECT_MIST);
        ASSUME(GetMoveEffect(MOVE_GROWL) == EFFECT_ATTACK_DOWN);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_MIST); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet is protected by the mist!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Klutz suppresses held item effects")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_LEFTOVERS].holdEffect == HOLD_EFFECT_LEFTOVERS);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(1); Item(ITEM_LEFTOVERS); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet restored a little HP using its Leftovers!");
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Klutz does not suppress held item effects")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LEFTOVERS].holdEffect == HOLD_EFFECT_LEFTOVERS);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); MaxHP(100); HP(1); Item(ITEM_LEFTOVERS); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet restored a little HP using its Leftovers!");
    } THEN {
        EXPECT_GT(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Klutz prevents Fling")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(B_KLUTZ_FLING_INTERACTION >= GEN_5);
        ASSUME(GetMoveEffect(MOVE_FLING) == EFFECT_FLING);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_RAZOR_CLAW); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_FLING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Fling!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_RAZOR_CLAW);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Klutz does not prevent Fling")
{
    GIVEN {
        ASSUME(B_KLUTZ_FLING_INTERACTION >= GEN_5);
        ASSUME(GetMoveEffect(MOVE_FLING) == EFFECT_FLING);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_RAZOR_CLAW); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_FLING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Fling!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Klutz makes Poltergeist fail")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_POLTERGEIST) == EFFECT_POLTERGEIST);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_RAZOR_CLAW); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_POLTERGEIST, WITH_RNG(RNG_ACCURACY, TRUE)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Poltergeist!");
        NONE_OF {
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Klutz does not make Poltergeist fail")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_POLTERGEIST) == EFFECT_POLTERGEIST);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_RAZOR_CLAW); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_POLTERGEIST, WITH_RNG(RNG_ACCURACY, TRUE)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Poltergeist!");
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Guts ignores burn Attack drop when pooled", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_KEEN_EYE; }
    PARAMETRIZE { ability = ABILITY_GUTS; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_MACHOP) { Ability(ability); Speed(70); }
        PLAYER(SPECIES_MUDKIP) { Status1(STATUS1_BURN); Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(3.0), results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Inner Focus prevents Fake Out flinch after switch-in")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Speed(6); }
        PLAYER(SPECIES_WYNAUT) { Speed(5); }
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); Speed(7); }
        OPPONENT(SPECIES_SABLEYE) { Speed(4); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerRight);
        }
    } THEN {
        EXPECT(!playerLeft->volatiles.flinched);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Inner Focus prevents Fake Out flinch")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(5); }
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); Speed(4); }
        OPPONENT(SPECIES_SABLEYE) { Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Speed(3); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_INNER_FOCUS);
        MESSAGE("Wobbuffet's Inner Focus prevents flinching!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
    } THEN {
        EXPECT(!playerLeft->volatiles.flinched);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Inner Focus does not prevent Fake Out flinch")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(5); }
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); Speed(4); }
        OPPONENT(SPECIES_SABLEYE) { Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Speed(3); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_INNER_FOCUS);
            MESSAGE("Wobbuffet's Inner Focus prevents flinching!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        }
        MESSAGE("Wobbuffet flinched and couldn't move!");
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Inner Focus + Contrary vs Intimidate (document current behavior)")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); Speed(10); }
        PLAYER(SPECIES_MALAMAR) { Ability(ABILITY_CONTRARY); Speed(8); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(6); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(3); }
    } WHEN {
        TURN { ; }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Wonder Guard popup is used after an Intimidate switch-in")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMovePower(MOVE_TACKLE) > 0);
        PLAYER(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); }
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INTIMIDATE);
        MESSAGE("Mightyena's Intimidate cuts the opposing Wobbuffet's Attack!");
        MESSAGE("The opposing Wobbuffet used Tackle!");
        ABILITY_POPUP(player, ABILITY_WONDER_GUARD);
    }
}

//test that a passive ability is shared
SINGLE_BATTLE_TEST("Shared Power: Adapatability is fully shared with a switched in pokemon", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
		ASSUME(GetMoveType(MOVE_SCRATCH) == TYPE_NORMAL);
        PLAYER(SPECIES_CRAWDAUNT) { Ability(ability); }
			PLAYER(SPECIES_ZIGZAGOON);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("Zigzagoon used Scratch!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The jump from 1.5x STAB to 2.0x STAB is a 1.33x boost.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}


SINGLE_BATTLE_TEST("Shared Power: Passive ability duplicates don't apply", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
		ASSUME(GetMoveType(MOVE_SCRATCH) == TYPE_NORMAL);
        PLAYER(SPECIES_CRAWDAUNT) { Ability(ability); }
			PLAYER(SPECIES_ZIGZAGOON) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        MESSAGE("Zigzagoon used Scratch!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The jump from 1.5x STAB to 2.0x STAB is a 1.33x boost.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Adaptability strengthens Terastal STAB", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_CRAWDAUNT) { Ability(ability); }
        PLAYER(SPECIES_ZIGZAGOON) { TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_TERA); }
    } SCENE {
        MESSAGE("Zigzagoon used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The jump from 1.5x Tera STAB to 2.0x Adaptability Tera STAB is a 1.33x boost.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: party Adaptability does not strengthen Terastal STAB", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        PLAYER(SPECIES_CRAWDAUNT) { Ability(ability); }
        PLAYER(SPECIES_ZIGZAGOON) { TeraType(TYPE_WATER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_WATER_GUN, gimmick: GIMMICK_TERA); }
    } SCENE {
        MESSAGE("Zigzagoon used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Battery boosts an active partner's special move", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_BATTERY; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_CHARJABUG) { Ability(ability); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); Speed(50); }
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerRight);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        MESSAGE("Zigzagoon used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.3), results[0].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: party Battery does not boost an active partner's special move", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_BATTERY; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_CHARJABUG) { Ability(ability); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); Speed(50); }
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerRight);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        MESSAGE("Zigzagoon used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Symbiosis lets an active partner pass its item")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_ROOM_SERVICE].holdEffect == HOLD_EFFECT_ROOM_SERVICE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ROOM_SERVICE); }
        PLAYER(SPECIES_ORANGURU) { Ability(ABILITY_SYMBIOSIS); }
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_FRIEND_GUARD); Item(ITEM_TOXIC_ORB); }
        OPPONENT(SPECIES_KIRLIA);
        OPPONENT(SPECIES_SHUCKLE);
    } WHEN {
        TURN { SWITCH(playerRight, 2); MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    } SCENE {
        MESSAGE("The opposing Kirlia used Trick Room!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        MESSAGE("Using Room Service, the Speed of Wobbuffet fell!");
        ABILITY_POPUP(playerRight, ABILITY_SYMBIOSIS);
        MESSAGE("Clefairy passed its Toxic Orb to Wobbuffet through Symbiosis!");
        MESSAGE("Wobbuffet was badly poisoned!");
        STATUS_ICON(playerLeft, STATUS1_TOXIC_POISON);
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_TOXIC_ORB);
        EXPECT_EQ(playerRight->item, ITEM_NONE);
        EXPECT(playerLeft->status1 & STATUS1_TOXIC_POISON);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: party Symbiosis does not let an active partner pass its item")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_ROOM_SERVICE].holdEffect == HOLD_EFFECT_ROOM_SERVICE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ROOM_SERVICE); }
        PLAYER(SPECIES_ORANGURU) { Ability(ABILITY_SYMBIOSIS); }
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_FRIEND_GUARD); Item(ITEM_TOXIC_ORB); }
        OPPONENT(SPECIES_KIRLIA);
        OPPONENT(SPECIES_SHUCKLE);
    } WHEN {
        TURN { SWITCH(playerRight, 2); MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    } SCENE {
        MESSAGE("The opposing Kirlia used Trick Room!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        MESSAGE("Using Room Service, the Speed of Wobbuffet fell!");
        NONE_OF {
            ABILITY_POPUP(playerRight, ABILITY_SYMBIOSIS);
            MESSAGE("Clefairy passed its Toxic Orb to Wobbuffet through Symbiosis!");
        }
        MESSAGE("Clefairy was badly poisoned!");
        STATUS_ICON(playerRight, STATUS1_TOXIC_POISON);
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(playerRight->item, ITEM_TOXIC_ORB);
        EXPECT(playerRight->status1 & STATUS1_TOXIC_POISON);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Sticky Hold prevents item theft")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_THIEF) == EFFECT_STEAL_ITEM);
        PLAYER(SPECIES_URSALUNA) { Item(ITEM_NONE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_LIFE_ORB); Speed(50); }
        OPPONENT(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THIEF, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Ursaluna used Thief!");
        ABILITY_POPUP(opponentLeft, ABILITY_STICKY_HOLD);
        MESSAGE("The opposing Wobbuffet's Sticky Hold made Thief ineffective!");
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(opponentLeft->item, ITEM_LIFE_ORB);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Sticky Hold does not prevent item theft")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_THIEF) == EFFECT_STEAL_ITEM);
        PLAYER(SPECIES_URSALUNA) { Item(ITEM_NONE); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_LIFE_ORB); Speed(50); }
        OPPONENT(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THIEF, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Ursaluna used Thief!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THIEF, playerLeft);
        HP_BAR(opponentLeft);
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_STICKY_HOLD);
            MESSAGE("The opposing Wobbuffet's Sticky Hold made Thief ineffective!");
        }
        MESSAGE("Ursaluna stole the opposing Wobbuffet's Life Orb!");
    } THEN {
        EXPECT_EQ(playerLeft->item, ITEM_LIFE_ORB);
        EXPECT_EQ(opponentLeft->item, ITEM_NONE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Gorilla Tactics locks the user into its first selected move")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_GROWL); Speed(100); }
        PLAYER(SPECIES_DARMANITAN) { Ability(ABILITY_GORILLA_TACTICS); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
    } THEN {
        EXPECT_EQ(gBattleStruct->choicedMove[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], MOVE_TACKLE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Gorilla Tactics does not lock the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_GROWL); Speed(100); }
        PLAYER(SPECIES_DARMANITAN) { Ability(ABILITY_GORILLA_TACTICS); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
    } THEN {
        EXPECT_EQ(gBattleStruct->choicedMove[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], MOVE_NONE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Flower Gift boosts a partner's physical move", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_FLOWER_GIFT; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_CELEBRATE, MOVE_SUNNY_DAY); Speed(100); }
        PLAYER(SPECIES_CHERRIM_OVERCAST) { Ability(ability); Speed(50); }
        PLAYER(SPECIES_CHERRIM_SUNSHINE) { Ability(ABILITY_LIMBER); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUNNY_DAY);
            SWITCH(playerRight, 2);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Sunny Day!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, playerLeft);
        MESSAGE("The sunlight turned harsh!");
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Scratch!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        MESSAGE("Cherrim used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: pooled Flower Gift source does not boost a partner's physical move", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_FLOWER_GIFT; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, MOVE_CELEBRATE, MOVE_SUNNY_DAY); Speed(100); }
        PLAYER(SPECIES_CHERRIM_OVERCAST) { Ability(ability); Speed(50); }
        PLAYER(SPECIES_CHERRIM_SUNSHINE) { Ability(ABILITY_LIMBER); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUNNY_DAY);
            SWITCH(playerRight, 2);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Sunny Day!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, playerLeft);
        MESSAGE("The sunlight turned harsh!");
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Scratch!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        MESSAGE("Cherrim used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Victory Star boosts an active ally's accuracy")
{
    PASSES_RANDOMLY(55, 100, RNG_ACCURACY);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON, MOVE_CELEBRATE); Speed(100); }
        PLAYER(SPECIES_VICTINI) { Ability(ABILITY_VICTORY_STAR); Speed(50); }
        PLAYER(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            SWITCH(playerRight, 2);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Zap Cannon!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: pooled Victory Star source does not boost an active ally's accuracy")
{
    PASSES_RANDOMLY(50, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON, MOVE_CELEBRATE); Speed(100); }
        PLAYER(SPECIES_VICTINI) { Ability(ABILITY_VICTORY_STAR); Speed(50); }
        PLAYER(SPECIES_ABRA) { Ability(ABILITY_SYNCHRONIZE); Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            SWITCH(playerRight, 2);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        MESSAGE("Wobbuffet used Zap Cannon!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Bounce reflects a status move for the target")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_TOXIC) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_TOXIC) == MOVE_EFFECT_TOXIC);
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        PLAYER(SPECIES_ABRA) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_ESPEON) { Ability(ABILITY_MAGIC_BOUNCE); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TOXIC, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(opponentLeft, ABILITY_MAGIC_BOUNCE);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, playerLeft);
        MESSAGE("Wynaut's Toxic was bounced back by the opposing Wobbuffet's Magic Bounce!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, opponentLeft);
        STATUS_ICON(playerLeft, badPoison: TRUE);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Bounce does not reflect a status move for the target")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TOXIC) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_TOXIC) == MOVE_EFFECT_TOXIC);
        PLAYER(SPECIES_WYNAUT) { Speed(100); }
        PLAYER(SPECIES_ABRA) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_ESPEON) { Ability(ABILITY_MAGIC_BOUNCE); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TOXIC, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_MAGIC_BOUNCE);
            MESSAGE("Wynaut's Toxic was bounced back by the opposing Wobbuffet's Magic Bounce!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC, playerLeft);
        STATUS_ICON(opponentLeft, badPoison: TRUE);
    }
}


SINGLE_BATTLE_TEST("Shared Power: Abilities are triggered in correct order in Singles"){
	GIVEN{
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_EXCADRILL) { Ability(ABILITY_MOLD_BREAKER); }
        OPPONENT(SPECIES_WOBBUFFET);
	} WHEN{
        TURN { SWITCH(player, 1); }
	} SCENE{
            ABILITY_POPUP(player, ABILITY_INTIMIDATE);
            MESSAGE("Arbok's Intimidate cuts the opposing Wobbuffet's Attack!");
            ABILITY_POPUP(player, ABILITY_INTIMIDATE);
            MESSAGE("Excadrill's Intimidate cuts the opposing Wobbuffet's Attack!");
            ABILITY_POPUP(player, ABILITY_MOLD_BREAKER);
	}
}


DOUBLE_BATTLE_TEST("Shared Power: Switch-in abilities trigger in Speed and Share Order at the battle's start")
{
    u32 spdPlayer1, spdPlayer2, spdOpponent1, spdOpponent2;

    PARAMETRIZE { spdPlayer1 = 5; spdPlayer2 = 4; spdOpponent1 = 3; spdOpponent2 = 2; }
    PARAMETRIZE { spdPlayer1 = 2; spdPlayer2 = 3; spdOpponent1 = 4; spdOpponent2 = 5; }
    PARAMETRIZE { spdPlayer1 = 4; spdPlayer2 = 3; spdOpponent1 = 5; spdOpponent2 = 2; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WO_CHIEN) { Speed(spdPlayer1); Ability(ABILITY_TABLETS_OF_RUIN); }
        PLAYER(SPECIES_GYARADOS) { Speed(spdPlayer2); Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_PORYGON2) { Speed(spdOpponent1); Ability(ABILITY_DOWNLOAD); }
        OPPONENT(SPECIES_PINSIR) { Speed(spdOpponent2); Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        TURN { ; }
    } SCENE {
        if (spdPlayer1 == 5) {
            ABILITY_POPUP(playerLeft, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerRight, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
            ABILITY_POPUP(opponentLeft, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentLeft, ABILITY_MOLD_BREAKER);
            ABILITY_POPUP(opponentRight, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentRight, ABILITY_MOLD_BREAKER);
        } else if (spdOpponent2 == 5) {
            ABILITY_POPUP(opponentRight, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentRight, ABILITY_MOLD_BREAKER);
            ABILITY_POPUP(opponentLeft, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentLeft, ABILITY_MOLD_BREAKER);
            ABILITY_POPUP(playerRight, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerLeft, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
        } else {
            ABILITY_POPUP(opponentLeft, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentLeft, ABILITY_MOLD_BREAKER);
            ABILITY_POPUP(playerLeft, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerRight, ABILITY_TABLETS_OF_RUIN);
            ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
            ABILITY_POPUP(opponentRight, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentRight, ABILITY_MOLD_BREAKER);
        }
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Switch-in abilities get shared in Double Battles")
{

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WO_CHIEN) { Speed(5); Ability(ABILITY_TABLETS_OF_RUIN); }
        PLAYER(SPECIES_GYARADOS) { Speed(4); Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_PORYGON2) { Speed(3); Ability(ABILITY_DOWNLOAD); }
        OPPONENT(SPECIES_PINSIR) { Speed(2); Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        TURN { ; }
    } SCENE {
            ABILITY_POPUP(playerRight, ABILITY_TABLETS_OF_RUIN);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Passive abilities apply before active ones", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTREPID_SWORD; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
	GIVEN{
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_ZACIAN) { Ability(ability); }
        PLAYER(SPECIES_MALAMAR) { Ability(ABILITY_CONTRARY); }
        OPPONENT(SPECIES_WOBBUFFET);
	} WHEN{
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
	} SCENE{
		HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}


SINGLE_BATTLE_TEST("Shared Power: Passive ability duplicates don't apply", s16 damage){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_KEEN_EYE; }
    PARAMETRIZE { ability = ABILITY_DRIZZLE; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
		ASSUME(GetMoveType(MOVE_SCRATCH) == TYPE_NORMAL);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_PELIPPER) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        // Rain boosts Water moves by 1.5x.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Rough Skin triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_GARCHOMP) { Ability(ABILITY_ROUGH_SKIN); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_ROUGH_SKIN);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Aftermath triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_VOLTORB) { Ability(ABILITY_AFTERMATH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH); SEND_OUT(player, 0); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        HP_BAR(player);
        MESSAGE("Wobbuffet fainted!");
        ABILITY_POPUP(player, ABILITY_AFTERMATH);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Innards Out triggers from pooled ability")
{
    u16 hp;
    PARAMETRIZE { hp = 10; }
    PARAMETRIZE { hp = 40; }
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_PYUKUMUKU) { Ability(ABILITY_INNARDS_OUT); }
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(70); SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET);
        ASSUME(!IsBattleMoveStatus(MOVE_PSYCHIC));
        ASSUME(GetMoveCategory(MOVE_PSYCHIC) == DAMAGE_CATEGORY_SPECIAL);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_PSYCHIC); SEND_OUT(player, 0); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Psychic!");
        HP_BAR(player, hp);
        ABILITY_POPUP(player, ABILITY_INNARDS_OUT);
        HP_BAR(opponent, hp);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Stamina triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_MUDSDALE) { Ability(ABILITY_STAMINA); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Moxie triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_MOXIE); }
        PLAYER(SPECIES_SALAMENCE) { Ability(ABILITY_INTIMIDATE); Attack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Grim Neigh triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_SPECTRIER) { Ability(ABILITY_GRIM_NEIGH); }
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        ASSUME(!IsBattleMoveStatus(MOVE_PSYCHIC));
        ASSUME(GetMoveCategory(MOVE_PSYCHIC) == DAMAGE_CATEGORY_SPECIAL);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Chilling Neigh triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_GLASTRIER) { Ability(ABILITY_CHILLING_NEIGH); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: As One triggers from pooled ability")
{
    u16 ability;
    u32 move;
    u16 stat;

    PARAMETRIZE { ability = ABILITY_AS_ONE_ICE_RIDER; move = MOVE_TACKLE; stat = STAT_ATK; }
    PARAMETRIZE { ability = ABILITY_AS_ONE_SHADOW_RIDER; move = MOVE_PSYCHIC; stat = STAT_SPATK; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_CALYREX_ICE) { Ability(ability); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        ASSUME(GetMovePower(move) > 0);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Soul-Heart triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_MAGEARNA) { Ability(ABILITY_SOUL_HEART); }
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        ASSUME(!IsBattleMoveStatus(MOVE_PSYCHIC));
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Poison Touch triggers from pooled ability")
{
    PASSES_RANDOMLY(3, 10, RNG_POISON_TOUCH);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMovePower(MOVE_SCRATCH) > 0);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_POISON_TOUCH); }
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(player, ABILITY_POISON_TOUCH);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        MESSAGE("The opposing Wobbuffet was poisoned by Grimer's Poison Touch!");
        STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Toxic Chain triggers from pooled ability")
{
    PASSES_RANDOMLY(3, 10, RNG_TOXIC_CHAIN);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) != DAMAGE_CATEGORY_STATUS);
        ASSUME(GetMovePower(MOVE_SCRATCH) > 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TOXIC_CHAIN); }
        PLAYER(SPECIES_OKIDOGI) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_CHAIN);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        MESSAGE("The opposing Wobbuffet was badly poisoned!");
        STATUS_ICON(opponent, badPoison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Toxic Chain lets Pecha Berry cure before Knock Off")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveCategory(MOVE_KNOCK_OFF) != DAMAGE_CATEGORY_STATUS);
        ASSUME(GetMoveEffect(MOVE_KNOCK_OFF) == EFFECT_KNOCK_OFF);
        ASSUME(GetMovePower(MOVE_KNOCK_OFF) > 0);
        ASSUME(gItemsInfo[ITEM_PECHA_BERRY].holdEffect == HOLD_EFFECT_CURE_PSN);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TOXIC_CHAIN); }
        PLAYER(SPECIES_OKIDOGI) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_PECHA_BERRY); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_KNOCK_OFF, WITH_RNG(RNG_TOXIC_CHAIN, TRUE)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_CHAIN);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        MESSAGE("The opposing Wobbuffet was badly poisoned!");
        STATUS_ICON(opponent, badPoison: TRUE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        STATUS_ICON(opponent, badPoison: FALSE);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
            MESSAGE("Okidogi knocked off the opposing Wobbuffet's Pecha Berry!");
        }
    } THEN {
        EXPECT(opponent->status1 == 0);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Toxic Chain does not cure berry before Knock Off")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_KNOCK_OFF) != DAMAGE_CATEGORY_STATUS);
        ASSUME(GetMoveEffect(MOVE_KNOCK_OFF) == EFFECT_KNOCK_OFF);
        ASSUME(GetMovePower(MOVE_KNOCK_OFF) > 0);
        ASSUME(gItemsInfo[ITEM_PECHA_BERRY].holdEffect == HOLD_EFFECT_CURE_PSN);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TOXIC_CHAIN); }
        PLAYER(SPECIES_OKIDOGI) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_PECHA_BERRY); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_KNOCK_OFF, WITH_RNG(RNG_TOXIC_CHAIN, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TOXIC_CHAIN);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
            STATUS_ICON(opponent, badPoison: TRUE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponent);
        }
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
        MESSAGE("Okidogi knocked off the opposing Wobbuffet's Pecha Berry!");
    } THEN {
        EXPECT(opponent->status1 == 0);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Poison Puppeteer triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveHasAdditionalEffect(MOVE_POISON_STING, MOVE_EFFECT_POISON) == TRUE);
        PLAYER(SPECIES_PECHARUNT) { Ability(ABILITY_POISON_PUPPETEER); }
        PLAYER(SPECIES_PECHARUNT) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_POISON_STING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_STING, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, opponent);
        STATUS_ICON(opponent, poison: TRUE);
        ABILITY_POPUP(player, ABILITY_POISON_PUPPETEER);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, opponent);
        MESSAGE("The opposing Wobbuffet became confused!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Own Tempo prevents confusion from moves")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OWN_TEMPO); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(opponent, MOVE_CONFUSE_RAY); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_OWN_TEMPO);
        MESSAGE("Wobbuffet's Own Tempo prevents confusion!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, player);
            MESSAGE("Wobbuffet became confused!");
        }
    } THEN {
        EXPECT(!player->volatiles.confusionTurns);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Own Tempo does not prevent confusion from moves")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OWN_TEMPO); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(opponent, MOVE_CONFUSE_RAY); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, player);
        MESSAGE("Wobbuffet became confused!");
    } THEN {
        EXPECT(player->volatiles.confusionTurns > 0);
    }
}

SINGLE_BATTLE_TEST("Shared Power: pooled Own Tempo prevents Berserk Gene confusion")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_BERSERK_GENE].holdEffect == HOLD_EFFECT_BERSERK_GENE);
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OWN_TEMPO); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_BERSERK_GENE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ABILITY_POPUP(player, ABILITY_OWN_TEMPO);
        MESSAGE("Wobbuffet's Own Tempo prevents confusion!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, player);
            MESSAGE("Wobbuffet became confused!");
        }
    } THEN {
        EXPECT(!player->volatiles.confusionTurns);
        EXPECT(!player->volatiles.infiniteConfusion);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: partner Own Tempo does not prevent Berserk Gene confusion")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_BERSERK_GENE].holdEffect == HOLD_EFFECT_BERSERK_GENE);
        PLAYER(SPECIES_SLOWPOKE) { Ability(ABILITY_OWN_TEMPO); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(ITEM_BERSERK_GENE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, player);
        MESSAGE("Wobbuffet became confused!");
    } THEN {
        EXPECT(player->volatiles.confusionTurns > 0);
        EXPECT(player->volatiles.infiniteConfusion);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Gooey triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_GOOMY) { Ability(ABILITY_GOOEY); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Tangling Hair triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_DIGLETT_ALOLA) { Ability(ABILITY_TANGLING_HAIR); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Mirror Armor reflects Tangling Hair at minimum Speed")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        ASSUME(GetMoveEffect(MOVE_SCARY_FACE) == EFFECT_SPEED_DOWN_2);
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_TANGLING_HAIR); Speed(80); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(70); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_CORVIKNIGHT) { Ability(ABILITY_MIRROR_ARMOR); Speed(40); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); SWITCH(opponentRight, 2); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_TANGLING_HAIR);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        ABILITY_POPUP(opponentLeft, ABILITY_MIRROR_ARMOR);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], MIN_STAT_STAGE);
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Mirror Armor does not reflect Tangling Hair at minimum Speed")
{
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        ASSUME(GetMoveEffect(MOVE_SCARY_FACE) == EFFECT_SPEED_DOWN_2);
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_TANGLING_HAIR); Speed(80); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(70); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(50); }
        OPPONENT(SPECIES_CORVIKNIGHT) { Ability(ABILITY_MIRROR_ARMOR); Speed(40); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SCARY_FACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); SWITCH(opponentRight, 2); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponentLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_TANGLING_HAIR);
            ABILITY_POPUP(opponentLeft, ABILITY_MIRROR_ARMOR);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], MIN_STAT_STAGE);
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Cursed Body triggers from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMovePower(MOVE_SCRATCH) > 0);
        PLAYER(SPECIES_DUSKULL) { Ability(ABILITY_CURSED_BODY); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH, WITH_RNG(RNG_CURSED_BODY, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ABILITY_POPUP(player, ABILITY_CURSED_BODY);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Effect Spore triggers from pooled ability")
{
    PASSES_RANDOMLY(3, 10, RNG_EFFECT_SPORE);
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_SHROOMISH) { Ability(ABILITY_EFFECT_SPORE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
        ABILITY_POPUP(player, ABILITY_EFFECT_SPORE);
        ONE_OF {
            STATUS_ICON(opponent, poison: TRUE);
            STATUS_ICON(opponent, paralysis: TRUE);
            STATUS_ICON(opponent, sleep: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Shared Power: Rain Dish heals from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_RAIN_DANCE) == EFFECT_RAIN_DANCE);
        PLAYER(SPECIES_LUDICOLO) { Ability(ABILITY_RAIN_DISH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RAIN_DISH);
        MESSAGE("Wobbuffet's Rain Dish restored its HP a little!");
        HP_BAR(player, damage: -(100 / 16));
    }
}

SINGLE_BATTLE_TEST("Shared Power: Dry Skin damages from pooled ability in sun")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_SUNNY_DAY) == EFFECT_SUNNY_DAY);
        PLAYER(SPECIES_PARASECT) { Ability(ABILITY_DRY_SKIN); }
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SUNNY_DAY); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DRY_SKIN);
        HP_BAR(player, damage: (100 / 8));
    }
}

SINGLE_BATTLE_TEST("Shared Power: End-turn iterator state resets independently per phase")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_RAIN_DANCE) == EFFECT_RAIN_DANCE);
        PLAYER(SPECIES_LUDICOLO) { Ability(ABILITY_RAIN_DISH); Speed(30); }
        PLAYER(SPECIES_NINJASK) { Ability(ABILITY_SPEED_BOOST); HP(50); MaxHP(100); Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 50 + (100 / 16) * 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power: move effectiveness indicator uses pooled Pixilate for non-native Hyper Voice user")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveType(MOVE_HYPER_VOICE) == TYPE_NORMAL);
        ASSUME(IsSoundMove(MOVE_HYPER_VOICE));
        ASSUME(GetSpeciesAbility(SPECIES_WOBBUFFET, 0) != ABILITY_PIXILATE);
        ASSUME(GetSpeciesAbility(SPECIES_WOBBUFFET, 1) != ABILITY_PIXILATE);
        ASSUME(GetSpeciesAbility(SPECIES_WOBBUFFET, 2) != ABILITY_PIXILATE);
        ASSUME(GetSpeciesType(SPECIES_DRAGONITE, 0) == TYPE_DRAGON || GetSpeciesType(SPECIES_DRAGONITE, 1) == TYPE_DRAGON);
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_DRAGONITE), FLAG_SET_SEEN);
        PLAYER(SPECIES_SYLVEON) { Ability(ABILITY_PIXILATE); Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_HYPER_VOICE); Speed(30); }
        OPPONENT(SPECIES_DRAGONITE) { Speed(5); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MOVE_EFFECTIVENESS(player, TEST_MOVE_EFFECTIVENESS_CANNOT_VIEW);
            MOVE_EFFECTIVENESS(player, TEST_MOVE_EFFECTIVENESS_NO_EFFECT);
            MOVE_EFFECTIVENESS(player, TEST_MOVE_EFFECTIVENESS_NOT_VERY_EFFECTIVE);
            MOVE_EFFECTIVENESS(player, TEST_MOVE_EFFECTIVENESS_NORMAL);
        }
        MOVE_EFFECTIVENESS(player, TEST_MOVE_EFFECTIVENESS_SUPER_EFFECTIVE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Shared Power: Magic Guard prevents poison damage from pooled ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); }
        PLAYER(SPECIES_WOBBUFFET) { HP(10); MaxHP(100); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Splash!");
        NONE_OF {
            MESSAGE("Wobbuffet is hurt by poison!");
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 10);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents damaging entry hazards")
{
    u16 move;

    PARAMETRIZE { move = MOVE_SPIKES; }
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_SPIKES) == EFFECT_SPIKES);
        ASSUME(GetMoveEffect(MOVE_STEALTH_ROCK) == EFFECT_STEALTH_ROCK);
        PLAYER(SPECIES_WYNAUT) { Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(160); MaxHP(160); Speed(15); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_CELEBRATE); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, move);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        NONE_OF {
            HP_BAR(playerLeft);
            MESSAGE("Wobbuffet was hurt by the spikes!");
            MESSAGE("Pointed stones dug into Wobbuffet!");
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 160);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent damaging entry hazards")
{
    u16 move;

    PARAMETRIZE { move = MOVE_SPIKES; }
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SPIKES) == EFFECT_SPIKES);
        ASSUME(GetMoveEffect(MOVE_STEALTH_ROCK) == EFFECT_STEALTH_ROCK);
        PLAYER(SPECIES_WYNAUT) { Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(160); MaxHP(160); Speed(15); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_CELEBRATE); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, move);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponentLeft);
        HP_BAR(playerLeft, damage: 160 / 8);
        if (move == MOVE_SPIKES)
            MESSAGE("Wobbuffet was hurt by the spikes!");
        else
            MESSAGE("Pointed stones dug into Wobbuffet!");
    } THEN {
        EXPECT_EQ(playerLeft->hp, 160 - (160 / 8));
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents Powder self-damage")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_POWDER) == EFFECT_POWDER);
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); Moves(MOVE_EMBER, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_VIVILLON) { Moves(MOVE_POWDER, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_POWDER, target: playerLeft);
            MOVE(playerLeft, MOVE_EMBER, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWDER, opponentLeft);
        NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent Powder self-damage")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_POWDER) == EFFECT_POWDER);
        ASSUME(GetMoveType(MOVE_EMBER) == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); Moves(MOVE_EMBER, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_VIVILLON) { Moves(MOVE_POWDER, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_POWDER, target: playerLeft);
            MOVE(playerLeft, MOVE_EMBER, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWDER, opponentLeft);
        NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, playerLeft); }
        HP_BAR(playerLeft, damage: 100 / 4);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100 - (100 / 4));
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents Flame Burst splash damage")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveAdditionalEffectById(MOVE_FLAME_BURST, 0)->moveEffect == MOVE_EFFECT_FLAME_BURST);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); HP(100); MaxHP(100); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_FLAME_BURST, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_FLAME_BURST, target: playerRight);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_BURST, opponentLeft);
        HP_BAR(playerRight);
        NONE_OF { HP_BAR(playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent Flame Burst splash damage")
{
    GIVEN {
        ASSUME(GetMoveAdditionalEffectById(MOVE_FLAME_BURST, 0)->moveEffect == MOVE_EFFECT_FLAME_BURST);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); HP(100); MaxHP(100); Speed(10); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_FLAME_BURST, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_FLAME_BURST, target: playerRight);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_BURST, opponentLeft);
        HP_BAR(playerRight);
        HP_BAR(playerLeft, damage: 100 / 16);
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - (playerLeft->maxHP / 16));
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents held item backlash")
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_ROCKY_HELMET; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_JABOCA_BERRY; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_ROWAP_BERRY; move = MOVE_WATER_GUN; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);
        ASSUME(gItemsInfo[ITEM_JABOCA_BERRY].holdEffect == HOLD_EFFECT_JABOCA_BERRY);
        ASSUME(gItemsInfo[ITEM_ROWAP_BERRY].holdEffect == HOLD_EFFECT_ROWAP_BERRY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); Moves(MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rocky Helmet!");
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Jaboca Berry!");
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rowap Berry!");
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent held item backlash")
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_ROCKY_HELMET; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_JABOCA_BERRY; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_ROWAP_BERRY; move = MOVE_WATER_GUN; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);
        ASSUME(gItemsInfo[ITEM_JABOCA_BERRY].holdEffect == HOLD_EFFECT_JABOCA_BERRY);
        ASSUME(gItemsInfo[ITEM_ROWAP_BERRY].holdEffect == HOLD_EFFECT_ROWAP_BERRY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Speed(20); Moves(MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        if (item == ITEM_ROCKY_HELMET)
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rocky Helmet!");
        else if (item == ITEM_JABOCA_BERRY)
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Jaboca Berry!");
        else
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rowap Berry!");
    } THEN {
        EXPECT_LT(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Ripen doubles Jaboca and Rowap Berry damage")
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_JABOCA_BERRY; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_ROWAP_BERRY; move = MOVE_WATER_GUN; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_JABOCA_BERRY].holdEffect == HOLD_EFFECT_JABOCA_BERRY);
        ASSUME(gItemsInfo[ITEM_ROWAP_BERRY].holdEffect == HOLD_EFFECT_ROWAP_BERRY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(100); Moves(MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(item); Speed(20); }
        OPPONENT(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponentLeft);
        HP_BAR(playerLeft, damage: 100);
        if (item == ITEM_JABOCA_BERRY)
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Jaboca Berry!");
        else
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rowap Berry!");
    } THEN {
        EXPECT_EQ(playerLeft->hp, 300);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Ripen does not double Jaboca and Rowap Berry damage")
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_JABOCA_BERRY; move = MOVE_SCRATCH; }
    PARAMETRIZE { item = ITEM_ROWAP_BERRY; move = MOVE_WATER_GUN; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_JABOCA_BERRY].holdEffect == HOLD_EFFECT_JABOCA_BERRY);
        ASSUME(gItemsInfo[ITEM_ROWAP_BERRY].holdEffect == HOLD_EFFECT_ROWAP_BERRY);
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(100); Moves(MOVE_SCRATCH, MOVE_WATER_GUN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Item(item); Speed(20); }
        OPPONENT(SPECIES_APPLIN) { Ability(ABILITY_RIPEN); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponentLeft);
        HP_BAR(playerLeft, damage: 50);
        if (item == ITEM_JABOCA_BERRY)
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Jaboca Berry!");
        else
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Rowap Berry!");
    } THEN {
        EXPECT_EQ(playerLeft->hp, 350);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents half-HP user-loss moves")
{
    u16 move;

    PARAMETRIZE { move = MOVE_STEEL_BEAM; }
    PARAMETRIZE { move = MOVE_MIND_BLOWN; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_STEEL_BEAM) == EFFECT_MAX_HP_50_RECOIL);
        ASSUME(GetMoveEffect(MOVE_MIND_BLOWN) == EFFECT_MAX_HP_50_RECOIL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(400); MaxHP(400); Speed(20); Moves(MOVE_STEEL_BEAM, MOVE_MIND_BLOWN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        NONE_OF { HP_BAR(playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 400);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent half-HP user-loss moves")
{
    u16 move;

    PARAMETRIZE { move = MOVE_STEEL_BEAM; }
    PARAMETRIZE { move = MOVE_MIND_BLOWN; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STEEL_BEAM) == EFFECT_MAX_HP_50_RECOIL);
        ASSUME(GetMoveEffect(MOVE_MIND_BLOWN) == EFFECT_MAX_HP_50_RECOIL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(400); MaxHP(400); Speed(20); Moves(MOVE_STEEL_BEAM, MOVE_MIND_BLOWN, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, move, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, playerLeft);
        HP_BAR(playerLeft, damage: 200);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 200);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents Spiky Shield backlash")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_SPIKY_SHIELD) == EFFECT_PROTECT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(400); MaxHP(400); Speed(20); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Moves(MOVE_SPIKY_SHIELD, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SPIKY_SHIELD);
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKY_SHIELD, opponentLeft);
        MESSAGE("The opposing Wobbuffet protected itself!");
        NONE_OF {
            HP_BAR(playerLeft);
            MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Spiky Shield!");
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 400);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent Spiky Shield backlash")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SPIKY_SHIELD) == EFFECT_PROTECT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(400); MaxHP(400); Speed(20); Moves(MOVE_SCRATCH, MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Moves(MOVE_SPIKY_SHIELD, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SPIKY_SHIELD);
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKY_SHIELD, opponentLeft);
        MESSAGE("The opposing Wobbuffet protected itself!");
        HP_BAR(playerLeft, damage: 400 / 8);
        MESSAGE("Wobbuffet was hurt by the opposing Wobbuffet's Spiky Shield!");
    } THEN {
        EXPECT_EQ(playerLeft->hp, 400 - (400 / 8));
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Magic Guard prevents holder item damage")
{
    u16 item;

    PARAMETRIZE { item = ITEM_BLACK_SLUDGE; }
    PARAMETRIZE { item = ITEM_STICKY_BARB; }

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(gItemsInfo[ITEM_BLACK_SLUDGE].holdEffect == HOLD_EFFECT_BLACK_SLUDGE);
        ASSUME(gItemsInfo[ITEM_STICKY_BARB].holdEffect == HOLD_EFFECT_STICKY_BARB);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Item(item); Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet was hurt by the Black Sludge!");
            MESSAGE("Wobbuffet was hurt by the Sticky Barb!");
            HP_BAR(playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Magic Guard does not prevent holder item damage")
{
    u16 item;

    PARAMETRIZE { item = ITEM_BLACK_SLUDGE; }
    PARAMETRIZE { item = ITEM_STICKY_BARB; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_BLACK_SLUDGE].holdEffect == HOLD_EFFECT_BLACK_SLUDGE);
        ASSUME(gItemsInfo[ITEM_STICKY_BARB].holdEffect == HOLD_EFFECT_STICKY_BARB);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); HP(100); MaxHP(100); Item(item); Speed(20); }
        PLAYER(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(playerLeft, damage: 100 / 8);
        if (item == ITEM_BLACK_SLUDGE)
            MESSAGE("Wobbuffet was hurt by the Black Sludge!");
        else
            MESSAGE("Wobbuffet was hurt by the Sticky Barb!");
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100 - (100 / 8));
    }
}

SINGLE_BATTLE_TEST("Shared Power: Poison Heal popup uses pooled ability over native ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_SHROOMISH) { Ability(ABILITY_POISON_HEAL); }
        PLAYER(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); Status1(STATUS1_POISON); HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_POISON_HEAL);
        MESSAGE("The poisoning healed Dustox a little bit!");
        HP_BAR(player, damage: -50);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHIELD_DUST);
        }
    }
}

SINGLE_BATTLE_TEST("Shared Power: Liquid Ooze popup uses pooled ability over native ability")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_LEECH_SEED) == EFFECT_LEECH_SEED);
        PLAYER(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); }
        PLAYER(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); HP(100); MaxHP(400); }
        OPPONENT(SPECIES_BULBASAUR) { HP(100); MaxHP(400); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_LEECH_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, opponent);
        HP_BAR(player, damage: 50);
        ABILITY_POPUP(player, ABILITY_LIQUID_OOZE);
        HP_BAR(opponent, damage: 50);
        MESSAGE("The opposing Bulbasaur sucked up the liquid ooze!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHIELD_DUST);
        }
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Liquid Ooze punishes Absorb")
{
    s16 damage;
    s16 drained;

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_ABSORB) == EFFECT_ABSORB);
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(20); }
        OPPONENT(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ABSORB, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &damage);
        ABILITY_POPUP(opponentLeft, ABILITY_LIQUID_OOZE);
        HP_BAR(playerLeft, captureDamage: &drained);
        MESSAGE("Wobbuffet sucked up the liquid ooze!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(0.5), drained);
        EXPECT_GT(drained, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Liquid Ooze does not punish Absorb")
{
    s16 healed;

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ABSORB) == EFFECT_ABSORB);
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Speed(20); }
        OPPONENT(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ABSORB, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, playerLeft);
        HP_BAR(opponentLeft);
        NONE_OF { ABILITY_POPUP(opponentLeft, ABILITY_LIQUID_OOZE); }
        HP_BAR(playerLeft, captureDamage: &healed);
    } THEN {
        EXPECT_LT(healed, 0);
    }
}

DOUBLE_BATTLE_TEST("Shared Power: pooled Liquid Ooze punishes Strength Sap")
{
    s16 lostHp;

    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        ASSUME(GetMoveEffect(MOVE_STRENGTH_SAP) == EFFECT_STRENGTH_SAP);
        PLAYER(SPECIES_WOBBUFFET) { HP(200); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Attack(100); Speed(20); }
        OPPONENT(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_STRENGTH_SAP, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Strength Sap!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        MESSAGE("The opposing Wobbuffet's Attack fell!");
        ABILITY_POPUP(opponentLeft, ABILITY_LIQUID_OOZE);
        HP_BAR(playerLeft, captureDamage: &lostHp);
        MESSAGE("Wobbuffet sucked up the liquid ooze!");
    } THEN {
        EXPECT_EQ(lostHp, 100);
    }
}

DOUBLE_BATTLE_TEST("Shared Power off: partner Liquid Ooze does not punish Strength Sap")
{
    s16 healed;

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_STRENGTH_SAP) == EFFECT_STRENGTH_SAP);
        PLAYER(SPECIES_WOBBUFFET) { HP(200); MaxHP(400); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Attack(100); Speed(20); }
        OPPONENT(SPECIES_TENTACOOL) { Ability(ABILITY_LIQUID_OOZE); Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_STRENGTH_SAP, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Strength Sap!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        MESSAGE("The opposing Wobbuffet's Attack fell!");
        NONE_OF {
            ABILITY_POPUP(opponentLeft, ABILITY_LIQUID_OOZE);
            MESSAGE("Wobbuffet sucked up the liquid ooze!");
        }
        HP_BAR(playerLeft, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -100);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: Bench ability does not grant pooled end-turn effects")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_RAIN_DANCE) == EFFECT_RAIN_DANCE);
        PLAYER(SPECIES_LUDICOLO) { Ability(ABILITY_RAIN_DISH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_RAIN_DANCE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_RAIN_DISH);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

WILD_BATTLE_TEST("Shared Power: wild battles pool switch-in abilities")
{
    GIVEN {
        BATTLE_TYPE(BATTLE_TYPE_SHARED_POWER);
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); Speed(8); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(6); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shared Power off: Bench ability does not grant pooled switch-in effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INTIMIDATE);
            MESSAGE("Wobbuffet's Intimidate cuts the opposing Wobbuffet's Attack!");
        }
    }
}

//TODO: write checks for AI visibility of abilities, cloud nine functionality, and 
