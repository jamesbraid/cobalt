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
window offset `0x8000 + 8 * i`; position `0x1800 + k` is the base-pointer entry at `0xc000 + 8 * k`. Established as what
the builders write; the engine's parsing is inferred.

**Class `10` offsets.** `0x18000 + 8 * (A - 0x400)` is the window offset of the first register written, with 8-byte
spacing and the payload dwords going to consecutive registers. Three matches with the miniport tie this down: the
scissor `0x84e44040` (`A` = `0x427`) is `0x18138` and `0x18140`, the two registers the miniport reads into a packet
headed `0x84e44040` when it saves a context; the chain packet `0x97000040` (`A` = `0x4b8`) is `0x185c0` and `0x185c8`;
`0x97400020` (`A` = `0x4ba`) is `0x185d0`, written with the current packet pointer. Inferred.

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
   driver: `B` = `0xa1` / `A` = `0x48e`, header `0x91ca1020` (`sgcodd.dll@0x1229c` and four other sites). The other gap
   in that run is `B` = `0xa3` (`A` = `0x490`), not seen in any binary.
6. The low `B` values (0 to 4) are not part of a run: the chain and packet-pointer registers are both `B` = 0 with
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
| `0x00210020` | `0x001` | `0x10` | 1 | draw flags word | `__glCoClipTriangle`, `__glCoClipLine`, `__glCoDoPolygonClip`, `__glCoSetVertexAttribsMask`, `__glCoDlistExecutePackets`, `___glcoim_Begin` and others | Bit 30 says the vertex packets carry the four words at vertex +0 (the engine transforms them), bit 31 the four at +0x20, neither the window x, y, z and the context's w. Bit 30 wins when both are set. Bit 29 is cleared on both paths that leave bits 31:30 non-zero (`___glcoim_Begin@0x6904327f`, `@0x690432a1`); nothing read sets it. The rule that sets them is in [draw-and-buffers.md](draw-and-buffers.md#who-does-what) |
| `0x00411200` | `0x002` | `0x11` | 16 | model-view-projection matrix | `__glCoPassMVPMatrix` | 16 floats copied from the transform stack, column by column |
| `0x02421120` | `0x012` | `0x21` | 9 | normal matrix | `__glCoPassNormalMatrix` | 3 by 3 floats |
| `0x0362a0c0` | `0x01b` | `0x2a` | 6 | viewport | `__glCoSetViewMatrix` | three scale floats then three offsets; the x and y offsets have a sub-pixel constant subtracted |
| `0x04230040` | `0x021` | `0x30` | 2 | x and y bias | `coLoadLinearTlb`, `coSetupTileToTile`, `coSetTexParam`; miniport `CoCopyRect` and the window packets | two floats. The miniport's copy sends `0.5` for both (`0x3f000000`); the ICD's pixel-transfer set-up adds a constant to both |
| `0x04640020` | `0x023` | `0x40` | 1 | line stipple repeat | `__glCoBeginLStrip`, `__glCoBeginLLoop`, `__glCoRenderAliasLine`, `__glCoRenderBitmap`, `__glCoWideStippledAliasLine`, `FillAASubTriangle`; display driver `DrvStrokePath`, `DrvTextOut` | the GL repeat factor, expanded |
| `0x04841020` | `0x024` | `0x41` | 1 | line stipple pattern | same as above | the 16-bit GL pattern in both halves |
| `0x04a42040` | `0x025` | `0x42` | 2 | polygon stipple | `__glCoConvertStipple`, `__glCoFillStippledTriangle`; display driver `vFillStip`, `bFillSetup`, `DrvFillPath` | an 8 by 8 bit pattern. `__glCoConvertStipple` reverses the bits of each byte of the 32 by 32 GL pattern and sends this only if the pattern is the same 8 by 8 block repeated; otherwise it falls back to host stippling (read in full) |
| `0x05260020` | `0x029` | `0x60` | 1 | lighting mode word | `LoadLightPath`, `__glCoClearBuffers`, `__glCoClipTriangle`, `__glCoRenderAAPoint` and others | one word kept in the context (`+0x2d5e4`): bit 31 is the lighting-model flag byte at `+0x1abd`, bit 26 is bit 16 of the context's enable word (`+0x1c4e` bit 0), bit 23 is set by `LoadLightPath` when lighting is enabled and the engine path is used and cleared on the host-lighting path ([draw-and-buffers.md](draw-and-buffers.md#lighting-and-material-state)); `LoadLightPath@0x690267b4` read in full, no other bit is set |
| `0x05461060` | `0x02a` | `0x61` | 3 | scene ambient | `LoadLightPath` | the GL light-model ambient colour (RGB floats) |
| `0x05a64060` | `0x02d` | `0x64` | 3 | emission (front) | `ChangeMaterialEmission`, `__glCoValidateMaterial` | RGB floats |
| `0x06067060` | `0x030` | `0x67` | 3 | emission (back) | same | RGB floats |
| `0x0666a060`, `0x0666a0e0` | `0x033` | `0x6a` | 3, 7 | ambient (front); ambient then diffuse | `ChangeMaterialAmbient`, `ChangeMaterialAmbientAndDiffuse`, `__glCoValidateMaterial` | ambient RGB; the 7-word form carries diffuse RGBA after it |
| `0x06c6d080`, `0x06c6d081` | `0x036` | `0x6d` | 4 | diffuse (front) | `ChangeMaterialDiffuse`, `__glCoGenerateNoXformHandler`, `GenerateSetAttribs`, `__glCoDlistToPackets` | RGBA. Instance 1 appears only in the generated handlers and the list conversion, in the colour-material mode "ambient and diffuse" (`gc+0x1aa4` = `0x1602`) when a context byte is set; its meaning is unknown |
| `0x07471060`, `0x074710e0` | `0x03a` | `0x71` | 3, 7 | ambient (back); ambient then diffuse | as `0x0666a060` | as `0x0666a060` |
| `0x07a74080`, `0x07a74081` | `0x03d` | `0x74` | 4 | diffuse (back) | as `0x06c6d080` | as `0x06c6d080` |
| `0x08278080` | `0x041` | `0x78` | 4 | specular and shininess (front) | `ChangeMaterialSpecular`, `__glCoValidateMaterial` | specular RGB then the shininess |
| `0x08a7c080` | `0x045` | `0x7c` | 4 | specular and shininess (back) | same | as above |
| `0x09280080` | `0x049` | `0x80` | 4 | current colour | `__glCoSetVertexAttribsMask`, `LoadLightPath`, `__glCoClearBuffers`, the span and bitmap routines | RGBA floats |
| `0x09a900c0` \| `i` | `0x04d` | `0x90` | 6 | light vectors, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | six floats: the light position or direction and a second vector. For a positional light the per-vertex routine sends it again for each vertex |
| `0x0ca96120` \| `i` | `0x065` | `0x96` | 9 | light colours, light `i` | `LoadLightSource` | ambient, diffuse and specular RGB of GL light `i` |
| `0x1129f020` \| `i` | `0x089` | `0x9f` | 1 | light scalar, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | 1.0 for a directional light, otherwise a value from the light's attenuation fields; sent per vertex for positional lights |
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
| `0x80003020` | `0x400` | `0x03` | `0x18000` | 1 | pixel-pipe mode word | the `__glCoEnable*` routines, `__glCoPickLineProcs`, `__glCoPickTriangleProcs`, `__glCoPickTextureProcs`, `__glCoPassColorMask`, the span stores, `__glCoClearBuffers`, `__glCoDlistExecutePackets`, `___glcoim_Begin`, the display driver in about forty places, miniport `CoCopyRect` | one 32-bit flag word kept in the context (`+0x2db9c`); every bit is decoded [below](#pixel-pipe-mode-word-0x80003020) |
| `0x84e44040` | `0x427` | `0x44` | `0x18138` | 2 | scissor box | `__glCoSetScissor`, `__glCoDrawBufferRegion`, `__glCoFillStippledTriangle`, the transfer routines, the miniport | low 16 bits x, high 16 bits y; first word the minimum corner, second the inclusive maximum (decompiled C only; the window origin and a vertical flip are folded in). The miniport uses `0xf000f000` and `0x0fff0fff` (signed 16-bit, plus or minus 4096) for "everything", and `0xf800f800` / `0x07ff07ff` in the pixel-transfer window packet |
| `0x91aa0020` | `0x48d` | `0xa0` | `0x18468` | 1 | draw-buffer descriptor | `__glCoSetDrawBuffer`, `__glCoLinearToTileXfer`, `__glCoTileToTileXfer`, `coTileToLinearSpans`, the miniport, the display driver | checked in disassembly: bits 24:19 are the low six bits of the first dword of the buffer's record; bits 30:25 hold the colour format (below); bit 31 is preserved by `__glCoInitRGB@0x69044c85`..`0x69044cc4` and never set by it. Always follows a `0x98804000` |
| `0x91ca1020` | `0x48e` | `0xa1` | `0x18470` | 1 | constant colour | display driver: `vFillStip`, `vXfer1bpp`, `bFillSetup`, `DrvFillPath`, `bPutGlyphInCache` | the foreground colour for stipple fills, 1 bit per pixel transfers and the glyph cache: the colour dword of the brush or text colour (converted to a packed form first for pixel-format code 4, `sgcodd.dll@0x15457`), or `0xff` for the glyph cache. Always used together with mode-word bit 15 and bit 13 or 14 (inferred from five sites). The OpenGL driver never sends it |
| `0x91ea2040` | `0x48f` | `0xa2` | `0x18478` | 2 | transfer parameters 2 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam`, miniport `CoCopyRect` (`0x62000000`, 0) | word 1 from the ICD is `(0x30 + (f & 0xf)) << 25`: bits 30:29 set and a four-bit context field `f` in 28:25; the second word is 0 |
| `0x922a4020` | `0x491` | `0xa4` | `0x18488` | 1 | fog colour | `__glCoPassFog` | three bytes in 31:8 |
| `0x924a5040` | `0x492` | `0xa5` | `0x18490` | 2 | alpha test | `__glCoPassAlphaFunc`, display driver | word 1: bit 31 set, function in 27:25; word 2: reference in 7:0 |
| `0x928a7040` | `0x494` | `0xa7` | `0x184a0` | 2 | blend | `__glCoPassBlend`, display driver | word 1: equation in 23:20 (from `+0x1c78`), source factor in 27:24 (from `+0x1c70`), destination factor in 31:28 (from `+0x1c74`), read in disassembly at `sgcoicd.dll@0x6902c8f3`..`0x6902c919`; `___glcoim_BlendFunc@0x690289d0` stores the source to `+0x1c70` and the destination to `+0x1c74`. Word 2: constant colour, alpha in 7:0. |
| `0x92ca9020` | `0x496` | `0xa9` | `0x184b0` | 1 | logic operation | `__glCoPassLogicOp`, display driver (a ROP table) | operation in 31:28 (GL enum minus `0x1500`) |
| `0x92eaa020` | `0x497` | `0xaa` | `0x184b8` | 1 | colour write mask | `__glCoPassColorMask`, the transfer and alpha-bit routines | 32-bit mask, red in the top byte |
| `0x930ab020` | `0x498` | `0xab` | `0x184c0` | 1 | depth and stencil buffer descriptor | `__glCoSetSzBuffer` | checked in disassembly: bits 28:23 are the low six bits of the first dword of the buffer's record; bits 31:29 select the depth format (below). Always follows a `0x98804000` |
| `0x932ac040` | `0x499` | `0xac` | `0x184c8` | 2 | polygon offset | `__glCoPassPolygonOffset` | checked in disassembly: word 1 is the first argument of `glPolygonOffset` (the factor) unchanged, word 2 is the second (the units) times a scale kept in the context |
| `0x936ae020` | `0x49b` | `0xae` | `0x184d8` | 1 | depth test | `__glCoPassDepthTest`, `__glCoClearBuffers`, `StoreALWAYS` | function in 31:29 (GL function minus `0x200`; the clear sends 7, "always"), mode in 28:27 |
| `0x938af040` | `0x49c` | `0xaf` | `0x184e0` | 2 | stencil | `__glCoPassStencil`, `__glCoClearBuffers`, `__glCoSpanRenderStencil`, `StoreStencil` | word 1: three 4-bit operation codes, a 3-bit compare function and two bytes of state; word 2 one more byte |
| `0x93cc0080` | `0x49e` | `0xc0` | `0x184f0` | 4 | transfer parameters 1 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam`, display driver, miniport | word 1 from the ICD is `(((((a & 0xf) + (b << 4)) << 2) + (c & 3)) << 6) + (d & 0x3f)` shifted left by 19: `d` in 24:19, `c` in 26:25, `a` in 30:27, low bit of `b` in 31. The miniport and display driver send `(source buffer id << 19) \| low bits`: the source buffer of a copy. Words 2 to 4 are 0 |
| `0x944c42c0` | `0x4a2` | `0xc4` | `0x18510` | 22 | colour matrix | `coSetColorMatrixRegisters`, display driver `CoYUVBlt_S2S` | 20 floats from the context (a 4 by 4 matrix and four more), two more dwords |
| `0x97000020` | `0x4b8` | `0x00` | `0x185c0` | 1 | chain, short form | miniport only | next position |
| `0x97000040` | `0x4b8` | `0x00` | `0x185c0` | 2 | chain | `ClosePacket`, `DispatchCurrentPkt`, `__glCoDlistExecutePackets`, `coCreateTexBindPkt`, `coDispatchTexBindPkt`; display driver `pulGetPacket`. The miniport sends only the short and long forms | next node position, this node position ([graphics-engine.md](graphics-engine.md)) |
| `0x97000060` | `0x4b8` | `0x00` | `0x185c0` | 3 | chain, long form | miniport only (`pCoContextSwitch`, `CoBuildUserTexTlbPacket`) | `CoBuildUserTexTlbPacket` sends the node position shifted left 12, 0 and a word from the texture record (`sgcomp.sys@0x17a6d`..`0x17a86`); the third word's meaning is unknown |
| `0x97400020` | `0x4ba` | `0x00` | `0x185d0` | 1 | packet pointer reset | `__glCoCreateTexLevel` | the payload is always 0. The ICD sends it, then runs its overflow routine, before it frees storage of the texture that is currently bound. The miniport writes the same register with the current packet pointer |
| `0x98250160` | `0x4c1` | `0x50` | `0x18608` | 11 | window parameters | miniport (`CoBuildWinparamsPacket`, `CoBuildWinparamsPacketForPxfer`), display driver | a clip-rectangle valid mask and five (minimum, inclusive maximum) corner pairs; see [window parameters](#window-parameter-packets) |
| `0x9845b020` | `0x4c2` | `0x5b` | `0x18610` | 1 | window origin | miniport, display driver | the window origin, y in 31:16 and x in 15:0 ([below](#window-parameter-packets)) |
| `0x9865c020` | `0x4c3` | `0x5c` | `0x18618` | 1 | window parameter | miniport only | a word from the window record at `+0x194`; the miniport never writes that field, and the packet is zero in every path read |
| `0x98804000` | `0x4c4` | `0x04` | `0x18620` | 0 | flush marker | `DispatchCurrentPkt`, every descriptor and mode write, the span stores; miniport and display driver | no payload. The meaning is unknown: the miniport never touches `0x18620`. It is placed before every descriptor or mode write, which says where software wants it, not what the engine does with it |
| `0x99ce00a0` | `0x4ce` | `0xe0` | `0x18670` | 5 | clear rectangle | `coClearBuffers`, miniport `CoClearRect`, display driver `CoRectFill` and `vClearSolid` | see [draw-and-buffers.md](draw-and-buffers.md#clears) |
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
| 28 | set by `__glCoPickTriangleProcs` when any of bits 20:18 of the context word `+0x3d80` is set and a local flag derived from the polygon-mode state is nonzero; cleared at the start of each pick. Meaning unknown | `__glCoPickTriangleProcs@0x6903517b` |
| 27 | the colour write mask is all zero (`+0x2e31c` is 0) | `__glCoPassColorMask@0x6902ccda` |
| 26 | partial colour write mask: set when `__glCoPassColorMask` sends `0x92eaa020` because the mask is neither all zero nor all ones (`push 1` at `@0x6902cd71`, `pop ebx` at `@0x6902cd7e`, `and ebx,1` / `shl ebx,0x1a` at `@0x6902cde6`); cleared otherwise. The transfer routines read it and merge it into the word they send (`__glCoLinearToTileXfer@0x6903b61f`, `@0x6903b6a6`) | same |
| 25 | dither: enable-word bit 3 | `__glCoEnableDither@0x6902c9ba` |
| 24 | logic operation: enable-word bit 2 in colour-index mode; in RGB mode set by `__glCoEnableBlend` when enable-word bit 21 is set, or blend is on and the blend-equation field is `0xbf1`. Setting it clears bit 23 | `__glCoEnableLogicOp@0x6902ca54`, `__glCoEnableBlend@0x6902c712` |
| 23 | blend: RGB mode, enable-word bit 1, not logic operation. When set the blend packet is sent | `__glCoEnableBlend` |
| 22 | alpha test: RGB mode, enable-word bit 0 | `__glCoEnableAlphaTest@0x6902c674` |
| 21 | fog: not colour-index mode, enable-word bit 5. When set the fog colour is sent | `__glCoEnableFogTest@0x6902cbec` |
| 20 | colour matrix on: pixel transfers set it when the context's colour-matrix flag (`+0x2db52`) is set and send the matrix; the display driver's YUV blit sets it (`0x1c0000`) | `__glCoLinearToTileXfer@0x6903b706`, `CoYUVBlt_S2S` |
| 19 | set only together with bit 18, in every pixel transfer and copy (`0xc0000`: the miniport copy, the display driver's copy, bitmap and YUV paths, the ICD's three transfer routines). The glyph blit sets 22, 19 and 18 (`0x4c0000`). Meaning unknown | `__glCoLinearToTileXfer@0x6903b6f9`, `__glCoTileToLinearXfer@0x6903c92a`, `CoCopyRect` |
| 18 | texture enabled: set by `__glCoPickTextureProcs` when `coSetTexParam` succeeds; the display driver sets it alone (`0x40000`) for pattern fills | `__glCoPickTextureProcs@0x6901db08` |
| 17 | separate specular colour sum: `__glCoEnableColorSum` sets it from enable-word bit 25 (`+0x1c4f` bit 1) unless the lighting-path byte `+0x2d5e6` has bit 7 set; `LoadLightPath` sets or clears it from bit 28 of `+0x3d80` | `__glCoEnableColorSum@0x690266ac`, `LoadLightPath@0x690267ff` |
| 16 | smooth shading: `gc+0x1aa8` is `0x1d01` | `__glCoShadeModel@0x6902ce10` |
| 15 | not set by any OpenGL routine. In the display driver it is set together with bit 14 or 13 in all five paths that also send the constant-colour register `0x91ca1020` (`0xc000`, `0xa000`) and never otherwise. Inferred: take the colour from that register | `vFillStip`, `vXfer1bpp`, `bFillSetup`, `DrvFillPath`, `bPutGlyphInCache` |
| 14 | polygon stipple: enable-word bit 13 | `__glCoEnablePolygonStipple@0x69024404` |
| 13 | line stipple: set when the line pattern is not `0xffff`, cleared in every pick; the display driver sets it alone (`0x2000`) for styled lines, 1 bit per pixel transfers and text | `__glCoPickLineProcs@0x690355ad`, `DrvStrokePath` |
| 12:0 | never set by the OpenGL driver, the miniport or the display driver | unknown |

Whole-word rules read in disassembly: a pixel transfer keeps bits 31:21 of the word and clears bits 20:0 before it adds
bits 19:18 and 20 (`__glCoLinearToTileXfer@0x6903b65f`..`0x6903b6f9`). The OpenGL driver sets the word to zero for the
span of a region copy and restores it (`__glCoDrawBufferRegion@0x6902309e`).

### Instance bits per block

| Block | Instance field | Rating |
|---|---|---|
| light vectors, colours and scalar (`0x09a900c0`, `0x0ca96120`, `0x1129f020`) | light index 0 to 3 (the loops stop at four lights): copy `i` is `A` + `i * n` in the four-copy bank; `B` is shared | what is written: established (`LoadLightSource@0x69026d06`, `__glCoSetVertexAttribsMask@0x6902d69e`); the bank reading: inferred |
| vertex ports A and B | bit 1 (`0x02`) the colour (four dwords) follows the header; bit 2 (`0x04`) the normal (three dwords); bit 3 (`0x08`) the texture coordinate (two dwords) goes last; bit 4 (`0x10`) one w dword goes after the colour and normal and before x, set when bit 30 of the draw flags is clear. Bit 0 is never set | what is written: established (`__glCoSetVertexAttribsMask@0x6902d69e`); the engine's reading: inferred |
| diffuse (`0x06c6d08x`, `0x07a7408x`) | 0 or 1; 1 only in the generated handlers and the list conversion, for colour-material mode ambient and diffuse | what is written: established; meaning unknown |
| every other header | 0 | established |

### Window parameter packets

All read in disassembly: `CoBuildWinparamsPacket@0x16e0a`, `CoBuildGdiWinparamsPacket@0x16d9e`,
`CoBuildWinparamsPacketForPxfer@0x16f7a`, `pCoSetCtxMask@0x19b5e` and the display driver's `vFillSolid@0x11e8c`.

| Dword | Register (`A`, `B`) | Content |
|---|---|---|
| 0 of `0x98250160` | `0x4c1`, `0x50` | the clip-rectangle valid mask: thermometer coded, rectangle `j` of five contributes bit `27 + j`. The miniport takes it from a six-entry table indexed by the rectangle count (`0`, `0x08000000`, `0x18000000`, `0x38000000`, `0x78000000`, `0xf8000000` at `sgcomp.sys@0x104e0`); more than five rectangles use the one-rectangle value with the window's bounding box; a zero count, or a window flag at `+0x68`, uses the one-rectangle value with an empty rectangle (minimum `0x00020002`, maximum `0x00010001`). The pixel-transfer variant sends `0x0c000030` (bits 27, 26, 5, 4) when the context flag at `+0x104` bit 5 is set, else 0; the meaning of bits 26, 5 and 4 is unknown |
| 1 to 10 | bulk range, `B` = `0x51` to `0x5a` | five pairs, minimum then inclusive maximum, of the window's clip rectangles, each corner `y << 16 \| x` with the maximum already reduced by one. The pixel-transfer variant sends one pair, minimum `0xf800f800`, maximum `0x07ff07ff`, then zeros. `pCoSetCtxMask` writes only the pairs for the rectangles it has and the valid mask hides the rest; whether the others are zero is not established |
| `0x9845b020` payload | `0x4c2`, `0x5b` | the window origin `y << 16 \| x`. The miniport adds `0x04000400` (1024 on each axis) to the origin it keeps in the window record; the display driver sends its origin without the bias, and 0 after a full-screen blit. The miniport sends 0 in the GDI mode (context type 3) and `0xf800f800` or 0 in the pixel-transfer variant |
| `0x9865c020` payload | `0x4c3`, `0x5c` | the record word at `+0x194`; zero in every path read |

The ten dwords after the first are consecutive in `B`; the bulk packet and the two single-register packets overlap in `A`
but not in `B` (see [`B`](#b-is-the-index-in-the-engines-register-file)). The engine's use (clipping to the union of the
rectangles, the origin applied to vertices) is inferred from the names the white paper gives (screen masks and window
clip IDs).

### Texture-table packets (class 11)

All read in disassembly: `CoBuildTlbPacket@0x16a7a`, `CoBuildUserTexTlbPacket@0x17a46`, `pCoMakeTlbDesc@0x170a2`,
`iCoAllocateTilesFromBank@0x1b780`, `iCoAllocateDynamicTiles@0x1b896`.

- **Translation-table entry** (window `0x8000 + 8 * i`, 2048 entries): the low 24 bits of the tile record, which is
  `(bank base >> 12) + page index` for pages taken from the loader's bank list and `physical address >> 12` for pages
  the miniport allocates at run time; bits 31:24 of the record (the bank number, or `0xfd` / `0xfe` for dynamic pages)
  are masked off before the dword is sent. So an entry is a 4 KB-page frame number. The engine's reading: inferred.
- **Base-pointer entry** (window `0xc000 + 8 * k`, 64 entries): a buffer record's descriptor dword with the first
  translation-table slot of the buffer in bits 31:21 (the running sum of the earlier buffers' slot counts, 11 bits).
  `pCoMakeTlbDesc` builds the low bits: bits 12:10 are `log4` of the pages per slot (1, 4, 16, 64, 256 pages give 0 to
  4), bit 19 is set when record flag bits 2:1 equal 2, and then the routine skips the rest (`@0x170f1` jumps to `@0x1711c`). Otherwise bit 13 is set when a record field is `0x200`, bit 9 when it is neither `0x80` nor `0x200`, and a second record field is shifted into bits 18:14. So bit 19 and the group of bits 13, 9 and 18:14 are mutually exclusive. What those two record fields are is
  unknown. User textures are placed from the top of the table: slot `0x800 - total` upward (`CoBuildUserTexTlbPacket`).
- A bind is: a flush marker, the one-dword base-pointer writes for the slots that changed (the miniport keeps a shadow
  and skips unchanged ones), the translation-table writes in groups of at most 64, and a chain packet.

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
operand in a linear disassembly sweep of `.text`, with payload constants such as `0x3f000000` left out. The same scan of the Windows 2000 display driver and miniport, and of the April 1999
builds, found the same headers and no others.

**Withdrawn.** The earlier page listed `0x9400014f` as a span header written by `__glStoreSpanARGB8`. The bytes occur
once in the image, at `sgcoicd.dll@0x690f2681`, in the export address table (they straddle two of its 15 entries), not
in the import tables, and no routine stores that value, so it is not a header the driver emits. Also `0x04a40000` is
not a header: the clear routine adds it to `0x99ce00a0` to pick the second bank.
