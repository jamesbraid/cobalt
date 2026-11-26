# Cobalt rendering engine: drawing, buffers and textures as the OpenGL driver programs them

What the packets in [packet-set.md](packet-set.md) do together. Source: the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4
and its symbols, read as described there; the miniport `sgcomp.sys` 5.2.4 and display driver `sgcodd.dll` 5.2.4 for the
rows marked so. A statement about what a routine does is **established** where it cites a routine read in disassembly
(`file@address`); everything else is **inferred** from decompiled C, and the engine's own behaviour is **unknown**
unless the SGI white paper ([board-data.md](board-data.md)) names it, and then the white paper is cited only as
agreement.

## Who does what

The driver transforms and clips on the host and also has a path that leaves the transform to the engine. Which one runs
is decided per `glBegin`, and the lighting path is decided per validation. The flags are also read outside `___glcoim_Begin` (`computeDoBits`, `CoExecuteDlist`, `__glCoDlistExecutePackets` read `+0x2d583` and `+0x2d960`), so the decision is not only per `glBegin`: a compiled list is replayed with its own. Everything in this section about the
decision is **established** (the routines named were read in disassembly); what the engine does on each path is
**unknown**.

**Transform: engine or host.** `___glcoim_Begin@0x6904319e` sets the draw flags word (`gc+0x2dbb0` (`xformmodeReg`), sent as
`0x00210020`) before every primitive, from the primitive's mask word (`vertex.primNeeds[type]`, `gc+0x2d920 + 4 * type`):

