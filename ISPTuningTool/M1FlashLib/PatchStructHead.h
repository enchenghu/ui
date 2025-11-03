#ifndef PATCH_STRUCT_HEAD_H
#define PATCH_STRUCT_HEAD_H

#include <stdint.h>

typedef struct _function_Header{
    uint16_t func_table;
    uint16_t func_idx;
    uint32_t func_ptr;
}Function_Header;

typedef struct _patch_Header{
    uint32_t magic_number;
    uint32_t version;
    uint16_t isp_patch_table_off;
    uint16_t isp_patch_table_len;
    uint16_t isp_patch_table_crc;
    uint16_t isp_patch_off;
    uint16_t isp_patch_len;
    uint16_t isp_patch_crc;
    uint16_t boot_patch_off;
    uint16_t boot_patch_len;
    uint16_t boot_patch_crc;
    uint16_t boot_patch_func_nums;
}Patch_Header;

#endif