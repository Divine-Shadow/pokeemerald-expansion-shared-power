#ifndef GUARD_HIGHLANDER_CHARM_H
#define GUARD_HIGHLANDER_CHARM_H

bool32 IsHighlanderCharmActive(void);
void SetHighlanderCharmActive(bool32 active);
void ToggleHighlanderCharmActive(void);
void ClearHighlanderCharmEncounterEmpty(void);
void MarkHighlanderCharmEncounterEmpty(void);
bool32 WasHighlanderCharmEncounterEmpty(void);
bool32 TryStartHighlanderCharmEmptyEncounterScript(void);

#endif // GUARD_HIGHLANDER_CHARM_H
