// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example of how to use DMA triggered by the HBL period in each scanline

#include <ugba/ugba.h>

#include "city.h" // Autogenerated from city.png

#define CITY_MAP_PALETTE    (0)
#define CITY_TILES_BASE     MEM_BG_TILES_BLOCK_ADDR(0)
#define CITY_MAP_BASE       MEM_BG_MAP_BLOCK_ADDR(8)

void load_bg(void)
{
    // Load the palette
    VRAM_BGPalette16Copy(cityPal, cityPalLen, CITY_MAP_PALETTE);

    // Load the tiles
    SWI_CpuSet_Copy16(cityTiles, (void *)CITY_TILES_BASE, cityTilesLen);

    // Load the map
    SWI_CpuSet_Copy16(cityMap, (void *)CITY_MAP_BASE, cityMapLen);

    // Setup background
    BG_RegularInit(0, BG_REGULAR_512x512, BG_16_COLORS,
                   CITY_TILES_BASE, CITY_MAP_BASE);
}

#define PTR_REG_BG0HOFS     ((uint16_t *)PTR_REG_16(OFFSET_BG0HOFS))

int x = 80, y = 120;
uint16_t horizontal_offsets[160];

void fill_array(void)
{
    for (int i = 0; i < 160; i++)
        horizontal_offsets[i] = x + ((i & 16) ? (i & 15) : 15 - (i & 15));
}

void vbl_handler(void)
{
    // Stop the DMA copy already running
    DMA_Stop(0);

    // The first HBL copy is done after line 0 is drawn. It is needed to start
    // the copy from index 1 in the array, and to set the first offset
    // explicitly during VBL.
    DMA_Transfer(0, &(horizontal_offsets[1]), PTR_REG_BG0HOFS, 2,
                 DMACNT_DST_FIXED | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_16_BITS | DMACNT_START_HBLANK |
                 DMACNT_REPEAT_ON);

    REG_BG0HOFS = horizontal_offsets[0];
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    load_bg();
    fill_array();
    REG_BG0VOFS = y;

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);
    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);

    // Set the display to mode 0 so that all backgrounds are in regular mode,
    // and turn on background 0.
    DISP_ModeSet(0);
    DISP_LayersEnable(1, 0, 0, 0, 0);

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        if (keys & KEY_UP)
            y--;
        else if (keys & KEY_DOWN)
            y++;

        if (keys & KEY_RIGHT)
            x++;
        else if (keys & KEY_LEFT)
            x--;

        fill_array();
        REG_BG0VOFS = y;
    }
}
