// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load a 256-color sprite

#include <ugba/ugba.h>

#include "ball.h" // Autogenerated from ball.png

// This defines the tile index where the data of the ball is loaded in tile VRAM
#define BALL_TILES_BASE    (16)

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Set object attributes
    // ---------------------

    // Get pointer to the entry that describes the attributes of object 0. Each
    // entry is composed by fields organized in an arbitrary way.
    oam_entry_t *obj = &MEM_OAM_ENTRIES[0];

    obj->attr0 =
        // Y coordinate
        ATTR0_Y(10) |
        // Disable rotoscale mode
        ATTR0_REGULAR |
        // Use a 256-color palette
        ATTR0_256_COLORS |
        // Object shape
        ATTR0_SHAPE_SQUARE;

    obj->attr1 =
        // X coordinate
        ATTR1_X(20) |
        // Object size
        ATTR1_SIZE_2;

    obj->attr2 =
        // Tile index of the tiles in MEM_VRAM_OBJ memory
        ATTR2_256_COLOR_TILE(BALL_TILES_BASE) |
        // Which palette index to use (not needed for 256-color sprites)
        ATTR2_PALETTE(0) |
        // Priority relative to other layers (sprites and backgrounds)
        ATTR2_PRIORITY(1);

    // The shape of the sprite (ATTR0_SQUARE) and the size (ATTR1_SIZE_32)
    // aren't intuitive. Check the following link for more information:
    //     https://www.coranac.com/tonc/text/regobj.htm#sec-oam-entry

    // Load the palette
    // ----------------

    // For 256-color sprites there is only one available palette that uses all
    // the available palette memory.
    SWI_CpuSet_Copy16(ballPal, MEM_PALETTE_OBJ, ballPalLen);

    // Load the tiles
    // --------------

    // 8 bits (256 colors), 8x8, transform from bits to bytes
#define TILE_SIZE (8 * 8 * 8) / 8
    uint8_t *spr_tiles = (uint8_t *)MEM_VRAM_OBJ + (BALL_TILES_BASE * TILE_SIZE);
    SWI_CpuFastSet_Copy32(ballTiles, spr_tiles, ballTilesLen);

    // Turn on the screen
    // ------------------

    REG_DISPCNT =
        // The mode doesn't matter here, it only affects the backgrounds. This
        // is just an arbitrary value.
        DISPCNT_BG_MODE(0) |
        // Turn on the rendering of sprites.
        DISPCNT_OBJ_ENABLE |
        // Enable 1D mapping. Check "8.2.1. The sprite mapping mode" in the
        // following link for more information:
        //     https://www.coranac.com/tonc/text/regobj.htm#sec-tiles
        DISPCNT_OBJ_1D_MAPPING;

    while (1)
        SWI_VBlankIntrWait();
}
