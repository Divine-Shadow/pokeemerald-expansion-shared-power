#include "global.h"
#include "automation_beacon.h"
#include "gpu_regs.h"
#include "main.h"
#include "palette.h"
#include "sprite.h"
#include "constants/rgb.h"

#if AUTOMATION_BEACON

#define BEACON_OAM_INDEX 127
#define BEACON_TILE_NUM 1023
#define BEACON_PALETTE_NUM 15
#define BEACON_TILE_TAG 0xBEAC
#define BEACON_PALETTE_TAG 0xBEAD
#define BEACON_TILE_SIZE 32
#define BEACON_SCREEN_X 16
#define BEACON_SCREEN_Y 16
#define BEACON_VISIBLE_PALETTE_INDEX(value) ((value) + 1)

static u8 sStageId;
static u8 sSubstageId;
static u8 sFlags;
static u8 sPulse;
static u8 sGender = AUTOMATION_BEACON_GENDER_UNKNOWN;
static u8 sNameLen;
static u8 sNameChar0;
static u8 sMapKind = AUTOMATION_BEACON_MAP_UNKNOWN;
static u8 sStarterSelection;
static u8 sInputReady;
static u8 sErrorCode;
static u8 sPlayerX;
static u8 sPlayerY;
static u8 sPlayerFacing;
static u8 sFrontX;
static u8 sFrontY;
static u8 sMapSlot;
static u8 sPlayerXHi;
static u8 sPlayerYHi;
static u8 sFrontXHi;
static u8 sFrontYHi;
static u8 sScriptStep;
static u8 sSpriteId;
static u8 sBeaconTileBuffer[BEACON_TILE_SIZE];

static const u16 sBeaconPalette[16] =
{
    RGB(0, 0, 0),      // Transparent index 0.
    RGB(2, 2, 2),      // value 0
    RGB(31, 31, 31),   // value 1
    RGB(31, 0, 0),     // value 2
    RGB(0, 31, 0),     // value 3
    RGB(0, 0, 31),     // value 4
    RGB(31, 31, 0),    // value 5
    RGB(31, 0, 31),    // value 6
    RGB(0, 31, 31),    // value 7
    RGB(16, 16, 16),   // value 8
    RGB(31, 16, 0),    // value 9
    RGB(16, 31, 0),    // value 10
    RGB(0, 31, 16),    // value 11
    RGB(0, 16, 31),    // value 12
    RGB(16, 0, 31),    // value 13
    RGB(31, 0, 16),    // value 14
};

static const union AnimCmd sBeaconAnim[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sBeaconAnims[] =
{
    sBeaconAnim,
};

static const struct OamData sBeaconOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = ST_OAM_SQUARE,
    .size = ST_OAM_SIZE_0,
    .priority = 0,
};

static const struct SpriteSheet sBeaconSpriteSheet =
{
    .data = sBeaconTileBuffer,
    .size = sizeof(sBeaconTileBuffer),
    .tag = BEACON_TILE_TAG,
};

static const struct SpritePalette sBeaconSpritePalette =
{
    .data = sBeaconPalette,
    .tag = BEACON_PALETTE_TAG,
};

static const struct SpriteTemplate sBeaconSpriteTemplate =
{
    .tileTag = BEACON_TILE_TAG,
    .paletteTag = BEACON_PALETTE_TAG,
    .oam = &sBeaconOam,
    .anims = sBeaconAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static u8 ClampBeaconValue(u8 value)
{
    if (value > AUTOMATION_BEACON_VALUE_MAX)
        return AUTOMATION_BEACON_VALUE_MAX;
    return value;
}

static void WriteBeaconRow(volatile u8 *tile, u8 row, const u8 *values)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        u8 left = BEACON_VISIBLE_PALETTE_INDEX(ClampBeaconValue(values[i * 2]));
        u8 right = BEACON_VISIBLE_PALETTE_INDEX(ClampBeaconValue(values[i * 2 + 1]));
        tile[row * 4 + i] = left | (right << 4);
    }
}

