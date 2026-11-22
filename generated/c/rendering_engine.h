/* generated from devices/rendering-engine/registers.toml by defs-gen - do not edit, change the source */
/* SPDX-License-Identifier: BSD-3-Clause */
/* defs-gen 2 */
/* input sha256 fb841a578c212e7cf6a2e7317ffba61e7675d09ac68395332fe6ea9bda2c1a22 */
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
 * Buffer descriptor table: entry k describes the buffer whose descriptor id is k, the six-bit id the colour-format, depth-format and texture-format words carry. Entry 0 is the packet pool. The low dword only is written. A tiled buffer is a run of tiles of 128 or 512 bytes in a row, with tiles_per_row tiles per row, whose pages start at the translation-table slot in offset; a user buffer mapped linearly sets bit 19 (tiles_per_row 32) with the page-size bits kept and the packet pool sets page_type and tiles_per_row 1.
 */
/* tlb_desc: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database */
#define RENG_TLB_DESC_OFFSET                         0xc000u /* inferred */
#define RENG_TLB_DESC_COUNT                          64
#define RENG_TLB_DESC_STRIDE                         8
#define RENG_TLB_DESC_OFFSET_SHIFT                   21 /* inferred */
#define RENG_TLB_DESC_OFFSET_MASK                    0x7ffu
#define RENG_TLB_DESC_OFFSET_INPLACE                 (RENG_TLB_DESC_OFFSET_MASK << RENG_TLB_DESC_OFFSET_SHIFT)
#define RENG_TLB_DESC_TILES_PER_ROW_SHIFT            14 /* inferred */
#define RENG_TLB_DESC_TILES_PER_ROW_MASK             0x7fu
#define RENG_TLB_DESC_TILES_PER_ROW_INPLACE          (RENG_TLB_DESC_TILES_PER_ROW_MASK << RENG_TLB_DESC_TILES_PER_ROW_SHIFT)
#define RENG_TLB_DESC_TILE_WIDTH_SHIFT               13 /* inferred */
#define RENG_TLB_DESC_TILE_WIDTH_MASK                0x1u
#define RENG_TLB_DESC_TILE_WIDTH_INPLACE             (RENG_TLB_DESC_TILE_WIDTH_MASK << RENG_TLB_DESC_TILE_WIDTH_SHIFT)
#define RENG_TLB_DESC_PAGE_SIZE_SHIFT                10 /* inferred */
#define RENG_TLB_DESC_PAGE_SIZE_MASK                 0x7u
#define RENG_TLB_DESC_PAGE_SIZE_INPLACE              (RENG_TLB_DESC_PAGE_SIZE_MASK << RENG_TLB_DESC_PAGE_SIZE_SHIFT)
#define RENG_TLB_DESC_PAGE_TYPE_SHIFT                9 /* inferred */
#define RENG_TLB_DESC_PAGE_TYPE_MASK                 0x1u
#define RENG_TLB_DESC_PAGE_TYPE_INPLACE              (RENG_TLB_DESC_PAGE_TYPE_MASK << RENG_TLB_DESC_PAGE_TYPE_SHIFT)

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
#define RENG_EXEC_CTL_IDLE_COUNT_SHIFT               0 /* inferred */
#define RENG_EXEC_CTL_IDLE_COUNT_MASK                0x7ffu
#define RENG_EXEC_CTL_IDLE_COUNT_INPLACE             (RENG_EXEC_CTL_IDLE_COUNT_MASK << RENG_EXEC_CTL_IDLE_COUNT_SHIFT)

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
/* tex_id: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch, stop-event and bind routines; the OpenGL driver's texture-free packet */
#define RENG_TEX_ID_OFFSET                           0x185d0u /* inferred */

/*
 * Written with the outgoing context's dump offset (the field the driver names as its context-switch dump offset) when another context is started, before the new context's start word.
 */
/* ctx_dump: w32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch */
#define RENG_CTX_DUMP_OFFSET                         0x18800u /* inferred */

/*
 * First word of the blend packet; the second is the constant colour.
 */
/* word BLEND_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoBlendModeReg, blend writer */
#define RENG_BLEND_MODE_WIDTH                        32 /* inferred */
#define RENG_BLEND_MODE_DST_FUNC_SHIFT               28 /* inferred */
#define RENG_BLEND_MODE_DST_FUNC_MASK                0xfu
#define RENG_BLEND_MODE_DST_FUNC_INPLACE             (RENG_BLEND_MODE_DST_FUNC_MASK << RENG_BLEND_MODE_DST_FUNC_SHIFT)
#define RENG_BLEND_MODE_SRC_FUNC_SHIFT               24 /* inferred */
#define RENG_BLEND_MODE_SRC_FUNC_MASK                0xfu
#define RENG_BLEND_MODE_SRC_FUNC_INPLACE             (RENG_BLEND_MODE_SRC_FUNC_MASK << RENG_BLEND_MODE_SRC_FUNC_SHIFT)
#define RENG_BLEND_MODE_OP_SHIFT                     20 /* inferred */
#define RENG_BLEND_MODE_OP_MASK                      0xfu
#define RENG_BLEND_MODE_OP_INPLACE                   (RENG_BLEND_MODE_OP_MASK << RENG_BLEND_MODE_OP_SHIFT)

/*
 * Draw-buffer descriptor.
 */
