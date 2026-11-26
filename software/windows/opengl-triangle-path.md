# What the OpenGL driver sends for unlit and lit triangles

The packet words the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 writes to draw a triangle, and what each choice implies
for the engine: the vertex that has no depth, the viewport and window-origin arithmetic with clipping, and the
lighting words. It extends [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md) and
[packet-set.md](../../devices/cobalt-graphics/packet-set.md) and repeats nothing they already hold; read those first.
How the driver is built is in [nt-opengl-icd.md](nt-opengl-icd.md).

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established**: the whole routine was read in
disassembly (the decompile of the typed project was used to navigate, never as the evidence), cited `sgcoicd.dll@address`.
**Inferred**: a reasoned reading, with the reasoning. **Unknown**: nothing shipped or published decides it. Nothing in the
driver reads a pixel or a register back from the 3D path, so every statement about what the *engine* does is inferred at
best; the driver fixes only what it writes, and what it relies on. Where a value below is for an emulator author to
choose, it is labelled a choice and is not a finding.

Routines read for this page, whole unless marked: `__glCoSetVertexAttribsMask@0x6902d69e` (again, for the fields below),
`___glcoim_Begin@0x6904319e`, `__glCoClearBuffers@0x69046827`, `coSetRectangle@0x690473fa`,
`__glCoSetViewMatrix@0x69018dfa`, `UpdateViewport@0x690198ee`, `__glCoPassMVPMatrix@0x69023d8a`,
`__glCoPassNormalMatrix@0x69023cbc`, `LoadLightSource@0x69026d06`, `LoadLightPath@0x690267b4`,
`ComputeLightState@0x69085b5a`, `__glCoCalcLightVectors@0x6902763a`, `__glCoCalcExtraColor@0x6902707c` (the
specular, spot and clamp parts), `__glCoCalcSpecColor@0x69027860` (decompile only), `__glCoCheckIfSpecularIsZero@0x6902953a`, `__glCoUpdateLightingState@0x690266ce`,
`__glCoEnableColorSum@0x690266ac`, and only the colour-material cases of the list converter at `@0x6906185e`..`@0x69061986`
and of the code generator at `@0x69049067`..`@0x69049099`. The record layouts (`__GLlightSourceMachineRec`,
`__GLvertexRec`, `__GLtransformRec`) are the program database's.

## The vertex that has no depth: port A

**What the driver sends.** Port A (`0x18a33060`) is chosen over port B (`0x18c33060`) when the vertex record's
component-count bits (bits 19:18 of its flags word, set by `glVertex2*`, `3*`, `4*`) read two components **and** either
the depth test is off (pixel-pipe bit 29 clear) or bit 30 of the draw flags is set
(`__glCoSetVertexAttribsMask`, established). The vertex then carries, in order: the colour
(instance bit 1), the normal (bit 2), **one w dword only when bit 30 of the draw flags is clear (instance bit 4)**, then
x, y, then z for port B only, then the texture coordinate (bit 3). Which words fill x, y, z, w follows the draw flags
(the same routine):

| Draw flags 31:30 | x, y, z come from | w dword | Port A | Established |
|---|---|---|---|---|
| `00` | window `x`, `y`, `z` (`+0x40`) | present, always the context constant 1.0 (`gc+0x250c`) | x, y, w; no z | yes |
| `10` | clip `x`, `y`, `z` (`+0x20`) | present, the clip `w` (`+0x2c`) | x, y, w; no z | yes |
| `11` | object `x`, `y`, `z` (`+0x00`) | absent | x, y; no z, no w | yes |
| `01` | object coordinates, as `11` | absent | not written by any routine | yes (never written) |

The only values the draw flags word is ever given are `00`, `10` and `11` (`___glcoim_Begin@0x69043249`..`@0x690432c0`:
host fill routine gives `00`; otherwise bit 31 is set, bit 29 is always cleared, and bit 30 is set when the engine
transform is allowed). A vertex with four components under `11` is the one exception: the routine clears bit 30, resends
the flags (now `10`), sends the clip coordinates through port B with a w dword, and sets bit 30 again afterwards
(`@0x6902da62`..`@0x6902dd8e`). So an object vertex under `11` never carries a w the engine could use. Established.

