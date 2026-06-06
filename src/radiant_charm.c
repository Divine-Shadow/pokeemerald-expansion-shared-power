#include "global.h"
#include "radiant_charm.h"
#include "pokemon.h"

bool32 IsRadiantCharmActive(void)
{
    return gSaveBlock3Ptr->radiantCharmActive;
}

void SetRadiantCharmActive(bool32 active)
{
    gSaveBlock3Ptr->radiantCharmActive = active ? TRUE : FALSE;
}

void ToggleRadiantCharmActive(void)
{
    SetRadiantCharmActive(!IsRadiantCharmActive());
}

bool32 ShouldRadiantCharmForceShiny(void)
{
    return IsRadiantCharmActive();
}

void ApplyRadiantCharmToEncounterMon(struct Pokemon *mon)
{
    bool32 isShiny = TRUE;

    if (!ShouldRadiantCharmForceShiny())
        return;

    SetMonData(mon, MON_DATA_IS_SHINY, &isShiny);
}
