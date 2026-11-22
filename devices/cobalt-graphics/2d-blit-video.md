# Cobalt graphics: 2D, blit and video as the NT 4.0 display driver drives them

What an XP display driver or an emulated engine has to honour so that the 2D, copy and video paths of SGI's own drivers
produce the same pixels. Back to the [overview](README.md). The packets are listed in [packet-set.md](packet-set.md), the
queue and completion path in [graphics-engine.md](graphics-engine.md), the DirectDraw YUV blit in
[nt-display-driver-video.md](../../software/windows/nt-display-driver-video.md), the scan-out registers (including the
overlay plane) in [display-engine.md](display-engine.md).

Source: `sgcodd.dll` 5.2.4 (NT 4.0 pack, SHA-256 prefix `aa932e96561ec739`; addresses are image virtual addresses, base
`0x10000`), with its program database for names and types, and `sgcomp.sys` 5.2.4 for the drop routines. Every immediate
quoted below was checked in the disassembly of the routine named; the control flow was read in the typed decompile.
Grades are the repository's three. **established** means the routine was read in full and the claim is what it does;
**inferred** means the hardware's reading of the words (no document or public driver describes the engine) or a partial
read of the routine, said in the row; **unknown** means nothing shipped decides it.

## 1. There is no 2D engine

Every 2D operation of the NT display driver (fills, copies, mono expansion, patterns, lines, text, stretches, host
transfers, DirectDraw blits) is a packet sequence for the same rendering engine the OpenGL driver feeds, written into the
same packet queue ([graphics-engine.md](graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)).
No register of the display back end takes a blit, and no packet outside the 53 headers of [packet-set.md](packet-set.md)
appears in the display driver (`0x91ca1020` is its only header the OpenGL driver lacks). So the frame-buffer floor
([framebuffer-driver-floor.md](framebuffer-driver-floor.md)) is enough for a driver that never touches `0xc8000000`
(`sgibvid.dll`, `sglfb.sys`, a replacement XP miniport), but SGI's `sgcodd.dll` cannot draw one pixel without the engine
model. Established.

**Engine primitives the 2D code uses** (all from [packet-set.md](packet-set.md)): rectangles are four port-A vertices
(`0x18a33060`, or `0x18a33068` with a texture coordinate) under primitive mode `0xa0000000` (type 5, a fan) or
`0x80000000` (type 4, a strip); line segments are two port-A vertices under `0x20008000` (type 1, width field 1), polylines
`0x40008000` (type 2), single points `0x8000` (type 0, one vertex per pixel; the brush cache fill) and `vFillRectSpans`
uses `0xc0008000` (type 6, which nothing else sends; two vertices per span). A rectangle's x and y are integers converted to float; no 2D routine sends z. Fixed point
GDI path coordinates (28.4) go through a table that divides by 16 exactly (`vInitFloatTable@0x1afd2`: entry `i` is the
float `i` with `0x2000000` subtracted from its bit pattern, which lowers the exponent by 4). Established.

## 2. Surfaces, formats and the descriptor ids

`bInitializeModeFields@0x20ea2` sets the words every packet below uses. Read in full. Established.

| Item | 8 bpp | 16 bpp | 32 bpp |
|---|---|---|---|
| colour format word (base of `0x91aa0020` payload and of texture word 1) | `0` | `0x12000000` (type 2, depth 1) | `0x24000000` (type 4, depth 2) |
| pixel layout GDI sees | index | red 15:11, green 10:6, blue 5:1, bit 0 unused (`ulWhitePixel` `0xfffe`, `ulColorMask` `0xf8f8f800`, test pixel `0x886`) | red in the top byte, low byte unused (`ulWhitePixel` `0xffffff00`, test pixel `0x12345600`) |
| tile width in pixels / 512-byte tile | 512 | 256 | 128 |
| tile height | 128 lines | 128 lines | 128 lines |

So the 16-bit surface is RGBA5551 with red in the top bits and the spare bit at bit 0, and the 32-bit surface is red-first
(`0xRRGGBBxx`). That is the byte order the NT format table and the Windows 2000 drivers show, and the third independent
place that fixes it for the engine's own pixel format, though not for what the display back end scans out
([framebuffer-driver-floor.md](framebuffer-driver-floor.md)). Where the driver sends a colour to the engine as a 16-bit
value it first spreads it to the 32-bit form (`(((c & 0xf800) << 3 | c & 0x7c0) << 3 | c & 0x3e) << 10`, red 31:27, green
23:19, blue 15:11; `vFillSolid@0x11cc6`, `vFillStip`, `vXfer1bpp`, `vPatRealize`). 8-bit colours go to the colour port as
the index, replicated to all four bytes for the clear packet.

