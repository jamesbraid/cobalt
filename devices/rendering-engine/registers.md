<!-- generated from devices/rendering-engine/registers.toml by defs-gen - do not edit, change the source -->
<!-- SPDX-License-Identifier: BSD-3-Clause -->
<!-- defs-gen 2 -->
<!-- input sha256 b9b418e921d382c9e05469ae1e7fceea13fce3301107ab67b40c20ebb695e1b2 -->

# Rendering engine register window

The registers of the rendering engine window that the Windows NT 4.0 kernel driver reads or writes directly, and the 32-bit payload words of the packets the drivers send. The window is the range the driver maps; offsets below are from its base. Access records what the drivers do to the register at its own address, directly: a register that only a packet writes shows the direct accesses only (the scissor pair is "r" for that reason). Nothing here is a reset value, and no register is read before it is written.

Unit `rendering_engine`, macro prefix `RENG`, register block `RENG`.
Default source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database.

Every item has a grade: established, inferred or unknown. A grade says how the item was derived from its source; it does not claim the item was measured on silicon.

## Registers

| Register | Offset | Access | Width | Count | Space | Reset | Grade | Source |
|---|---|---|---|---|---|---|---|---|
| `tlb_entry` | 0x8000 | w | 32 | 2048 |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets |
| `tlb_desc` | 0xc000 | w | 32 | 64 |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database |
| `exec_ctl` | 0x10000 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines |
| `doorbell` | 0x10008 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routines that queue work or resume a suspended context |
| `int_cause` | 0x10018 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines |
| `int_enable` | 0x10020 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up |
| `scissor_min` | 0x18138 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |
| `scissor_max` | 0x18140 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |
| `chain_next` | 0x185c0 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, stop-event routine, and the chain packet the OpenGL driver ends a node with |
| `chain_this` | 0x185c8 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch and stop-event routines |
| `tex_id` | 0x185d0 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch, stop-event and bind routines; the OpenGL driver's texture-free packet |
| `ctx_dump` | 0x18800 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch |

### Register `tlb_entry`

Translation table: one entry per tile of the buffers the engine addresses. A tile is 1, 4, 16, 64 or 256 pages of 4 KB, as the buffer's descriptor says; the packet pool is 16 tiles of 256 pages, so it fills 16 entries. Only the low dword is ever written, with the tile's first 4 KB page frame number masked to 24 bits; entries not in use hold the first entry's value. The packet pool, the display buffers and the textures are named through these entries, so the engine fetches packets through this table rather than by physical address.

Array of 2048 registers, stride 8.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:0 | `page_frame` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets | 4 KB page frame number of the first page of the tile |

### Register `tlb_desc`

Buffer descriptor table: entry k describes the buffer whose descriptor id is k, the six-bit id the colour-format, depth-format and texture-format words carry. Entry 0 is the packet pool. The low dword only is written. A tiled buffer is a run of tiles of 128 or 512 bytes in a row, with tiles_per_row tiles per row, whose pages start at the translation-table slot in offset; a user buffer mapped linearly sets bit 19 (tiles_per_row 32) with the page-size bits kept and the packet pool sets page_type and tiles_per_row 1.

