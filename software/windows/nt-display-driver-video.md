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
