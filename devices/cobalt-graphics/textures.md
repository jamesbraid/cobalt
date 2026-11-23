# Cobalt rendering engine: textures, pixel-format codes and sampler state

How the OpenGL driver `sgcoicd.dll` 5.2.4 (NT 4.0 build; the Windows 2000 build carries the same strings and the same
format table, found by byte search) describes a texture to the engine, and what it stores for each GL format. The packets
are in [packet-set.md](packet-set.md), the drawing model in [draw-and-buffers.md](draw-and-buffers.md), the driver's
limits and strings in [opengl-expectations.md](../../software/windows/opengl-expectations.md).

Grades are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established** here means a whole routine was read in
disassembly and the table or encoding is what it writes; the routine is cited as `sgcoicd.dll@address`. What the engine
does with a word is at best **inferred** (no document or public driver describes the engine), and anything not marked is
**unknown**. Field names in the vendor's own spelling come from the driver's program database (the
`CoTexGen*` and `CoTexEnv*` types and the `__GLCOmipMapLevelRec` record).

## What the driver sends

`coSetTexParam@0x6901df5f` (read in full) runs when a texture is bound or its parameters change. It builds three words
from the texture object and sends up to three packets:

1. **Texture parameters**, `0x93cc0080`, four words (format, mode, level of detail, border colour), only when any word
   differs from the shadow kept in the context (`+0x2d9b0` to `+0x2d9bc`) or when texturing was off in the pixel-pipe mode
   word (bit 18 clear). The pixel-transfer routines write the same packet through `__glCoSetXferParam@0x6903a260`.
2. **Texture environment**, `0x91ea2040`, two words (mode, colour), under the same rule with its own shadow
   (`+0x2d9c0`, `+0x2d9c4`).
3. **Bias**, `0x04230040`, two floats, both `0.0`, every time.

The routine returns 0, and the caller leaves texturing off, when the rounded minimum level of detail is above the rounded
maximum, when the shared small-level tile cannot be updated (`coUpdateCommonTile@0x6901ec68`) or when the texture's tiles
cannot be defined (`coDefineTexture@0x6901cb6a`); the debug text calls this "Texture Disabled - failed define" or
"update". Established.

## Word 1: format