Array of 64 registers, stride 8.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:21 | `offset` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database |  |
| 20:14 | `tiles_per_row` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database |  |
| 13 | `tile_width` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database | 0 for a 128-byte tile, 1 for 512 bytes |
| 12:10 | `page_size` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database | log base 4 of the pages in one tile: 1, 4, 16, 64 or 256 pages give 0 to 4 |
| 9 | `page_type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; type CoTlbDescReg in the OpenGL driver's program database | set for the packet pool; the driver sets it when the tile width is neither 128 nor 512 |

### Register `exec_ctl`

Starts the engine on a packet position and reports where it is. A write is the packet position (in 32-byte units from the pool base) shifted to bit 12, bit 11 and the idle count; the driver writes 2 as the idle count with every start and 0x40 with 0xfffff in the position field at start-up. A read returns the position in bits 31:12, and zero there means none, in which case the driver reads the chain register instead.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `pkt_pos` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines |  |
| 11 | `start` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines | set in every write that starts or restarts a node, clear at start-up |
| 10:0 | `idle_count` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines | the driver writes 2 here with every start and keeps the value in a field it names as the packet idle count; what the engine does with the count is unknown |

### Register `doorbell`

Written with a single bit from three routines. Two write it when work is appended to a non-deferred node or a timer event fires, with the current queue at 2, a node in the hardware and that node's chunk count 0. The third, which suspends a node, writes it when the node's status bits 3:1 are clear, its chunk count is below 1, status bit 9 is clear and the status byte above bit 7 is non-zero.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 10 | `kick` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routines that queue work or resume a suspended context |  |

### Register `int_cause`

The interrupt handler treats a read of zero as not its own, masks the value with the two cause bits, writes zero and waits for the masked bits to change. The context switch waits for bit 31 to read set and then writes every bit but 31 set.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `ctx_wait` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines | polled after the outgoing context's dump offset is written, before the next context starts |
| 30 | `cause_30` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines | second cause bit: gates status bit 5 of the stopped node, the chunk decrement and the wake of waiter kind 8 in the event routine |
| 29 | `cause_29` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines | first cause bit: queues the same event and wakes the same waiters bar kind 8 |

### Register `int_enable`

Written once, with both cause bits set.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 30 | `en_30` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up |  |
| 29 | `en_29` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up |  |

### Register `scissor_min`

Scissor minimum corner, read into the window-parameter packet when the node already in the hardware is restarted. The window offset follows from the scissor packet's address field.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:16 | `y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |  |
| 15:0 | `x` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |  |

### Register `scissor_max`

Scissor maximum corner, inclusive.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:16 | `y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |  |
| 15:0 | `x` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |  |

### Register `chain_next`

The first word of the chain packet. The stop-event routine reads it: the low 12 bits are the high half of a 24-bit sequence number and bits 31:12 a node position, which it falls back to when the execution register reads zero.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `node_pos` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, stop-event routine, and the chain packet the OpenGL driver ends a node with |  |
| 11:0 | `seq_hi` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, stop-event routine, and the chain packet the OpenGL driver ends a node with |  |

### Register `chain_this`

The second word of the chain packet. Bits 31:12 are the position of the node the engine stopped in and bits 11:0 the low half of the sequence number. The context switch writes zero to it when the incoming node is already the one in hardware.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `node_pos` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch and stop-event routines |  |
| 11:0 | `seq_lo` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch and stop-event routines |  |

### Register `tex_id`

The third word of the long chain packet. The kernel driver keeps a per-context current-texture id and writes it here when it starts the context and reads it back at a stop; the OpenGL driver zeroes it before it frees the pages of the bound texture.

### Register `ctx_dump`

Written with the outgoing context's dump offset (the field the driver names as its context-switch dump offset) when another context is started, before the new context's start word.

## Words

| Word | Width | Grade | Source |
|---|---|---|---|
| `BLEND_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoBlendModeReg, blend writer |
| `COLOR_FORMAT` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |
| `COLOR_TEST_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoColorTestModeReg, alpha-test writer |
| `DEPTH_TEST` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers |
| `DRAW_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |
| `FB_ADDR` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoFbAddrType, scissor and clear-rectangle writers |
| `LIGHT_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |
| `LINK_WORD` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |
| `LOGIC_OP` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoLogicOpReg, logic-operation writer |
| `PRIM_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |
| `STENCIL_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |
| `SZ_FORMAT` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoSzFormatReg, depth and stencil descriptor writers |
| `TEXENV_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoTexEnvModeReg, texture-environment writers |
| `TEXGEN_FORMAT` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers |
| `TEXGEN_LOD` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg |
| `TEXGEN_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |
| `XFORM_MODE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver |

### Word `BLEND_MODE`