/* word COLOR_FORMAT: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers */
#define RENG_COLOR_FORMAT_WIDTH                      32 /* inferred */
#define RENG_COLOR_FORMAT_LINEAR_SHIFT               31 /* inferred */
#define RENG_COLOR_FORMAT_LINEAR_MASK                0x1u
#define RENG_COLOR_FORMAT_LINEAR_INPLACE             (RENG_COLOR_FORMAT_LINEAR_MASK << RENG_COLOR_FORMAT_LINEAR_SHIFT)
#define RENG_COLOR_FORMAT_COLOR_TYPE_SHIFT           27 /* inferred */
#define RENG_COLOR_FORMAT_COLOR_TYPE_MASK            0xfu
#define RENG_COLOR_FORMAT_COLOR_TYPE_INPLACE         (RENG_COLOR_FORMAT_COLOR_TYPE_MASK << RENG_COLOR_FORMAT_COLOR_TYPE_SHIFT)
#define RENG_COLOR_FORMAT_BUF_DEPTH_SHIFT            25 /* inferred */
#define RENG_COLOR_FORMAT_BUF_DEPTH_MASK             0x3u
#define RENG_COLOR_FORMAT_BUF_DEPTH_INPLACE          (RENG_COLOR_FORMAT_BUF_DEPTH_MASK << RENG_COLOR_FORMAT_BUF_DEPTH_SHIFT)
#define RENG_COLOR_FORMAT_DESC_ID_SHIFT              19 /* inferred */
#define RENG_COLOR_FORMAT_DESC_ID_MASK               0x3fu
#define RENG_COLOR_FORMAT_DESC_ID_INPLACE            (RENG_COLOR_FORMAT_DESC_ID_MASK << RENG_COLOR_FORMAT_DESC_ID_SHIFT)
#define RENG_COLOR_FORMAT_SPAN_MODE_SHIFT            18 /* inferred */
#define RENG_COLOR_FORMAT_SPAN_MODE_MASK             0x1u
#define RENG_COLOR_FORMAT_SPAN_MODE_INPLACE          (RENG_COLOR_FORMAT_SPAN_MODE_MASK << RENG_COLOR_FORMAT_SPAN_MODE_SHIFT)

/*
 * First word of the alpha-test packet; the second is the reference colour.
 */
/* word COLOR_TEST_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoColorTestModeReg, alpha-test writer */
#define RENG_COLOR_TEST_MODE_WIDTH                   32 /* inferred */
#define RENG_COLOR_TEST_MODE_EN_COMP_SHIFT           28 /* inferred */
#define RENG_COLOR_TEST_MODE_EN_COMP_MASK            0xfu
#define RENG_COLOR_TEST_MODE_EN_COMP_INPLACE         (RENG_COLOR_TEST_MODE_EN_COMP_MASK << RENG_COLOR_TEST_MODE_EN_COMP_SHIFT)
#define RENG_COLOR_TEST_MODE_FUNC_SHIFT              25 /* inferred */
#define RENG_COLOR_TEST_MODE_FUNC_MASK               0x7u
#define RENG_COLOR_TEST_MODE_FUNC_INPLACE            (RENG_COLOR_TEST_MODE_FUNC_MASK << RENG_COLOR_TEST_MODE_FUNC_SHIFT)

/*
 * Depth test word.
 */
/* word DEPTH_TEST: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers */
#define RENG_DEPTH_TEST_WIDTH                        32 /* inferred */
#define RENG_DEPTH_TEST_FUNC_SHIFT                   29 /* inferred */
#define RENG_DEPTH_TEST_FUNC_MASK                    0x7u
#define RENG_DEPTH_TEST_FUNC_INPLACE                 (RENG_DEPTH_TEST_FUNC_MASK << RENG_DEPTH_TEST_FUNC_SHIFT)
#define RENG_DEPTH_TEST_INIT_OCCLUSION_TEST_SHIFT    28 /* inferred */
#define RENG_DEPTH_TEST_INIT_OCCLUSION_TEST_MASK     0x1u
#define RENG_DEPTH_TEST_INIT_OCCLUSION_TEST_INPLACE  (RENG_DEPTH_TEST_INIT_OCCLUSION_TEST_MASK << RENG_DEPTH_TEST_INIT_OCCLUSION_TEST_SHIFT)
#define RENG_DEPTH_TEST_EN_OCCLUSION_TEST_SHIFT      27 /* inferred */
#define RENG_DEPTH_TEST_EN_OCCLUSION_TEST_MASK       0x1u
#define RENG_DEPTH_TEST_EN_OCCLUSION_TEST_INPLACE    (RENG_DEPTH_TEST_EN_OCCLUSION_TEST_MASK << RENG_DEPTH_TEST_EN_OCCLUSION_TEST_SHIFT)
#define RENG_DEPTH_TEST_EQUAL_RANGE_SHIFT            23 /* inferred */
#define RENG_DEPTH_TEST_EQUAL_RANGE_MASK             0xfu
#define RENG_DEPTH_TEST_EQUAL_RANGE_INPLACE          (RENG_DEPTH_TEST_EQUAL_RANGE_MASK << RENG_DEPTH_TEST_EQUAL_RANGE_SHIFT)

/*
 * Pixel-pipe mode word.
 */
