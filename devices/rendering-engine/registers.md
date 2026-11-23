<!-- generated from devices/rendering-engine/registers.toml by defs-gen - do not edit, change the source -->
<!-- SPDX-License-Identifier: BSD-3-Clause -->
<!-- defs-gen 2 -->
<!-- input sha256 40f341e6f3e5ecb9130c295b7acf23a6bcfe5c990967fc3b5f7bc2ac1002cb50 -->

# Rendering engine register window

The registers of the rendering engine window that the Windows NT 4.0 kernel driver reads or writes directly, and the 32-bit payload words of the packets the drivers send. The window is the range the driver maps; offsets below are from its base. Access records what the drivers do to the register at its own address, directly: a register that only a packet writes shows the direct accesses only (the scissor pair is "r" for that reason). Nothing here is a reset value, and no register is read before it is written.

Unit `rendering_engine`, macro prefix `RENG`, register block `RENG`.
Default source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations.

Every item has a grade: established, inferred or unknown. A grade says how the item was derived from its source; it does not claim the item was measured on silicon.

## Registers

| Register | Offset | Access | Width | Count | Space | Reset | Grade | Source |
|---|---|---|---|---|---|---|---|---|
| `tlb_entry` | 0x8000 | w | 32 | 2048 |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets |
| `buffer_desc` | 0xc000 | w | 32 | 64 |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers |
| `exec_ctl` | 0x10000 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines |
| `doorbell` | 0x10008 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routines that queue work or resume a suspended context |
| `int_cause` | 0x10018 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, interrupt handler, context switch and stop-event routines |
| `int_enable` | 0x10020 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up |
| `scissor_min` | 0x18138 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |
| `scissor_max` | 0x18140 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, the routine that builds the window-parameter packet; the OpenGL driver's scissor packet |
| `chain_next` | 0x185c0 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, stop-event routine, and the chain packet the OpenGL driver ends a node with |
| `chain_this` | 0x185c8 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch and stop-event routines |
| `bound_texture_id` | 0x185d0 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch, stop-event and bind routines; the OpenGL driver's texture-free packet |
| `ctx_dump` | 0x18800 | w | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 kernel driver, context switch |

### Register `tlb_entry`

Translation table: one entry per tile of the buffers the engine addresses. A tile is 1, 4, 16, 64 or 256 pages of 4 KB, as the buffer's descriptor says; the packet pool is 16 tiles of 256 pages, so it fills 16 entries. Only the low dword is ever written, with the tile's first 4 KB page frame number masked to 24 bits; entries not in use hold the first entry's value. The packet pool, the display buffers and the textures are named through these entries, so the engine fetches packets through this table rather than by physical address.

Array of 2048 registers, stride 8.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:0 | `page_frame` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets | 4 KB page frame number of the first page of the tile |

### Register `buffer_desc`

Buffer descriptor table: entry k describes the buffer whose descriptor id is k, the six-bit id the colour-format, depth-format and texture-format words carry. Entry 0 is the packet pool. The low dword only is written. A tiled buffer is a run of tiles of 128 or 512 bytes in a row, with row_tiles tiles per row, whose pages start at the translation-table slot named by slot; a user buffer mapped linearly sets bit 19 (row_tiles 32) with the tile_pages_log4 bits kept, and the packet pool sets pool_flag and row_tiles 1.

Array of 64 registers, stride 8.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:21 | `slot` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers | first translation-table slot of the buffer |
| 20:14 | `row_tiles` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers |  |
| 13 | `tile_size_sel` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers | 0 for a 128-byte tile, 1 for 512 bytes |
| 12:10 | `tile_pages_log4` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers | log base 4 of the pages in one tile: 1, 4, 16, 64 or 256 pages give 0 to 4 |
| 9 | `pool_flag` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up and the translation-table packets; the OpenGL driver's buffer-descriptor writers | set for the packet pool; the driver sets it when the tile width is neither 128 nor 512 |

### Register `exec_ctl`

Starts the engine on a packet position and reports where it is. A write is the packet position (in 32-byte units from the pool base) shifted to bit 12, bit 11 and the idle count; the driver writes 2 as the idle count with every start and 0x40 with 0xfffff in the position field at start-up. A read returns the position in bits 31:12, and zero there means none, in which case the driver reads the chain register instead.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `pkt_pos` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines |  |
| 11 | `start` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines | set in every write that starts or restarts a node, clear at start-up |
| 10:0 | `idle_wait` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 kernel driver, hardware start-up, context switch and stop-event routines | the driver writes 2 here with every start and keeps the value in a per-adapter setting; what the engine does with the count is unknown |

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

### Register `bound_texture_id`

The third word of the long chain packet. The kernel driver keeps a per-context current-texture id and writes it here when it starts the context and reads it back at a stop; the OpenGL driver zeroes it before it frees the pages of the bound texture.

### Register `ctx_dump`

Written with the outgoing context's dump offset (a per-context field the driver keeps for the purpose) when another context is started, before the new context's start word.

## Words

