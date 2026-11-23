# Pixel transfers: source types, the colour matrix, and the draw, read and copy packet sequences

How the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 moves pixels and texture images through the engine: which transfer type it
declares for each host format, what the colour-matrix packet holds and when it is sent, and the packet sequence of the
three transfer routines. It extends [textures.md](../../devices/cobalt-graphics/textures.md) (type codes) and
[packet-set.md](../../devices/cobalt-graphics/packet-set.md) (the headers) and repeats nothing they hold. The texture
side (levels, the shared small tile) is in [opengl-texture-levels.md](opengl-texture-levels.md); depth formats and clears
are in [opengl-depth-stencil-fog.md](opengl-depth-stencil-fog.md).

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established**: read in disassembly (the decompile of the
typed project only navigated), cited `sgcoicd.dll@address`. **Inferred**: a reasoned reading, with the reasoning. **Unknown**:
nothing shipped or published decides it. The driver reads nothing back from these routines, so what the *engine* does is
inferred from what the driver writes and relies on. A value marked *choice* is for an emulator author.

What was read, and how. In disassembly: the order and values of every packet-header store in `__glCoLinearToTileXfer@0x6903b471`,
`__glCoTileToLinearXfer@0x6903c561` and `__glCoTileToTileXfer@0x6903ce87` (the three sequences below are that order), the whole of
`coInitTexImageStore@0x69020110` format selection, `coLinearToTileSpans@0x6903a7e6`'s shifts and constants, `__glCoExtEscape`'s
call sites, and the two miniport arms. From the decompile only, so inferred and **Unverified** in the details: the conditions
that choose between the branches of the three routines, the vertex arithmetic of `coTileToLinearSpans@0x6903c176` and the copy,
`coProcessRGBLinearToTile@0x6903b060`, `coLoadLinearTlb@0x6903ab8c`, `__glCoSetXferParam@0x6903a260`,
`coSetColorMatrixRegisters@0x6903bcac` (its constants were decoded and its matrix arithmetic checked, below), `coSetupTileToTile`,
`coCopyPixelsRW@0x6903de92` and the format selection of `__glCoPickDrawPixels@0x690383d2`. `__glCoPickReadPixels` and
`__glCoPickCopyPixels` were not read; the read and copy types come from the routines that call the transfers.

## The shape of every transfer

