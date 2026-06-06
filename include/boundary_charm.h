#ifndef GUARD_BOUNDARY_CHARM_H
#define GUARD_BOUNDARY_CHARM_H

struct Pokemon;

bool32 IsBoundaryCharmActive(void);
void SetBoundaryCharmActive(bool32 active);
void ToggleBoundaryCharmActive(void);
void ClearBoundaryCharmClaims(void);
bool32 IsBoundaryCharmLocationClaimed(u8 mapSec);
void ClaimBoundaryCharmLocation(u8 mapSec);
void ClaimBoundaryCharmLocationForMon(struct Pokemon *mon);
bool32 IsBoundaryCharmCurrentLocationClaimed(void);
bool32 ShouldSuppressBoundaryCharmEncounter(void);
bool32 ShouldSuppressBoundaryCharmEncounterAt(u8 mapSec);
void ClearBoundaryCharmEncounterSuppressed(void);
bool32 WasBoundaryCharmEncounterSuppressed(void);
bool32 TryStartBoundaryCharmEncounterSuppressedScript(void);

#endif // GUARD_BOUNDARY_CHARM_H
