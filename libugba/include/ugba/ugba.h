// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef UGBA_H__
#define UGBA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "background.h"
#include "bios.h"
#include "bios_wrappers.h"
#include "console.h"
#include "dma.h"
#include "display.h"
#include "debug.h"
#include "definitions.h"
#include "hardware.h"
#include "input.h"
#include "interrupts.h"
#include "obj.h"
#include "timer.h"
#include "version.h"
#include "vram.h"

// Initialize library. This function needs to be called at the start of main().
EXPORT_API void UGBA_Init(int *argc, char **argv[]);

#ifndef __GBA__
// Initialize library with no video output (for testing). This function needs to
// be called at the start of main(). Not implemented in GBA as it isn't usedul
// there.
EXPORT_API void UGBA_InitHeadless(int *argc, char **argv[]);
#endif

#ifdef __cplusplus
}
#endif

#endif // UGBA_H__
