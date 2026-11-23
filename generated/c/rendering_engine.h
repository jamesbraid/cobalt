/* generated from devices/rendering-engine/registers.toml by defs-gen - do not edit, change the source */
/* SPDX-License-Identifier: BSD-3-Clause */
/* defs-gen 2 */
/* input sha256 40f341e6f3e5ecb9130c295b7acf23a6bcfe5c990967fc3b5f7bc2ac1002cb50 */
/* Rendering engine register window */
#ifndef RENG_GEN_H
#define RENG_GEN_H

#include <stdint.h>


/*
 * Translation table: one entry per tile of the buffers the engine addresses. A tile is 1, 4, 16, 64 or 256 pages of 4 KB, as the buffer's descriptor says; the packet pool is 16 tiles of 256 pages, so it fills 16 entries. Only the low dword is ever written, with the tile's first 4 KB page frame number masked to 24 bits; entries not in use hold the first entry's value. The packet pool, the display buffers and the textures are named through these entries, so the engine fetches packets through this table rather than by physical address.
 */
/* tlb_entry: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets */
#define RENG_TLB_ENTRY_OFFSET                        0x8000u /* inferred */
#define RENG_TLB_ENTRY_COUNT                         2048
#define RENG_TLB_ENTRY_STRIDE                        8
#define RENG_TLB_ENTRY_PAGE_FRAME_SHIFT              0 /* inferred */
#define RENG_TLB_ENTRY_PAGE_FRAME_MASK               0xffffffu
#define RENG_TLB_ENTRY_PAGE_FRAME_INPLACE            (RENG_TLB_ENTRY_PAGE_FRAME_MASK << RENG_TLB_ENTRY_PAGE_FRAME_SHIFT)

/*
 * Buffer descriptor table: entry k describes the buffer whose descriptor id is k, the six-bit id the colour-format, depth-format and texture-format words carry. Entry 0 is the packet pool. The low dword only is written. A tiled buffer is a run of tiles of 128 or 512 bytes in a row, with row_tiles tiles per row, whose pages start at the translation-table slot named by slot; a user buffer mapped linearly sets bit 19 (row_tiles 32) with the tile_pages_log4 bits kept, and the packet pool sets pool_flag and row_tiles 1.
 */
/* buffer_desc: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers */
#define RENG_BUFFER_DESC_OFFSET                      0xc000u /* inferred */
#define RENG_BUFFER_DESC_COUNT                       64
#define RENG_BUFFER_DESC_STRIDE                      8
#define RENG_BUFFER_DESC_SLOT_SHIFT                  21 /* inferred */
#define RENG_BUFFER_DESC_SLOT_MASK                   0x7ffu
#define RENG_BUFFER_DESC_SLOT_INPLACE                (RENG_BUFFER_DESC_SLOT_MASK << RENG_BUFFER_DESC_SLOT_SHIFT)
#define RENG_BUFFER_DESC_ROW_TILES_SHIFT             14 /* inferred */
#define RENG_BUFFER_DESC_ROW_TILES_MASK              0x7fu
#define RENG_BUFFER_DESC_ROW_TILES_INPLACE           (RENG_BUFFER_DESC_ROW_TILES_MASK << RENG_BUFFER_DESC_ROW_TILES_SHIFT)
#define RENG_BUFFER_DESC_TILE_SIZE_SEL_SHIFT         13 /* inferred */
#define RENG_BUFFER_DESC_TILE_SIZE_SEL_MASK          0x1u
#define RENG_BUFFER_DESC_TILE_SIZE_SEL_INPLACE       (RENG_BUFFER_DESC_TILE_SIZE_SEL_MASK << RENG_BUFFER_DESC_TILE_SIZE_SEL_SHIFT)
#define RENG_BUFFER_DESC_TILE_PAGES_LOG4_SHIFT       10 /* inferred */
#define RENG_BUFFER_DESC_TILE_PAGES_LOG4_MASK        0x7u
#define RENG_BUFFER_DESC_TILE_PAGES_LOG4_INPLACE     (RENG_BUFFER_DESC_TILE_PAGES_LOG4_MASK << RENG_BUFFER_DESC_TILE_PAGES_LOG4_SHIFT)
#define RENG_BUFFER_DESC_POOL_FLAG_SHIFT             9 /* inferred */
#define RENG_BUFFER_DESC_POOL_FLAG_MASK              0x1u
#define RENG_BUFFER_DESC_POOL_FLAG_INPLACE           (RENG_BUFFER_DESC_POOL_FLAG_MASK << RENG_BUFFER_DESC_POOL_FLAG_SHIFT)

/*
 * Starts the engine on a packet position and reports where it is. A write is the packet position (in 32-byte units from the pool base) shifted to bit 12, bit 11 and the idle count; the driver writes 2 as the idle count with every start and 0x40 with 0xfffff in the position field at start-up. A read returns the position in bits 31:12, and zero there means none, in which case the driver reads the chain register instead.
 */
/* exec_ctl: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines */
#define RENG_EXEC_CTL_OFFSET                         0x10000u /* inferred */
#define RENG_EXEC_CTL_PKT_POS_SHIFT                  12 /* inferred */
#define RENG_EXEC_CTL_PKT_POS_MASK                   0xfffffu
#define RENG_EXEC_CTL_PKT_POS_INPLACE                (RENG_EXEC_CTL_PKT_POS_MASK << RENG_EXEC_CTL_PKT_POS_SHIFT)
#define RENG_EXEC_CTL_START_SHIFT                    11 /* inferred */
#define RENG_EXEC_CTL_START_MASK                     0x1u
#define RENG_EXEC_CTL_START_INPLACE                  (RENG_EXEC_CTL_START_MASK << RENG_EXEC_CTL_START_SHIFT)
#define RENG_EXEC_CTL_IDLE_WAIT_SHIFT                0 /* inferred */
#define RENG_EXEC_CTL_IDLE_WAIT_MASK                 0x7ffu
#define RENG_EXEC_CTL_IDLE_WAIT_INPLACE              (RENG_EXEC_CTL_IDLE_WAIT_MASK << RENG_EXEC_CTL_IDLE_WAIT_SHIFT)

/*
 * Written with a single bit from three routines. Two write it when work is appended to a non-deferred node or a timer event fires, with the current queue at 2, a node in the hardware and that node's chunk count 0. The third, which suspends a node, writes it when the node's status bits 3:1 are clear, its chunk count is below 1, status bit 9 is clear and the status byte above bit 7 is non-zero.
 */
/* doorbell: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routines that queue work or resume a suspended context */
#define RENG_DOORBELL_OFFSET                         0x10008u /* inferred */
#define RENG_DOORBELL_KICK_SHIFT                     10 /* inferred */
#define RENG_DOORBELL_KICK_MASK                      0x1u
#define RENG_DOORBELL_KICK_INPLACE                   (RENG_DOORBELL_KICK_MASK << RENG_DOORBELL_KICK_SHIFT)

/*
 * The interrupt handler treats a read of zero as not its own, masks the value with the two cause bits, writes zero and waits for the masked bits to change. The context switch waits for bit 31 to read set and then writes every bit but 31 set.
 */
