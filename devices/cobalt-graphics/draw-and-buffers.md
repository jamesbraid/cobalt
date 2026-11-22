# Cobalt rendering engine: drawing, buffers and textures as the OpenGL driver programs them

What the packets in [packet-set.md](packet-set.md) do together. Source: the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4
and its symbols, read as described there; the miniport `sgcomp.sys` 5.2.4 for the rows marked miniport. Everything
here is **inferred** from decompiled C unless a statement says otherwise; the engine's own behaviour is **unknown**
unless the SGI white paper ([board-data.md](board-data.md)) names it, and then the white paper is cited only as
agreement.

## Who does what

The driver transforms and clips on the host and also has paths that leave the transform to the engine.

- **Host.** `GenerateTransformer` and `GenerateTransformerKNI` (plain and Pentium III versions) generate x86 code at
  run time that takes an object vertex to clip coordinates, window coordinates and clip codes in the driver's vertex
  record (0x170 bytes). `__glDrawVertexes_*` walk the vertex arrays; they test the clip codes and call
  `__glDoClipTriangle` or the Cobalt clippers for primitives that cross the frustum, and otherwise call the context's
  render procedure for the primitive, which ends in the vertex packets below. `__glDrawVertexes_*` and the `Generate*`
  routines never write a header themselves; the headers come from the render procedures, the `__glCoBegin*` and
  `RestartDispatch*` routines and the generated handlers (`__glCoGeneratePrimitiveHandler`,
  `GenerateSetAttribs`, `GenerateQuadsHandler`, `GenerateHostCullHandler`, `__glCoGenerateNoXformHandler`), which
  carry the same header constants as immediates. Established for `__glDrawVertexes_Triangles` (read in full).
- **Engine.** The white paper lists vertex transform, frustum clip check, normal transform, lighting for up to four
  lights, face culling and set-up as engine functions. The ICD matches: it sends the model-view-projection matrix
  (`A` = `0x002`), the normal matrix (`0x012`), the viewport (`0x01b`), material blocks, light blocks and the scene
  ambient, a cull field in the primitive mode word, and in display-list replay it sets bits 31 and 30 of the flags
  word (`A` = `0x001`) before sending the matrix and then replays recorded vertex packets
  (`__glCoDlistExecutePackets`, read in full). So the engine can transform and light vertices; whether it does for
  immediate-mode vertices depends on the flags word, and the ICD also keeps a host lighting path
  (`__glCoValidateSoftLighting`, `__glCoGenerateCalcColor`, `GenerateCalcLightVectors`). The rule that picks one is
  not read. This answers the open question on host versus engine set-up for lighting and culling: both exist in the
  driver, the engine ones are the packets above.
- The three coordinate modes in bits 31:30 of the flags word are, from which vertex words the packet routine copies:
  `01` words 0 to 3 of the vertex record, `10` words 8 to 11, `00` the window x, y, z and the context's w; `11` is
  the display-list replay mode. Which are object, eye and clip coordinates is inferred from the order the driver
  computes them in.

## Primitive mode word

Header `0x18002020` (`A` = `0x0c0`). The ICD writes it from every `__glCoBegin*` and every `RestartDispatch*`
routine, from the fill routines and, in the miniport, from `CoCopyRect`. Fields, from the routines that build it:

| Bits | Meaning | Source |
|---|---|---|
| 31:29 | primitive type: 1 lines, 2 line strip and line loop, 3 triangles, 4 triangle strip and quad strip, 5 triangle fan, polygon and quads | the constants OR-ed in by `__glCoBeginLines`, `BeginLStrip`, `BeginLLoop`, `BeginTriangles`, `BeginTStrip`, `BeginQStrip`, `BeginTFan`, `BeginPolygon`, `RestartDispatchQuads` |
| 14:13 | cull: `01` cull front faces (bit 13), `10` cull back faces (bit 14), `11` both | `__glCoPassCullFace` (GL front `0x404`, back `0x405`, front and back `0x408`) |
| 26:15 (lines) | line width, 12 bits; the anti-aliased form ORs `0x3000` into the field | `__glCoPickLineProcs` |
| 15, 11 (points) | a point is `(0x10 or aa) * 0x800`: bit 15 set, bit 11 for anti-aliased | `__glCoRenderPoint` |

