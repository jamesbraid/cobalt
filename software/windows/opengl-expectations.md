# What the OpenGL driver expects of the engine

The facts an x64 or other OpenGL path must satisfy to stand in for the Cobalt rendering engine: the identity strings and
extension list the NT 4.0 driver `sgcoicd.dll` 5.2.4 reports, the implementation limits it sets, the switches it reads, and
the queries it makes of the engine and kernel driver. The Windows 2000 5.2.4 build carries the same extension string,
version string and format table (found by byte search); its routines were not read one by one. How the driver is built is
in [nt-opengl-icd.md](nt-opengl-icd.md); the packets are in
[packet-set.md](../../devices/cobalt-graphics/packet-set.md); textures are in
[textures.md](../../devices/cobalt-graphics/textures.md).

Grades are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established**: the constant or branch was read in
disassembly, cited as `sgcoicd.dll@address`. **Inferred**: a reading of names or of a partly read path. **Unknown**:
nothing shipped or published decides it.

## Identity strings

`GetString` returns pointers kept in the context (`___glim_GetString@0x690a1644` reads `gc+0x2528` to `0x2534`).
A context the hardware path accepts sets, in `MakeCurrentRead@0x69019f84` (stores at `0x6901a165` to `0x6901a179`):

| String | Value |
|---|---|
| `GL_VENDOR` | `SGI` (kept from the generic initialisation, `__glSoftResetContext@0x690768c1`) |
| `GL_RENDERER` | `Cobalt` |
| `GL_VERSION` | `1.1.5.24 SGI` |
| `GL_EXTENSIONS` | the list below, one trailing space after the last name |

A context that does not take the hardware path (the other branch of the same routine does not overwrite what `__glSoftResetContext` stored)
reports renderer `Generic/MMX` or `Generic`, version `1.1.3 SGI` and a shorter list: `GL_EXT_abgr GL_EXT_bgra
GL_EXT_compiled_vertex_array GL_EXT_color_matrix GL_EXT_color_table GL_EXT_convolution GL_EXT_histogram
GL_EXT_packed_pixels GL_EXT_paletted_texture GL_EXT_vertex_array GL_WIN_swap_hint`. Established (strings at
`0x69019d20` to `0x69019d38` and `0x690767ca` to `0x690767ee`). The version number is OpenGL 1.1.

## Extensions of the hardware context

The extension string, in order. The right-hand column says what the extension needs of the engine, where the driver shows
it; an entry the driver implements entirely on the host says so.