| Bits | Field (vendor name) | Value | Grade |
|---|---|---|---|
| 31 | `linear` | 0 for a texture (`coSetTexParam`); 1 in the transfer routine, where the source is a host image read as a texture | established (writers), inferred (meaning) |
| 30:27 | `texType` | the four-bit type code ([below](#type-codes)), from the level record's `texelType` | established, inferred |
| 26:25 | `bufDepth` | the level record's `texelDepth`, two bits | established, inferred |
| 24:19 | `descId` | the six-bit id of the buffer descriptor holding the level, from the level's `tlbDesc.id` | established |
| 18:0 | | zero | established |

## Type codes

The same 4-bit type and 2-bit depth appear in three independent writers: the level records the texture routines build
(`coComputeTexLevelSize@0x6901f596`), the colour-buffer format word (`__glCoInitRGB@0x69044b18`, below) and the display
driver's colour format words (`0x12000000` and `0x24000000`, [2d-blit-video.md](2d-blit-video.md)). Equal values in
independent writers are what the codes' meanings rest on, which is why those rows are inferred, not unknown.

| Type | Depth | Meaning | Seen as | Grade |
|---|---:|---|---|---|
| 0 | 0 | 8 bits per pixel, colour index | display driver colour format `0`; never a texture | inferred |
| 2 | 1 | 16 bits, red 5, green 5, blue 5, alpha 1 | texture RGB5 and RGB5_A1; colour buffer with 5 bits per component (`0b001001`, `0x12000000`, white `0xfffe`); host transfer source `GL_UNSIGNED_SHORT_5_5_5_1` | established (writers), inferred (layout) |
| 3 | 1 | 16 bits, 4 bits per component | texture RGB4, RGBA2, RGBA4, R3_G3_B2; colour buffer with 4 bits per component (`0b001101`); transfer source `GL_UNSIGNED_SHORT_4_4_4_4` | same |
| 4 | 2 | 32 bits, 8 bits per component | texture RGB8 to RGB16 and RGBA8 to RGBA16 (all stored as 8 bits); colour buffer with 8 bits per component (`0b010010`, `0x24000000`, red in the top byte); transfer source BGRA bytes (type 4 or 5 by a transfer-state flag the routine reads from `gc+0x3dd4` and `gc+0x3dd9`) | same |
| 5 | 2 | 32 bits, a second channel order | transfer source RGBA bytes, `GL_UNSIGNED_INT_8_8_8_8`, `GL_UNSIGNED_SHORT` RGB and RGBA (converted on the host), ABGR; BGRA bytes (see type 4) | established (writers), unknown (what differs from 4) |
| 6 | 0 | 8-bit luminance | texture LUMINANCE4 to 16; transfer source luminance bytes | established (writers), inferred (layout) |
| 7 | 1 | 16 bits, luminance and alpha, 8 each | texture LUMINANCE_ALPHA and the `LUMINANCEn_ALPHAn` formats; transfer source luminance-alpha bytes | same |
| 8 | 0 | 8-bit alpha or intensity | texture ALPHA4 to 16 and INTENSITY4 to 16 (the environment's type field tells them apart, [below](#environment-word)); a transfer source of RGB bytes with a swizzle converter | same |
| 9, 10 | 1, 2 | depth transfer types: `FetchDepth@0x6902d0ae` writes 9 with depth 1 for a 16-bit depth buffer and 10 with depth 2 otherwise; 9 is also set at `0x690389a4` and `0x69021e10`. In `coInitTexImageStore` the 9 written for the SGIX YCrCb formats is overwritten, so it is dead there | established (writers), inferred (depth) |
| 14, 15 | 1 | YUV 4:2:2 forms | the display driver's YUV blit (`0x72100000`, `0x7a100000`); the transfer routine when a YCrCb conversion is selected | established (writers), unknown (which is which) |
| 0 (colour index, depth 0) | | also written at `0x69038b4d` | | |
| 1, 11 to 13 | | no immediate writer found; the writers that store a computed value were not all resolved, so "never written" is not established | | unknown |

A colour or depth buffer carries the same fields in its own words ([draw-and-buffers.md](draw-and-buffers.md#colour-depth-and-stencil-buffers)):
`__glCoInitRGB` sets bits 30:25 of the draw-buffer descriptor to `0b001101` for 4 bits per component, `0b001001` for 5 and
`0b010010` for 8, which is type 3 depth 1, type 2 depth 1 and type 4 depth 2. Established.

## Word 2: sampler mode

All of it is built by `coSetTexParam`; bits 15:0 are always zero. The GL value is the one the texture object holds
(`__GLtextureParamStateRec`: wrap S `+0x8`, wrap T `+0xc`, minification `+0x14`, magnification `+0x18` in the texture
record). Established for the table, inferred for the hardware meaning of each code.

| Bits | Field | Code | GL value |
|---|---|---:|---|
| 31:29 | `minFilter` | 0 | `GL_NEAREST` |
| | | 1 | `GL_LINEAR` |
| | | 2 | `GL_NEAREST_MIPMAP_NEAREST` |
| | | 3 | `GL_LINEAR_MIPMAP_NEAREST` |
| | | 4 | `GL_NEAREST_MIPMAP_LINEAR` |
| | | 5 | `GL_LINEAR_MIPMAP_LINEAR` |
| 28 | `magFilter` | 0 / 1 | `GL_NEAREST` / `GL_LINEAR` |
| 27:26 | wrap T | 0 | `GL_REPEAT` |
| | | 1 | `GL_CLAMP` (the border colour is used) |
| | | 2 | `GL_CLAMP_TO_EDGE_SGIS` (`0x812f`, `GL_SGIS_texture_edge_clamp`) |
| 25:24 | wrap S | same codes | |
| 23:20 | map height | | log base 2 of the level-0 height less twice the border |
| 19:16 | map width | | log base 2 of the level-0 width less twice the border |

The two debug strings the driver prints for the mipmap filters call code 3 "LINEAR_NEAREST" and code 4
"NEAREST_LINEAR", which are the vendor's short names for the first-then-second filter (the minification filter
first), so codes 3 and 4 read as above. The wrap codes 0 and 1 and the clamp-to-edge code are the only ones the
routine writes; `GL_CLAMP` also makes it build the border word. Wrap R is not sent (there are no 3D textures:
`max3DTextureSize` is never set, [opengl-expectations.md](../../software/windows/opengl-expectations.md#limits)).
The mode word is an uninitialised local: a minification value outside the six, or a wrap value outside the three, leaves stack garbage in that field (the routine has no error path for it).

## Word 3: level of detail

`coSetTexParam` builds `((maxLevel << 4 | (minLevel & 0xf)) << 3 | (lodFrac & 7)) << 21`, so:

| Bits | Field | Value |
|---|---|---|
| 31:28 | `maxLevel` | `min(round(maxLod), the object's maxLevel, the texture record's top level `p`)`, where `p` is shifted down as the base level rises (the shift count was read, its meaning is inferred) |
| 27:24 | `minLevel` | `max(round(minLod), 0)`, where round is the sum with the context's `half` constant (`gc+0x2508`) truncated (`_ftol`) |
| 23:21 | `lodFracSize` | 4 for a 1-D texture (the texture record's `dim` is 1), 3 otherwise; overridden by the variable `GLCOTEXLODFRACSIZE` |
| 20:0 | | zero. The vendor's type declares a `lodBias` in 20:11 that nothing sets |

Established for the packing; inferred for the meaning of `lodFracSize` (the number of fraction bits the engine keeps
of the level of detail; the only evidence is the name). A minification filter that is not a mipmap form (code 0 or 1) forces both the minimum and the maximum level to 0 (`0x6901e152`, `0x6901e157`). `GLCOTEXLODFRACSIZE` is clamped to 0 to 4 (default -2 becomes -1, meaning "use the computed value"). The code compares `minLevel` and `maxLevel` and refuses the
texture (returns 0) if the rounded minimum is above the rounded maximum.

## Word 4: border colour

Built only when a wrap mode is `GL_CLAMP`. When neither wrap mode clamps the routine leaves its local for the word
uninitialised and still sends it whenever it sends the packet, so the fourth word is then stack garbage and a model must
not depend on it. The border colour floats
(texture record `+0x1c`, `+0x20`, `+0x24`, `+0x28`, which are red, green, blue and alpha) are scaled by 255 and truncated
(no `+0.5`; `0x6901e5ad` to `0x6901e612`) and packed red in 31:24, green 23:16, blue 15:8, alpha 7:0. Established. The environment colour
below has the same order and the same truncation (`0x6901e91a` to `0x6901e991`); the alpha reference, blend colour and fog colour round instead.

## Environment word

The first word is `(function << 4 | type) << 25`, so `function` is bits 31:29 and `type` bits 28:25 (`CoTexEnvModeReg`).

| Function | GL environment mode | Grade |
|---:|---|---|
| 0 | `GL_MODULATE` | established |
| 1 | `GL_DECAL` | established |
| 2 | `GL_BLEND` (the environment colour is sent) | established |
| 3 | `GL_REPLACE` | established |
| 4 | `GL_ADD` (`GL_SGI_texture_add_env`, the environment colour is sent) | established |

The `type` field is the base format class of the level, not its storage type: the level record's `texelMode`
(`__GLCOmipMapLevelRec +0x14`), set with the storage type by the format table below: 0 RGB, 1 RGBA, 2 LUMINANCE,
3 ALPHA, 4 INTENSITY, 5 LUMINANCE_ALPHA. The debug text names the same six. The pixel-transfer routine writes function 3
(`0x30` added) and its own `pixMode` in the same field (a transfer uses mode 1, `CoCopyRect` too).

The second word is the environment colour, packed as the border colour is (red top byte) from the environment state's
four floats (`+0x4` to `+0x10`), sent only for BLEND and ADD. For ADD the routine then masks it by class: for RGB and
LUMINANCE bases (classes 0 and 2) the alpha byte is cleared, for ALPHA (class 3) the three colour bytes are cleared, and
other classes keep all four. Established for the packing; the reason is inferred (add of the colour only where the base
format has it).

## GL formats to storage

`coComputeTexLevelSize@0x6901f596` (read in full; the jump table at `0x6901f9ee` and the 25-entry byte table at
`0x6901fa12` decoded from the file) maps the format a program asks for to the format the engine stores and fills the level
record. Established.

| Requested format | Stored as | Type / depth | Class | Bytes per texel |
|---|---|---|---|---:|
| `GL_ALPHA`, `GL_ALPHA4/8/12/16` | `GL_ALPHA8` | 8 / 0 | ALPHA | 1 |
| `GL_LUMINANCE` (or 1), `GL_LUMINANCE4/8/12/16` | `GL_LUMINANCE8` | 6 / 0 | LUMINANCE | 1 |
| `GL_LUMINANCE_ALPHA` (or 2), `GL_LUMINANCE4_ALPHA4` to `GL_LUMINANCE16_ALPHA16` | `GL_LUMINANCE8_ALPHA8` | 7 / 1 | LUMINANCE_ALPHA | 2 |
| `GL_INTENSITY`, `GL_INTENSITY4/8/12/16` | `GL_INTENSITY8` | 8 / 0 | INTENSITY | 1 |
| `GL_R3_G3_B2`, `GL_RGB4` | RGB with 4 bits per component (reported as `GL_RGBA4`) | 3 / 1 | RGB | 2 |
| `GL_RGB5` | RGB with 5 bits per component (reported as `GL_RGB5_A1`) | 2 / 1 | RGB | 2 |
| `GL_RGB`, `GL_RGB8/10/12/16` | RGB with 8 bits per component (reported as `GL_RGBA8`) | 4 / 2 | RGB | 4 |
| `GL_RGBA2`, `GL_RGBA4` | `GL_RGBA4` | 3 / 1 | RGBA | 2 |
| `GL_RGB5_A1` | `GL_RGB5_A1` | 2 / 1 | RGBA | 2 |
| `GL_RGBA`, `GL_RGBA8/10_A2/12/16` | `GL_RGBA8` | 4 / 2 | RGBA | 4 |
| the legacy count 3 or 4 | by the window's colour depth: with up to 16 bits, 3 gives RGB5 and 4 gives RGBA4; with 24 or 32 bits, 3 gives RGB8 and 4 gives RGBA8 | | | |
| anything else (including `0x804e`) | refused, the level is not created | | | |

The routine loads the requested format from a stack slot that, for a context that is not RGB or whose four colour-bit
counts do not sum to 15, 16, 24 or 32, holds the record being filled instead of the format (`0x6901f6fd` reads `[ebp+0xc]`,
the output record, where the other branches read the requested format at `[level+0x40]`). The range test then refuses every
format, so colour-index contexts and contexts of any other depth get no hardware texture. Established (the load is read);
that this is a defect and not an intent is inferred.

No colour-index, paletted or compressed texture format exists in the hardware path; the extension string in
[opengl-expectations.md](../../software/windows/opengl-expectations.md) lists none for the Cobalt context. The byte
count is the *stored* size, so a 1 or 2 byte request widens to 4 for the RGB8 family.

A level's width and height are the image's less twice the border; the log2 of each is the map size field. A refused format gives a level size of 0 (descriptor id 0, texturing off). A level
wider than the context's `maxTextureSize` (4096) after scaling by its level number returns -1, and the caller raises the GL error `0x501` (`GL_INVALID_VALUE`). A level no larger than 16
by 16 is flagged (flag bit 0 of the record) and is held in the texture's one shared "common" tile, a 128-byte-wide
single 4 KB tile (`coAllocCommonLevel@0x6901efde`: slots 1, tiles per row 1), not in tiles of its own. Established
for the driver's structure; the engine's reading of the flag is inferred.

## Tile geometry of a level

For a level not in the common tile, `coComputeTexLevelSize` computes (established, from the same routine) with
`rowBytes = bytesPerTexel * width`:

- tile row length: 128 bytes if `rowBytes` is at most 384, otherwise 512;
- tiles per row: `ceil(rowBytes / tileRowLength)`;
- tile size from `tileRowLength * height`: at least `0x40000` bytes gives a 64-page tile, at least `0x10000` a 16-page
  tile, at least `0x4000` a 4-page tile, otherwise one 4 KB page (page-size code 3, 2, 1, 0);
- rows per tile: tile bytes divided by the tile row length (32, 128, 512 or 2048 rows of 128 bytes; 8, 32, 128 or 512
  rows of 512 bytes);
- tile rows: `ceil(height / rowsPerTile)`, and tile count = tile rows times tiles per row.

The buffer descriptor word the routine writes has the vendor's `CoTlbDescReg` layout: bits 31:21 all ones (the first
slot, filled in by the kernel driver when the level is bound), 20:14 tiles per row, bit 13 set for 512-byte tile
rows, 12:10 the page-size code, bit 9 clear and 8:0 left as they were
([packet-set.md](packet-set.md#texture-table-packets-class-11)). The texels of a level fill tiles row by row: that the
engine addresses texel (x, y) at tile `(y / rowsPerTile) * tilesPerRow + (x * bytesPerTexel / tileRowLength)` and, inside
the tile, row `y mod rowsPerTile` at byte `x * bytesPerTexel mod tileRowLength` is **inferred** from the descriptor
fields and the context's tile bookkeeping (`tileRowLen`, `tileRowInc`, `tileSize`, `tileHeight`); the CPU store
routines were not followed to the address arithmetic, so a model should treat the within-tile order as a choice to confirm
against a guest that reads a texture back.

## How texels get into texture memory

A texture's pages come from the kernel driver (`coAllocTextureTiles@0x6901f0e1`, escape traffic as in
[graphics-engine.md](graphics-engine.md)); the driver then writes the texels through one of three routes, chosen in
`coInitTexImageStore@0x69020110` and `coStoreTexImage@0x690207ca`. The routines were read; the routing flags are
global variables the vendor names and one environment variable sets:

| Route | When | Grade |
|---|---|---|
| **Engine transfer** (`___dmaXferTextures`, default 1) | the source format and type are one the type list above can describe and the pixel-transfer state `coInitTexImageStore` tests allows it (the gates are transfer state, not capability flags); the full condition list was not decoded (the checks seen include an 8-byte multiple for the row and offset, `(x << depthShift) & 7`). Levels of 16 by 16 or less (the common tile) and levels where `width << level` is under 4 skip the engine transfer (`0x69020a7d`, `0x69020a87`, `0x6902089a`). The host image is described as a *linear* buffer (format-word bit 31 set) and a pixel-transfer packet sequence draws it into the tiled level; a rectangle is cut so that it does not cross destination row 4064 (`0xfe0`, `coXferTexImage@0x6902118a`) | inferred (routing, from partly decoded gates), established (the cut) |
| **CPU stores** | the transfer is not possible: the driver converts and copies texels through the CPU's mapping of the pages (UMA); `GLCOTEXLOADSPANMODE` selects span-at-a-time copying | established |
| **Hook** (`___vhdlLoadTextures`, default 0) | emits packet `0x9ed380e0` (`coVhdlTexImage`); a simulation aid, never on by default | established |

A correction to an earlier page: the transfer route is the default (the global is initialised to 1 in the texture manager
start-up, `__glCoInitTextureManager@0x6901c0ff`), and only the hook route is off. The hardware therefore has to read linear
host memory as a texture and write a tiled one, and to convert 16 and 32-bit and YCrCb formats on the way with its colour
matrix. A model that implements texturing from tiled levels only still has to honour the linear flag for uploads.

## Limits

| Item | Value | Where | Grade |
|---|---|---|---|
| Largest level, either dimension | 4096 | `gc+0x25b4` set in `__glCoCreateContext@0x69019596` | established |
| Number of mipmap levels | 13 (`maxMipMapLevel`, `gc+0x25bc`) | same | established |
| Texture units | 1 (`maxActiveTextures`, `numberOfTextureEnvs`) | same | established |
| Border | supported: stored in the level, the map size field excludes it | `coComputeTexLevelSize` | established |
| Non power of two | not supported; the size fields are log2 | the log2 helper `0x69075e5a` is used for both | inferred |
| Three-dimensional textures | none (no wrap R, no depth) | | established |

## What the engine does with the words

Everything above is what the driver writes. What the engine does with it is **unknown**, with these routes tried: the
OpenGL driver in both builds, the display driver's YUV and pattern paths, the vendor's types, the white paper (features
only). The texel addressing inside a tile, the order of channels in types 4 and 5, the filter arithmetic and its rounding,
the level selection, the meaning of `lodFracSize` and the colour-matrix conversion during an upload are unknown.
[questions/open-questions.md](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle)
lists each with the next route.
