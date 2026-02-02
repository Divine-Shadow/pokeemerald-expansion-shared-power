#include "global.h"
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

//TODO: write checks for AI visibility of abilities, cloud nine functionality, and 
