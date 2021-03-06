// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <ugba/ugba.h>

EWRAM_CODE uint32_t divide_ewram(uint32_t a, uint32_t b)
{
    uint32_t y = 0;

    while (a > b)
    {
        a -= b;
        y++;
    }

    return y;
}
