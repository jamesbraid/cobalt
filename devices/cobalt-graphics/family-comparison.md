# Cobalt graphics against the other SGI graphics families

Whether the Visual Workstation's rendering engine (Cobalt) and display back end (Arsenic) share a design with SGI's other
graphics families, and what a model of those families can be reused for. Back to the [overview](README.md).

Every row cites a page of this repository and a page of the other family's repository. Grades are this repository's three
([AGENTS.md](../../AGENTS.md)). The comparison is between two sets of readings, so the grades are about the comparison: a value
that both sides write is **established** as a match, because each side's page establishes its own value; that the two come
from one design, or that the hardware treats them alike, is at best **inferred**. Anything argued from a name, a layout that
looks alike or the shape of a driver is **inferred**; where only one side is known it is **unknown**.

## Short answer

| Question | Answer | Grade |
|---|---|---|
| Is Arsenic a display chip of another family? | It matches the O2's GBE register for register wherever both are known, carries the same identity word, and reports the revision number the O2's second-generation GBE reports. Treat it as a GBE revision 2 variant. | match established; same design inferred |
| Does the engine share a design with another family? | With CRIME, the O2's engine: the same state-word codes, the same feature set in its mode word, five clip rectangles and a clip-ID plane, the same 512-byte by 128-line frame tile, the same naming of the driver's per-board back end. Not the transport, not the translation table, not the context-switch method. | codes established; shared design inferred |
| Any relation to IMPACT, VPro, InfiniteReality or the ATI parts? | Only in ideas (a table of page frames, a context save into host memory, a chained command list). No format or code matches. | inferred (weak) |
| Could one scan-out model serve both? | Yes for the GBE-shaped block, with five extensions to add. | inferred |
| Could one engine model serve both? | No. A CRIME engine model can supply the pixel pipeline's codes and arithmetic, but Cobalt needs its own front end (packet queue, translation table, geometry stage, context switch). | inferred |

## What was read