The line and triangle mode words are different context words (`+0x2dba4` and `+0x2dba8`) with the same layout; the
driver keeps the last value it sent and does not resend an unchanged word. Other bits of the word are not decoded.
Polygon modes (point or line instead of fill) are generated into the handlers (`__glCoGeneratePolygonMode`); not read.

Quads are sent as fans of four (type 5), quad strips and triangle strips as strips (type 4), polygons as fans; the
engine therefore has the strip and fan connectivity in hardware, as the white paper says ("connected line and triangle
mesh interface"). Inferred. The driver calls the same vertex-send routine for every primitive; only the mode word and
the number of vertices differ.

## Vertex packets

Two ports, `0x18a33060` (`A` = `0x0c5`) and `0x18c33060` (`A` = `0x0c6`), each with count 3. The routine
`__glCoSetVertexAttribsMask(vertex, mask, last)` builds them. Its steps, in the order it writes:

1. For each attribute selected by `mask` and enabled in the context, a packet of its own: bit 7 (`0x80`) a call to a
   colour-material helper; bit 3 (`0x08`) the per-light vector packets for lights 0 to 3; bit 4 the secondary colour
   (`0x190b1020`); bit 5 the texture coordinate (`0x198da080`); bit 6 the fog value (`0x192b2020`); bit 1 the colour
   (`0x09280080` or the colour index `0x18e84020`); bit 2 the normal (`0x19a85060`).
2. If bit 0 is set, the vertex itself: a header from the port table, `0x18a33060` or `0x18c33060` plus an
   **attribute-presence mask in the instance field**, then the payload in this order: the colour (four dwords) if mask
   bit 1, the normal (three dwords) if bit 2, a w dword if bit 4 (value `0x10`, set when bit 30 of the flags word is
   clear), the x and y coordinates, the z coordinate for port B only, and the texture coordinate (two dwords) if bit 3.
   A mask of zero means the coordinates only. The headers `0x18a33068`, `0x18c33062` and `0x18c33070` that occur in
   the ICD and the display driver are these sums (mask 8, 2 and `0x10`). The count field stays 3 for both ports, so it
   is not the payload length: port A sends two coordinates and port B three, and the extra dwords are implied by the
   instance bits. Checked in disassembly (`__glCoSetVertexAttribsMask`, the stores after the header); the engine's
   parser is unknown.
3. Port A (`0x18a33060`) is chosen when bits 19:18 of the vertex record's clip word (offset `0x30`) equal `01` and,
   in addition, either a context bit (`+0x2db9f` bit 5) is clear or the flags word has bit 30 set; otherwise port B.
   What the clip-word bits are, and why port A drops the z coordinate, is not known. The fill routines call the
   routine in pairs that suggest "vertex that continues the primitive" and "vertex that completes a triangle", but no
   routine states it.
4. A call with `last` set marks the end of a primitive; the routine avoids closing a packet node inside a primitive
   (it checks the context's no-break flag before it calls the overflow routine).

The fill routines call it with a fixed mask pattern. A smooth-shaded triangle calls the "send everything" entry
(`__glCoSetVertexAttribs`, mask `0xff`) for each of the three vertices. A flat-shaded triangle sends the attribute
bits `0x9e` once for the provoking vertex (colour, normal, lights, secondary colour) and then mask `0x61` (position,
texture coordinate, fog) for each of the three. Read in full for `__glCoFillFlatTriangle` and
`__glCoFillSmoothTriangle`; the mask bits are read from `__glCoSetVertexAttribsMask`. Clearing a buffer, writing a
pixel in the software span stores and copying a rectangle in the miniport use the same ports, so even 2D drawing is
vertex packets (`CoCopyRect` sends `0x18002020` and `0x18a33068` vertices).

## Lighting and material state

- Per-light state is a bank of four. `LoadLightSource` writes `0x0ca96120 | i` (9 words: the three colours of GL
  light `i`), `0x1129f020 | i` (one scalar, 1.0 for a directional light) and `0x09a900c0 | i` (6 words). For a light
  with a position the driver keeps per-light enable bits in a context word (`+0x2d5cc`) and resends the scalar and
  the six words with every vertex through the attribute-mask routine (bit 3).
- Materials are eight blocks, one per colour and side (front and back) (table in [packet-set.md](packet-set.md)):
  `ChangeMaterial*` send the single block, `__glCoValidateMaterial` sends all that changed. With colour-material
  tracking the driver sends the current colour and a `0x06c6d081` or `0x07a74081` form; the flag's meaning is not
  known.
- The scene ambient and a mode word (`0x05461060`, `0x05260020`) come from `LoadLightPath`. The mode word carries
  bit 31 from a lighting-model byte.

Agreement with the white paper: four lights; "support for accumulating a host-supplied contribution from other light
sources" fits the host lighting path and the `0x09280080` colour sent with a lit vertex. Not shown by the ICD: any use
of more than four lights in the engine.

## Colour, depth and stencil buffers

The two buffer descriptors are read-modify-write words kept in the context and sent after a flush marker.

**Draw-buffer descriptor** (`0x91aa0020`, context word `+0x2e30c`). Disassembly-checked: bits 24:19 are the low six
bits of the first dword of the buffer's record (the record that `CREATE_BUFFER` and `BIND_BUFFERS` set up), the other
bits are preserved. `__glCoInitRGB` clears bits 18:0 and sets the format field in bits 30:25 from the context's bits
per colour component (`gc+0x2460`): 4 gives `0b001101`, 5 gives `0b001001`, 8 gives `0b010010`. So bits 26:25 are a
size class (1 for the two 16-bit layouts 4-4-4-4 and 5-5-5-1, 2 for 8-8-8-8) and bits 30:27 the layout. Evidence for
the size class: `__glCoClearBuffers` reads `(descriptor >> 25) & 3` and branches on 0 (byte colour index), 1 (16-bit)
and 2 (32-bit, per-byte write masks). Inferred. The white paper's 8-, 16- and 32-bit colour formats fit.

**Depth and stencil descriptor** (`0x930ab020`, context word `+0x2e310`). Disassembly-checked: bits 28:23 are the low
six bits of the first dword of the buffer record (the record the context keeps for the current depth buffer, chosen
by whether the draw buffer is the left or right one). `__glCoInitDepth` sets bits 31:29 to `001` for a 16-bit depth
buffer and to `110` for 24 bits, and `__glCoClearBuffers` reads `(descriptor >> 29) & 3` as 1 or 2 accordingly. The
white paper says 16-bit floating point and 24-bit fixed point depth, with 8-bit stencil; the format field agrees by
size, and which size is the float is the white paper's, not the driver's. Whether stencil is in the same 32-bit word
as 24-bit depth is not shown by the descriptor.

Depth test (`0x936ae020`: function in 31:29), stencil (`0x938af040`), alpha test, blend, logic operation, colour mask
and fog colour are per-state registers; their layouts are in [packet-set.md](packet-set.md).

## Scissor, windows and clipping

- The scissor packet (`0x84e44040`) holds the minimum and maximum corner of the rectangle, x in the low half and y in
  the high half of each word, signed 16-bit, inclusive maximum. The ICD folds in the window origin and flips y
  against the window height before sending (decompiled C). The miniport's default is minimum `0xf000f000` and maximum
  `0x0fff0fff`, plus and minus 4096, which matches "addressing up to 4k x 4k" in the white paper.
- Window clipping ("screen masks and window clip IDs" in the white paper) is the miniport's work: it builds three
  window parameter packets (`0x98250160`, `0x9845b020`, `0x9865c020`) from the window record for each context switch
  and for GDI and pixel-transfer packets. The ICD never writes them. What the parameters are is not read.
- Face culling is a field in the mode word; frustum clipping is on the host (`__glDrawVertexes_*`).

## Clears

Two ways. A clear that needs the depth or stencil operations is drawn: `__glCoClearBuffers` sets the depth function
to "always" (`0x936ae020`, payload `0xe0000000`), the stencil registers (`0x938af040`, payload `0x2227ff00` OR a byte)
and the colour write mask, then sends the rectangle as vertex packets. A plain colour or depth clear uses the
hardware rectangle clear, `0x99ce00a0` (and `0x9e7200a0`, a second register bank, when the context has a second
buffer), five words:

1. the value to write (a colour in the layout of the buffer, or the depth);
2. a byte-enable mask for the 32-byte row;
3. the second value, from a parallel array (for depth and stencil clears);
4. the minimum corner as `y << 16 | tile column`;
5. the maximum corner, same form.

The rectangle is in units of 32-byte memory rows: 32 pixels per column at 8 bits, 16 at 16 bits and 8 at 32 bits
(`coClearBuffers`, the part that builds the packets, and the miniport's `CoClearRect`, read in full). A rectangle that does not start or end on a
column boundary is split into a left edge, a middle and a right edge, and the edge packets carry a partial mask:
`-1 << (pixels * bits)` on the left, `-1 >> ...` on the right, where a pixel is 1, 2 or 4 mask bits. So the mask is
one bit per byte of the 32-byte row, and the engine is addressed in 256-bit memory words, which matches the 288-bit
(256 data plus ECC) main memory bus the white paper describes. The white paper's "clear operations at full memory
bandwidth" fits a register-driven clear. The width of the unit is established from two routines; the register
meanings are inferred.

## Pixel transfers and textures

- **Pixel transfers** (`glDrawPixels`, `glReadPixels`, `glCopyPixels`, and bitmaps) are done with the same packets: a
  primitive-mode word, vertex packets for the destination rectangle, the transfer parameters (`0x93cc0080`,
  `0x91ea2040`), the colour matrix (`0x944c42c0`) and the 4 by 4 scale-and-bias the white paper calls "4x4 color
  matrix for color space conversion, scale/bias, and component swizzling". The four context fields packed into the two
  parameter packets are not decoded. The routines are `__glCoLinearToTileXfer`,
  `__glCoTileToLinearXfer`, `__glCoTileToTileXfer`, `coProcess*` and `coLoadLinearTlb`.
- **Texture images are stored by the CPU.** `coStoreTexImage` and its helpers copy texels into texture memory pages
  from the host with ordinary stores (UMA), after `coAllocTextureTiles` asked the miniport for the pages. The only
  engine packet it can add is `0x9ed380e0` (`coVhdlTexImage`), and that is sent only when a global that defaults to 0
  is set (`coUpdateCommonTile`, `coStoreTexImage`), so it is a debugging or simulation hook: its name says VHDL, and
  its payload is three caller words, a pixel count, the width and the corner pair. Read in full. The white paper's
  "memory management hardware for allocation of graphics buffers in system memory" and the miniport's translation
  table are the allocation side.
- **Texture level lifecycle.** `__glCoCreateTexLevel` computes the level's size (`coComputeTexLevelSize`), asks for
  pages, and, if the texture is currently bound, writes `0x97400020` with payload 0 before freeing the old pages.
- **Binding.** `coCreateTexBindPkt` asks the miniport (escape `0x57`, `CREATE_TEXTLBPKT`) for a prebuilt packet that
  loads the translation-table entries for the texture's pages (class `11` headers, built by `CoBuildTlbPacket`, which
  starts the body with a flush marker and ends it with a chain packet). The ICD replaces the marker at the start of
  the body with the chain header `0x97000040`; `coDispatchTexBindPkt` then ends the current node with a chain packet
  and stores the position words of the texture packet, so the texture packet runs between two ICD nodes. A bind is
  therefore made of chain packets, not of a register write from the ICD. The order of the jump into the packet and the
  return is not decoded. Texture addressing, filtering and wrap modes are not packets of their own in anything
  read: the texture parameters go in the two transfer-parameter packets (`coSetTexParam` writes `0x91ea2040` and
  `0x93cc0080`), which is why they are named "transfer parameters" here.
- **Texture memory and unified memory addressing.** Buffers and textures sit in 4 KB-multiple pages named through the
  engine's translation table, 2048 entries of 8 bytes at `0x8000` ([graphics-engine.md](graphics-engine.md)), with
  the per-context base-pointer table at `0xc000`. The white paper says the pages are "organized linearly (without
  tiles)". The entry format, page size and which addresses go through the table are not determined.

## Unknown, with the route

- What the engine does with the attribute-presence mask in the vertex header, and the difference between ports `0xc5`
  and `0xc6`. Route: the miniport's use of `0x18a33068` in `CoCopyRect` (only its header constants were read) and the
  display driver `sgcodd.dll`, which draws 2D with the same ports.
- The `B` field of every header. Route: all three binaries use it consistently; a table of (A, B) pairs is in
  [packet-set.md](packet-set.md#header-word-layout) and a relation between them across the whole register file is
  the next thing to look for.
- Bits of the pixel-pipe mode word (`0x80003020`) other than 13, 22 to 25. Route: the Enable* routines for each
  GL capability set one bit; the remaining ones are in `GenerateSetAttribs`.
- The window parameter packets. Route: `CoBuildWinparamsPacket` and the window record it reads.