Descriptor ids (the six-bit field in 24:19 of every format word, the index of `0xc000 + 8 * id`): 2 front buffer and the
GDI draw target (`ulFbDescId`, `ulGdiDescId`, format `base | 0x100000`), 3 back buffer (`| 0x180000`), 5 overlay plane
(`0x280000`, 8 bit) and 6 its back buffer (`0x300000`), 7 and 8 right-eye front and back (stereo only), 10 the clip-ID
plane (`ulCidDescId`, `0x500000`), 11 the linear host-memory buffer used by pixel transfers (`ulLnDescId`, format
`ulLnFormat | 0x80580000`: bit 31 "linear" set), 12 the texture used for pattern fills (`ulTexDescId`, `base | 0x600000`).
Id 0 is the packet pool. Ids 1, 4 and 9 are not set by the mode set-up. Established (`bInitializeModeFields`).

Texture type codes seen in 2D: 0 (8-bit index), 2 (16-bit), 4 (32-bit), 8 (8-bit glyph mask,
`DrvTextOut@0x2dfbe`, word `0x40100000`), 14 and 15 (YUY2 and UYVY, [nt-display-driver-video.md](../../software/windows/nt-display-driver-video.md)).
Inferred (the codes are what is written; the meaning of 8, 14 and 15 is read from the FOURCC and glyph uses). The
OpenGL driver's own table of the other codes is not read here ([questions](../../questions/open-questions.md)).

The CPU can also reach the screen surface directly. When no stereo or overlay-GDI mode is on (`bLnAccess`) the driver's
host-to-screen routines (`vXferNative8bpp@0x13c94` and the 16 and 32 bit forms) write through the screen mapping at
`ulScreenBase` with a **fixed row pitch of `0x2000` bytes** (`(y + yOffset) * 0x2000 + x`), whatever the mode's width, after
a `DrvSynchronize`. So the miniport's mapping of video memory is not a tile-by-tile or `lDelta`-strided view, it
is a 8 KiB-row linear window. Established (routine read for 8 bpp, the others share the stride); how the miniport builds
that mapping is in the miniport's map-memory IOCTL (not re-read for this page).

## 3. Common set-up, completion and the fence