/* word DRAW_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver */
#define RENG_DRAW_MODE_WIDTH                         32 /* inferred */
#define RENG_DRAW_MODE_EN_STENCIL_TEST_SHIFT         31 /* inferred */
#define RENG_DRAW_MODE_EN_STENCIL_TEST_MASK          0x1u
#define RENG_DRAW_MODE_EN_STENCIL_TEST_INPLACE       (RENG_DRAW_MODE_EN_STENCIL_TEST_MASK << RENG_DRAW_MODE_EN_STENCIL_TEST_SHIFT)
#define RENG_DRAW_MODE_EN_DEPTH_MASK_SHIFT           30 /* inferred */
#define RENG_DRAW_MODE_EN_DEPTH_MASK_MASK            0x1u
#define RENG_DRAW_MODE_EN_DEPTH_MASK_INPLACE         (RENG_DRAW_MODE_EN_DEPTH_MASK_MASK << RENG_DRAW_MODE_EN_DEPTH_MASK_SHIFT)
#define RENG_DRAW_MODE_EN_DEPTH_TEST_SHIFT           29 /* inferred */
#define RENG_DRAW_MODE_EN_DEPTH_TEST_MASK            0x1u
#define RENG_DRAW_MODE_EN_DEPTH_TEST_INPLACE         (RENG_DRAW_MODE_EN_DEPTH_TEST_MASK << RENG_DRAW_MODE_EN_DEPTH_TEST_SHIFT)
#define RENG_DRAW_MODE_EN_DEPTH_OFFSET_SHIFT         28 /* inferred */
#define RENG_DRAW_MODE_EN_DEPTH_OFFSET_MASK          0x1u
#define RENG_DRAW_MODE_EN_DEPTH_OFFSET_INPLACE       (RENG_DRAW_MODE_EN_DEPTH_OFFSET_MASK << RENG_DRAW_MODE_EN_DEPTH_OFFSET_SHIFT)
#define RENG_DRAW_MODE_EN_COLOR_DISCARD_SHIFT        27 /* inferred */
#define RENG_DRAW_MODE_EN_COLOR_DISCARD_MASK         0x1u
#define RENG_DRAW_MODE_EN_COLOR_DISCARD_INPLACE      (RENG_DRAW_MODE_EN_COLOR_DISCARD_MASK << RENG_DRAW_MODE_EN_COLOR_DISCARD_SHIFT)
#define RENG_DRAW_MODE_EN_COLOR_MASK_SHIFT           26 /* inferred */
#define RENG_DRAW_MODE_EN_COLOR_MASK_MASK            0x1u
#define RENG_DRAW_MODE_EN_COLOR_MASK_INPLACE         (RENG_DRAW_MODE_EN_COLOR_MASK_MASK << RENG_DRAW_MODE_EN_COLOR_MASK_SHIFT)
#define RENG_DRAW_MODE_EN_DITHER_SHIFT               25 /* inferred */
#define RENG_DRAW_MODE_EN_DITHER_MASK                0x1u
#define RENG_DRAW_MODE_EN_DITHER_INPLACE             (RENG_DRAW_MODE_EN_DITHER_MASK << RENG_DRAW_MODE_EN_DITHER_SHIFT)
#define RENG_DRAW_MODE_EN_LOGIC_OP_SHIFT             24 /* inferred */
#define RENG_DRAW_MODE_EN_LOGIC_OP_MASK              0x1u
#define RENG_DRAW_MODE_EN_LOGIC_OP_INPLACE           (RENG_DRAW_MODE_EN_LOGIC_OP_MASK << RENG_DRAW_MODE_EN_LOGIC_OP_SHIFT)
#define RENG_DRAW_MODE_EN_BLEND_SHIFT                23 /* inferred */
#define RENG_DRAW_MODE_EN_BLEND_MASK                 0x1u
#define RENG_DRAW_MODE_EN_BLEND_INPLACE              (RENG_DRAW_MODE_EN_BLEND_MASK << RENG_DRAW_MODE_EN_BLEND_SHIFT)
#define RENG_DRAW_MODE_EN_COLOR_TEST_SHIFT           22 /* inferred */
#define RENG_DRAW_MODE_EN_COLOR_TEST_MASK            0x1u
#define RENG_DRAW_MODE_EN_COLOR_TEST_INPLACE         (RENG_DRAW_MODE_EN_COLOR_TEST_MASK << RENG_DRAW_MODE_EN_COLOR_TEST_SHIFT)
#define RENG_DRAW_MODE_EN_FOG_SHIFT                  21 /* inferred */
#define RENG_DRAW_MODE_EN_FOG_MASK                   0x1u
#define RENG_DRAW_MODE_EN_FOG_INPLACE                (RENG_DRAW_MODE_EN_FOG_MASK << RENG_DRAW_MODE_EN_FOG_SHIFT)
#define RENG_DRAW_MODE_EN_COLOR_MATRIX_SHIFT         20 /* inferred */
#define RENG_DRAW_MODE_EN_COLOR_MATRIX_MASK          0x1u
#define RENG_DRAW_MODE_EN_COLOR_MATRIX_INPLACE       (RENG_DRAW_MODE_EN_COLOR_MATRIX_MASK << RENG_DRAW_MODE_EN_COLOR_MATRIX_SHIFT)
#define RENG_DRAW_MODE_EN_PIXEL_XFER_SHIFT           19 /* inferred */
#define RENG_DRAW_MODE_EN_PIXEL_XFER_MASK            0x1u
#define RENG_DRAW_MODE_EN_PIXEL_XFER_INPLACE         (RENG_DRAW_MODE_EN_PIXEL_XFER_MASK << RENG_DRAW_MODE_EN_PIXEL_XFER_SHIFT)
#define RENG_DRAW_MODE_EN_TEXTURE_SHIFT              18 /* inferred */
#define RENG_DRAW_MODE_EN_TEXTURE_MASK               0x1u
#define RENG_DRAW_MODE_EN_TEXTURE_INPLACE            (RENG_DRAW_MODE_EN_TEXTURE_MASK << RENG_DRAW_MODE_EN_TEXTURE_SHIFT)
#define RENG_DRAW_MODE_EN_SECOND_COLOR_SHIFT         17 /* inferred */
#define RENG_DRAW_MODE_EN_SECOND_COLOR_MASK          0x1u
#define RENG_DRAW_MODE_EN_SECOND_COLOR_INPLACE       (RENG_DRAW_MODE_EN_SECOND_COLOR_MASK << RENG_DRAW_MODE_EN_SECOND_COLOR_SHIFT)
#define RENG_DRAW_MODE_EN_SMOOTH_SHADE_SHIFT         16 /* inferred */
#define RENG_DRAW_MODE_EN_SMOOTH_SHADE_MASK          0x1u
#define RENG_DRAW_MODE_EN_SMOOTH_SHADE_INPLACE       (RENG_DRAW_MODE_EN_SMOOTH_SHADE_MASK << RENG_DRAW_MODE_EN_SMOOTH_SHADE_SHIFT)
#define RENG_DRAW_MODE_EN_OPAQ_STIPPLE_SHIFT         15 /* inferred */
#define RENG_DRAW_MODE_EN_OPAQ_STIPPLE_MASK          0x1u
#define RENG_DRAW_MODE_EN_OPAQ_STIPPLE_INPLACE       (RENG_DRAW_MODE_EN_OPAQ_STIPPLE_MASK << RENG_DRAW_MODE_EN_OPAQ_STIPPLE_SHIFT)
#define RENG_DRAW_MODE_EN_POLY_STIPPLE_SHIFT         14 /* inferred */
#define RENG_DRAW_MODE_EN_POLY_STIPPLE_MASK          0x1u
#define RENG_DRAW_MODE_EN_POLY_STIPPLE_INPLACE       (RENG_DRAW_MODE_EN_POLY_STIPPLE_MASK << RENG_DRAW_MODE_EN_POLY_STIPPLE_SHIFT)
#define RENG_DRAW_MODE_EN_LINE_STIPPLE_SHIFT         13 /* inferred */
#define RENG_DRAW_MODE_EN_LINE_STIPPLE_MASK          0x1u
#define RENG_DRAW_MODE_EN_LINE_STIPPLE_INPLACE       (RENG_DRAW_MODE_EN_LINE_STIPPLE_MASK << RENG_DRAW_MODE_EN_LINE_STIPPLE_SHIFT)
#define RENG_DRAW_MODE_RESERVED_SHIFT                0 /* unknown */
#define RENG_DRAW_MODE_RESERVED_MASK                 0x1fffu
#define RENG_DRAW_MODE_RESERVED_INPLACE              (RENG_DRAW_MODE_RESERVED_MASK << RENG_DRAW_MODE_RESERVED_SHIFT)

