# OpenGL driver: buffer swap, points, pixel formats, host span stores, read-backs, context dump and face state

What the Cobalt OpenGL driver (`sgcoicd.dll`) sends the engine, and expects back, for nine things that the other pages leave open.
Source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 (patch SG0020054), read in Ghidra with the vendor's symbols and types and
checked in the disassembly; the display driver (`sgcodd.dll`) and miniport (`sgcomp.sys`) of the same pack where named. The Windows 2000
5.2.4 OpenGL driver is the same program ([comparison](w2k-opengl-driver-comparison.md)); its request values at the call sites below
were compared and match. Grades as in [AGENTS.md](../../AGENTS.md): what a routine writes is established where the routine was read in
full; what the engine does with it is inferred or unknown, with an emulator choice where software depends on it.

## Buffer swap: an engine copy, or a flip of the displayed tiles

**What the driver sends. Established.** `__glCoIcdSwapBuffers@0x69018562` dispatches the open node with request 2 (link flag 3), and for a
double-buffered context sends escape `0x48` ([escapes](opengl-escapes.md)) with the node position, the swap rectangle hint, flags and
interval. The miniport queues it as drop type 4. `CoSetChunk@0x1905d` rewrites a type 4 drop to type 9 when the byte `VrSyncSwapbuf`
(device extension `+0x101`) is 0. That byte is the registry value `SwapVertRetrace` under
`HKLM\Software\Silicon Graphics\Control Panel\Graphics`, read in `CobaltStartIO@0x12634`: default 1, 1 when the value is absent, and
forced to 1 when stereo is on.

- **Type 4 (the default). A copy of the partly covered tiles and a flip of the whole ones.** `CoScheduleSwap@0x1934a` makes engine copies of the drawable's rectangles with `CoCopyRect`
  (the copy region, then the clip region when flag 4 is set), run after the vertical retrace; the whole tiles are flipped when the held chunk is released ([the tile flip](nt-swap-tile-flip.md)): `CoProcessRetraceEvent@0x17dae` releases the
  held chunks on the retrace event only while the beam is between the pre-blank scan line and `height - cmcfrtsqOverhead`.
- **Type 9 (`SwapVertRetrace` 0). A flip.** `CoScheduleUnsyncedSwap@0x194be` does no engine work. With the flip flag, `asFlipTiles@0x1c08c`
  exchanges the translation-table entries of the window's tiles between the front and back global buffers (ids 2 and 3, and the two
  right-eye buffers in stereo), writes the new front's frame numbers (`tileAndBank >> 4`) into the display's alternate tile list, and
  marks the table to be reloaded; the next vertical-blank event toggles the current list and writes its physical address with bit 0 to
  the display register at `+0x3000c` ([display-engine.md](../../devices/cobalt-graphics/display-engine.md)). When GDI is on the overlay
  and one OpenGL node exists, `asFlipFrontBack@0x1c220` exchanges the whole front and back lists instead.

So a swap reaches the screen by an engine copy of the partly covered tiles and a flip of the whole ones, and by the flip alone when the registry value turns the unsynced form on. The pixel-format list advertises both: each
double-buffered format appears twice, plain and with `PFD_SWAP_COPY` (`0x400`), and the driver sends the same escape for both.
**Emulator choice:** implement both. The default swap needs the copy and the tile exchange together, with the display tile-list register latching on vertical blank;
the flip alone is the registry-selected form.

## Points

