# Cobalt rendering engine: the packet set

The header words the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 (and, for the engine-register class, the miniport
`sgcomp.sys` 5.2.4) puts in front of the data it hands the engine, one row per header. Back to the
[overview](README.md); queue, chaining and link words are in [graphics-engine.md](graphics-engine.md); what the draw
and buffer packets do together is in [draw-and-buffers.md](draw-and-buffers.md); how the driver is built and how the
names were applied is in [nt-opengl-icd.md](../../software/windows/nt-opengl-icd.md).

## How the table was made

- **Header and emitting routine: established.** Every dword in the ICD's `.text` and `.data` whose bits 31:12 equal a
  known target was collected from the raw image (not from a disassembly), then attributed to the routine that contains
  it by the vendor's own function names (symbols from the `.dbg` file). Decompiled C of each routine was read in full
  to see the payload; the headers' bytes were spot-checked in disassembly (`__glCoSetDrawBuffer`, `__glCoSetSzBuffer`,
  `__glCoPassPolygonOffset`, `__glcoim_PolygonOffset`).
- **Payload layout: inferred.** Read from decompiled C, not from a complete disassembly, so a layout is inferred
  unless the row says it was checked in disassembly.
- **What the engine does with it: unknown**, except where a driver-side register read or write ties a header to a
  register (the engine-register rows) and where the SGI white paper names the function. A white paper feature that
  matches a row is cited as agreement, never as the meaning.
- Names in the "Name" column are mine; function names are the vendor's.

## Header word layout

A packet is one header dword followed by the payload dwords. The driver pads each packet to 32 bytes.

| Bits | Field | Grade |
|---|---|---|
| 31:30 | class: `00` geometry state and draw ports (every header seen has `A` below `0x100`), `10` engine register file, `11` direct register-window write; `01` not seen | inferred (see below) |
| 31:21 | `A`, start address of the first payload dword | inferred |
| 20:12 | `B`, a second address that tracks `A` inside a block | unknown |
| 11:5 | payload dword count `n` (0 to 127) | inferred; checked against 17 of 18 shadow-table entries earlier, and by every row below whose routine writes exactly `n` dwords (the vertex ports are the exception) |
| 4:0 | instance (see the rows: light index, attribute-presence mask, or a variant flag) | inferred |

**`A` is an address, and a block's count is the step to the next block.** Taking the blocks in `A` order, `A` of the
next block equals `A` plus `n` of the previous: `0x023`+6 = `0x029`, `0x2d`+3 = `0x30`, `0x30`+3 = `0x33`,
`0x36`+4 = `0x3a`, `0x45`+4 = `0x49`, and in the register-file class `0x491`+1, `0x492`+2, `0x494`+2, `0x496`+1,
`0x497`+1, `0x498`+1, `0x499`+2, `0x49b`+1, `0x49c`+2 = `0x49e`. The big shadow packets are bulk loads over a range:
`0x00210440` (`n` = 34) covers `0x001` to `0x022`, which is exactly the union of the smaller blocks at `0x001`,
`0x002` (16), `0x012` (9), `0x01b` (6) and `0x021` (2). Established as arithmetic over the headers listed here.

**Instance banks.** The light blocks come in four copies with the instance field 0 to 3 (the context shadow table has
all four). The step from the first light block (`0x4d`, `n` = 6) to the next (`0x65`) is 24, four copies of six; from
`0x65` (`n` = 9) to `0x89` it is 36, four copies of nine. So `A` is the address of copy 0 and copy `i` sits `i * n`
words on, which makes `A` a physical address in a state RAM and `B` the address with the copies folded (`B` steps by
6 and 9 across the same two blocks). Inferred.

**Class `10` is the register window.** In the register-file class, `0x18000 + 8 * (A - 0x400)` is the offset in the
engine window of the first register written, with 8-byte spacing and the payload dwords going to consecutive
registers. Three independent matches with what the miniport does tie this down:

- scissor `0x84e44040` (`A` = `0x427`, two dwords) is `0x18138` and `0x18140`, the two registers the miniport reads
  into a packet headed `0x84e44040` when it saves a context;
- the chain packet `0x97000040` (`A` = `0x4b8`, two dwords) is `0x185c0` and `0x185c8`, the two registers it reads
  to find where the engine stopped;