/*
 * A corner: x in the low half, y in the high half (signed 16-bit).
 */
/* word FB_ADDR: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoFbAddrType, scissor and clear-rectangle writers */
#define RENG_FB_ADDR_WIDTH                           32 /* inferred */
#define RENG_FB_ADDR_Y_SHIFT                         16 /* inferred */
#define RENG_FB_ADDR_Y_MASK                          0xffffu
#define RENG_FB_ADDR_Y_INPLACE                       (RENG_FB_ADDR_Y_MASK << RENG_FB_ADDR_Y_SHIFT)
#define RENG_FB_ADDR_X_SHIFT                         0 /* inferred */
#define RENG_FB_ADDR_X_MASK                          0xffffu
#define RENG_FB_ADDR_X_INPLACE                       (RENG_FB_ADDR_X_MASK << RENG_FB_ADDR_X_SHIFT)

/*
 * Lighting mode word.
 */
/* word LIGHT_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers */
#define RENG_LIGHT_MODE_WIDTH                        32 /* inferred */
#define RENG_LIGHT_MODE_TWO_SIDED_SHIFT              31 /* inferred */
#define RENG_LIGHT_MODE_TWO_SIDED_MASK               0x1u
#define RENG_LIGHT_MODE_TWO_SIDED_INPLACE            (RENG_LIGHT_MODE_TWO_SIDED_MASK << RENG_LIGHT_MODE_TWO_SIDED_SHIFT)
#define RENG_LIGHT_MODE_EN_LIGHT3_SHIFT              30 /* inferred */
#define RENG_LIGHT_MODE_EN_LIGHT3_MASK               0x1u
#define RENG_LIGHT_MODE_EN_LIGHT3_INPLACE            (RENG_LIGHT_MODE_EN_LIGHT3_MASK << RENG_LIGHT_MODE_EN_LIGHT3_SHIFT)
#define RENG_LIGHT_MODE_EN_LIGHT2_SHIFT              29 /* inferred */
#define RENG_LIGHT_MODE_EN_LIGHT2_MASK               0x1u
#define RENG_LIGHT_MODE_EN_LIGHT2_INPLACE            (RENG_LIGHT_MODE_EN_LIGHT2_MASK << RENG_LIGHT_MODE_EN_LIGHT2_SHIFT)
#define RENG_LIGHT_MODE_EN_LIGHT1_SHIFT              28 /* inferred */
#define RENG_LIGHT_MODE_EN_LIGHT1_MASK               0x1u
#define RENG_LIGHT_MODE_EN_LIGHT1_INPLACE            (RENG_LIGHT_MODE_EN_LIGHT1_MASK << RENG_LIGHT_MODE_EN_LIGHT1_SHIFT)
#define RENG_LIGHT_MODE_EN_LIGHT0_SHIFT              27 /* inferred */
#define RENG_LIGHT_MODE_EN_LIGHT0_MASK               0x1u
#define RENG_LIGHT_MODE_EN_LIGHT0_INPLACE            (RENG_LIGHT_MODE_EN_LIGHT0_MASK << RENG_LIGHT_MODE_EN_LIGHT0_SHIFT)
#define RENG_LIGHT_MODE_EN_NORMALIZE_SHIFT           26 /* inferred */
#define RENG_LIGHT_MODE_EN_NORMALIZE_MASK            0x1u
#define RENG_LIGHT_MODE_EN_NORMALIZE_INPLACE         (RENG_LIGHT_MODE_EN_NORMALIZE_MASK << RENG_LIGHT_MODE_EN_NORMALIZE_SHIFT)
#define RENG_LIGHT_MODE_EN_SPECULAR_SHIFT            25 /* inferred */
#define RENG_LIGHT_MODE_EN_SPECULAR_MASK             0x1u
#define RENG_LIGHT_MODE_EN_SPECULAR_INPLACE          (RENG_LIGHT_MODE_EN_SPECULAR_MASK << RENG_LIGHT_MODE_EN_SPECULAR_SHIFT)
#define RENG_LIGHT_MODE_SPEC_ONLY_SHIFT              24 /* inferred */
#define RENG_LIGHT_MODE_SPEC_ONLY_MASK               0x1u
#define RENG_LIGHT_MODE_SPEC_ONLY_INPLACE            (RENG_LIGHT_MODE_SPEC_ONLY_MASK << RENG_LIGHT_MODE_SPEC_ONLY_SHIFT)
#define RENG_LIGHT_MODE_EN_LIGHTING_SHIFT            23 /* inferred */
#define RENG_LIGHT_MODE_EN_LIGHTING_MASK             0x1u
#define RENG_LIGHT_MODE_EN_LIGHTING_INPLACE          (RENG_LIGHT_MODE_EN_LIGHTING_MASK << RENG_LIGHT_MODE_EN_LIGHTING_SHIFT)

