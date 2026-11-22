# The NT 4.0 display driver's video and 2D routines

What the display driver `sgcodd.dll` 5.2.4 (NT 4.0 pack, 299,696 bytes, the files list has its hash) does in its DirectDraw
video routines and its 2D paths, read for the mode word, the constant colour and the packets it builds. Routine names are
the vendor's, from the pack's `.dbg`; addresses are virtual addresses in the image (base `0x10000`). Ratings: what a routine
does is **established** (the routine read in disassembly); what the engine does with a word is **inferred**.

## DdBlt and the YUV blit

`DdBlt@0x1f6ce` is the DirectDraw blit entry. It first calls `ddrvalUpdateFlipStatus@0x1e514`, which compares the flip
target the driver last asked for (`ppdev+0x60c`) with the live copy of the display back end's frame control word
(`+0x30008`) and answers `DDERR_WASSTILLDRAWING` (`0x8876021c`) until they agree. It then branches on the blit flags and
the surfaces:

- the colour-fill flag (`0x400`) goes to `CoRectFill@0x1f25a`;
- a source surface whose pixel format carries the FOURCC flag (surface `+0x28` bit 2) goes to `CoYUVBlt_S2S@0x1e54a` (called at `0x1f7f3`) when the
  destination's flag word has bit 14 set, and otherwise returns `DDERR_INVALIDPARAMS` (`0x80070057`);
- other surfaces go to the 2D copy and stretch routines (`vCopyBlt@0x12e30`, `EngStretchBlt` fall-backs).

None of the other video routines (`DdFlip`, `DdLock`, `DdMapMemory`, `DdCanCreateSurface`, `DdCreateSurface`,
`DdDestroySurface`, `DdGetBltStatus`, `DdGetFlipStatus`, `DdWaitForVerticalBlank`, `DdGetScanLine`) stores a packet header:
`DdFlip@0x1fc54` is the video IOCTL `0x232080` (12 bytes in, 4 out) and records the flip target for the check above;
`DdGetScanLine@0x1fe8c` and `VBLANK_IS_ACTIVE@0x1fe54`, which `DdWaitForVerticalBlank@0x1fde6` polls, read the **display
back end's** live scanline (`vt_xy` bits 23:12) and compare it with a 48-line window that starts at the mode's vertical blank
start less a device offset (`ppdev+0xd4`). The `0x10000` they read is that block's, not the engine's. Established.

