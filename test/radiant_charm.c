#include "global.h"
#include "boundary_charm.h"
#include "event_data.h"
#include "highlander_charm.h"
#include "item.h"
#include "pokedex.h"
#include "pokemon.h"
#include "radiant_charm.h"
#include "script_pokemon_util.h"
#include "wild_encounter.h"
#include "test/test.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/pokedex.h"
#include "constants/pokemon.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"
#include "constants/vars.h"

static void ResetRadiantCharmTestState(void)
{
    ClearBag();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    gPlayerPartyCount = 0;
    SetRadiantCharmActive(FALSE);
    SetHighlanderCharmActive(FALSE);
    SetBoundaryCharmActive(FALSE);
    ClearBoundaryCharmClaims();
    ClearBoundaryCharmEncounterSuppressed();
    ResetPokedex();
    VarSet(VAR_REPEL_STEP_COUNT, 0);
}

static void MarkSpeciesCaught(u16 species)
{
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_SET_CAUGHT);
}

static const struct WildPokemonInfo *GetRadiantCharmWaterMonsInfo(void)
{
    static const struct WildPokemon wildMons[WATER_WILD_COUNT] =
    {
        { 7, 7, SPECIES_RATTATA },
        { 7, 7, SPECIES_RATTATA },
        { 7, 7, SPECIES_RATTATA },
        { 7, 7, SPECIES_RATTATA },
        { 7, 7, SPECIES_RATTATA },
    };
    static const struct WildPokemonInfo wildMonInfo =
    {
        .encounterRate = 100,
        .wildPokemon = wildMons,
    };

    return &wildMonInfo;
}

TEST("Radiant Charm uses a saved toggle instead of bag presence")
{
    ResetRadiantCharmTestState();
    EXPECT(!IsRadiantCharmActive());

    EXPECT(AddBagItem(ITEM_RADIANT_CHARM, 1));
    EXPECT(!IsRadiantCharmActive());

    SetRadiantCharmActive(TRUE);
    EXPECT(IsRadiantCharmActive());
    EXPECT(gSaveBlock3Ptr->radiantCharmActive);

    ClearBag();
    EXPECT(IsRadiantCharmActive());

    ToggleRadiantCharmActive();
    EXPECT(!IsRadiantCharmActive());
    EXPECT(!gSaveBlock3Ptr->radiantCharmActive);
}

TEST("Radiant Charm disabled does not force a non-shiny encounter mon shiny")
{
    struct Pokemon mon;
    bool32 isShiny = FALSE;

    ResetRadiantCharmTestState();
    CreateMon(&mon, SPECIES_RATTATA, 7, 0, TRUE, 0, OT_ID_PRESET, 0);
    SetMonData(&mon, MON_DATA_IS_SHINY, &isShiny);

    ApplyRadiantCharmToEncounterMon(&mon);
    EXPECT(!IsMonShiny(&mon));
}

TEST("Radiant Charm forces normal wild encounters shiny without changing species or level")
{
    ResetRadiantCharmTestState();
    SetRadiantCharmActive(TRUE);

    EXPECT(Test_TryGenerateUnfilteredWildMonAtMapSec(GetRadiantCharmWaterMonsInfo(), WILD_AREA_WATER, MAPSEC_ROUTE_102));
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_RATTATA);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 7);
    EXPECT(IsMonShiny(&gEnemyParty[0]));
}

TEST("Radiant Charm forces scripted static wild encounters shiny")
{
    ResetRadiantCharmTestState();
    SetRadiantCharmActive(TRUE);

    CreateScriptedWildMon(SPECIES_KECLEON, 30, ITEM_NONE);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_KECLEON);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 30);
    EXPECT(IsMonShiny(&gEnemyParty[0]));
}

TEST("Radiant Charm forces both scripted double static wild mons shiny")
{
    ResetRadiantCharmTestState();
    SetRadiantCharmActive(TRUE);

    CreateScriptedDoubleWildMon(SPECIES_VOLTORB, 25, ITEM_NONE, SPECIES_ELECTRODE, 30, ITEM_NONE);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_VOLTORB);
    EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_SPECIES), SPECIES_ELECTRODE);
    EXPECT(IsMonShiny(&gEnemyParty[0]));
    EXPECT(IsMonShiny(&gEnemyParty[1]));
}

TEST("Radiant Charm does not bypass Highlander Charm empty encounter pools")
{
    ResetRadiantCharmTestState();
    MarkSpeciesCaught(SPECIES_RATTATA);
    SetHighlanderCharmActive(TRUE);
    SetRadiantCharmActive(TRUE);

    EXPECT(!Test_TryGenerateHighlanderWildMon(GetRadiantCharmWaterMonsInfo(), WILD_AREA_WATER));
    EXPECT(WasHighlanderCharmEncounterEmpty());
}

TEST("Radiant Charm does not bypass Extinction Charm claimed locations")
{
    ResetRadiantCharmTestState();
    ClaimBoundaryCharmLocation(MAPSEC_ROUTE_102);
    SetBoundaryCharmActive(TRUE);
    SetRadiantCharmActive(TRUE);

    EXPECT(!Test_TryGenerateBoundaryWildMonAtMapSec(GetRadiantCharmWaterMonsInfo(), WILD_AREA_WATER, MAPSEC_ROUTE_102));
    EXPECT(WasBoundaryCharmEncounterSuppressed());
}

TEST("Radiant Charm does not force gift Pokemon shiny")
{
    ResetRadiantCharmTestState();
    SetRadiantCharmActive(TRUE);

    EXPECT_EQ(ScriptGiveMon(SPECIES_RATTATA, 7, ITEM_NONE), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_RATTATA);
    EXPECT(!IsMonShiny(&gPlayerParty[0]));
}