First word of the blend packet; the second is the constant colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `dst_func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoBlendModeReg, blend writer |  |
| 27:24 | `src_func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoBlendModeReg, blend writer |  |
| 23:20 | `op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoBlendModeReg, blend writer |  |

### Word `COLOR_FORMAT`

Draw-buffer descriptor.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `linear` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |  |
| 30:27 | `color_type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |  |
| 26:25 | `buf_depth` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |  |
| 24:19 | `desc_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |  |
| 18 | `span_mode` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorFormatReg, draw-buffer descriptor writers |  |

### Word `COLOR_TEST_MODE`

First word of the alpha-test packet; the second is the reference colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `en_comp` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorTestModeReg, alpha-test writer |  |
| 27:25 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoColorTestModeReg, alpha-test writer |  |

### Word `DEPTH_TEST`

Depth test word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers |  |
| 28 | `init_occlusion_test` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers |  |
| 27 | `en_occlusion_test` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers |  |
| 26:23 | `equal_range` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDepthTestReg, depth-test writers |  |

### Word `DRAW_MODE`

Pixel-pipe mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `en_stencil_test` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 30 | `en_depth_mask` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 29 | `en_depth_test` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 28 | `en_depth_offset` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver | set when polygon offset is enabled for the primitive and the factor or units is non-zero |
| 27 | `en_color_discard` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver | the colour write mask is all zero |
| 26 | `en_color_mask` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 25 | `en_dither` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 24 | `en_logic_op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 23 | `en_blend` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 22 | `en_color_test` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 21 | `en_fog` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 20 | `en_color_matrix` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 19 | `en_pixel_xfer` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 18 | `en_texture` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 17 | `en_second_color` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 16 | `en_smooth_shade` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 15 | `en_opaq_stipple` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 14 | `en_poly_stipple` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 13 | `en_line_stipple` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver |  |
| 12:0 | `reserved` | unknown |  | SGI Cobalt Graphics Driver 5.2.4 type CoDrawModeReg, pixel-pipe mode-word writers in the OpenGL driver | no routine of the three drivers sets a bit here |

### Word `FB_ADDR`

A corner: x in the low half, y in the high half (signed 16-bit).

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:16 | `y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoFbAddrType, scissor and clear-rectangle writers |  |
| 15:0 | `x` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoFbAddrType, scissor and clear-rectangle writers |  |

### Word `LIGHT_MODE`

Lighting mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `two_sided` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 30 | `en_light3` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 29 | `en_light2` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 28 | `en_light1` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 27 | `en_light0` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 26 | `en_normalize` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 25 | `en_specular` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 24 | `spec_only` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |
| 23 | `en_lighting` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLightModeReg, lighting-path writers |  |

### Word `LINK_WORD`

Link word of a packet node. It sits in node dword 0 and the drivers write it through the second CPU window (base 0xc0000000) plus the node offset. Bit 0 set means the engine owns the node and clears it when done. The size field counts 32-byte units of data after the 32-byte header unit.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `next` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |  |
| 11:3 | `size` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |  |
| 2 | `sync` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |  |
| 1 | `intr` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |  |
| 0 | `ready` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPktDescType, packet link-word writers in both drivers |  |

### Word `LOGIC_OP`

Logic operation word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoLogicOpReg, logic-operation writer |  |

### Word `PRIM_MODE`

Primitive mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `prim_type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 28 | `aa_line` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 27 | `closed_line` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 26:15 | `line_width` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 14:13 | `cull_face` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 12 | `front_face` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 11 | `back_face_line` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 10:9 | `prim_dir` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 8 | `sel_prim_dir` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |
| 7:0 | `reserved` | unknown |  | SGI Cobalt Graphics Driver 5.2.4 type CoPrimModeReg, primitive-mode writers in the OpenGL driver |  |

### Word `STENCIL_MODE`

First word of the stencil packet.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `dppass` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |
| 27:24 | `dpfail` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |
| 23:20 | `sfail` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |
| 19:16 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |
| 15:8 | `mask` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |
| 7:0 | `ref` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoStencilModeReg, stencil writers |  |

### Word `SZ_FORMAT`

Depth and stencil buffer descriptor.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `sz_type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoSzFormatReg, depth and stencil descriptor writers |  |
| 30:29 | `buf_depth` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoSzFormatReg, depth and stencil descriptor writers |  |
| 28:23 | `desc_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoSzFormatReg, depth and stencil descriptor writers |  |

### Word `TEXENV_MODE`