/* int_cause: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines */
#define RENG_INT_CAUSE_OFFSET                        0x10018u /* inferred */
#define RENG_INT_CAUSE_CTX_WAIT_SHIFT                31 /* inferred */
#define RENG_INT_CAUSE_CTX_WAIT_MASK                 0x1u
#define RENG_INT_CAUSE_CTX_WAIT_INPLACE              (RENG_INT_CAUSE_CTX_WAIT_MASK << RENG_INT_CAUSE_CTX_WAIT_SHIFT)
#define RENG_INT_CAUSE_CAUSE_30_SHIFT                30 /* inferred */
#define RENG_INT_CAUSE_CAUSE_30_MASK                 0x1u
#define RENG_INT_CAUSE_CAUSE_30_INPLACE              (RENG_INT_CAUSE_CAUSE_30_MASK << RENG_INT_CAUSE_CAUSE_30_SHIFT)
#define RENG_INT_CAUSE_CAUSE_29_SHIFT                29 /* inferred */
#define RENG_INT_CAUSE_CAUSE_29_MASK                 0x1u
#define RENG_INT_CAUSE_CAUSE_29_INPLACE              (RENG_INT_CAUSE_CAUSE_29_MASK << RENG_INT_CAUSE_CAUSE_29_SHIFT)

/*
 * Written once, with both cause bits set.
 */
/* int_enable: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up */
#define RENG_INT_ENABLE_OFFSET                       0x10020u /* inferred */
#define RENG_INT_ENABLE_EN_30_SHIFT                  30 /* inferred */
#define RENG_INT_ENABLE_EN_30_MASK                   0x1u
#define RENG_INT_ENABLE_EN_30_INPLACE                (RENG_INT_ENABLE_EN_30_MASK << RENG_INT_ENABLE_EN_30_SHIFT)
#define RENG_INT_ENABLE_EN_29_SHIFT                  29 /* inferred */
#define RENG_INT_ENABLE_EN_29_MASK                   0x1u
#define RENG_INT_ENABLE_EN_29_INPLACE                (RENG_INT_ENABLE_EN_29_MASK << RENG_INT_ENABLE_EN_29_SHIFT)

/*
 * Scissor minimum corner, read into the window-parameter packet when the node already in the hardware is restarted. The window offset follows from the scissor packet's address field.
 */
/* scissor_min: r32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet */
#define RENG_SCISSOR_MIN_OFFSET                      0x18138u /* inferred */
#define RENG_SCISSOR_MIN_Y_SHIFT                     16 /* inferred */
#define RENG_SCISSOR_MIN_Y_MASK                      0xffffu
#define RENG_SCISSOR_MIN_Y_INPLACE                   (RENG_SCISSOR_MIN_Y_MASK << RENG_SCISSOR_MIN_Y_SHIFT)
#define RENG_SCISSOR_MIN_X_SHIFT                     0 /* inferred */
#define RENG_SCISSOR_MIN_X_MASK                      0xffffu
#define RENG_SCISSOR_MIN_X_INPLACE                   (RENG_SCISSOR_MIN_X_MASK << RENG_SCISSOR_MIN_X_SHIFT)

/*
 * Scissor maximum corner, inclusive.
 */
/* scissor_max: r32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet */
#define RENG_SCISSOR_MAX_OFFSET                      0x18140u /* inferred */
#define RENG_SCISSOR_MAX_Y_SHIFT                     16 /* inferred */
#define RENG_SCISSOR_MAX_Y_MASK                      0xffffu
#define RENG_SCISSOR_MAX_Y_INPLACE                   (RENG_SCISSOR_MAX_Y_MASK << RENG_SCISSOR_MAX_Y_SHIFT)
#define RENG_SCISSOR_MAX_X_SHIFT                     0 /* inferred */
#define RENG_SCISSOR_MAX_X_MASK                      0xffffu
#define RENG_SCISSOR_MAX_X_INPLACE                   (RENG_SCISSOR_MAX_X_MASK << RENG_SCISSOR_MAX_X_SHIFT)

/*
 * The first word of the chain packet. The stop-event routine reads it: the low 12 bits are the high half of a 24-bit sequence number and bits 31:12 a node position, which it falls back to when the execution register reads zero.
 */
/* chain_next: r32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, stop-event routine, and the chain packet the OpenGL driver ends a node with */
#define RENG_CHAIN_NEXT_OFFSET                       0x185c0u /* inferred */
#define RENG_CHAIN_NEXT_NODE_POS_SHIFT               12 /* inferred */
#define RENG_CHAIN_NEXT_NODE_POS_MASK                0xfffffu
#define RENG_CHAIN_NEXT_NODE_POS_INPLACE             (RENG_CHAIN_NEXT_NODE_POS_MASK << RENG_CHAIN_NEXT_NODE_POS_SHIFT)
#define RENG_CHAIN_NEXT_SEQ_HI_SHIFT                 0 /* inferred */
#define RENG_CHAIN_NEXT_SEQ_HI_MASK                  0xfffu
#define RENG_CHAIN_NEXT_SEQ_HI_INPLACE               (RENG_CHAIN_NEXT_SEQ_HI_MASK << RENG_CHAIN_NEXT_SEQ_HI_SHIFT)

/*
 * The second word of the chain packet. Bits 31:12 are the position of the node the engine stopped in and bits 11:0 the low half of the sequence number. The context switch writes zero to it when the incoming node is already the one in hardware.
 */
/* chain_this: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch and stop-event routines */
#define RENG_CHAIN_THIS_OFFSET                       0x185c8u /* inferred */
#define RENG_CHAIN_THIS_NODE_POS_SHIFT               12 /* inferred */
#define RENG_CHAIN_THIS_NODE_POS_MASK                0xfffffu
#define RENG_CHAIN_THIS_NODE_POS_INPLACE             (RENG_CHAIN_THIS_NODE_POS_MASK << RENG_CHAIN_THIS_NODE_POS_SHIFT)
#define RENG_CHAIN_THIS_SEQ_LO_SHIFT                 0 /* inferred */
#define RENG_CHAIN_THIS_SEQ_LO_MASK                  0xfffu
#define RENG_CHAIN_THIS_SEQ_LO_INPLACE               (RENG_CHAIN_THIS_SEQ_LO_MASK << RENG_CHAIN_THIS_SEQ_LO_SHIFT)

/*
 * The third word of the long chain packet. The kernel driver keeps a per-context current-texture id and writes it here when it starts the context and reads it back at a stop; the OpenGL driver zeroes it before it frees the pages of the bound texture.
 */
/* bound_texture_id: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch, stop-event and bind routines; the OpenGL driver's texture-free packet */
#define RENG_BOUND_TEXTURE_ID_OFFSET                 0x185d0u /* inferred */

/*
 * Written with the outgoing context's dump offset (a per-context field the driver keeps for the purpose) when another context is started, before the new context's start word.
 */
/* ctx_dump: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch */
#define RENG_CTX_DUMP_OFFSET                         0x18800u /* inferred */

/*
 * First word of the alpha-test packet; the second is the reference colour.
 */
/* word ALPHA_TEST_CONFIG: 32 bits; SGI Cobalt Graphics Driver 5.2.4 alpha-test writer */
#define RENG_ALPHA_TEST_CONFIG_WIDTH                 32 /* inferred */
#define RENG_ALPHA_TEST_CONFIG_COMPONENT_ENABLES_SHIFT 28 /* inferred */
#define RENG_ALPHA_TEST_CONFIG_COMPONENT_ENABLES_MASK 0xfu
#define RENG_ALPHA_TEST_CONFIG_COMPONENT_ENABLES_INPLACE (RENG_ALPHA_TEST_CONFIG_COMPONENT_ENABLES_MASK << RENG_ALPHA_TEST_CONFIG_COMPONENT_ENABLES_SHIFT)
#define RENG_ALPHA_TEST_CONFIG_FUNC_SHIFT            25 /* inferred */
#define RENG_ALPHA_TEST_CONFIG_FUNC_MASK             0x7u
#define RENG_ALPHA_TEST_CONFIG_FUNC_INPLACE          (RENG_ALPHA_TEST_CONFIG_FUNC_MASK << RENG_ALPHA_TEST_CONFIG_FUNC_SHIFT)