/*
 * Link word of a packet node: bit 0 set means the engine owns the node and clears it when done.
 */
/* word LINK_WORD: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers */
#define RENG_LINK_WORD_WIDTH                         32 /* inferred */
#define RENG_LINK_WORD_NEXT_SHIFT                    12 /* inferred */
#define RENG_LINK_WORD_NEXT_MASK                     0xfffffu
#define RENG_LINK_WORD_NEXT_INPLACE                  (RENG_LINK_WORD_NEXT_MASK << RENG_LINK_WORD_NEXT_SHIFT)
#define RENG_LINK_WORD_SIZE_SHIFT                    3 /* inferred */
#define RENG_LINK_WORD_SIZE_MASK                     0x1ffu
#define RENG_LINK_WORD_SIZE_INPLACE                  (RENG_LINK_WORD_SIZE_MASK << RENG_LINK_WORD_SIZE_SHIFT)
#define RENG_LINK_WORD_SYNC_SHIFT                    2 /* inferred */
#define RENG_LINK_WORD_SYNC_MASK                     0x1u
#define RENG_LINK_WORD_SYNC_INPLACE                  (RENG_LINK_WORD_SYNC_MASK << RENG_LINK_WORD_SYNC_SHIFT)
#define RENG_LINK_WORD_INTR_SHIFT                    1 /* inferred */
#define RENG_LINK_WORD_INTR_MASK                     0x1u
#define RENG_LINK_WORD_INTR_INPLACE                  (RENG_LINK_WORD_INTR_MASK << RENG_LINK_WORD_INTR_SHIFT)
#define RENG_LINK_WORD_READY_SHIFT                   0 /* inferred */
#define RENG_LINK_WORD_READY_MASK                    0x1u
#define RENG_LINK_WORD_READY_INPLACE                 (RENG_LINK_WORD_READY_MASK << RENG_LINK_WORD_READY_SHIFT)

/*
 * Logic operation word.
 */
/* word LOGIC_OP: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoLogicOpReg, logic-operation writer */
#define RENG_LOGIC_OP_WIDTH                          32 /* inferred */
#define RENG_LOGIC_OP_OP_SHIFT                       28 /* inferred */
#define RENG_LOGIC_OP_OP_MASK                        0xfu
#define RENG_LOGIC_OP_OP_INPLACE                     (RENG_LOGIC_OP_OP_MASK << RENG_LOGIC_OP_OP_SHIFT)

/*
 * Primitive mode word.
 */
