# Cobalt rendering engine: drawing, buffers and textures as the OpenGL driver programs them

What the packets in [packet-set.md](packet-set.md) do together. Source: the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4
and its symbols, read as described there; the miniport `sgcomp.sys` 5.2.4 and display driver `sgcodd.dll` 5.2.4 for the
rows marked so. A statement about what a routine does is **established** where it cites a routine read in disassembly
(`file@address`); everything else is **inferred** from decompiled C, and the engine's own behaviour is **unknown**
unless the SGI white paper ([board-data.md](board-data.md)) names it, and then the white paper is cited only as
agreement.

## Who does what

The driver transforms and clips on the host and also has a path that leaves the transform to the engine. Which one runs
is decided per `glBegin`, and the lighting path is decided per validation. Everything in this section about the
decision is **established** (the routines named were read in disassembly); what the engine does on each path is
**unknown**.

**Transform: engine or host.** `___glcoim_Begin@0x6904319e` sets the draw flags word (`gc+0x2dbb0`, sent as
`0x00210020`) before every primitive, from the primitive's mask word (`gc+0x2d920 + 4 * type`):

| Condition | Draw flags bits 31:30 | Meaning |
|---|---|---|
| mask bit `0x400` set: the primitive is drawn by a host fill routine | `00` | the host sends window x, y, z (vertex +0x40) and the context's w |
| else, engine transform not allowed or not requested | `10` | the host sends the four words at vertex +0x20 (computed on the host) |
| else, engine transform on | `11` | the vertex packets carry the four words at vertex +0, and the modelview-projection matrix is resent if it changed (`__glCoPassMVPMatrix`); the vertex-send routine tests bit 30 first, so `11` reads as `01` |

Engine transform is on when (`gc+0x2d583` is nonzero and `gc+0x2d960` is nonzero) or bit 2 of `gc+0x2e494` is set
(`sgcoicd.dll@0x69043267`..`0x690432b5`). `gc+0x2d583` is 1 unless the variable `GLCOALLOWCHIPXFORM` is set to 0.
`gc+0x2d960` has bit 0 from the variable `GLCOSIMFORCECHIPXFORM` (read once, `GetBoardInfo@0x6901ae92`) and bit 1 set
while the hint `0x80F0` (`GL_CLIP_VOLUME_CLIPPING_HINT_EXT`; the context word `+0x1cd0` equal to `GL_FASTEST`) is in force (`___glcoim_Hint@0x69029090`,
`__glCoProcessHints@0x690291c8`). `gc+0x2e494` bit 2 comes from a control-panel registry value named `ShadowTransform`,
read only for 32-bit RGBA contexts. So an application gets the engine transform by setting that hint, which is its
promise that the geometry needs no clipping, or the user turns it on in the control panel; otherwise the driver transforms
and frustum-clips on the host. The mask bit `0x400` is set (with `0x40`, as `mask | 0x440`) in
`__glCoPickTriangleProcs@0x690353ac` when the triangle goes through a host fill routine: anti-aliased polygons
(`__glCoFillAntiAliasTriangle`), a polygon stipple that `__glCoConvertStipple` could not reduce to an 8 by 8 pattern
(`__glCoFillStippledTriangle`) and two-sided processing (`__glCoFillFrontBackTriangle`); `__glCoPickPointProcs` sets
`0xc40` or `0x440` for points the same way. In feedback and selection mode every mask is `0x40`.

**Clip.** The host clips. The guard-band factors are computed from the viewport in `UpdateViewport@0x690198ee` (the
variable `GLCOFORCECLIP` turns the factors off, `GLCOCLIPTOBIGFRUSTUM` copies them to a global table). Whether the engine
clips when it transforms is not read; the hint above is the application's promise that nothing needs clipping.

**Lighting: engine or host** (`__glCoValidateLighting@0x690269f6`, `__glCoValidateSoftLighting@0x6902702a`,
`LoadLightPath@0x690267b4`, `__glCoPickSetMask@0x6902ddf8`, `GetBoardInfo`):

1. Soft lighting. `gc+0x2d57e` is set at start-up when the context is colour-index (`gc+0x2459`) or the variable
   `GLCOSOFTLIGHTING` is nonzero. Then `InitFnPtrs` (stores at `sgcoicd.dll@0x6901a668` and `0x6901a67e`) installs `__glCoValidateSoftLighting` and the generic
   colour-material pick: the lighting-mode packet is sent with bit 23 clear, the path word is 2 (10 with `+0x3d80` bit
   28), the host computes every colour and the vertex packets carry it. This is the only lighting the driver has for
   colour-index contexts.
