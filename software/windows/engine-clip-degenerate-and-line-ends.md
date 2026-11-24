# What the drivers expect of clipping, degenerate primitives and line ends

Four questions about engine behaviour that the drivers' packet streams bear on: whether the window-parameter clip cuts writes
to a flat host bitmap, whether the drivers send collinear fans or empty rectangles, and whether a one-row textured line is
end-exclusive. Source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 (the Windows 2000 OpenGL driver is the same
program, [comparison](w2k-opengl-driver-comparison.md)), read in Ghidra with the vendor's symbols; grades as in
[AGENTS.md](../../AGENTS.md). Flat buffers and their pitch are in
[nt-display-driver-video.md](nt-display-driver-video.md#flat-host-buffers-row-pitch-and-the-staging-area): bit 19 of the
descriptor is a flat marker and the row pitch is 2048 bytes.

## 1. Does the window-parameter clip cut writes to a flat host bitmap?

**What the driver sends. Established** (`vXferDMA_SB@0x148d0`, `vXferDMA_SB_Buf@0x14442`, `vFillSolid@0x11cc6`,
`CoBuildWinparamsPacketForPxfer@0x16f7a`, read in full).

- **Display driver readback and upload.** Each batch of the screen-to-host routines (`vXferDMA_SB`, `vXferDMA_SB_Buf`) begins with the
  window-parameter packet `0x98250160` with a valid mask of 0 and zeroes after it, then `0x9845b020` with origin 0, then the
  primitive mode `0x18002020` / `0x20008000` (lines, width 1). So a clip packet does precede the readback lines, but it names no
  rectangle. The same mask-0 packet heads every unclipped fill (`vFillSolid` with no clip object): the driver draws a whole rectangle
  under mask 0 and relies on nothing being cut, so mask 0 means "no clip rectangle is active" in the driver's own use. The vertices carry
  buffer coordinates: x in pixels and y as the row of the flat buffer (the offset split of the pitch page).
- **OpenGL transfers.** The miniport builds the window packet for a node. When the node's flag word has bit 5 set (the flag the
  pixel-transfer set-up uses), `CoBuildWinparamsPacketForPxfer` sends mask `0x0c000030`, one rectangle with minimum `0xf800f800` and
  maximum `0x07ff07ff` (x and y from -2048 to 2047 as signed 16-bit halves), and origin `0xf800f800`; otherwise mask 0. It then sends the
  scissor packet `0x84e44040` with minimum 0 and maximum `0x0fff0fff` (0 to 4095 in each axis), and the texture-window origin. Every
  value is the whole addressable range; nothing in a transfer is narrowed to the destination. The destination's extent is the rectangle
  of lines the driver draws.

**Engine side. Unknown for coordinates; inferred for effect.** No shipped path wants a flat bitmap write cut by the window clip, and a
cut at the window rectangle would drop readback lines when the host bitmap is wider or taller than the window. **Emulator choice:** apply
the window clip and scissor only to the colour or depth buffers they describe; do not cut writes whose destination is a flat buffer,
and treat mask 0 as unclipped. Which coordinates the clip rectangle is expressed in (before or after the origin) is not decided by
any driver (the question stays in [open-questions.md](../../questions/open-questions.md)).

## 2. Flat row pitch

Answered in [the pitch section](nt-display-driver-video.md#flat-host-buffers-row-pitch-and-the-staging-area): descriptor bit 19 is a flat marker and rows are 2048 bytes apart.

## 3. Collinear fans and zero-width rectangles: does the driver filter them?

**OpenGL driver. Established for the host triangle path, inferred for the rest.** `__glCoRenderTriangle@0x69024864` sorts the three
vertices by y and forms the signed area from the two edge vectors. It then uses only the sign bit of that area (with the swap parity) as
the index into the cull table, and returns only when that face is culled (the face byte at context `+0x3dd0`). A zero area has its sign
bit clear, so it is classified as one face and sent unless that face is being culled. There is no test for zero area. The fan, strip
and polygon begin routines (`__glCoBeginTFan`, `RestartDispatchTFan` and the clipped paths) send their vertices without an area test.
So the driver can send collinear triangles and fans; it filters only by cull face and by host clipping. A zero-width rectangle from
`glRect` goes through the polygon path as a degenerate quad, unfiltered the same way.

**Display driver. Established for the routines read.** Rectangles reach the engine as fans of four (primitive type 5, `0xa0000000`):
`vFillSolid` builds them from the destination or clip rectangles with no emptiness test, and `vXferDMA_SB` computes the width as
`right - left` without a test. GDI does not hand a driver an empty destination rectangle or an empty clip rectangle, so an empty
fan reaches the engine only if GDI breaks that rule. The driver has no filter of its own.

**Engine side. Unknown.** The model draws nothing for a zero-area fan and logs it; no shipped software depends on a degenerate fan
drawing pixels (the OpenGL driver's culling already assumes zero-area triangles cover no pixels). The choice is safe.

## 4. Is a textured line in pixel-transfer mode end-exclusive?

**What the driver computes. Established** (`coTileToLinearSpans@0x6903c176`, `coLinearToTileSpans@0x6903a7e6`, `vXferDMA_SB@0x148d0`,
`vXferDMA_SB_Buf@0x14442`, `vInitFloatTable@0x1afd2`). Every row of a transfer is two vertices under `0x18a33068` (port A with the
texture coordinate pair) in a primitive of type 1, width 1 (`0x20008000`):

- start vertex: x = the row's first pixel, texture s = the source column of the first texel;
- end vertex: x = **first pixel + width**, texture s = **source column + width**.

The coordinates are whole numbers (the display driver's float table holds `(float)i`, no half-pixel offset), so the end vertex is one
past the last pixel to write, not the last pixel. The same holds for draw (host image to buffer, destination x from `x0` to `x0 + width`),
read (`x` to `x + width` over the flat host buffer) and the three-pass RGB route (texture runs over `3 * width` texels while the line
covers `width` pixels). Copy (buffer to buffer, `__glCoTileToTileXfer`) and the display driver's `vCopyBlt` send rectangles with
the right and bottom edge one past the last pixel (`(bottom - 1)` appears only in clip rectangles, which are inclusive).

**What it relies on.** If the engine drew the end pixel the driver would write `width + 1` pixels per row. Each line is a single row of
the destination: the routines step the flat buffer's position by one 2048-byte pitch per row and send one line per row, so a line
never runs on to another row. When `width * bytesPerPixel` equals the 2048-byte pitch, an inclusive end pixel would be the first
pixel of the next row and would overwrite it; and for any width the texture coordinate would read one texel past the span. So the driver
depends on the end vertex being exclusive for textured lines in transfer mode, at least on the pixel and texel counts. Graded inferred for
the engine (the rule is read off what the driver would otherwise corrupt), established for the endpoints the driver computes.
Whether the rule is "end-exclusive for every line" or only for horizontal textured lines is not decided by any driver, because every
transfer line is horizontal: its y is the same at both ends.

**Emulator choice:** for a line with a texture coordinate, in pixel-transfer mode or any other, a horizontal line of integer endpoints
covers `x0 .. x1 - 1`, and the texture coordinate steps from `s0` over `x1 - x0` pixels. Keep the choice for non-horizontal lines
unspecified; no driver sends a textured diagonal line in this mode.
