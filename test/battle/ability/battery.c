#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Battery increases Sp. Attack damage of allies by ~30%", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_BATTERY; }
    PARAMETRIZE { ability = ABILITY_LIMBER; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_WATER_GUN) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_CHARJABUG) { Ability(ability); Speed(40); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_ABRA) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Wobbuffet used Water Gun!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerRight);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        MESSAGE("Charjabug used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        MESSAGE("The opposing Wynaut used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        MESSAGE("The opposing Abra used Celebrate!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.3), results[0].damage);
    }
}

TO_DO_BATTLE_TEST("Battery does not increase its own Sp. Attack damage");