**What the driver sends. Established** (`__glCoPickPointProcs@0x69037732`, `__glCoRenderPoint@0x69036c56`, `__glCoRenderWidePoint@0x69036cb4`,
`__glCoRenderAAPoint@0x69036e94`, `__glim_PointSize@0x690a274a`, `RoundWidth@0x690a27e2`). `glPointSize` keeps the float and an integer
size `S`: 1 below 1.0, otherwise `trunc(size + 0.5)`. All three renderers write the primitive mode word (`0x18002020`); the two forms of its
size field are the ones [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#primitive-mode-word) lists for lines.

| Case | Mode word | Vertices |
|---|---|---|
| `S` = 1, not anti-aliased | `0x8000` (type 0, size field 1 in bits 26:15) | one, on port B |
| `S` > 1, not anti-aliased | `((S & 0xfff \| 0x4000) << 4 \| a) << 11`: bit 29 (type 1, lines), `S` in bits 26:15, bit 11 `a` | two, at `(x0, y)` and `(x0 + S, y)` |
| Anti-aliased, any size | `0x8000` | one 1-pixel point per covered pixel, alpha scaled by a coverage the host computes |

For the second row `x0` is `trunc(x - 0.5) - S/2` for odd `S` and `trunc(x - S/2)` for even `S`, and `y` is `trunc(y - 0.5)`: the host
snaps the centre to the pixel grid and draws a horizontal line of length and width `S`, which covers an `S` by `S` square if the engine
centres the width on the line. The driver also masks the draw-flags word with `0xffffdfff` (clearing bit 13, the line stipple) around the wide
point and restores it after. A mask of `0x440` goes with a size-1 point, `0xc40` with any other. Sizes above 1 are therefore
drawn by the engine as a wide line; nothing is drawn on the host except the per-pixel coverage of anti-aliased points. Which side of a
line the width extends to, and the rule for the ends, are the engine's line rules (unknown, [open questions](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle)).
`GL_POINT_SIZE_RANGE` is 0.5 to 32 with granularity 0.125, so `S` is 1 to 32. **Emulator choice:** treat type 1 with width `S` and the two
vertices as sent; no point-sprite path is needed.

## The display driver's pixel-format list (escape `0x5d`) and the accumulation buffer

**What the driver sends. Established for the table, inferred for the loop reading.** `CreatePixFmt@0x2f5b4` builds the table. The
extended list for escape `0x5d` (`DrvOpenGLBppEXT@0x2f98e`) is the fixed flag word `0x4000007` applied to four colour kinds in this
order; the plain list (`DrvOpenGLBpp@0x2f8c4`) uses the per-mode flag word the miniport computes (`DescribeValidMode`: double buffer,
depth variants, overlay layers, stereo).

| Kind | Colour | Alpha | Accumulation |
|---|---|---|---|
| 6 | 32 bits, 8:8:8:8, red shift 24, green 16, blue 8 | 8 | 16 bits per channel, 64 total |
| 5 | 15 bits, 5:5:5, shifts 11, 6, 1 | 0 | 16 per channel, 64 |
| 4 | 12 bits, 4:4:4:4, shifts 12, 8, 4 | 4 | 16 per channel, 64 |
| 1 | 8-bit colour index (`PFD_TYPE_COLORINDEX`, red bits 8, palette needed) | 0 | none |

For each kind the extended list has nine entries: depth 24 with stencil 8, depth 16 with no stencil, and depth 0, each as double buffered,
double buffered with `PFD_SWAP_COPY`, and single buffered. No format has auxiliary buffers. The `dwFlags` composition (OpenGL, window, GDI and swap bits) was not decoded bit by bit. Unverified. That is 36 entries; the loop nest was hand-decoded from the typed decompile, so the
counts are inferred.

**The accumulation buffer is the host's. Inferred (partial read).** The driver advertises 64 accumulation bits for every colour-format
kind and implements them with SGI's generic software accumulation (`__glInitAccum64`, `__glCoInitAccum64@0x6904383d`): the cells live in
host memory, `coAccumLoad@0x69043896` and `coAccumulate@0x69043d54` first read the colour rectangle with an engine transfer
(`coGetBuffer`), and `coAccumRtrn@0x6904426c` writes the result back through the draw-pixels path (`GLCOACCUMSPANS` in the environment
selects a per-span return, `coPerSpanRtrn@0x69044160`). The engine needs no accumulation support; it must run the pixel transfers.

## Host span stores and the dither pattern

**What the driver sends. Established** (`StoreRGBASpan@0x690453ee`, `__glDitherRGBASpan@0x690b90c2`, `__glGenericPickSpanProcs@0x69077bf6`).
The span store used when the driver draws spans itself does not write memory. It sends, for each pixel, a colour register write
(`0x18e84020`) and a one-pixel point (mode `0x8000`, vertex `0x18c33070` for the first pixel, `0x18a33060` for the rest), after setting
the draw-flags word to `word & 0x4000000`: every engine stage, including the dither bit (25), is off and only the partial-colour-write
flag survives. The dither is applied on the host before the store, by SGI's generic ordered dither: `c' = min(max, (trunc(c * 16 + 0.5)
+ d) >> 4)`, with `d` from a 4 by 4 table indexed by `(y & 3, x & 3)`:

```
 0  8  2 10
12  4 14  6
 3 11  1  9
15  7 13  5
```

(read from the data at `0x690f6168`). So the host spans mirror nothing of the engine's dither: the engine's is switched off for them,
and this table is generic code, not evidence of the engine's. **Unknown:** the engine's own dither pattern and rounding; no shipped
routine depends on them. **Emulator choice:** any ordered pattern; the 4 by 4 table above is a reasonable one.

## CPU reads of colour and depth

**What the driver does. Established for the OpenGL driver, partly for the display driver.** No OpenGL driver path reads a colour or depth
buffer by CPU after the node's owner bit clears without an engine read in between. The span readers (`ReadRGBA8Span@0x69044ef8`,
`ReadCISpan@0x69045ed6`, `__glCoSpanReadDepth@0x690417a4`, `__glCoSpanReadStencil@0x69041abe`, and `coGetBuffer` for the accumulation
buffer) all call `__glCoTileToLinearXfer`, the engine transfer with its finish and wait; none maps an aperture window. The display
driver's CPU reads of the tiled screen (`DdLock`, the cache-line fix-up after a screen-to-host transfer, the CPU window copies of GDI
fallbacks) all follow `DrvSynchronize@0x2b2ea`, which sends a flush marker, waits for the miniport's retired serial and then spins on the
owner bit of the last node. Consequence for a lazy write-back mode: it is safe for every OpenGL path; for GDI and DirectDraw the write-back must complete
no later than the fence's return, which is the same moment the owner bit clears, since those readers do not issue a transfer.

## The occlusion count

[The read-backs page](engine-readbacks.md#1-the-packet-nodes-first-dword) holds the driver side. One addition from the link flags: the
three instrument routines dispatch the open node with request 4, so the node's link word is `5` (owner and sync); the verdict is bit 31
of that node's first dword, read when the ring reuses the node, as before. **Emulator choice (unknown in hardware):** set bit 31 when the
node retired and any fragment passed the depth test while the occlusion bits (28:27 of the depth word) were on; a constant 0 is accepted.

## The context dump

**What the driver does. Established** (`pCoContextSwitch@0x16610` read in full). The miniport restores a context by chaining packets:
on a real switch it writes the node's dump position to `0x18800` for the outgoing context, then builds a restore sequence in the incoming
node's context-switch page: the window-parameter node, the translation-table node, and the **saved-state node**, a 26-unit node whose link
word is `resumePosition << 12 | 0xd1`. The saved image is the memory the engine wrote at the dump position. The miniport stores into it:
dwords 200 and 201 (the window's texture origin, only when the window moved), and a long chain packet at byte `+0x328` (dword 202):
header `0x97000060`, next position `resumePosition << 12`, zero, and the current texture id at `+0x334` (dword 205). So the image is replayed as packets. An
engine's dump must therefore be a valid packet stream in the layout of the default image, at most 200 dwords, with dwords 200 to 207 left
alone. **Emulator choice:** write the same packets the default image holds, for the state the model keeps, and leave the tail untouched.

## Two-sided lighting and `FillFrontBack`

**The driver never splits two-sided fills by face. Established.** `__glCoFillFrontBackTriangle@0x69025c2e` and the flag at
`gc+0x3e54` that selects it concern drawing to more than one *buffer*: `__glcoim_DrawBuffer@0x69028d30` and `__glCoSetCurrentBuffers`
set the flag when the draw buffer is front and back (or left and right in stereo), and the fill routine runs the same render procedure once per buffer
(`__glCoSetDrawBuffer` to each of the saved buffer pointers, with depth write cleared on the later passes). The name is the buffer pair,
as in `asFlipFrontBack`. An earlier version of these pages read it as two-sided face handling; that reading is withdrawn.

What the driver does for two-sided lighting is send it to the engine: `LoadLightPath@0x690267b4` sets bit 31 of the lighting-mode word
(`0x05260020`) from the GL light-model flag whenever the engine does the lighting, and the driver sends both material sets, front
(`0x06c6d08x`...) and back (`0x07a7408x`...). So the engine's two-sided selection is exercised by any program that enables two-sided
lighting on the engine path. What the engine does is inferred from the names: with bit 31 set, a back-facing triangle is lit with the back
material block, with the normal reversed. **Emulator choice:** implement that; nothing shipped shows more.

## Cull bits and the front-face bit with y running down

**What the driver sends. Established** (`__glCoPassCullFace@0x69024320`, `__glCoPassFrontFace@0x69024378`, `UpdateViewport@0x690198ee`).
In the triangle mode word, `glFrontFace(GL_CCW)` (`0x901`) sets bit 12 and `GL_CW` (`0x900`) clears it, with no inversion: the driver does
not compensate for its y flip anywhere else. `glCullFace` sets bit 13 for front (`0x404`) or bit 14 for back (`0x405`), both for front
and back (`0x408`), and enabling culling clears them again. The viewport y scale is the negative of half the height
(`UpdateViewport`: `-0.5 * height`), so every vertex the engine receives has y running down the screen.

**What the engine must do for the driver to be correct. Inferred.** A triangle that is counter-clockwise on the screen, as the viewer
sees it, is counter-clockwise in OpenGL's y-up window space, and the flip preserves that. With bit 12 set meaning "`GL_CCW` is front",
the engine must call a triangle front-facing when it looks counter-clockwise on screen, which in the y-down numbers it receives is a negative value of
`(x1 - x0)(y2 - y0) - (x2 - x0)(y1 - y0)`; with bit 12 clear, front is the clockwise look. Bits 13 and 14 then cull the faces so called.
No shipped routine contradicts it and none shows the hardware's own formula, so the sign is a requirement of the driver, not a read
register. Strips alternate their winding; the driver sends them as type 4 and type 5 packets with no per-triangle flip, so the engine is
inferred to correct the parity itself. **Emulator choice:** as above.
