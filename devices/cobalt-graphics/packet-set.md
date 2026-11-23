# Cobalt rendering engine: the packet set

The header words the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 puts in front of the data it hands the engine, one row per
header, plus the headers the miniport `sgcomp.sys` 5.2.4 and the display driver `sgcodd.dll` 5.2.4 add. Back to the
[overview](README.md); queue, chaining and link words are in [graphics-engine.md](graphics-engine.md); what the draw
and buffer packets do together is in [draw-and-buffers.md](draw-and-buffers.md); how the drivers are built and how the
names were applied is in [nt-opengl-icd.md](../../software/windows/nt-opengl-icd.md).

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)). A statement about what a shipped routine does is
**established** when the whole routine was read in disassembly, and is cited as `file@address`. What the hardware does
with a word is **inferred** at best, because no document or public driver describes the engine. Anything not marked is
**unknown**.

## How the table was made

- **Symbols.** The `.dbg` file next to each binary holds a COFF symbol table, parsed directly. For `sgcoicd.dll` it has
  13,109 table records; 8,883 are symbols (the rest are auxiliary records), 3,929 of those are function-typed symbols
  in `.text`, and they sit at 3,633 distinct addresses. The Windows 2000 `.dbg` files hold no symbol table (only
  frame-pointer data and a program-database reference), so that ICD's routines are located by their header constants,
  not by name.
- **Header scan, established.** Every 32-bit immediate operand in `.text` was collected with a linear disassembly sweep,
  and separately every dword (aligned or not) of `.text` and `.data` whose bits 31:12 equal a known `(A, B)` pair. An
  immediate is attributed to the routine that contains it by the vendor's own function names. The body of each routine
  named in a row was read in disassembly. Raw dword hits are not evidence on their own (see the withdrawn header below).
- **Payload layout: inferred** unless the row says it was read in disassembly.
- **What the engine does with a word: unknown**, except where a driver-side register read or write ties a header to a
  register and where the SGI white paper names the function. A white paper feature that matches a row is agreement,
  never the meaning.
- Names in the "Name" column are mine; function names are the vendor's.
- **Typed re-check.** The rows were read again in Ghidra projects of the NT 4.0 5.2.4 miniport, display driver and OpenGL driver with the
  vendor's program databases applied (names, prototypes and structure types; the OpenGL driver's context is passed in a register, so its
  routines were retyped `__fastcall` to make the context's fields name themselves). Every structure offset the page used to give raw now carries the
  vendor's field name, and every payload bit field was compared with the program database's declaration. The 54 constant headers the OpenGL
  driver's decompile contains are exactly the set below without the six miniport headers and the display driver's `0x91ca1020`;
  the second clear bank is computed.
- Binaries read: the SGI Cobalt Graphics 5.2.4 driver packs for Windows NT 4.0 and Windows 2000 (the files in
  [windows-files.md](../../references/windows-files.md)), and for the display driver and miniport also the April 1999
  builds on the NT 4.0 OEM set (disk 1, `I386\SGCODD.DLL` and `I386\SGCOMP.SYS`), which are byte-identical to the ones
  on SGI's Silicon Graphics Recovery CD-1 v1.1.

## Header word layout

A packet is one header dword followed by the payload dwords. Packets are written back to back (the scissor packet is
three dwords); only a node that is being closed is padded to a 32-byte boundary with zero dwords, by `ClosePacket` and
by the miniport's builders, which end with a chain packet and then pad (`sgcomp.sys@0x16d3e`, `sgcomp.sys@0x16f32`).