2. Otherwise `__glCoValidateLighting` installs and runs. With `GL_LIGHTING` off the path word is 0 and no lighting
   packet is sent (the vertex colour is sent, plus the secondary colour when `+0x3d80` bit 29 is set). With it on the
   path word is 1 and the engine does the lighting for the lights it was given, lights loaded in order into at most four
   slots (`LoadLightSource`). A light is left to the host, and the path word becomes 3 (engine lighting plus a host contribution), when any of:
   - it is the fifth or later enabled light;
   - one of its ambient, diffuse or specular colours is outside 0 to 1 (`IsColorOutOfRange@0x6902978c`, called by
     `___glcoim_Lightfv` for each of the three, sets a bit in the flag word at light `+0x108`);
   - it is positional, its constant attenuation is below 0.5 and either the variable `GLCOLIGHTINGWAR` is nonzero or its
     linear and quadratic attenuation are both zero (the node fields `+0xf0`, `+0xf4`, `+0xf8` are the stores
     `___glcoim_Lightfv` makes for `GL_CONSTANT_ATTENUATION`, `GL_LINEAR_ATTENUATION` and `GL_QUADRATIC_ATTENUATION`: the
     base of the two structures is inferred, the offsets agree).
3. Colour-material tracking also puts the colour on the host: when the tracked-material mask `gc+0x2d5a8 & 0x199` is
   nonzero and the path word is above 0, `LoadLightPath` is called with the host flag, which clears bit 23 of the lighting
   mode word, and `gc+0x2d608` becomes `__glCoCalcRGBColor` (the host computes the lit colour per vertex).
4. With the engine path, the per-vertex helper in `gc+0x2d608` is picked from a table by a small index: 2 for a host
   flag, 1 for local lights, 4 for `+0x3d80` bit 28 (index 1 `__glCoCalcLightVectors`, 2 `__glCoCalcExtraColor`, 4
   `__glCoCalcSpecColor`, every combination `__glCoCalcAllCases`, 0 a no-op; `sgcoicd.dll@0x690f390c`). These produce the extra per-vertex packets the engine needs:
   the light vector and scalar packets for positional lights (attribute-mask bit `0x08`), and a host colour contribution
   (`0x02`) which matches the white paper's "accumulating a host-supplied contribution".
5. `__glCoPickSetMask` turns the path word into the vertex attribute mask (`gc+0x2d91c`): position `0x01` always; colour
   `0x02` when there is no lighting or a host colour; normal `0x04` when lighting is on; light vectors `0x08` for
   positional lights; secondary colour `0x10` (`+0x3d80` bit 29 without lighting, bit 28 with it); texture coordinate
   `0x20` (`+0x3d80` bit 3); fog `0x40` (`+0x3d80` bit 13, RGB mode); `0x80` calls the colour-material helper when colour
   material is enabled with lighting.

**Texture coordinates.** The variable `GLCOSIMFORCECHIPTEXXFORM` is read into `gc+0x2d584` and nothing reads it
afterwards, so the decision between host and engine texture-coordinate generation is not seen; the texture coordinate
the host sends (vertex +0x2c, +0x60) is already generated, and `__glCoPickCalcTextureProcs` stays unread.

**Display lists and arrays.** `__glCoPickVertexProcs@0x6902e4ba` does not choose the engine or host path. In feedback
mode it installs the colour-save routines; otherwise it indexes two tables (by lighting, colour-material and
sphere-map texgen state) to pick which per-vertex attributes (`CoSaveN`, `CoSaveNT`, `CoSaveCNT` and the generic
`__glSaveC` family) a compiled list records, and when `gc+0x3434` is nonzero it installs `CoValidateVertex2/3/4` as the
per-vertex validators for glVertex2, 3 and 4. `__glCoPickOGVertexProcs@0x69030568` installs the run-time-generated
`GenerateVertex2fv/3fv/4fv` as the dispatch-table targets of `glVertex2fv`, `glVertex3fv` and `glVertex4fv` when
`GLCOUSEOG` is not zero (default 1); with it zero the generic functions stay. The generated handlers carry the same
header constants; `GLCOFASTSETATTRIBS` (default 1) turns the generated attribute-set code on.

The white paper lists vertex transform, frustum clip check, normal transform, lighting for up to four lights, face culling
and set-up as engine functions, and the ICD matches: it sends the model-view-projection matrix (`A` = `0x002`), the
normal matrix (`0x012`), the viewport (`0x01b`), material blocks, light blocks and the scene ambient, a cull field in the
primitive mode word, and in display-list replay it sets bits 31 and 30 of the flags word (`A` = `0x001`) and replays
recorded vertex packets (`__glCoDlistExecutePackets`, read in full).

- The three coordinate modes in bits 31:30 of the flags word are, from which vertex words the packet routine copies:
  `01` words 0 to 3 of the vertex record, `10` words 8 to 11, `00` the window x, y, z and the context's w; `11` reads as
  `01`. Which are object, eye and clip coordinates is inferred from the order the driver computes them in.

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

Two ports, `0x18a33060` (`A` = `0x0c5`) and `0x18c33060` (`A` = `0x0c6`), each with count 3 and both register `B` =
`0x33`. `__glCoSetVertexAttribsMask@0x6902d69e` builds them, read in full. Its steps, in the order it writes:

1. For each attribute selected by the mask and enabled in the context, a packet of its own: bit 7 (`0x80`) a call to a
   colour-material helper; bit 3 (`0x08`) the per-light vector packets for lights 0 to 3; bit 4 the secondary colour
   (`0x190b1020`); bit 5 the texture coordinate (`0x198da080`); bit 6 the fog value (`0x192b2020`); bit 1 the colour
   (`0x09280080` or the colour index `0x18e84020`); bit 2 the normal (`0x19a85060`).
2. If bit 0 is set, the vertex itself: a header from the port table plus an **attribute-presence mask in the instance
   field**, then the payload in this order: the colour (four dwords) if instance bit 1, the normal (three dwords) if bit 2,
   a w dword if bit 4 (set when bit 30 of the draw flags is clear), the x and y coordinates, the z coordinate for port B
   only, and the texture coordinate (two dwords) if bit 3. With mask bit 0 set, the colour (RGB mode, when `gc+0x2d620` is nonzero) and the
   normal go into the vertex packet instead of a packet of their own, and so does the texture coordinate when its fourth
   word equals the context constant at `gc+0x250c`; otherwise each has its own packet. A colour index always has its own
   packet. A mask of zero means the
   coordinates only. The header count stays 3 for both ports whatever the mask adds, so it is not the payload length.
3. **The ports differ in the vertex's component count, and so in whether z is sent.** The routine chooses port A when
   bits 19:18 of the vertex record's clip word (offset `0x30`) are `01` and either depth test is off in the pixel-pipe
   mode word (bit 29 clear) or bit 30 of the draw flags is set; otherwise port B. Those two bits are set by the
   immediate-mode entry points: `0x40000` by `glVertex2*`, `0x80000` by `glVertex3*` and `0xc0000` by `glVertex4*`
   (`___glim_Vertex2fv@0x690b391e`, `___glim_Vertex3fv@0x690b398e`, `___glim_Vertex4fv@0x690b3a03`). Port A therefore
   carries only x and y and is used for a two-component vertex when z is not needed (the engine transforms it, or no
   depth test uses it); port B carries x, y and z. The display driver and the miniport's copy use port A only, with
   instance 8 for the texture coordinate (`CoCopyRect` sends four dwords per vertex: x, y, then s and t equal to x and
   y). Established as the driver's rule; what the engine does with the missing z is unknown (inferred: z is 0 and w is 1
   when the engine transforms a two-component vertex).
4. A four-component vertex (clip-word bits 19:18 both set) sent while bit 30 of the draw flags is set is the one case
   where the routine changes the coordinate set: it clears bit 30, sends `0x00210020` with the new flags, sends the vertex
   from the other coordinate words, then sets bit 30 again and resends the flags at the end
   (`sgcoicd.dll@0x6902da62`..`0x6902da7c`, `0x6902dd8e`..`0x6902ddf1`).
5. A call with `last` set marks the end of a primitive; the routine avoids closing a packet node inside a primitive (it
   checks the context's no-break flag before it calls the overflow routine and holds the final dword back).

The fill routines call it with a fixed mask pattern. A smooth-shaded triangle calls the "send everything" entry
(`__glCoSetVertexAttribs`, mask `0xff`) for each of the three vertices. A flat-shaded triangle sends the attribute bits
`0x9e` once for the provoking vertex (colour, normal, lights, secondary colour) and then mask `0x61` (position, texture
coordinate, fog) for each of the three. Read in full for `__glCoFillFlatTriangle` and `__glCoFillSmoothTriangle`.
Clearing a buffer, writing a pixel in the software span stores and, in the display driver, every 2D fill, copy, glyph
and line is drawn with these ports (`CoCopyRect` sends `0x18002020` with type 4 and four `0x18a33068` vertices in strip
order; `vFillSolid` sends the fill colour to `0x18e84020`, `0xa0000000` to the primitive mode and `0x18a33060` vertices).

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
  and for GDI and pixel-transfer packets. The ICD never writes them. What the parameters are is in
  [packet-set.md](packet-set.md#window-parameter-packets): a valid mask, five clip rectangles and the window origin.
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
  tiles)". An entry is a 4 KB page frame number and the base-pointer entry carries the buffer's first slot
  ([packet-set.md](packet-set.md#texture-table-packets-class-11)); which addresses go through the table is not determined.

## Unknown, with the route

- What the engine does with the attribute-presence mask in the vertex header, and with a port A vertex that has no z.
  The drivers' side is read ([packet-set.md](packet-set.md#instance-bits-per-block)); the engine's side has no source.
- What mode-word bits 28, 26, 19 and 12:0 select, and what the flush marker does.
- The lighting rule is read; the texture-coordinate rule (`GLCOSIMFORCECHIPTEXXFORM`, `__glCoPickCalcTextureProcs`) and
  `__glCoPickPhase1Procs` are not.
- The two record fields `pCoMakeTlbDesc` shifts into a base-pointer entry (bits 18:14 and 13, 9).
