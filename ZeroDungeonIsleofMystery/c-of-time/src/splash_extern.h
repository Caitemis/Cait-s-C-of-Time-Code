#pragma once

#include <pmdsky.h>
#include <cot.h>

struct bgp1c {
    undefined1 kind;
    undefined1 unk_0x1;
    undefined field_0x2;
    undefined field_0x3;
    undefined4 unk_0x4;
    undefined4 unk_0x8;
    undefined4 unk_0xc;
    undefined4 unk_0x10;
    uint32_t tile_table_len;
    undefined2* tile_table;
};
ASSERT_SIZE(struct bgp1c, 0x1C);

struct bgp18 {
    uint32_t subregion_id;
    uint32_t screen_id;
    struct bgp1c* bgp1c_ptr;
    struct palette_data* palette_data_ptr;
    int unk_bitfield_0x10;
    undefined field_0x14;
    undefined field_0x15;
    undefined1 unk_0x16;
    undefined field_0x17; 
};
ASSERT_SIZE(struct bgp18, 0x18);

struct bgp_region_area {
    undefined4 size;
    undefined4 addr_decomp;
    undefined4 unk_bitfield;
    undefined4 maybe_kind;
    struct palette_data* palette;
};
ASSERT_SIZE(struct bgp_region_area, 0x14);

struct bgp_region_area_manager {
    struct bgp_region_area region_areas[16];
    uint16_t num_active_areas;
    undefined2 padding_0x142;
};
ASSERT_SIZE(struct bgp_region_area_manager, 0x144);

struct bgp21c {
    struct bgp18 bgp18_table[2][2];
    struct bgp_region_area_manager area_manager;
    struct palette_data_substruct palette_subdata[2];
    struct palette_data palette_data[2];
};
ASSERT_SIZE(struct bgp21c, 0x21C);

extern struct bgp21c* BGP21C_PTR;

extern void LoadBackgroundBgp(char * filepath, uint32_t param_2, int vertical_trim, int transparency_threshold, int subregion, int screen);
