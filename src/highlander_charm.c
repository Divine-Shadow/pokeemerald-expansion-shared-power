#include "global.h"
#include "highlander_charm.h"
#include "event_scripts.h"
#include "script.h"

EWRAM_DATA static bool8 sHighlanderCharmEncounterEmpty = FALSE;

bool32 IsHighlanderCharmActive(void)
{
    return gSaveBlock3Ptr->highlanderCharmActive;
}

void SetHighlanderCharmActive(bool32 active)
{
    gSaveBlock3Ptr->highlanderCharmActive = active ? TRUE : FALSE;
}

void ToggleHighlanderCharmActive(void)
{
    SetHighlanderCharmActive(!IsHighlanderCharmActive());
}

void ClearHighlanderCharmEncounterEmpty(void)
{
    sHighlanderCharmEncounterEmpty = FALSE;
}

void MarkHighlanderCharmEncounterEmpty(void)
{
    sHighlanderCharmEncounterEmpty = TRUE;
}

bool32 WasHighlanderCharmEncounterEmpty(void)
{
    return sHighlanderCharmEncounterEmpty;
}

bool32 TryStartHighlanderCharmEmptyEncounterScript(void)
{
    if (!sHighlanderCharmEncounterEmpty)
        return FALSE;

    sHighlanderCharmEncounterEmpty = FALSE;
    ScriptContext_SetupScript(EventScript_HighlanderCharmNoEncounters);
    return TRUE;
}