/*
 * First word of the blend packet; the second is the constant colour.
 */
/* word BLEND_CONFIG: 32 bits; SGI Cobalt Graphics Driver 5.2.4 blend writer */
#define RENG_BLEND_CONFIG_WIDTH                      32 /* inferred */
#define RENG_BLEND_CONFIG_DST_FACTOR_SHIFT           28 /* inferred */
#define RENG_BLEND_CONFIG_DST_FACTOR_MASK            0xfu
#define RENG_BLEND_CONFIG_DST_FACTOR_INPLACE         (RENG_BLEND_CONFIG_DST_FACTOR_MASK << RENG_BLEND_CONFIG_DST_FACTOR_SHIFT)
#define RENG_BLEND_CONFIG_SRC_FACTOR_SHIFT           24 /* inferred */
#define RENG_BLEND_CONFIG_SRC_FACTOR_MASK            0xfu
#define RENG_BLEND_CONFIG_SRC_FACTOR_INPLACE         (RENG_BLEND_CONFIG_SRC_FACTOR_MASK << RENG_BLEND_CONFIG_SRC_FACTOR_SHIFT)
#define RENG_BLEND_CONFIG_OP_SHIFT                   20 /* inferred */
#define RENG_BLEND_CONFIG_OP_MASK                    0xfu
#define RENG_BLEND_CONFIG_OP_INPLACE                 (RENG_BLEND_CONFIG_OP_MASK << RENG_BLEND_CONFIG_OP_SHIFT)

/*
 * Draw-buffer descriptor.
 */
/* word COLOR_BUFFER_DESC: 32 bits; SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers */
#define RENG_COLOR_BUFFER_DESC_WIDTH                 32 /* inferred */
#define RENG_COLOR_BUFFER_DESC_LINEAR_SHIFT          31 /* inferred */
#define RENG_COLOR_BUFFER_DESC_LINEAR_MASK           0x1u
#define RENG_COLOR_BUFFER_DESC_LINEAR_INPLACE        (RENG_COLOR_BUFFER_DESC_LINEAR_MASK << RENG_COLOR_BUFFER_DESC_LINEAR_SHIFT)
#define RENG_COLOR_BUFFER_DESC_PIXEL_KIND_SHIFT      27 /* inferred */
#define RENG_COLOR_BUFFER_DESC_PIXEL_KIND_MASK       0xfu
#define RENG_COLOR_BUFFER_DESC_PIXEL_KIND_INPLACE    (RENG_COLOR_BUFFER_DESC_PIXEL_KIND_MASK << RENG_COLOR_BUFFER_DESC_PIXEL_KIND_SHIFT)
#define RENG_COLOR_BUFFER_DESC_SIZE_CLASS_SHIFT      25 /* inferred */
#define RENG_COLOR_BUFFER_DESC_SIZE_CLASS_MASK       0x3u
#define RENG_COLOR_BUFFER_DESC_SIZE_CLASS_INPLACE    (RENG_COLOR_BUFFER_DESC_SIZE_CLASS_MASK << RENG_COLOR_BUFFER_DESC_SIZE_CLASS_SHIFT)
#define RENG_COLOR_BUFFER_DESC_BUFFER_ID_SHIFT       19 /* inferred */
#define RENG_COLOR_BUFFER_DESC_BUFFER_ID_MASK        0x3fu
#define RENG_COLOR_BUFFER_DESC_BUFFER_ID_INPLACE     (RENG_COLOR_BUFFER_DESC_BUFFER_ID_MASK << RENG_COLOR_BUFFER_DESC_BUFFER_ID_SHIFT)
#define RENG_COLOR_BUFFER_DESC_SPAN_FLAG_SHIFT       18 /* inferred */
#define RENG_COLOR_BUFFER_DESC_SPAN_FLAG_MASK        0x1u
#define RENG_COLOR_BUFFER_DESC_SPAN_FLAG_INPLACE     (RENG_COLOR_BUFFER_DESC_SPAN_FLAG_MASK << RENG_COLOR_BUFFER_DESC_SPAN_FLAG_SHIFT)

/*
 * A corner: x in the low half, y in the high half (signed 16-bit).
 */
/* word CORNER_XY: 32 bits; SGI Cobalt Graphics Driver 5.2.4 scissor and clear-rectangle writers */
#define RENG_CORNER_XY_WIDTH                         32 /* inferred */
#define RENG_CORNER_XY_Y_SHIFT                       16 /* inferred */
#define RENG_CORNER_XY_Y_MASK                        0xffffu
#define RENG_CORNER_XY_Y_INPLACE                     (RENG_CORNER_XY_Y_MASK << RENG_CORNER_XY_Y_SHIFT)
#define RENG_CORNER_XY_X_SHIFT                       0 /* inferred */
#define RENG_CORNER_XY_X_MASK                        0xffffu
#define RENG_CORNER_XY_X_INPLACE                     (RENG_CORNER_XY_X_MASK << RENG_CORNER_XY_X_SHIFT)

/*
 * Depth test word.
 */
/* word DEPTH_COMPARE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 depth-test writers */
#define RENG_DEPTH_COMPARE_WIDTH                     32 /* inferred */
#define RENG_DEPTH_COMPARE_FUNC_SHIFT                29 /* inferred */
#define RENG_DEPTH_COMPARE_FUNC_MASK                 0x7u
#define RENG_DEPTH_COMPARE_FUNC_INPLACE              (RENG_DEPTH_COMPARE_FUNC_MASK << RENG_DEPTH_COMPARE_FUNC_SHIFT)
#define RENG_DEPTH_COMPARE_OCCLUSION_RESET_SHIFT     28 /* inferred */
#define RENG_DEPTH_COMPARE_OCCLUSION_RESET_MASK      0x1u
#define RENG_DEPTH_COMPARE_OCCLUSION_RESET_INPLACE   (RENG_DEPTH_COMPARE_OCCLUSION_RESET_MASK << RENG_DEPTH_COMPARE_OCCLUSION_RESET_SHIFT)
#define RENG_DEPTH_COMPARE_OCCLUSION_ON_SHIFT        27 /* inferred */
#define RENG_DEPTH_COMPARE_OCCLUSION_ON_MASK         0x1u
#define RENG_DEPTH_COMPARE_OCCLUSION_ON_INPLACE      (RENG_DEPTH_COMPARE_OCCLUSION_ON_MASK << RENG_DEPTH_COMPARE_OCCLUSION_ON_SHIFT)
#define RENG_DEPTH_COMPARE_EQUAL_WINDOW_SHIFT        23 /* inferred */
#define RENG_DEPTH_COMPARE_EQUAL_WINDOW_MASK         0xfu
#define RENG_DEPTH_COMPARE_EQUAL_WINDOW_INPLACE      (RENG_DEPTH_COMPARE_EQUAL_WINDOW_MASK << RENG_DEPTH_COMPARE_EQUAL_WINDOW_SHIFT)

/*
 * Depth and stencil buffer descriptor.
 */