| Word | Width | Grade | Source |
|---|---|---|---|
| `ALPHA_TEST_CONFIG` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 alpha-test writer |
| `BLEND_CONFIG` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 blend writer |
| `COLOR_BUFFER_DESC` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |
| `CORNER_XY` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 scissor and clear-rectangle writers |
| `DEPTH_COMPARE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 depth-test writers |
| `DEPTH_STENCIL_BUFFER_DESC` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 depth and stencil descriptor writers |
| `LIGHTING_FLAGS` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |
| `NODE_LINK` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |
| `PIXEL_PIPE_FLAGS` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |
| `PRIMITIVE_SETUP` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |
| `RASTER_LOGIC` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 logic-operation writer |
| `STENCIL_CONFIG` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 stencil writers |
| `TEXTURE_APPLY` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 texture-environment writers |
| `TEXTURE_FORMAT_WORD` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |
| `TEXTURE_LOD_RANGE` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |
| `TEXTURE_SAMPLING` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |
| `TRANSFORM_FLAGS` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver |

### Word `ALPHA_TEST_CONFIG`

First word of the alpha-test packet; the second is the reference colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `component_enables` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 alpha-test writer |  |
| 27:25 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 alpha-test writer |  |

### Word `BLEND_CONFIG`

First word of the blend packet; the second is the constant colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `dst_factor` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 blend writer |  |
| 27:24 | `src_factor` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 blend writer |  |
| 23:20 | `op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 blend writer |  |

### Word `COLOR_BUFFER_DESC`

Draw-buffer descriptor.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `linear` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |  |
| 30:27 | `pixel_kind` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |  |
| 26:25 | `size_class` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |  |
| 24:19 | `buffer_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |  |
| 18 | `span_flag` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-buffer descriptor writers |  |

### Word `CORNER_XY`

A corner: x in the low half, y in the high half (signed 16-bit).

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:16 | `y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 scissor and clear-rectangle writers |  |
| 15:0 | `x` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 scissor and clear-rectangle writers |  |

### Word `DEPTH_COMPARE`

Depth test word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth-test writers |  |
| 28 | `occlusion_reset` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth-test writers |  |
| 27 | `occlusion_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth-test writers |  |
| 26:23 | `equal_window` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth-test writers |  |

### Word `DEPTH_STENCIL_BUFFER_DESC`

Depth and stencil buffer descriptor.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `kind_flag` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth and stencil descriptor writers |  |
| 30:29 | `size_class` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth and stencil descriptor writers |  |
| 28:23 | `buffer_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 depth and stencil descriptor writers |  |

### Word `LIGHTING_FLAGS`

Lighting mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `two_sided` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 30 | `light3_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 29 | `light2_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 28 | `light1_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 27 | `light0_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 26 | `normalize_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 25 | `specular_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 24 | `specular_only` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |
| 23 | `lit_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 lighting-path writers |  |

### Word `NODE_LINK`

Link word of a packet node. It sits in node dword 0 and the drivers write it through the second CPU window (base 0xc0000000) plus the node offset. Bit 0 set means the engine owns the node and clears it when done. The size field counts 32-byte units of data after the 32-byte header unit.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:12 | `next` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |  |
| 11:3 | `size` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |  |
| 2 | `sync` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |  |
| 1 | `interrupt` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |  |
| 0 | `ready` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 packet link-word writers in both drivers |  |

### Word `PIXEL_PIPE_FLAGS`

Pixel-pipe mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `stencil_test_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 30 | `depth_write_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 29 | `depth_test_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 28 | `depth_offset_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver | set when polygon offset is enabled for the primitive and the factor or units is non-zero |
| 27 | `color_discard_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver | the colour write mask is all zero |
| 26 | `color_mask_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 25 | `dither_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 24 | `logic_op_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 23 | `blend_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 22 | `alpha_test_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 21 | `fog_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 20 | `matrix_convert_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 19 | `pixel_transfer_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 18 | `texture_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 17 | `secondary_color_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 16 | `smooth_shade_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 15 | `opaque_stipple_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 14 | `polygon_stipple_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 13 | `line_stipple_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver |  |
| 12:0 | `reserved` | unknown |  | SGI Cobalt Graphics Driver 5.2.4 pixel-pipe mode-word writers in the OpenGL driver | no routine of the three drivers sets a bit here |

### Word `PRIMITIVE_SETUP`

Primitive mode word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `primitive_kind` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 28 | `line_antialias` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 27 | `line_loop` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 26:15 | `line_width` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 14:13 | `cull_face` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 12 | `front_face` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 11 | `back_face_as_line` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 10:9 | `direction_10_9` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 8 | `direction_select_8` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |
| 7:0 | `reserved` | unknown |  | SGI Cobalt Graphics Driver 5.2.4 primitive-mode writers in the OpenGL driver |  |

### Word `RASTER_LOGIC`

Logic operation word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 logic-operation writer |  |

### Word `STENCIL_CONFIG`

First word of the stencil packet.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `pass_op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |
| 27:24 | `depth_fail_op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |
| 23:20 | `stencil_fail_op` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |
| 19:16 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |
| 15:8 | `mask` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |
| 7:0 | `ref` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 stencil writers |  |

