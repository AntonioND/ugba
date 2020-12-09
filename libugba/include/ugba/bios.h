// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef BIOS_H__
#define BIOS_H__

#include "definitions.h"
#include "hardware.h"

// Note: Names taken from GBATEK

// Perform a soft reset. Not supported on the SDL2 port.
EXPORT_API NORETURN void SWI_SoftReset(void);

// Wait until an interrupt happens.
EXPORT_API void SWI_Halt(void);

// Wait until the VBlank interrupt happens.
EXPORT_API void SWI_VBlankIntrWait(void);

// For SWI_CpuSet() and SWI_CpuFastSet()
#define SWI_MODE_COPY   (0 << 24)
#define SWI_MODE_FILL   (1 << 24)
// For SWI_CpuSet() only
#define SWI_MODE_16BIT  (0 << 26)
#define SWI_MODE_32BIT  (1 << 26)

// Source and destination must be aligned to 4 bytes for 32-bit copies, or 2
// bytes for 16-bit copies. The length is expressed either in 32-bit or 16-bit
// chunks (words or halfwords).
EXPORT_API void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode);

// Source and destination must be aligned to 4 bytes. The length must be a
// multiple of 8 bytes.
EXPORT_API void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode);

// Calculate square root.
EXPORT_API uint16_t SWI_Sqrt(uint32_t value);

// Calculate arc tangent
EXPORT_API int16_t SWI_ArcTan(int16_t tan);

// Calculate arc tangent 2
EXPORT_API int16_t SWI_ArcTan2(int16_t x, int16_t y);

// Known values that SWI_GetBiosChecksum() can return
#define SWI_CHECKSUM_GBA    (0xBAAE187F) // GBA, GBA SP, GB Micro
#define SWI_CHECKSUM_NDS    (0xBAAE1880) // NDS, 3DS in NDS mode

// Returns checksum of the BIOS
EXPORT_API uint32_t SWI_GetBiosChecksum(void);

// Struct that holds the input to SWI_BgAffineSet()
#pragma pack(push, 1)
typedef struct {
    int32_t bgx;        // 24.8 fixed point
    int32_t bgy;        // 24.8 fixed point
    int16_t scrx;
    int16_t scry;
    int16_t scalex;     // 8.8 fixed point
    int16_t scaley;     // 8.8 fixed point
    uint16_t angle;     // 8.8 fixed point. Range: 0 - 0xFFFF
    uint16_t padding;
} bg_affine_src;
#pragma pack(pop)

// Struct that holds the state of a background affine transformation. It is used
// as container of the output of SWI_BgAffineSet()
#pragma pack(push, 1)
typedef struct {
    int16_t pa;
    int16_t pb;
    int16_t pc;
    int16_t pd;
    int32_t xoff;
    int32_t yoff;
} bg_affine_dst;
#pragma pack(pop)

// This function gets a list of background transformations and outputs the
// correct affine matrices for the GBA hardware.
EXPORT_API
void SWI_BgAffineSet(const bg_affine_src *src, bg_affine_dst *dst,
                     uint32_t count);

// Struct that holds the input to SWI_ObjAffineSet()
#pragma pack(push, 1)
typedef struct {
    int16_t sx;         // 8.8 fixed point
    int16_t sy;         // 8.8 fixed point
    uint16_t angle;     // 8.8 fixed point. Range: 0 - 0xFFFF
    uint16_t padding;
} obj_affine_src;
#pragma pack(pop)

// This function gets a list of objects transformations and outputs the correct
// affine matrices for the GBA hardware.
EXPORT_API
void SWI_ObjAffineSet(const obj_affine_src *src, void *dst,
                      uint32_t count, uint32_t increment);

// Decompresses LZ77 data from the source and writes the result to the
// destination using 8-bit writes. It can't be used to decompress directly to
// VRAM, as it only accepts 16 and 32-bit accesses.
EXPORT_API
void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest);

// Decompresses LZ77 data from the source and writes the result to the
// destination using 16-bit writes. VRAM can be used as destination.
EXPORT_API
void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest);

// Decompresses Huffman-encoded data from the source and writes the result to
// the destination using 32-bit writes. VRAM can be used as destination.
EXPORT_API void SWI_HuffUnComp(const void *source, void *dest);

// Decompresses Run-Length data from the source and writes the result to the
// destination using 8-bit writes. It can't be used to decompress directly to
// VRAM, as it only accepts 16 and 32-bit accesses.
EXPORT_API void SWI_RLUnCompWram(const void *source, void *dest);

// Decompresses Run-Length data from the source and writes the result to the
// destination using 16-bit writes. VRAM can be used as destination.
EXPORT_API void SWI_RLUnCompVram(const void *source, void *dest);

// Convert data in diff format to original data. In this version, data elements
// are 8 bits wide and the result is writen in 8-bit accesses.
EXPORT_API void SWI_Diff8bitUnFilterWram(const void *source, void *dest);

// Convert data in diff format to original data. In this version, data elements
// are 8 bits wide and the result is writen in 16-bit accesses. This means that
// VRAM can be used as destination, as it doesn't accept 8-bit writes.
EXPORT_API void SWI_Diff8bitUnFilterVram(const void *source, void *dest);

// Convert data in diff format to original data. In this version, data elements
// are 16 bits wide and the result is writen in 16-bit accesses.
EXPORT_API void SWI_Diff16bitUnFilter(const void *source, void *dest);

// Perform a hard reset. Not supported on the SDL2 port.
EXPORT_API NORETURN void SWI_HardReset(void);

#endif // BIOS_H__
