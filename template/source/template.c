// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

uint16_t r, g, b;

void VBLHandler(void)
{
    b = 0;

    r = ~REG_KEYINPUT & 0x1F;

    ((uint16_t *)MEM_PALETTE_ADDR)[0] = (b << 10) | (g << 5) | r;
}

void HBLHandler(void)
{
    g = REG_VCOUNT;

    ((uint16_t *)MEM_PALETTE_ADDR)[0] = (b << 10) | (g << 5) | r;
}

void VcountHandler(void)
{
    b = 0x1F;
}

int GBA_main(int argc, char *argv[])
{
    IRQ_SetHandler(IRQ_VBLANK, VBLHandler);
    IRQ_SetHandler(IRQ_HBLANK, HBLHandler);
    IRQ_SetHandler(IRQ_VCOUNT, VcountHandler);
    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_HBLANK);
    IRQ_Enable(IRQ_VCOUNT);

    REG_DISPSTAT |= 100 << 8;

    REG_DISPCNT = 3;

    while (1)
    {
        SWI_VBlankIntrWait();
#if 0
        while (REG_VCOUNT < 160)
        {
            REG_BG2X = REG_VCOUNT << 8;
            GBA_RegisterUpdated(REG_BG2X);
            SWI_Halt();
        }

        SWI_VBlankIntrWait();

        REG_BG2X = 0;
        GBA_RegisterUpdated(REG_BG2X);

        while (REG_VCOUNT >= 160)
            SWI_Halt();
#endif
    }

    return 0;
}