static void WriteBeaconTile(void)
{
    u8 i;
    u8 checksum;
    u8 header[8];
    u8 proof[8];
    u8 nav[8];
    u8 map[8];
    u8 *tile = sBeaconTileBuffer;
    volatile u16 *fallbackTile = (u16 *)(OBJ_VRAM0 + BEACON_TILE_NUM * BEACON_TILE_SIZE);

    for (i = 0; i < BEACON_TILE_SIZE; i++)
        tile[i] = 0;

    checksum = (AUTOMATION_BEACON_PROTOCOL_VERSION + sStageId + sSubstageId + sFlags + sPulse) % 15;
    header[0] = 14;
    header[1] = 13;
    header[2] = AUTOMATION_BEACON_PROTOCOL_VERSION;
    header[3] = sStageId;
    header[4] = sSubstageId;
    header[5] = sFlags;
    header[6] = sPulse;
    header[7] = checksum;

    proof[0] = sGender;
    proof[1] = sNameLen;
    proof[2] = sNameChar0;
    proof[3] = sMapKind;
    proof[4] = sStarterSelection;
    proof[5] = sInputReady;
    proof[6] = sErrorCode;
    proof[7] = 0;

    nav[0] = 12;
    nav[1] = 11;
    nav[2] = sPlayerX;
    nav[3] = sPlayerY;
    nav[4] = sPlayerFacing;
    nav[5] = sFrontX;
    nav[6] = sFrontY;
    nav[7] = (nav[0] + nav[1] + nav[2] + nav[3] + nav[4] + nav[5] + nav[6]) % 15;

    map[0] = 10;
    map[1] = 9;
    map[2] = sMapSlot;
    map[3] = sPlayerXHi;
    map[4] = sPlayerYHi;
    map[5] = sFrontXHi;
    map[6] = sFrontYHi;
    map[7] = (map[0] + map[1] + map[2] + map[3] + map[4] + map[5] + map[6]) % 15;

    WriteBeaconRow(tile, 0, header);
    WriteBeaconRow(tile, 1, proof);
    WriteBeaconRow(tile, 2, nav);
    WriteBeaconRow(tile, 3, map);

    for (i = 0; i < BEACON_TILE_SIZE / 2; i++)
        fallbackTile[i] = tile[i * 2] | (tile[i * 2 + 1] << 8);
}

static void WriteBeaconPalette(void)
{
    u8 i;
    u16 *unfaded = &gPlttBufferUnfaded[OBJ_PLTT_ID(BEACON_PALETTE_NUM)];
    u16 *faded = &gPlttBufferFaded[OBJ_PLTT_ID(BEACON_PALETTE_NUM)];
    volatile u16 *palette = (u16 *)PLTT + OBJ_PLTT_ID(BEACON_PALETTE_NUM);

    for (i = 0; i < ARRAY_COUNT(sBeaconPalette); i++)
    {
        unfaded[i] = sBeaconPalette[i];
        faded[i] = sBeaconPalette[i];
        palette[i] = sBeaconPalette[i];
    }
}

static void WriteBeaconOam(void)
{
    struct OamData *buffer = &gMain.oamBuffer[BEACON_OAM_INDEX];

    buffer->y = BEACON_SCREEN_Y;
    buffer->affineMode = ST_OAM_AFFINE_OFF;
    buffer->objMode = ST_OAM_OBJ_NORMAL;
    buffer->mosaic = FALSE;
    buffer->bpp = ST_OAM_4BPP;
    buffer->shape = ST_OAM_SQUARE;
    buffer->x = BEACON_SCREEN_X;
    buffer->matrixNum = 0;
    buffer->size = ST_OAM_SIZE_0;
    buffer->tileNum = BEACON_TILE_NUM;
    buffer->priority = 0;
    buffer->paletteNum = BEACON_PALETTE_NUM;
    buffer->affineParam = 0;
    CpuCopy16(buffer, (struct OamData *)OAM + BEACON_OAM_INDEX, sizeof(*buffer));
}

static bool8 BeaconSpriteIsActive(void)
{
    return sSpriteId < MAX_SPRITES
        && gSprites[sSpriteId].inUse
        && gSprites[sSpriteId].template == &sBeaconSpriteTemplate;
}

