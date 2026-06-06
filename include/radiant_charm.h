#ifndef GUARD_RADIANT_CHARM_H
#define GUARD_RADIANT_CHARM_H

struct Pokemon;

bool32 IsRadiantCharmActive(void);
void SetRadiantCharmActive(bool32 active);
void ToggleRadiantCharmActive(void);
bool32 ShouldRadiantCharmForceShiny(void);
void ApplyRadiantCharmToEncounterMon(struct Pokemon *mon);

#endif // GUARD_RADIANT_CHARM_H
