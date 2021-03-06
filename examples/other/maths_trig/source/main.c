// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that plots y = sin(x) and y = cos(x) scaled to match the screen size.

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

    for (uint16_t x = 0; x < GBA_SCREEN_W; x++)
    {
        int32_t screen_h_half = GBA_SCREEN_H / 2;

        {
            // Draw two full waves
            int32_t sine = FP_Sin(x * FP_2_PI * 2 / GBA_SCREEN_W);
            int32_t y = screen_h_half - 1 - ((sine * screen_h_half) >> 16);
            framebuffer[y * GBA_SCREEN_W + x] = RGB15(31, 0, 0);
        }

        {
            // Draw two full waves
            int32_t cosine = FP_Cos(x * FP_2_PI * 2 / GBA_SCREEN_W);
            int32_t y = screen_h_half - 1 - ((cosine * screen_h_half) >> 16);

            framebuffer[y * GBA_SCREEN_W + x] = RGB15(0, 31, 0);
        }
    }

    while (1)
        SWI_VBlankIntrWait();
}