- **Device state cache.** The driver keeps the last draw-buffer descriptor sent (`pudev->ulColorFormat`) and resends
  `0x98804000 0x91aa0020 <format>` only when the format it needs differs. Every operation then writes the whole mode word
  `0x80003020` (the pixel-pipe word, [packet-set.md](packet-set.md#pixel-pipe-mode-word-0x80003020)) afresh, so the engine
  state between operations is not relied on, except the draw-buffer descriptor, the constant colour and the line-stipple pair
  (set per use). Established (`vFillSolid`, `vCopyBlt`, `vFillPat`, `vFillStip`, `vXfer1bpp`, `vStretchCopyBlt`).
- **Packets.** `pulGetPacket@0x27748` takes the next 4 KiB node of a ring (the data starts `0x2c` bytes in, after a pre-written chain
  packet `0x97000040` whose two words are the next node's and this node's positions in 32-byte units shifted left 12,
  OR-ed with serial bits), waiting until bit 0 of the node's first byte clears (calling the miniport's wait when the
  GDI queue flag says to). `vDispatchPacket@0x2786c` zero-pads to 32 bytes, stores the link word
  `(next position << 9 | units - 1) << 3 | flags` at dword 2 and again at the flush offset (flags `1` ready; `3` ready and
  interrupt; `7` ready, interrupt and sync) and the running serial at dword 3. Read in full; the engine's side of the flags is inferred
  ([graphics-engine.md](graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)). The
  routines pass `1` for the last packet of an operation: it requests the interrupt-and-sync form only when the GDI queue
  flags ask for per-packet interrupts or the flush count has run out (`ulPktFlushCount`, reloaded from `ulPktChunkCount`), so
  a model must not assume that every operation raises the engine interrupt.
- **Fence.** Each dispatch sets `bMustSynchronize`. `DrvSynchronize@0x2b2ea` (and `DdLock`, and every CPU access in `vXferNative*`)
  clears it as follows: send a flush marker dispatched with argument 2 (sync form, `7`), ask the miniport to wait for the serial
  of the last packet if that serial is not already the one the miniport has seen (event kind 3), then spin until bit 0 of the
  first byte of the current node clears. Read in full. So a CPU read or write of the frame buffer is legal when the node's
  bit 0 is clear and the miniport's completed serial equals the last serial; a model that completes each node as soon as it is
  queued satisfies it, and a model that never clears bit 0 hangs the driver. Established as the driver's rule.
- **Window parameters.** Every drawing operation puts a `0x98250160` packet (valid mask in dword 0, a clip rectangle pair
  per set bit, then zero fill to 12 dwords) and `0x9845b020` (the window origin, `y << 16 | x`) before its primitives.
  For a window with an offset (DirectDraw and GDI window surfaces) the origin is `xOffset & 0xffff | yOffset << 16`, otherwise 0
  (`DrvBitBlt@0x10726`, `vFillSolid`). The vertices then carry window-relative coordinates; the copy and glyph routines
  instead add `xOffset` and `yOffset` themselves and send origin 0. Established.
- **Clipping.** With a rectangular clip (`DC_RECT`) the driver loads the valid mask `0x08000000` and one rectangle,
  minimum `y << 16 | x`, maximum `(bottom - 1) << 16 | (right - 1)` (inclusive), built from the destination
  rectangle, and draws the clip's bounds. With a complex clip (`DC_COMPLEX`) it loads the same one-rectangle mask from the
  destination rectangle and draws **one quad per clip rectangle** that `CLIPOBJ_bEnum` returns (up to 0x2c rectangles per packet; a
  new packet and window parameters after that). The copy, stretch and pixel-transfer routines load a valid mask of 0 and
  rely entirely on drawing exactly the clipped rectangles. The hardware's clip rectangles are therefore never needed to cut a
  rectangle that is already clipped; a model that treats the valid mask as "no clip at all" still gives the right pixels
  for these routines. A window with more than five rectangles needs the clip-ID plane (`vGetCID@0x18190`, id 10,
  [packet-set.md](packet-set.md#window-parameter-packets)). Established for what is written; the engine's use inferred.

## 4. Operations

Packet order is as the routine writes it. "Mode" is the payload of `0x80003020`. `ROP2` means the 16-entry table of §5.

| Operation | Routine | Packets | Grade |
|---|---|---|---|
| Solid fill, rectangle, ROP2 `PATCOPY`, `BLACKNESS`, `WHITENESS`, no hardware clip needed | `vFillSolid@0x11cc6` calls `vClearSolid@0x1199a` | flush marker, then one `0x99ce00a0` per column group ([below](#rectangle-clear)) | established; engine inferred |
| Solid fill, any other ROP2 or a complex clip | `vFillSolid` | [draw buffer], flush marker + `0x92ca9020` (ROP2 unless copy), mode `0` or `0x1000000`, window params, `0x18002020` `0xa0000000`, `0x18e84020` colour, four port-A vertices | established |
| Fill spans | `vFillSpans@0x11384`, `vFillRectSpans@0x1153e` | window params, prim `0x20008000` (type 1, width 1) with two port-A vertices (left, right, same y) per span and the colour in `ulColor4I`; the rect form uses `0xc0008000` | established (both read in full); what type 6 does is unknown |
| Pattern fill (8 by 8 colour brush) | `vFillPat@0x12846`, `vTexRealize@0x1718c`, `vPatRealize@0x176a2` | brush drawn once into the brush cache as 64 points (colour to `0x18e84020`, vertex to `0x18a33060`, mode `0` and prim `0x8000`), then a copy of the pattern with the brush origin's `x & 7`, `y & 7` rotation into the texture area, then the fill as a textured quad per clip rect: mode `0x40000 (\| 0x1000000)`, `0x93cc0080` word 1 `ulTexFormat`, word 2 `0x330000` (map height 3 and width 3: 8 by 8 texels), `0x91ea2040` `0x62000000` 0, bias `0` `0`, prim `0x80000000`, port A with texture, texture coordinate `(2 * x + 1) / 16` | established |
| Stipple fill (mono brush, 8 by 8) | `vFillStip@0x12282` | `0x91ca1020` background colour, `0x04a42040` the 8 by 8 pattern as two words, mode `0xc000` (opaque stipple and polygon stipple) `\| 0x1000000` for a ROP2, `0x18e84020` foreground colour, quads | inferred (the first 190 lines of the routine and every immediate; the clip loop is the one of `vFillSolid`) |
| Screen to screen copy | `vCopyBlt@0x12e30` | see [below](#screen-to-screen-copy) | established |
| Host bitmap to screen, same format | `vXferNative8bpp`, `vXferNative16bpp`, `vXferNative32bpp@0x13c94..0x13ee2` | when the ROP is `SRCCOPY` (`0xcccc`) and `bLnAccess`: CPU copy through the `0x2000`-pitch window after `DrvSynchronize`; otherwise `vXferDMA_BS@0x13722`: textured quad from the linear buffer (descriptor 11, `ulLnFormat`) | established for the CPU copy; DMA inferred (partial) |
| Host bitmap to screen, 1 bit per pixel | `vXfer1bpp@0x15322`, `vXfer1bppInv@0x16274` | [colour expansion](#mono-colour-expansion) | inferred (partial read, immediates verified) |
| Screen to host memory | `vXferDMA_SB@0x148d0` | draw buffer is the linear descriptor, source texture is the screen (`ulGdiColorFormat`), rectangle prim; only for ROP `0xcccc` (`DrvBitBlt`) | inferred (partial) |
| Stretch | `vStretchCopyBlt@0x29ac2` | like a copy, but the vertices are the destination rectangle and the texture coordinates the source rectangle, bias `0 0`, prim `0x80000000`, texture mode word `0` (nearest filter, no wrap) | established for the sequence; filter reading inferred |
| Solid lines, polylines | `vHWSolidLines@0x25f3a`, `DrvLineTo@0x25188` | prim `0x40008000` (type 2 strip, width 1), colour to `0x18e84020`, up to 0x147 vertices per packet, vertices from the 28.4 path coordinates divided by 16 | inferred (partial) |
| Styled lines | `lHWStippleLines@0x26368`, `DrvStrokePath@0x26ee6` | mode bit 13 (`0x2000`), `0x04841020` pattern, `0x04640020` repeat word, then the same line prim | inferred (partial) |
| Text | `DrvTextOut@0x2dfbe`, `bPutGlyphInCache@0x2da2a`, `vTextOutFW_P*C` | [glyph cache](#text) | inferred (partial) |
| YUV to RGB blit | `CoYUVBlt_S2S@0x1e54a` | [nt-display-driver-video.md](../../software/windows/nt-display-driver-video.md#ddblt-and-the-yuv-blit) | established |
| Fill with colour (DirectDraw) | `CoRectFill@0x1f25a` | the rectangle clear below, with window parameters first | established |

Operations the driver does not accelerate fall back to GDI's own code through `bPuntBlt@0x103d0`, which draws through the
CPU window with `Eng*` calls: any ROP4 with a mask, any ROP3 that depends on both source and pattern, a copy of a
format the screen does not have, `DrvPaint` with a mask, `DrvStrokePath` with geometric or complex-style lines. The display
driver exports no alpha-blend, transparent-blit or gradient hook (the hook mask `flHooks` is `0x157b`), so those are software on
this driver (**established**, absence in the routine list of 344 functions). `DrvBitBlt` accepts hardware for: a destination
fill with any source-independent ROP3, a screen copy with any source-and-destination ROP3, a 1-bit source with ROP `0xe2`
(transparent mono: background not written) or `0xb8`, a same-format host source with any source-and-destination ROP3, a copy to
host memory with `0xcc`. Read in `DrvBitBlt@0x10726` in full.

### Rectangle clear

`vClearSolid@0x1199a`: a flush marker, then per clip rectangle (or the whole rectangle) up to three `0x99ce00a0` packets of
five words: the descriptor word (`ulGdiDescId`, the raw id with no shift: `2`), a byte mask, the value, minimum, maximum. The
coordinates are in units of 32-byte memory rows: `column = (x << log2(bytes per pixel)) >> 5`, minimum `y << 16 | column`, maximum
`(bottom - 1) << 16 | column`, with the end column taken from the last byte of the last pixel. A rectangle whose first
column is partial gets a left-edge packet (mask `-1 << (start byte & 31)`), whose last column is partial a right-edge packet
(mask `-1 >> (32 - end bytes)`), and the full columns in between one packet with mask `0xffffffff`; a rectangle inside one
column gets the intersection of the two masks. The value is the colour replicated to 32 bits (8 bpp: the byte four times; 16 bpp:
the half-word twice; 32 bpp: as is). The packet count per dispatch is `0xa4`. Last packet carries a flush marker and is
dispatched as the last one. Established (routine read in full, immediates checked); the engine's mask and column reading is
inferred. This is the **only** 2D operation that uses a coordinate unit other than pixels, and the only one that ignores
the window origin and clip (`CoRectFill@0x1f25a` for DirectDraw sends window parameters first, but the clear packets do not
read them: unknown).

### Screen to screen copy

`vCopyBlt@0x12e30` (also the template of `vCopyBackToFront@0x12c4a`, `vMoveBits@0x1a3ae`, `vXferDMA_*`): the screen is
read **as a texture of its own format**. Packets: flush marker, draw-buffer descriptor if needed, ROP2 packet unless
`COPYPEN`, optionally the colour-key alpha test `0x924a5040` (mode bit 22, [below](#colour-key)) and the blend packet
`0x928a7040` (mode bit 23; `ulAlphaBlend`, set by no routine of this binary that was read), mode `0xc0000` (texture and pixel
transfer) plus those bits plus `0x1000000` for a ROP2, `0x93cc0080` with word 1 = the screen format word (`ulGdiColorFormat`)
and three zero words, `0x91ea2040` `0x62000000` 0, bias `0x04230040` `0.5` `0.5`, a 32-byte pad (dispatched as one
packet), then window parameters (mask 0, origin 0) and per rectangle one primitive: `0x18002020` with mode word, four
`0x18a33068` vertices `(x, y, s, t)` where `x, y` are destination (plus `xOffset`, `yOffset`) and `s, t` the source
position in texels, unnormalised, floats made by the integer table. Established.

- **Overlapping copies.** When the source rectangle overlaps the destination (the source's left and top are inside the
  destination's span on both axes: `param_5->x < param_6->right`, `param_5->y < param_6->bottom` and the two spans intersect) the
  primitive word is chosen by `dx = src.x - dst.left` and `dy = src.y - dst.top`: `dx >= 0, dy >= 0` gives `0x80000500`,
  `dx >= 0, dy < 0` `0x80000100`, `dx < 0, dy < 0` `0x80000300`, `dx < 0, dy >= 0` `0x80000700`; any other copy uses `0x80000000`. Bits 10:9 and 8 are the vendor's primitive direction and direction select
  fields ([packet-set.md](packet-set.md#the-vendors-register-word-types)). The triangle strip's vertex order is also
  rewritten (two orderings per case, chosen by comparing `width * dy` with `height * dx`) so the diagonal split falls on the side that
  reads before it writes. A model that copies through a temporary gets the same pixels; a model that streams texel reads and
  pixel writes in raster order must honour the direction bits or it will corrupt overlapped copies. Established (what is
  written); the direction bits' meaning is inferred from the field names and the case table.
- **Raster ops on a copy** are the 16 ROP2 codes of §5 applied between the source texel and the destination pixel, with the
  source on the GL "source" side. Whether the destination read in `D` is the pixel before this primitive or one the
  same primitive already wrote is what the direction bits above guard. Inferred.
- **Pixel centres.** Copy and pixel transfers use bias `0.5 0.5` (texel centre at integer vertex), stretch uses `0 0`. Established as sent; the engine's sampling is inferred.

### Mono colour expansion

`vXfer1bpp@0x15322`: for each source scanline, one horizontal line of two port-A vertices (prim `0x20008000`, line width 1, from
`x0` to `x1` at row `y`), preceded by the pair `0x04841020` (pattern) and `0x04640020` (repeat word). The pattern word is up to
32 bits of the source row **bit-reversed per byte** (the table `_ajRev`: GDI's most significant bit is leftmost, the engine's
first pixel is the least significant bit), up to four bytes for a row of at most 32 pixels after alignment to the byte, rows
wider than that are split in 32-pixel pieces; the repeat word is `0x1f00 \| (x & 7)`, a pattern length of 32 in bits
15:8 (value 31) and the starting bit in 7:0. The foreground colour goes to `0x18e84020`; with the mode word `0xa000` (opaque
stipple, line stipple) the background goes to `0x91ca1020`; for `0xe2` ("transparent") the mode is `0x2000` alone and the
background is not written; `vXfer1bppInv` swaps. A new packet starts after 0x62 lines. The colours come from the
translation (`pulXlate[1]` the colour of a set bit, `pulXlate[0]` of a clear bit). The same repeat word `0x1f00` and 16 or 32 bit pattern
are used by `bPutGlyphInCache` and `vTextOutFW_P2C..P5C` for glyph rows. Immediates and the loop structure read;
inferred as a whole (partial read). `DrvStrokePath` (styled lines) writes the repeat word differently: `(length - 1) << 8 \|
0x2000000` with the start phase in bits 7:0 and `phase mod 3` in bits 17:16 (the line is drawn at three times the pixel
density: `xyDensity` 3); the alternate-style case uses `0x100` and pattern 1. Inferred.

**Which colour a pattern bit takes.** Three paths agree: a set bit selects the colour sent to `0x18e84020` and a clear bit
the colour of `0x91ca1020` when mode bit 15 is set. (1) `DrvRealizeBrush@0x17970` stores the colour of translation entry 1 in the
brush's second colour and entry 0 in the first, and `vFillStip` sends the first to `0x91ca1020` and the second to
`0x18e84020`. (2) `vXfer1bpp` sends `pulXlate[1]` to the colour port. (3) The glyph cache fills a glyph's set pixels with colour
0 (colour port) and the rest with `0xff` (`0x91ca1020`, `bPutGlyphInCache@0x2da2a`), and the later alpha test discards `0xff`
(below). This settles the open item in [packet-set.md](packet-set.md) in favour of the inference. Inferred; no hardware
read is possible.

### Colour key

DirectDraw's source colour key (`DDBLT_KEYSRCOVERRIDE`, flag `0x10000` of `DdBlt@0x1f6ce`) is the alpha-test packet on the source
texel: `0x924a5040`, word 1 `(7 or 8) << 28 \| 0xa000000`, word 2 the key. `0xa000000` is function 5 in bits 27:25, which
`__glCoPassAlphaFunc` produces for GL_NOTEQUAL (the GL function minus `0x200`): the pixel is written when the source differs from the key.
The value in 31:28 is 8 for an 8-bit surface and 7 for 16 and 32 bit ones. The key is passed as the colour with its
channels expanded to eight bits by bit replication for 16 bpp
(`k5 << 3 \| k5 >> 2`, the code replicates the top three bits into the low three), as is for 32 bpp and 8 bpp.
Mode word bit 22 enables it. Established (`DdBlt` first 200 lines, `vCopyBlt`, `vStretchCopyBlt`, `vXferDMA_*`); the field readings are inferred:
31:28 reads as which channels take part (3 colour channels, or the index), and a 16-bit texel is compared after expansion
to eight bits per channel by replication, since the key is built that way. The `DDCKEYCAPS` the driver reports (`0x200`) is
the colour-space source-blit key only, with no destination key and no overlay key.

### Text

`DrvTextOut@0x2dfbe` caches each glyph bitmap in an 8-bit offscreen area, then draws the string as textured quads from
that area: mode `0x4c0000` (alpha test, texture, pixel transfer), alpha test `0x924a5040` with word 1 `0x8a000000` and
reference `0xff` (not equal to `0xff`), `0x93cc0080` word 1 `0x40100000` (type 8, depth 0, descriptor 2, the glyph area is
inside the screen buffer's own tiles), `0x91ea2040` `0x66000000` (environment function 3, type 3; the copy and the pattern
fill use type 1, `0x62000000`) and the colour for the text in `0x18e84020`. Box fills for opaque backgrounds are plain quads
under mode word `0`. The glyph cache is filled by `bPutGlyphInCache`: draw buffer `ulGdiColorFormat`, mode `0xa000`, prim
`0x20008000`, colour port 0, constant colour `0xff`, one two-vertex line per glyph row with the stipple pair above. The fixed-pitch
fast paths `vTextOutFW_P2C..P5C` write the bitmaps straight as stippled rows. Inferred (partial reading of
`DrvTextOut`, the cache routines read in the typed decompile).

## 5. The ROP2 table and the ROP3 reduction

The logic-operation word `0x92ca9020` has the hardware operation in bits 31:28. `sgcodd.dll` indexes a 17-dword table
`_lHWLogicOp@0x30b20` by the Windows `R2_*` number 1 to 16 (entry 0 is not used). Read from the file:

| `R2_` | 1 `BLACK` | 2 `NOTMERGEPEN` | 3 `MASKNOTPEN` | 4 `NOTCOPYPEN` | 5 `MASKPENNOT` | 6 `NOT` | 7 `XORPEN` | 8 `NOTMASKPEN` |
|---|---|---|---|---|---|---|---|---|
| word | `0x00000000` | `0x80000000` | `0x40000000` | `0xc0000000` | `0x20000000` | `0xa0000000` | `0x60000000` | `0xe0000000` |

| `R2_` | 9 `MASKPEN` | 10 `NOTXORPEN` | 11 `NOP` | 12 `MERGENOTPEN` | 13 `COPYPEN` | 14 `MERGEPENNOT` | 15 `MERGEPEN` | 16 `WHITE` |
|---|---|---|---|---|---|---|---|
| word | `0x10000000` | `0x90000000` | `0x50000000` | `0xd0000000` | `0x30000000` | `0xb0000000` | `0x70000000` | `0xf0000000` |

The value in bits 31:28 is the GL logic-operation code (GL enum minus `0x1500`: clear 0, and 1, and-reverse 2, copy 3,
and-inverted 4, no-op 5, xor 6, or 7, nor 8, equiv 9, invert 0xa, or-reverse 0xb, copy-inverted 0xc, or-inverted 0xd, nand 0xe,
set 0xf) with the pen or source as the GL source: every entry above is the GL code that computes the named R2 function.
Established (table read; the mapping to GL names is checked entry by entry; the engine implementing GL semantics is inferred).

The drivers reduce a ROP3 `r` to an R2 index by `((r >> 2) & 0xc) + 1 + (r & 3)` for fills (the operation does not
depend on the source) and `(r & 0xf) + 1` for copies (which do not depend on the pattern); `PATCOPY` (`0xf0`) gives 13,
`BLACKNESS` 1, `WHITENESS` 16, `DSTINVERT` (`0x55`) 6, `NOP` (`0xaa`) 11 which returns without a packet. The ROP2
packet is skipped for index 13 and the mode word's bit 24 (logic operation) is set otherwise. Established
(`vFillSolid@0x11cc6`, `vCopyBlt@0x12e30`, `vFillPat@0x12846`, `vFillStip@0x12282`, `DrvBitBlt@0x10726`).

## 6. DirectDraw, overlay and video

- **Capabilities** (`DrvGetDirectDrawInfo@0x1f4c9`, read in full): `dwCaps` `0x044a0240`, `dwCKeyCaps` `0x200`, `dwFXCaps` `0x15400`,
  `ddsCaps` `0x250`; plus `0x100` in `dwCaps` and two FOURCC codes (`YUY2`, `UYVY`) when the mode is not 8 bit and the engine
  floating-point save and restore entry points exist (`EngSaveFloatingPointState`, `EngRestoreFloatingPointState`). Decoded
  against the public DirectDraw header: block transfer, stretched block transfer, colour fill by blit, source colour key, read scan
  line, and a "vertical blank" item; FOURCC block transfers; surfaces: primary, off-screen, flippable.
  **No overlay (`DDCAPS_OVERLAY`), no destination key, no alpha capability, no video port bit.** The `0x4000` bit
  `DdBlt` tests on a surface is `DDSCAPS_VIDEOMEMORY`; the driver supports a YUV surface only as the source of a blit, in video
  memory, at x offset 0 (`DdCreateSurface@0x1ff28`). Established; the decode of the FX and key bits not checked beyond the raw values. Inferred.
- **Video on the engine**: video playback to the NT desktop is "blit with colour space conversion" through
  `CoYUVBlt_S2S`: a YUY2 or UYVY surface (type 14 or 15, 16 bits per pixel, as a texture with the colour matrix on) rendered into the
  screen, by way of a scratch rectangle and a second pass or a direct strip. The **matrix** is the table `CoMatrixYUV2RGB@0x33d24`: twenty floats
  `1.164383, 1.596027, 0, 0, -0.870788, 1.164383, -0.812969, -0.391762, 0, 0.529591, 1.164383, 0, 2.017231, 0, -1.081389, 0, 0, 0, 1, 0`
  then two words `0` and `0xffffffff` (the 22 words of `0x944c42c0`). The coefficients are the studio-swing BT.601 conversion
  (255/219 = 1.164383, 1.596027, 0.391762, 0.812968, 2.017232; the three constants are `-(16 * 1.164383 + 128 * k) / 255` for each
  output channel's offset). Established (table read); the BT.601 reading and the application order (column-major, applied to
  `(Y, Cb, Cr, 1)`) inferred; how the engine orders the YUY2 samples into Y, Cb, Cr is unknown.
  Scaling is the texture filter: nearest (word 2 of `0x93cc0080` is 0 in the 2D routines), so a stretched YUV blit
  does not interpolate (inferred).
- **Overlay plane.** The 8-bit overlay of the display back end (`ovr_control`, `0x20008`) is not a DirectDraw feature. The display driver
  uses it for an OpenGL layer plane (descriptor 5, `bHasOverlay` from mode flag `0x10`, double-buffered with descriptor 6 when
  flag `0x2000`) and for the software pointer (`vSetOvrFetch@0x1af5a` starts the fetch with bit 0 only when the
  cursor state or an OpenGL window needs it and stops it by writing the pointer without the bit). Established
  ([display-engine.md](display-engine.md#overlay-normal-plane-did)). Scaling, colour keys and YUV in the overlay are not used by any shipped software.
- **Flip.** `DdFlip@0x1fc54` is the miniport IOCTL `0x232080` (12 bytes in, 4 out); the 4-byte answer is the frame control word the next
  retrace should load. `ddrvalUpdateFlipStatus@0x1e514` compares it with the live copy at `+0x30008`; the driver answers
  `DDERR_WASSTILLDRAWING` until they agree. Established.
- **Video input and output.** The graphics blocks have none of it in the display back end. The Livid driver `sgilivid.sys` (disk 2 of the SGI
  digital media pack 5.2.2 for NT 4.0, 17,648 bytes, build 2001-10-29) maps four windows of `0x80` bytes each, at
  physical `0xf8000000`, `0xf9000000`, `0xfa000000` and `0xfb000000`, each with a second 8-byte mapping (that channel's time-stamp and counter pair at `0xff001340` to `0xff001358`) and a third 8-byte mapping at `0xff001200`
  (mapped again for each channel, read only), and creates four devices named `sgilivideoin1`, `sgilivideoin2`, `sgilivideoout1`, `sgilivideoout2` in the order
  `0xfa000000`, `0xfb000000` (flag 0) then `0xf8000000`, `0xf9000000` (flag 1). It locks user pages with an MDL and reads their
  physical addresses (`MmProbeAndLockPages`, `MmGetPhysicalAddress`), so the ports DMA to host memory by physical address.
  Established for the addresses, sizes and calls (the driver's init at `0x10d68`, image base `0x10000`); that the flag is
  direction and that the interrupts are Cobalt pins 16 to 19 are inferred. The pair and the `0xff001200` block are not video registers: they lie in the Lithium audio engine's
  page (`0xff001000`, the pair array from `+0x300` and the 64-bit system time counter at `+0x200`), which the video driver reads to time-stamp frames; the Lithium pages
  in the chips repository own them. **The `0xd0080000` capture words of the display back end are not this
  hardware's registers**: neither this driver nor the NT or Windows 2000 display drivers touch them. The register layout of the four ports was not decoded
  ([questions](../../questions/open-questions.md)).

## 7. What a guest can see exactly, and what may be approximated

**Bit-exact (a model must give these results).**

1. Every ROP2 of §5 applied to 8, 16 and 32 bit pixels in a fill, a copy and a host transfer, including `XOR`, `INVERT`,
   `NOP` (no write) and the copy of overlapped regions equal to a `memmove`.
2. Rectangle coverage of quads: pixels `[left, right) x [top, bottom)`. Fills with integer rectangles have no edge ambiguity.
3. The rectangle clear: 32-byte rows, left, middle, right edge masks, value replication.
4. Mono expansion: bit order (bit 0 leftmost), start phase `x & 7`, foreground, background, transparent mode `0xe2`.
5. Pattern and stipple alignment to the brush origin (`x & 7`, `y & 7`) and the 8 by 8 wrap; texture coordinate `(2x + 1) / 16`.
6. Colour key: source texel not equal to key, after the 5 to 8 bit replication for 16 bpp.
7. 16-bit and 32-bit pixel layouts of §2, and the 16-bit colour spread of `vFillSolid` (the colour the engine receives).
8. The YUV to RGB matrix of §6 and the `YUY2` / `UYVY` channel order, to the rounding the engine uses (rounding itself unknown).
9. The fence: bit 0 of the node's first byte clears when the engine is done, the serial the miniport sees reaches the last one
   sent, and a flush-marker packet with sync flags completes.
10. The `0x2000`-byte pitch of the CPU window onto the screen and its cache behaviour after `DrvSynchronize`.

**May be approximated** (nothing shipped reads the difference): line rasterisation rules for sub-pixel coordinates (the
vertices carry 1/16 pixel); stretch sampling (nearest at texel centre with bias 0 is what the driver asks for, but GDI stretch
tolerance is loose); the order in which an engine writes pixels within a quad other than the copy direction; whether the
engine clips to the one-rectangle screen mask (never needed by the routines read); any use of the interrupt flag of a
packet beyond completing it; timing.

## 8. Corrections to earlier pages

- [packet-set.md](packet-set.md) listed the line stipple repeat word as "the GL repeat factor, expanded" and the pattern as "the
  16-bit GL pattern in both halves"; that is the OpenGL driver's use. The display driver uses the same pair as a 32-bit pattern with the length field
  31 and a phase (§4). The `0x91ca1020` row said the glyph cache did not obviously fit the cleared-pixel reading; §4 shows it does. The note
  in [nt-display-driver-video.md](../../software/windows/nt-display-driver-video.md) that `0x20008000` is "a point" was wrong: it is a type 1 (line) word, and
  `0x8000` is the point word.
- The extent the chip set decodes for the four video windows is not shown by what a driver maps (`0x80` bytes each); keep the platform pages' extent.
