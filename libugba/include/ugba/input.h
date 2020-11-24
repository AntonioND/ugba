// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef INPUT_H__
#define INPUT_H__

#include <stdint.h>

#include "definitions.h"

EXPORT_API void KEYS_Update(void);

EXPORT_API uint16_t KEYS_Pressed(void);
EXPORT_API uint16_t KEYS_Held(void);
EXPORT_API uint16_t KEYS_Released(void);

#endif // INPUT_H__