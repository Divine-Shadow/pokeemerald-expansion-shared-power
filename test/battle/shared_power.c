#include "global.h"
#include "test/battle.h"

//test that an active ability is shared
SINGLE_BATTLE_TEST("Shared Power: Intimidate is fully shared with a switched in pokemon"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
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
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Active ability only triggers once with two copies"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
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

//test that a passive ability is shared
SINGLE_BATTLE_TEST("Shared Power: Adapatability is fully shared with a switched in pokemon"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
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


SINGLE_BATTLE_TEST("Shared Power: Passive ability duplicates don't apply"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_HYPER_CUTTER; }
    PARAMETRIZE { ability = ABILITY_ADAPTABILITY; }
    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
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


SINGLE_BATTLE_TEST("Shared Power: Abilities are triggered in correct order in Singles"){
	GIVEN{
		FLAG_SET(SHARED_POWER_FLAG);
		PLAYER(SPECIES_ARBOK) { ABILITY_INTIMIDATE; }
		PLAYER(SPECIES_EXCADRILL) { ABILITY_MOLD_BREAKER; }
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
		FLAG_SET(SHARED_POWER_FLAG);
        PLAYER(SPECIES_WO_CHIEN) { Speed(spdPlayer1); Ability(ABILITY_TABLET_OF_RUIN); }
        PLAYER(SPECIES_GYARADOS) { Speed(spdPlayer2); Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_PORYGON2) { Speed(spdOpponent1); Ability(ABILITY_DOWNLOAD); }
        OPPONENT(SPECIES_PINSIR) { Speed(spdOpponent2); Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        TURN { ; }
    } SCENE {
        if (spdPlayer1 == 5) {
            ABILITY_POPUP(playerLeft, ABILITY_TABLET_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerRight, ABILITY_TABLET_OF_RUIN);
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
            ABILITY_POPUP(playerRight, ABILITY_TABLET_OF_RUIN);
            ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerLeft, ABILITY_TABLET_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
        } else {
            ABILITY_POPUP(opponentLeft, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentLeft, ABILITY_MOLD_BREAKER);
            ABILITY_POPUP(playerLeft, ABILITY_TABLET_OF_RUIN);
            ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
            ABILITY_POPUP(playerRight, ABILITY_TABLET_OF_RUIN);
            ABILITY_POPUP(playerRight, ABILITY_INTIMIDATE);
            ABILITY_POPUP(opponentRight, ABILITY_DOWNLOAD);
            ABILITY_POPUP(opponentRight, ABILITY_MOLD_BREAKER);
        }
    }
}

DOUBLE_BATTLE_TEST("Shared Power: Switch-in abilities get shared in Double Battles")
{

    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
        PLAYER(SPECIES_WO_CHIEN) { Speed(spdPlayer1); Ability(ABILITY_TABLET_OF_RUIN); }
        PLAYER(SPECIES_GYARADOS) { Speed(spdPlayer2); Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_PORYGON2) { Speed(spdOpponent1); Ability(ABILITY_DOWNLOAD); }
        OPPONENT(SPECIES_PINSIR) { Speed(spdOpponent2); Ability(ABILITY_MOLD_BREAKER); }
    } WHEN {
        TURN { ; }
    } SCENE {
            ABILITY_POPUP(playerRight, ABILITY_TABLET_OF_RUIN);
    }
}

SINGLE_BATTLE_TEST("Shared Power: Passive abilities apply before active ones"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_INTREPID_SWORD; }
    PARAMETRIZE { ability = ABILITY_SHED_SKIN; }
	GIVEN{
		FLAG_SET(SHARED_POWER_FLAG);
		PLAYER(SPECIES_ZACIAN) { Ability(ability); }
		PLAYER(SPECIES_MALAMAR) { ABILITY_CONTRARY; }
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


SINGLE_BATTLE_TEST("Shared Power: Passive ability duplicates don't apply"){
	u32 ability;
    PARAMETRIZE { ability = ABILITY_KEEN_EYE; }
    PARAMETRIZE { ability = ABILITY_DRIZZLE; }
    GIVEN {
		FLAG_SET(SHARED_POWER_FLAG);
		ASSUME(GetMoveType(MOVE_SCRATCH) == TYPE_NORMAL);
		PLAYER(SPECIES_ZIGZAGOON) { ability = ABILITY_INTIMIDATE; } 
        PLAYER(SPECIES_PELIPPER) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        MESSAGE("Zigzagoon used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        // The jump from 1.5x STAB to 2.0x STAB is a 1.33x boost.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.33), results[1].damage);
    }
}

//TODO: write checks for AI visibility of abilities, cloud nine functionality, and 