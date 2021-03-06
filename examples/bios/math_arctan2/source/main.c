// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that plots angle = arctan2(x, y). It needs to use color to draw it as
// it isn't practical to generate a 3D plot.

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(3);
    DISP_LayersEnable(0, 0, 1, 0, 0);

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *framebuffer = BG_Mode3FramebufferGet();

    uint32_t zero = 0;
    size_t size = GBA_SCREEN_W * GBA_SCREEN_H * sizeof(uint16_t);
    SWI_CpuFastSet_Fill32(&zero, framebuffer, size);

    for (uint16_t y = 0; y < GBA_SCREEN_H; y++)
    {
        for (uint16_t x = 0; x < GBA_SCREEN_W; x++)
        {
            int16_t xdelta = x - (GBA_SCREEN_W / 2);
            int16_t ydelta = y - (GBA_SCREEN_H / 2);

            uint16_t val = SWI_ArcTan2(xdelta, ydelta) >> 1;

            framebuffer[y * GBA_SCREEN_W + x] = val;
        }
    }

    while (1)
        SWI_VBlankIntrWait();
}
