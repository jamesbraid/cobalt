# Depth and stencil words, the clears, the 16-bit depth encoding, and the fog value

What the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 writes to clear, test, store and read depth and stencil, and the integer it
sends for fog. It extends [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#colour-depth-and-stencil-buffers)
and [packet-set.md](../../devices/cobalt-graphics/packet-set.md) and repeats nothing they hold; read those first. The
transfer sequences that move depth words are in [opengl-pixel-transfers.md](opengl-pixel-transfers.md).

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established**: read in disassembly (the decompile of the
typed project only navigated), cited `sgcoicd.dll@address`. **Inferred**: a reasoned reading, with the reasoning. **Unknown**:
nothing shipped or published decides it. The driver reads the engine's memory only through the transfer of the previous page, so
what the *engine* does with each word is inferred from what the driver writes and expects back. A value marked *choice* is for an
emulator author.

Read in disassembly: the depth, stencil and merge part of `__glCoClearBuffers@0x69046827`, `__glCoInitDepth@0x6902cfbe` and
`FetchDepth@0x6902d0ae` (the stores that matter), `Uncompress16bitDepthValue@0x69041940` with both tables, the depth and stencil
destination words of `__glCoLinearToTileXfer`, `__glCoPassFog@0x6902cc3e`, `__glFogVertex`, `__glFogVertexByTable` and
`__glFogVertexLinear` (`@0x690b5700`..`@0x690b58dd`, whole), the fog-value store in `__glCoSetVertexAttribsMask@0x6902d69e`, and the
context constants at `@0x69076539`. From the decompile only, so inferred and **Unverified** in the details: `coClearBuffers@0x690475c6`
(the unit table and mask arithmetic), `StoreDepth@0x6902d196`, `__glCoSpanReadDepth@0x690417a4`, `__glCoSetSzBuffer@0x69018a3c` and
`__glCoInitStencil`.

## Question 7: depth and stencil formats and clears

### The two formats and the byte layout

The depth and stencil descriptor word (`0x930ab020`, the word kept at `gc+0x2e310`) holds the descriptor id in 28:23 and a
three-bit format code in 31:29, which `__glCoInitDepth` sets once from the context's depth size (`+0x2498`):

| Depth size | Code in 31:29 | Class (code & 3) | Bytes per pixel | Layout of the word |
|---|---|---:|---:|---|
| 16 | `001` | 1 | 2 | 16-bit depth, in the encoding [below](#the-16-bit-depth-encoding) |
| 24 | `110` | 2 | 4 | depth in the **low three bytes**, stencil in the **top byte** |

The class is what every consumer of the word reads (`__glCoClearBuffers` and the transfer routines take `(word >> 29) & 3`)
and equals the bytes per pixel in the clear routine's unit table (class 0, 1, 2 give 1, 2, 4 bytes). Stencil shares the 32-bit
word with 24-bit depth: three independent writers agree.

- The clear merges a depth clear and a stencil clear into **one rectangle clear** with mask `0x77777777 | 0x88888888` and one
  value (below), `@0x69046bcd`..`@0x69046c1e` (a clear of depth alone uses `0x77777777`, of stencil alone `0x88888888`).
- A pixel upload to the depth buffer is described as a 32-bit colour word with the colour write mask set to `0x00ffffff`, and
  one to the stencil buffer with the mask `0xff000000` (the colour mask's top byte is "red", the buffer's top byte); the
  routine picks between them by comparing the destination with the depth buffer object
  (`__glCoLinearToTileXfer@0x6903b5bc`..`@0x6903b63b`, read in disassembly).
- `__glCoSetSzBuffer` copies the depth buffer's descriptor id into the stencil descriptor word (`+0x2e318`) when the combined
  flag (`+0x2e2d0`) is set, so the two views name the same memory.

Established. With a 16-bit depth buffer the stencil has no place in the word and no routine gives it one: the clear's merge
would write the stencil byte into the top of each 16-bit half, which the driver can only meet if no stencil is requested with
16-bit depth. Which pixel formats the display driver offers is not read here (it is still an
[open question](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle)). Unknown
for 16-bit depth with stencil.

### The 16-bit depth encoding

The driver decodes a raw 16-bit depth word with `Uncompress16bitDepthValue` (read in full, both tables read from the file at
`0x690f4910` and `0x690f4920`). Write `e` for bits 15:13 of the stored word and `m` for bits 12:0. The decoded linear 16-bit
value is

`linear = ones(e) | ((m << 2) >> shift(e))`, with `ones(e)` the top `e` bits set (`0, 0x8000, 0xc000, 0xe000, 0xf000, 0xf800, 0xfc00,
0xfe00`) and `shift(e)` equal to `0, 1, 2, 3, 4, 5, 6, 6` for `e` = 0 to 7.

So the stored value is a small floating-point code: a three-bit exponent that is the number of leading ones of the 16-bit linear
depth (capped at 7), then thirteen mantissa bits of which the low ones are dropped as `e` grows. The eight bands, in order
(each band's start is the previous band's end plus one step):

| `e` | Linear range decoded | Linear step | Codes per linear value |
|---:|---|---:|---:|
| 0 | `0x0000`..`0x7ffc` | 4 | 1 |
| 1 | `0x8000`..`0xbffe` | 2 | 1 |
| 2 | `0xc000`..`0xdfff` | 1 | 1 |
| 3 | `0xe000`..`0xefff` | 1 | 2 |
| 4 | `0xf000`..`0xf7ff` | 1 | 4 |
| 5 | `0xf800`..`0xfbff` | 1 | 8 |
| 6 | `0xfc00`..`0xfdff` | 1 | 16 |
| 7 | `0xfe00`..`0xffff` | 1 | 16 |

Stored `0x0000` decodes to `0` and `0xffff` to `0xffff`, so both ends of the range agree with the linear value. Examples:
`0x1fff` decodes to `0x7ffc`, `0x2000` to `0x8000`, `0x7fff` to `0xefff`, `0xa000` to `0xf800`, `0xe000` to `0xfe00`. Established
(the decode, both tables read from the file), and it is the only 16-bit depth conversion in the driver; the white paper's
"16-bit floating point" depth is the same fact from the vendor. The bands are the arithmetic of the table, not a claim about
how the engine rounds when it writes the code.

The 24-bit depth is plain unsigned fixed point: the span read takes `(word & 0xffffff)` and multiplies by `1 / 0xffffff`
(`__glCoSpanReadDepth@0x690417a4`, whole). Established.

**The encoding direction.** The driver never *encodes* a 16-bit depth. The engine does: a depth written by the CPU path is a
point drawn through the depth path (`StoreDepth`, a port B vertex `(x, y, z)` with `z = value / 0x3fffffff`, mode word
`0x28000000` = depth test on and colour mask all zero, primitive mode `0x8000` for a point; its companion `StoreALWAYS` sends the
depth function "always", inferred from the name and from the clear's `0xe0000000`), so the
engine takes a window z in 0..1 and stores it in the format of the buffer. The engine's rounding, and whether it truncates to
the nearest representable code below the value, are unknown.

**An asymmetry the driver has.** The two paths that write a *raw* word, the rectangle clear and a draw of depth pixels (type
9 or 10, [opengl-pixel-transfers.md](opengl-pixel-transfers.md)), put linear fixed-point values into the buffer without encoding
them, while the read-back decodes. Values other than 0 and `0xffff` therefore do not round-trip in a 16-bit buffer. The
driver ships with it, which says nobody uses it; the model should not "fix" it. Established (the three routines);
the consequence is inferred.

*Choice for an emulator:* encode on the depth-test and depth-write path only (z in 0..1 to a code by truncation), store raw words
exactly as sent by clears and by type-9 transfers, decode with the table on reads. Approximation. A discriminating test is the
asymmetry itself: clear to 0.5, read back, and expect `0.9375` (`0xefff / 0xffff`); a truncating encode and the decode are
identity at the codes the table produces.

### What the rectangle clear sends

The clear packet is `0x99ce00a0` (or the second bank `0x9e7200a0` when the context's `fastClear` byte is set): descriptor id,
byte mask, value, minimum corner, maximum corner ([draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#clears)).
For depth and stencil, `__glCoClearBuffers` builds, for each buffer it clears:

- **Depth.** `v = 0x3fffffff` when the clear depth is above `0.999999`, otherwise `trunc(0x3fffffff * clearDepth)`
  (`@0x69046b61`..`@0x69046b9b`). Then by class: class 1: `h = v >> 14`, **value `h | h << 16`, mask `0xffffffff`**; class 2:
  **value `v >> 6`, mask `0x77777777`**.
- **Stencil.** Value `stencilClear << 24` (the context's 16-bit clear value, only the top byte matters), **mask `0x88888888`**,
  with the stencil buffer's own descriptor id (the depth id when combined).
- **Both at once** (depth flag and stencil flag, one buffer): a single clear with **mask `0xffffffff`**, value `(v >> 6) | stencil << 24`.

| Clear | 16-bit class (2 bytes per pixel, 16 pixels per 32-byte column, 2 mask bits per pixel) | 24-bit class (4 bytes per pixel, 8 pixels per column, 4 mask bits per pixel) |
|---|---|---|
| depth 1.0 | value `0xffffffff`, mask `0xffffffff` | value `0x00ffffff`, mask `0x77777777` |
| depth 0.0 | value `0`, mask `0xffffffff` | value `0`, mask `0x77777777` |
| stencil `s` only | not meaningful (see above) | value `s << 24`, mask `0x88888888` |
| depth 1.0 and stencil `s` | not meaningful | value `s << 24 \| 0x00ffffff`, mask `0xffffffff` |

The corners are in 32-byte column units, with partial masks at the edges, as the existing clears section says. A clear that
needs the depth or stencil *operations* (a masked clear, or a stencil write mask) is drawn instead, as before.

**Engine implications.** Inferred: the mask has one bit per byte of the 32-byte memory row and a clear writes the value
into every byte whose bit is set; mask `0x77777777` therefore writes three bytes of each 32-bit word and leaves the top byte,
and `0x88888888` only the top byte. The clear does not transform the value, so a 16-bit depth clear to any value but 0 or 1.0
stores a linear word that the decode reads back differently (above). *Choice:* store the value bytes under the mask exactly.

## Question 8: the fog value

**What the driver sends.** When fog is on (mode word bit 21, [packet-set.md](../../devices/cobalt-graphics/packet-set.md#pixel-pipe-mode-word-0x80003020))
the vertex attributes carry the fog value as its own packet `0x192b2020`, one dword, **`trunc(255.0 * f)`** where `f` is a float
in 0..1 (`__glCoSetVertexAttribsMask@0x6902d916`..`@0x6902d93a`: load the vertex's fog float at `+0x3c`, multiply by `255.0`, call
the C runtime's truncating float-to-integer; no mask on the result; the pixel transfers send the same form, taken
from the raster position's fog at `gc+0x161c`, times `255.0`, `@0x6903b919`). The run-time code generator emits
the same store; its emitted multiplier was not read, so that arm is inferred.

**`f` is OpenGL's own fog factor, not its complement.** The host computes, per vertex, from the eye-space distance `|z_e|` (the
vertex's eye z at `+0x58`):

| Fog mode (`gc+0x1b68`) | `f` |
|---|---|
| `GL_LINEAR` (`0x2601`) | `(end - |z_e|) * (1 / (end - start))`, the two constants stored by `glFog` at `+0x1b84` and `+0x1b88` |
| `GL_EXP` (`0x800`) | `exp(-density * |z_e|)` |
| `GL_EXP2` (`0x801`) | `exp(-(density * |z_e|)^2)` |

The result is clamped to 0 below and to the context's `one` above (`gc+0x250c`, set to 1.0 by `fld1` at `@0x69076539` in the
context set-up). A table-driven variant, used when the context asks for it, gives the same values to the table's precision.
`f = 1` is "no fog" in OpenGL, so an unfogged vertex gets `255`. Established (the host routines read in full).

**What it implies.** Inferred: the engine does **not** compute a fog factor from depth or w. It receives an integer per vertex,
interpolates it across the primitive and blends. Because the driver passes OpenGL's `f` unmodified and OpenGL's blend is
`C = f * C_fragment + (1 - f) * C_fog`, the engine must blend the same way with the 8-bit value `F = 0..255` as `f * 255`:
`C = (F * C_fragment + (255 - F) * C_fog) / 255` per colour channel, with `C_fog` the packet `0x922a4020` (three bytes in 31:8,
`trunc(255 * c + 0.5)`, `__glCoPassFog@0x6902cc3e`, read in full). A model that took `F` as the amount of fog would invert every
fogged scene. The fog colour has no alpha. Whether the engine interpolates `F` with perspective correction, its rounding of
the blend, and whether alpha is blended are unknown.

**Per fragment.** A case where the driver skips the vertex fog procedure entirely (`__glCoPickFogProcs@0x69035dd6`, a clear bit 12
of `gc+0x3d80`, tested but not set by any routine found) would mean the engine supplies the fog value; no writer was found, so
it is unreached in what was read. Unknown.

**Choice for an emulator:** interpolate `F` linearly in screen space (the same interpolation as colour), blend with `C_fog` as
above after the texture and colour-sum stages and before the alpha test, per [the pixel-pipe order the mode word implies](../../devices/cobalt-graphics/packet-set.md#pixel-pipe-mode-word-0x80003020);
the order is inferred from the mode word's bit order, not read.

Recorded as emulator choices in [questions/open-questions.md](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle).