There is no transfer packet. A transfer is **primitives drawn through the texture path**, which is why the pixel-pipe mode
word carries bits 19 and 18 together ([packet-set.md](../../devices/cobalt-graphics/packet-set.md#pixel-pipe-mode-word-0x80003020)).
Draw: the host image is a texture (linear flag set) and the destination buffer is the draw buffer. Read: the colour or depth
buffer is the texture and the *host* buffer is the draw buffer, with its linear flag set in the draw-buffer descriptor.
Copy: the buffer is both. The texture is sampled with nearest filtering, repeat wrap, no mipmaps and environment `REPLACE`.
Established (the words below).

Inferred engine reading, used throughout: the texture unit decodes a source texel by its type into four channels, the
colour matrix (if its mode bit is on) maps them, and the draw buffer's format packs them. The type in the *source* word and
the format in the *destination* descriptor are independent, so one routine converts any source to any buffer.

## Question 6: type 5 against type 4, and the colour matrix

### Channel order of types 4 and 5

`coInitTexImageStore` (the texture-upload selector) and `__glCoPickDrawPixels` (the glDrawPixels selector) choose a type from
the host format and component type, and the same pairs recur. For eight-bit-per-channel four-component sources on this
little-endian host, with `gc+0x19ec` clear (read as the unpack byte-swap flag; it flips types 4 and 5 exactly where a
byte swap would):

| Host format, component type | Memory order of the bytes | Type (depth 2) | Matrix |
|---|---|---:|---|
| `GL_ABGR_EXT` (`0x8000`), unsigned byte | A, B, G, R | **4** | none |
| `GL_RGBA`, packed `UNSIGNED_INT_8_8_8_8` (`0x8035`) | A, B, G, R (word `RRGGBBAA`) | **4** | none |
| `GL_RGBA`, unsigned byte | R, G, B, A | **5** | none |
| `GL_ABGR_EXT`, packed 8888 | R, G, B, A | **5** | none |
| `GL_BGRA_EXT` (`0x80e1`), unsigned byte | B, G, R, A | **5** | red-blue swap |
| `GL_BGRA_EXT`, packed 8888 | A, R, G, B | **4** | red-blue swap |

(`@0x69020604`..`@0x69020646`, `@0x690206bf`..`@0x690206ff`, `@0x69020652`..`@0x690206e5`, the draw-pixels equivalents at
`@0x69038b79`..`@0x69038d38`, read in disassembly.) With the flag set, 4 and 5 trade places in every row.

So **type 4 is the engine's native 32-bit layout, red in the top byte of the little-endian word (memory A, B, G, R), and type 5
is the same four bytes in the opposite order (memory R, G, B, A, red at the lowest address)**. The six rows are consistent
only with that reading, and the BGRA rows need the red-blue swap matrix exactly because their memory order is B, G, R, A
under type 5 or A, R, G, B under type 4. Established (selection), inferred (channel meaning).

Other source types the same selectors write, all established as what is sent, each with depth in parentheses:

| Host source | Type |
|---|---|
| `GL_LUMINANCE`, unsigned byte | 6 (0) |
| `GL_LUMINANCE_ALPHA`, unsigned byte | 7 (1) |
| `GL_RGB`, unsigned byte | 8 (0), three passes, see [below](#rgb-bytes-three-passes); or 5 (2) when the host expands to RGBA first (`coRGBUbyteToRGBA`, chosen when the one-bit flag at level record `+0x20`, which the tile-allocation reply sets and whose meaning is unknown, is set) |
| `GL_RGBA`, `UNSIGNED_SHORT_4_4_4_4` (`0x8033`) | 3 (1) |
| `GL_RGBA`, `UNSIGNED_SHORT_5_5_5_1` (`0x8034`) | 2 (1) |
| `GL_RGB` or `GL_RGBA`, `UNSIGNED_SHORT` | 5 (2), host expands first (`coRGBUshortToRGBA`, `coRGBAUshortToRGBA`) |
| `GL_YCRCB_422_SGIX` (`0x81bb`) or `0x8318`, unsigned byte, into an RGB texture or the colour buffer | 10 (2), with the YCbCr-to-RGB matrix. The selector first writes 9, then overwrites it (dead) |
| the same, into a texture whose internal format is one of `0x81bd`, `0x85a3` or `0x81be`, `0x85a4` | 14 for the first pair, 15 for the second, depth 1 |
| `GL_STENCIL_INDEX`, unsigned byte (draw) | 8 (0), to the stencil buffer, see the depth page |
| `GL_DEPTH_COMPONENT`, unsigned short or unsigned int | 9 (1) or 10 (2): raw words, no conversion, see the depth page |

Type 5's channel order, the conversion of the packed 16-bit types, and which of 14 and 15 is Y-first are inferred from the
pairing with the host formats; the engine's decoder is not described anywhere else. Types 9 and 10 are the raw 16-bit and
32-bit word types: they are also what a depth read-back and a copy of a depth buffer use
([opengl-depth-stencil-fog.md](opengl-depth-stencil-fog.md)).

#### RGB bytes: three passes

`GL_RGB` bytes have no four-byte pixel to describe, so `coProcessRGBLinearToTile@0x6903b060` (read in full) draws the row
three times as an **8-bit type-8 source of width `3 * pixels`**, one pass per channel, steering the result with the colour
write mask:

| Pass | Colour mask packet `0x92eaa020`, payload | Source start | Lands in |
|---|---|---|---|
| 1 | `mask & 0xff000000` | byte 0 of the first pixel | red |
| 2 | `mask & 0x00ff0000` | byte 1 | green |
| 3 | `mask & 0x0000ffff` | byte 2 | blue and alpha |

Each pass is one line per row whose texture coordinate runs over `3 * width` texels while the line covers `width` pixels, so
the engine samples every third byte. Rows are cut into pieces of at most `maxRgbWidth` pixels (default 512, from an
environment variable clamped to 32..896). Before the first pass the routine sets mode bit 26 (partial colour mask) and puts
the application's mask back afterwards. Inferred consequences for the engine: a type-8 texel is written to all four colour
channels (`REPLACE`, class 1), the colour mask selects the one that is kept, and the line's texture coordinate steps by
`3 * width / width` per pixel. Established (the packets), inferred (the engine consequences).

### The colour-matrix packet `0x944c42c0`

22 payload dwords, written by `coSetColorMatrixRegisters` (read in full). The layout, now decoded:

| Dwords | Content |
|---|---|
| 0..4, 5..9, 10..14, 15..19 | four groups of **row `r` of the 4 by 4 matrix (four floats), then the bias `b[r]`** (one float): `out[r] = sum_c M[r][c] * in[c] + b[r]` |
| 20 | clamp minimum, default `0` |
| 21 | clamp maximum, default `0xffffffff` |

The rows are output channels and the columns input channels, in the channel order of the source type (red first for the
user matrix). Established that the groups are rows then bias (the loop writes four matrix floats then one bias float, four
times). That it is `M * in` and not `in * M` is inferred from the two video matrices below, which are correct only in this
form. The clamp words come from two environment variables read once (`GLCOPIXPOSTCMCLAMPMIN` and `GLCOPIXPOSTCMCLAMPMAX`,
hexadecimal, the strings sit in the code at `@0x6903bc7e` and `@0x6903bc96`); no routine sets them otherwise. Their meaning
(a bound on the packed result after the matrix) is inferred from the names only.

**When it is sent.** Only when the transfer needs it. `coStoreTexImage` sets the context's matrix flag (`+0x2db52`) when the
user's colour matrix is active (`+0x2da7d`), the swap flag is set (`+0x2db51`), or a post-pass flag is set; the transfer
routine then ors mode bit 20 into the pixel-pipe word and sends the packet. Without the flag the mode word has bit 20 clear
and the packet is not sent, so **the matrix is a plain pass-through when bit 20 is clear** (inferred) and the driver relies on
nothing about the stale register contents.

**Which matrix, per case** (established as what is written; the decompile of `coSetColorMatrixRegisters` was checked
against the stores for the constants):

| Case (flag) | Matrix rows (output R, G, B, A; columns in 0..3) | Bias |
|---|---|---|
| Red-blue swap (`+0x2db51`, from BGRA host formats) | `(0,0,1,0) (0,1,0,0) (1,0,0,0) (0,0,0,1)`, an anti-diagonal swapping columns 0 and 2 | zero, or the user's bias with its red and blue exchanged |
| YCbCr to RGB (`+0x2da88`, uploads of `0x81bb` and `0x8318`) | `(1.1644, 1.5960, 0, 0)`, `(1.1644, -0.8130, -0.3918, 0)`, `(1.1644, 0, 2.0172, 0)`, `(0,0,0,1)` | `(-0.8708, 0.5296, -1.0814, 0)` |
| RGB to YCbCr (`+0x2da89`, a texture read-back in a YCbCr format) | `(0.2568, 0.5041, 0.0979, 0)`, `(-0.1482, -0.2910, 0.4392, 0)`, `(0.4392, -0.3678, -0.0714, 0)`, `(0,0,0,1)` | `(0.0625, 0.5, 0.5, 0)` |
| The user's `GL_COLOR_MATRIX` and scale/bias (`+0x2da94`) | the application's matrix, multiplied with any of the above | the application's bias, carried through the same product |

The video constants are the BT.601 studio-range pair (the first is `255/219` for luma and the offsets are `16/255` and `128/255`
folded in). In the YCbCr-to-RGB case the input channels are, in column order, Y, Cr, Cb; in the RGB-to-YCbCr case the output
rows are Y, Cb, Cr. The display driver's own YUV blit loads a matrix too ([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md)).
When the application's matrix is also active the driver multiplies it in first, then applies the swap last. Established
(values), inferred (which channel is which, from the arithmetic).

**Choice for an emulator:** apply `out = M * in + b` on channels as floats in 0..1, then clamp to 0..1 and pack; treat the two
clamp dwords as a bound on the packed colour with the defaults a no-op.

## Question 9: the draw, read and copy packet sequences

All three first save and restore state, and all words below are read in `__glCoLinearToTileXfer`,
`__glCoTileToLinearXfer` and `__glCoTileToTileXfer` as written. `X(hdr, ...)` is a packet; "if changed" means the driver
compares with its shadow of the register and skips the packet when equal.

### Draw (host linear image to the colour, depth or stencil buffer)

1. For a 24-bit depth or stencil destination only: `X(0x92eaa020, mask)` with `0x00ffffff` when the target is the depth buffer and
   `0xff000000` when it is the stencil buffer (stencil is the top byte, "red"); mode bit 26 is set for the transfer.
2. If the colour matrix is needed: the matrix packet, mode bit 20 set.
3. Flush marker `0x98804000`, then `X(0x91aa0020, destination descriptor | 0x40000)`.
   *Colour buffer*: the context's draw-buffer word. *Depth or stencil buffer*: built from the depth descriptor, described as
   **type 3 depth 1** (16-bit depth) or **type 4 depth 2** (24-bit depth with stencil), bit 31 cleared. Established.
4. If the pixel-pipe word differs: marker, `X(0x80003020, word)`. The word keeps bits 31:21 of the current mode (stencil
   test, depth write and test, offset, mask, dither, logic op, blend, alpha test, fog), clears bits 20:0, and adds
   bits 19:18, bit 20 when the matrix is on and bit 26 for the partial-mask cases. So per-fragment operations apply to a draw.
5. Scissor: the application's (`__glCoSetScissor`) for a draw to a window, or `X(0x84e44040, 0, 0x0fff0fff)` for an internal
   transfer (the texture upload, the copy back).
6. `X(0x00210020, 0)`: draw flags set to window coordinates; the previous value is remembered and restored at the end.
7. If fog is on in the mode word: the raster position's fog value, `trunc(255 * f)`, is sent as `X(0x192b2020, value)` before
   both vertices of the first line of each batch ([opengl-depth-stencil-fog.md](opengl-depth-stencil-fog.md)).
8. The host image is pinned: `coLoadLinearTlb` calls escape `0x4f` (`PIXELDMA`, input `0x7c`, output `0x10`) with the host
   address range, gets a descriptor id and a page count back, and sends `X(0x04230040, bx, by)` (below). The kernel queues
   the escape as a drop in packet order; its pixel-DMA arm (`CoSchedulePixelDma@sgcomp.sys 0x1960a`, read in full) only
   records the run as a buffer of the node, marked "reload the translation table", and counts its slots, so the pages reach
   the table when the node's buffers are next loaded. The driver keeps the run pinned and re-uses it while it covers the next
   transfer's range (default; an environment variable turns it off); in that case it sends only a cache operation (escape
   `0x50`, `CACHEOP`) instead of a new pin. It then sends
   `X(0x93cc0080, word1, 0, 0, 0)` if changed, with word 1 `((type | 1 << 4) << 2 | depth) << 6 | id) << 19`, i.e. the host
   image as a **linear texture of the selected type and depth, id the pinned descriptor**, and `X(0x91ea2040, 0x30 << 25 | class, 0)`
   if changed, which is `REPLACE` and the base-format class 1.
9. If the primitive-mode word differs: `X(0x18002020, 0x20008000)`, type 1 (lines) with the line-width field equal to 1.
10. Per source row, one **line**: two vertices on port A (header `0x18a33068`: count 3, instance 8, payload `x, y, s, t`) or
   port B (`0x18c33068`, payload `x, y, z, s, t`, `z` the raster-position depth, chosen by a flag of the transfer record that
   was not traced; inferred to be set when the depth test applies). The
   first vertex is `(x0, y, s, t)`, the second `(x0 + dest width, y, s + source width, t)`, with `(s, t)` below. `y` steps
   by the vertical zoom per row; the first fog value (step 7) precedes the vertices. After `__forceSpans` lines the driver
   flushes the packet buffer.
11. Restore: the application's colour mask packet if it was changed, `__glCoSetDrawBuffer` (marker and `0x91aa0020` for the
    real draw buffer), marker and `0x80003020` with the saved mode word, `X(0x00210020, saved)`, and `__glCoRestoreXferParam`
    (the texture packets from their shadows). A pinned run that no longer covers the next range is released with escape
    `0x51` (`PIXELDMA_CLEANUP`) before the new pin.

**How a linear host image is described.** Established: the driver gives the engine a *list of the host pages* (the escape
fills translation-table entries for them) and addresses the image through the vertex's `(s, t)` as if it were a surface
**2048 bytes wide**: with `off` the byte offset of the texel from the start of the first page, `s = (off mod 2048) /
bytesPerTexel` and `t = off div 2048` (`coLinearToTileSpans@0x6903a831`..`@0x6903a85a`, the shifts `0xb` and the mask `0x7ff`).
The row start offset steps by the host row pitch from row to row. The descriptor of the pinned run has only a base address and
slot count (`coLoadLinearTlb`); the geometry fields the descriptor has for a tiled buffer are not what the driver relies on.
Inferred for the engine: when the linear flag of a texture or draw-buffer word is set, the address of `(s, t)` is the flat byte
offset `t * 2048 + s * bytesPerTexel` translated page by page through the descriptor's table slots. This is the reading that
makes both the draw and the read below consistent with one set of vertices.

**Alignment rules the driver enforces before it trusts the engine** (established): the host base address and row pitch must be
multiples of the texel size; for a read the row length must be at least 128 bytes, the byte count a multiple of 8, and the base
128-byte aligned unless a cache-bypass flag is set. Otherwise the driver reads into an aligned scratch buffer and copies on the
host. The engine writes whole 128-byte lines to host memory (inferred).

### Bias

`X(0x04230040, bx, by)` follows the pinning. `bx = by = 0.5` normally; with pixel zoom above 1 each is `0.5 / zoom`; when the
destination is a window (`flag & 0x10`) 1024.0 is added to both (and the copy uses 1024.5). Established. This is the texel-centre
shift of the source lookup in the 1024-biased window space ([opengl-triangle-path.md](opengl-triangle-path.md#viewport-bias-window-origin-and-clipping)).

### Read (colour or depth buffer to a host linear image)

1. `__glCoSetXferParam`: the **source buffer as a texture**, `X(0x93cc0080, ((type, depth, id of the buffer's descriptor)), 0, 0, 0)`
   with linear 0, and the environment packet as above. For the depth buffer the type is **9** (16-bit) or **10** (otherwise),
   depth 1 or 2 (`FetchDepth@0x6902d0ae`, `__glCoSpanReadDepth`).
2. `X(0x84e44040, 0, 0x0fff0fff)`, `X(0x00210020, 0)`, the matrix if needed, and if changed marker and
   `X(0x80003020, 0xc0000 or 0x1c0000)`. No per-fragment bits: a read never depth tests or blends.
3. `coTileToLinearSpans`: `coLoadLinearTlb` pins the host buffer, then marker and `X(0x91aa0020, word)` with
   **bit 31 set, the destination format in 30:25 and the pinned descriptor id in 24:19**: *this is how the linear host destination
   is described*, the linear flag of the draw-buffer descriptor, `word = (((type & 0xf) << 2 | depth & 3) << 25) | 0x80040000`
   with the id inserted. Established.
4. Primitive mode `X(0x18002020, 0x20008000)` once, then per row one line on port A with instance 8: first vertex
   `(s, t, srcX, srcY)`, second `(s + dest width, t, srcX + width, srcY)`. The **destination** coordinates are the 2048-byte
   virtual surface of the pinned run, the texture coordinates are the source rectangle in the buffer's own space. `srcY` steps
   by the source row step.
5. Restore as for a draw, plus `__glCoSetScissor`.

### Copy (buffer to buffer)

Used for `glCopyPixels`, the texture sub-image copy and the colour-buffer region copy. `__glCoTileToTileXfer` first tries the
host: if source and destination overlap in one buffer it uses `coCopyPixelsRW`, which does a **read into a scratch image and a
draw back** (both routes above), never a CPU read of the buffer. Otherwise:

1. Marker and `X(0x91aa0020, destination descriptor)` (preceded by a colour-mask packet for a depth or stencil destination), the
   matrix if needed, marker and the mode word as above, `X(0x00210020, 0)`, then the source as a texture through
   `__glCoSetXferParam` (types 3 or 4 for a 16- or 32-bit colour buffer, 9 or 10 for depth, `coCopyPixelsRW` and the depth copy
   use 3 and 4). 2. `coSetupTileToTile(flags)`: flush the packet buffer, then escape `0x52` (`SETUP_SCR2SCR`) with `flags` `0x10`, `0x18` or `0x30`
   (a second call after the vertices sends 0 to clear them). The kernel's arm (`CoScheduleScr2Scr@sgcomp.sys 0x195d8`, read
   in full) only replaces bits 5:3 of the render node's flags with that value; nothing is copied. Those bits choose the node's
   window-parameter packet at the next switch-in (`CoBuildWinparamsPacketForPxfer@sgcomp.sys 0x16f7a`, read in full): with bit
   5 clear all five clip rectangles and the origin are zero; with it set the valid mask is `0x0c000030`, the origin `0xf800f800`
   and a rectangle `0xf800f800`..`0x07ff07ff`, so "everything"; when the caller asks, the packet is followed by the scissor
   `0x84e44040, 0, 0x0fff0fff`, and with bit 4 set also by the bias `0x04230040` taken from the window's `texWinoffsetS`
   and `texWinoffsetT`. Bit 3 is never read there. Then, for a window destination, the driver sends `X(0x04230040, 0.5, 0.5)` (1024.5 each when the draw buffer is
   a window) itself. Established.
3. After `coSetupTileToTile`, the scissor (the application's, or `0x84e44040, 0, 0x0fff0fff` for an internal copy).
   **A rectangle as two triangles**: primitive mode `X(0x18002020, 0x60000000 | ...)`: type 3 (triangles). When source and
   destination overlap, the low bits get `0x100 | (direction << 9)`, a two-bit code chosen from the signs of the horizontal and
   vertical offset between source and destination. Then six vertices, each `(x, y, s, t)` on port A instance 8 (or port B
   with z when depth is involved, preceded by `X(0x192b2020, fog)` when fog is on): the destination rectangle's corners with
   the source rectangle's as texture coordinates.
4. A second `coSetupTileToTile(0)` and the restore sequence.

Unknown: what bit 8 and bits 10:9 of the primitive mode word do (the driver sets them only for overlapping copies, so they read
as a scan direction, which the engine needs to avoid overwriting its own source); what bit 3 of the render node's flags
does.

## What the engine must do, as a list

- Render lines and triangles through the texture path as ordinary primitives, with the mode word's bits 19:18 set, texture
  parameters as sent, and take the colour matrix when bit 20 is set.
- Support a *linear* source texture (a page list addressed `t * 2048 + s * bytesPerTexel`) and a *linear* destination (bit 31 of
  the draw-buffer descriptor) with the same addressing.
- Decode source types 2 to 8 and 10, 14, 15, with the channel orders above, and encode destination formats; for types 9 and 10
  copy the words raw.
- Honour the colour mask for the three-pass RGB case and for the depth/stencil byte split.

Recorded as emulator choices in [questions/open-questions.md](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle).
