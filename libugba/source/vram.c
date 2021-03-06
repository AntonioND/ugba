// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

void VRAM_OBJTiles16Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 4 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (4 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_OBJ + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_OBJTiles256Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 8 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (8 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_OBJ + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_OBJPalette16Copy(const void *src, size_t size, uint32_t pal_index)
{
    uint16_t *dst = MEM_PALETTE_OBJ + (pal_index * 16);
    SWI_CpuSet_Copy16(src, dst, size);
}

void VRAM_OBJPalette256Copy(const void *src, size_t size)
{
    SWI_CpuSet_Copy16(src, MEM_PALETTE_OBJ, size);
}

void VRAM_BGTiles16Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 4 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (4 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_BG + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_BGTiles256Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 8 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (8 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_BG + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_BGPalette16Copy(const void *src, size_t size, uint32_t pal_index)
{
    uint16_t *dst = MEM_PALETTE_BG + (pal_index * 16);
    SWI_CpuSet_Copy16(src, dst, size);
}

void VRAM_BGPalette256Copy(const void *src, size_t size)
{
    SWI_CpuSet_Copy16(src, MEM_PALETTE_BG, size);
}
