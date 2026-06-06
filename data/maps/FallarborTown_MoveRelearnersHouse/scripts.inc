FallarborTown_MoveRelearnersHouse_MapScripts::
	.byte 0

FallarborTown_MoveRelearnersHouse_EventScript_MoveRelearner::
	lockall
	applymovement LOCALID_MOVE_RELEARNER, Common_Movement_FacePlayer
	waitmovement 0
	goto_if_set FLAG_TEMP_1, FallarborTown_MoveRelearnersHouse_EventScript_AskTeachMove
	msgbox FallarborTown_MoveRelearnersHouse_Text_ImTheMoveTutor, MSGBOX_DEFAULT
	setflag FLAG_TEMP_1
	goto FallarborTown_MoveRelearnersHouse_EventScript_AskTeachMove
	end

FallarborTown_MoveRelearnersHouse_EventScript_AskTeachMove::
	checkitem ITEM_HEART_SCALE
	goto_if_eq VAR_RESULT, FALSE, FallarborTown_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	msgbox FallarborTown_MoveRelearnersHouse_Text_ThatsAHeartScaleWantMeToTeachMove, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, FallarborTown_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	goto FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon::
	msgbox FallarborTown_MoveRelearnersHouse_Text_TutorWhichMon, MSGBOX_DEFAULT
	special ChooseMonForMoveRelearner
	waitstate
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, FallarborTown_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	special IsSelectedMonEgg
	goto_if_eq VAR_RESULT, TRUE, FallarborTown_MoveRelearnersHouse_EventScript_CantTeachEgg
	goto_if_eq VAR_0x8005, 0, FallarborTown_MoveRelearnersHouse_EventScript_NoMoveToTeachMon
	goto FallarborTown_MoveRelearnersHouse_EventScript_ChooseMove
	end

FallarborTown_MoveRelearnersHouse_EventScript_ChooseMove::
	msgbox FallarborTown_MoveRelearnersHouse_Text_TeachWhichMove, MSGBOX_DEFAULT
	special TeachMoveRelearnerMove
	waitstate
	goto_if_eq VAR_0x8004, 0, FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon
	msgbox FallarborTown_MoveRelearnersHouse_Text_HandedOverHeartScale, MSGBOX_DEFAULT
	removeitem ITEM_HEART_SCALE
	goto FallarborTown_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale
	end

FallarborTown_MoveRelearnersHouse_EventScript_NoMoveToTeachMon::
	msgbox FallarborTown_MoveRelearnersHouse_Text_DontHaveMoveToTeachPokemon, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_CantTeachEgg::
	msgbox FallarborTown_MoveRelearnersHouse_Text_CantTeachEgg, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_ChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapper::
	lock
	faceplayer
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperIntro, MSGBOX_DEFAULT
	getpartysize
	goto_if_eq VAR_RESULT, 0, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoPokemon
	checkitem ITEM_HEART_SCALE
	goto_if_eq VAR_RESULT, FALSE, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoHeartScale
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperAsk, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperChooseMon::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperChooseMon, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	goto_if_ge VAR_0x8004, PARTY_SIZE, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	specialvar VAR_RESULT, AbilitySwapper_TryPreview
	switch VAR_RESULT
	case ABILITY_SWAPPER_RESULT_READY, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperPreview
	case ABILITY_SWAPPER_RESULT_CANCELLED, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	case ABILITY_SWAPPER_RESULT_EGG, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperEgg
	case ABILITY_SWAPPER_RESULT_HIDDEN_ABILITY, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperHiddenAbility
	case ABILITY_SWAPPER_RESULT_NO_ORDINARY_SWAP, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoOrdinarySwap
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperPreview::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperPreview, MSGBOX_YESNO
	switch VAR_RESULT
	case NO, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	specialvar VAR_RESULT, AbilitySwapper_TrySwap
	switch VAR_RESULT
	case ABILITY_SWAPPER_RESULT_SWAPPED, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperSwapped
	case ABILITY_SWAPPER_RESULT_NO_HEART_SCALE, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoHeartScale
	case ABILITY_SWAPPER_RESULT_EGG, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperEgg
	case ABILITY_SWAPPER_RESULT_HIDDEN_ABILITY, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperHiddenAbility
	case ABILITY_SWAPPER_RESULT_NO_ORDINARY_SWAP, FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoOrdinarySwap
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperSwapped::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperSwapped, MSGBOX_DEFAULT
	release
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoHeartScale::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoHeartScale, MSGBOX_DEFAULT
	release
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoPokemon::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoPokemon, MSGBOX_DEFAULT
	release
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperEgg::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperEgg, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperHiddenAbility::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperHiddenAbility, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperNoOrdinarySwap::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoOrdinarySwap, MSGBOX_DEFAULT
	goto FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperChooseMon
	end

