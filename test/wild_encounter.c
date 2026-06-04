#include "global.h"
#include "event_data.h"
#include "item.h"
#include "pokedex.h"
#include "pokemon.h"
#include "wild_encounter.h"
#include "test/test.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/pokedex.h"
#include "constants/vars.h"

static void ResetHighlanderCharmTestState(void)
{
    ClearBag();
    CpuFastFill(0, gSaveBlock1Ptr->pcItems, sizeof(gSaveBlock1Ptr->pcItems));
    ResetPokedex();
    VarSet(VAR_REPEL_STEP_COUNT, 0);
}

static void MarkSpeciesCaught(u16 species)
{
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_CAUGHT);
}

TEST("Highlander Charm is active in the bag but inactive when absent or stored")
{
    ResetHighlanderCharmTestState();
    EXPECT(!IsHighlanderCharmActive());

    EXPECT(AddPCItem(ITEM_HIGHLANDER_CHARM, 1));
    EXPECT(!IsHighlanderCharmActive());

    EXPECT(AddBagItem(ITEM_HIGHLANDER_CHARM, 1));
    EXPECT(IsHighlanderCharmActive());
}

TEST("Highlander Charm filters evolutionary families by caught Pokedex state")
{
    ResetHighlanderCharmTestState();
    MarkSpeciesCaught(SPECIES_KIRLIA);

    EXPECT(!Test_IsWildSpeciesAllowedByHighlanderCharm(SPECIES_RALTS));
    EXPECT(!Test_IsWildSpeciesAllowedByHighlanderCharm(SPECIES_GARDEVOIR));
    EXPECT(Test_IsWildSpeciesAllowedByHighlanderCharm(SPECIES_ZIGZAGOON));
}

TEST("Highlander Charm preserves proportional land encounter weights after filtering")
{
    static const struct WildPokemon wildMons[LAND_WILD_COUNT] =
    {
        { 2, 2, SPECIES_RATTATA },
        { 2, 2, SPECIES_RATTATA },
        { 2, 2, SPECIES_RATTATA },
        { 2, 2, SPECIES_POOCHYENA },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
        { 2, 2, SPECIES_LOTAD },
    };
    u8 wildMonIndex = 0;
    u32 totalWeight = 0;

    ResetHighlanderCharmTestState();
    MarkSpeciesCaught(SPECIES_RATTATA);

    EXPECT(Test_TryChooseHighlanderCharmWildMonIndex(wildMons, WILD_AREA_LAND, 0, 0, &wildMonIndex, &totalWeight));
    EXPECT_EQ(totalWeight, 50);
    EXPECT_EQ(wildMonIndex, 3);

    EXPECT(Test_TryChooseHighlanderCharmWildMonIndex(wildMons, WILD_AREA_LAND, 0, 10, &wildMonIndex, &totalWeight));
    EXPECT_EQ(totalWeight, 50);
    EXPECT_EQ(wildMonIndex, 4);
}

TEST("Highlander Charm reports empty filtered encounter pools")
{
    static const struct WildPokemon wildMons[WATER_WILD_COUNT] =
    {
        { 5, 5, SPECIES_RATTATA },
        { 5, 5, SPECIES_RATTATA },
        { 5, 5, SPECIES_RATTATA },
        { 5, 5, SPECIES_RATTATA },
        { 5, 5, SPECIES_RATTATA },
    };
    u8 wildMonIndex = 0;
    u32 totalWeight = 100;

    ResetHighlanderCharmTestState();
    MarkSpeciesCaught(SPECIES_RATTATA);

    EXPECT(!Test_TryChooseHighlanderCharmWildMonIndex(wildMons, WILD_AREA_WATER, 0, 0, &wildMonIndex, &totalWeight));
    EXPECT_EQ(totalWeight, 0);
    EXPECT(WasHighlanderCharmEncounterEmpty());
}

TEST("Highlander Charm filters Old Rod encounter slots")
{
    static const struct WildPokemon wildMons[FISH_WILD_COUNT] =
    {
        { 5, 5, SPECIES_RATTATA },
        { 5, 5, SPECIES_LOTAD },
    };
    u8 wildMonIndex = 0;
    u32 totalWeight = 0;

    ResetHighlanderCharmTestState();
    MarkSpeciesCaught(SPECIES_RATTATA);

    EXPECT(Test_TryChooseHighlanderCharmWildMonIndex(wildMons, WILD_AREA_FISHING, OLD_ROD, 0, &wildMonIndex, &totalWeight));
    EXPECT_EQ(totalWeight, 30);
    EXPECT_EQ(wildMonIndex, 1);
}