/* word PRIM_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver */
#define RENG_PRIM_MODE_WIDTH                         32 /* inferred */
#define RENG_PRIM_MODE_PRIM_TYPE_SHIFT               29 /* inferred */
#define RENG_PRIM_MODE_PRIM_TYPE_MASK                0x7u
#define RENG_PRIM_MODE_PRIM_TYPE_INPLACE             (RENG_PRIM_MODE_PRIM_TYPE_MASK << RENG_PRIM_MODE_PRIM_TYPE_SHIFT)
#define RENG_PRIM_MODE_AA_LINE_SHIFT                 28 /* inferred */
#define RENG_PRIM_MODE_AA_LINE_MASK                  0x1u
#define RENG_PRIM_MODE_AA_LINE_INPLACE               (RENG_PRIM_MODE_AA_LINE_MASK << RENG_PRIM_MODE_AA_LINE_SHIFT)
#define RENG_PRIM_MODE_CLOSED_LINE_SHIFT             27 /* inferred */
#define RENG_PRIM_MODE_CLOSED_LINE_MASK              0x1u
#define RENG_PRIM_MODE_CLOSED_LINE_INPLACE           (RENG_PRIM_MODE_CLOSED_LINE_MASK << RENG_PRIM_MODE_CLOSED_LINE_SHIFT)
#define RENG_PRIM_MODE_LINE_WIDTH_SHIFT              15 /* inferred */
#define RENG_PRIM_MODE_LINE_WIDTH_MASK               0xfffu
#define RENG_PRIM_MODE_LINE_WIDTH_INPLACE            (RENG_PRIM_MODE_LINE_WIDTH_MASK << RENG_PRIM_MODE_LINE_WIDTH_SHIFT)
#define RENG_PRIM_MODE_CULL_FACE_SHIFT               13 /* inferred */
#define RENG_PRIM_MODE_CULL_FACE_MASK                0x3u
#define RENG_PRIM_MODE_CULL_FACE_INPLACE             (RENG_PRIM_MODE_CULL_FACE_MASK << RENG_PRIM_MODE_CULL_FACE_SHIFT)
#define RENG_PRIM_MODE_FRONT_FACE_SHIFT              12 /* inferred */
#define RENG_PRIM_MODE_FRONT_FACE_MASK               0x1u
#define RENG_PRIM_MODE_FRONT_FACE_INPLACE            (RENG_PRIM_MODE_FRONT_FACE_MASK << RENG_PRIM_MODE_FRONT_FACE_SHIFT)
#define RENG_PRIM_MODE_BACK_FACE_LINE_SHIFT          11 /* inferred */
#define RENG_PRIM_MODE_BACK_FACE_LINE_MASK           0x1u
#define RENG_PRIM_MODE_BACK_FACE_LINE_INPLACE        (RENG_PRIM_MODE_BACK_FACE_LINE_MASK << RENG_PRIM_MODE_BACK_FACE_LINE_SHIFT)
#define RENG_PRIM_MODE_PRIM_DIR_SHIFT                9 /* inferred */
#define RENG_PRIM_MODE_PRIM_DIR_MASK                 0x3u
#define RENG_PRIM_MODE_PRIM_DIR_INPLACE              (RENG_PRIM_MODE_PRIM_DIR_MASK << RENG_PRIM_MODE_PRIM_DIR_SHIFT)
#define RENG_PRIM_MODE_SEL_PRIM_DIR_SHIFT            8 /* inferred */
#define RENG_PRIM_MODE_SEL_PRIM_DIR_MASK             0x1u
#define RENG_PRIM_MODE_SEL_PRIM_DIR_INPLACE          (RENG_PRIM_MODE_SEL_PRIM_DIR_MASK << RENG_PRIM_MODE_SEL_PRIM_DIR_SHIFT)
#define RENG_PRIM_MODE_RESERVED_SHIFT                0 /* unknown */
#define RENG_PRIM_MODE_RESERVED_MASK                 0xffu
#define RENG_PRIM_MODE_RESERVED_INPLACE              (RENG_PRIM_MODE_RESERVED_MASK << RENG_PRIM_MODE_RESERVED_SHIFT)

/*
 * First word of the stencil packet.
 */
/* word STENCIL_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers */
#define RENG_STENCIL_MODE_WIDTH                      32 /* inferred */
#define RENG_STENCIL_MODE_DPPASS_SHIFT               28 /* inferred */
#define RENG_STENCIL_MODE_DPPASS_MASK                0xfu
#define RENG_STENCIL_MODE_DPPASS_INPLACE             (RENG_STENCIL_MODE_DPPASS_MASK << RENG_STENCIL_MODE_DPPASS_SHIFT)
#define RENG_STENCIL_MODE_DPFAIL_SHIFT               24 /* inferred */
#define RENG_STENCIL_MODE_DPFAIL_MASK                0xfu
#define RENG_STENCIL_MODE_DPFAIL_INPLACE             (RENG_STENCIL_MODE_DPFAIL_MASK << RENG_STENCIL_MODE_DPFAIL_SHIFT)
#define RENG_STENCIL_MODE_SFAIL_SHIFT                20 /* inferred */
#define RENG_STENCIL_MODE_SFAIL_MASK                 0xfu
#define RENG_STENCIL_MODE_SFAIL_INPLACE              (RENG_STENCIL_MODE_SFAIL_MASK << RENG_STENCIL_MODE_SFAIL_SHIFT)
#define RENG_STENCIL_MODE_FUNC_SHIFT                 16 /* inferred */
#define RENG_STENCIL_MODE_FUNC_MASK                  0xfu
#define RENG_STENCIL_MODE_FUNC_INPLACE               (RENG_STENCIL_MODE_FUNC_MASK << RENG_STENCIL_MODE_FUNC_SHIFT)
#define RENG_STENCIL_MODE_MASK_SHIFT                 8 /* inferred */
#define RENG_STENCIL_MODE_MASK_MASK                  0xffu
#define RENG_STENCIL_MODE_MASK_INPLACE               (RENG_STENCIL_MODE_MASK_MASK << RENG_STENCIL_MODE_MASK_SHIFT)
#define RENG_STENCIL_MODE_REF_SHIFT                  0 /* inferred */
#define RENG_STENCIL_MODE_REF_MASK                   0xffu
#define RENG_STENCIL_MODE_REF_INPLACE                (RENG_STENCIL_MODE_REF_MASK << RENG_STENCIL_MODE_REF_SHIFT)