static void QueueBeaconSpriteTileCopy(void)
{
    u16 tileStart = GetSpriteTileStartByTag(BEACON_TILE_TAG);

    if (tileStart != TAG_NONE)
        RequestSpriteCopy(sBeaconTileBuffer, (u8 *)OBJ_VRAM0 + tileStart * BEACON_TILE_SIZE, BEACON_TILE_SIZE);
}

static void EnsureBeaconSprite(void)
{
    WriteBeaconTile();
    WriteBeaconPalette();

    if (IndexOfSpritePaletteTag(BEACON_PALETTE_TAG) != BEACON_PALETTE_NUM)
        LoadSpritePaletteInSlot(&sBeaconSpritePalette, BEACON_PALETTE_NUM);
    if (GetSpriteTileStartByTag(BEACON_TILE_TAG) == TAG_NONE)
        LoadSpriteSheet(&sBeaconSpriteSheet);

    if (!BeaconSpriteIsActive())
    {
        u32 spriteId = CreateSprite(&sBeaconSpriteTemplate, BEACON_SCREEN_X + 4, BEACON_SCREEN_Y + 4, 0);

        if (spriteId < MAX_SPRITES)
        {
            sSpriteId = spriteId;
            gSprites[sSpriteId].oam.priority = 0;
            gSprites[sSpriteId].invisible = FALSE;
        }
        else
        {
            sSpriteId = SPRITE_NONE;
        }
    }

    QueueBeaconSpriteTileCopy();
}

void AutomationBeacon_SetStage(u8 stageId, u8 substageId, u8 flags)
{
    sStageId = ClampBeaconValue(stageId);
    sSubstageId = ClampBeaconValue(substageId);
    sFlags = ClampBeaconValue(flags);
    EnsureBeaconSprite();
}

void AutomationBeacon_SetProof(u8 gender, u8 nameLen, u8 nameChar0, u8 mapKind, u8 starterSelection, bool8 inputReady)
{
    sGender = ClampBeaconValue(gender);
    sNameLen = ClampBeaconValue(nameLen);
    sNameChar0 = ClampBeaconValue(nameChar0);
    sMapKind = ClampBeaconValue(mapKind);
    sStarterSelection = ClampBeaconValue(starterSelection);
    sInputReady = inputReady ? 1 : 0;
    EnsureBeaconSprite();
}

void AutomationBeacon_SetErrorCode(u8 errorCode)
{
    sErrorCode = ClampBeaconValue(errorCode);
    EnsureBeaconSprite();
}

void AutomationBeacon_SetNavProof(
    u8 playerX,
    u8 playerY,
    u8 playerFacing,
    u8 frontX,
    u8 frontY,
    u8 mapSlot,
    u8 playerXHi,
    u8 playerYHi,
    u8 frontXHi,
    u8 frontYHi)
{
    sPlayerX = ClampBeaconValue(playerX);
    sPlayerY = ClampBeaconValue(playerY);
    sPlayerFacing = ClampBeaconValue(playerFacing);
    sFrontX = ClampBeaconValue(frontX);
    sFrontY = ClampBeaconValue(frontY);
    sMapSlot = ClampBeaconValue(mapSlot);
    sPlayerXHi = ClampBeaconValue(playerXHi);
    sPlayerYHi = ClampBeaconValue(playerYHi);
    sFrontXHi = ClampBeaconValue(frontXHi);
    sFrontYHi = ClampBeaconValue(frontYHi);
    EnsureBeaconSprite();
}

void AutomationBeacon_SetScriptStep(u8 scriptStep)
{
    sScriptStep = ClampBeaconValue(scriptStep);
}

u8 AutomationBeacon_GetScriptStep(void)
{
    return sScriptStep;
}

void AutomationBeacon_Render(void)
{
    SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);
    REG_DISPCNT |= DISPCNT_OBJ_ON;
    WriteBeaconPalette();
    WriteBeaconTile();
    QueueBeaconSpriteTileCopy();
    WriteBeaconOam();
    sPulse++;
    if (sPulse >= 15)
        sPulse = 0;
}

void AutomationBeacon_RenderLate(void)
{
    SetGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON);
    REG_DISPCNT |= DISPCNT_OBJ_ON;
    WriteBeaconPalette();
    WriteBeaconTile();
    QueueBeaconSpriteTileCopy();
    WriteBeaconOam();
}

#endif