First word of the two-word texture-environment packet; the second is the environment colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexEnvModeReg, texture-environment writers |  |
| 28:25 | `type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexEnvModeReg, texture-environment writers |  |

### Word `TEXGEN_FORMAT`

First word of the four-word texture packet; the display driver's YUV blit sends 14 or 15 as the texture type.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `linear` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers |  |
| 30:27 | `tex_type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers |  |
| 26:25 | `buf_depth` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers |  |
| 24:19 | `desc_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenFormatReg, texture-parameter writers in both drivers |  |

### Word `TEXGEN_LOD`

Third word of the texture packet; the fourth is the border colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `max_level` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg |  |
| 27:24 | `min_level` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg |  |
| 23:21 | `lod_frac_size` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg |  |
| 20:11 | `lod_bias` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenLodReg |  |

### Word `TEXGEN_MODE`

Second word of the texture packet.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `min_filter` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |
| 28 | `mag_filter` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |
| 27:26 | `wrap_t` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |
| 25:24 | `wrap_s` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |
| 23:20 | `map_height` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |
| 19:16 | `map_width` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoTexGenModeReg |  |

### Word `XFORM_MODE`

Transform mode word , the draw flags word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `en_viewport_xform` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver |  |
| 30 | `en_model_xform` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver |  |
| 29 | `en_tex_xform` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver | cleared on both paths that leave bits 31:30 set; no routine sets it |
| 28 | `en_clip_check` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 type CoXformModeReg, draw-flags writers in the OpenGL driver |  |

## Constants

| Constant | Value | Grade | Source | Description |
|---|---|---|---|---|
| `BLEND_CONSTANT_ALPHA` | 0xa | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_CONSTANT_COLOR` | 0x8 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_DST_ALPHA` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | the driver sends 1 instead when the buffer has no alpha |
| `BLEND_ONE` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_ONE_MINUS_CONSTANT_ALPHA` | 0xb | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_ONE_MINUS_CONSTANT_COLOR` | 0x9 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_ONE_MINUS_DST_ALPHA` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | the driver sends 0 instead when the buffer has no alpha |
| `BLEND_ONE_MINUS_OTHER_COLOR` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_ONE_MINUS_SRC_ALPHA` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OP_ADD` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OP_MAX` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OP_MIN` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OP_REVERSE_SUBTRACT` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OP_SUBTRACT` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_OTHER_COLOR` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | destination colour in src_func, source colour in dst_func |
| `BLEND_SRC_ALPHA` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `BLEND_SRC_ALPHA_SATURATE` | 0xc | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | src_func only |
| `BLEND_ZERO` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_ALWAYS` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_EQUAL` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_GEQUAL` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_GREATER` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_LEQUAL` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_LESS` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_NEVER` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `CMP_NOTEQUAL` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `FILTER_LINEAR` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter and mag_filter |
| `FILTER_LINEAR_MIP_LINEAR` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter only |
| `FILTER_LINEAR_MIP_NEAREST` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter only |
| `FILTER_NEAREST` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter and mag_filter |
| `FILTER_NEAREST_MIP_LINEAR` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter only |
| `FILTER_NEAREST_MIP_NEAREST` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | min_filter only |
| `STENCIL_DECR` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `STENCIL_INCR` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `STENCIL_INVERT` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `STENCIL_KEEP` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `STENCIL_REPLACE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `STENCIL_ZERO` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_ALPHA` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_INTENSITY` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_LUMINANCE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_LUMINANCE_ALPHA` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_RGB` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXCLASS_RGBA` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXENV_ADD` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXENV_BLEND` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXENV_DECAL` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXENV_MODULATE` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXENV_REPLACE` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `TEXTYPE_ALPHA_OR_INTENSITY8` | 0x8 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 8-bit alpha or intensity, depth 0 |
| `TEXTYPE_INDEX8` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 8-bit colour index, depth 0; display driver colour format word 0 |
| `TEXTYPE_LUMINANCE8` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 8-bit luminance, depth 0 |
| `TEXTYPE_LUMINANCE_ALPHA8` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 16 bits, luminance and alpha, depth 1 |
| `TEXTYPE_RGB5A1` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 16 bits, 5-5-5-1, depth 1 |
| `TEXTYPE_RGBA4` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 16 bits, 4-4-4-4, depth 1 |
| `TEXTYPE_RGBA8` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 32 bits, 8 per component, depth 2 |
| `TEXTYPE_RGBA8_ALT` | 0x5 | unknown | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | 32 bits, depth 2, written for host transfer sources; how it differs from type 4 is unknown |
| `TEXTYPE_YUV_A` | 0xe | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | YUV 4:2:2 form, depth 1; the display driver's YUV blit |
| `TEXTYPE_YUV_B` | 0xf | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | the other YUV 4:2:2 form, depth 1 |
| `WINDOW_PHYS` | 0xc8000000 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `WINDOW_SIZE` | 0x19000 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `WRAP_CLAMP` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database | uses the border colour |
| `WRAP_CLAMP_TO_EDGE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
| `WRAP_REPEAT` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the type declarations in the driver's program database |  |