/*
 * Depth and stencil buffer descriptor.
 */
/* word SZ_FORMAT: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoSzFormatReg, depth and stencil descriptor writers */
#define RENG_SZ_FORMAT_WIDTH                         32 /* inferred */
#define RENG_SZ_FORMAT_SZ_TYPE_SHIFT                 31 /* inferred */
#define RENG_SZ_FORMAT_SZ_TYPE_MASK                  0x1u
#define RENG_SZ_FORMAT_SZ_TYPE_INPLACE               (RENG_SZ_FORMAT_SZ_TYPE_MASK << RENG_SZ_FORMAT_SZ_TYPE_SHIFT)
#define RENG_SZ_FORMAT_BUF_DEPTH_SHIFT               29 /* inferred */
#define RENG_SZ_FORMAT_BUF_DEPTH_MASK                0x3u
#define RENG_SZ_FORMAT_BUF_DEPTH_INPLACE             (RENG_SZ_FORMAT_BUF_DEPTH_MASK << RENG_SZ_FORMAT_BUF_DEPTH_SHIFT)
#define RENG_SZ_FORMAT_DESC_ID_SHIFT                 23 /* inferred */
#define RENG_SZ_FORMAT_DESC_ID_MASK                  0x3fu
#define RENG_SZ_FORMAT_DESC_ID_INPLACE               (RENG_SZ_FORMAT_DESC_ID_MASK << RENG_SZ_FORMAT_DESC_ID_SHIFT)

/*
 * First word of the two-word texture-environment packet; the second is the environment colour.
 */
/* word TEXENV_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoTexEnvModeReg, texture-environment writers */
#define RENG_TEXENV_MODE_WIDTH                       32 /* inferred */
#define RENG_TEXENV_MODE_FUNC_SHIFT                  29 /* inferred */
#define RENG_TEXENV_MODE_FUNC_MASK                   0x7u
#define RENG_TEXENV_MODE_FUNC_INPLACE                (RENG_TEXENV_MODE_FUNC_MASK << RENG_TEXENV_MODE_FUNC_SHIFT)
#define RENG_TEXENV_MODE_TYPE_SHIFT                  25 /* inferred */
#define RENG_TEXENV_MODE_TYPE_MASK                   0xfu
#define RENG_TEXENV_MODE_TYPE_INPLACE                (RENG_TEXENV_MODE_TYPE_MASK << RENG_TEXENV_MODE_TYPE_SHIFT)

/*
 * First word of the four-word texture packet; the display driver's YUV blit sends 14 or 15 as the texture type.
 */
/* word TEXGEN_FORMAT: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers */
#define RENG_TEXGEN_FORMAT_WIDTH                     32 /* inferred */
#define RENG_TEXGEN_FORMAT_LINEAR_SHIFT              31 /* inferred */
#define RENG_TEXGEN_FORMAT_LINEAR_MASK               0x1u
#define RENG_TEXGEN_FORMAT_LINEAR_INPLACE            (RENG_TEXGEN_FORMAT_LINEAR_MASK << RENG_TEXGEN_FORMAT_LINEAR_SHIFT)
#define RENG_TEXGEN_FORMAT_TEX_TYPE_SHIFT            27 /* inferred */
#define RENG_TEXGEN_FORMAT_TEX_TYPE_MASK             0xfu
#define RENG_TEXGEN_FORMAT_TEX_TYPE_INPLACE          (RENG_TEXGEN_FORMAT_TEX_TYPE_MASK << RENG_TEXGEN_FORMAT_TEX_TYPE_SHIFT)
#define RENG_TEXGEN_FORMAT_BUF_DEPTH_SHIFT           25 /* inferred */
#define RENG_TEXGEN_FORMAT_BUF_DEPTH_MASK            0x3u
#define RENG_TEXGEN_FORMAT_BUF_DEPTH_INPLACE         (RENG_TEXGEN_FORMAT_BUF_DEPTH_MASK << RENG_TEXGEN_FORMAT_BUF_DEPTH_SHIFT)
#define RENG_TEXGEN_FORMAT_DESC_ID_SHIFT             19 /* inferred */
#define RENG_TEXGEN_FORMAT_DESC_ID_MASK              0x3fu
#define RENG_TEXGEN_FORMAT_DESC_ID_INPLACE           (RENG_TEXGEN_FORMAT_DESC_ID_MASK << RENG_TEXGEN_FORMAT_DESC_ID_SHIFT)

/*
 * Third word of the texture packet; the fourth is the border colour.
 */
/* word TEXGEN_LOD: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg */
#define RENG_TEXGEN_LOD_WIDTH                        32 /* inferred */
#define RENG_TEXGEN_LOD_MAX_LEVEL_SHIFT              28 /* inferred */
#define RENG_TEXGEN_LOD_MAX_LEVEL_MASK               0xfu
#define RENG_TEXGEN_LOD_MAX_LEVEL_INPLACE            (RENG_TEXGEN_LOD_MAX_LEVEL_MASK << RENG_TEXGEN_LOD_MAX_LEVEL_SHIFT)
#define RENG_TEXGEN_LOD_MIN_LEVEL_SHIFT              24 /* inferred */
#define RENG_TEXGEN_LOD_MIN_LEVEL_MASK               0xfu
#define RENG_TEXGEN_LOD_MIN_LEVEL_INPLACE            (RENG_TEXGEN_LOD_MIN_LEVEL_MASK << RENG_TEXGEN_LOD_MIN_LEVEL_SHIFT)
#define RENG_TEXGEN_LOD_LOD_FRAC_SIZE_SHIFT          21 /* inferred */
#define RENG_TEXGEN_LOD_LOD_FRAC_SIZE_MASK           0x7u
#define RENG_TEXGEN_LOD_LOD_FRAC_SIZE_INPLACE        (RENG_TEXGEN_LOD_LOD_FRAC_SIZE_MASK << RENG_TEXGEN_LOD_LOD_FRAC_SIZE_SHIFT)
#define RENG_TEXGEN_LOD_LOD_BIAS_SHIFT               11 /* inferred */
#define RENG_TEXGEN_LOD_LOD_BIAS_MASK                0x3ffu
#define RENG_TEXGEN_LOD_LOD_BIAS_INPLACE             (RENG_TEXGEN_LOD_LOD_BIAS_MASK << RENG_TEXGEN_LOD_LOD_BIAS_SHIFT)

