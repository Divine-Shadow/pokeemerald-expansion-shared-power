#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sleep prevents the battler from using a move")
{
    u32 turns, j;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_SLEEP_TURN(turns)); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        for (j = 0; j < turns; j++)
            TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        for (j = 0; j < turns - 1; j++)
            MESSAGE("Wobbuffet is fast asleep.");
        MESSAGE("Wobbuffet woke up!");
        STATUS_ICON(player, none: TRUE);
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Sleep: Spore doesn't affect grass types (Gen 6+)")
{
    GIVEN {
        ASSUME(IsPowderMove(MOVE_SPORE));
        ASSUME(B_POWDER_GRASS >= GEN_6);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CHIKORITA);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
    }
}

SINGLE_BATTLE_TEST("Sleep: Spore does not print damage effectiveness messages")
{
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SPORE) == DAMAGE_CATEGORY_STATUS);
        ASSUME(GetMoveType(MOVE_SPORE) == TYPE_GRASS);
        ASSUME(GetSpeciesType(SPECIES_WAILMER, 0) == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WAILMER);
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        MESSAGE("The opposing Wailmer fell asleep!");
        NOT MESSAGE("It's super effective!");
    }
}

AI_SINGLE_BATTLE_TEST("AI avoids hypnosis when it can not put target to sleep")
{
    u32 species, ability;

    PARAMETRIZE { species = SPECIES_HOOTHOOT; ability = ABILITY_INSOMNIA; }
    PARAMETRIZE { species = SPECIES_MANKEY; ability = ABILITY_VITAL_SPIRIT; }
    PARAMETRIZE { species = SPECIES_KOMALA; ability = ABILITY_COMATOSE; }
    PARAMETRIZE { species = SPECIES_NACLI; ability = ABILITY_PURIFYING_SALT; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_HYPNOSIS); }
    } WHEN {
        TURN { SCORE_EQ(opponent, MOVE_CELEBRATE, MOVE_HYPNOSIS); } // Both get -10
    }
}
