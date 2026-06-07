#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Gorilla Tactics locks the user into its first selected move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_GORILLA_TACTICS); Moves(MOVE_TACKLE, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(gBattleStruct->choicedMove[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], MOVE_TACKLE);
    }
}
