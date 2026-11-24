# The NT 4.0 display driver's video and 2D routines

What the display driver `sgcodd.dll` 5.2.4 (NT 4.0 pack, 299,696 bytes, the files list has its hash) does in its DirectDraw
video routines and its 2D paths, read for the mode word, the constant colour and the packets it builds. Routine names are
the vendor's, from the pack's `.dbg`; addresses are virtual addresses in the image (base `0x10000`). Ratings: what a routine
does is **established** (the routine read in disassembly); what the engine does with a word is **inferred**.

## DdBlt and the YUV blit

`DdBlt@0x1f6ce` is the DirectDraw blit entry. It first calls `ddrvalUpdateFlipStatus@0x1e514`, which compares the flip
target the driver last asked for (the device's `flipRecord.ulNextHwCtrl`, the 4-byte answer of the flip IOCTL) with the live
copy of the display back end's frame control word (`frm_inhwctrl`, `+0x30008`) and answers `DDERR_WASSTILLDRAWING`
(`0x8876021c`) until they agree. It then branches on the blit flags and the surfaces:

- the colour-fill flag (`0x400`) goes to `CoRectFill@0x1f25a`;
- a source surface whose pixel format carries the FOURCC flag (`ddpfSurface.dwFlags` bit 2, `DDPF_FOURCC`) goes to
  `CoYUVBlt_S2S@0x1e54a` (called at `0x1f7f3`) when the destination's `ddsCaps.dwCaps` has `0x4000` (video memory) set, and
  otherwise returns `DDERR_INVALIDPARAMS` (`0x80070057`);
- other surfaces go to the 2D copy and stretch routines (`vCopyBlt@0x12e30`, `EngStretchBlt` fall-backs).

None of the other video routines (`DdFlip`, `DdLock`, `DdMapMemory`, `DdCanCreateSurface`, `DdCreateSurface`,
`DdDestroySurface`, `DdGetBltStatus`, `DdGetFlipStatus`, `DdWaitForVerticalBlank`, `DdGetScanLine`) stores a packet header:
`DdFlip@0x1fc54` is the video IOCTL `0x232080` (12 bytes in, 4 out) and records the flip target for the check above;
`DdGetScanLine@0x1fe8c` and `VBLANK_IS_ACTIVE@0x1fe54`, which `DdWaitForVerticalBlank@0x1fde6` polls, read the **display
back end's** live scanline (`vt_xy` bits 23:12) and compare it with a 48-line window that starts at the screen height
(`cyScreen`, which equals the mode's vertical blank start in every row of the mode table) less `ulVBlankFudge`, a field of the
device record the two display-driver instances share (`+0xd4` there, not in the per-surface device). The `0x10000` they read is
that block's, not the engine's. Established.

`CoYUVBlt_S2S@0x1e54a` converts a packed YUV surface to RGB on the engine. `DdBlt` passes it the destination rectangle,
the source rectangle and the source surface. It first calls the device's two floating-point state hooks (`pEngineFPSave`
with a 0x24-byte buffer, and `pEngineFPRestore` on the way out); if either hook is missing or the save returns zero it
returns failure (`0x80004005`) without a packet. It takes only the FOURCC codes `YUY2` (`0x32595559`) and `UYVY`
(`0x59565955`); any other code ends the routine with `0x887601fe`. There are two paths. The scratch-rectangle path below is
taken only when the source rectangle is the whole source surface (its width and height equal the surface's `wWidth` and
`wHeight`), the destination is larger than the source in at least one axis, and the off-screen heap gives it a scratch
rectangle the size of the source (`pohAllocate@0x24c24`); every other case takes the direct path, described after the
numbered steps. An earlier version of this page said the routine returned failure in those cases and built no packet;
that was a misreading, the routine builds packets on both paths. The scratch-rectangle path builds packets with
`pulGetPacket@0x27748` and `vDispatchPacket@0x2786c`:

1. A flush marker, the draw-buffer descriptor `0x91aa0020` (only if the device's `ulGdiColorFormat`, `+0x6bc`, differs from the copy cached
   in the shared device record, `ulColorFormat` at `+0x90`), the mode word `0x80003020` with payload **`0x1c0000`** (bits 20, 19 and 18: colour matrix, pixel transfer,
   texture), the four-word texture packet `0x93cc0080` with word 1 `0x72100000` for `YUY2` or `0x7a100000` for `UYVY` and
   three zero words, the texture-environment packet `0x91ea2040` (`0x62000000`, 0), the texture window offsets `0x04230040`
   (`0.5`, `0.5`) and the colour matrix `0x944c42c0`: twenty floats copied from the table `CoMatrixYUV2RGB@0x33d24`, then 0 and
   `0xffffffff`, which fill its 22 words; the packet is padded to a 32-byte boundary.
2. The window parameters (`0x98250160` with a valid mask of 0, `0x9845b020` with payload 0), the primitive mode word
   `0x18002020` with payload `0x80000000` (type 4, a strip) and four vertices on port A with the texture-coordinate mask
   (`0x18a33068`, four dwords each) for the scratch rectangle, whose coordinates come from an integer-to-float table at
   `0x3423c`. The packet ends with a flush marker, the descriptor again if it changed, and the mode word **`0xc0000`**
   (texture and pixel transfer, colour matrix off), then a texture packet whose word 1 is the draw buffer's own format word
   (`ulGdiColorFormat`) with `0x30000000` in the next word, and the environment packet.
3. The third packet repeats the window parameters, the texture window offsets (0, 0) and the strip mode word, then sends the
   scratch rectangle's pixels to the destination as port A vertices whose coordinates are floating point, in bands that
   end on the next 32-line boundary of the screen (128-line from row 256 down) and are cut into columns `ulTilePix` pixels
   wide, starting a new packet after 43 band edges. The last packet ends with a flush marker, is dispatched with the
   last-packet argument and the scratch rectangle is freed (`pohFree`). Read in the typed decompile; the band arithmetic
   was not checked against the hardware's tile size, which is unknown.

So the conversion is a textured rectangle with the colour matrix on, drawn into the scratch rectangle; the second mode
word and texture packet read the screen's own format as a texture, which fits a final stretch from the scratch rectangle
to the destination.

**The direct path** builds the same first packet (mode word `0x1c0000`, the texture format `0x72100000` or `0x7a100000`, the
environment, the colour matrix) and then a window-parameter packet with texture window offsets of `0x3a83126f` for both
(a float of about 0.001). If the destination is at least 1024 by 768 it draws one horizontal strip per source row, each
inside a scissor-style clip rectangle built from the destination rectangle through the device's `SmaskInfo` record (valid
mask `0x08000000`, two corner words), with a primitive mode word `(ratio | 0x4000) << 15` where `ratio` is the destination
height over the source height rounded up, and a new packet after about 97 strips; a smaller destination is a single
primitive mode word `0xa0000000` and four port A vertices. Each ends with a flush marker. Established as what the routine
writes (typed decompile and the disassembly at the two entry branches); the engine's reading of the clip rectangle and the
ratio field is inferred.

