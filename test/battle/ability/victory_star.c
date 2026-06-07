#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Victory Star raises its ally's accuracy by 1.1x")
{
    PASSES_RANDOMLY(55, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) == 50);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON); Speed(100); }
        PLAYER(SPECIES_VICTINI) { Ability(ABILITY_VICTORY_STAR); Speed(1); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_ABRA) { Speed(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ZAP_CANNON, target: opponentLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, playerLeft);
        HP_BAR(opponentLeft);
    }
}
