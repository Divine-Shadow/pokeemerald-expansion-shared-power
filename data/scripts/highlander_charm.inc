EventScript_HighlanderCharmNoEncounters::
	lock
	call EventScript_ShowHighlanderCharmNoEncounters
	releaseall
	end

EventScript_ShowHighlanderCharmNoEncounters::
	msgbox Text_HighlanderCharmNoEncounters, MSGBOX_SIGN
	return

Text_HighlanderCharmNoEncounters:
	.string "No unfamiliar POKéMON seem to be\n"
	.string "nearby.$"