/* word DEPTH_STENCIL_BUFFER_DESC: 32 bits; SGI Cobalt Graphics Driver 5.2.4 depth and stencil descriptor writers */
#define RENG_DEPTH_STENCIL_BUFFER_DESC_WIDTH         32 /* inferred */
#define RENG_DEPTH_STENCIL_BUFFER_DESC_KIND_FLAG_SHIFT 31 /* inferred */
#define RENG_DEPTH_STENCIL_BUFFER_DESC_KIND_FLAG_MASK 0x1u
#define RENG_DEPTH_STENCIL_BUFFER_DESC_KIND_FLAG_INPLACE (RENG_DEPTH_STENCIL_BUFFER_DESC_KIND_FLAG_MASK << RENG_DEPTH_STENCIL_BUFFER_DESC_KIND_FLAG_SHIFT)
#define RENG_DEPTH_STENCIL_BUFFER_DESC_SIZE_CLASS_SHIFT 29 /* inferred */
#define RENG_DEPTH_STENCIL_BUFFER_DESC_SIZE_CLASS_MASK 0x3u
#define RENG_DEPTH_STENCIL_BUFFER_DESC_SIZE_CLASS_INPLACE (RENG_DEPTH_STENCIL_BUFFER_DESC_SIZE_CLASS_MASK << RENG_DEPTH_STENCIL_BUFFER_DESC_SIZE_CLASS_SHIFT)
#define RENG_DEPTH_STENCIL_BUFFER_DESC_BUFFER_ID_SHIFT 23 /* inferred */
#define RENG_DEPTH_STENCIL_BUFFER_DESC_BUFFER_ID_MASK 0x3fu
#define RENG_DEPTH_STENCIL_BUFFER_DESC_BUFFER_ID_INPLACE (RENG_DEPTH_STENCIL_BUFFER_DESC_BUFFER_ID_MASK << RENG_DEPTH_STENCIL_BUFFER_DESC_BUFFER_ID_SHIFT)

/*
 * Lighting mode word.
 */
/* word LIGHTING_FLAGS: 32 bits; SGI Cobalt Graphics Driver 5.2.4 lighting-path writers */
#define RENG_LIGHTING_FLAGS_WIDTH                    32 /* inferred */
#define RENG_LIGHTING_FLAGS_TWO_SIDED_SHIFT          31 /* inferred */
#define RENG_LIGHTING_FLAGS_TWO_SIDED_MASK           0x1u
#define RENG_LIGHTING_FLAGS_TWO_SIDED_INPLACE        (RENG_LIGHTING_FLAGS_TWO_SIDED_MASK << RENG_LIGHTING_FLAGS_TWO_SIDED_SHIFT)
#define RENG_LIGHTING_FLAGS_LIGHT3_ON_SHIFT          30 /* inferred */
#define RENG_LIGHTING_FLAGS_LIGHT3_ON_MASK           0x1u
#define RENG_LIGHTING_FLAGS_LIGHT3_ON_INPLACE        (RENG_LIGHTING_FLAGS_LIGHT3_ON_MASK << RENG_LIGHTING_FLAGS_LIGHT3_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_LIGHT2_ON_SHIFT          29 /* inferred */
#define RENG_LIGHTING_FLAGS_LIGHT2_ON_MASK           0x1u
#define RENG_LIGHTING_FLAGS_LIGHT2_ON_INPLACE        (RENG_LIGHTING_FLAGS_LIGHT2_ON_MASK << RENG_LIGHTING_FLAGS_LIGHT2_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_LIGHT1_ON_SHIFT          28 /* inferred */
#define RENG_LIGHTING_FLAGS_LIGHT1_ON_MASK           0x1u
#define RENG_LIGHTING_FLAGS_LIGHT1_ON_INPLACE        (RENG_LIGHTING_FLAGS_LIGHT1_ON_MASK << RENG_LIGHTING_FLAGS_LIGHT1_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_LIGHT0_ON_SHIFT          27 /* inferred */
#define RENG_LIGHTING_FLAGS_LIGHT0_ON_MASK           0x1u
#define RENG_LIGHTING_FLAGS_LIGHT0_ON_INPLACE        (RENG_LIGHTING_FLAGS_LIGHT0_ON_MASK << RENG_LIGHTING_FLAGS_LIGHT0_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_NORMALIZE_ON_SHIFT       26 /* inferred */
#define RENG_LIGHTING_FLAGS_NORMALIZE_ON_MASK        0x1u
#define RENG_LIGHTING_FLAGS_NORMALIZE_ON_INPLACE     (RENG_LIGHTING_FLAGS_NORMALIZE_ON_MASK << RENG_LIGHTING_FLAGS_NORMALIZE_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_SPECULAR_ON_SHIFT        25 /* inferred */
#define RENG_LIGHTING_FLAGS_SPECULAR_ON_MASK         0x1u
#define RENG_LIGHTING_FLAGS_SPECULAR_ON_INPLACE      (RENG_LIGHTING_FLAGS_SPECULAR_ON_MASK << RENG_LIGHTING_FLAGS_SPECULAR_ON_SHIFT)
#define RENG_LIGHTING_FLAGS_SPECULAR_ONLY_SHIFT      24 /* inferred */
#define RENG_LIGHTING_FLAGS_SPECULAR_ONLY_MASK       0x1u
#define RENG_LIGHTING_FLAGS_SPECULAR_ONLY_INPLACE    (RENG_LIGHTING_FLAGS_SPECULAR_ONLY_MASK << RENG_LIGHTING_FLAGS_SPECULAR_ONLY_SHIFT)
#define RENG_LIGHTING_FLAGS_LIT_ON_SHIFT             23 /* inferred */
#define RENG_LIGHTING_FLAGS_LIT_ON_MASK              0x1u
#define RENG_LIGHTING_FLAGS_LIT_ON_INPLACE           (RENG_LIGHTING_FLAGS_LIT_ON_MASK << RENG_LIGHTING_FLAGS_LIT_ON_SHIFT)

/*
 * Link word of a packet node. It sits in node dword 0 and the drivers write it through the second CPU window (base 0xc0000000) plus the node offset. Bit 0 set means the engine owns the node and clears it when done. The size field counts 32-byte units of data after the 32-byte header unit.
 */
/* word NODE_LINK: 32 bits; SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers */
#define RENG_NODE_LINK_WIDTH                         32 /* inferred */
#define RENG_NODE_LINK_NEXT_SHIFT                    12 /* inferred */
#define RENG_NODE_LINK_NEXT_MASK                     0xfffffu
#define RENG_NODE_LINK_NEXT_INPLACE                  (RENG_NODE_LINK_NEXT_MASK << RENG_NODE_LINK_NEXT_SHIFT)
#define RENG_NODE_LINK_SIZE_SHIFT                    3 /* inferred */
#define RENG_NODE_LINK_SIZE_MASK                     0x1ffu
#define RENG_NODE_LINK_SIZE_INPLACE                  (RENG_NODE_LINK_SIZE_MASK << RENG_NODE_LINK_SIZE_SHIFT)
#define RENG_NODE_LINK_SYNC_SHIFT                    2 /* inferred */
#define RENG_NODE_LINK_SYNC_MASK                     0x1u
#define RENG_NODE_LINK_SYNC_INPLACE                  (RENG_NODE_LINK_SYNC_MASK << RENG_NODE_LINK_SYNC_SHIFT)
#define RENG_NODE_LINK_INTERRUPT_SHIFT               1 /* inferred */
#define RENG_NODE_LINK_INTERRUPT_MASK                0x1u
#define RENG_NODE_LINK_INTERRUPT_INPLACE             (RENG_NODE_LINK_INTERRUPT_MASK << RENG_NODE_LINK_INTERRUPT_SHIFT)
#define RENG_NODE_LINK_READY_SHIFT                   0 /* inferred */
#define RENG_NODE_LINK_READY_MASK                    0x1u
#define RENG_NODE_LINK_READY_INPLACE                 (RENG_NODE_LINK_READY_MASK << RENG_NODE_LINK_READY_SHIFT)

/*
 * Pixel-pipe mode word.
 */