/*
 * Second word of the texture packet.
 */
/* word TEXGEN_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg */
#define RENG_TEXGEN_MODE_WIDTH                       32 /* inferred */
#define RENG_TEXGEN_MODE_MIN_FILTER_SHIFT            29 /* inferred */
#define RENG_TEXGEN_MODE_MIN_FILTER_MASK             0x7u
#define RENG_TEXGEN_MODE_MIN_FILTER_INPLACE          (RENG_TEXGEN_MODE_MIN_FILTER_MASK << RENG_TEXGEN_MODE_MIN_FILTER_SHIFT)
#define RENG_TEXGEN_MODE_MAG_FILTER_SHIFT            28 /* inferred */
#define RENG_TEXGEN_MODE_MAG_FILTER_MASK             0x1u
#define RENG_TEXGEN_MODE_MAG_FILTER_INPLACE          (RENG_TEXGEN_MODE_MAG_FILTER_MASK << RENG_TEXGEN_MODE_MAG_FILTER_SHIFT)
#define RENG_TEXGEN_MODE_WRAP_T_SHIFT                26 /* inferred */
#define RENG_TEXGEN_MODE_WRAP_T_MASK                 0x3u
#define RENG_TEXGEN_MODE_WRAP_T_INPLACE              (RENG_TEXGEN_MODE_WRAP_T_MASK << RENG_TEXGEN_MODE_WRAP_T_SHIFT)
#define RENG_TEXGEN_MODE_WRAP_S_SHIFT                24 /* inferred */
#define RENG_TEXGEN_MODE_WRAP_S_MASK                 0x3u
#define RENG_TEXGEN_MODE_WRAP_S_INPLACE              (RENG_TEXGEN_MODE_WRAP_S_MASK << RENG_TEXGEN_MODE_WRAP_S_SHIFT)
#define RENG_TEXGEN_MODE_MAP_HEIGHT_SHIFT            20 /* inferred */
#define RENG_TEXGEN_MODE_MAP_HEIGHT_MASK             0xfu
#define RENG_TEXGEN_MODE_MAP_HEIGHT_INPLACE          (RENG_TEXGEN_MODE_MAP_HEIGHT_MASK << RENG_TEXGEN_MODE_MAP_HEIGHT_SHIFT)
#define RENG_TEXGEN_MODE_MAP_WIDTH_SHIFT             16 /* inferred */
#define RENG_TEXGEN_MODE_MAP_WIDTH_MASK              0xfu
#define RENG_TEXGEN_MODE_MAP_WIDTH_INPLACE           (RENG_TEXGEN_MODE_MAP_WIDTH_MASK << RENG_TEXGEN_MODE_MAP_WIDTH_SHIFT)

/*
 * Transform mode word , the draw flags word.
 */
/* word XFORM_MODE: 32 bits; SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver */
#define RENG_XFORM_MODE_WIDTH                        32 /* inferred */
#define RENG_XFORM_MODE_EN_VIEWPORT_XFORM_SHIFT      31 /* inferred */
#define RENG_XFORM_MODE_EN_VIEWPORT_XFORM_MASK       0x1u
#define RENG_XFORM_MODE_EN_VIEWPORT_XFORM_INPLACE    (RENG_XFORM_MODE_EN_VIEWPORT_XFORM_MASK << RENG_XFORM_MODE_EN_VIEWPORT_XFORM_SHIFT)
#define RENG_XFORM_MODE_EN_MODEL_XFORM_SHIFT         30 /* inferred */
#define RENG_XFORM_MODE_EN_MODEL_XFORM_MASK          0x1u
#define RENG_XFORM_MODE_EN_MODEL_XFORM_INPLACE       (RENG_XFORM_MODE_EN_MODEL_XFORM_MASK << RENG_XFORM_MODE_EN_MODEL_XFORM_SHIFT)
#define RENG_XFORM_MODE_EN_TEX_XFORM_SHIFT           29 /* inferred */
#define RENG_XFORM_MODE_EN_TEX_XFORM_MASK            0x1u
#define RENG_XFORM_MODE_EN_TEX_XFORM_INPLACE         (RENG_XFORM_MODE_EN_TEX_XFORM_MASK << RENG_XFORM_MODE_EN_TEX_XFORM_SHIFT)
#define RENG_XFORM_MODE_EN_CLIP_CHECK_SHIFT          28 /* inferred */
#define RENG_XFORM_MODE_EN_CLIP_CHECK_MASK           0x1u
#define RENG_XFORM_MODE_EN_CLIP_CHECK_INPLACE        (RENG_XFORM_MODE_EN_CLIP_CHECK_MASK << RENG_XFORM_MODE_EN_CLIP_CHECK_SHIFT)

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