The Cobalt pages of this repository, and for the others the pages (and the generated register tables and annotated headers)
of [crime](https://github.com/jamesbraid/crime) (O2 CRIME, GBE and MTE), [gr4](https://github.com/jamesbraid/gr4) on its
`origami-re` branch (IMPACT and MGRAS), [odsy](https://github.com/jamesbraid/odsy) (VPro),
[kona](https://github.com/jamesbraid/kona) (InfiniteReality), [voyager](https://github.com/jamesbraid/voyager) (the ATI R300
parts of the Onyx4), [psitech](https://github.com/jamesbraid/psitech) (the RAD4 board) and
[chips](https://github.com/jamesbraid/chips) (the Cobalt memory controller and HAL pages). Only pages and register tables
were used; the vendor-derived listings that some of those repositories also carry were not.

Not compared:

- **GR2 (Express graphics).** No repository of ours holds it. The READMEs of this repository and of chips only credit a public
  workspace, which was not read. Every GR2 row would be **unknown**.
- **PsiTech RAD4.** A frame buffer card with a PLX bus bridge and an IBM RAMDAC and no draw engine
  ([README](https://github.com/jamesbraid/psitech/blob/main/README.md)). It has nothing in common with the Cobalt engine and
  its RAMDAC is a PC part, not a GBE.
- **Voyager (ATI R300 and R350).** A PC design whose OpenGL layer is the vendor's own
  ([README](https://github.com/jamesbraid/voyager/blob/main/README.md)). It shares no format with Cobalt (below); it appears
  only as a contrast.

## 1. Packet and command formats

The shapes side by side:

| | Cobalt | CRIME (O2) | IMPACT (Octane, Indigo2) | VPro (Octane2) | InfiniteReality (Onyx2) | ATI R300 (Onyx4) |
|---|---|---|---|---|---|---|
| How work reaches the engine | packets in main memory, fetched by DMA; the host never writes engine registers except to start and kick ([graphics-engine.md](graphics-engine.md)) | host stores register address and data pairs into a mapped interface buffer; a store with the start-alias bit commits a primitive ([registers](https://github.com/jamesbraid/crime/blob/main/devices/re/registers.md), [opengl.md](https://github.com/jamesbraid/crime/blob/main/software/opengl/opengl.md)) | 32-bit stores to a command port; display lists are DMA from host memory ([gl-commands.md](https://github.com/jamesbraid/gr4/blob/origami-re/software/opengl/gl-commands.md), [HQ3.h](https://github.com/jamesbraid/gr4/blob/origami-re/devices/hq3/HQ3.h)) | 32-bit command words to a FIFO, DMA by descriptor ([words.md](https://github.com/jamesbraid/odsy/blob/main/devices/buzz/words.md), [dma](https://github.com/jamesbraid/odsy/blob/main/devices/dma/README.md)) | command tokens, DMA by kick command ([tokens](https://github.com/jamesbraid/kona/blob/main/formats/token/format.md), [hip](https://github.com/jamesbraid/kona/blob/main/devices/hip/README.md)) | ring buffer with write and read pointers ([r300-kernel-dma.md](https://github.com/jamesbraid/voyager/blob/main/software/kernel/r300-kernel-dma.md)) |
| Header word | class in 31:30, window address in 31:21, engine register index in 20:12, dword count in 11:5, instance in 4:0 | none: the register offset is the address | class in 31:16 or an inline-block bit, opcode in 15:8, payload bytes in 7:0 | slot, operand count and opcode packed in the word | token number plus operands | type and count fields of the public PM4 format (not decoded in the voyager pages) |
| Completion | ownership bit in each link word, engine clears it; two interrupt causes | status word bits (idle, FIFO pointers), FIFO-level interrupts | status and flag words, polled | polled and interrupt | completion event | read-pointer write-back |

Findings:

- **Same register-file idea, different carrier.** Cobalt's header addresses a register file by index and carries a count, so a
  packet is a block write into engine registers. CRIME writes the same kind of register file one register at a time through
  its interface buffer. The register contents are alike (next bullet); the framing is not, and nothing in the Cobalt header
  repeats a CRIME offset or a bit field. Grade: framing differs, established as a comparison of two layouts; no shared opcode
  numbering found.
- **A mode word with the same feature set.** Cobalt's pixel-pipe mode word ([packet-set.md](packet-set.md#pixel-pipe-mode-word-0x80003020))
  and CRIME's `DrawMode` are the same kind of register: enables for texture, fog, alpha test, blend, logic operation, dither,
  smooth shading, polygon, line and opaque stipple, colour mask, depth test, depth mask and stencil, plus a pixel-transfer
  enable. The order and bit positions differ (Cobalt counts down from bit 31 with stencil first; only smooth shading sits at
  bit 16 in both), and both have an opaque-stipple enable (Cobalt's vendor field is `enOpaqStipple`; CRIME's is bit 17). Grade: inferred
  shared lineage; no bit-for-bit match beyond one bit. Source: [CRIME_RE.h](https://github.com/jamesbraid/crime/blob/main/devices/re/CRIME_RE.h), `DM_*` constants.
- **Per-primitive registers.** Both have a primitive word with a primitive type field and an edge or width field, a stipple
  register set (line repeat and pattern, polygon pattern), a scissor and a window origin. CRIME's primitive type values are
  point 0, line 1, triangle 2, rectangle 3; Cobalt's go in 31:29 as 1 lines, 2 line strips, 3 triangles, 4 strips, 5 fans
  ([draw-and-buffers.md](draw-and-buffers.md#primitive-mode-word)). Different. Grade: established difference.
- **Five clip rectangles and a clip-ID plane.** Cobalt's window-parameter packet is a thermometer-coded valid mask
  (vendor name `scrMaskMode`), five corner pairs (`scrMask`) and a clip-ID word (`cidMode`)
  ([packet-set.md](packet-set.md#window-parameter-packets)). CRIME has five screen-mask rectangles, a clip-mode word with
  per-mask enable and inside-select bits, and clip-ID test and map bits backed by a four-register table the public driver header calls clip-ID
  ([registers.toml](https://github.com/jamesbraid/crime/blob/main/devices/re/registers.toml), `scrmask` and `clipmode`;
  [memory.md](https://github.com/jamesbraid/crime/blob/main/devices/re/memory.md)). The count, the names and the idea agree; the encodings differ
  (Cobalt takes y in 31:16 and a reduced maximum, CRIME's mask words are 64 bits). Grade: inferred shared design.
- **Chaining.** Cobalt's link word (length 11:3, next position 31:12, interrupt and sync flags 2:1, owner bit 0) chains
  packet buffers; no return or call is seen. IMPACT chains display-list segments with jump, push and return commands and a table of
  32-bit page frame numbers ([HQ3.h](https://github.com/jamesbraid/gr4/blob/origami-re/devices/hq3/HQ3.h), display lists).
  Nothing to reuse; the only similarity is that both walk memory the host described by page frames. Grade: inferred,
  by analogy only.
- **Doorbell and ownership.** Cobalt's start write (`0x10000`, position in 31:12) and doorbell (`0x10008`) have no equivalent
  elsewhere. The nearest are CRIME's start alias (commit by writing a register at an offset) and the R300's write-pointer
  update. No ownership bit cleared by the engine appears in the other families' pages that were read. Grade: unknown whether any
  other SGI design used one.
- **Geometry in the engine.** Cobalt's engine can transform, light (up to four lights) and take vertex attributes on two
  vertex ports; the host does only what the driver's rules leave ([draw-and-buffers.md](draw-and-buffers.md#who-does-what)).
  CRIME has no geometry stage: the host transforms, lights and clips and sends window coordinates
  ([opengl.md](https://github.com/jamesbraid/crime/blob/main/software/opengl/opengl.md), first section). IMPACT, VPro and InfiniteReality all
  have geometry engines but take GL-level commands (vertex, normal, colour words), a model Cobalt's vertex-attribute packets
  resemble in outline only. Grade: established difference from CRIME.

## 2. Pixel and texture formats, ids, tiles, state codes

State codes are where the strongest match is. The values below were read from the same kind of place (the driver's setter, the
whole routine) on both sides.

| Item | Cobalt | CRIME | Verdict |
|---|---|---|---|
| Comparison function (alpha, depth, stencil) | GL enum less `0x200`: 0 never to 7 always | the same; depth func in 27:25, alpha in 11:8 | **value match**, field positions differ. Established |
| Blend equation | add 0, min 1, max 2, subtract 3, reverse subtract 4 | the same, bits 11:8 | **bit-for-bit match** of the codes. Established |
| Blend factors | zero 0, one 1, the other operand's colour 2 and 3, source alpha 4 and 5, destination alpha 6 and 7, constant colour 8 and 9, constant alpha 10 and 11, alpha saturate 12 (source only) | the same table, including the saturate code 12 and the swap of destination colour and source colour between source and destination fields | **bit-for-bit match.** Established. Cobalt packs destination 31:28, source 27:24, equation 23:20; CRIME packs equation 11:8, source 7:4, destination 3:0 |
| Logic operation | GL enum less `0x1500`, 16 codes | the same, bits 3:0 | **value match.** Established |
| Stencil operations | keep 0, zero 1, replace 2, incr 3, decr 4, invert 5 | the same enumeration | **value match**; the fields run in opposite order (Cobalt: pass, depth fail, stencil fail from bit 31 down, then function, read mask, reference; CRIME: reference, read mask, function, then stencil fail, depth fail, pass), so the whole word is mirrored. Established |
| Alpha reference | float times 255 plus 0.5, truncated | truncated, no half added | differ in rounding. Established |
| Minification filter | none 0, linear 1, nearest and linear mipmap 2 to 5 | same six codes, 9:7 | **value match.** Established |
| Magnification filter | one bit | one bit, bit 6 | match. Established |
| Texture environment | modulate 0, decal 1, blend 2, replace 3; add 4 added | modulate 0, decal 1, blend 2, replace 3 | **value match** for 0 to 3. Established |
| Wrap | repeat 0, clamp 1, clamp to edge 2 | clamp 0, repeat 1, clamp to edge 2 | repeat and clamp **swapped**; edge matches. Established |
| Texel type and depth | 4-bit type, 2-bit depth (0 8-bit, 1 16-bit, 2 32-bit) | 4-bit type, 2-bit depth (16 and 32 only, depth is log2 of bytes) | depth codes match for 16 and 32 bits. The type values do not: Cobalt 2 is RGB5A1, 3 is RGBA4, 4 is RGBA8, 6 luminance, 7 luminance-alpha, 8 alpha or intensity; CRIME 1 RGB, 2 RGBA, 4 RGBA4, 5 alpha, 6 intensity, 7 luminance, 8 luminance-alpha. Looks alike, **differs**. Established |
| Level of detail | max level 31:28, min level 27:24, three-bit fraction size | `maxLevel` 13:10, `mapLevel` 17:14 (a table slot), no fraction field | similar idea, different words. Inferred |
| Depth and stencil word | 16 or 24 bit depth; whether stencil shares the word with 24-bit depth is open | 24-bit depth in bits 23:0 and 8-bit stencil in 31:24 of one word, 25.12 stepper | CRIME's layout is a candidate for Cobalt's open question (section 6). Unknown for Cobalt |
| Colour pixel layouts | 8-bit index, 5551 with red in the top bits, 8888 red first | 8-bit index, 5551, 8888 (RGBA, red in 31:24 of a flat colour) | agree. Inferred (Cobalt's engine format is inferred from three writers) |
| Texture sizes and tiles | 128-byte or 512-byte tile rows; tile of 1, 4, 16, 64 or 256 pages set by the descriptor ([textures.md](textures.md#tile-geometry-of-a-level)) | one fixed tile: 64 KiB, 512 bytes by 128 rows, for frame buffers and textures; a fixed start table and a shared tile for levels of 64 texels or less ([memory.md](https://github.com/jamesbraid/crime/blob/main/devices/re/memory.md), texture level layout) | the 512 by 128 tile is shared; the variable tile and the 128-byte row are Cobalt's own. Both hold small levels in a shared tile (Cobalt: up to 16 by 16 in one 4 KB page; CRIME: 64 or fewer in one 64 KiB tile). Established as layouts; the engine's reading of either is inferred |
| Descriptor ids | six-bit descriptor id in 24:19 (colour, texture) and 28:23 (depth), indexing 64 descriptors | buffer mode words select a TLB group; texture slots are a fixed table | no match. Established |
| Translation table | 2048 entries of 8 bytes, each the 4 KB frame of a tile's first page, plus 64 descriptors | 64-bit registers holding four 16-bit entries (64 KiB tile numbers, bit 15 valid): three groups of 256 for the two colour buffers and the depth buffer, 28 registers (112 entries) for textures, and linear tables of 32-bit page frames | different structure and entry width. Established |
| Vertex coordinates | 1/64 pixel (the driver's `subpixelBits` is 6), floats | 19-bit fixed point, six sub-pixel bits, biased floats | same sub-pixel precision. Inferred (both are a driver constant) |

The other families do not match Cobalt in these tables:

- VPro's texture format index is a six-bit number in bits 25:20 of a level word, with 50 defined values and widths up to 16 bits
  per component ([texture-formats.md](https://github.com/jamesbraid/odsy/blob/main/devices/texture/texture-formats.md)). Cobalt has 4-bit type
  plus depth and no 12 or 16-bit components. No value agrees. Established.
- IMPACT's attribute headers carry component count and source type in the header word
  ([gl-commands.md](https://github.com/jamesbraid/gr4/blob/origami-re/software/opengl/gl-commands.md)); Cobalt's vertex packets carry an attribute mask in the
  instance bits. Same idea, no common value. Established as a difference.
- The R300's macro tiles are 2048 bytes of 32-byte micro blocks with a swizzle
  ([tiled-scanout.md](https://github.com/jamesbraid/voyager/blob/main/devices/display/tiled-scanout.md)). Nothing like the 512 by 128 tile. Established.
- No other family's pages describe a 128-byte tile row. Unknown whether any does.

Reading across the table: Cobalt and CRIME agree wherever the code is a GL enumeration with a fixed ordering (comparison
functions, blend equations and factors, logic operations, stencil operations, filters, environment modes). That is also what
the GL enumerants and the 1.2 specification's order would give a hardware designer who wanted a cheap decode, so the match
shows a shared convention at least; that the same team designed both engines is **inferred**, with the mode-word feature set,
the clip rectangles and the driver naming (section 5) as the other supports.

## 3. Display back end

**Arsenic is a GBE.** The pieces:

- Same register offsets for everything either driver touches: control and status `0x000`, dot clock `0x004`, monitor and
  panel I2C `0x008` and `0x010`, identity `0x014`, the timing block at `0x10000`, overlay, frame and DID control at `0x20000`,
  `0x30000` and `0x40000`, window mode table `0x48000`, colour map `0x50000`, colour-map FIFO `0x58000`, gamma `0x60000`,
  cursor `0x70000`, capture `0x80000`. Compare [display-engine.md](display-engine.md) with
  [registers.toml](https://github.com/jamesbraid/crime/blob/main/devices/gbe/registers.toml) (`OFF_*`). Established for the offsets, field by field where both
  drivers use them.
- Same identity word `0x666` at `+0x14`, accepted by both the O2's kernel probe and the NT miniport, and the same low-nibble
  chip revision in `+0x000`. The GBE 1.1 specification's part reads 1; the O2's IRIX tests for a value of 2 or more to pick the
  second-generation colour-map window tables (named with an `ar` prefix there), the 6144-entry colour map and a 63-deep
  colour-map FIFO ([GBE.h](https://github.com/jamesbraid/crime/blob/main/devices/gbe/GBE.h), [gbe.md](https://github.com/jamesbraid/crime/blob/main/devices/gbe/gbe.md)). The
  Cobalt side: the firmware reads that nibble, a real machine's registry shows it as revision 2 ("Display Engine: 002",
  [board-data.md](board-data.md)), the Linux header and the vendor's own declaration give a 6144-entry colour map, and the colour-map FIFO status that
  satisfies all four readers is `0x3f` ([display-engine.md](display-engine.md#colour-map-gamma-cursor-video-capture)). Revision, entry count and FIFO depth all
  point the same way. Grade: **inferred** that Arsenic is, or is a close sibling of, the O2's revision 2 GBE. The `ar`
  prefix against the NT driver's `as` prefix is a hint, not evidence.
- Same tile geometry: 512-byte rows by 128 lines, 64 KiB tiles, 16-bit tile-list entries that are the physical address shifted
  right by 16, a frame size word with whole tile columns in `[12:5]`, a 32-byte-unit remainder in `[4:0]`, depth in `[14:13]`
  and a FIFO-reset pulse, a shadow control word copied to a live twin at vertical blank, DMA enable in bit 0 polled by
  software. [crime frame registers](https://github.com/jamesbraid/crime/blob/main/devices/gbe/registers.toml) against [display-engine.md](display-engine.md#overlay-normal-plane-did). Established for
  the layout on both sides.
- Same cursor block (position, control, three colours at `0x70008`, a 64-dword glyph at `0x78000`, a 32 by 32 pixel image
  of two bits per pixel), same colour packing `R<<24 | G<<16 | B<<8`, same timing-window registers and the same
  scanline-interrupt pairs (`vt_intr01`, `vt_intr23`) that both drivers park at `0xfff`. Established for offsets and packing;
  the cursor's behaviour on Cobalt is inferred from the GBE because only the NT display driver writes it.
- Same I2C convention: bit 0 SDA, bit 1 SCL, a written 1 pulls the line low, a read of 1 means the line is low (both
  buses). The crime page grades the O2 convention inferred because the specification does not say; the Cobalt NT routines
  establish it for Cobalt. A model of one can take the other's rule.
- Same DID table: 32 window entries with buffer mode in `[1:0]` and pixel type in `[4:2]`. Cobalt lists types 0 to 7; the
  O2 page lists 0, 1, 2, 4 and 5, and type 3 (RGB4), 6 and 7 come only from the Linux header on the Cobalt side.

What differs, so one scan-out model needs these:

| Item | Cobalt (Arsenic) | O2 (GBE) | Grade |
|---|---|---|---|
| Linear frame buffer | `frm_control` bit 1, pointer in `[31:9]` is the buffer's address shifted right 9; the pitch is the tile-column count rounded up, times 512 bytes (firmware and the Windows 2000 drivers) | not in the published 1.1 pages; the GBE's pointer field is given as `[31:5]` | unknown for GBE; inferred for Arsenic |
| 10-bit gamma map | 1024 dwords at `0x68000`, written by the NT miniport | not in the GBE pages (256-entry gamma only) | meaning inferred; extension unknown for GBE |
| Colour map | 6144 entries | 4608 (revision 1.1) or 6144 (revision 2, inferred) | match with revision 2, inferred |
| Interrupt delivery | vertical retrace is a status bit in a separate 32-bit register at `0xca000000`, not in the block; the engine and retrace are two lines of Cobalt's own interrupt controller | GBE interrupts are inputs of the CRIME interrupt controller | the register is outside the GBE-shaped block, so which chip decodes it is unknown ([open-questions.md](../../questions/open-questions.md)) |
| Config bits | `0x01`, `0x02`, `0x04`, `0x08` flat panel, `0x20` little-endian, `0x40` tile history, `0x80` extended addressing | not compared field by field | unknown |
| Panel and EDID | flat-panel I2C at `0x010` with panel id on device `0x71` | the same register exists | the panel protocol on the O2 is not read; unknown |
| Pixel clock | PLL fields `M-1`, `N-1`, `P`, run bit 20, 27 MHz reference | the same register at `+0x04`: run bit 20 and `P` in 15:14 on both, and the O2's video format files carry the three PLL scalars `DacPll_M`, `_N`, `_P`; the GBE adds out-of-lock (bit 23) and out-of-range (bit 22) status bits | match of fields, inferred; the status bits are not read by any Cobalt driver |

**Other display chips.** IMPACT's VC3 has a 64 by 64 two-bit cursor engine, a timing generator and DID composition
([VC3.h](https://github.com/jamesbraid/gr4/blob/origami-re/devices/vc3/VC3.h)), VPro's PB&J sits behind a display FIFO with
its own address space ([README](https://github.com/jamesbraid/odsy/blob/main/devices/pbj/README.md)), InfiniteReality uses a
VOC with XMAP and DAC chips, and the R300 has CRTCs. None has Arsenic's memory-mapped layout. The one thing shared with VPro
is a name: its kernel calls the display side "DBE", the name Linux gives Cobalt's block. Grade: inferred, name only, no
register match.

**Could one scan-out model serve both?** Yes. The tiled fetch (tile list, depth, remainder, shadow copy, FIFO reset), the
timing window, DID table, colour map and FIFO, gamma, cursor and I2C pair are the same block. A model parameterised by
revision would need: the linear mode, the 10-bit gamma window, the `0x666` and revision-2 identity and a `bist` register
(the PROM writes 0 and polls bit 31, [prom-init.md](prom-init.md)), the `config` bits, a way to take a retrace line that is
not CRIME's, and a model of the capture block if video is wanted. All five extensions are inferred from the drivers;
the block's reset values are unknown on both sides.

## 4. Unified memory

Both put the frame buffer, depth buffer, textures and command data in main memory that the processors also use, and both
reach it through a table the host fills.

| | Cobalt | CRIME |
|---|---|---|
| Translation | engine window table of 2048 entries; entry `i` at `0x8000 + 8i`, the 24-bit 4 KB frame of a tile's first page; tile size (1 to 256 pages) comes from a descriptor ([graphics-engine.md](graphics-engine.md)) | TLB groups in the engine: 16-bit tile numbers (64 KiB tiles, bit 15 as the valid bit), 256 entries per frame buffer group, 112 texture entries, linear tables of 32-bit page frames ([memory.md](https://github.com/jamesbraid/crime/blob/main/devices/re/memory.md)) |
| Unused entries | filled with the first tile of the packet pool by the driver; no valid bit is used | zero, bit 15 clear; a request through an invalid entry is flagged and the memory access is suppressed (specification, as quoted in the same page) |
| Command fetch | by DMA from a pool of 16 tiles of 256 pages through the same table; a packet position is a pool position, not an address | none: host stores into the interface buffer; DMA only for transfers |
| Display fetch | by Arsenic through a tile list in memory (section 3) | by GBE through a tile list in memory |
| Memory controller | in the Cobalt system chip: ECC enable bit 0 of the first register; error status, address and syndrome at `+0x18`, `+0x28`, `+0x30`; 256-bit-wide memory with ECC (the width is inferred from users' posts; [chips cobalt page](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md), [HAL](https://github.com/jamesbraid/chips/blob/main/software/visws/nt-hal.md)) | in CRIME: ECC enable, error status `0x250`, address `0x258`, syndrome `0x260`, 256-bit memory word with 8 check bits per 64 data bits ([CRIME.h](https://github.com/jamesbraid/crime/blob/main/devices/crime/CRIME.h)) |
| Error classes | the HAL reports invalid address on read or write, hard ECC, soft ECC, multiple errors; the status bit meanings are inferred from those messages | invalid address (read, write, read-modify-write), ECC (read, read-modify-write), multiple, hard, soft, plus which client made the access (CPU, VICE, GBE, rendering engine, MACE) |
| Interrupts | engine on line 25 and retrace on 24 of the system chip's controller; memory errors on two further entries | all in one controller: rendering FIFO levels, GBE lines, CPU and memory error inputs |

Grades:

- The memory controllers share a register order (control, then status, address, syndrome), the same word width and the same
  error categories. The status bit positions do not match (CRIME puts soft at bit 20 and hard at bit 21; the HAL treats bits
  16 and 17 as the non-fatal pair), and the spacing is different. Grade: inferred shared design of the controller; no
  register match.
- The two translation schemes are **not** the same. CRIME fixes the tile at 64 KiB and the entry at 16 bits; Cobalt lets each
  buffer choose its tile size and stores a page frame. A design that serves both would hold the table as page frames plus a
  per-buffer tile size and derive CRIME's entries from it. Established as a difference; the equivalence is inferred.
- Cobalt never leaves an entry invalid, so whether its table has a valid bit, and what a stray access does, is
  **unknown** (the driver avoids the case); do not copy CRIME's invalid-entry behaviour onto it.
- Error reporting for the engine's own faults (a bad table entry, a bad packet) has no reading on Cobalt. The CRIME model's
  invalid-entry rule is a candidate (section 6), grade inferred.

## 5. Driver structure

| Piece | Cobalt (Windows NT 4.0) | O2 (IRIX) | IMPACT and VPro (IRIX) |
|---|---|---|---|
| Where the application's commands are written | the OpenGL driver writes packets into memory the kernel mapped for it and calls an escape per submission ([graphics-engine.md](graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)) | libGLcore writes engine registers directly through a mapping the kernel gave it; ioctls load TLBs ([context.md](https://github.com/jamesbraid/crime/blob/main/software/kernel/context.md)) | libGLcore stores to a mapped command FIFO; the FIFO page is unmapped when the pipe changes owner ([HQ3.h](https://github.com/jamesbraid/gr4/blob/origami-re/devices/hq3/HQ3.h), context management) |
| Who runs the engine | the miniport: a scheduler of per-context queues ("drops"), start by one register write | the kernel only loads and replays state | the kernel's generic resource layer plus a board hook |
| Context switch | build restore packets in the node's page, tell the engine where to dump the outgoing context (`0x18800`), wait on the stop cause, start the next | no hardware context: the kernel reloads a per-client shadow of the registers | IMPACT: the board holds a context, swapped by a kernel call on a fault; VPro: the board saves its context to a physical address the kernel provides and a request port starts it ([board-registers.md](https://github.com/jamesbraid/odsy/blob/main/devices/board/board-registers.md)) |
| Back end naming | `__glCoPass*` setters, `___glcoim_*` immediate entry points | `__glCrmPass*` setters (`PassBlendFunc`, `PassDepthFunc`, `PassStencilMode`, `PassLogicOp`, `PassAlphaFunc`, `PassColorMask`, `PassFog`) | MGRAS `__glMgrPass*`; VPro `__glOdyim_*` immediate and `__glOdylc_*` list-compile entry points |

Findings:

- **One source tree, many boards (inferred).** Three of the four drivers keep a per-board back end with setter routines named
  `Pass<state>` that turn a state change into the board's register word, and VPro and Cobalt use the same `im` and `lc`
  suffixes for the immediate and list-compile entry points. Cobalt's OpenGL driver carries the generic vendor machinery
  (`___glim_*`, `___gllc_*`, evaluators, clipping, lighting maths) around a Cobalt back end of about 570 routines
  ([nt-opengl-icd.md](../../software/windows/nt-opengl-icd.md)). A model that wants to execute the Cobalt back end's decisions
  can lean on the CRIME pages for the codes each setter emits, because the setters are parallel (section 2). Grade: inferred.
- **The scheduler is Cobalt's own.** The multi-context scheduler with timers, chunks and swap-at-retrace is Windows driver
  code, not the engine. Its structure is a host-side state machine a guest runs, not hardware; the other families' kernels
  have a resource manager of the same job (swap, retrace, clip) but their pages say nothing about a per-context drop queue.
  Nothing to reuse; model the engine, not the scheduler.
- **Context save by the engine into host memory.** Cobalt's dump offset register and VPro's save-buffer address plus request
  port are the same idea, and CRIME and IMPACT do it differently (software shadow, board context RAM). What Cobalt's engine
  writes at the dump offset is unknown. Grade: inferred, by analogy to VPro; the contents unknown.
- **Escape per submission.** Cobalt is the only family in which the user-mode driver never touches the hardware; that is a
  consequence of the engine fetching packets by DMA and so is not a design choice that other families share. Established.

## 6. What can be reused, what is new, what the other families can settle

### Reuse

| Cobalt need | Existing model or page | What carries over | Grade |
|---|---|---|---|
| Scan-out, tiled fetch, DID table, colour map and FIFO, gamma, cursor, I2C, timing | an O2 GBE model (where one exists) and the GBE pages | the tiled fetch and the register block; add the five extensions of section 3 | inferred (design), established (offsets) |
| Blend, logic operation, alpha test, depth and stencil comparison, filter and environment decoders | a CRIME pixel-pipeline model | the codes and, where the model implements the arithmetic from the published specification, the arithmetic; field positions differ, so decode the Cobalt words and map to the model's inputs. Wrap codes 0 and 1 swap | codes established; arithmetic inferred |
| Frame, depth and texture tile addressing at 512 bytes by 128 lines | CRIME tile addressing | address arithmetic inside a 64 KiB tile; Cobalt adds the descriptor-driven tile sizes and the 128-byte row | established (layout), inferred (texel order) |
| Texture filtering, level selection and fixed-point steppers | CRIME's pixel-arithmetic page and model | the 4-bit weights, coordinate scales and the stepper widths are a better starting point than anything written from scratch, but only for what Cobalt's drivers also show (below) | inferred |
| Vertex transform, lighting and four lights | a shared GL-math layer (matrix, normal and light vector maths) from the engines that have a geometry stage | the maths; the vertex port and attribute presence mask are Cobalt's | inferred |
| A batch recorder, tile store and soft-float for CPU and GPU raster back ends | the shared GPU layer of the engines that already use it | the infrastructure; Cobalt's raster rules are unknown (section 6, unknowns), so the first model defines a reference | inferred |

### Needs a new model

- The packet queue: link-word walk, ownership bit, chain registers, start and doorbell writes, the stop interrupt causes and
  the stop-time sequence-number registers. No other family has it.
- The translation table and descriptor table with variable tile sizes, and the packet pool read through it.
- The context-switch sequence with the engine-side dump.
- The engine's geometry front end on its two vertex ports, with the host/engine split of
  [draw-and-buffers.md](draw-and-buffers.md#who-does-what).
- The retrace register and Cobalt's own interrupt lines, and the Cobalt memory controller (the CRIME one is an analogy, not a
  clone).
- The 2D paths of the NT display driver, which go through the rendering engine as rectangles ([2d-blit-video.md](2d-blit-video.md)); CRIME's
  separate memory-transfer engine ([mte.md](https://github.com/jamesbraid/crime/blob/main/devices/mte/mte.md)) is not the model for them.

### Cobalt unknowns another family can settle by analogy (all graded inferred)

| Cobalt unknown ([open-questions.md](../../questions/open-questions.md)) | Where the other family answers | Reading |
|---|---|---|
| Whether stencil shares a 32-bit word with 24-bit depth | CRIME: depth bits 23:0, stencil 31:24 ([pixel-arithmetic.md](https://github.com/jamesbraid/crime/blob/main/devices/re/pixel-arithmetic.md), Depth and Stencil) | yes, 24-bit depth and 8-bit stencil in one word; the 16-bit format has no stencil |
| Order of depth, stencil and masking in the pixel pipe | CRIME: depth stepping, depth test, stencil test, then stencil and depth masking (published specification, same page) | the same order |
| Texel order inside a tile and the order of tiles | CRIME: rows of 512 bytes, 128 rows per tile, tiles in raster order (same page's texture layout section, `CRIME_RE.h`) | row-major within a tile, tiles in raster order |
| What the colour and logic-operation codes compute | CRIME: the 16 logic operations and blend equation and factor formulas from the published specification | the formulas, with 8-bit components |
| What the blend does at the rounding level, how filter weights are formed | CRIME: also open (the same page lists them unknown); the software blender and texture unit of the public sample implementation give a float reference | no settlement; both unknown, a model defines a reference |
| What the engine does with the clip rectangles' valid mask, and the clip-ID word | CRIME: five screen masks enabled by a clip-mode word, each with an inside-select, and clip-ID test and map bits | a per-rectangle enable and inside-or-outside choice, as in CRIME, so a model can test each rectangle separately; how the results combine and a clip-ID test against a per-pixel id are not shown by CRIME's pages either; bits 26, 5 and 4 stay unknown |
| What the second interrupt register at `0xca000000` is (write-one-to-clear or latching) | CRIME clears its edge-triggered status by writing zero to the bits | the handler writes back the complement of what it read. A write of 1 to clear would leave the set bits and clear the clear ones, which is nonsense, so write-zero-to-clear is the reading that makes the handler sensible (unless writes are ignored). It follows from the handler; CRIME only shows the convention exists in the family |
| Whether the dot clock reports lock (the NT driver reads no lock bit and waits a fixed time) | GBE: out-of-lock bit 23 and out-of-range bit 22 of the same register | the same two status bits, not yet read by any Cobalt software |
| What the cursor, capture and 10-bit gamma words do | GBE 1.1 pages (cursor, capture, the 256-entry gamma map) | the cursor and capture words behave as the GBE's; the 10-bit map has no counterpart |
| Reset values of the display registers | GBE 1.1 reset values (revision 1.1 only) | take the revision 1.1 values; revision 2 is unknown on both sides |
| Whether an invalid translation entry is a fault | CRIME: the request is flagged and the access suppressed | unproven for Cobalt (its driver never leaves an entry invalid) |

Not settled by any family: the packet semantics, the fill rule and rasterisation, the packet pool's internal fetch order,
what the doorbell does, and everything that is about the Cobalt engine's own chain registers. The biggest open item is the
engine's behaviour: no family has a packet-fed design, so the packet queue, the chain registers, the two stop causes and the
dump offset can be settled only by the Cobalt software itself or by hardware documentation.