`CoYUVBlt_S2S@0x1e54a` converts a packed YUV surface to RGB on the engine. It takes only the FOURCC codes `YUY2`
(`0x32595559`) and `UYVY` (`0x59565955`); any other code ends the routine with a failure result. `DdBlt` passes it the
destination rectangle, the source rectangle and the source surface. The routine does the work only when the source
rectangle is the whole source surface (its width and height equal the surface's `+0x1c` and `+0x18`) and the destination
is larger than the source in at least one axis; otherwise it returns failure (`0x80004005`) without a packet. It then
allocates a scratch rectangle the size of the source in the off-screen heap (`pohAllocate@0x24c24`) and builds packets with
`pulGetPacket@0x27748` and `vDispatchPacket@0x2786c`:

1. A flush marker, the draw-buffer descriptor `0x91aa0020` (only if `ppdev+0x6bc` differs from the copy cached at
   `dev+0x90`), the mode word `0x80003020` with payload **`0x1c0000`** (bits 20, 19 and 18: colour matrix, pixel transfer,
   texture), the four-word texture packet `0x93cc0080` with word 1 `0x72100000` for `YUY2` or `0x7a100000` for `UYVY` and
   three zero words, the texture-environment packet `0x91ea2040` (`0x62000000`, 0), the texture window offsets `0x04230040`
   (`0.5`, `0.5`) and the colour matrix `0x944c42c0`: twenty floats copied from the table `CoMatrixYUV2RGB@0x33d24`, then 0 and
   `0xffffffff`, which fill its 22 words; the packet is padded to a 32-byte boundary.
2. The window parameters (`0x98250160` with a valid mask of 0, `0x9845b020` with payload 0), the primitive mode word
   `0x18002020` with payload `0x80000000` (type 4, a strip) and four vertices on port A with the texture-coordinate mask
   (`0x18a33068`, four dwords each) for the scratch rectangle, whose coordinates come from an integer-to-float table at
   `0x3423c`. The packet ends with a flush marker, the descriptor again if it changed, and the mode word **`0xc0000`**
   (texture and pixel transfer, colour matrix off), then a texture packet whose word 1 is the draw buffer's own format word
   (`ppdev+0x6bc`) with `0x30000000` in the next word, and the environment packet.
3. Later packets (the routine dispatches more than three) repeat the pattern with the texture window offsets 0, the strip mode
   word, port A vertices whose coordinates are built in floating point from the rectangles' corners, and, after another
   window-parameter packet, a second strip: the packets this routine writes between the colour-matrix pass and the final
   stretch were not followed to the end.

So the conversion is a textured rectangle with the colour matrix on, drawn into the scratch rectangle; the second mode
word and texture packet read the screen's own format as a texture, which fits a final stretch from the scratch rectangle
to the destination. Read as the vendor names
the fields ([packet-set.md](../../devices/cobalt-graphics/packet-set.md#the-vendors-register-word-types)), `0x72100000` and
`0x7a100000` are a texture format of type 14 and 15, depth 1 (16 bits) and descriptor id 2. Descriptor 2 is the entry
`CobaltStartIO` fills with the global buffer, where video-memory surfaces are mapped. Established as what is written; that
types 14 and 15 are the engine's codes for `YUY2` and `UYVY` is inferred, from the two FOURCC codes that select them; so is
the final stretch.

## The 2D mode words and the constant colour

The display driver's mode-word payloads that are immediates are `0x1000000` (a logic-operation word, the base of
`vFillStip@0x123ae`), `0xc000` and `0xa000` (added for the stipple fills and the 1-bit transfers), `0x2000` (line stipple,
`vXfer1bpp@0x153f2`), `0x40000` (texture), `0xc0000` (texture and pixel transfer: `vCopyBackToFront@0x12ca5`,
`vTexRealize@0x171ef`), `0x4c0000` (the glyph blit) and `0x1c0000` (the YUV blit); the other stores load the word from a
register built from device state. In the thirteen mode-word
stores that write the header as an immediate and the immediates within sixty instructions of them (the value `0x20008000`
that recurs in the transfer routines is a primitive mode word, a point, not a draw mode word):

- bit 28 (the vendor's depth offset) never occurs: 2D has no polygon offset;
- bit 19 occurs only with bit 18, in the copy, bitmap, glyph and YUV paths;
- bits 12:0 never occur, so nothing in 2D writes a reserved bit.

Words a routine builds in a register were followed only in `vFillStip` (the logic-operation base plus `0xc000`), so these
three statements cover the immediates and that routine, not every computed word. Inferred.

`0x91ca1020` is sent by five routines: `vFillStip@0x1229c`, `vXfer1bpp@0x15443`, `bFillSetup`, `DrvFillPath` and
`bPutGlyphInCache`. At the stipple and transfer sites the second colour goes to `0x18e84020` right after the set-up, and the mode word has bit 15 and bit 14 or 13. The colour is the brush's or translation's first colour, expanded
for pixel-format code 4 (RGBA5551) to red in 31:27, green in 23:19 and blue in 15:11. With bit 15 named as the opaque-stipple
enable by the vendor's mode type, the register is read as the colour of the pixels the pattern or the 1-bit source leaves
clear. The glyph-cache site (`bPutGlyphInCache@0x2da2a`) sends `0x18e84020` with 0 first and `0x91ca1020` with `0xff` after it, and does not obviously fit the cleared-pixel inference. Inferred (the sites read; the pairing of the two colours with the pattern's set and clear bits is not).
