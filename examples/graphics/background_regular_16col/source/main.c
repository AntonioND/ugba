// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load a 16-color background

#include <ugba/ugba.h>

#include "city.h" // Autogenerated from city.png

#define CITY_MAP_PALETTE    (0)
#define CITY_TILES_BASE     ((uintptr_t)MEM_VRAM_BG + 0)
#define CITY_MAP_BASE       ((uintptr_t)MEM_VRAM_BG + 0x4000)

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Load the palette
    VRAM_BGPalette16Copy(cityPal, cityPalLen, CITY_MAP_PALETTE);

    // Load the tiles
    VRAM_BGTiles16Copy(cityTiles, cityTilesLen, 0);

    // Load the map
    SWI_CpuSet_Copy16(cityMap, (void *)CITY_MAP_BASE, cityMapLen);

    // Setup background
    BG_RegularInit(0, BG_REGULAR_512x512, BG_16_COLORS,
                   CITY_TILES_BASE, CITY_MAP_BASE);

    int x = 80, y = 120;
    BG_RegularScrollSet(0, x, y);

    // Set the display to mode 0 so that all backgrounds are in regular mode
    DISPLAY_ModeSet(0);

    // Turn on the background
    REG_DISPCNT |= DISPCNT_BG0_ENABLE;

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        BG_RegularScrollGet(0, &x, &y);

        if (keys & KEY_UP)
            y--;
        else if (keys & KEY_DOWN)
            y++;

        if (keys & KEY_RIGHT)
            x++;
        else if (keys & KEY_LEFT)
            x--;

        BG_RegularScrollSet(0, x, y);
    }
}