/* word PIXEL_PIPE_FLAGS: 32 bits; SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver */
#define RENG_PIXEL_PIPE_FLAGS_WIDTH                  32 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_STENCIL_TEST_ON_SHIFT  31 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_STENCIL_TEST_ON_MASK   0x1u
#define RENG_PIXEL_PIPE_FLAGS_STENCIL_TEST_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_STENCIL_TEST_ON_MASK << RENG_PIXEL_PIPE_FLAGS_STENCIL_TEST_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_WRITE_ON_SHIFT   30 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_WRITE_ON_MASK    0x1u
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_WRITE_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_DEPTH_WRITE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_DEPTH_WRITE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_TEST_ON_SHIFT    29 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_TEST_ON_MASK     0x1u
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_TEST_ON_INPLACE  (RENG_PIXEL_PIPE_FLAGS_DEPTH_TEST_ON_MASK << RENG_PIXEL_PIPE_FLAGS_DEPTH_TEST_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_OFFSET_ON_SHIFT  28 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_OFFSET_ON_MASK   0x1u
#define RENG_PIXEL_PIPE_FLAGS_DEPTH_OFFSET_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_DEPTH_OFFSET_ON_MASK << RENG_PIXEL_PIPE_FLAGS_DEPTH_OFFSET_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_COLOR_DISCARD_ON_SHIFT 27 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_COLOR_DISCARD_ON_MASK  0x1u
#define RENG_PIXEL_PIPE_FLAGS_COLOR_DISCARD_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_COLOR_DISCARD_ON_MASK << RENG_PIXEL_PIPE_FLAGS_COLOR_DISCARD_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_COLOR_MASK_ON_SHIFT    26 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_COLOR_MASK_ON_MASK     0x1u
#define RENG_PIXEL_PIPE_FLAGS_COLOR_MASK_ON_INPLACE  (RENG_PIXEL_PIPE_FLAGS_COLOR_MASK_ON_MASK << RENG_PIXEL_PIPE_FLAGS_COLOR_MASK_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_DITHER_ON_SHIFT        25 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_DITHER_ON_MASK         0x1u
#define RENG_PIXEL_PIPE_FLAGS_DITHER_ON_INPLACE      (RENG_PIXEL_PIPE_FLAGS_DITHER_ON_MASK << RENG_PIXEL_PIPE_FLAGS_DITHER_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_LOGIC_OP_ON_SHIFT      24 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_LOGIC_OP_ON_MASK       0x1u
#define RENG_PIXEL_PIPE_FLAGS_LOGIC_OP_ON_INPLACE    (RENG_PIXEL_PIPE_FLAGS_LOGIC_OP_ON_MASK << RENG_PIXEL_PIPE_FLAGS_LOGIC_OP_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_BLEND_ON_SHIFT         23 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_BLEND_ON_MASK          0x1u
#define RENG_PIXEL_PIPE_FLAGS_BLEND_ON_INPLACE       (RENG_PIXEL_PIPE_FLAGS_BLEND_ON_MASK << RENG_PIXEL_PIPE_FLAGS_BLEND_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_ALPHA_TEST_ON_SHIFT    22 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_ALPHA_TEST_ON_MASK     0x1u
#define RENG_PIXEL_PIPE_FLAGS_ALPHA_TEST_ON_INPLACE  (RENG_PIXEL_PIPE_FLAGS_ALPHA_TEST_ON_MASK << RENG_PIXEL_PIPE_FLAGS_ALPHA_TEST_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_FOG_ON_SHIFT           21 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_FOG_ON_MASK            0x1u
#define RENG_PIXEL_PIPE_FLAGS_FOG_ON_INPLACE         (RENG_PIXEL_PIPE_FLAGS_FOG_ON_MASK << RENG_PIXEL_PIPE_FLAGS_FOG_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_MATRIX_CONVERT_ON_SHIFT 20 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_MATRIX_CONVERT_ON_MASK 0x1u
#define RENG_PIXEL_PIPE_FLAGS_MATRIX_CONVERT_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_MATRIX_CONVERT_ON_MASK << RENG_PIXEL_PIPE_FLAGS_MATRIX_CONVERT_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_PIXEL_TRANSFER_ON_SHIFT 19 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_PIXEL_TRANSFER_ON_MASK 0x1u
#define RENG_PIXEL_PIPE_FLAGS_PIXEL_TRANSFER_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_PIXEL_TRANSFER_ON_MASK << RENG_PIXEL_PIPE_FLAGS_PIXEL_TRANSFER_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_TEXTURE_ON_SHIFT       18 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_TEXTURE_ON_MASK        0x1u
#define RENG_PIXEL_PIPE_FLAGS_TEXTURE_ON_INPLACE     (RENG_PIXEL_PIPE_FLAGS_TEXTURE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_TEXTURE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_SECONDARY_COLOR_ON_SHIFT 17 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_SECONDARY_COLOR_ON_MASK 0x1u
#define RENG_PIXEL_PIPE_FLAGS_SECONDARY_COLOR_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_SECONDARY_COLOR_ON_MASK << RENG_PIXEL_PIPE_FLAGS_SECONDARY_COLOR_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_SMOOTH_SHADE_ON_SHIFT  16 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_SMOOTH_SHADE_ON_MASK   0x1u
#define RENG_PIXEL_PIPE_FLAGS_SMOOTH_SHADE_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_SMOOTH_SHADE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_SMOOTH_SHADE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_OPAQUE_STIPPLE_ON_SHIFT 15 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_OPAQUE_STIPPLE_ON_MASK 0x1u
#define RENG_PIXEL_PIPE_FLAGS_OPAQUE_STIPPLE_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_OPAQUE_STIPPLE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_OPAQUE_STIPPLE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_POLYGON_STIPPLE_ON_SHIFT 14 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_POLYGON_STIPPLE_ON_MASK 0x1u
#define RENG_PIXEL_PIPE_FLAGS_POLYGON_STIPPLE_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_POLYGON_STIPPLE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_POLYGON_STIPPLE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_LINE_STIPPLE_ON_SHIFT  13 /* inferred */
#define RENG_PIXEL_PIPE_FLAGS_LINE_STIPPLE_ON_MASK   0x1u
#define RENG_PIXEL_PIPE_FLAGS_LINE_STIPPLE_ON_INPLACE (RENG_PIXEL_PIPE_FLAGS_LINE_STIPPLE_ON_MASK << RENG_PIXEL_PIPE_FLAGS_LINE_STIPPLE_ON_SHIFT)
#define RENG_PIXEL_PIPE_FLAGS_RESERVED_SHIFT         0 /* unknown */
#define RENG_PIXEL_PIPE_FLAGS_RESERVED_MASK          0x1fffu
#define RENG_PIXEL_PIPE_FLAGS_RESERVED_INPLACE       (RENG_PIXEL_PIPE_FLAGS_RESERVED_MASK << RENG_PIXEL_PIPE_FLAGS_RESERVED_SHIFT)

/*
 * Primitive mode word.
 */
/* word PRIMITIVE_SETUP: 32 bits; SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver */
#define RENG_PRIMITIVE_SETUP_WIDTH                   32 /* inferred */
#define RENG_PRIMITIVE_SETUP_PRIMITIVE_KIND_SHIFT    29 /* inferred */
#define RENG_PRIMITIVE_SETUP_PRIMITIVE_KIND_MASK     0x7u
#define RENG_PRIMITIVE_SETUP_PRIMITIVE_KIND_INPLACE  (RENG_PRIMITIVE_SETUP_PRIMITIVE_KIND_MASK << RENG_PRIMITIVE_SETUP_PRIMITIVE_KIND_SHIFT)
#define RENG_PRIMITIVE_SETUP_LINE_ANTIALIAS_SHIFT    28 /* inferred */
#define RENG_PRIMITIVE_SETUP_LINE_ANTIALIAS_MASK     0x1u
#define RENG_PRIMITIVE_SETUP_LINE_ANTIALIAS_INPLACE  (RENG_PRIMITIVE_SETUP_LINE_ANTIALIAS_MASK << RENG_PRIMITIVE_SETUP_LINE_ANTIALIAS_SHIFT)
#define RENG_PRIMITIVE_SETUP_LINE_LOOP_SHIFT         27 /* inferred */
#define RENG_PRIMITIVE_SETUP_LINE_LOOP_MASK          0x1u
#define RENG_PRIMITIVE_SETUP_LINE_LOOP_INPLACE       (RENG_PRIMITIVE_SETUP_LINE_LOOP_MASK << RENG_PRIMITIVE_SETUP_LINE_LOOP_SHIFT)
#define RENG_PRIMITIVE_SETUP_LINE_WIDTH_SHIFT        15 /* inferred */
#define RENG_PRIMITIVE_SETUP_LINE_WIDTH_MASK         0xfffu
#define RENG_PRIMITIVE_SETUP_LINE_WIDTH_INPLACE      (RENG_PRIMITIVE_SETUP_LINE_WIDTH_MASK << RENG_PRIMITIVE_SETUP_LINE_WIDTH_SHIFT)
#define RENG_PRIMITIVE_SETUP_CULL_FACE_SHIFT         13 /* inferred */
#define RENG_PRIMITIVE_SETUP_CULL_FACE_MASK          0x3u
#define RENG_PRIMITIVE_SETUP_CULL_FACE_INPLACE       (RENG_PRIMITIVE_SETUP_CULL_FACE_MASK << RENG_PRIMITIVE_SETUP_CULL_FACE_SHIFT)
#define RENG_PRIMITIVE_SETUP_FRONT_FACE_SHIFT        12 /* inferred */
#define RENG_PRIMITIVE_SETUP_FRONT_FACE_MASK         0x1u
#define RENG_PRIMITIVE_SETUP_FRONT_FACE_INPLACE      (RENG_PRIMITIVE_SETUP_FRONT_FACE_MASK << RENG_PRIMITIVE_SETUP_FRONT_FACE_SHIFT)
#define RENG_PRIMITIVE_SETUP_BACK_FACE_AS_LINE_SHIFT 11 /* inferred */
#define RENG_PRIMITIVE_SETUP_BACK_FACE_AS_LINE_MASK  0x1u
#define RENG_PRIMITIVE_SETUP_BACK_FACE_AS_LINE_INPLACE (RENG_PRIMITIVE_SETUP_BACK_FACE_AS_LINE_MASK << RENG_PRIMITIVE_SETUP_BACK_FACE_AS_LINE_SHIFT)
#define RENG_PRIMITIVE_SETUP_DIRECTION_10_9_SHIFT    9 /* inferred */
#define RENG_PRIMITIVE_SETUP_DIRECTION_10_9_MASK     0x3u
#define RENG_PRIMITIVE_SETUP_DIRECTION_10_9_INPLACE  (RENG_PRIMITIVE_SETUP_DIRECTION_10_9_MASK << RENG_PRIMITIVE_SETUP_DIRECTION_10_9_SHIFT)
#define RENG_PRIMITIVE_SETUP_DIRECTION_SELECT_8_SHIFT 8 /* inferred */
#define RENG_PRIMITIVE_SETUP_DIRECTION_SELECT_8_MASK 0x1u
#define RENG_PRIMITIVE_SETUP_DIRECTION_SELECT_8_INPLACE (RENG_PRIMITIVE_SETUP_DIRECTION_SELECT_8_MASK << RENG_PRIMITIVE_SETUP_DIRECTION_SELECT_8_SHIFT)
#define RENG_PRIMITIVE_SETUP_RESERVED_SHIFT          0 /* unknown */
#define RENG_PRIMITIVE_SETUP_RESERVED_MASK           0xffu
#define RENG_PRIMITIVE_SETUP_RESERVED_INPLACE        (RENG_PRIMITIVE_SETUP_RESERVED_MASK << RENG_PRIMITIVE_SETUP_RESERVED_SHIFT)

/*
 * Logic operation word.
 */
/* word RASTER_LOGIC: 32 bits; SGI Cobalt Graphics Driver 5.2.4 logic-operation writer */
#define RENG_RASTER_LOGIC_WIDTH                      32 /* inferred */
#define RENG_RASTER_LOGIC_OP_SHIFT                   28 /* inferred */
#define RENG_RASTER_LOGIC_OP_MASK                    0xfu
#define RENG_RASTER_LOGIC_OP_INPLACE                 (RENG_RASTER_LOGIC_OP_MASK << RENG_RASTER_LOGIC_OP_SHIFT)

/*
 * First word of the stencil packet.
 */
/* word STENCIL_CONFIG: 32 bits; SGI Cobalt Graphics Driver 5.2.4 stencil writers */
#define RENG_STENCIL_CONFIG_WIDTH                    32 /* inferred */
#define RENG_STENCIL_CONFIG_PASS_OP_SHIFT            28 /* inferred */
#define RENG_STENCIL_CONFIG_PASS_OP_MASK             0xfu
#define RENG_STENCIL_CONFIG_PASS_OP_INPLACE          (RENG_STENCIL_CONFIG_PASS_OP_MASK << RENG_STENCIL_CONFIG_PASS_OP_SHIFT)
#define RENG_STENCIL_CONFIG_DEPTH_FAIL_OP_SHIFT      24 /* inferred */
#define RENG_STENCIL_CONFIG_DEPTH_FAIL_OP_MASK       0xfu
#define RENG_STENCIL_CONFIG_DEPTH_FAIL_OP_INPLACE    (RENG_STENCIL_CONFIG_DEPTH_FAIL_OP_MASK << RENG_STENCIL_CONFIG_DEPTH_FAIL_OP_SHIFT)
#define RENG_STENCIL_CONFIG_STENCIL_FAIL_OP_SHIFT    20 /* inferred */
#define RENG_STENCIL_CONFIG_STENCIL_FAIL_OP_MASK     0xfu
#define RENG_STENCIL_CONFIG_STENCIL_FAIL_OP_INPLACE  (RENG_STENCIL_CONFIG_STENCIL_FAIL_OP_MASK << RENG_STENCIL_CONFIG_STENCIL_FAIL_OP_SHIFT)
#define RENG_STENCIL_CONFIG_FUNC_SHIFT               16 /* inferred */
#define RENG_STENCIL_CONFIG_FUNC_MASK                0xfu
#define RENG_STENCIL_CONFIG_FUNC_INPLACE             (RENG_STENCIL_CONFIG_FUNC_MASK << RENG_STENCIL_CONFIG_FUNC_SHIFT)
#define RENG_STENCIL_CONFIG_MASK_SHIFT               8 /* inferred */
#define RENG_STENCIL_CONFIG_MASK_MASK                0xffu
#define RENG_STENCIL_CONFIG_MASK_INPLACE             (RENG_STENCIL_CONFIG_MASK_MASK << RENG_STENCIL_CONFIG_MASK_SHIFT)
#define RENG_STENCIL_CONFIG_REF_SHIFT                0 /* inferred */
#define RENG_STENCIL_CONFIG_REF_MASK                 0xffu
#define RENG_STENCIL_CONFIG_REF_INPLACE              (RENG_STENCIL_CONFIG_REF_MASK << RENG_STENCIL_CONFIG_REF_SHIFT)

/*
 * First word of the two-word texture-environment packet; the second is the environment colour.
 */
/* word TEXTURE_APPLY: 32 bits; SGI Cobalt Graphics Driver 5.2.4 texture-environment writers */
#define RENG_TEXTURE_APPLY_WIDTH                     32 /* inferred */
#define RENG_TEXTURE_APPLY_FUNC_SHIFT                29 /* inferred */
#define RENG_TEXTURE_APPLY_FUNC_MASK                 0x7u
#define RENG_TEXTURE_APPLY_FUNC_INPLACE              (RENG_TEXTURE_APPLY_FUNC_MASK << RENG_TEXTURE_APPLY_FUNC_SHIFT)
#define RENG_TEXTURE_APPLY_TYPE_SHIFT                25 /* inferred */
#define RENG_TEXTURE_APPLY_TYPE_MASK                 0xfu
#define RENG_TEXTURE_APPLY_TYPE_INPLACE              (RENG_TEXTURE_APPLY_TYPE_MASK << RENG_TEXTURE_APPLY_TYPE_SHIFT)

/*
 * First word of the four-word texture packet; the display driver's YUV blit sends 14 or 15 as the texture type.
 */
/* word TEXTURE_FORMAT_WORD: 32 bits; SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers */
#define RENG_TEXTURE_FORMAT_WORD_WIDTH               32 /* inferred */
#define RENG_TEXTURE_FORMAT_WORD_LINEAR_SHIFT        31 /* inferred */
#define RENG_TEXTURE_FORMAT_WORD_LINEAR_MASK         0x1u
#define RENG_TEXTURE_FORMAT_WORD_LINEAR_INPLACE      (RENG_TEXTURE_FORMAT_WORD_LINEAR_MASK << RENG_TEXTURE_FORMAT_WORD_LINEAR_SHIFT)
#define RENG_TEXTURE_FORMAT_WORD_TEXTURE_KIND_SHIFT  27 /* inferred */
#define RENG_TEXTURE_FORMAT_WORD_TEXTURE_KIND_MASK   0xfu
#define RENG_TEXTURE_FORMAT_WORD_TEXTURE_KIND_INPLACE (RENG_TEXTURE_FORMAT_WORD_TEXTURE_KIND_MASK << RENG_TEXTURE_FORMAT_WORD_TEXTURE_KIND_SHIFT)
#define RENG_TEXTURE_FORMAT_WORD_SIZE_CLASS_SHIFT    25 /* inferred */
#define RENG_TEXTURE_FORMAT_WORD_SIZE_CLASS_MASK     0x3u
#define RENG_TEXTURE_FORMAT_WORD_SIZE_CLASS_INPLACE  (RENG_TEXTURE_FORMAT_WORD_SIZE_CLASS_MASK << RENG_TEXTURE_FORMAT_WORD_SIZE_CLASS_SHIFT)
#define RENG_TEXTURE_FORMAT_WORD_BUFFER_ID_SHIFT     19 /* inferred */
#define RENG_TEXTURE_FORMAT_WORD_BUFFER_ID_MASK      0x3fu
#define RENG_TEXTURE_FORMAT_WORD_BUFFER_ID_INPLACE   (RENG_TEXTURE_FORMAT_WORD_BUFFER_ID_MASK << RENG_TEXTURE_FORMAT_WORD_BUFFER_ID_SHIFT)

/*
 * Third word of the texture packet; the fourth is the border colour.
 */
/* word TEXTURE_LOD_RANGE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers */
#define RENG_TEXTURE_LOD_RANGE_WIDTH                 32 /* inferred */
#define RENG_TEXTURE_LOD_RANGE_MAX_LEVEL_SHIFT       28 /* inferred */
#define RENG_TEXTURE_LOD_RANGE_MAX_LEVEL_MASK        0xfu
#define RENG_TEXTURE_LOD_RANGE_MAX_LEVEL_INPLACE     (RENG_TEXTURE_LOD_RANGE_MAX_LEVEL_MASK << RENG_TEXTURE_LOD_RANGE_MAX_LEVEL_SHIFT)
#define RENG_TEXTURE_LOD_RANGE_MIN_LEVEL_SHIFT       24 /* inferred */
#define RENG_TEXTURE_LOD_RANGE_MIN_LEVEL_MASK        0xfu
#define RENG_TEXTURE_LOD_RANGE_MIN_LEVEL_INPLACE     (RENG_TEXTURE_LOD_RANGE_MIN_LEVEL_MASK << RENG_TEXTURE_LOD_RANGE_MIN_LEVEL_SHIFT)
#define RENG_TEXTURE_LOD_RANGE_FRAC_BITS_SHIFT       21 /* inferred */
#define RENG_TEXTURE_LOD_RANGE_FRAC_BITS_MASK        0x7u
#define RENG_TEXTURE_LOD_RANGE_FRAC_BITS_INPLACE     (RENG_TEXTURE_LOD_RANGE_FRAC_BITS_MASK << RENG_TEXTURE_LOD_RANGE_FRAC_BITS_SHIFT)
#define RENG_TEXTURE_LOD_RANGE_LOD_BIAS_SHIFT        11 /* inferred */
#define RENG_TEXTURE_LOD_RANGE_LOD_BIAS_MASK         0x3ffu
#define RENG_TEXTURE_LOD_RANGE_LOD_BIAS_INPLACE      (RENG_TEXTURE_LOD_RANGE_LOD_BIAS_MASK << RENG_TEXTURE_LOD_RANGE_LOD_BIAS_SHIFT)

/*
 * Second word of the texture packet.
 */
/* word TEXTURE_SAMPLING: 32 bits; SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers */
#define RENG_TEXTURE_SAMPLING_WIDTH                  32 /* inferred */
#define RENG_TEXTURE_SAMPLING_MIN_FILTER_SHIFT       29 /* inferred */
#define RENG_TEXTURE_SAMPLING_MIN_FILTER_MASK        0x7u
#define RENG_TEXTURE_SAMPLING_MIN_FILTER_INPLACE     (RENG_TEXTURE_SAMPLING_MIN_FILTER_MASK << RENG_TEXTURE_SAMPLING_MIN_FILTER_SHIFT)
#define RENG_TEXTURE_SAMPLING_MAG_FILTER_SHIFT       28 /* inferred */
#define RENG_TEXTURE_SAMPLING_MAG_FILTER_MASK        0x1u
#define RENG_TEXTURE_SAMPLING_MAG_FILTER_INPLACE     (RENG_TEXTURE_SAMPLING_MAG_FILTER_MASK << RENG_TEXTURE_SAMPLING_MAG_FILTER_SHIFT)
#define RENG_TEXTURE_SAMPLING_WRAP_T_SHIFT           26 /* inferred */
#define RENG_TEXTURE_SAMPLING_WRAP_T_MASK            0x3u
#define RENG_TEXTURE_SAMPLING_WRAP_T_INPLACE         (RENG_TEXTURE_SAMPLING_WRAP_T_MASK << RENG_TEXTURE_SAMPLING_WRAP_T_SHIFT)
#define RENG_TEXTURE_SAMPLING_WRAP_S_SHIFT           24 /* inferred */
#define RENG_TEXTURE_SAMPLING_WRAP_S_MASK            0x3u
#define RENG_TEXTURE_SAMPLING_WRAP_S_INPLACE         (RENG_TEXTURE_SAMPLING_WRAP_S_MASK << RENG_TEXTURE_SAMPLING_WRAP_S_SHIFT)
#define RENG_TEXTURE_SAMPLING_HEIGHT_CODE_SHIFT      20 /* inferred */
#define RENG_TEXTURE_SAMPLING_HEIGHT_CODE_MASK       0xfu
#define RENG_TEXTURE_SAMPLING_HEIGHT_CODE_INPLACE    (RENG_TEXTURE_SAMPLING_HEIGHT_CODE_MASK << RENG_TEXTURE_SAMPLING_HEIGHT_CODE_SHIFT)
#define RENG_TEXTURE_SAMPLING_WIDTH_CODE_SHIFT       16 /* inferred */
#define RENG_TEXTURE_SAMPLING_WIDTH_CODE_MASK        0xfu
#define RENG_TEXTURE_SAMPLING_WIDTH_CODE_INPLACE     (RENG_TEXTURE_SAMPLING_WIDTH_CODE_MASK << RENG_TEXTURE_SAMPLING_WIDTH_CODE_SHIFT)

/*
 * Transform mode word , the draw flags word.
 */
/* word TRANSFORM_FLAGS: 32 bits; SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver */
#define RENG_TRANSFORM_FLAGS_WIDTH                   32 /* inferred */
#define RENG_TRANSFORM_FLAGS_VIEWPORT_XFORM_ON_SHIFT 31 /* inferred */
#define RENG_TRANSFORM_FLAGS_VIEWPORT_XFORM_ON_MASK  0x1u
#define RENG_TRANSFORM_FLAGS_VIEWPORT_XFORM_ON_INPLACE (RENG_TRANSFORM_FLAGS_VIEWPORT_XFORM_ON_MASK << RENG_TRANSFORM_FLAGS_VIEWPORT_XFORM_ON_SHIFT)
#define RENG_TRANSFORM_FLAGS_MODEL_XFORM_ON_SHIFT    30 /* inferred */
#define RENG_TRANSFORM_FLAGS_MODEL_XFORM_ON_MASK     0x1u
#define RENG_TRANSFORM_FLAGS_MODEL_XFORM_ON_INPLACE  (RENG_TRANSFORM_FLAGS_MODEL_XFORM_ON_MASK << RENG_TRANSFORM_FLAGS_MODEL_XFORM_ON_SHIFT)
#define RENG_TRANSFORM_FLAGS_TEX_XFORM_ON_SHIFT      29 /* inferred */
#define RENG_TRANSFORM_FLAGS_TEX_XFORM_ON_MASK       0x1u
#define RENG_TRANSFORM_FLAGS_TEX_XFORM_ON_INPLACE    (RENG_TRANSFORM_FLAGS_TEX_XFORM_ON_MASK << RENG_TRANSFORM_FLAGS_TEX_XFORM_ON_SHIFT)
#define RENG_TRANSFORM_FLAGS_CLIP_CHECK_ON_SHIFT     28 /* inferred */
#define RENG_TRANSFORM_FLAGS_CLIP_CHECK_ON_MASK      0x1u
#define RENG_TRANSFORM_FLAGS_CLIP_CHECK_ON_INPLACE   (RENG_TRANSFORM_FLAGS_CLIP_CHECK_ON_MASK << RENG_TRANSFORM_FLAGS_CLIP_CHECK_ON_SHIFT)

#define RENG_BLEND_CONSTANT_ALPHA                    0xau /* inferred */
#define RENG_BLEND_CONSTANT_COLOR                    0x8u /* inferred */
#define RENG_BLEND_DST_ALPHA                         0x6u /* inferred */
#define RENG_BLEND_ONE                               0x1u /* inferred */
#define RENG_BLEND_ONE_MINUS_CONSTANT_ALPHA          0xbu /* inferred */
#define RENG_BLEND_ONE_MINUS_CONSTANT_COLOR          0x9u /* inferred */
#define RENG_BLEND_ONE_MINUS_DST_ALPHA               0x7u /* inferred */
#define RENG_BLEND_ONE_MINUS_OTHER_COLOR             0x3u /* inferred */
#define RENG_BLEND_ONE_MINUS_SRC_ALPHA               0x5u /* inferred */
#define RENG_BLEND_OP_ADD                            0x0u /* inferred */
#define RENG_BLEND_OP_MAX                            0x2u /* inferred */
#define RENG_BLEND_OP_MIN                            0x1u /* inferred */
#define RENG_BLEND_OP_REVERSE_SUBTRACT               0x4u /* inferred */
#define RENG_BLEND_OP_SUBTRACT                       0x3u /* inferred */
#define RENG_BLEND_OTHER_COLOR                       0x2u /* inferred */
#define RENG_BLEND_SRC_ALPHA                         0x4u /* inferred */
#define RENG_BLEND_SRC_ALPHA_SATURATE                0xcu /* inferred */
#define RENG_BLEND_ZERO                              0x0u /* inferred */
#define RENG_CMP_ALWAYS                              0x7u /* inferred */
#define RENG_CMP_EQUAL                               0x2u /* inferred */
#define RENG_CMP_GEQUAL                              0x6u /* inferred */
#define RENG_CMP_GREATER                             0x4u /* inferred */
#define RENG_CMP_LEQUAL                              0x3u /* inferred */
#define RENG_CMP_LESS                                0x1u /* inferred */
#define RENG_CMP_NEVER                               0x0u /* inferred */
#define RENG_CMP_NOTEQUAL                            0x5u /* inferred */
#define RENG_FILTER_LINEAR                           0x1u /* inferred */
#define RENG_FILTER_LINEAR_MIP_LINEAR                0x5u /* inferred */
#define RENG_FILTER_LINEAR_MIP_NEAREST               0x3u /* inferred */
#define RENG_FILTER_NEAREST                          0x0u /* inferred */
#define RENG_FILTER_NEAREST_MIP_LINEAR               0x4u /* inferred */
#define RENG_FILTER_NEAREST_MIP_NEAREST              0x2u /* inferred */
#define RENG_STENCIL_DECR                            0x4u /* inferred */
#define RENG_STENCIL_INCR                            0x3u /* inferred */
#define RENG_STENCIL_INVERT                          0x5u /* inferred */
#define RENG_STENCIL_KEEP                            0x0u /* inferred */
#define RENG_STENCIL_REPLACE                         0x2u /* inferred */
#define RENG_STENCIL_ZERO                            0x1u /* inferred */
#define RENG_TEXCLASS_ALPHA                          0x3u /* inferred */
#define RENG_TEXCLASS_INTENSITY                      0x4u /* inferred */
#define RENG_TEXCLASS_LUMINANCE                      0x2u /* inferred */
#define RENG_TEXCLASS_LUMINANCE_ALPHA                0x5u /* inferred */
#define RENG_TEXCLASS_RGB                            0x0u /* inferred */
#define RENG_TEXCLASS_RGBA                           0x1u /* inferred */
#define RENG_TEXENV_ADD                              0x4u /* inferred */
#define RENG_TEXENV_BLEND                            0x2u /* inferred */
#define RENG_TEXENV_DECAL                            0x1u /* inferred */
#define RENG_TEXENV_MODULATE                         0x0u /* inferred */
#define RENG_TEXENV_REPLACE                          0x3u /* inferred */
#define RENG_TEXTYPE_ALPHA_OR_INTENSITY8             0x8u /* inferred */
#define RENG_TEXTYPE_INDEX8                          0x0u /* inferred */
#define RENG_TEXTYPE_LUMINANCE8                      0x6u /* inferred */
#define RENG_TEXTYPE_LUMINANCE_ALPHA8                0x7u /* inferred */
#define RENG_TEXTYPE_RGB5A1                          0x2u /* inferred */
#define RENG_TEXTYPE_RGBA4                           0x3u /* inferred */
#define RENG_TEXTYPE_RGBA8                           0x4u /* inferred */
#define RENG_TEXTYPE_RGBA8_ALT                       0x5u /* unknown */
#define RENG_TEXTYPE_YUV_A                           0xeu /* inferred */
#define RENG_TEXTYPE_YUV_B                           0xfu /* inferred */
#define RENG_WINDOW_PHYS                             0xc8000000u /* inferred */
#define RENG_WINDOW_SIZE                             0x19000u /* inferred */
#define RENG_WRAP_CLAMP                              0x1u /* inferred */
#define RENG_WRAP_CLAMP_TO_EDGE                      0x2u /* inferred */
#define RENG_WRAP_REPEAT                             0x0u /* inferred */

#endif /* RENG_GEN_H */