### Word `TEXTURE_APPLY`

First word of the two-word texture-environment packet; the second is the environment colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `func` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-environment writers |  |
| 28:25 | `type` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-environment writers |  |

### Word `TEXTURE_FORMAT_WORD`

First word of the four-word texture packet; the display driver's YUV blit sends 14 or 15 as the texture type.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `linear` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 30:27 | `texture_kind` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 26:25 | `size_class` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 24:19 | `buffer_id` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |

### Word `TEXTURE_LOD_RANGE`

Third word of the texture packet; the fourth is the border colour.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:28 | `max_level` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 27:24 | `min_level` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 23:21 | `frac_bits` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 20:11 | `lod_bias` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |

### Word `TEXTURE_SAMPLING`

Second word of the texture packet.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:29 | `min_filter` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 28 | `mag_filter` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 27:26 | `wrap_t` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 25:24 | `wrap_s` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 23:20 | `height_code` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |
| 19:16 | `width_code` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 texture-parameter writers in both drivers |  |

### Word `TRANSFORM_FLAGS`

Transform mode word , the draw flags word.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `viewport_xform_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver |  |
| 30 | `model_xform_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver |  |
| 29 | `tex_xform_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver | cleared on both paths that leave bits 31:30 set; no routine sets it |
| 28 | `clip_check_on` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 draw-flags writers in the OpenGL driver |  |

## Constants

| Constant | Value | Grade | Source | Description |
|---|---|---|---|---|
| `BLEND_CONSTANT_ALPHA` | 0xa | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_CONSTANT_COLOR` | 0x8 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_DST_ALPHA` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | the driver sends 1 instead when the buffer has no alpha |
| `BLEND_ONE` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_ONE_MINUS_CONSTANT_ALPHA` | 0xb | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_ONE_MINUS_CONSTANT_COLOR` | 0x9 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_ONE_MINUS_DST_ALPHA` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | the driver sends 0 instead when the buffer has no alpha |
| `BLEND_ONE_MINUS_OTHER_COLOR` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_ONE_MINUS_SRC_ALPHA` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OP_ADD` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OP_MAX` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OP_MIN` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OP_REVERSE_SUBTRACT` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OP_SUBTRACT` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_OTHER_COLOR` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | destination colour in src_factor, source colour in dst_factor |
| `BLEND_SRC_ALPHA` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `BLEND_SRC_ALPHA_SATURATE` | 0xc | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | src_factor only |
| `BLEND_ZERO` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_ALWAYS` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_EQUAL` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_GEQUAL` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_GREATER` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_LEQUAL` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_LESS` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_NEVER` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `CMP_NOTEQUAL` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `FILTER_LINEAR` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter and mag_filter |
| `FILTER_LINEAR_MIP_LINEAR` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter only |
| `FILTER_LINEAR_MIP_NEAREST` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter only |
| `FILTER_NEAREST` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter and mag_filter |
| `FILTER_NEAREST_MIP_LINEAR` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter only |
| `FILTER_NEAREST_MIP_NEAREST` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | min_filter only |
| `STENCIL_DECR` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `STENCIL_INCR` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `STENCIL_INVERT` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `STENCIL_KEEP` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `STENCIL_REPLACE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `STENCIL_ZERO` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_ALPHA` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_INTENSITY` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_LUMINANCE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_LUMINANCE_ALPHA` | 0x5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_RGB` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXCLASS_RGBA` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXENV_ADD` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXENV_BLEND` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXENV_DECAL` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXENV_MODULATE` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXENV_REPLACE` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `TEXTYPE_ALPHA_OR_INTENSITY8` | 0x8 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 8-bit alpha or intensity, depth 0 |
| `TEXTYPE_INDEX8` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 8-bit colour index, depth 0; display driver colour format word 0 |
| `TEXTYPE_LUMINANCE8` | 0x6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 8-bit luminance, depth 0 |
| `TEXTYPE_LUMINANCE_ALPHA8` | 0x7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 16 bits, luminance and alpha, depth 1 |
| `TEXTYPE_RGB5A1` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 16 bits, 5-5-5-1, depth 1 |
| `TEXTYPE_RGBA4` | 0x3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 16 bits, 4-4-4-4, depth 1 |
| `TEXTYPE_RGBA8` | 0x4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 32 bits, 8 per component, depth 2 |
| `TEXTYPE_RGBA8_ALT` | 0x5 | unknown | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | 32 bits, depth 2, written for host transfer sources; how it differs from type 4 is unknown |
| `TEXTYPE_YUV_A` | 0xe | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | YUV 4:2:2 form, depth 1; the display driver's YUV blit |
| `TEXTYPE_YUV_B` | 0xf | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | the other YUV 4:2:2 form, depth 1 |
| `WINDOW_PHYS` | 0xc8000000 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `WINDOW_SIZE` | 0x19000 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `WRAP_CLAMP` | 0x1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations | uses the border colour |
| `WRAP_CLAMP_TO_EDGE` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
| `WRAP_REPEAT` | 0x0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, kernel driver and display driver code, with the bit layouts of the register words checked against the driver's type declarations |  |
