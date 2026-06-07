#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Infiltrator bypasses Substitute")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SUBSTITUTE) == EFFECT_SUBSTITUTE);
        ASSUME(!MoveIgnoresSubstitute(MOVE_SCRATCH));
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(300); HP(300); Speed(60); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_LT(opponent->hp, 225);
    }
}

SINGLE_BATTLE_TEST("Infiltrator bypasses Mist stat protection")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_MIST) == EFFECT_MIST);
        ASSUME(GetMoveEffect(MOVE_GROWL) == EFFECT_ATTACK_DOWN);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INFILTRATOR); Speed(80); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MIST); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet's Attack fell!");
    }
}