- `0x97400020` (`A` = `0x4ba`, one dword) is `0x185d0`, the register it writes with the current packet pointer.

Inferred (three matches, no document). The same layout puts `0x4c4` (the flush marker, no payload) at `0x18620`.

**Class `11` writes the window directly.** `CoBuildTlbPacket` builds two kinds of header by arithmetic, not as
constants: `0xc0000001 + ((0x1800 + k) << 16)` (one dword to index `0x1800 + k`) and `0xc0000000 + ((index + 0x1000) << 16) + count` (up to 64 dwords, each the low 24 bits of a table entry). With an 8-byte unit these are byte offsets
`0xc000 + 8k` (the per-context base-pointer table) and `0x8000 + 8 * index` (the translation table), the same
two ranges the miniport fills at start-up ([graphics-engine.md](graphics-engine.md)). Inferred. The ICD never emits
class `11`; the miniport builds it into the texture table packets the ICD chains to.

## The table

`n` is the count field. "ICD" means `sgcoicd.dll` 5.2.4; "miniport" means `sgcomp.sys` 5.2.4; the display driver
`sgcodd.dll` 5.2.4 uses the same ICD headers for 2D work (a scan for the listed headers found them there; other headers
were not searched for in it). Address rows are in `A` order; payload text
is inferred unless marked.

### Geometry state and draw ports (class 00)

