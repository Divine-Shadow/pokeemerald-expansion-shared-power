#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Recharge moves only force one recharge turn")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_GIGA_IMPACT, MOVE_EFFECT_RECHARGE) == TRUE);
        PLAYER(SPECIES_WOBBUFFET) { Level(10); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_GIGA_IMPACT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GIGA_IMPACT, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet must recharge!");
    } THEN {
        EXPECT_EQ((u32)player->volatiles.recharge, 0);
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].rechargeTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Truant users can move after spending the recharge turn")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_GIGA_IMPACT, MOVE_EFFECT_RECHARGE) == TRUE);
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Level(10); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_GIGA_IMPACT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GIGA_IMPACT, player);
        HP_BAR(opponent);
        MESSAGE("Slaking must recharge!");
        NONE_OF {
            MESSAGE("Slaking is loafing around!");
        }
    } THEN {
        EXPECT_EQ((u32)player->volatiles.recharge, 0);
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].rechargeTimer, 0);
        EXPECT_EQ((u32)gDisableStructs[B_POSITION_PLAYER_LEFT].truantCounter, 0);
    }
}