Three more writers of vertices bear on the question:

- The drawn depth and stencil clear forces the flags word to `00`, then sends its two triangles as six port B vertices
  of exactly three payload dwords each: x, y and the depth value as z, **with no w dword even though the flags are
  `00`** (`__glCoClearBuffers@0x69046827`, `coSetRectangle@0x690473fa`, whole routines). Established.
- The display driver and the miniport's copy send port A vertices of two dwords (x, y) or four (x, y, s, t), instance
  mask 0 or 8, never a w dword, and never write the draw flags, the matrix or the viewport
  ([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md)). A byte search of `sgcodd.dll` and `sgcomp.sys`
  for the draw-flags header `0x00210020` and the viewport header `0x0362a0c0` finds none. Established (search), so
  the 2D drivers rely on the context's default draw flags.
- The miniport's default context image carries the header `0x00210440` (the block that holds the draw flags, the matrix,
  the normal matrix, the viewport and the bias: 34 payload dwords) at dword 167, and its payload is not in the table
  (`sgcomp.sys`, `initialCxShadow` at file offset `0xddd8`, a list of header and position pairs; the image page is zeroed
  first, per [graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md#per-context-shadow)). So a new
  context starts with draw flags `00`. Inferred (the zero fill is the existing reading of `CoInitShadowRam`, not
  re-read here).

**What it implies.**

1. *Port A under `11`, z is the vertex's own z of 0 and w is 1.* The driver sends only x and y of an object vertex, and
   the engine multiplies by the model-view-projection matrix. A result that matches the host path (which the driver also
   runs for the same primitives) needs z = 0 and w = 1, the OpenGL meaning of `glVertex2`. Port B under `11` likewise
   needs w = 1 (a three-component vertex). Inferred, strongly: any other defaulting would make the engine path draw
   different pixels from the host path for ordinary two-dimensional geometry.
2. *Port A under `10` or `00`, z is not used.* Port A is chosen only when the depth test is off, and depth writing
   needs the test on in the driver's own setter (`__glCoPassDepthMask` ANDs the mask with enable-word bit 4). So no
   depth value is needed. What the engine puts in the depth buffer, if anything, is unknown; the model must not write
   one. The occlusion-count control in the depth-test word (bits 28:27) is independent of pixel-pipe bit 29, so a
   port A vertex drawn while the occlusion count runs is a case the driver permits and the engine's behaviour for it
   is unknown. Inferred (rule), unknown (that case).
3. *w under `00` is a placeholder.* The host sends 1.0, not the reciprocal of its own clip w, so no perspective
   correction can come from w on that path; the host fill routines that use `00` (anti-aliased, stippled and two-sided
   fills) draw textured fragments with whatever the engine does with w = 1. Inferred.
4. *w under `10` is the clip w.* The viewport packet carries scale and offset only, so the engine's perspective divide
   needs the sent w; the driver sends it for no other reason. Inferred.
5. *A missing w dword means w = 1.* The depth clear (flags `00`) and every 2D vertex omit the w dword and still draw
   correctly under the drivers, while the generic routine in the same mode sends 1.0. So the presence bit (instance bit 4)
   is a presence bit and an absent w behaves as 1. Inferred.

**Unknown.** Whether the engine, given a port A vertex under `00` or `10`, leaves the z of the previous vertex or uses
zero: no shipped path depends on it, because depth is off. The same for the occlusion case in point 2.

**What would settle it.** Nothing shipped depends on it, so the repository rule applies: an emulator author chooses,
and the page that records the choice rates it an approximation. A choice that is consistent with every reading above
is z = 0 and w = 1 for an absent component, in all three modes. A run of the model's own captured driver packets
confirms only what the driver writes (the table above), not the hardware; the Windows 2000 build of the OpenGL driver,
whose vertex routine was not read here, is the one further binary route and can only repeat the driver side.

## Viewport, bias, window origin and clipping

**What the driver sends.** Three packets set the geometry space, and a fourth is the miniport's.

- Viewport (`0x0362a0c0`, 6 floats, `__glCoSetViewMatrix@0x69018dfa`, whole): x scale, y scale, z scale, x offset,
  y offset, z offset. The scale and offset are as [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#viewport-and-coordinate-conventions)
  derives them (`UpdateViewport@0x690198ee`, whole); the x offset is the viewport's left edge plus half its width,
  minus 1024 and minus 0.492, and the y offset the window height less the viewport's bottom edge and half height, minus
  1024 and minus 0.508. The y scale is negative.
- Bias (`0x04230040`, two floats). The OpenGL driver's own writes are 0, 0 (after a texture bind,
  `coSetTexParam@0x6901ebbb`), a pixel-transfer value of 1024.0 or 1024.5 per axis (constants at `@0x69002710` and
  `@0x6900272c`, sites `@0x6903af11`..`@0x6903af75` and `@0x6903de1b`..`@0x6903de49`), and nothing around a triangle
  draw. The display driver, for a window, computes the pair from the window's top-left corner, and the miniport copies it
  into the saved context image at each switch-in (`sgcodd.dll@0x1cec5`..`@0x1cf70`, `sgcomp.sys@0x16738`..`@0x16758`).
  Each axis is `+0.5` when the corner plus 1024 is 0, `-0.5` when it is -1, and otherwise the corner plus 1024 plus 0.5 (the
  midpoint of two consecutive entries of the driver's integer-to-float table), with the index clamped to -4096 to 4094.
  Established as what is written.
- Window origin (`0x9845b020`, y in 31:16, x in 15:0): the miniport adds `0x04000400` (1024 each axis); the display
  driver sends the origin without the bias ([packet-set.md](../../devices/cobalt-graphics/packet-set.md#window-parameter-packets)).
- The miniport's window mask packet (`0x98250160`) clips to up to five rectangles; the OpenGL driver does not touch it.

**Order of application.** Nothing in the driver orders these in one expression; the engine's order is inferred from
which terms cancel. The viewport offset contains -1024 and the origin contains +1024, so the two integers cancel only
if the engine **adds** the origin to the viewport-mapped coordinate; the sum is then the window-relative position
plus the window's own origin, which is what a window on the screen needs. Addition is commutative, so the order of
offset and origin matters only where the engine rounds or clamps between them; that is unknown. So:

1. `x_win = x_clip / w * x_scale + x_offset` (and y; z with its own scale and offset): inferred, from the packet and the
   driver's use of clip coordinates plus w under `10`. Under `11` the engine first applies the model-view-projection
   matrix to the object coordinates (inferred, from what `11` sends): clip = M * (x, y, z, 1) with the sixteen words as rows, word `4*i + k` being row `i`, column `k` (the translation in words 3, 7 and 11; [packet table](../../devices/cobalt-graphics/packet-set.md)). Reading them as columns gives a wrong result for any matrix that is not symmetric.
2. The origin is added afterwards: inferred (cancellation of the +/-1024 pair). The 1/64-pixel resolution the driver's
   `subpixelBits` of 6 implies is reached after this sum: unknown.
3. The bias pair is not part of the position chain. Its only OpenGL uses are zero and a pixel-transfer constant, and the
   display driver's value is the window corner plus the pixel centre, in the same 1024-biased space as the origin
   register. The vendor's names (`texWinoffsetS`, `texWinoffsetT`) and the 2D copy (whose s and t equal its x and y)
   say it shifts texture coordinates, not positions. Inferred. How the 3D path survives the miniport patching a
   window-derived value over the driver's 0, 0 at a switch-in is not decided here: either the patch does not reach
   a 3D context's bias or the bias is reset when the texture state is next validated. Unknown.

**Clipping.** The host clips in the `10` case and the engine is not given the chance to differ: the guard band comes
from the viewport (`UpdateViewport`): the x plane factor is `(2048 - x_offset) / x_scale`, the y factor the same with
the y offset and the absolute y scale, and the z factor the variable `GLCOZSLOP` (the default `zSlop` was not read; 1.0 when
`GLCOFORCECLIP` is set). The constant 2048 equals the `maxViewportWidth` of the context limits ([opengl-expectations.md](opengl-expectations.md#limits)),
so the clipper keeps the window x and y, after the viewport transform and before the origin, below 2048 on the positive
side; both plane signs share the one factor, so the negative side lies further out. Established (stores read); the
engine's limit is inferred to be at least that. Under `11` (the clip-volume hint, a promise that nothing needs
clipping) nothing clips on the host. The transform word's clip-check bit (28) is never set by any routine, so the driver
never asks the engine to clip or to flag.

**Unknown.** Whether the engine clips at all, what it does with a vertex whose w is at or below zero (under `10` the
host's near-plane clip keeps w positive; under `11` an unclipped object can produce it), and the width of its internal
coordinate after the sum. The vendor's `enClipCheck` name and the white paper's "frustum clip check" suggest a flag
or reject, not a clip: inferred from names, not from a routine. A model should treat w at or below zero under `11` as
unspecified and pick a documented choice (drop the primitive, as a trivial reject).

**Refines an earlier statement.** [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#viewport-and-coordinate-conventions)
says "how the engine combines the two is unknown". That stands for the rounding, but the cancellation argument above
narrows the order to an addition.

## Lighting words

**Space.** All light vectors are in eye space. `ComputeLightState@0x69085b5a` takes each light's `positionEye` (the eye-space
position the application's position was transformed to when the light was set); vertex vectors use the vertex's `eye`
coordinate (`+0x50`). The normal is the application's own normal in object space (`userNormal`, `+0x10`) in every draw-flags mode:
`__glCoSetVertexAttribsMask` copies it from there, and the driver sends the 3 by 3 inverse-transpose of the model-view
matrix (`inverseTranspose` of the transform record, `+0x50`, nine floats in header `0x02421120`) whenever the
model-view matrix changes (`@0x69023ad6`..`@0x69023b12`, `@0x69023b50`..`@0x69023bce`; the routine's only gate is the
soft-lighting flag, not the draw flags). The nine words are sent row by row (word `3*i + k` is row `i`, column `k` of the matrix that maps an object normal to an eye normal), the same order as the model-view-projection words; established from the routine, run on a test matrix. So the engine turns object normals into eye normals itself, in all three
modes; GL_NORMALIZE becomes lighting-word bit 26. Established (what is sent); the engine's use is inferred.

**Light vector words (`0x09a900c0 | i`, six floats).** For light `i`:

| Words | Directional, constant (loaded once) | Per-vertex (positional, spot, or local viewer) | Grade |
|---|---|---|---|
| 0 to 2 | unit vector from the light's `x, y, z` (`unitVPpli`, record `+0x20`): the light's eye position normalised (`ComputeLightState@0x69085d0c`..`@0x69085d14`) | unit vector from the vertex's eye position to the light (`lightVPli`, vertex `+0xf0 + 16*i`; `__glCoCalcLightVectors@0x6902763a`) | established (written); that the engine takes it as the direction toward the light: inferred |
| 3 to 5 | half-vector `hHat` (record `+0xb8`): the normalised sum of that unit vector and `(0, 0, 1)`, formed as `(x, y, z + 1)` then normalised (`@0x69085d3d`..`@0x69085d61`) | `lightHAngle` (vertex `+0x130 + 16*i`): the same with the vertex-to-viewer unit vector `-eye` normalised in place of `(0, 0, 1)` when the viewer is local (lighting-model local-viewer byte `gc+0x1abc`); otherwise `(x, y, z + 1)` normalised | established (written); use: inferred |

A light takes the once-loaded path only when `slowPath` (`record +0x104`) is 0: directional (`position.w == 0`), not a
spot, and non-local viewer (`@0x69085d20`..`@0x69085d7c`). Every other light sets `slowPath` and gets a per-vertex vector
packet through attribute-mask bit 3 for each light in the vertex-light mask `gc+0x2d5bc`; the loader sends no vectors for
it. For a per-vertex light whose specular colour is all zero (`nonzeroSpecular`, `+0x10c`, clear) the routine does not
compute the half-vector, so words 3 to 5 carry whatever was left in its stack frame; the engine's specular term is
zero for that light anyway (inferred), so a model must not depend on them.

**Light scalar word (`0x1129f020 | i`, one float).** At load, 1.0 for a directional light, and for a positional light
1.0 when either of its linear or quadratic attenuation is non-zero, otherwise `1 / constantAttenuation` (record `+0x30`,
set in `ComputeLightState`) (`LoadLightSource@0x69026d06`, whole). Per vertex, only for lights whose `+0x2d5cc` bit
`1 << (i + 8)` is set (positional with a spot or with non-constant attenuation), the scalar is the product of

- the attenuation `1 / ((quadratic * d + linear) * d + constant)` with `d` the distance from vertex to light (or `+0x30`
  when linear and quadratic are both zero, or 1.0 for a directional light), and
- for a spot light, a lookup in a 256-entry table indexed by `(cos - threshold) * scale + 0.5`, where `cos` is the
  negated dot of the unit vertex-to-light vector and the spot direction (record `+0x10`); the scalar is **0** when `cos`
  is below the threshold or below the cut-off cosine (`__glCoCalcLightVectors@0x690277b8`..`@0x69027817`, whole).

The scalar's effect on the ambient term is not shown: the driver sends one scalar and does not send an ambient
attenuation separately. OpenGL attenuates all three terms; whether the engine scales the ambient colour with the scalar is
inferred (GL semantics), not read. The light colours (`0x0ca96120 | i`, nine floats, ambient, diffuse, specular) are
the light's own colours, un-scaled through `__glUnScaleColorf`, not the products with the material.

**Host colour (`0x09280080`, four floats).** When lighting is on the engine path, `LoadLightPath@0x690267b4` sends
this packet with four zeros when it enables the path, so the register the engine
reads as the colour of an unlit vertex becomes a zero host contribution. A light the driver keeps on the host
(a fifth or later light, an out-of-range colour, a degenerate attenuation) is summed by `__glCoCalcExtraColor@0x6902707c`
(read in part: the per-light specular and spot arms and the tail): each host light's colour terms times the material
(the specular one through the specular table), then **clamped** per channel to 0 up to the context's per-channel colour
limits (`gc+0x3ed4`, `+0x3ed8`, `+0x3edc`) with the alpha dword set to 0, and sent per vertex in the colour slot of the vertex packet or as `0x09280080`. So the
driver clamps its own contribution before the engine sees it. Whether the engine then adds it before or after the
engine's own clamp is **unknown**: the driver gives no case where the sum exceeds 1 and the result depends on the
order. Choice for a model: add before the final clamp, which is also what the unclamped sum of OpenGL gives; the host
term is already within 0 to 1, so only the sum with the engine's own result can exceed 1. The white paper's
"accumulating a host-supplied contribution" fits an addition (inferred).

**Instance 1 on the diffuse block (`0x06c6d081`, `0x07a74081`).** Written in exactly one situation: colour-material mode
`GL_AMBIENT_AND_DIFFUSE` (`gc+0x1aa4` = `0x1602`), in the list converter (always, `@0x69061884`..`@0x69061887`,
payload one RGBA colour of four dwords) and in the code generator when a context byte is set (`@0x69049085`..`@0x6904908b`,
otherwise the generator writes the 7-dword ambient-then-diffuse block `0x0666a0e0`). The other colour-material modes
write one block of their own (ambient `0x0666a060`, diffuse `0x06c6d080`, specular `0x08278080`, emission `0x05a64060`),
each with instance 0, three or four dwords. So in the one mode where a single colour must update two material terms,
the driver replaces the seven-dword block by a four-dword diffuse block with instance 1. Inferred: **instance 1 on the
diffuse block means the colour also becomes the ambient colour of that side** (front for `0x06c6d081`, back for
`0x07a74081`). The alternative, that it is a no-op flag, would leave the ambient stale after a `glColor` under
colour-material `GL_AMBIENT_AND_DIFFUSE`, which the generic 7-dword form of the same mode does not. Not settled: what
the context byte that selects between the two forms in the generator means (the routine's byte `[esp+0x26]` is not
traced to its source here), and whether the engine's ambient takes the RGB only or the alpha as well (the block
has no alpha for ambient).

**Specular-only, separate specular and secondary colour.** Three bits and one packet interact, and the driver sets them
together:

| GL state | Lighting-word bit 25 (`enSpecular`) | Pixel-pipe bit 17 (colour sum) | Secondary colour (`0x190b1020`) |
|---|---|---|---|
| lighting on, single colour (default) | set | clear | not sent |
| lighting on, `GL_SEPARATE_SPECULAR_COLOR_EXT` | **clear** | set | sent per vertex, computed on the host by `__glCoCalcSpecColor@0x69027860` |
| lighting off, colour sum enabled | n/a | set | sent when the application supplies a secondary colour |
| lighting off, colour sum disabled | n/a | clear | not sent |

(`LoadLightPath@0x69026806`..`@0x69026849`, `__glCoEnableColorSum@0x690266ac`, `__glCoPickSetMask` bits `+0x3d80` 28 and 29;
established.) So with separate specular the driver clears the engine's specular enable and computes the specular sum
itself over all lights, per vertex, clamped, then sends it as three bytes in bits 31:8; the engine adds that colour after
texturing (bit 17), which is OpenGL's separate-specular order. With a single colour the engine adds its own specular
term into the primary colour. Bit 17 is the engine's "add the secondary colour after the texture stage": inferred
from these two writers. **Lighting-word bit 24 (`specOnly`) is never set by any routine** (the writes to the byte are
`| 2` for bit 25, `| 8, 0x10, 0x20, 0x40` for the four light enables, and `& 3` keeps bits 25:24 across a state update;
nothing ors `1`), so the driver has no use for a specular-only mode; what the engine does with it is unknown, and a
model may leave it unimplemented, as an unsupported, observable access.

The four light enables (lighting-word bits 30:27) follow the loaded slots: slot 0 sets bit 27, slot 1 bit 28, slot 2
bit 29, slot 3 bit 30 (`LoadLightSource` ORs `0x08`, `0x10`, `0x20`, `0x40` into the byte). Two-sided lighting is bit 31,
from the light-model flag; normalisation bit 26. What the engine does with a back-facing triangle's second set of
material blocks (`0x0746...`, `0x07a7...`) in two-sided mode is unknown (the driver splits two-sided fills on the host,
[draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)).

## For an emulator author

What a stage-2 and stage-3 model must do to run this driver, as a list of what the sections above fix or leave open:

- Parse the vertex ports with instance bits 1 to 4 as a presence mask; treat a missing w as 1 and a missing z as 0.
- Apply, per draw-flags mode, the pipeline in the table: mode `11` model-view-projection of object x, y, z (w = 1) then
  viewport, mode `10` viewport only on clip x, y, z and w, mode `00` window coordinates as sent. Add the window origin after.
- Keep eye-space lighting: light vectors as sent, normals through the nine-float matrix, light scalar as a multiplier,
  host colour as an additive term.
- Honour instance 1 on the diffuse block as "also set ambient", lighting-word bits 25 and 17 as in the table, and leave
  bit 24, the bias on positions and the clip-check bit unimplemented and visible.

Open items for the engine's side are recorded in [questions/open-questions.md](../../questions/open-questions.md#rendering-engine-arithmetic-and-vertex-semantics).