| Condition | Draw flags bits 31:30 | Meaning |
|---|---|---|
| mask bit `0x400` set: the primitive is drawn by a host fill routine | `00` | the host sends window x, y, z (the vertex record's `window`, `+0x40`) and the context's w |
| else, engine transform not allowed or not requested | `10` | the host sends the four clip coordinates (`clip`, `+0x20`), which it computed; the vendor's names for the bits are `enViewportXform` (31) and `enModelXform` (30), so the engine applies only the viewport transform |
| else, engine transform on | `11` | the vertex packets carry the four object coordinates (`obj`, `+0x00`), and the modelview-projection matrix is resent if it changed (`__glCoPassMVPMatrix`); the vertex-send routine tests bit 30 first, so `11` reads as `01`: the engine applies the model transform, and the viewport transform if bit 31 is also set |

Engine transform is on when (`gc+0x2d583` is nonzero and `gc+0x2d960` is nonzero) or bit 2 of `gc+0x2e494` (`glCoStateFlags1`) is set
(`sgcoicd.dll@0x69043267`..`0x690432b5`). `gc+0x2d583` (`modes.allowChipXForm`) is 1 unless the variable `GLCOALLOWCHIPXFORM` is set to 0.
`gc+0x2d960` (`vertex.noNeedToClip`) has bit 0 from the variable `GLCOSIMFORCECHIPXFORM` (read once, `GetBoardInfo@0x6901ae92`) and bit 1 set
while the hint `0x80F0` (`GL_CLIP_VOLUME_CLIPPING_HINT_EXT`; the context word `+0x1cd0` equal to `GL_FASTEST`) is in force (`___glcoim_Hint@0x69029090`,
`__glCoProcessHints@0x690291c8`). `gc+0x2e494` bit 2 comes from a control-panel registry value named `ShadowTransform`,
read only for 32-bit RGBA contexts. So an application gets the engine transform by setting that hint, which is its
promise that the geometry needs no clipping, or the user turns it on in the control panel; otherwise the driver transforms
and frustum-clips on the host. The mask bit `0x400` is set (with `0x40`, as `mask | 0x440`) in
`__glCoPickTriangleProcs@0x690353ac` when the triangle goes through a host fill routine: anti-aliased polygons
(`__glCoFillAntiAliasTriangle`), a polygon stipple that `__glCoConvertStipple` could not reduce to an 8 by 8 pattern
(`__glCoFillStippledTriangle`) and two-sided processing (`__glCoFillFrontBackTriangle`); `__glCoPickPointProcs` sets
`0xc40` or `0x440` for points the same way. In feedback and selection mode every mask is `0x40`.

**Display lists take the engine transform after a bounding-box test.** Established (`CoExecuteDlist@0x69033758` and `__glCoDlistExecutePackets@0x6906227a` read in full;
the readings marked inferred are about what the test's inputs are). `__glCoDlistExecutePackets` sends draw flags `0xc0000000` and replays the list's recorded
object-space packets only when `noNeedToClip` (`gc+0x2d960`, any bit) and `gc+0x2d583` are nonzero, the list has recorded packets and a run of state checks pass; otherwise it
returns 0 and `CoExecuteDlist` interprets the list on the host (`__glDoInterpret`), which clips. For a list whose flags bit 0 is set (inferred: the list has a bounding box, the eight `corner` entries of its
info record) and that is the outermost one being executed (`gc+0x2ae0` equal to 1, inferred) or whose bit 3 is already set, `CoExecuteDlist` runs eight rounds of a transform followed by
`__glCoIsOutsideBigFrustum@0x6902eb3e`, ORs the results and sets bit 3 of `noNeedToClip` only if none is outside. That routine accepts a clip position whose x, y and z lie strictly inside plus and minus a
viewport factor (`gc+0x2d950`, `+0x2d954`, `+0x2d958`) times its w and whose w is below 2^24 (the overflow the release note names). The bit is cleared when the outermost list ends.
If the corners are those of the box around the list's vertices (inferred), then because the clip coordinates are linear in the object coordinates every vertex of a list that passes has w above zero
and lies in the same volume, so the engine is never given geometry that the host would have had to clip. A nonzero `noNeedToClip` from the application hint or `GLCOSIMFORCECHIPXFORM` replays every
list through the engine without the test.

**Clip.** The host clips. The guard-band factors are computed from the viewport in `UpdateViewport@0x690198ee` (the
variable `GLCOFORCECLIP` turns the factors off, `GLCOCLIPTOBIGFRUSTUM` copies them to a global table). Whether the engine
clips when it transforms is not read; the hint above is the application's promise that nothing needs clipping.

**Lighting: engine or host** (`__glCoValidateLighting@0x690269f6`, `__glCoValidateSoftLighting@0x6902702a`,
`LoadLightPath@0x690267b4`, `__glCoPickSetMask@0x6902ddf8`, `GetBoardInfo`):

1. Soft lighting. `gc+0x2d57e` (`modes.softLighting`) is set at start-up when the context is colour-index (`gc+0x2459` (`gc.modes.colorIndexMode`)) or the variable
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
     `___glcoim_Lightfv` for each of the three, sets a bit in the light record's `outOfRange` word, `+0x108`);
   - it is positional, its constant attenuation is below 0.5 and either the variable `GLCOLIGHTINGWAR` is nonzero or its
     linear and quadratic attenuation are both zero (the light record's `constantAttenuation`, `linearAttenuation` and
     `quadraticAttenuation`, `+0xf0`, `+0xf4` and `+0xf8` of the vendor's `__GLlightSourceMachineRec`, are the stores
     `___glcoim_Lightfv` makes for `GL_CONSTANT_ATTENUATION`, `GL_LINEAR_ATTENUATION` and `GL_QUADRATIC_ATTENUATION`; the
     structure is the one the program database declares, so this is no longer an inferred base).
3. A material or scene colour outside 0 to 1 also puts the colour on the host. `light.cOutOfRange` (`gc+0x2d5a8`) holds one flag per
   colour parameter: `ApplyParameterF2` sets bit `n` for the front side's ambient (0), diffuse (1), specular (2) and emission (3), shifted
   up by 4 for the back side, through `IsColorOutOfRange` (which tests the red, green and blue components against 0 and 1), and
   `___glcoim_LightModelfv` and `___glcoim_LightModeliv` set `0x100` for the light-model ambient. When any bit of `0x199` (front and back ambient, front and back
   emission, light-model ambient) is set and lighting is on, `__glCoEnableLighting` and `__glCoValidateLighting` call `LoadLightPath`
   with the host flag, which clears bit 23 of the lighting mode word, and `light.calcColor` (`gc+0x2d608`) becomes `__glCoCalcRGBColor`
   (the host computes the lit colour per vertex). An earlier version called `gc+0x2d5a8` the tracked-material mask and tied the rule to
   colour-material tracking; the vendor's name and the two setters say otherwise.
4. With the engine path, the per-vertex helper in `gc+0x2d608` is picked from a table by a small index: 2 for a host
   flag, 1 for local lights, 4 for `+0x3d80` bit 28 (index 1 `__glCoCalcLightVectors`, 2 `__glCoCalcExtraColor`, 4
   `__glCoCalcSpecColor`, every combination `__glCoCalcAllCases`, 0 a no-op; `sgcoicd.dll@0x690f390c`). These produce the extra per-vertex packets the engine needs:
   the light vector and scalar packets for positional lights (attribute-mask bit `0x08`), and a host colour contribution
   (`0x02`) which matches the white paper's "accumulating a host-supplied contribution".
5. `__glCoPickSetMask` turns the path word into the vertex attribute mask (`gc+0x2d91c` (`vertex.attribSetMask`)): position `0x01` always; colour
   `0x02` when there is no lighting or a host colour; normal `0x04` when lighting is on; light vectors `0x08` for
   positional lights; secondary colour `0x10` (`+0x3d80` bit 29 without lighting, bit 28 with it); texture coordinate
   `0x20` (`+0x3d80` bit 3); fog `0x40` (`+0x3d80` bit 13, RGB mode); `0x80` calls the colour-material helper when colour
   material is enabled with lighting.

**Texture coordinates: always the host's. Established** (`__glCoPickCalcTextureProcs@0x6901f31a` and
`__glCoPickPhase1Procs@0x6903572a` read in full; every reference to the flag's offset listed). `GLCOSIMFORCECHIPTEXXFORM`
lands in `modes.forceChipTexXForm` (`gc+0x2d584`) and `GetBoardInfo` is the only routine that touches that byte: there
is no branch on it, so there is no engine rule to find. `__glCoPickCalcTextureProcs` chooses a host routine for the
context's texture-calc slot (`gc+0x3100` (`gc.procs.calcTexture`)) from the texture-generation enables (`gc+0x1c54` (`gc.state.enables.texture`), bits 3 to 6) and the S and T
generation modes (`gc+0x1d0c` (`gc.state.texture.set[0].s.mode`), `gc+0x1d30` (`gc.state.texture.set[0].t.mode`)): equal eye-linear modes pick `__glCalcEyeLinear` (state flag 1, and flag
`0x10` when the six plane components it tests are all zero), object-linear `__glCalcObjectLinear` (flag 2), sphere map
`__glCalcSphereMap` (flag 4 or 8 by bit 2 of `gc+0x2a24` (`gc.constants.cpuFlags`), after `__glValidateTexGenModelView`); any other mix with
generation on picks `__glCalcMixedTexture` (flag 8); none picks `__glCalcTexture`, or a no-op when the
texture matrix's type (`gc.transform.texture[0]`, `matrix.matrixType`) is 4; an earlier reading called that the bound texture's kind field.
Flag `0x20` is added when the type is not 4 and the texture matrix flag
`vertex.enableTexMatrix` (`gc+0x2d96e`) is clear. The flags go to `texture.controlWord` (`gc+0x2da78`), which the four
code generators (`GenerateValidate`, `GenerateCalcTexture` and their Pentium III versions) read to emit x86 code for
the vertex path. The texture coordinate the engine gets is the packet `0x198da080` the host fills, and the engine's own
transform mode has no texture bit that anything sets (`enTexXform` stays clear).

`__glCoPickPhase1Procs` makes no engine-or-host choice about transform or texture coordinates; it neither reads
`gc+0x2d583` nor `gc+0x2d960`. It is the first stage of state validation: it folds the dirty-state words, installs the
vertex procedures (`__glCoPickOGVertexProcs`, `__glCoPickVertexArraysProc`, `__glCoPickSetMask`), and builds the
attribute-need words. From the enables it sets the polygon-offset bits of `gc+0x3d80` (`gc.polygon.shader.modeFlags`) (the three enables, each only when the
offset factor or units is non-zero), the lighting and fog needs, and in `gc+0x3430` (`gc.vertex.needs`), `+0x3434` and `+0x3438` (the two-dword `gc.vertex.faceNeeds`) the coordinate
sets the host must produce (eye coordinates `8` for eye-linear or sphere-map generation, local lights, a local viewer or
clip planes; the normal `4` for lighting and sphere map; `0x80` for texturing), which it copies to the vertex needs
`gc+0x2d918` (`vertex.needs`), `+0x2d948` and `+0x2d94c` (`vertex.faceNeeds`), from the soft-lighting variants when `modes.softLighting` is set. Established.

**Display lists and arrays.** `__glCoPickVertexProcs@0x6902e4ba` does not choose the engine or host path. In feedback
mode it installs the colour-save routines; otherwise it indexes two tables (by lighting, colour-material and
sphere-map texgen state) to pick which per-vertex attributes (`CoSaveN`, `CoSaveNT`, `CoSaveCNT` and the generic
`__glSaveC` family) a compiled list records, and when `gc+0x3434` (`gc.vertex.faceNeeds`) is nonzero it installs `CoValidateVertex2/3/4` as the
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

Quads are sent as fans of four (type 5), quad strips and triangle strips as strips (type 4), polygons as fans
(established as what the driver sends). What the engine does with the types is unknown; the white paper's "connected
line and triangle mesh interface" is agreement, not the meaning. The driver calls the same vertex-send routine for every primitive; only the mode word and
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
   only, and the texture coordinate (two dwords) if bit 3. With mask bit 0 set, the colour (RGB mode, when `gc+0x2d620` (`vertex.colorPerVertex`) is nonzero) and the
   normal go into the vertex packet instead of a packet of their own, and so does the texture coordinate when its fourth
   word equals 1.0 (`gc.constants.one`, `gc+0x250c`); otherwise each has its own packet. A colour index always has its own
   packet. A mask of zero means the
   coordinates only. The header count stays 3 for both ports whatever the mask adds, so it is not the payload length.
3. **The ports differ in the vertex's component count, and so in whether z is sent.** The routine chooses port A when
   bits 19:18 of the vertex record's `flags` word (offset `0x30`, the clip-code word) are `01` and either depth test is off in the pixel-pipe
   mode word (bit 29 clear) or bit 30 of the draw flags is set; otherwise port B. Those two bits are set by the
   immediate-mode entry points: `0x40000` by `glVertex2*`, `0x80000` by `glVertex3*` and `0xc0000` by `glVertex4*`
   (`___glim_Vertex2fv@0x690b391e`, `___glim_Vertex3fv@0x690b398e`, `___glim_Vertex4fv@0x690b3a03`). Port A therefore
   carries only x and y and is used for a two-component vertex when z is not needed (the engine transforms it, or no
   depth test uses it); port B carries x, y and z. The display driver and the miniport's copy use port A only, with
   instance 8 for the texture coordinate (`CoCopyRect` sends four dwords per vertex: x, y, then s and t equal to x and
   y). Established as the driver's rule; what the engine does with the missing z, and with w, is unknown.
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
per colour component (`gc+0x2460` (`gc.modes.redBits`)): 4 gives `0b001101`, 5 gives `0b001001`, 8 gives `0b010010`. What the driver does with the
field is established: `__glCoClearBuffers` reads `(descriptor >> 25) & 3` and branches on 0 (byte colour index), 1
(16-bit) and 2 (32-bit, per-byte write masks), which is why this page calls bits 26:25 a size class (1 for 4-4-4-4 and
5-5-5-1, 2 for 8-8-8-8) and bits 30:27 the layout. What the engine reads from the field is unknown; the white paper's 8-,
16- and 32-bit colour formats are agreement.

**Depth and stencil descriptor** (`0x930ab020`, context word `+0x2e310`). Disassembly-checked: bits 28:23 are the low
six bits of the first dword of the buffer record (the record the context keeps for the current depth buffer, chosen
by whether the draw buffer is the left or right one). `__glCoInitDepth` sets bits 31:29 to `001` for a 16-bit depth
buffer and to `110` for 24 bits, and `__glCoClearBuffers` reads `(descriptor >> 29) & 3` as 1 or 2 accordingly. The
white paper says 16-bit floating point and 24-bit fixed point depth, with 8-bit stencil; the driver's sizes (16 and 24)
match it, but what the engine reads from the field is unknown. Stencil is in the same 32-bit word as 24-bit depth (depth in the
low three bytes, stencil in the top byte), and the 16-bit depth word is a floating-point code that the driver decodes with a
table: both are established from the clear, transfer and read-back routines in
[opengl-depth-stencil-fog.md](../../software/windows/opengl-depth-stencil-fog.md).

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

## Function and operand encodings

Every word below is the payload the named setter writes, read whole in disassembly (`sgcoicd.dll` 5.2.4): **established**
for what is written. What the hardware does with each code is **inferred**: the codes sit in the fields the vendor's types
declare and the GL meaning is the driver's. Bit positions are in [packet-set.md](packet-set.md#the-vendors-register-word-types).

**Comparison functions** (alpha test `__glCoPassAlphaFunc@0x6902c696`, depth test `__glCoPassDepthTest@0x6902ce58`,
stencil `__glCoPassStencil@0x69023ece`): the GL enum minus `0x200`, so 0 never, 1 less, 2 equal, 3 less or equal,
4 greater, 5 not equal, 6 greater or equal, 7 always. The alpha test sets bit 31 of its first word as an always-set
enable and holds the function in 27:25 and the reference in the second word's low byte (the float reference times 255,
plus 0.5, truncated); the depth function is in 31:29 of its word; the stencil function is in 19:16.

**Blend** (`__glCoPassBlend@0x6902c780`; first word destination 31:28, source 27:24, equation 23:20):

| Equation | Code |
|---|---:|
| `GL_FUNC_ADD_EXT` (`0x8006`) | 0 |
| `GL_MIN_EXT` (`0x8007`) | 1 |
| `GL_MAX_EXT` (`0x8008`) | 2 |
| `GL_FUNC_SUBTRACT_EXT` (`0x800a`) | 3 |
| `GL_FUNC_REVERSE_SUBTRACT_EXT` (`0x800b`) | 4 |

`GL_LOGIC_OP` as the equation (`0xbf1`) sends no blend packet; the logic-operation path is used. An equation outside this
list leaves the field as the routine's uninitialised local, so the value is not defined; the same holds for a factor outside the lists below.

| Factor | Source code | Destination code |
|---|---:|---:|
| `GL_ZERO` | 0 | 0 |
| `GL_ONE` | 1 | 1 |
| `GL_DST_COLOR` | 2 | not allowed |
| `GL_SRC_COLOR` | not allowed | 2 |
| `GL_ONE_MINUS_DST_COLOR` | 3 | not allowed |
| `GL_ONE_MINUS_SRC_COLOR` | not allowed | 3 |
| `GL_SRC_ALPHA` | 4 | 4 |
| `GL_ONE_MINUS_SRC_ALPHA` | 5 | 5 |
| `GL_DST_ALPHA` | 6 if the buffer has alpha, else 1 | 6 if the buffer has alpha, else 1 |
| `GL_ONE_MINUS_DST_ALPHA` | 7 if the buffer has alpha, else 0 | 7 if the buffer has alpha, else 0 |
| `GL_SRC_ALPHA_SATURATE` | 12 if the buffer has alpha, else 0 | not allowed |
| `GL_CONSTANT_COLOR_EXT` (`0x8001`) | 8 | 8 |
| `GL_ONE_MINUS_CONSTANT_COLOR_EXT` | 9 | 9 |
| `GL_CONSTANT_ALPHA_EXT` | 10 | 10 |
| `GL_ONE_MINUS_CONSTANT_ALPHA_EXT` | 11 | 11 |

So the codes 2 and 3 name "the other operand's colour": the destination colour in the source field and the source colour
in the destination field. "The buffer has alpha" is the context's alpha bit count above zero. The second word is the
constant colour packed red in 31:24, green 23:16, blue 15:8, alpha 7:0, each the float times 255 plus 0.5, truncated. The
blend is sent when the pixel-pipe mode word's blend bit is set; an RGBA context only (a colour-index context sends none).

**Logic operation** (`__glCoPassLogicOp@0x6902cafa`): the GL enum minus `0x1500` in 31:28 (clear 0, and 1, and-reverse 2,
copy 3, and-inverted 4, noop 5, xor 6, or 7, nor 8, equiv 9, invert 10, or-reverse 11, copy-inverted 12, or-inverted 13,
nand 14, set 15); an enum outside the sixteen sends 0. The ROP2 table the display driver uses is in
[2d-blit-video.md](2d-blit-video.md).

**Stencil operations** (31:28 pass, 27:24 depth fail, 23:20 stencil fail): 0 `GL_KEEP`, 1 `GL_ZERO`, 2 `GL_REPLACE`,
3 `GL_INCR`, 4 `GL_DECR`, 5 `GL_INVERT`; an unknown enum gives 0. The first word then has the function in 19:16, the
read mask in 15:8 and the reference in 7:0; the second word's low byte is the write mask.

**Depth test word** (`0x936ae020`): the function in 31:29; bits 28:27 are the occlusion-count control the instruments
extension drives, 11 to start (initialise and enable), 01 while counting, 00 otherwise; the context field that holds the
state is `gc+0x2e1c4`. The remaining bits are zero. The depth-write enable and the depth-test enable are in the pixel-pipe
mode word (bits 30 and 29), not here.

**Fog colour** (`__glCoPassFog@0x6902cc3e`): the three colour floats times 255 plus 0.5, red in 31:24, green 23:16, blue
15:8; not sent in a colour-index context.

**Cull and face orientation** (`__glCoPassCullFace@0x69024320`, `__glCoPassFrontFace@0x69024378`): when face culling
is enabled the primitive mode word's bits 14:13 get 01 for `GL_FRONT`, 10 for `GL_BACK` and 11 for
`GL_FRONT_AND_BACK` (so bit 13 culls front faces and bit 14 culls back faces); with culling off both are cleared; an unknown cull or front-face enum leaves the bits as they were, and `__glCoFillStippledTriangle@0x69025634` rewrites the cull bits (`0x69025947`) while it draws. Bit 12
is set for `GL_CCW` (`0x901`) and cleared for `GL_CW` (`0x900`).

**Polygon offset** (`__glCoPassPolygonOffset@0x69024398`): word 1 is the GL factor as a float, word 2 is the units times
the depth buffer's minimum resolution (`gc+0x4244`).

## Viewport and coordinate conventions

What the driver sends for the viewport (`__glCoSetViewMatrix@0x69018dfa`, `UpdateViewport@0x690198ee`, both read in full)
fixes the coordinate space it expects. Established as what is written; the engine's reading is inferred.

- The six words of `0x0362a0c0` are x scale, y scale, z scale, x offset, y offset, z offset. The x scale is half the
  viewport width, the y scale is the negative of half the height (the engine's y runs down the screen: the driver flips it
  against the window height), the z scale and offset come from `glDepthRange` as
  `(far - near) * 0.49999997` (`0x3effffff`, so a window z never reaches 1.0 exactly) and `(far + near) * 0.49999997`
  (`__glCoUpdateDepthRange@0x6902cee2`, called from `___glcoim_DepthRange@0x6902cf7e`). When a depth-reversal byte in the
  context is set, each value is first mirrored (`max - x`, with `max` the context's `one` constant at `gc+0x250c`), not
  swapped, so the packet carries `(near - far) * k` and `(2 * max - near - far) * k`. A second writer of the same state exists:
  `__glUpdateDepthRange@0x690823fc` calls `__glSetDepthRange`, reached through `__glValidateZCount` from
  `__glGenericPickDepthProcs` (procedure slot `gc+0x3060`); whether it runs on the Cobalt path was not traced.
- The x offset is the viewport's left edge plus half its width, plus the float x adjustment, less `0.492`; the y offset is
  the window height less (the viewport's bottom edge plus half its height), plus the float y adjustment, less `0.508`.
  The adjustments are -1024.0 each: `__glCoCreateContext` stores `0xfffffc00` to `gc+0x2550` and `gc+0x2554`
  (`0x69019604`, `0x6901960a`), `__glEarlyInitContext` converts them to floats at `gc+0x2558` and `gc+0x255c`
  (`0x6907651f` to `0x69076533`), and `UpdateViewport` adds them (`0x69019920`, `0x69019956`). So the x offset is
  `left + halfWidth - 1024 - 0.492` and the y offset is `H - (bottom + halfHeight) - 1024 - 0.508`, which matches the
  miniport adding 1024 to the window origin it sends in `0x9845b020` ([packet-set.md](packet-set.md#window-parameter-packets));
  how the engine combines the two is unknown. The constants are at `0x69002680` (0.492) and `0x6900267c` (0.508): the
  half-pixel of OpenGL's pixel centres, offset by 0.008 (about 1/125 pixel) in opposite directions. The driver's
  `subpixelBits` is 6, so the engine is expected to take window coordinates with 1/64 pixel resolution; that the offset
  exists to settle ties in the engine's fill rule is inferred, and the fill rule itself is unknown.
- The integer adjustments also enter a range test (`0x69019a6c`) that decides whether the viewport lies inside the
  guard-band rectangle the host clipper assumes.
- Clip volume: guard-band factors come from the viewport and the viewport maximum of 2048 by 2048
  ([opengl-expectations.md](../../software/windows/opengl-expectations.md#limits)).
- The rasterisation rules proper (coverage, top-left or other fill convention, pixel centre, rounding of the fixed-point
  position, interpolation precision, the perspective correction of texture coordinates) are **unknown**: no shipped
  routine depends on them, and the host path that draws a fill itself (`__glCoFillAntiAliasTriangle`, the stipple and
  two-sided fills) sends window coordinates to the same vertex port the engine rasterises from.
  [questions/open-questions.md](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle)
  records what a model must choose.

## Clears

Two ways. A clear that needs the depth or stencil operations is drawn: `__glCoClearBuffers` sets the depth function
to "always" (`0x936ae020`, payload `0xe0000000`), the stencil registers (`0x938af040`, payload `0x2227ff00` OR a byte)
and the colour write mask, then sends the rectangle as vertex packets. A plain colour or depth clear uses the
hardware rectangle clear, `0x99ce00a0` (and `0x9e7200a0`, a second register bank, when `fastClear` is set), five words:

1. the buffer's descriptor word (`tlbDesc` of the clear request), so the clear names the buffer it writes;
2. a byte-enable mask for the 32-byte row;
3. the value to write (a colour in the layout of the buffer, or the depth);
4. the minimum corner as `y << 16 | tile column`;
5. the maximum corner, same form.

That is the order both builders write and the vendor's `CoClearModeReg` declares (descriptor, mask, value, start, end); an earlier
reading of this list had the value first. The second bank (`0x9e7200a0`) is chosen by the context's `fastClear` byte
(`gc+0x2e2cc` (`fastClear`)), not by a second buffer.

The rectangle is in units of 32-byte memory rows: 32 pixels per column at 8 bits, 16 at 16 bits and 8 at 32 bits
(`coClearBuffers`, the part that builds the packets, and the miniport's `CoClearRect`, read in full). A rectangle that does not start or end on a
column boundary is split into a left edge, a middle and a right edge, and the edge packets carry a partial mask:
`-1 << (pixels * bits)` on the left, `-1 >> ...` on the right, where a pixel is 1, 2 or 4 mask bits. The driver's unit and mask
width are established from the two routines: 32-byte rows, and the mask carries one bit per byte of the row. What the
engine does with the mask and the unit is unknown; the white paper's 288-bit main memory bus and its "clear operations at
full memory bandwidth" are agreement, not the meaning.

## Pixel transfers and textures

- **Pixel transfers** (`glDrawPixels`, `glReadPixels`, `glCopyPixels`, and bitmaps) are done with the same packets: a
  primitive-mode word, vertex packets for the destination rectangle, the transfer parameters (`0x93cc0080`,
  `0x91ea2040`), the colour matrix (`0x944c42c0`) and the 4 by 4 scale-and-bias the white paper calls "4x4 color
  matrix for color space conversion, scale/bias, and component swizzling". The four context fields packed into the two
  parameter packets are the source type, depth, linear flag and descriptor id; the packet sequence of each routine, the
  colour-matrix words and the host formats' types are in
  [opengl-pixel-transfers.md](../../software/windows/opengl-pixel-transfers.md). The routines are `__glCoLinearToTileXfer`,
  `__glCoTileToLinearXfer`, `__glCoTileToTileXfer`, `coProcess*` and `coLoadLinearTlb`.
- **Texture images** reach texture memory by one of three routes, set out in [textures.md](textures.md#how-texels-get-into-texture-memory):
  by default through the engine (the vendor's `dmaXferTextures` is 1: a pixel-transfer packet sequence draws a linear host
  image into the tiled level, and the context's colour matrix converts channel order and depth), otherwise by converting into
  a host copy of the level first and then the same transfer (no routine of the driver stores into the tile pages by CPU,
  [opengl-texture-levels.md](../../software/windows/opengl-texture-levels.md#question-5-texel-order-inside-a-tile-and-tile-order-in-a-level)),
  and, only when a global that
  defaults to 0 is set (`vhdlLoadTextures`), through the packet `0x9ed380e0` (`coVhdlTexImage`), whose name says VHDL: a
  simulation hook with three caller words, a pixel count, the width and the corner pair. Read in full. An earlier version of
  this page said the images are stored by the CPU and the engine packet is a hook only; the first half was wrong (the
  transfer route is the default). The white paper's "memory management hardware for allocation of graphics buffers in
  system memory" and the miniport's translation table are the allocation side.
- **Texture level lifecycle.** `__glCoCreateTexLevel` computes the level's size (`coComputeTexLevelSize`), asks for
  pages, and, if the texture is currently bound, writes `0x97400020` with payload 0 before freeing the old pages.
- **Binding.** `coCreateTexBindPkt` asks the miniport (escape `0x57`, `CREATE_TEXTLBPKT`) for a prebuilt packet that
  loads the translation-table entries for the texture's pages (class `11` headers, built by `CoBuildTlbPacket`, which
  starts the body with a flush marker and ends it with a chain packet). The ICD replaces the marker at the start of
  the body with the chain header `0x97000040`; `coDispatchTexBindPkt` then ends the current node with a chain packet
  and stores the position words of the texture packet, so the texture packet runs between two ICD nodes. A bind is
  therefore made of chain packets, not of a register write from the ICD. The order of the jump into the packet and the
  return is not decoded. Texture addressing, filtering and wrap modes are not packets of their own in anything
  read: the texture parameters go in the two packets `coSetTexParam` writes, `0x91ea2040` and `0x93cc0080`, whose fields are all
  decoded in [textures.md](textures.md). The vendor's
  types for them are the texture environment and the texture-generator parameters (format, filters, wraps, map size,
  level of detail and border colour: [packet-set.md](packet-set.md#the-vendors-register-word-types)); this page calls
  them "transfer parameters" because the pixel-transfer routines load the same two packets.
- **Texture memory and unified memory addressing.** Buffers and textures sit in 4 KB-multiple pages named through the
  engine's translation table, 2048 entries of 8 bytes at `0x8000` ([graphics-engine.md](graphics-engine.md)), with
  the 64-entry buffer descriptor table at `0xc000`. The white paper says the pages are "organized linearly (without
  tiles)". An entry is a 4 KB page frame number and the descriptor carries the buffer's first slot and tile geometry
  ([packet-set.md](packet-set.md#texture-table-packets-class-11)). The packet pool is descriptor 0, so packet fetch goes
  through the table too (`CoInitHw` loads the pool's pages into it); that pixel and texture accesses go through it is the
  same reading, inferred.

## Unknown, with the route

- What the engine does with the attribute-presence mask in the vertex header, and with a port A vertex that has no z.
  The drivers' side is read ([packet-set.md](packet-set.md#instance-bits-per-block)); the engine's side has no source.
  The per-mode table of which words a port A and a port B vertex carry, the viewport and origin arithmetic and the
  lighting words are in [opengl-triangle-path.md](../../software/windows/opengl-triangle-path.md).
- What the flush marker does, and what the engine does with every field the vendor's types name but no routine sets
  (mode-word bits 12:0 are declared reserved).

## What must be exact and what may be approximated

For a model of the engine that runs the shipped OpenGL driver and display driver. Grades are the repository's three.

| Item | Class | Why | Grade |
|---|---|---|---|
| Header decoding: class, `A`, `B`, count, instance; vertex port word order and the attribute-presence mask | exact | every packet the drivers write is parsed by these | established (writers), inferred (engine) |
| Descriptor and translation-table words, tile geometry of levels and buffers, class-11 table loads | exact | the drivers allocate memory by them and the kernel driver reads the packet pool through them | established, inferred |
| Format words: type, depth and id of colour, depth and texture buffers; the colour byte order (red in the top byte at 32 bits) | exact | the display driver writes the screen through them | established, inferred |
| Link words, chain packets, serial numbers, stop interrupts, the context-switch handshake | exact | [graphics-engine.md](graphics-engine.md) | established, inferred |
| Comparison, blend, logic-operation, stencil and cull codes and the occlusion control bits | exact | GL state the drivers rely on; the codes are in the tables above | established, inferred |
| Texture filter, wrap, environment and level-of-detail codes, GL-format-to-type mapping, linear-source uploads | exact | [textures.md](textures.md) | established, inferred |
| Rectangle clear and copy with the valid mask, the stipple and styled-line repeat words | exact | the 2D paths of the display driver | see [2d-blit-video.md](2d-blit-video.md) |
| Coverage and fill convention, 1/64 pixel rounding, attribute interpolation precision | approximable | no shipped routine reads a pixel back to depend on them; an exact-pixel guest check would | unknown |
| Filter arithmetic, level selection, blend rounding, dither, perspective correction | approximable | same | unknown |
| Texel order inside a tile, channel order of type 5 | approximable while the model reads back what it wrote | the driver never reads a level back through the engine | unknown |
| The 16-bit depth representation and precision, polygon-offset arithmetic | approximable | not shown | unknown |
| Throughput, packet fetch timing | approximable | only the stop and idle handshakes are observed | inferred |

