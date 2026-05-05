LilycoveCity_DepartmentStore_4F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_PrecisionItems
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_PrecisionItems:
	.2byte ITEM_WIDE_LENS
	.2byte ITEM_SCOPE_LENS
	.2byte ITEM_SHELL_BELL
	.2byte ITEM_QUICK_CLAW
	.2byte ITEM_KINGS_ROCK
	pokemartlistend

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_PowerItems
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_PowerItems:
	.2byte ITEM_MUSCLE_BAND
	.2byte ITEM_WISE_GLASSES
	.2byte ITEM_EXPERT_BELT
	.2byte ITEM_RAZOR_CLAW
	.2byte ITEM_FOCUS_BAND
	pokemartlistend

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM:
	.string "Hmm…\p"
	.string "A sharper strike…\n"
	.string "Or a steadier one…\p"
	.string "It's no easy matter to decide which\n"
	.string "held item a POKéMON should carry…$"

LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs:
	.string "This floor used to stock many\n"
	.string "different kinds of TMs.\p"
	.string "Now it specializes in held items for\n"
	.string "serious TRAINERS.$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves:
	.string "A POKéMON can hold only one item.\p"
	.string "That makes choosing the right one\n"
	.string "part of the strategy.$"
