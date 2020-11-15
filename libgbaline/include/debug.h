// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DEBUG_H__
#define DEBUG_H__

// This file has debug utilities that don't work on GBA binaries. However, there
// are defines here that remove them automatically so that code that uses them
// can still build without errors.

#include <stdint.h>

#ifdef __GBA__
#define Debug_Autotest()    0
#define Debug_Screenshot()  do { } while (0)
#else
int Debug_Autotest(void);
void Debug_Screenshot(void);
#endif

#endif // DEBUG_H__
