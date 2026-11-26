# The swap's tile flip and the translation-table loads it causes

What the miniport does to the translation table when a double-buffered window swaps, how the drawable's list of flipped tiles
is made, and how to read the class-11 table-load packets an engine trace shows beside a swap. The earlier swap page described a
default swap as an engine copy only and the tile flip as a separate, registry-selected mode; the code does both in one swap.

Source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 (patch SG0020054): `sgcomp.sys` and `sgcodd.dll` read with the vendor's
symbols and types (`_GRMDRAWABLEINFO` and `CO_TILELIST` are the vendor's). The Windows 2000 display driver has the same
`pCoGrmUpdateDrawable` and the same defaults for the three flags below (compared by address of the flags and of the compares).
Addresses are image addresses. Grades as in [AGENTS.md](../../AGENTS.md).

## Answers

| Question | Answer | Grade |
|---|---|---|
| What are the class-11 writes beside an NT 4.0 swap? | Translation-table entries (not descriptors) of the front and back global buffers, reloaded because a tile flip exchanged some of them. | established for the driver side, inferred for the trace reading |
| Tile flip, source and destination descriptors, or damaged tiles? | A tile flip of every tile wholly inside the window, plus an engine copy from the back buffer (descriptor 3) to the front buffer (descriptor 2) of the rest of the window's area. The copy rectangles are not "damaged tiles"; they are the window's copy region. | established for the flip and the copy, inferred for the choice of tiles |
| Does the driver ever grow the entries? | No tile is added to a global buffer after the mode is set. The descriptor and table loads are re-sent whenever the set of bound buffers changes or a flip marks a buffer. | inferred |

## What a swap does

**The flip. Established** (`asFlipTiles@0x1c08c`, read in full). It takes a list of tile indices and a count from the drawable. The
buffers it works on are the device's global buffer records for descriptor ids 2 and 3 (front and back), or 7 and 8 for the
right eye when stereo is on. For each listed tile it:

1. writes the back buffer's frame number for that tile (the tile record's `tileAndBank` shifted right by 4, a 16-bit entry) into the display's
   alternate tile list, the list the display switches to at the next vertical blank
   ([display-engine.md](../../devices/cobalt-graphics/display-engine.md));
2. exchanges the tile's record between the front and back buffers, so that the front buffer's translation-table entry for that tile
   now holds the frame that used to be the back buffer's and the reverse.