| Bits | Field | Rating |
|---|---|---|
| 31:30 | class: `00` geometry state and draw ports, `10` engine register file, `11` direct register-window write (different layout, below); `01` not seen | inferred |
| 31:21 | `A`, start address of the first payload dword (11 bits; classes `00` and `10` are `A` below `0x200` and from `0x400`) | inferred |
| 20:12 | `B`, the register's index in the engine's own register file (9 bits; [below](#b-is-the-index-in-the-engines-register-file)) | inferred |
| 11:5 | payload dword count `n` (0 to 127). It is not the payload length for the vertex ports (below) | inferred; checked against 17 of 18 shadow-table entries earlier |
| 4:0 | instance: light index, vertex attribute-presence mask, or a variant flag ([per block](#instance-bits-per-block)) | inferred |

**Class `11` has a different layout.** The only builders are the miniport's texture-table packets
(`sgcomp.sys@0x16a7a`, `sgcomp.sys@0x17a46`). The header is `0xc0000000 | (position << 16) | count`: bits 29:16 are a
window position in 8-byte units and bits 15:0 are the dword count (the builders send at most 64 per header, and start a
new header at every 64-entry group of the translation table). Position `0x1000 + i` is the translation-table entry at
window offset `0x8000 + 8 * i`; position `0x1800 + k` is the buffer descriptor `k` at `0xc000 + 8 * k`. Established as what
the builders write; the engine's parsing is inferred.

**Class `10` offsets.** `0x18000 + 8 * (A - 0x400)` is the window offset of the first register written, with 8-byte
spacing and the payload dwords going to consecutive registers. Three matches with the miniport tie this down: the
scissor `0x84e44040` (`A` = `0x427`) is `0x18138` and `0x18140`, the two registers the miniport reads into a packet
headed `0x84e44040` when it restarts the node already in the hardware; the chain packet `0x97000040` (`A` = `0x4b8`) is `0x185c0` and `0x185c8`;
`0x97400020` (`A` = `0x4ba`) is `0x185d0`, the current-texture id. Inferred.

## `B` is the index in the engine's register file

Sorting every header by `B` instead of `A` gives one sequence of consecutive registers. Across 53 distinct `B` values,
taking the largest `n` seen at each `B`, the next `B` equals `B` plus `n` of the previous in 39 of 52 adjacent pairs; three more are overlaps (the long form of a
packet already includes the next register: the 7-dword ambient-and-diffuse materials include the diffuse block, and the
3-dword long chain packet at `B` = 0 reaches the primitive mode at `B` = 2), and the remaining ten are gaps at registers that no header in
the OpenGL driver names. (With the smallest `n` at each `B` the split is 41 exact, 11 gaps and no overlaps.) The runs, with `A` for comparison (`A9` is the low nine bits of `A`):

| `B` run | Registers | `A` | `B - A9` |
|---|---|---|---|
| `0x02` to `0x04` | primitive mode, pixel-pipe mode word, flush marker | `0x0c0`, `0x400`, `0x4c4` | scattered |
| `0x10` to `0x32` | draw flags, matrix, normal matrix, viewport, bias | `0x001` to `0x022` | `0x0f` |
| `0x33` | vertex port (two headers share it) | `0x0c5` and `0x0c6` | |
| `0x40` to `0x46` | line stipple repeat and pattern, polygon stipple, scissor | `0x023` to `0x025`, then `0x427` | `0x1d` |
| `0x50` to `0x5c` | window parameters, then the two one-register packets | `0x4c1`, `0x4c2`, `0x4c3` | `0x4c1` overlaps `0x4c2` to `0x4c4` |
| `0x60` to `0x84` | lighting mode word, scene ambient, emission, ambient, diffuse, specular, current colour; colour index at `0x84` | `0x029` to `0x04c`, `0x0c7` | `0x37` |
| `0x85` | normal | `0x0cd` | |
| `0x90` to `0x9f` | light vectors, light colours, light scalar (one copy each) | `0x04d`, `0x065`, `0x089` (four copies each) | `0x43`, `0x31`, `0x16` |
| `0xa0` to `0xb2` | draw-buffer descriptor, constant colour, transfer parameters 2, fog colour, alpha test, blend, logic op, colour mask, depth buffer descriptor, polygon offset, depth test, stencil, secondary colour, fog value | `0x48d` to `0x49c`, `0x0c8`, `0x0c9` | `0x13` |
| `0xc0` to `0xde` | transfer parameters 1, colour matrix (22 registers), texture coordinate | `0x49e`, `0x4a2`, `0x0cc` | `0x22`, `0x0e` |
| `0xe0` to `0xe5`, `0x120` to `0x125`, `0x138` | clear rectangle, second bank, image transfer | `0x4ce`, `0x4f3`, `0x4f6` | |

What this shows, all inferred (the arithmetic is established, the reading is not):

1. `B` is a register number. The classes do not change it: the scissor (class `10`) sits between the polygon stipple and
   the window parameters, the colour index and normal ports (class `00`) sit inside the lighting state, and the draw
   flags run through the matrices into the vertex port at `0x33`.
2. The earlier statement that the next `A` equals `A` plus `n` holds only inside a run. Its exceptions are explained by
   `B`: the `0x023` to `0x025` blocks end at `0x027`, where the next `B` (`0x44`) is the class-`10` scissor at `0x427`,
   not a class-`00` block; `0x4c1` (`n` = 11) overlaps `0x4c2` to `0x4c4` because `B` continues (`0x50` to `0x5a`, then
   `0x5b`, `0x5c`) while `A` does not; and `0x4f3` (`n` = 5) overlaps `0x4f6` because the image transfer is the next
   `B` after the second clear bank, not the next `A`.
3. Banked blocks: `A` holds four copies and `B` one. The three light blocks step `A` by `4 * n` (`0x04d` + 24 = `0x065`,
   `0x065` + 36 = `0x089`, `0x089` + 4 = `0x08d`), and the `A` after the scalars, `0x08d`, is the draw-buffer
   descriptor's `A9` (`0x48d`). `B` steps by `n` once. The instance field picks the copy.
4. Two `A` values can name one `B`: the two vertex ports are `0x0c5` and `0x0c6`, both `B` = `0x33`. The second clear
   bank is `0x04a40000` above the first in the header (`A` + `0x25`, `B` + `0x40`).
5. A gap in `B` is a register nothing in the OpenGL driver writes. One was predicted from this and found in the display
   driver: `B` = `0xa1` / `A` = `0x48e`, header `0x91ca1020` (`sgcodd.dll@0x1229c` and four other sites). The other
   apparent gap, `B` = `0xa3` (`A` = `0x490`), is the second word of the texture-environment packet `0x91ea2040` (the
   vendor's environment colour), so every register of the run is written.
6. The low `B` values (0 to 4) are not part of a run: the chain and texture-id registers are both `B` = 0 with
   different `A`, the primitive mode is `B` = 2, the pixel-pipe mode word 3 and the flush marker 4.

The big shadow packets are bulk loads over a run: `0x00210440` (`n` = 34) covers `B` = `0x10` to `0x31`, which is the
union of the smaller blocks at `0x001` (`n` = 1, the draw flags), `0x002` (16), `0x012` (9), `0x01b` (6) and `0x021`
(2). Established as arithmetic over the headers listed here.

## The table

`n` is the count field. "ICD" means `sgcoicd.dll` 5.2.4; "miniport" means `sgcomp.sys` 5.2.4; "display driver" means
`sgcodd.dll` 5.2.4. The display driver and miniport use headers from this set and one more (`0x91ca1020`); the scan
found nothing else in either and nothing outside the set in the Windows 2000 ICD ([coverage](#coverage)). The
Generate* routines (`GenerateSetAttribs`, `__glCoGenerateNoXformHandler`, `__glCoGeneratePrimitiveHandler` and the
others) emit x86 code that stores the headers: the constants are immediates in the generator, and the stores happen when
the generated code runs. Address rows are in `A` order; payload text is inferred unless marked.

### Geometry state and draw ports (class 00)

| Header | `A` | `B` | `n` | Name | Written by (ICD unless noted) | Payload |
|---|---|---|---|---|---|---|
| `0x00210020` | `0x001` | `0x10` | 1 | draw flags word | `__glCoClipTriangle`, `__glCoClipLine`, `__glCoDoPolygonClip`, `__glCoSetVertexAttribsMask`, `__glCoDlistExecutePackets`, `___glcoim_Begin` and others | The vendor's transform mode word (`CoXformModeReg`): bit 31 `enViewportXform`, bit 30 `enModelXform`, bit 29 `enTexXform`, bit 28 `enClipCheck`. The vertex packets carry object coordinates (the vertex record's `obj`, offset `+0x00`) when bit 30 is set, clip coordinates (`clip`, `+0x20`) when only bit 31 is, and window coordinates (`window`, `+0x40`) and the context's w when neither is; bit 30 wins when both are set. Read as the vendor names them: with bit 30 the engine applies the model-view-projection transform, with bit 31 the viewport transform, and the host has done whatever the bits leave clear. Bit 29 is cleared on both paths that leave bits 31:30 non-zero (`___glcoim_Begin@0x6904327f`, `@0x690432a1`) and nothing read sets it, so texture-coordinate transform is never the engine's; bit 28 is never set either. The rule that sets the other bits is in [draw-and-buffers.md](draw-and-buffers.md#who-does-what) |
| `0x00411200` | `0x002` | `0x11` | 16 | model-view-projection matrix | `__glCoPassMVPMatrix` | 16 floats copied from the transform stack, column by column |
| `0x02421120` | `0x012` | `0x21` | 9 | normal matrix | `__glCoPassNormalMatrix` | 3 by 3 floats |
| `0x0362a0c0` | `0x01b` | `0x2a` | 6 | viewport | `__glCoSetViewMatrix` | three scale floats then three offsets; the x and y offsets have a sub-pixel constant subtracted |
| `0x04230040` | `0x021` | `0x30` | 2 | x and y bias | `coLoadLinearTlb`, `coSetupTileToTile`, `coSetTexParam`; miniport `CoCopyRect` and the window packets | two floats: the window record's texture window offsets (`texWinoffsetS` and `texWinoffsetT`, record offsets `+0x1c4` and `+0x1c8`), which the miniport's context switch copies into the context's saved packet image. The miniport's copy sends `0.5` for both (`0x3f000000`); the ICD's pixel-transfer set-up adds a constant to both |
| `0x04640020` | `0x023` | `0x40` | 1 | line stipple repeat | `__glCoBeginLStrip`, `__glCoBeginLLoop`, `__glCoRenderAliasLine`, `__glCoRenderBitmap`, `__glCoWideStippledAliasLine`, `FillAASubTriangle`; display driver `DrvStrokePath`, `DrvTextOut`, `vXfer1bpp`, `bPutGlyphInCache` | the GL repeat factor, expanded. The display driver sends `0x1f00 \| (x & 7)` for every mono bitmap row and glyph row (length field 31, start bit in 7:0) and `(length - 1) << 8 \| 0x2000000` with the start phase in 7:0 and a third-of-pixel step in 17:16 for styled lines ([2d-blit-video.md](2d-blit-video.md#mono-colour-expansion)); inferred |
| `0x04841020` | `0x024` | `0x41` | 1 | line stipple pattern | same as above | the 16-bit GL pattern in both halves; the display driver sends up to 32 bits, a mono bitmap row bit-reversed per byte (bit 0 is the leftmost pixel) |
| `0x04a42040` | `0x025` | `0x42` | 2 | polygon stipple | `__glCoConvertStipple`, `__glCoFillStippledTriangle`; display driver `vFillStip`, `bFillSetup`, `DrvFillPath` | an 8 by 8 bit pattern. `__glCoConvertStipple` reverses the bits of each byte of the 32 by 32 GL pattern and sends this only if the pattern is the same 8 by 8 block repeated; otherwise it falls back to host stippling (read in full) |
| `0x05260020` | `0x029` | `0x60` | 1 | lighting mode word | `LoadLightPath`, `__glCoClearBuffers`, `__glCoClipTriangle`, `__glCoRenderAAPoint` and others | one word kept in the context (`light.lightModeReg`, a `CoLightModeReg`, `+0x2d5e4`): bit 31 (`twoSided`) is the two-sided lighting-model flag (`state.light.model.twoSided`, `+0x1abd`), bit 26 (`enNormalize`) is bit 16 of the context's enable word (`state.enables.general`, `+0x1c4c`; byte `+0x1c4e` bit 0), bit 23 (`enLighting`) is set by `LoadLightPath` when lighting is enabled and the engine path is used and cleared on the host-lighting path ([draw-and-buffers.md](draw-and-buffers.md#lighting-and-material-state)); `LoadLightPath@0x690267b4` read in full also sets and clears bit 25 (`enSpecular`). The light enables in bits 30:27 are set by `LoadLightSource@0x69026d06`, which ORs `0x40`, `0x20`, `0x10` or `0x08` per light into the byte at `+0x2d5e7`, and cleared with the rest of bits 31:26 by `__glCoUpdateLightingState@0x690266ce` (`and byte [+0x2d5e7],3`); no routine writes bit 24 (`specOnly`) explicitly. An earlier version said no bit other than 31, 26 and 23 was set |
| `0x05461060` | `0x02a` | `0x61` | 3 | scene ambient | `LoadLightPath` | the GL light-model ambient colour (RGB floats) |
| `0x05a64060` | `0x02d` | `0x64` | 3 | emission (front) | `ChangeMaterialEmission`, `__glCoValidateMaterial` | RGB floats |
| `0x06067060` | `0x030` | `0x67` | 3 | emission (back) | same | RGB floats |
| `0x0666a060`, `0x0666a0e0` | `0x033` | `0x6a` | 3, 7 | ambient (front); ambient then diffuse | `ChangeMaterialAmbient`, `ChangeMaterialAmbientAndDiffuse`, `__glCoValidateMaterial` | ambient RGB; the 7-word form carries diffuse RGBA after it |
| `0x06c6d080`, `0x06c6d081` | `0x036` | `0x6d` | 4 | diffuse (front) | `ChangeMaterialDiffuse`, `__glCoGenerateNoXformHandler`, `GenerateSetAttribs`, `__glCoDlistToPackets` | RGBA. Instance 1 appears only in the generated handlers and the list conversion, in the colour-material mode "ambient and diffuse" (`gc+0x1aa4` = `0x1602`) when a context byte is set; inferred to mean the colour also becomes the ambient colour ([opengl-triangle-path.md](../../software/windows/opengl-triangle-path.md#lighting-words)) |
| `0x07471060`, `0x074710e0` | `0x03a` | `0x71` | 3, 7 | ambient (back); ambient then diffuse | as `0x0666a060` | as `0x0666a060` |
| `0x07a74080`, `0x07a74081` | `0x03d` | `0x74` | 4 | diffuse (back) | as `0x06c6d080` | as `0x06c6d080` |
| `0x08278080` | `0x041` | `0x78` | 4 | specular and shininess (front) | `ChangeMaterialSpecular`, `__glCoValidateMaterial` | specular RGB then the shininess |
| `0x08a7c080` | `0x045` | `0x7c` | 4 | specular and shininess (back) | same | as above |
| `0x09280080` | `0x049` | `0x80` | 4 | current colour | `__glCoSetVertexAttribsMask`, `LoadLightPath`, `__glCoClearBuffers`, the span and bitmap routines | RGBA floats |
| `0x09a900c0` \| `i` | `0x04d` | `0x90` | 6 | light vectors, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | six floats: the unit light vector and the half-vector, in eye space; sent once for a directional light and again per vertex for the others ([opengl-triangle-path.md](../../software/windows/opengl-triangle-path.md#lighting-words)) |
| `0x0ca96120` \| `i` | `0x065` | `0x96` | 9 | light colours, light `i` | `LoadLightSource` | ambient, diffuse and specular RGB of GL light `i` |
| `0x1129f020` \| `i` | `0x089` | `0x9f` | 1 | light scalar, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | 1.0 for a directional light, otherwise attenuation (and spot factor) as decoded in [opengl-triangle-path.md](../../software/windows/opengl-triangle-path.md#lighting-words); sent per vertex only when it varies |
| `0x18002020` | `0x0c0` | `0x02` | 1 | primitive mode | every `__glCoBegin*`, `RestartDispatch*`, `__glCoFill*Triangle`, `__glCoRenderPoint`, the display driver and miniport `CoCopyRect` | see [draw-and-buffers.md](draw-and-buffers.md#primitive-mode-word) |
| `0x18a33060` + mask | `0x0c5` | `0x33` | 3 | vertex port A: the two-component vertex | `__glCoSetVertexAttribsMask`, `GenerateSetAttribs`, the span-store routines, `FillAASubTriangle`, the display driver (every 2D routine), miniport `CoCopyRect` | see [draw-and-buffers.md](draw-and-buffers.md#vertex-packets). Constants in the binaries: mask 0 (`0x18a33060`) and mask `0x08` (`0x18a33068`) |
| `0x18c33060` + mask | `0x0c6` | `0x33` | 3 | vertex port B: the vertex with z | same, plus `__glCoRenderBitmap`, `__glCoRenderAAPoint`, `__glCoDlistToPackets`; never the display driver or miniport | as above. Constants: mask 0 (`0x18c33060`), `0x02` (`0x18c33062`), `0x10` (`0x18c33070`) |
| `0x18e84020` | `0x0c7` | `0x84` | 1 | colour index | `__glCoSetVertexAttribsMask`, `__glCoClearBuffers`, the span stores, display driver (solid fill colour) | the colour index (low 8 bits) in colour-index mode, 0 otherwise; the display driver sends its fill colour here |
| `0x190b1020` | `0x0c8` | `0xb1` | 1 | secondary colour | `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | three bytes packed in bits 31:8 |
| `0x192b2020` | `0x0c9` | `0xb2` | 1 | fog value | same, `__glCoRenderBitmap` | integer (the float converted) |
| `0x198da080` | `0x0cc` | `0xda` | 4 | texture coordinate | same, `__glCoSpanRenderRGBA` | four floats; the third is 0.5 when the texture has fewer than two dimensions |
| `0x19a85060` | `0x0cd` | `0x85` | 3 | normal | `__glCoSetVertexAttribsMask`, `resendFirstVertThenResetState` | three floats |

### Engine register file (class 10)

| Header | `A` | `B` | Window offset | `n` | Name | Written by | Payload |
|---|---|---|---|---|---|---|---|
| `0x80003020` | `0x400` | `0x03` | `0x18000` | 1 | pixel-pipe mode word | the `__glCoEnable*` routines, `__glCoPickLineProcs`, `__glCoPickTriangleProcs`, `__glCoPickTextureProcs`, `__glCoPassColorMask`, the span stores, `__glCoClearBuffers`, `__glCoDlistExecutePackets`, `___glcoim_Begin`, the display driver in about forty places, miniport `CoCopyRect` | one 32-bit flag word kept in the context (`drawmode`, a `CoDrawModeReg`, `+0x2db9c`); every bit is decoded [below](#pixel-pipe-mode-word-0x80003020) |
| `0x84e44040` | `0x427` | `0x44` | `0x18138` | 2 | scissor box | `__glCoSetScissor`, `__glCoDrawBufferRegion`, `__glCoFillStippledTriangle`, the transfer routines, the miniport | low 16 bits x, high 16 bits y; first word the minimum corner, second the inclusive maximum (decompiled C only; the window origin and a vertical flip are folded in). The miniport uses `0xf000f000` and `0x0fff0fff` (signed 16-bit, plus or minus 4096) for "everything" in the GDI and type 3 window packets; the pixel-transfer window packet sends minimum 0 and maximum `0x0fff0fff` in its scissor and carries `0xf800f800` / `0x07ff07ff` in the clip-rectangle pair |
| `0x91aa0020` | `0x48d` | `0xa0` | `0x18468` | 1 | draw-buffer descriptor | `__glCoSetDrawBuffer`, `__glCoLinearToTileXfer`, `__glCoTileToTileXfer`, `coTileToLinearSpans`, the miniport, the display driver | checked in disassembly: bits 24:19 are the low six bits of the first dword (`id`) of the buffer's `__GLCOtlbDescRec`; bits 30:25 hold the colour format (below); bit 31 is preserved by `__glCoInitRGB@0x69044c85`..`0x69044cc4` and never set by it. Always follows a `0x98804000` |
| `0x91ca1020` | `0x48e` | `0xa1` | `0x18470` | 1 | constant colour | display driver: `vFillStip`, `vXfer1bpp`, `bFillSetup`, `DrvFillPath`, `bPutGlyphInCache` | the first of the two colours of a stipple fill, a 1 bit per pixel transfer or the glyph cache: the first colour dword of the brush or the transfer's colour translation, with the second sent to the colour register `0x18e84020` (`sgcodd.dll@0x1229c`..`0x124bd`, `@0x15443`..`0x15498`, read in disassembly). For pixel-format code 4 (RGBA5551) it is first expanded to red in 31:27, green in 23:19 and blue in 15:11 (`sgcodd.dll@0x122c5`..`0x122d5`, `@0x15457`); the glyph cache sends `0xff`. Always sent with mode-word bit 15, which the vendor's mode type calls the opaque-stipple enable, and bit 13 or 14, at five sites. Inferred: the colour of the pixels the pattern leaves clear; three display-driver paths agree (the realised mono brush, the 1 bit per pixel transfer and the glyph cache, whose set pixels are colour 0 in the colour port and clear pixels `0xff` here, with an alpha test against `0xff`: [2d-blit-video.md](2d-blit-video.md#mono-colour-expansion)). The OpenGL driver never sends it |
| `0x91ea2040` | `0x48f` | `0xa2` | `0x18478` | 2 | transfer parameters 2 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam`, miniport `CoCopyRect` (`0x62000000`, 0) | the vendor's texture-environment register (`CoTexEnvParamReg`): word 1 is the mode (function in 31:29, type in 28:25) and word 2 the environment colour, so `B` = `0xa3` is the second word of this packet and not a register nobody writes. Word 1 from the ICD is `(0x30 + (f & 0xf)) << 25`: function 3 and type `f`, a four-bit context field; the colour is 0. The miniport's copy sends `0x62000000` and 0 (function 3, type 1). `coSetTexParam@0x6901df5f` sends it for a texture with function 0 to 4 (modulate, decal, blend, replace, add) and the level's base-format class 0 to 5 as the type, and the environment colour for blend and add only: [textures.md](textures.md#environment-word). Read in disassembly (`__glCoSetXferParam`, `CoCopyRect`, `CoYUVBlt_S2S`, `coSetTexParam`); the field names are the vendor's |
| `0x922a4020` | `0x491` | `0xa4` | `0x18488` | 1 | fog colour | `__glCoPassFog` | three bytes in 31:8 |
| `0x924a5040` | `0x492` | `0xa5` | `0x18490` | 2 | alpha test | `__glCoPassAlphaFunc`, display driver | word 1: `(7 or 8) << 28 \| 0xa000000` in the display driver's colour key (31:28 channel enables, function 5 = not-equal in 27:25; bit 31 set), word 2: the reference, a full 32-bit key in the display driver's colour key and the low byte in the OpenGL driver's alpha test |
| `0x928a7040` | `0x494` | `0xa7` | `0x184a0` | 2 | blend | `__glCoPassBlend`, display driver | word 1: equation in 23:20 (from `+0x1c78`), source factor in 27:24 (from `+0x1c70`), destination factor in 31:28 (from `+0x1c74`), read in disassembly at `sgcoicd.dll@0x6902c8f3`..`0x6902c919`; `___glcoim_BlendFunc@0x690289d0` stores the source to `+0x1c70` and the destination to `+0x1c74`. Word 2: constant colour, alpha in 7:0. |
| `0x92ca9020` | `0x496` | `0xa9` | `0x184b0` | 1 | logic operation | `__glCoPassLogicOp`, display driver (a ROP table) | operation in 31:28 (GL enum minus `0x1500`) |
| `0x92eaa020` | `0x497` | `0xaa` | `0x184b8` | 1 | colour write mask | `__glCoPassColorMask`, the transfer and alpha-bit routines | 32-bit mask, red in the top byte |
| `0x930ab020` | `0x498` | `0xab` | `0x184c0` | 1 | depth and stencil buffer descriptor | `__glCoSetSzBuffer` | checked in disassembly: bits 28:23 are the low six bits of the first dword (`id`) of the buffer's `__GLCOtlbDescRec`; bits 31:29 select the depth format (below). Always follows a `0x98804000` |
| `0x932ac040` | `0x499` | `0xac` | `0x184c8` | 2 | polygon offset | `__glCoPassPolygonOffset` | checked in disassembly: word 1 is the first argument of `glPolygonOffset` (the factor) unchanged, word 2 is the second (the units) times `depthBuffer.minResolution`, the float the depth-buffer set-up computes |
| `0x936ae020` | `0x49b` | `0xae` | `0x184d8` | 1 | depth test | `__glCoPassDepthTest`, `__glCoClearBuffers`, `StoreALWAYS` | function in 31:29 (GL function minus `0x200`; the clear sends 7, "always"), mode in 28:27 |
| `0x938af040` | `0x49c` | `0xaf` | `0x184e0` | 2 | stencil | `__glCoPassStencil`, `__glCoClearBuffers`, `__glCoSpanRenderStencil`, `StoreStencil` | word 1 is the vendor's `CoStencilModeReg`: reference 7:0, mask 15:8, compare function 19:16 (a 4-bit field), then the three operations 23:20, 27:24 and 31:28 (`sfail`, `dpfail`, `dppass`); word 2 one more byte |
| `0x93cc0080` | `0x49e` | `0xc0` | `0x184f0` | 4 | transfer parameters 1 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam`, display driver, miniport | the vendor's four-word texture register (`CoTexGenParamReg`): the format word, the mode word (min filter 31:29, mag filter 28, wrap T 27:26, wrap S 25:24, map height 23:20 and width 19:16), the level-of-detail word and the border colour. The format word (`CoTexGenFormatReg`) is `linear` in 31, `texType` in 30:27, `bufDepth` in 26:25 and the buffer descriptor id in 24:19; the ICD's `(((((a & 0xf) + (b << 4)) << 2) + (c & 3)) << 6) + (d & 0x3f)` shifted left by 19 is the type `a`, `linear` as the low bit of `b`, the depth `c` and the id `d`, which are the five fields the vendor names in the context's transfer-parameter record (`pixType`, `pixMode`, `pixDepth`, `descId`, `linear`). The miniport and the display driver send an id in 24:19 with the other fields zero, and the display driver's YUV blit sends `0x72100000` or `0x7a100000` (type 14 or 15, depth 1, id 2) with the three other words 0. `coSetTexParam` sends the texture's type, depth and id in the format word, the filter, wrap and map-size codes in the mode word, the level-of-detail word and the border colour; every code is in [textures.md](textures.md). Established as what is written; the field meanings are the vendor's names |
| `0x944c42c0` | `0x4a2` | `0xc4` | `0x18510` | 22 | colour matrix | `coSetColorMatrixRegisters`, display driver `CoYUVBlt_S2S` | four rows, each the four coefficients of one matrix row followed by that row's bias (20 floats), then a clamp minimum (default 0) and a clamp maximum (default `0xffffffff`); which matrix is sent in which case, and the layout's evidence, are in [opengl-pixel-transfers.md](../../software/windows/opengl-pixel-transfers.md#the-colour-matrix-packet-0x944c42c0) |
| `0x97000020` | `0x4b8` | `0x00` | `0x185c0` | 1 | chain, short form | miniport only | next position |
| `0x97000040` | `0x4b8` | `0x00` | `0x185c0` | 2 | chain | `ClosePacket`, `DispatchCurrentPkt`, `__glCoDlistExecutePackets`, `coCreateTexBindPkt`, `coDispatchTexBindPkt`; display driver `pulGetPacket`. The miniport sends only the short and long forms | next node position, this node position ([graphics-engine.md](graphics-engine.md)) |
| `0x97000060` | `0x4b8` | `0x00` | `0x185c0` | 3 | chain, long form | miniport only (`pCoContextSwitch`, `CoBuildUserTexTlbPacket`) | `CoBuildUserTexTlbPacket` sends the node position shifted left 12, 0 and a word from the texture record (`sgcomp.sys@0x17a6d`..`0x17a86`); the third word's meaning is unknown |
| `0x97400020` | `0x4ba` | `0x00` | `0x185d0` | 1 | current-texture id reset | `__glCoCreateTexLevel` | the payload is always 0. The ICD sends it, then runs its overflow routine, before it frees storage of the texture that is currently bound. The miniport writes the same register with the node's current-texture id |
| `0x98250160` | `0x4c1` | `0x50` | `0x18608` | 11 | window parameters | miniport (`CoBuildWinparamsPacket`, `CoBuildWinparamsPacketForPxfer`), display driver | a clip-rectangle valid mask and five (minimum, inclusive maximum) corner pairs; see [window parameters](#window-parameter-packets) |
| `0x9845b020` | `0x4c2` | `0x5b` | `0x18610` | 1 | window origin | miniport, display driver | the window origin, y in 31:16 and x in 15:0 ([below](#window-parameter-packets)) |
| `0x9865c020` | `0x4c3` | `0x5c` | `0x18618` | 1 | window parameter | miniport only | the window record's clip-ID word (`cidMode`, `+0x194`). The miniport only reads it; the display driver writes it ([window parameters](#window-parameter-packets)) |
| `0x98804000` | `0x4c4` | `0x04` | `0x18620` | 0 | flush marker | `DispatchCurrentPkt`, every descriptor and mode write, the span stores; miniport and display driver | no payload. The meaning is unknown: the miniport never touches `0x18620`. It is placed before every descriptor or mode write, which says where software wants it, not what the engine does with it |
| `0x99ce00a0` | `0x4ce` | `0xe0` | `0x18670` | 5 | clear rectangle | `coClearBuffers`, miniport `CoClearRect`, display driver `CoRectFill` and `vClearSolid` | the vendor's `CoClearModeReg`: the buffer descriptor (`tlbDesc`), the byte mask, the value (`color`), the minimum and the maximum corner (`CoFbAddrType`: x in 15:0, y in 31:16). That is the order both builders write (`CoClearRect@0x174dd`..`0x17502`, `coClearBuffers@0x69047837`..`0x6904787e`), not value first as an earlier reading had it. See [draw-and-buffers.md](draw-and-buffers.md#clears) |
| `0x9e7200a0` | `0x4f3` | `0x120` | `0x18798` | 5 | clear rectangle, second bank | `coClearBuffers`: computed as `0x99ce00a0 + 0x04a40000` when the context byte at `+0x2e2cc` is set (`sgcoicd.dll@0x6904770f`..`0x69047714`) | as `0x99ce00a0` |
| `0x9ed380e0` | `0x4f6` | `0x138` | `0x187b0` | 7 | image transfer | `coVhdlTexImage` | three caller words, a pixel count (width times height), a width, then the minimum and maximum corner packed y:x as 16 bits each |

### Pixel-pipe mode word (`0x80003020`)

The context keeps the word at `gc+0x2db9c` and the last value sent at `gc+0x2dba0`. A routine that changes it sends a
flush marker, the header and the word, and only if the word differs from the last one sent (`___glcoim_Begin`,
`__glCoEnableDither` and others, all read in full). Every bit below is a routine in `sgcoicd.dll` that sets or clears
exactly that bit, read in disassembly, except where noted. "Enable word" is the context word at `gc+0x1c4c`.

| Bit | Meaning (what the driver sets it from) | Routine |
|---|---|---|
| 31 | stencil test: enable-word bit 15 and a stencil buffer exists (`+0x2d579`) | `__glCoEnableStencilTest@0x69023eac` |
| 30 | depth write: buffer present, the GL depth-mask byte, enable-word bit 4 (depth test); cleared and restored around the back pass of a two-sided triangle | `__glCoPassDepthMask@0x6902ce2c`, `__glCoFillFrontBackTriangle@0x69025cb5` |
| 29 | depth test: buffer present and enable-word bit 4 | `__glCoEnableDepthTest@0x6902ceba` |
| 28 | depth offset (the vendor's `enDepthOffset`): set by `__glCoPickTriangleProcs` when any of bits 20:18 of the context word `+0x3d80` is set and a local flag derived from the polygon-mode state is nonzero; cleared at the start of each pick. `__glCoPickPhase1Procs@0x69035965`..`0x690359e5` sets those three bits of `+0x3d80` from the three polygon-offset enables (points, lines, fill: enable-word bits 20, 19 and 18) only when the factor or the units (`gc+0x177c`, `+0x1780`) is non-zero. Inferred | `__glCoPickTriangleProcs@0x6903517b` |
| 27 | the colour write mask is all zero (`+0x2e31c` is 0) | `__glCoPassColorMask@0x6902ccda` |
| 26 | partial colour write mask: set when `__glCoPassColorMask` sends `0x92eaa020` because the mask is neither all zero nor all ones (`push 1` at `@0x6902cd71`, `pop ebx` at `@0x6902cd7e`, `and ebx,1` / `shl ebx,0x1a` at `@0x6902cde6`); cleared otherwise. The transfer routines read it and merge it into the word they send (`__glCoLinearToTileXfer@0x6903b61f`, `@0x6903b6a6`) | same |
| 25 | dither: enable-word bit 3 | `__glCoEnableDither@0x6902c9ba` |
| 24 | logic operation: enable-word bit 2 in colour-index mode; in RGB mode set by `__glCoEnableBlend` when enable-word bit 21 is set, or blend is on and the blend-equation field is `0xbf1`. Setting it clears bit 23 | `__glCoEnableLogicOp@0x6902ca54`, `__glCoEnableBlend@0x6902c712` |
| 23 | blend: RGB mode, enable-word bit 1, not logic operation. When set the blend packet is sent | `__glCoEnableBlend` |
| 22 | alpha test: RGB mode, enable-word bit 0 | `__glCoEnableAlphaTest@0x6902c674` |
| 21 | fog: not colour-index mode, enable-word bit 5. When set the fog colour is sent | `__glCoEnableFogTest@0x6902cbec` |
| 20 | colour matrix on: pixel transfers set it when the context's colour-matrix flag (`+0x2db52`) is set and send the matrix; the display driver's YUV blit sets it (`0x1c0000`) | `__glCoLinearToTileXfer@0x6903b706`, `CoYUVBlt_S2S` |
| 19 | pixel transfer (the vendor's `enPixelXfer`): set only together with bit 18 (`enTexture`), in every pixel transfer and copy (`0xc0000`: the miniport copy, the display driver's copy, bitmap and YUV paths, the ICD's three transfer routines), so a transfer runs through the texture path. The glyph blit sets 22, 19 and 18 (`0x4c0000`). Inferred (vendor name; setters read) | `__glCoLinearToTileXfer@0x6903b6f9`, `__glCoTileToLinearXfer@0x6903c92a`, `CoCopyRect` |
| 18 | texture enabled: set by `__glCoPickTextureProcs` when `coSetTexParam` succeeds; the display driver sets it alone (`0x40000`) for pattern fills | `__glCoPickTextureProcs@0x6901db08` |
| 17 | separate specular colour sum: `__glCoEnableColorSum` sets it from enable-word bit 25 (`+0x1c4f` bit 1) unless the lighting-path byte `+0x2d5e6` has bit 7 set; `LoadLightPath` sets or clears it from bit 28 of `+0x3d80` | `__glCoEnableColorSum@0x690266ac`, `LoadLightPath@0x690267ff` |
| 16 | smooth shading: `gc+0x1aa8` is `0x1d01` | `__glCoShadeModel@0x6902ce10` |
| 15 | opaque stipple (the vendor's `enOpaqStipple`): not set by any OpenGL routine. In the display driver it is set together with bit 14 or 13 in all five paths that also send the colour register `0x91ca1020` (`0xc000`, `0xa000`) and never otherwise: the pixels the pattern leaves clear are written with that colour. Inferred | `vFillStip`, `vXfer1bpp`, `bFillSetup`, `DrvFillPath`, `bPutGlyphInCache` |
| 14 | polygon stipple: enable-word bit 13 | `__glCoEnablePolygonStipple@0x69024404` |
| 13 | line stipple: set when the line pattern is not `0xffff`, cleared in every pick; the display driver sets it alone (`0x2000`) for styled lines, 1 bit per pixel transfers and text | `__glCoPickLineProcs@0x690355ad`, `DrvStrokePath` |
| 12:0 | never set by the OpenGL driver, the miniport or the display driver; the vendor's type declares the thirteen bits `reserved`. Inferred (declaration); the hardware's use is unknown | |

Whole-word rules read in disassembly: a pixel transfer keeps bits 31:21 of the word and clears bits 20:0 before it adds
bits 19:18 and 20 (`__glCoLinearToTileXfer@0x6903b65f`..`0x6903b6f9`). The OpenGL driver sets the word to zero for the
span of a region copy and restores it (`__glCoDrawBufferRegion@0x6902309e`).

### Instance bits per block

| Block | Instance field | Rating |
|---|---|---|
| light vectors, colours and scalar (`0x09a900c0`, `0x0ca96120`, `0x1129f020`) | light index 0 to 3 (the loops stop at four lights): copy `i` is `A` + `i * n` in the four-copy bank; `B` is shared | what is written: established (`LoadLightSource@0x69026d06`, `__glCoSetVertexAttribsMask@0x6902d69e`); the bank reading: inferred |
| vertex ports A and B | bit 1 (`0x02`) the colour (four dwords) follows the header; bit 2 (`0x04`) the normal (three dwords); bit 3 (`0x08`) the texture coordinate (two dwords) goes last; bit 4 (`0x10`) one w dword goes after the colour and normal and before x, set when bit 30 of the draw flags is clear. Bit 0 is never set | what is written: established (`__glCoSetVertexAttribsMask@0x6902d69e`); the engine's reading: inferred |
| diffuse (`0x06c6d08x`, `0x07a7408x`) | 0 or 1; 1 only in the generated handlers and the list conversion, for colour-material mode ambient and diffuse | what is written: established; meaning: inferred, "also the ambient colour" |
| every other header | 0 | established |

### Window parameter packets

All read in disassembly: `CoBuildWinparamsPacket@0x16e0a`, `CoBuildGdiWinparamsPacket@0x16d9e`,
`CoBuildWinparamsPacketForPxfer@0x16f7a`, `pCoSetCtxMask@0x19b5e` and the display driver's `vFillSolid@0x11e8c`.

| Dword | Register (`A`, `B`) | Content |
|---|---|---|
| 0 of `0x98250160` | `0x4c1`, `0x50` | the clip-rectangle valid mask (the window record's `scrMaskMode`, `+0x198`): thermometer coded, rectangle `j` of five contributes bit `27 + j`. The miniport takes it from a six-entry table indexed by the rectangle count (`_lHWClipMode`: `0`, `0x08000000`, `0x18000000`, `0x38000000`, `0x78000000`, `0xf8000000` at `sgcomp.sys@0x104e0`; read from the file); more than five rectangles use the one-rectangle value with the window's bounding box; a zero count, or the window record's `bLostSurface` (`+0x68`), uses the one-rectangle value with an empty rectangle (minimum `0x00020002`, maximum `0x00010001`) (`pCoSetCtxMask@0x19b5e`). The pixel-transfer variant sends `0x0c000030` (bits 27, 26, 5, 4) when the context flag at `+0x104` bit 5 is set, else 0; the meaning of bits 26, 5 and 4 is unknown |
| 1 to 10 | bulk range, `B` = `0x51` to `0x5a` | the record's `scrMask` (`+0x19c`): five pairs, minimum then inclusive maximum, of the window's clip rectangles, each corner `y << 16 \| x` with the maximum already reduced by one. The pixel-transfer variant sends one pair, minimum `0xf800f800`, maximum `0x07ff07ff`, then zeros. `pCoSetCtxMask` writes only the pairs for the rectangles it has and the valid mask hides the rest; whether the others are zero is not established |
| `0x9845b020` payload | `0x4c2`, `0x5b` | the window origin `y << 16 \| x`. The miniport adds `0x04000400` (1024 on each axis) to the origin it keeps in the window record; the display driver sends its origin without the bias, and 0 after a full-screen blit. The miniport sends 0 in the GDI mode (context type 3) and `0xf800f800` or 0 in the pixel-transfer variant |
| `0x9865c020` payload | `0x4c3`, `0x5c` | the record word at `+0x194`, the vendor's `cidMode`. The miniport never writes it; the display driver's `vGetCID@0x18190` does, through the pointer the miniport returned for the window record: `(ppdev[+0x6fc] << 16 | id) << 10 | 0x03fc0200`, where `id` is a clip ID from 1 to `0xfe` taken from a bit map in the display driver's shared device record (`ulCIDEnable`, eight dwords, bits 1 to `0xfe` searched in order); the routine's "none free" branch, which would store the bare word `0xff`, is unreachable (the search loop ends at `0xff` and the test is for `0x100`), so with every ID taken the word is built with ID `0xff` and no bit is set and `ppdev[+0x6fc]` (`ulCidDescId`) is the constant 10 that `bInitializeModeFields` stores. `vUpdateCIDs@0x18286` asks for an ID only for a window that has more than five clip rectangles and paints it into the screen with `vPaintCID@0x1824e` (a solid fill of the window's rectangles); `vFreeCID@0x18208` clears the word and returns the ID. Zero for a window that needs none. Established (the three routines read); what the engine does with the ID is inferred from the white paper's "window clip IDs" |

The ten dwords after the first are consecutive in `B`; the bulk packet and the two single-register packets overlap in `A`
but not in `B` (see [`B`](#b-is-the-index-in-the-engines-register-file)). The engine's use (clipping to the union of the
rectangles, the origin applied to vertices) is inferred from the names the white paper gives (screen masks and window
clip IDs).

### Texture-table packets (class 11)

All read in disassembly: `CoBuildTlbPacket@0x16a7a`, `CoBuildUserTexTlbPacket@0x17a46`, `pCoMakeTlbDesc@0x170a2`,
`iCoAllocateTilesFromBank@0x1b780`, `iCoAllocateDynamicTiles@0x1b896`.

- **Translation-table entry** (window `0x8000 + 8 * i`, 2048 entries; one entry per tile, the tile being the 1, 4, 16, 64 or 256 pages the buffer's descriptor gives): the low 24 bits of the tile record (`CO_TILELIST.tileAndBank`), which is
  `(bank base >> 12) + page index` for pages taken from the loader's bank list and `physical address >> 12` for pages
  the miniport allocates at run time; bits 31:24 of the record (the bank number, or `0xfd` / `0xfe` for dynamic pages)
  are masked off before the dword is sent. So an entry is the 4 KB-page frame number of the first page of a tile. The engine's reading: inferred.
- **Buffer descriptor** (the vendor's `CoTlbDescReg`; window `0xc000 + 8 * k`, 64 entries, `k` the six-bit descriptor id the colour, depth and texture format words carry in 24:19, 28:23 and 24:19): bits 31:21 the buffer's first translation-table slot (the running sum of the earlier buffers' slot counts), bits 20:14 `tilesPerRow` (the buffer's row length in tiles), bit 13 `tileWidth` (0 for a 128-byte tile, 1 for 512), bits 12:10 `pageSize` (log4 of the pages in a tile: 1, 4, 16, 64, 256 pages give 0 to 4) and bit 9 `pageType`. `pCoMakeTlbDesc@0x170a2` builds the low bits from the buffer record: the buffer record's `cgb_pagespertile` (`CGBUFFER`, `+0x24`) gives `pageSize`; if the record's `cgb_flags` bits 2:1 equal 2 (a user buffer the kernel mapped linearly, `CobaltStartIO@0x1301b`) it sets bit 19 (the page-size bits stay), which is `tilesPerRow` = 32 with a 128-byte tile (one 4 KB page per row), and stops; otherwise `cgb_tilewidth` (`+0x1c`) sets bit 13 for 512, nothing for 128 and bit 9 for any other value, and `cgb_widthintiles` (`+0x20`, the row length) is shifted into bit 14. `cgb_widthintiles` comes from the second input dword of `CO_CMD_CREATE_BUFFER` (`CobaltStartIO@0x13153`, `pAllocateTilesForBuffer`): the display driver's `BufferTileAlloc@0x1da64` passes the tile count per row, `ceil(row bytes / tile width)`, and `CoGrmAllocateTiledBuffer@0x19b28` rejects a count above `0x7f` after turning 128 columns of 128 bytes into 32 columns of 512, so the field is seven bits wide, bits 20:14, not five. The packet pool's record has width 0, row length 1 and 256 pages, so its descriptor has bit 9, bit 14 and `pageSize` 4. Descriptor 0 is the pool: `CoInitHw@0x1712a` loads the pool's 16 tiles of 256 pages into the first 16 translation entries and the pool's descriptor into entry 0, so the engine fetches packets through the table. User textures are placed from the top of the table: slot `0x800 - total` upward (`CoBuildUserTexTlbPacket`). Established (routines read); the engine's reading is inferred.
- A bind is: a flush marker, the one-dword descriptor writes for the slots that changed (the miniport keeps a shadow
  and skips unchanged ones), the translation-table writes in groups of at most 64, and a chain packet.

## The vendor's register word types

The program database of each driver declares the words the packets load as bit-field types ([the OpenGL driver
page](../../software/windows/nt-opengl-icd.md#the-program-databases-types) says how they were read). Every
field below was compared with the routines that write the word; where the rows above differ from an earlier
reading it is because the declaration and the code agree against it. The fields are in
[the register definitions](../rendering-engine/registers.md); this table says which header carries which type.

| Header | Vendor type | What the comparison with the writers settled |
|---|---|---|
| `0x00210020` | `CoXformModeReg` | bit 31 viewport, 30 model, 29 texture, 28 clip check; the row above |
| `0x80003020` | `CoDrawModeReg` | all 19 enable bits match the setter table below, and bits 28, 19 and 15 get their names; bits 12:0 are declared reserved |
| `0x18002020` | `CoPrimModeReg` | the primitive type in 31:29, the line width in 26:15, the cull field in 14:13 match [the primitive mode word](draw-and-buffers.md#primitive-mode-word); bit 28 is the anti-aliased line flag, 27 closed line, 12 front face, 11 back-face line, 10:9 primitive direction, 8 direction select |
| `0x05260020` | `CoLightModeReg` | bit 31 two-sided, 30:27 the four light enables, 26 normalise, 25 specular, 24 specular-only, 23 lighting; the page's bit 26 (the normalise enable) is that field |
| `0x91aa0020` | `CoColorFormatReg` | linear 31, colour type 30:27, depth 26:25, descriptor id 24:19, span mode 18: the layout the clear routine already decodes |
| `0x930ab020` | `CoSzFormatReg` | type 31, depth 30:29, id 28:23 |
| `0x924a5040` | `CoColorTestReg` | the alpha test: enable-compare 31:28 and function 27:25, then the reference colour |
| `0x928a7040` | `CoBlendReg` | destination 31:28, source 27:24, equation 23:20, then the constant colour: the layout read at `sgcoicd.dll@0x6902c8f3` |
| `0x92ca9020` | `CoLogicOpReg` | operation 31:28 |
| `0x936ae020` | `CoDepthTestReg` | function 31:29, initialise-occlusion 28, enable-occlusion 27, equal range 26:23: the "mode in 28:27" of the row is the two occlusion-test bits |
| `0x938af040` | `CoStencilModeReg` | reference 7:0, mask 15:8, function 19:16, fail 23:20, depth-fail 27:24, pass 31:28, then one more word |
| `0x91ea2040` | `CoTexEnvParamReg` | mode and colour; the row above |
| `0x93cc0080` | `CoTexGenParamReg` | format, mode, level of detail, border colour; the row above |
| `0x99ce00a0`, `0x9e7200a0` | `CoClearModeReg` | descriptor, mask, value, two corners |
| `0x84e44040` | `CoClipRectReg` | the scissor corners |
| link words | `CoPktDescType` | bit 0 ready, bit 1 interrupt, bit 2 sync, bits 11:3 size, bits 31:12 next |

The codes each of these words carries for the GL comparison functions, blend equations and factors, logic operations and stencil operations are in [draw-and-buffers.md](draw-and-buffers.md#function-and-operand-encodings), and the texture words in [textures.md](textures.md). The stencil, alpha-test and blend second words and the level-of-detail word are written by the OpenGL driver only; the
display driver and miniport leave them at the values listed. Established (the declarations read in full, the writers
named in the rows read); the engine's behaviour for any field is inferred.

## Coverage

Constant header values found in `sgcoicd.dll` (Windows NT 4.0 5.2.4 and Windows 2000 5.2.4, identical sets): 52 (32 in
class `00` and 20 in the register file; a count variant such as `0x0666a060` and `0x0666a0e0` is a separate value, an
instance or attribute-mask variant counts with its base). Each has an emitting routine established and a payload
layout read. Computed forms: the vertex port plus attribute mask (constants seen: `0x18a33060`, `0x18a33068`,
`0x18c33060`, `0x18c33062`, `0x18c33070`; computed: the port plus any combination of `0x02`, `0x04`, `0x08`, `0x10` as
`__glCoSetVertexAttribsMask` composes it), the light blocks plus light index, the second clear bank (`0x9e7200a0`), and
the class `11` table loads (miniport only). Not emitted by the ICD but by the miniport: `0x97000020`, `0x97000060`,
`0x98250160`, `0x9845b020`, `0x9865c020`, class `11`. The display driver adds `0x91ca1020` and uses 22 of the other
headers; the miniport uses 15. The count is of distinct header values from the tables above that occur as an immediate
operand in a linear disassembly sweep of `.text`, with payload constants such as `0x3f000000` left out. The same scan of the Windows 2000 display driver and miniport found the same headers and no others (a raw search of each file for the
header dwords, repeated in the typed re-check, gives identical sets for the NT 4.0 and Windows 2000 builds of both). The April 1999 builds
found no header outside the set but are smaller: the miniport has no long chain packet `0x97000060`, and the display driver has no blend packet `0x928a7040`. The April 1999 OpenGL driver has 50 of the 52 base values as immediates (no `0x97400020`, no
`0x99ce00a0`; the rectangle clear is still emitted there, built by arithmetic from the immediates `0xfb5c0000` and `0x618dff60`, and neither `0x99ce00a0` nor `0x9e7200a0` occurs as a raw dword in that image), and no header outside the set ([nt-opengl-icd.md](../../software/windows/nt-opengl-icd.md#the-april-1999-build)).

**Withdrawn.** The earlier page listed `0x9400014f` as a span header written by `__glStoreSpanARGB8`. The bytes occur
once in the image, at `sgcoicd.dll@0x690f2681`, in the export address table (they straddle two of its 15 entries), not
in the import tables, and no routine stores that value, so it is not a header the driver emits. Also `0x04a40000` is
not a header: the clear routine adds it to `0x99ce00a0` to pick the second bank.
