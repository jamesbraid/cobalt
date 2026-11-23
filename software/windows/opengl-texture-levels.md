# How the OpenGL driver lays out a texture: mip levels, small levels and texel order

What the NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4 does to give the engine the levels of one texture, how it fills the
smallest levels, and what it shows (and does not show) about the order of texels inside a tile. It extends
[textures.md](../../devices/cobalt-graphics/textures.md) (the words the driver sends) and
[packet-set.md](../../devices/cobalt-graphics/packet-set.md#texture-table-packets-class-11) (the table packets) and corrects
one claim in them; read those first. How the driver is built is in [nt-opengl-icd.md](nt-opengl-icd.md).

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)). **Established**: read in disassembly
(the decompile of the typed project only navigated), cited `sgcoicd.dll@address` or `sgcomp.sys@address`. **Inferred**: a
reasoned reading, with the reasoning. **Unknown**: nothing shipped or published decides it. Nothing in either driver reads a
texel or a descriptor back, so everything said about what the *engine* does is inferred at best. Where a value is for an
emulator author to choose, it is labelled a choice.

Read in disassembly: the descriptor loop and stores of `coBindTexture@0x6901cd56`, `__glCoGetTlbDescRange@0x69018688`,
`CoBuildUserTexTlbPacket@sgcomp.sys 0x17a46` (whole), `coAllocCommonLevel@0x6901efde` (whole), the offset loads, row stride
and staging-image allocation of `coUpdateCommonTile@0x6901ec68`, the format selection of `coInitTexImageStore@0x69020110`, the
format-word build of `coSetTexParam@0x6901df5f`, and the call sites of `__glCoExtEscape` (escape codes and sizes). Read from the
decompile only, so inferred and **Unverified** in the details: the rest of `coBindTexture`, `coCreateTexBindPkt@0x6901d422`,
`coAllocTextureTiles@0x6901f0e1`, `coStoreTexImage@0x690207ca` and `coXferTexImage@0x6902118a`. The two data tables are read
from the file.

## Question 4: how the engine finds level k

**What the driver sends.** The texture word names **one** descriptor id (`0x93cc0080`, word 1, bits 24:19). It is the
bind record's `baseId`, which `coBindTexture` sets from the descriptor it gives to level 0 (`@0x6901d082`, whole loop read).
Everything else is arranged around that id:

- *One descriptor per size, consecutive ids.* The bind record keeps nine slots, indexed by `12 - log2(longer side)` for sizes
  from 4096 down to 32, and one more slot (index 8) for every level whose longer side is 16 or less. The loop walks the
  levels from level 0 upward. For each level that exists and whose slot is still empty it copies the level's six-dword
  descriptor record into the next entry of a run of descriptor-table entries, stores that entry's id in the slot, and adds
  one to the id (`@0x6901d02a`..`@0x6901d107`). So the ids are consecutive in level order, one per distinct size, and all
  levels of 16 by 16 and below share the last one. Established.
- *The run is allocated as a whole.* `__glCoGetTlbDescRange` looks for as many free consecutive entries as the texture needs
  inside ids `0x0c` to `0x3f` (the first twelve are never given to textures; a free entry is marked `-2`) and
  `coBindTexture` fails the bind if there is no such run. Established.
- *The kernel loads the run as one packet.* The prebuilt bind packet starts with a chain header, then a class `11`
  header whose position is `0x1800 + first descriptor id` and whose count is the number of descriptors, then one dword per
  descriptor: the level's descriptor bits with its first translation-table slot in 31:21. The slots are consecutive: the
  texture occupies slots `0x800 - total` upward, and each descriptor's first slot is the previous one's plus its tile
  count. Translation-table entries follow in runs of at most 64, in the same order
  (`CoBuildUserTexTlbPacket@sgcomp.sys 0x17a46`, read in disassembly). Established.
- *A level with no tiles is skipped, not left as a hole.* The ids stay consecutive over the levels that exist. A texture with
  a missing middle level would therefore have its later levels one id early. The driver does not guard against it for a new
  bind (it only checks consecutive ids when it re-uses an existing bind). Whether a texture with a missing middle level can
  reach a bind was not traced. Inferred.

**What it implies for the engine.** Inferred, from the word carrying only the base id and from the descriptors being
consecutive:

1. Level k has descriptor `id + k` while level k's longer side is 32 or more.
2. The first level whose longer side is 16 or less, and every smaller level, lives in the descriptor after the last big one,
   `id + kc`, with `kc` the number of levels whose longer side is 32 or more. The engine can compute `kc` and every level's
   size from the mode word's map width and height (log2 of the level-0 sizes less twice the border).
3. Inside that shared descriptor each small level has its own start row, found from its longer side (next section).

An engine that instead took level k as an offset inside one buffer would contradict the per-level descriptors the driver
builds (they carry their own `tilesPerRow` and tile size), so that alternative is not supported by anything shipped.

**Choice for an emulator** (an approximation, rated as such where the model records it): level k from descriptor
`id + min(k, kc)`, with the common-tile row rule below for `k >= kc`.

**Not decided.** Whether the engine clamps an out-of-range level number against the lod word's maximum before or after the
add; what it does with a bordered level (the equality tests that route a level to the shared tile use the level record's
width and height, and whether those include the border was not traced).