FallarborTown_MoveRelearnersHouse_EventScript_AbilitySwapperGoodbye::
	msgbox FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperGoodbye, MSGBOX_DEFAULT
	release
	end

FallarborTown_MoveRelearnersHouse_EventScript_ComeBackWithHeartScale::
	msgbox FallarborTown_MoveRelearnersHouse_Text_ComeBackWithHeartScale, MSGBOX_DEFAULT
	releaseall
	end

FallarborTown_MoveRelearnersHouse_Text_ImTheMoveTutor:
	.string "I'm the MOVE TUTOR.\p"
	.string "I know all the moves that POKéMON\n"
	.string "learn--every one of them--and I can\l"
	.string "teach POKéMON those moves.\p"
	.string "I can teach a move to a POKéMON\n"
	.string "of yours if you'd like.\p"
	.string "I'll do it for a HEART SCALE.\n"
	.string "I'm collecting those now.$"

FallarborTown_MoveRelearnersHouse_Text_ThatsAHeartScaleWantMeToTeachMove:
	.string "Oh! That's it! That's an honest to\n"
	.string "goodness HEART SCALE!\p"
	.string "Let me guess, you want me to teach\n"
	.string "a move?$"

FallarborTown_MoveRelearnersHouse_Text_TutorWhichMon:
	.string "Which POKéMON needs tutoring?$"

FallarborTown_MoveRelearnersHouse_Text_TeachWhichMove:
	.string "Which move should I teach?$"

FallarborTown_MoveRelearnersHouse_Text_DontHaveMoveToTeachPokemon:
	.string "Sorry…\p"
	.string "It doesn't appear as if I have any move\n"
	.string "I can teach that POKéMON.$"

FallarborTown_MoveRelearnersHouse_Text_HandedOverHeartScale:
	.string "{PLAYER} handed over one HEART SCALE\n"
	.string "in exchange.$"

FallarborTown_MoveRelearnersHouse_Text_ComeBackWithHeartScale:
	.string "If your POKéMON need to learn a move,\n"
	.string "come back with a HEART SCALE.$"

FallarborTown_MoveRelearnersHouse_Text_CantTeachEgg:
	.string "Hunh? There isn't a single move that\n"
	.string "I can teach an EGG.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperIntro:
	.string "I'm the ABILITY SWAPPER.\p"
	.string "For one HEART SCALE, I can help a\n"
	.string "POKéMON switch between its ordinary\l"
	.string "abilities.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperAsk:
	.string "Hidden abilities are beyond my work.\p"
	.string "Want me to tune up a POKéMON's\n"
	.string "ordinary ability?$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperChooseMon:
	.string "Which POKéMON should I study?$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperPreview:
	.string "{STR_VAR_1} has {STR_VAR_2}.\p"
	.string "I can change it to {STR_VAR_3}\n"
	.string "for one HEART SCALE. Shall I?$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperSwapped:
	.string "Done! {STR_VAR_1}'s ability\n"
	.string "became {STR_VAR_3}.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoHeartScale:
	.string "You'll need a HEART SCALE\n"
	.string "for my work.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoPokemon:
	.string "I need a POKéMON to study first.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperEgg:
	.string "I can't study an EGG's ability.\n"
	.string "Choose a hatched POKéMON.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperHiddenAbility:
	.string "{STR_VAR_1}'s HIDDEN ABILITY\n"
	.string "isn't mine to alter.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperNoOrdinarySwap:
	.string "{STR_VAR_1} doesn't have another\n"
	.string "ordinary ability I can reach.$"

FallarborTown_MoveRelearnersHouse_Text_AbilitySwapperGoodbye:
	.string "Come back if you want an ordinary\n"
	.string "ability tuned.$"