| Extension | What it needs of the engine | Grade |
|---|---|---|
| `GL_EXT_abgr`, `GL_EXT_bgra` | pixel upload and read formats; the transfer path converts channel order with the colour matrix ([textures.md](../../devices/cobalt-graphics/textures.md#type-codes)) | established |
| `GL_EXT_blend_color` | constant blend colour in the second word of the blend packet; factor codes 8 to 11 | established |
| `GL_EXT_blend_minmax`, `GL_EXT_blend_subtract` | blend equation codes 1 and 2 (minimum, maximum) and 3 and 4 (subtract, reverse subtract) | established |
| `GL_EXT_clip_volume_hint` | `GL_FASTEST` makes the driver leave transform and viewport mapping to the engine ([draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)) | established |
| `GL_EXT_color_table`, `GL_EXT_convolution`, `GL_EXT_histogram` | none found: image-pipeline stages the driver runs on the host (the context declares convolution widths of 11) | inferred |
| `GL_EXT_packed_pixels` | transfer sources of type 2, 3 and 5 | established |
| `GL_EXT_polygon_offset` | the polygon-offset packet and the mode-word bit 28 | established |
| `GL_EXT_secondary_color`, `GL_EXT_separate_specular_color` | secondary colour port (`0x190b1020`) and mode-word bit 17 | established |
| `GL_EXT_vertex_array`, `GL_SGI_compiled_vertex_array` | none: vertex handling on the host; the driver's arrays and locked arrays feed the same vertex ports | inferred |
| `GL_KTX_buffer_region` | region copies of colour or depth (`__glCoDrawBufferRegion@0x69022d22` zeroes the mode word and runs a copy) | inferred |
| `GL_SGI_color_matrix` | the colour matrix packet `0x944c42c0` and mode-word bit 20 | established |
| `GL_SGI_make_current_read` | none from the engine (separate read and draw buffers use the second descriptor set) | inferred |
| `GL_SGI_texture_add_env` | environment function 4 | established |
| `GL_SGI_texture_edge_clamp` | wrap code 2 | established |
| `GL_SGI_texture_lod` | the level-of-detail word | established |
| `GL_SGIX_instruments`, `GL_SGIX_depth_pass_instrument` | the occlusion bits of the depth-test word (28:27): `glStartInstrumentsSGIX` (`0x690434e2`) sends 11 and every later depth-test update sends 01, `glStopInstrumentsSGIX` (`0x6904353c`) sends 00 and then dispatches the packet node with its flag byte at `+0x20` set; how the count comes back is not decoded | established (the bits), unknown (the readback) |
| `GL_SGIX_interlace` | none found | unknown |
| `GL_SGIX_pbuffer` | off-screen buffers: escapes `0x5a` and `0x5b` create and delete one | established |
| `GL_SGIX_subsample`, `GL_SGIX_ycrcb_subsample` | the YCrCb transfer types 14 and 15 (types 9 and 10 are depth transfers) | inferred |
| `GL_WIN_swap_hint` | none from the engine | inferred |

**Not advertised.** No multitexture (`GL_ARB_multitexture` names appear in the dispatch table but not in the list, and
the context has one texture unit), no texture compression, no cube map, no 3-D texture, no `GL_EXT_texture_object` (core
in 1.1), no fog coordinate, no paletted texture (only the generic context lists it), no point parameters. Established
(absence from the string).

## Limits

Set in `__glCoCreateContext@0x69019596` (stores at `0x690195f1` to `0x690196f9`; the vendor's names are those of the
`__GLcontextConstantsRec` declaration in the program database, with `gc+0x2508` as its base). Established.

| Vendor name | Value | GL query it answers |
|---|---:|---|
| `numberOfLights` | 8 | `GL_MAX_LIGHTS` (the engine holds four; the other four are host lighting, [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)) |
| `numberOfClipPlanes` | 6 | `GL_MAX_CLIP_PLANES` (clipping is on the host) |
| `maxViewportWidth`, `maxViewportHeight` | 2048, 2048 | `GL_MAX_VIEWPORT_DIMS` |
| `viewportXAdjust`, `viewportYAdjust` | -1024, -1024 | -1024.0 is added to the x and y offsets of the viewport packet and used in the guard-band range test; the miniport adds 1024 to the window origin ([packet-set.md](../../devices/cobalt-graphics/packet-set.md#window-parameter-packets)) |
| `maxTextureSize` | 4096 | `GL_MAX_TEXTURE_SIZE` |
| `maxMipMapLevel` | 13 | levels 0 to 12 |
| `maxActiveTextures`, `numberOfTextureEnvs` | 1, 1 | one texture unit |
| `subpixelBits` | 6 | `GL_SUBPIXEL_BITS` |
| `maxListNesting` | 64 | `GL_MAX_LIST_NESTING` |
| `pointSizeMinimum`, `pointSizeMaximum`, `pointSizeGranularity` | 0.5, 32.0, 0.125 | `GL_POINT_SIZE_RANGE`, `GL_POINT_SIZE_GRANULARITY` |
| `lineWidthMinimum`, `lineWidthMaximum`, `lineWidthGranularity` | 0.5, 1.0, 0.5 | `GL_LINE_WIDTH_RANGE`, `GL_LINE_WIDTH_GRANULARITY` |
| `maxEvalOrder` | 30 | `GL_MAX_EVAL_ORDER` |
| `maxPixelMapTable` | 65536 | `GL_MAX_PIXEL_MAP_TABLE` |
| `maxAttribStackDepth`, `maxClientAttribStackDepth` | 16, 16 | |
| `maxNameStackDepth` | 128 | |
| `maxModelViewStackDepth` | 32 | |
| `maxProjectionStackDepth`, `maxTextureStackDepth` | 10, 10 | |
| `maxColorStackDepth` | 2 | colour-matrix stack |
| `maxConvolution1DWidth` and the 2-D and separable widths and heights | 11 | |

The line-width range reads 0.5 to 1.0: the granularity store pops the 0.5 the minimum used (`fld`, two `fst`, then `fld1`
and two `fstp`, `0x69019623` to `0x690196ba`). Because the driver clamps `glLineWidth` to this range
(`0x690a2678`), the 12-bit width field of the primitive mode word ([draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#primitive-mode-word))
only ever carries a width up to 1: the plain field holds the integer width, the anti-aliased one twice the width plus
`0x3000`. A model should still accept the whole field. Inferred (the clamp routine was read in part).

The hardware context also has per-context depth and stencil sizes (`hardDepthBits`, `hardStencilBits`, `__GLCOconstantsRec`)
filled from the pixel format; the formats themselves are listed by the display driver (escape `0x5d`, pixel-format
describe, output `0x30` bytes) and were not decoded here.

## What the driver reads and asks

**Environment variables** the OpenGL driver reads (all by name from the process environment; established from
`GetBoardInfo@0x6901ae92` and `__glCoInitTextureManager@0x6901c0ff`):

| Variable | Effect |
|---|---|
| `GLCOSOFTLIGHTING` | lighting always on the host |
| `GLCOUSEOG` | generated vertex code (default on) |
| `GLCOFORCECLIP`, `GLCOCLIPTOBIGFRUSTUM` | guard-band clipping switches |
| `GLCOSIMFORCECHIPXFORM`, `GLCOALLOWCHIPXFORM`, `GLCOSIMFORCECHIPTEXXFORM` | engine transform switches (the texture one is read and never used) |
| `GLCOFASTSETATTRIBS` | generated attribute-set code (default on) |
| `GLCONOSYNCCTX`, `GLCOBUSYWAITFORPACKET` | scheduling: no synchronise on context switch; busy wait for a packet buffer |
| `GLCOSIMPKTCOUNT`, `GLCOSIMPKTSIZE` | number and size of packet buffers; defaults 4 and `0x4000` bytes (size rounded up to 32), asked of the kernel driver in the 0x2c-byte first-context escape (`FirstSetContext@0x6901815b`) |
| `GLCOTEXINFO` | texture debug printing |
| `GLCONOUSERLEVELBINDS`, `GLCOUSERBINDTHRESHOLD`, `GLCOUSERLEVELBINDLEVEL` | texture bind through prebuilt translation-table packets |
| `GLCOTEXLOADSPANMODE`, `GLCONODYNAMICALLOC`, `GLCOTEXFULLUNBIND`, `GLCONOTEXOBJLOCK`, `GLCOTEXLODFRACSIZE` | texture upload and memory behaviour |

**Queries of the engine.** The driver never reads an engine register; the ICD runs in user mode and sees the engine only
through the packet buffers and the kernel driver's escapes ([graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md)).
All of the ICD's kernel traffic is `ExtEscape` (33 call sites, codes `0x40` to `0x64`) sent through `_openglcmd`, which is
`0x1100`, or `0x3f` under an environment variable; it never sends `OPENGL_GETINFO` (`0x1101`). Escapes such as `0x42`, the
pixel-format describe (`0x5d`) and the first-context escape (`0x41`) return values the ICD stores. The driver's wait loops
(`0x6901aa0b`, `0x690614c2`) poll a word at `[node+0xc]`; what updates it is unknown. No escape returns a register value; a
replacement driver has to answer the escapes in [graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md) and
the wait-word update. Established (the call sites read), the wait word unknown.

**What breaks if the engine is replaced.** The driver depends on the engine for correctness in: the colour and depth
formats it sets up (type 3/2/4 for colour, depth 16 or 24), the blend equations and factors, the logic operation, the
stencil operations and the alpha, depth and stencil comparisons ([draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#function-and-operand-encodings)),
texture sampling with the filters and wraps in [textures.md](../../devices/cobalt-graphics/textures.md), the
four-light model, two-sided lighting, the colour matrix on pixel transfers, and the buffer and window mechanisms in
[graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md). A host-side OpenGL implementation can answer the
extension string only if it implements those; it can drop `GL_SGIX_*` and `GL_KTX_buffer_region` without a shipped
program of this driver set noticing, since the pack's own tools were not found to use them (inferred).