Read as the vendor names
the fields ([packet-set.md](../../devices/cobalt-graphics/packet-set.md#the-vendors-register-word-types)), `0x72100000` and
`0x7a100000` are a texture format of type 14 and 15, depth 1 (16 bits) and descriptor id 2. Descriptor 2 is the entry
`CobaltStartIO` fills with the global buffer, where video-memory surfaces are mapped. Established as what is written; that
types 14 and 15 are the engine's codes for `YUY2` and `UYVY` is inferred, from the two FOURCC codes that select them; so is
the final stretch.

The 2D, blit and host-transfer routines are read in [2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md): packet
sequences per operation, the logic-operation table, the colour key, the descriptor ids and the DirectDraw capabilities.

## The 2D mode words and the constant colour

The display driver's mode-word payloads that are immediates are `0x1000000` (a logic-operation word, the base of
`vFillStip@0x123ae`), `0xc000` and `0xa000` (added for the stipple fills and the 1-bit transfers), `0x2000` (line stipple,
`vXfer1bpp@0x153f2`), `0x40000` (texture), `0xc0000` (texture and pixel transfer: `vCopyBackToFront@0x12ca5`,
`vTexRealize@0x171ef`), `0x4c0000` (the glyph blit) and `0x1c0000` (the YUV blit); the other stores load the word from a
register built from device state. In the thirteen mode-word
stores that write the header as an immediate and the immediates within sixty instructions of them (the value `0x20008000`
that recurs in the transfer routines is a primitive mode word, a line segment (type 1, width 1; `0x8000` alone is the point word), not a draw mode word):

- bit 28 (the vendor's depth offset) never occurs: 2D has no polygon offset;
- bit 19 occurs only with bit 18, in the copy, bitmap, glyph and YUV paths;
- bits 12:0 never occur, so nothing in 2D writes a reserved bit.

Words a routine builds in a register were followed only in `vFillStip` (the logic-operation base plus `0xc000`), so these
three statements cover the immediates and that routine, not every computed word. Inferred.

`0x91ca1020` is sent by five routines: `vFillStip@0x1229c`, `vXfer1bpp@0x15443`, `bFillSetup`, `DrvFillPath` and
`bPutGlyphInCache`. At the stipple and transfer sites the second colour goes to `0x18e84020` right after the set-up, and the mode word has bit 15 and bit 14 or 13. The colour is the brush's or translation's first colour, expanded
for pixel-format code 4 (RGBA5551) to red in 31:27, green in 23:19 and blue in 15:11. With bit 15 named as the opaque-stipple
enable by the vendor's mode type, the register is read as the colour of the pixels the pattern or the 1-bit source leaves
clear. The glyph-cache site (`bPutGlyphInCache@0x2da2a`) sends `0x18e84020` with 0 first and `0x91ca1020` with `0xff` after it, and fits it once the later steps are read: the cache holds colour 0 for set pixels and `0xff` for clear ones, and `DrvTextOut` alpha-tests the cache against `0xff` ([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md#text)). Inferred (the sites read; the pairing of the two colours with the pattern's set and clear bits is not).

## The 8 bits per pixel palette, colour translation and the CPU-drawn paths

Read from the April 1999 NT 4.0 build of `sgcodd.dll` (disc 1, 334,704 bytes, hash prefix in the files list), the build installed
from the OEM media, in the typed decompile with the key routines checked in the disassembly. The 5.2.4 build was compared only at
two points (the colour map loop starting at index `0x1101` and the default graphics-capability word below), which match. Names
are the vendor's, from the `.dbg`; addresses are image virtual addresses. The miniport `sgcomp.sys` takes no part: the display
driver maps the display back end itself (the base at `*(*pdev+0xc8)`) and never issues the miniport's colour-register
IOCTL (`0x23041c`; no push of it in the image).

**What the driver tells GDI (`DrvEnablePDEV@0x24120`, `bInitializeModeFields@0x24c70`).** Established.

- The default graphics-capability word copied into the device-info record is `0x28552c`: alternate and winding fill, the colour
  dither bit (`0x20`), opaque rectangles, mono dither, asynchronous pointer move, direct draw, high-resolution text, and
  `GCAPS_PALMANAGED` (`0x80000`); bit `0x200000` is not one this page names. For 16 and 32 bits per pixel the routine clears
  `0x80000` and `0x20`, so **at 8 bits per pixel the device reports a palette-managed colour-dither device**.
- The default graphics-info record gives 20 static colours (`ulNumColors`) and 256 palette registers (`ulNumPalReg`); the
  mode's bitmap format is `BMF_8BPP` and the device-info dither format is `BMF_8BPP` with an 8 by 8 cell (the record holds
  `0x80008`).
- `bInitializePalette@0x2d290` builds one 256-entry table in memory and creates the default palette with `EngCreatePalette` in
  indexed mode (mode 1, 256 entries); at other depths it creates a bit-field palette from the mode's masks. The table is a 3-3-2
  ramp (red in the top three index bits, green next, blue in the low two; each component widened by bit replication), with the
  twenty static colours of the usual Windows layout copied over entries 0 to 9 and 246 to 255 from a table in the image. The
  palette handle goes into both the device record and the device-info `hpalDefault`.

**What the driver does with the palette hardware.** Established for the two routines read in full.

- `bEnablePalette@0x2d3d0` (called from `DrvEnableSurface`) writes the colour map at `+0x50000` of the display back end twice.
  First entries `0x1101` to `0x11ff` get the same 3-3-2 ramp, with index `0x1120` forced to zero; then, at 8 bits per pixel only,
  bank 0 (entries 0 to 255, written from 255 down to 0) gets the table above. Entry format as the register page: red in
  31:24, green 23:16, blue 15:8. Before every 32nd write, and once before each run, it spins until `+0x58000` has its low six
  bits zero or reads `0x30` or more. That status word is the only thing it reads from the device; nothing reads a colour map entry.
- `DrvSetPalette@0x2d560` takes `PALOBJ_cGetColors` (the start index and count GDI passes), refuses if GDI returns fewer entries
  than asked, waits on the same status word, then copies the entries into the driver's own table and into bank 0, from the
  last entry down, each time at index `i` of the run with the **start index ignored** (the run always lands at entry 0). Read in the
  disassembly. Whether GDI ever passes a non-zero start index for this driver is unknown.
- The window mode table entry at `+0x48000` is written with `3` for 8 bits per pixel (`0x1b` for 16, `0x17` for 32), in the
  routine that follows the pixel-size test at `0x2481c`. Entry bits 9:5 (the colour map bank field of the register page) are
  clear in all three, so bank 0 is the bank an 8-bit pixel goes through; what bank `0x11` is for is unknown (inferred: a
  second table for an OpenGL colour-index or RGB-332 window mode, because no 2D path selects it).

So **the 8 bpp palette, GDI's translation tables and the dither tables are all computed without reading any device state** other
than the status word, whose wanted value (low six bits zero, or `0x30` and above) any model value of `0x3f` satisfies.

**Dither.** `DrvDitherColor@0x1a160` and `vDitherColor@0x19f30` build the 8 by 8 pattern from a table that turns a 16-colour
code into a palette index: codes 0 to 6 map to indices 0 to 6, code 7 to `0xf8`, code 8 to 7, codes 9 to 15 to `0xf9` to `0xff`
(table at `0x39348`, replicated bytes at `0x39208`). It assumes the standard static layout in the table above and reads nothing from
the device. The only indices a non-solid fill can produce are those sixteen. Established.

**Which surface GDI draws into.** Established.

- The screen is a device-managed surface (`EngCreateDeviceSurface`, then `EngAssociateSurface` with hook mask `0x157b`: bit blt,
  stretch blt, text out, paint, stroke path, fill path, line to, copy bits, synchronize). `DrvCreateDeviceBitmap@0x29a90` returns
  zero (GDI makes its own bitmap) unless the pixel size matches, the pixel is larger than 8 by 8, the off-screen heap has room
  and one more device-record field (`+0x88`) is not 1; a bitmap it accepts lives in the off-screen heap and carries a record with type 0.
- `bEnableOffscreenHeap@0x29c60` also creates two **engine-managed 8 bpp bitmaps** (pitch `0x2000`, a placeholder bits pointer
  `0xbadf00d`), locks them and keeps them in the device record (`+0x168`, `+0x16c`). Before a punt the driver sets the first one's
  `pvScan0` to the CPU aperture of the screen or of the bitmap's off-screen rectangle (the id 2 window, pitch `0x2000`) and
  calls `EngBitBlt` or `EngCopyBits` on it. **That is the source of byte and dword stores through window 2 with no reads**: GDI's
  own routines write the translated bytes straight into the aperture.
- `DrvCopyBits@0x10f40` (source a plain DIB, target the screen, **non-trivial translation**) takes that path when the device
  record flag at `+0x628` is set (the mode routine sets it when two mode flags, `+0x614` and `+0x618`, are both clear; what they
  mean is not read), unless a 16 or 32 bpp XRGB fast path applies.
  A source of the same pixel format with a trivial translation goes to `vXferNative8bpp@0x14e10` instead, which stores dwords
  through the window when the logic operation is copy (`0xcccc`) and otherwise sends the engine's pixel DMA packets. 4 bpp and
  1 bpp sources never match that test, except 1 bpp, which goes to `vXfer1bpp`.
- `DrvBitBlt@0x10860` only accepts a DIB source for the direct transfer when the translation is trivial and the formats are
  equal; anything else, and every operation with a mask or a pattern ROP, goes to `bPuntBlt@0x10370`. For a screen target and
  an operation other than copy, `bPuntBlt` allocates a host bitmap (`EngAllocMem`, then `EngCreateBitmap` and
  `EngAssociateSurface` with no hooks), **reads the screen rectangle into it with `vXferDMA_SB@0x15d10`**, lets `EngBitBlt` compose
  in host memory (the translation is GDI's, passed through unchanged as the `XLATEOBJ`), and writes the result back with
  `vXferNative8bpp` (CPU window stores).
- `vXferDMA_SB` (typed decompile only) builds engine pixel-transfer packets (a destination buffer descriptor, then vertex pairs under the header `0x18a33068`, a
  fence), calls `DrvSynchronize`, and then, only for the first and last partial 32-byte lines of the host buffer, **copies those
  bytes with CPU reads through the same window (pitch `0x2000`)**. Its main body is the engine writing screen pixels to system memory.

### Flat host buffers: row pitch and the staging area

A flat buffer is a run of pinned host pages that the engine reads or writes as a surface. The display driver's `CoGrmMapLinearBuffer@0x19c62`
sends video request `0x232050` (flags `0x10`, cache flags 3, address, length); `CobaltStartIO@0x12634` locks the pages, makes a buffer
record with one translation-table entry per 4 KB page (page size code 0, row length 1, flags `0x14`) and `pCoMakeTlbDesc@0x170a2` turns it into
a descriptor. Established (the routines read in full).

**Descriptor `0x0b280000`, field by field.** Bits 31:21 are `0x59`: the buffer's first translation-table slot (89), which the miniport
adds when it writes the descriptor packet (`CoBuildTlbPacket`, `slot << 21`). Bit 19 is set. Every other bit is zero: page-size code 0
(bits 12:10, one 4 KB page), tile-width bit 13 clear, page-type bit 9 clear, and bits 20:14 hold only the bit-19 contribution. For a
buffer of this kind `pCoMakeTlbDesc` returns `pageSizeBits | 0x80000` and stops before it looks at the tile width or the row length.
Bit 19 is also bit 5 of the row-length field (read as a length, 32 tiles of 128 bytes, a 4096-byte pitch), but the driver sets no geometry for these buffers: the bit is the flat-buffer marker.

**Row pitch: 2048 bytes. Established for how every driver addresses it, inferred for the engine.** All users cut a byte offset into the
buffer as `y = offset >> 11` and `x = offset & 0x7ff` (in pixels: the display driver's `vXferDMA_SB@0x148d0`, `vXferDMA_BS@0x13722` and
`vXferDMA_SB_Buf@0x14442` shift the masked offset right by the pixel-size shift; the OpenGL driver's `coLinearToTileSpans@0x6903a7e6` and
`coTileToLinearSpans@0x6903c176` use `x = offset & (0x7ff >> shift)` and `y = offset >> (11 - shift)`, the same split). The pixel `(x, y)`
then sits at byte `y * 2048 + x * bytesPerPixel`, which is the offset itself: with 2048-byte rows a flat buffer is the host bytes in order,
so one host row of any width can be walked by stepping the offset. A pitch of 4096 would put `(x, y)` at `y * 4096 + ...`, and no driver
read the data back that way. The descriptor holds no row length for a flat buffer, so the pitch does not depend on it; a 4 KB page holds
two rows. Emulator choice, only because the engine's own rule is unknown: flat address `y * 2048 + x * bytesPerPixel` over the page list in
table order, with x and y as sent.

**Host pages against the staging area. Established for the choice, which lives in software, not in a field.** Nothing in the descriptor,
the colour-format word or the packets differs between the two: both are the same flat descriptor bound to descriptor id `ulLnDescId`.
`vXferDMA_SB` takes the buffer GDI gave it directly only when the start address, the row byte count and the absolute row delta are all
multiples of 8; otherwise it calls `vXferDMA_SB_Buf@0x14442`, which maps a static array inside the display driver's own image
(`_jReadBufGlobal`, offset `0x2c0`, at most `0x20000` bytes per round, rows padded to 32 bytes) with the same request. After the fence
(`DrvSynchronize`) it copies each row from that array to the destination bitmap with the CPU. GDI's screen-to-bitmap readback bitmap is a
DIB whose row delta is padded to 4 bytes, which is why GDI's readbacks mostly take the staging route (a delta divisible by 8 takes the direct one). The direct route copies nothing afterwards except, when the driver's cache-operation workaround is on
(`bCacheOpWar`), the first and last partial 32-byte lines, read through the CPU window. So the engine writes into pages the miniport
holds locked, the driver copies out after the fence, and a model sees no difference in what it is asked to do.

**Colour translation of a different-format source (4, 8 or 24 bpp).** The driver never builds its own `XLATEOBJ`. It receives GDI's
and either calls it (`XLATEOBJ_iXlate`, only in the 16 and 32 bpp XRGB test) or hands it through to `EngBitBlt` / `EngCopyBits`. The
translation to the 8 bpp indexed palette is computed by GDI from the palette created above and, for a palette-managed device,
GDI's idea of the realised system palette. **Inferred:** nothing in the driver can make that translation return only black.
The driver's palette entries (statics at 0 to 9 and 246 to 255) match its dither tables.

### Answers to the three questions that prompted this reading

1. **Palette and translation (established).** As above: `GCAPS_PALMANAGED` set at 8 bpp, indexed 256-entry palette from a table
   built in the driver, hardware loaded by direct stores, and the only device read is the colour map FIFO status. No read-back of
   the colour map, no capability word, no colour-format query reaches the palette code. `DrvEnablePDEV` also issues two private IOCTLs (`0x232060`, `0x232064`) whose answers fill mode and aperture fields; that is the
   only other device input to the set-up (inferred from the stores; the output layout was not decoded).
2. **CPU-drawn blits (established for the paths; inferred for the cause).** Non-trivial-translation DIB copies and masked or
   non-copy ROPs are computed by GDI into the aperture (`pvScan0` swap) or into a host bitmap seeded by an engine DMA read-back
   of the screen. Because the symptom shows exactly the silhouette of the mask with every opaque pixel zero, while background
   pixels keep their value, either the colour source arrives as zero index or the xor/overwrite step that adds colour never
   lands; the driver reads no device state that could cause the first.
3. **The IE background (inferred).** A solid colour that is not a static is a dithered brush, built from the 16 fixed indices
   above without any device read, or a DIB fill by the same two paths. It fits the same family only if the cause lies in
   GDI's translation or in the pixel DMA; it does not fit a palette-hardware cause, because no palette value reaches those bytes.

**Two later readings.** Text at 8 bits per pixel is the one engine path with a format of its own: the cached-glyph route draws into the
screen buffer under colour type 6 with the palette index shifted left eight bits, so white text needs the engine to write bits 15:8
([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md#text)). The masked-bitmap passes (check boxes, radio buttons) are two
`vXferDMA_BS` transfers, AND then XOR, whose image comes entirely from host memory
([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md#host-buffer-transfers-span-mode-and-the-and-and-xor-passes)); both are
engine paths that the CPU-aperture paths above do not reach. Established for the driver side.

What would settle it is the open question on the 8 bpp icons in [questions/open-questions.md](../../questions/open-questions.md);
the driver alone does not decide it (**unknown**).
