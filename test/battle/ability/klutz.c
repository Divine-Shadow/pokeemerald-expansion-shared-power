#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Klutz suppresses held item effects")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LEFTOVERS].holdEffect == HOLD_EFFECT_LEFTOVERS);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); MaxHP(100); HP(1); Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Buneary restored a little HP using its Leftovers!");
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Klutz makes Poltergeist fail")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_POLTERGEIST) == EFFECT_POLTERGEIST);
        PLAYER(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Item(ITEM_RAZOR_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_POLTERGEIST, WITH_RNG(RNG_ACCURACY, TRUE)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Poltergeist!");
        NONE_OF {
            HP_BAR(player);
        }
    }
}