Afterwards it counts the flip, raises a table-changed counter in the device, and sets the reload byte (`+0x18`) of both buffer records, and of
the right-eye pair in stereo. A buffer whose reload byte is set has its translation-table entries re-sent the next time the miniport
loads a node (the context restore, [`pCoContextSwitch`](../../devices/cobalt-graphics/packet-set.md#texture-table-packets-class-11)). So after
a flip the engine's front table and the display's tile list agree on the new front, and no pixel of a flipped tile is copied.

**Who calls it. Established.**

- `CoMoveChunksFromRetraceToSerializedQueue@0x17d12`, for every chunk held for retrace (the default swap, drop type 4). Per chunk it calls the flip
  when the drawable's tile count (`_GRMDRAWABLEINFO.numTiles`, `+0x84`) is non-zero, the drop's flag value 2 is set (the same flag that
  asks `CoScheduleSwap` to copy the copy region) and the drawable is not lost (`bLostSurface`, `+0x68`, is 0). The chunk then goes to the
  serialised queue, where its copy nodes run.
- `CoScheduleUnsyncedSwap@0x194be` for drop type 9 (`SwapVertRetrace` 0), with the same two conditions. With the overlay and one node it exchanges the whole front
  and back lists instead (`asFlipFrontBack@0x1c220`).

So the default swap is the flip and the copy together, not a copy alone. The note in [the escapes](opengl-escapes.md#the-table) that a swap through
escape `0x48` "is a copy, not a flip" is wrong for the default drop type: the miniport flips the whole tiles when it releases the held chunk.

**Which tiles. Established for the code that picks them, inferred for what it means** (`pCoGrmUpdateDrawable@0x1ccde`, partly read). The display driver
fills the drawable's `rgnCopy` (`+0x70`, the copy region) and `tileList` (`+0x88`, 256 byte-sized tile indices) when the window's clip changes. For
each tile of the screen buffer it calls `coRectIn@0x1c6c8` against the window's clip region. That routine returns 1 for no overlap, 2 for a
partial overlap and 0 when the tile's rectangle lies wholly inside the clip (read in full). With `bFlipTiles` set (default 1), a tile with result 0 goes on
`tileList`; a tile with result 2 contributes its intersection with the clip to `rgnCopy`; a tile with result 1 contributes nothing. `bFlipAllTiles` (default 0) puts partial tiles on
the list too, and `bFlipScrTiles` (default 0) is a third variant for the screen. The 2D escape `0x11` code 3 sets `bFlipTiles`
([escapes](opengl-escapes.md#private-escapes-and-the-other-entry-points)); the OpenGL driver never sends it, so the default stays.

**The copy. Established.** `CoScheduleSwap@0x1934a` runs `CoCopyRect@0x176ae` over `rgnCopy`: draw buffer = the front (descriptor 2), texture = the back (descriptor 3),
mode `0xc0000`, bias 0.5, window parameters 0, one primitive of type 4 per rectangle with the texture coordinates equal to the pixel position. When
flag value 4 is set it copies the clip region as well. A window whose tiles are all wholly inside it has an empty `rgnCopy`
and needs no copy.

## Reading the table-load words in a trace

A class-11 header is `0xc0000000 | position << 16 | count`. The labels `0x680`, `0x682`, `0x683`, `0x686` and `0x6c0`, `0x6c1`, `0x6bf` that an engine trace
shows are bits 31:21 of that header (the "A" field of the other classes). For a translation-table load, position `0x1000 + slot`
gives `A = 0x680 + (slot >> 5)`, one label per 32 slots; for a descriptor load, position `0x1800 + id` gives `A = 0x6c0 + (id >> 5)`.
So `0x680` is a load starting in slots 0 to 31, `0x682` in 64 to 95, `0x683` in 96 to 127, `0x684` in 128 to 159, `0x686` in 192 to 223 and `0x6bf` in 2016 to 2047,
and `0x6c0` and `0x6c1` are descriptor loads for ids 0 to 31 and 32 to 63. A label says where a load starts, not how long it is (the trace's `x21` suffix is the dword count).
Established as arithmetic on the builders' header formula; checked against the three loads of one NT 4.0 swap, below.

The three loads of one NT 4.0 swap in an emulator trace are headers `0xd0100040`, `0xd0500010` and `0xd0600014`: 64 entries from slot 16, 16 from slot 80, 20 from
slot 96, labels `0x680`, `0x682`, `0x683`. They carry the front buffer's 80 entries (16 to 95) and the back buffer's 20 (96 to 115); the entries are
the same frames as before except tile 6 (the tile at pixels 128 to 255 in both axes of a five-tile-wide buffer), whose entries are
exchanged between the two buffers and alternate between two values on successive swaps. That is the flip above at work on the one tile wholly inside a window of
320 by 240 pixels at (24, 43). Inferred (a model run, not hardware).

## Mapping a window's back and depth buffers

**There is no per-window back or depth buffer. Established.** The back buffer is the global buffer with id 3 and the depth buffer the one with id 4, both
allocated for the whole screen when the mode is set
(`pCoGrmInitializeGdi@0x1c9fe` calls `CoGrmAllocateTiledBuffer`; the ICD's `CREATE_BUFFER` is used for textures and pixel buffers only). A window's pixels are the same
coordinates in those buffers. The descriptor ids are 2 front, 3 back, 4 depth, 5 and 6 overlay and its back, 7 to 9 the right-eye buffers and 10 the clip-ID plane
([clip page](engine-clip-degenerate-and-line-ends.md)); a context's `SETCONTEXT` names them by id.

**Slots. Inferred (from the trace above).** Slots run in descriptor order after the packet pool's 16: front 16 to 95 (80 tiles, so the front's table is taller than the screen: it
covers the off-screen heap of the 2D driver), back 96 to 115. Depth would follow from 116, which puts its first group in `0x683` and the rest in `0x684`. Which buffer `0x686`
(slots 192 to 223) belongs to cannot be said without the descriptor words: it lies after the depth buffer, so it is a later buffer or texture, not the back buffer.

**When the driver changes them. Inferred.** No driver routine adds tiles to a bound buffer or resizes it in place. The loads are re-sent when: a node is loaded
after `BIND_BUFFERS` or `UNBIND_BUFFERS` or `SETCONTEXT` (descriptors from the node's bound set, with each descriptor's first slot the running sum of the earlier
buffers' counts: `pCoMakeTlbDesc@0x170a2`), after a flip marks a buffer, or when a texture is created (a prebuilt table packet from `CREATE_TEXTLBPKT`, the `0x6bf` group at the top
of the table).  In the Quake II trace of the same emulator run, the texture binds load descriptor group `0x6c0` and slots 2016 to 2047 (`0x6bf`).

## What happens to a draw into a tile with no table entry

**Unknown: no shipped software depends on it.** The drivers load an entry before any draw that uses it, and the OpenGL driver sends no draw by design whose pixels fall outside the
descriptor's tile rows ([clip page](engine-clip-degenerate-and-line-ends.md#5-do-fills-and-clears-ever-run-past-the-descriptors-tile-columns)). A draw into an unmapped tile in an emulator trace
therefore most likely means the emulator's table is behind the guest's: a reload not applied (a flip's reload byte, a bind) or a descriptor not yet loaded. That reading is inferred.
An emulator choice: drop the pixels and log it, as the model does. Not searched: a driver path that sends a draw before its table load, in either Windows build.
