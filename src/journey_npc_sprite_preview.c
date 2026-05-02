#include "global.h"
#include "journey_npc_sprite_preview.h"

#if AUTOMATION_PROBE

#include "event_object_movement.h"
#include "gpu_regs.h"
#include "main.h"
#include "menu_helpers.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "trainer_pokemon_sprites.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"
#include "constants/trainers.h"

static void CB2_JourneyNpcSpritePreview(void);
static void VBlankCB_JourneyNpcSpritePreview(void);

static void CreateBarryOverworldSprite(s16 x, s16 y, u8 animNum)
{
    u8 spriteId = CreateObjectGraphicsSprite(OBJ_EVENT_GFX_BERRY_BARRY, SpriteCallbackDummy, x, y, 0);

    if (spriteId != MAX_SPRITES)
        StartSpriteAnim(&gSprites[spriteId], animNum);
}

void CB2_InitJourneyNpcSpritePreview(u32 mode)
{
    u16 backdrop = RGB(18, 22, 17);
    bool32 showAll = mode == JOURNEY_NPC_SPRITE_PREVIEW_ALL;

    SetVBlankCallback(NULL);
    ResetVramOamAndBgCntRegs();
    ResetPaletteFade();
    ResetSpriteData();
    ResetOamRange(0, 128);
    LoadOam();
    FreeAllSpritePalettes();
    ResetTasks();
    LoadPalette(&backdrop, BG_PLTT_ID(0), sizeof(backdrop));

    if (showAll || mode == JOURNEY_NPC_SPRITE_PREVIEW_BARRY_TRAINER)
        CreateTrainerPicSprite(TRAINER_PIC_BERRY_BARRY, TRUE, showAll ? 68 : 120, 78, 0, TAG_NONE);
    if (showAll || mode == JOURNEY_NPC_SPRITE_PREVIEW_BARRY_OW_SOUTH)
        CreateBarryOverworldSprite(showAll ? 144 : 120, 76, ANIM_STD_FACE_SOUTH);
    if (showAll || mode == JOURNEY_NPC_SPRITE_PREVIEW_BARRY_OW_NORTH)
        CreateBarryOverworldSprite(showAll ? 168 : 120, 76, ANIM_STD_FACE_NORTH);
    if (showAll || mode == JOURNEY_NPC_SPRITE_PREVIEW_BARRY_OW_WEST)
        CreateBarryOverworldSprite(showAll ? 192 : 120, 76, ANIM_STD_FACE_WEST);
    if (showAll || mode == JOURNEY_NPC_SPRITE_PREVIEW_BARRY_OW_EAST)
        CreateBarryOverworldSprite(showAll ? 216 : 120, 76, ANIM_STD_FACE_EAST);

    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    SetVBlankCallback(VBlankCB_JourneyNpcSpritePreview);
    SetMainCallback2(CB2_JourneyNpcSpritePreview);
}

static void CB2_JourneyNpcSpritePreview(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
}

static void VBlankCB_JourneyNpcSpritePreview(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

#endif // AUTOMATION_PROBE