| Header | `A` | `n` | Name | Written by (ICD) | Payload |
|---|---|---|---|---|---|
| `0x00210020` | `0x001` | 1 | draw flags word | `__glCoClipTriangle`, `__glCoClipLine`, `__glCoDoPolygonClip`, `__glCoSetVertexAttribsMask`, `__glCoDlistExecutePackets` and others | Bits 31 and 30 say which coordinate set the vertex packets carry (30: the four words at vertex +0; 31: the four at +0x20; neither: the window x, y, z and the context's w). The routines that clip on the host clear bit 30 before sending clipped vertices. |
| `0x00411200` | `0x002` | 16 | model-view-projection matrix | `__glCoPassMVPMatrix` | 16 floats copied from the transform stack, column by column |
| `0x02421120` | `0x012` | 9 | normal matrix | `__glCoPassNormalMatrix` | 3 by 3 floats |
| `0x0362a0c0` | `0x01b` | 6 | viewport | `__glCoSetViewMatrix` | three scale floats then three offsets; the x and y offsets have a sub-pixel constant subtracted |
| `0x04230040` | `0x021` | 2 | x and y bias | `coLoadLinearTlb`, `coSetupTileToTile` | two floats; the pixel-transfer set-up adds a constant to both |
| `0x04640020` | `0x023` | 1 | line stipple repeat | `__glCoBeginLStrip`, `__glCoBeginLLoop`, `__glCoRenderAliasLine`, `__glCoRenderBitmap`, `__glCoWideStippledAliasLine`, `FillAASubTriangle` | the GL repeat factor, expanded |
| `0x04841020` | `0x024` | 1 | line stipple pattern | same as above | the 16-bit GL pattern in both halves |
| `0x04a42040` | `0x025` | 2 | polygon stipple | `__glCoConvertStipple`, `__glCoFillStippledTriangle` | an 8 by 8 bit pattern. `__glCoConvertStipple` reverses the bits of each byte of the 32 by 32 GL pattern and sends this only if the pattern is the same 8 by 8 block repeated; otherwise it falls back to host stippling (read in full) |
| `0x05260020` | `0x029` | 1 | lighting mode word | `LoadLightPath`, `__glCoClearBuffers`, `__glCoClipTriangle`, `__glCoRenderAAPoint` and others | flag bits kept in the context: bit 31 comes from a lighting-model flag byte, bit 26 is set when a context enable bit is on, bit 23 is cleared on the unlit path; the bits are not decoded |
| `0x05461060` | `0x02a` | 3 | scene ambient | `LoadLightPath` | the GL light-model ambient colour (RGB floats) |
| `0x05a64060` | `0x02d` | 3 | emission (front) | `ChangeMaterialEmission`, `__glCoValidateMaterial` | RGB floats |
| `0x06067060` | `0x030` | 3 | emission (back) | same | RGB floats |
| `0x0666a060`, `0x0666a0e0` | `0x033` | 3, 7 | ambient (front); ambient then diffuse | `ChangeMaterialAmbient`, `ChangeMaterialAmbientAndDiffuse`, `__glCoValidateMaterial` | ambient RGB; the 7-word form carries diffuse RGBA after it |
| `0x06c6d080`, `0x06c6d081` | `0x036` | 4 | diffuse (front) | `ChangeMaterialDiffuse`, `__glCoGenerateNoXformHandler` | RGBA; instance 1 appears for `GL_AMBIENT_AND_DIFFUSE` colour-material tracking in one generated handler. Meaning of the flag unknown. |
| `0x07471060`, `0x074710e0` | `0x03a` | 3, 7 | ambient (back); ambient then diffuse | as `0x0666a060` | as `0x0666a060` |
| `0x07a74080`, `0x07a74081` | `0x03d` | 4 | diffuse (back) | as `0x06c6d080` | as `0x06c6d080` |
| `0x08278080` | `0x041` | 4 | specular and shininess (front) | `ChangeMaterialSpecular`, `__glCoValidateMaterial` | specular RGB then the shininess |
| `0x08a7c080` | `0x045` | 4 | specular and shininess (back) | same | as above |
| `0x09280080` | `0x049` | 4 | current colour | `__glCoSetVertexAttribsMask`, `LoadLightPath`, `__glCoClearBuffers`, the span and bitmap routines | RGBA floats |
| `0x09a900c0` \| `i` | `0x04d` | 6 | light vectors, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | six floats: the light position or direction and a second vector. For a local light the per-vertex routine sends it again for each vertex |
| `0x0ca96120` \| `i` | `0x065` | 9 | light colours, light `i` | `LoadLightSource` | ambient, diffuse and specular RGB of GL light `i` |
| `0x1129f020` \| `i` | `0x089` | 1 | light scalar, light `i` | `LoadLightSource`, `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | 1.0 for a directional light, otherwise a spot or attenuation term; sent per vertex for local lights |
| `0x18002020` | `0x0c0` | 1 | primitive mode | every `__glCoBegin*`, `RestartDispatch*`, `__glCoFill*Triangle`, `__glCoRenderPoint` and the display driver | see [draw-and-buffers.md](draw-and-buffers.md#primitive-mode-word) |
| `0x18a33060` + mask | `0x0c5` | 3 (sends 2 coordinates) | vertex port, variant A | `__glCoSetVertexAttribsMask`, `GenerateSetAttribs`, the span-store routines, `FillAASubTriangle`, `CoCopyRect` in the miniport | see [draw-and-buffers.md](draw-and-buffers.md#vertex-packets). Seen with masks 0 and `0x08` |
| `0x18c33060` + mask | `0x0c6` | 3 | vertex port, variant B | same, plus `__glCoRenderBitmap`, `__glCoRenderAAPoint`, `__glCoDlistToPackets` | as above. Seen with masks 0, 2 and `0x10` |
| `0x18e84020` | `0x0c7` | 1 | colour index | `__glCoSetVertexAttribsMask`, `__glCoClearBuffers`, the span stores | the colour index (low 8 bits) in colour-index mode, 0 otherwise |
| `0x190b1020` | `0x0c8` | 1 | secondary colour | `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | three bytes packed in bits 31:8 |
| `0x192b2020` | `0x0c9` | 1 | fog value | same, `__glCoRenderBitmap` | integer (the float converted) |
| `0x198da080` | `0x0cc` | 4 | texture coordinate | same, `__glCoSpanRenderRGBA` | four floats; the third is 0.5 when the texture has fewer than two dimensions |
| `0x19a85060` | `0x0cd` | 3 | normal | `__glCoSetVertexAttribsMask`, `resendFirstVertThenResetState` | three floats |

### Engine register file (class 10)

| Header | `A` | Window offset | `n` | Name | Written by | Payload |
|---|---|---|---|---|---|---|
| `0x80003020` | `0x400` | `0x18000` | 1 | pixel-pipe mode word | `__glCoEnableDither`, `__glCoEnableLogicOp`, `__glCoEnableBlend`, the span stores, `__glCoClearBuffers`, `__glCoDlistExecutePackets` and the miniport | one flag word kept in the context. Bits seen set by name: 13 line stipple, 25 dither, bits 24 and 23 a two-bit mode (logic operation or blend, one at a time), 22 alpha test. The rest not decoded |
| `0x84e44040` | `0x427` | `0x18138` | 2 | scissor box | `__glCoSetScissor`, `__glCoDrawBufferRegion`, the transfer routines, the miniport | low 16 bits x, high 16 bits y; first word the minimum corner, second the inclusive maximum (decompiled C only, the window origin and a vertical flip are folded in). An empty box is minimum one past maximum. The miniport uses `0xf000f000` and `0x0fff0fff` for "everything" (signed 16-bit, plus or minus 4096) |
| `0x91aa0020` | `0x48d` | `0x18468` | 1 | draw-buffer descriptor | `__glCoSetDrawBuffer`, `__glCoLinearToTileXfer`, `__glCoTileToTileXfer`, `coTileToLinearSpans`, the miniport | checked in disassembly: bits 24:19 are the low six bits of the first dword of the buffer's record; bits 31:25 hold the colour format (below). Always follows a `0x98804000` |
| `0x91ea2040` | `0x48f` | `0x18478` | 2 | transfer parameters 2 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam` | word 1 is `(0x30 + (f & 0xf)) << 25`: bits 30:29 set and a four-bit context field `f` in 28:25; the second word is 0 |
| `0x922a4020` | `0x491` | `0x18488` | 1 | fog colour | `__glCoPassFog` | three bytes in 31:8 |
| `0x924a5040` | `0x492` | `0x18490` | 2 | alpha test | `__glCoPassAlphaFunc` | word 1: bit 31 set, function in 27:25; word 2: reference in 7:0 |
| `0x928a7040` | `0x494` | `0x184a0` | 2 | blend | `__glCoPassBlend` | word 1: destination factor 23:20, source 27:24, equation 31:28; word 2: constant colour, alpha in 7:0 |
| `0x92ca9020` | `0x496` | `0x184b0` | 1 | logic operation | `__glCoPassLogicOp` | operation in 31:28 (GL enum minus `0x1500`) |
| `0x92eaa020` | `0x497` | `0x184b8` | 1 | colour write mask | `__glCoPassColorMask`, the transfer and alpha-bit routines | 32-bit mask, red in the top byte |
| `0x930ab020` | `0x498` | `0x184c0` | 1 | depth and stencil buffer descriptor | `__glCoSetSzBuffer` | checked in disassembly: bits 28:23 are the low six bits of the first dword of the buffer's record; bits 31:29 select the depth format (below). Always follows a `0x98804000` |
| `0x932ac040` | `0x499` | `0x184c8` | 2 | polygon offset | `__glCoPassPolygonOffset` | checked in disassembly: word 1 is the first argument of `glPolygonOffset` (the factor) unchanged, word 2 is the second (the units) times a scale kept in the context. The earlier reading of this packet had the two swapped. |
| `0x936ae020` | `0x49b` | `0x184d8` | 1 | depth test | `__glCoPassDepthTest`, `__glCoClearBuffers`, `StoreALWAYS` | function in 31:29 (GL function minus `0x200`; the clear sends 7, "always"), mode in 28:27 |
| `0x938af040` | `0x49c` | `0x184e0` | 2 | stencil | `__glCoPassStencil`, `__glCoClearBuffers`, `__glCoSpanRenderStencil`, `StoreStencil` | word 1: three 4-bit operation codes, a 3-bit compare function and two bytes of state; word 2 one more byte |
| `0x93cc0080` | `0x49e` | `0x184f0` | 4 | transfer parameters 1 | `__glCoSetXferParam`, `__glCoRestoreXferParam`, `coSetTexParam` | word 1 is `(((((a & 0xf) + (b << 4)) << 2) + (c & 3)) << 6) + (d & 0x3f)`, then shifted left by 19, from four context fields: `d` in 24:19, `c` in 26:25, `a` in 30:27 and the low bit of `b` in 31; words 2 to 4 are 0 |
| `0x944c42c0` | `0x4a2` | `0x18510` | 22 | colour matrix | `coSetColorMatrixRegisters` | 20 floats from the context (a 4 by 4 matrix and four more), two more dwords |
| `0x97000020` | `0x4b8` | `0x185c0` | 1 | chain, short form | miniport only | next position |
| `0x97000040` | `0x4b8` | `0x185c0` | 2 | chain | `ClosePacket`, `DispatchCurrentPkt`, `__glCoDlistExecutePackets`, `coCreateTexBindPkt`, `coDispatchTexBindPkt`, miniport | next node position, this node position ([graphics-engine.md](graphics-engine.md)) |
| `0x97000060` | `0x4b8` | `0x185c0` | 3 | chain, long form | miniport only (`pCoContextSwitch`, `CoBuildUserTexTlbPacket`) | not read |
| `0x97400020` | `0x4ba` | `0x185d0` | 1 | packet pointer reset | `__glCoCreateTexLevel` | the payload is always 0. The ICD sends it, then runs its overflow routine, before it frees storage of the texture that is currently bound. The miniport writes the same register with the current packet pointer; the earlier "one texture-level word" was wrong. |
| `0x98250160` | `0x4c1` | `0x18608` | 11 | window parameters | miniport (`CoBuildWinparamsPacket`) | an id word and five pairs from the window record |
| `0x9845b020` | `0x4c2` | `0x18610` | 1 | window parameter | miniport | read from the window record |
| `0x9865c020` | `0x4c3` | `0x18618` | 1 | window parameter | miniport | read from the window record |
| `0x98804000` | `0x4c4` | `0x18620` | 0 | flush marker | `DispatchCurrentPkt`, every descriptor and mode write, the span stores | no payload; a write to a register that takes none. Meaning (pipeline flush before a descriptor changes) inferred from where it is placed |
| `0x99ce00a0` | `0x4ce` | `0x18670` | 5 | clear rectangle | `coClearBuffers`, miniport `CoClearRect` | see [draw-and-buffers.md](draw-and-buffers.md#clears) |
| `0x9e7200a0` | `0x4f3` | `0x18798` | 5 | clear rectangle, second bank | `coClearBuffers` (as `0x99ce00a0` plus `0x04a40000`, when the context has a second buffer) | as above |
| `0x9ed380e0` | `0x4f6` | `0x187b0` | 7 | image transfer | `coVhdlTexImage` | three caller words, a pixel count (width times height), a width, then the minimum and maximum corner packed y:x as 16 bits each |

## Coverage

Constant header values found in `sgcoicd.dll`: 52 (32 in class `00` and 20 in the register file; a count variant such
as `0x0666a060` and `0x0666a0e0` is a separate value, an instance or attribute-mask variant counts with its base).
Each has an emitting routine established and a payload layout read; the engine's use of each is unknown
except where the white paper names the function. Computed forms: the vertex port plus attribute mask, the light
blocks plus light index, the second clear bank (`0x9e7200a0`), and the class `11` table loads (miniport only). Not emitted by the
ICD but by the miniport: `0x97000020`, `0x97000060`, `0x98250160`, `0x9845b020`, `0x9865c020`, class `11`. The
earlier list of 20 headers is superseded.

**Withdrawn.** The earlier page listed `0x9400014f` as a span header written by `__glStoreSpanARGB8`. The bytes occur
once in the image, at a place past the last function (in the import tables), and no routine stores that value, so it
is not a header the driver emits. Also `0x04a40000` is not a header: the clear routine adds it to `0x99ce00a0` to pick
the second bank.

## Another emulator's model

A public emulator that models this machine has an engine window and an interrupt register
([source](../../references/documents/INDEX.md#public-sources)); nothing else of the packet set.

- It maps `0xc8000000` for the engine, 32 MB (its own comment says the extent is unverified), returns 0 for every
  read and only logs writes: no packet, register or queue is decoded. So it neither agrees nor disagrees with any
  row above. Software expectation (another emulator's model); it carries no information about the packets.
- It maps `0xca000000` for a 4-byte register spread over 32 MB, keeps the low four bits written and returns them. NT's
  retrace handler reads that register and writes back the complement; a model that latches the low four bits of the
  write does not clear anything that way, and the model never raises the interrupt. The address agrees with the
  miniport; the behaviour is the open question in [questions](../../questions/open-questions.md).
- Its Cobalt revision register reads `0xa4` (or `0xa5`). The registry export of a real machine shows `Cobalt: a4`
  ([board-data.md](board-data.md)), so the first value is the one the driver would accept on that machine.
