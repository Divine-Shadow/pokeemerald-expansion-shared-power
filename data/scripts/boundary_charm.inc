EventScript_BoundaryCharmNoEncounters::
	lock
	call EventScript_ShowBoundaryCharmNoEncounters
	releaseall
	end

EventScript_ShowBoundaryCharmNoEncounters::
	msgbox Text_BoundaryCharmNoEncounters, MSGBOX_SIGN
	return

Text_BoundaryCharmNoEncounters:
	.string "This area already has a\n"
	.string "caught POKéMON.$"
