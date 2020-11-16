// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load a 16-color sprite

#include <string.h>

#include <gbaline.h>

#include "ball.h" // Autogenerated from ball.png

// This defines the tile index where the data of the ball is loaded in tile VRAM
#define BALL_TILES_BASE     (16)

// This is the palette index to be used for the sprite. There are 16 palettes of
// 16 colors each available.
#define BALL_PALETTE        (2)

int main(int argc, char *argv[])
{
    GBALINE_Init(&argc, &argv);

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
        // Use a 16-color palette
        ATTR0_16_COLORS |
        // Object shape
        ATTR0_SHAPE_SQUARE;

    obj->attr1 =
        // X coordinate
        ATTR1_X(20) |
        // Object size
        ATTR1_SIZE_2;

    obj->attr2 =
        // Tile index of the tiles in MEM_VRAM_OBJ memory
        ATTR2_16_COLOR_TILE(BALL_TILES_BASE) |
        // Which palette index to use (not needed for 256-color sprites)
        ATTR2_PALETTE(BALL_PALETTE) |
        // Priority relative to other layers (sprites and backgrounds)
        ATTR2_PRIORITY(1);

    // The shape of the sprite and the size aren't intuitive. Check the
    // following link for more information:
    //     https://www.coranac.com/tonc/text/regobj.htm#sec-oam-entry

    // Load the palette
    // ----------------

    // Calculate the base address of the sprite palette. There are 16 possible
    // palettes of 16 colors each. The base address where they can be found is
    // MEM_PALETTE_OBJ. The 16 palettes could be used together as one 256 color
    // palette.
    uint16_t *spr_pal_2 = MEM_PALETTE_OBJ + (BALL_PALETTE * 16);
    memcpy(spr_pal_2, ballPal, ballPalLen);

    // Load the tiles
    // --------------

    // 4 bits (16 colors), 8x8, transform from bits to bytes
#define TILE_SIZE (4 * 8 * 8) / 8
    uint8_t *spr_tiles = (uint8_t *)MEM_VRAM_OBJ + (BALL_TILES_BASE * TILE_SIZE);
    memcpy(spr_tiles, ballTiles, ballTilesLen);

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


