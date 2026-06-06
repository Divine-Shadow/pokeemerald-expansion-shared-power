#include "global.h"
#include "boundary_charm.h"
#include "event_scripts.h"
#include "overworld.h"
#include "pokemon.h"
#include "script.h"
#include "constants/region_map_sections.h"

EWRAM_DATA static bool8 sBoundaryCharmEncounterSuppressed = FALSE;

static bool32 IsBoundaryCharmValidMapSec(u8 mapSec)
{
    return mapSec < MAPSEC_NONE;
}

bool32 IsBoundaryCharmActive(void)
{
    return gSaveBlock3Ptr->boundaryCharmActive;
}

void SetBoundaryCharmActive(bool32 active)
{
    gSaveBlock3Ptr->boundaryCharmActive = active ? TRUE : FALSE;
}

void ToggleBoundaryCharmActive(void)
{
    SetBoundaryCharmActive(!IsBoundaryCharmActive());
}

void ClearBoundaryCharmClaims(void)
{
    memset(gSaveBlock3Ptr->boundaryCharmMapSecFlags, 0, sizeof(gSaveBlock3Ptr->boundaryCharmMapSecFlags));
}

bool32 IsBoundaryCharmLocationClaimed(u8 mapSec)
{
    if (!IsBoundaryCharmValidMapSec(mapSec))
        return FALSE;

    return (gSaveBlock3Ptr->boundaryCharmMapSecFlags[mapSec / 8] & (1 << (mapSec % 8))) != 0;
}

void ClaimBoundaryCharmLocation(u8 mapSec)
{
    if (!IsBoundaryCharmValidMapSec(mapSec))
        return;

    gSaveBlock3Ptr->boundaryCharmMapSecFlags[mapSec / 8] |= (1 << (mapSec % 8));
}

void ClaimBoundaryCharmLocationForMon(struct Pokemon *mon)
{
    u8 mapSec = GetMonData(mon, MON_DATA_MET_LOCATION, NULL);

    ClaimBoundaryCharmLocation(mapSec);
}

bool32 IsBoundaryCharmCurrentLocationClaimed(void)
{
    return IsBoundaryCharmLocationClaimed(GetCurrentRegionMapSectionId());
}

bool32 ShouldSuppressBoundaryCharmEncounterAt(u8 mapSec)
{
    sBoundaryCharmEncounterSuppressed = FALSE;
    if (!IsBoundaryCharmActive())
        return FALSE;
    if (!IsBoundaryCharmLocationClaimed(mapSec))
        return FALSE;

    sBoundaryCharmEncounterSuppressed = TRUE;
    return TRUE;
}

bool32 ShouldSuppressBoundaryCharmEncounter(void)
{
    return ShouldSuppressBoundaryCharmEncounterAt(GetCurrentRegionMapSectionId());
}

void ClearBoundaryCharmEncounterSuppressed(void)
{
    sBoundaryCharmEncounterSuppressed = FALSE;
}

bool32 WasBoundaryCharmEncounterSuppressed(void)
{
    return sBoundaryCharmEncounterSuppressed;
}

bool32 TryStartBoundaryCharmEncounterSuppressedScript(void)
{
    if (!sBoundaryCharmEncounterSuppressed)
        return FALSE;

    sBoundaryCharmEncounterSuppressed = FALSE;
    ScriptContext_SetupScript(EventScript_BoundaryCharmNoEncounters);
    return TRUE;
}