## The shared tile for small levels

`coAllocCommonLevel` gives every texture one extra level record for the small levels: one 4 KB page, tile row 128 bytes
(descriptor bits: first slot all ones, `tilesPerRow` 1, tile width bit 0, page-size code 0), so the tile is **128 bytes by 32
rows**. The driver keeps a 4 KB host staging image of it, allocated with the context's allocator (a request of `0x1fff` bytes,
aligned up to a page). `coUpdateCommonTile` fills the staging image and then sends it to the engine; nothing here is a store
into the engine's memory.

How a level is placed (established, loop and tables read; the offset table is the 32-bit values at `sgcoicd.dll@0x690f38f0`):

| Longer side of the level | 16 | 8 | 4 | 2 | 1 |
|---|---:|---:|---:|---:|---:|
| Byte offset of the level in the staging image | 0 | 2048 | 3072 | 3584 | 3840 |
| Start row (offset / 128) | 0 | 16 | 24 | 28 | 30 |

The start row is `32 - 2 * longer side`. Level rows are 128 bytes apart: texel (x, y) of the level is at
`offset + 128 * y + x * bytesPerTexel`. A level row shorter than 4 bytes (a 1 or 2 texel row of 8-bit texels) is first
repeated until it fills 4 bytes, so a 1 by 1 level of 8 bits is four equal bytes. The routine runs when a level of 16 by 16
or less changes and sends the whole staged page in one engine upload (next section), 128 bytes by 32 rows, with the source
description of the level it was triggered by.

This layout is the driver's own arrangement of the staged page; the engine's addressing of a small level is whatever
turns `(descriptor, level)` into those rows, so the table **is** the contract the engine has to meet for small levels.
Inferred (a different engine rule would sample the wrong rows of a texture the driver filled this way).

## Question 5: texel order inside a tile, and tile order in a level

**What the driver shows: nothing at tile level (inferred, a negative over the routines listed above).** No texture routine read forms an address inside a tile.
Every path that puts texels into texture memory builds a *host* image and has the engine copy it:

- the default route (`___dmaXferTextures` is 1) describes the host image as a linear buffer and sends it through the
  pixel-transfer sequence ([opengl-pixel-transfers.md](opengl-pixel-transfers.md)); `coXferTexImage` only splits a rectangle
  that would cross destination row 4064 (`0xfe0`) into two;
- the route when that switch is off (`coStoreTexImage`, `coInitTexImageStore`) converts into the level's own host copy (the
  level record's pointer at `+0x18`, or a temporary) and then either runs the same transfer or, for the third route (the
  `___vhdlLoadTextures` hook, off by default), sends packet `0x9ed380e0`; it writes nothing into engine-visible pages;
- the small-level staging above ends in the same transfer.

**Correction.** [textures.md](../../devices/cobalt-graphics/textures.md#how-texels-get-into-texture-memory) and
[draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md#pixel-transfers-and-textures) said the CPU-stores route
copies texels through the CPU's mapping of the tile pages. That is not what the code does. `coAllocTextureTiles` (escape
`0x53`, `CREATE_BUFFER`) returns an id, a tile count and a bank flag, never an address, and none of the texture routines read
maps or addresses tile pages. Those two pages are changed to say so. (The OpenGL driver as a whole was not searched for a
CPU window use; the display driver draws through the windows.)

So the order of texels inside a tile is not observable from this driver's texture side, and the same holds for the order of
tiles in a level: the driver only counts them (`tilesPerRow` times tile rows) and hands the kernel a list.

**What fixes it anyway.** The CPU aperture windows are the one CPU-visible view of a tiled buffer: the display driver draws
into the screen through them, so for a buffer the engine and the CPU share, the window's raster arrangement is a software
contract ([graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md#cpu-aperture-windows-at-0x80000000)). That
page already infers the arrangement: tile column `x / tileRowBytes`, tile row `y / rowsPerTile`, slot `first + tileRow *
tilesPerRow + tileColumn`, row-major rows of `tileRowBytes` inside the tile. Textures and colour buffers use the same
descriptor and table mechanism, and the common tile's staged page above is 128-byte rows in row order, so the same
arrangement is the consistent reading for textures. Inferred.

**Choice for an emulator.** Row-major tiles (slot `first + ty * tilesPerRow + tx`), row-major rows inside a tile (row `r` at
`r * tileRowBytes`, `tileRowBytes` 128 or 512), texel `x` at `x * bytesPerTexel`. Any other consistent order is
unobservable to SGI's drivers. A guest that reads a texture's pages by CPU (none found) would be the first test.
Approximation; the discriminating check is the screen path: a window drawn by the engine and read back through the aperture.

## What stays unknown

- How the engine handles an out-of-range level number or a level between `kc` and the top when `kc` is zero (every level of a
  texture of 16 by 16 or less is in the shared tile).
- Border storage: the descriptor holds the border texels inside the level; whether the engine's addressing offsets by the
  border is not shown.
- The descriptor's tile geometry for the shared tile (`tilesPerRow` 1) against a texture whose big levels are wider than one tile
  (each level has its own descriptor, so each is consistent by itself).

Recorded as emulator choices in
[questions/open-questions.md](../../questions/open-questions.md#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle).
