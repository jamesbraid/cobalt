# Cobalt rendering engine: register window, packet queue, interrupts

How software feeds the Cobalt rendering engine. Back to the [overview](README.md). Everything here comes from one
place, the Windows NT 4.0 miniport `sgcomp.sys` v5.2.4 and its symbols, read function by function; no public document
describes this block and Linux never drove it (SGI's hardware OpenGL for Linux was a 1999 demonstration that was not
released, per SGI engineers' posts in comp.sys.sgi.admin, 2004). Where a register is named below the name is mine unless
quoted as a symbol. Grades: see the overview.

**Short answer to "FIFO or DMA".** The engine fetches commands by DMA from packet buffers in main memory. The host
builds packets in memory (the memory is the same SDRAM the processors use), links them, and tells the engine which
packet position to run from through one control register; completion and stalls come back as two interrupt causes. There is no
register FIFO that the host pushes words into. Inferred (driver behaviour). The header-word count field and the chaining packet are now known ([below](#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)); the opcode set is only partly known (the constant headers are [listed below](#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)).

## Register window at `0xc8000000` (`0x19000` bytes)

`CobaltInitialize` maps exactly this window (`MmMapIoSpace`, uncached, `0xc8000000`, `0x19000` bytes) before it looks
at the display back end's ID. Accesses seen are 32-bit. Offsets are from the base.

| Offset | Seen as | What software does | Grade |
|---|---|---|---|
| `0x00000` to `0x07fff` | nothing | the miniport never touches this range in the routines read | unknown |
| `0x08000` to `0x0bfff` | 2048 entries of 8 bytes | the translation table. `pCoInitializePacketPool`, `CoInitHw`, the tile and texture allocators and `CoBuildTlbPacket` write the low dword of an entry with a 24-bit value (`tileAndBank & 0xffffff`, the first 4 KB page frame of a tile); entry `i` is at `0x8000 + 8*i`. A table entry is a tile, not a 4 KB page: the packet pool is 16 tiles of 256 pages (1 MB) each (`cgb_ntiles` 16, `cgb_pagespertile` `0x100`, the `CGBUFFER` record `pCoInitializePacketPool` fills), so `CoInitHw` loads 16 entries, and the tile size of every other buffer comes from its descriptor (below). `pCoInitializePacketPool` first sets all 2048 entries to the pool's first tile and `CoInitHw` loads the pool's tiles into entries 0 to 15; `CobaltStartIO` (video IOCTL `0x230458`) loads the global buffer `GlobalBuffers[2]` from entry 32 and sets every later entry up to `0x7ff` to the first pool tile, so entries not in use hold the value of the first. A packet node is therefore named by a position in the pool, not by a physical address | inferred (access and reading) (a translation table from tile numbers to memory) |
| `0x0c000` to `0x0c1ff` | 64 entries of 8 bytes | buffer descriptors: entry `k` describes the buffer whose descriptor id is `k`, the id the colour, depth and texture format words carry; each is the register file's `buffer_desc` ([packet-set.md](packet-set.md#texture-table-packets-class-11)). `pCoInitializePacketPool` (before `CoInitHw`) writes all 64 entries with the packet pool's descriptor, `CoInitHw` entry 0 again, and `CobaltStartIO` writes entry 2 as `(GlobalBuffers[2]->cgb_tlbdesc & 0x1fffff) \| 0x4000000` (slot 32) when it maps the global buffer; entries for later buffers are loaded by class `11` packets at position `0x1800 + k`. An earlier reading, "every entry gets the physical base of the context packet pool", is withdrawn: start-up writes every entry with the pool's descriptor, not a base | inferred (access and reading) |
| `0x10000` | start and position | write: `idle_wait \| (position << 12) \| 0x800`, where the idle count is the adapter field the vendor names `pktIdleCount` (set to 2 once, in `CoInitHw`, and never changed) and the position is a node position in 32-byte units from the pool base (`pCoContextSwitch@0x169ce`..`0x169d6`, the event routine at `@0x1813c`); `CoInitHw` writes `0xfffff040` (position `0xfffff`, bit 6, bit 11 clear). Read: bits `[31:12]` are the node position the engine is at; the stop-event routine shifts right 12, tests it with `CoIsUserPktOffset` (a position outside the node's two context-switch page ranges) and, when it reads 0, uses the chain register `0x185c0` instead. An earlier reading made this field a context number; it is a packet position (the saved value is the rendering node's `savedPktOffset`, `+0x108`) | inferred |
| `0x10008` | doorbell | write `0x400` from three routines, under different conditions. `CoSetChunk@0x190e3` (work appended to a node whose `flags` bit 1 is set, read here as "not deferred": the flag is unnamed, so that meaning is inferred) and `CoProcessTimeoutEvent@0x182f8` (timer event, no re-idle pending) write it when the current queue (`currentQueue`) is 2, a node is in the hardware (`inHwRNode`) and that node's `chunkCount` is 0. `CoSuspendRNode@0x18e0c` does not test the queue or the node in hardware: it writes it when the node's status (`nodeStatus`) has bits 3:1 clear, its `chunkCount` is below 1, status bit 9 is clear and the status byte above bit 7 is non-zero. An earlier reading joined the three under "the scheduler is in its second mode", which names the `currentQueue` value, not the registry-selected `schedulerMode` | inferred |
| `0x10018` | interrupt cause | `CoInterruptHandler` reads it (zero is "not mine"), masks the value with `0x60000000`, writes 0 and re-reads until the masked bits differ from what it read. `pCoContextSwitch@0x16951` polls bit 31 until it reads set and then writes `0x7fffffff` (only when another context was running when the switch began). `CoInitHw` writes 0. `pCoWaitForEvent` does not touch it, as an earlier version said: it waits on a serial number stored by the stop event | inferred |
| `0x10020` | interrupt enable | `CoInitHw` writes `0x60000000` (both causes on) | inferred |
| `0x18138`, `0x18140` | read | read into the packet being built after a header word `0x84e44040`, which is the scissor box packet the OpenGL driver writes ([packet-set.md](packet-set.md)): the two registers are the scissor minimum and maximum corner. `CoBuildWinparamsPacket` does this only when its restart argument is set, that is when the incoming node is the one already in the hardware (`local_8 == 0` in `pCoContextSwitch`) and the window's `drawIncarn` differs from the node's (the call at `0x166c9` is behind the compare at `0x1669b`); on a real switch the packet is built without the read and the engine's own dump (through `0x18800`) is the only save. An earlier version said the pair was saved and restored across a context switch, and an earlier one read the two as instrument counters; both are withdrawn | inferred (header `A` = `0x427` maps to this offset; the miniport and the ICD agree on the packet) |
| `0x185c0`, `0x185c8` | read | on a stop interrupt (whatever the cause): `((R185c0 & 0xfff) << 12 \| (R185c8 & 0xfff))` is a 24-bit sequence number and `R185c8 >> 12` the node position; if bit 23 of the sequence is set the driver keeps its low 20 bits in the rendering node's `serialOnRemove`, otherwise it reads dword 3 of the node at that position (the sequence number the ICD stored there) and keeps its low 20 bits (both the OpenGL driver's `DispatchCurrentPkt` and the display driver's `pulGetPacket` put `0x800000` in the sequence word of every chain packet they write, so that second path is not taken for their chains); a zero node position only counts a stop (`idle_wait` is bumped, otherwise it is cleared) (`CoProcessCobaltEvent@0x17f88`..`0x18027`). These are the two registers the chain packet `0x97000040` writes (next node, this node); `pCoContextSwitch@0x169bc` writes 0 to `0x185c8` when the incoming node is already the one in hardware (a restart with no switch) | inferred |
| `0x185d0` | read, write | the current-texture id: the third word of the long chain packet `0x97000060` carries the rendering node's `currentTexId` (`pCoContextSwitch@0x167c3`), a stop of an OpenGL node (`gfxType` 1) reads it back into that field (`@0x17fd2`), `CoScheduleBindBuffers` writes it, and the OpenGL driver writes 0 with packet `0x97400020` before it frees the pages of the texture that is bound. An earlier name for it was the current-context packet pointer | inferred |
| `0x18800` | write | the outgoing context's dump offset (the rendering node's `cxswDumpOffset`, `+0x500`, a position in the context-switch packet page), written when another context is started while one was running (`pCoContextSwitch@0x16666`), before the poll of bit 31 of `0x10018`. The driver treats it as a request to save the outgoing context into the memory at that position and then waits for bit 31 of `0x10018` (the saved-state paragraph above). What the engine stores there is unknown | inferred (access and expectation), unknown (contents) |
| `0x19000` end | | | |

Reset values are unknown for all of them. The retrace and engine handlers are the only paths that read status, and both
treat a read of zero as "not mine".

The display back end also uses `0x10000`, `0x10018` and `0x10020` in its own block, at `0xd0000000` (see
[display-engine.md](display-engine.md)). The two windows share offsets, not a register file: the engine's are at
`0xc8010000` and up.

## CPU aperture windows at `0x80000000`

The NT 4.0 miniport reaches buffers through a second set of physical windows, one per buffer descriptor id, apart from the
register window above. Everything below is the miniport `sgcomp.sys` 5.2.4 (April 1999 build); the two NT 4.0 HALs, the Windows 2000 HAL and the PROM
contain nothing like it, and the Windows 2000 frame buffer drivers take a RAM base from the registry instead
([w2k-display-drivers.md](../../software/windows/w2k-display-drivers.md)). Grades: the driver's behaviour is established where the whole routine was read; what
the hardware does with these addresses is inferred, because no document describes them.

**What the driver maps. Established.** `pCoInitializePacketPool` (the routine that also fills the 2048-entry table above) makes four
`MmMapIoSpace` calls of `0x1000000` (16 MiB) each, in this order, at physical `0x80000000`, `0x82000000`, `0x85000000` and `0x86000000`
(`sgcomp.sys@0x15386`, `0x153de`, `0x1543c`, `0x1549a`). Each is first mapped with a third argument of 2 (inferred to be the NT 4.0 kernel's frame-buffer caching type, a write-combining
request on P6 parts; not checked against the kernel) and, if that returns null, again with 0 (uncached); if both fail the routine fails and the engine is not brought up. The
earlier statement here and in [the driver-floor page](framebuffer-driver-floor.md) that the fourth window is at `0x87000000` was a
misreading: the immediate is `0x86000000`. The window address is `0x80000000 + (descriptor id << 24)`, and the ids are the ones the
display driver sets in `bInitializeModeFields` ([2d-blit-video.md](2d-blit-video.md)):

| Window | Id | Buffer | Kept at |
|---|---|---|---|
| `0x80000000` | 0 | the packet pool (descriptor 0; the pool is bank 0 of the graphics memory, up to 16 tiles of 1 MiB) | pool record `+8` |
| `0x82000000` | 2 | the front buffer and GDI's default draw target | device extension `+0xd4c`, reply word `+0x38` |
| `0x85000000` | 5 | the 8-bit overlay plane | device extension `+0xd50`, reply word `+0x3c` |
| `0x86000000` | 6 | the overlay plane's back buffer | device extension `+0xd54`, reply word `+0x40` |

The mapped virtual addresses of ids 2, 5 and 6 are copied into the miniport's global-information reply (`CobaltStartIO@0x13272`..`0x1328d`),
which is how the display and OpenGL drivers get them. Id 3, the colour back buffer, is not mapped. The miniport itself addresses the
pool through the id 0 window: its pool addresses are `0x80000000` plus a byte offset, and the routine at `0x150b0` converts one back
to a page of the mapped window and zeroes it.

**The frame buffer the video port hands out is the id 2 window. Established.** The standard map-video-memory control code (`0x230458`,
handler at `0x12b56`) first writes buffer descriptor 2 (`+0xc010`) with the buffer record's descriptor bits OR `0x4000000`, which is
first slot 32; loads the translation table from slot 32 with one 24-bit value (the tile's first 4 KB frame number) per tile of the
screen buffer; fills every later slot up to `0x7ff` with the pool's first tile; and then calls `VideoPortMapMemory` on physical
`0x82000000`, high dword 0, with the P6-cache space flag (8) and a length of the mode's `VideoMemoryBitmapHeight` times its
`ScreenStride`. The returned address is both `VideoRamBase` and `FrameBufferBase` of the standard reply. The share-video-memory code
(`0x230474`, `0x12cec`) maps a sub-range of the same physical window into user mode (flags `0xa`). So text-mode setup (which uses this
miniport as its display driver; `TXTSETUP.SIF` lists `sgcomp.sys` for display) and the display driver draw into physical
`0x82000000` and above, not into RAM.

**The window's address format. Established as the driver's expectation.** The mode records have `ScreenStride` fixed at `0x2000`
(8192 bytes, stored at `0x1aca6`, whatever the width) and a bitmap height of at most `0x800` lines (`0x1ad2e`..`0x1ad3f`), which is a
16 MiB window of 2048 lines. A routine in the interface table the miniport hands out (`0x19ed0`, table slot at `0x1ed3c`) decodes a
mapped address of the id 2 window by asking the kernel for its physical address and, if that lies in `0x82000000` to `0x82ffffff`, splits
it into three fields: bits 29:24 the descriptor id, bits 12:0 the byte within the line, bits 23:13 the line number. So the aperture
address of pixel byte `(x, y)` is `0x80000000 | id << 24 | y << 13 | x`.

**What the hardware does with it. Inferred.** The window is a linear raster view of a tiled buffer, with the line stride fixed at
8192 bytes whatever the buffer's real row length. The converting agent has to be the engine's own address translation, the one
the table (`+0x8000`) and descriptors (`+0xc000`) belong to, because the driver loads only those two structures and writes nothing
else that describes the window. The conversion that fits every observed field is: tile column `x >> 9` (512-byte tiles, or `x >> 7` for
128-byte ones), tile row `y >> 7`, table slot `firstSlot + tile row * tilesPerRow + tile column` (the descriptor's fields,
[packet-set.md](packet-set.md#texture-table-packets-class-11)), frame number from the table entry, and inside a 64 KiB tile
512-byte rows of 128 lines. Whether the hardware indexes this way or by 8 KiB steps of the window, what a read of a line beyond the
buffer's row length returns, and whether the window is also usable through any other physical range are not settled by any software we
hold. Neither the memory-controller block (`0xc6000000`) nor the Lithium windows are involved as far as any binary shows: the miniport
touches no register of the first and the second decode `0xf0000000` upward.

**Register-write count that supports this reading. Inferred (arithmetic, not a trace).** One start-up of the miniport writes the
window at `0xc8000000` as follows: 64 descriptor entries and 2048 table entries in `pCoInitializePacketPool`, N table entries
(N is the pool's tile count: the bank's size in MiB, capped at 16 at `0x15206`), descriptor 0 and three control words in `CoInitHw`,
and 2016 table entries plus descriptor 2 in the map-video-memory handler: 4133 + N stores, 4149 for a 16-tile pool (a bank of 16 MiB
or more). The writes into `0x82000000` and above are the frame-buffer clears and glyph drawing of text-mode setup (4-byte fills,
1-byte pixels at 8 bits per pixel; inferred from the modes, not from a trace).

## Interrupts

- **Pin 25 (the "RE" pin): the engine.** `CoInterruptHandler` reads `+0x10018` and returns "not mine" when it reads zero.
  Otherwise it masks the value with `0x60000000`, writes 0, waits for the masked bits to change, and queues an event carrying
  those bits for a deferred routine (`CoDPC`, `CoProcessCobaltEvent`, read in full). The event routine takes the node the
  engine was running. On cause `0x40000000` it sets bit 5 of the node's status (unless the node's own flag word `+0x104` has
  bit 1) and, when the current queue is 1, decrements the node's chunk count. It reads the position from `0x10000` (or
  `0x185c0`) whatever the cause, and asks `CoIsUserPktOffset` whether it lies outside the node's two context-switch page
  ranges. A position inside them is a stop in the state-restore packets: the routine writes `0x10000` back with the same
  position, unlocks and returns (`@0x18128`..`0x1823f`); it wakes nothing and schedules nothing. A user position is saved as the
  node's `savedPktOffset`, the sequence number is recovered from the chain registers ([the register
  table](#register-window-at-0xc8000000-0x19000-bytes)), and the waiters are woken: either cause wakes the waiter of kind 4
  and, when the node has no pending chunks, the first of the kinds `0x10`, `0x20` and `0x40` its wait word names (each also
  sets one bit of the node status); cause `0x40000000` also wakes kind 8. The routine ends by asking the scheduler for the
  next ready node and starting it with `pCoContextSwitch` until one starts. The cause bits gate only status bit 5, the chunk
  decrement and the kind-8 wake; what tells them apart in hardware terms is unknown. The link word's `interrupt` and `sync` bits
  (bits 1 and 2 of a node's link word, the register file's `NODE_LINK`) are what would ask for an interrupt: inferred from the
  names. Inferred (driver behaviour).
- **Pin 24 (the "AS" pin): vertical retrace.** `CoVRetraceHandler` reads the 32-bit register at `0xca000000` and writes
  back its complement. If bit 1 of the value read was set it adds one to `FrameCounter` when stereo is off; in the stereo
  case it adds one only when `StereoField` bit 0 is set, then does a dword load of the display back end's `vt_f2rf_lock`
  (`+0x1001c`) whose value is discarded (`mov ecx,[eax+0x1001c]` at `sgcomp.sys@0x191f2`, overwritten by the next instruction; the Windows 2000 build has it at
  `0x18a2a` and the April 1999 build a `test byte [eax+0x1001c],0` at `0x192d3`), and rewrites `vt_flags` (`+0x10018`)
  with its low six bits and bit 6 set; when the driver's `bFreeRun` flag is set it writes `vt_flags` a second time with bit 6 cleared (`0x19215`).
  The decompile drops the discarded load, so an earlier version of this page said the routine never touches `+0x1001c`; a model must treat that read as an access that
  may have side effects. It then queues a
  retrace event, or counts one more when one is already queued, which the deferred routine uses to run the swaps and queued
  work that were waiting for retrace. Established (the routine read in full in the typed decompile); the hardware's reading is inferred. The
  write-back of the complement clears the bits that were set only if a written 0 clears, so the register is
  write-0-to-clear (inferred); the Cobalt timer status bit is cleared the same way ([chips](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/registers.md), `cpu_stat`).

Both pins are on the Cobalt interrupt controller at `0xc4000000` ([the controller's registers and acknowledge sequence](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md#interrupt-controller-the-cobalt-apic),
[the line assignments](https://github.com/jamesbraid/chips/blob/main/platforms/visws/interrupts.md#cobalt-lines-established-from-the-header) and [how the HAL delivers them](https://github.com/jamesbraid/chips/blob/main/software/visws/nt-hal.md#interrupt-delivery-in-the-hal)
are in the chips repository; the miniport itself never touches the controller). The miniport asks the HAL for the vector of internal
level 25 and of level 24 (`HalGetInterruptVector` with interface type 0, bus 0, level and vector both `0x19` and `0x18`), then connects the engine handler to the first and the retrace
handler to the second with `IoConnectInterrupt`: both are latched (interrupt mode 1), not shared, share one spin lock (`isrSpinLock`), take the larger of the
two request levels as the synchronise level, and save no floating-point state. Established (the call read in the typed decompile).

## Command interface in main memory

All of this is main-memory data the engine reads (UMA), so an emulator must resolve engine reads of physical RAM; the
engine never needs the host's cache.

- **Packet buffers.** The driver keeps a "render node" (RN) per drawing context, each with one or more packet buffers
  allocated by `pCoAllocatePacketPages` and growing by `pCoExpandPacketBuffer`. A single packet may not cross a
  `grmPacketSize` boundary, which is 128 dwords (512 bytes); the dispatcher chains buffers when a packet would.
  Inferred (driver behaviour).
- **Alignment.** Software pads every packet with zero dwords up to a 32-byte boundary before dispatching it
  (`grmDispatchPacket`, `CoSetGrmPktHeader`). Size and position are counted in 32-byte units after that. Inferred.
- **Link word and ownership.** For each packet the driver also writes one dword into a link table (the RN's table, the
  `pktBaseFlushAddress` pointer of its first `pktBufferAllocations` entry, record offset `0x120`) that carries, in units of 32 bytes, the packet's size and the next buffer's position, and sets bit
  0. To wait for the engine to finish with a buffer the driver spins until bit 0 of a header or link dword in memory reads
  clear (`grmDispatchPacket`, the tail of the routine). So bit 0 means "owned by the engine" and the engine clears it.
  The field split of the link word is read from every writer (node layout paragraph below): length in bits 11:3, next position in bits 31:12, flags in bits 2:1. The table is a second view of the packet pages, so the word lands in the node's own first dword.
- **Per-context shadow.** Every new context starts from a 208-dword image whose first 202 dwords come from a table (`CoInitShadowRam` copies the table
  `initialCxShadow`, 19 pairs of header word and dword index, into the RN's packet area) that holds default state. The
  headers, in order of position, are: `0x80003020` (index 0), `0x046400c0` (2), `0x05260480` (9), `0x09a900c0` to
  `0x09a900c3` (`0x2e`, `0x35`, `0x3c`, `0x43`: four packets seven dwords apart), `0x0ca96120` to `0x0ca96123` (`0x4a`, `0x54`,
  `0x5e`, `0x68`: four of ten dwords), `0x1129f020` to `0x1129f023` (`0x72` to `0x78`: four of two), `0x91aa0220` (`0x7a`),
  `0x93cc0340` (`0x8c`), `0x00210440` (`0xa7`), end at `0xd0`. The four-times structure with the low two bits counting
  0 to 3 matches the datasheet's four hardware lights; that is inferred, no source names the packets. Inferred for the table's contents.
- **Header words seen elsewhere.** Packet builders in the driver and in the OpenGL driver store header dwords of the form
  `0x9880_4000`, `0x8000_3020`, `0x84e4_4040`, `0x99ce_00a0`, `0x9865_c020`, `0x9700_0020`, `0x9700_0040`, `0x9700_0060`,
  `0x91aa_0020`, `0x91ea_2040`, `0x93cc_0080`. The low byte tracks the packet's length in 32-byte units times 32
  (`0x20`, `0x40`, `0x60`, `0xa0`, `0x80` seen), and a family (`0x97...`, `0x98...`) repeats with different lengths. So the top
  bits look like an opcode and the low bits a size. Inferred; the headers the ICD writes as constants are listed below, the computed ones are not.
- **Texture-table packets.** `CoBuildTlbPacket` builds a packet that loads engine table entries (the `0x8000` window) from
  a list of memory pages for a texture; `CO_CREATE_TEXTLBPKT`, `CO_FREE_TEXTLBPKT` and `CO_SET_READY_TEXTLBPKT` are the
  commands the OpenGL driver uses to ask for one. Whether the table is walked by the engine or loaded by it from the
  packet is unknown.
- **Host-side scheduler.** The miniport multiplexes contexts in software: per-context work queues ("drops" and "chunks"),
  a round-robin timer, a wait-on-completion timer, an idle timer, a serialised queue that is drained after each
  vertical retrace, and a swap that is scheduled to happen at retrace (`CoScheduleSwap`) or immediately for unsynchronised
  swaps. Registry values `IdleOnOpenGL`, `IdleOnGDI`, `IdleOnGDIwithOGL`, `IdleOnD3D` and `SchedulerMode` tune it. None of
  this is hardware, but it shows what the hardware cannot do: there is no hardware context queue, only one running
  context at a time, started by writing a packet position to `+0x10000`.

**Starting a node: `pCoContextSwitch@0x16610`. Established (read in full); the engine's side is inferred.**
The routine takes the adapter and the incoming rendering node.

1. The node in hardware (`inHwRNode`, `+0xe4`) is the outgoing one, unless it is a GDI node (`gfxType` 4). When there is an
   outgoing node that is not the incoming one, its `cxswDumpOffset` goes to `0x18800`.
2. The incoming node's window parameters are written as a packet in its context-switch page: `CoBuildWinparamsPacket` for an
   OpenGL node, `CoBuildGdiWinparamsPacket` for a GDI node, `CoBuildWinparamsPacketForPxfer` for a pixel-transfer node (flag
   bits `0x28`), and, on a real switch of a node whose `flags` has bit 4 set, when the window's incarnation changed, the texture window
   offsets (`texWinoffsetS` and `texWinoffsetT`, of the read drawable when the node has one, else of the draw drawable) are
   stored into the saved packet image.
3. On any switch (the incoming node differs from the one in hardware, including from an idle engine or a GDI node), a long
   chain packet `0x97000060` is built at the incoming node's slot of the packet pool with payload (resume position, 0, `currentTexId`); the first dword is filled in once the position is known.
4. The translation-table and descriptor packets (`CoBuildTlbPacket`) and the link words (`(next position << 12) | (length
   << 3) | 1`) of the pages are written.
5. After an outgoing node, the routine waits for bit 31 of `0x10018` and writes `0x7fffffff`; when the incoming node is
   already the one in hardware (a restart) it writes 0 to `0x185c8` instead.
6. It starts the engine with `0x10000` = `pktIdleCount | (position << 12) | 0x800`, marks the node's saved position with
   the sentinel `0xbabeface`, clears the adapter's pending flag and, when the incoming node is the resource manager's (`grmRN`, `+0xec`), runs the
   drop it had deferred. Which position it writes depends on whether a state restore was built: the
   node's saved position (`+0x108`), the context-switch dump offset less one, or an offset in the node's own
   context-switch page.


**The context-switch page and the saved state. Established as driver behaviour (`sgcomp.sys@0x14d96`, `@0x16610`, `@0x16a7a`, `@0x17f88`); the engine's side is inferred.**
Each render node owns one 4 KB page of the packet pool, with a node view and a flush view of it (`+0x4e4`, `+0x4e8`; page position `+0x4f0`). Three nodes live in it:

| Page bytes | Node | What it holds | Link word |
|---|---|---|---|
| `0x000` | restore node | the window-parameter packet (and pixel-transfer or GDI variant) built at each switch | next is the node at `0x3e0`, length as built |
| `0x080` header, `0x0a0` to `0x3e0` data | saved-state node | 26 units, 208 dwords: the context's register state as replayable packets (the page is zeroed when the node is created and a 202-dword default table is copied in, `@0x171ec`), with the texture window offsets patched into payload dwords 200 and 201 by the driver (the default table's last header is at index 167 and nothing in the driver writes dword 199, so any header there comes from the engine's save) and the long chain packet at dwords 202 to 205 | next is the resume position, length 26 (`0xd1` plus the position shifted left 12) |
| `0x3e0` header, `0x400` on | table node | the translation-table and descriptor packets for the node's buffers, ending in a short chain packet `0x97000020` | next is the saved-state node |

The dump offset the miniport keeps (`+0x500`) is the position of the first data unit of the saved-state node (page position plus 5). A switch-in starts the engine at the page start, so
it runs the restore node, the table node and the saved-state node and then continues at the resume position; a restart of the node already in the hardware skips all of this and starts at the saved position.

**What the long chain packet does. Established for what is written; the engine's side inferred.** The `0x97000060` packet carries three payload dwords: the resume position shifted left 12, zero, and the render node's current texture id. The packet header class is the
register-write class of the other chain packets, whose payload dwords land in the chain registers `0x185c0`, `0x185c8` and `0x185d0` ([packet-set.md](packet-set.md)), so the long packet leaves the next-position register
holding the resume position, the node-and-serial register zero, and the texture register holding the node's texture id. The miniport rewrites it at every real switch-in (the incoming node differs from the one in the hardware), so whatever an
engine dump left at dwords 202 to 205 does not matter. The resume position comes from the stop handler (`@0x17f88`): on every stop it reads the position in `0x10000` bits 31:12, falling back to the chain register when that is zero; a position inside one of the
node's own context-switch pages makes the handler restart the engine at once at the same position, and any other position is saved as the node's resume position. A node never yet stopped resumes at the first buffer its context allocated.
Inferred reason for the zero second dword: the stop handler takes the node position from the second register and, finding zero, only counts a stop and records no serial, so an engine that stops right after a switch-in is not mistaken for finishing a node;
the restart path writes 0 to `0x185c8` for the same reason. The ordinary chain packet written by the OpenGL driver puts the next node position (and serial bits `19:12` with `0x800` set) in the first dword and the node's own position and serial bits `11:0` in the second;
the stop handler rebuilds a 24-bit serial from those two registers.
In every chain I read, the position in the chain packet is the same node as the `next` field of the link word of the node that holds it (OpenGL driver nodes, the miniport's table node and saved-state node), so a model that follows the link word and treats the chain packet only as three register writes
reaches the same node as one that jumps on the chain value. Whether the engine does either, or both, is unknown.

**What a write to `0x18800` does. The driver expects a save; the engine's behaviour is inferred.** On a real switch the miniport writes the outgoing node's dump offset (a memory position, not a flag) to `0x18800` (`@0x16666`). It then builds everything for the incoming node in memory first (restore node `@0x166c9` or `@0x166e8`, table node `@0x16887`, the saved-state link and the long chain rewrite, the link stores `@0x168d5`, `@0x16904`, `@0x16934`).
Only after that does it poll bit 31 of `0x10018` (`@0x16951`), write `0x7fffffff` there, clear `0x185c8` for a restart (`@0x169bc`) and write the start word (`@0x169d6`). So the poll gates the start-word write, not the building. From the driver's side the write is a request for the engine to save the outgoing context into the memory at that position, for these reasons.
The offset points at the saved-state node, whose contents are replayable register packets; the miniport reads no engine register to save state on a switch (the scissor pair `0x18138`, `0x18140` is read only on a restart that skips the dump);
the saved-state node is exactly what runs when the node is switched in next; and the driver edits that image afterwards, storing the texture window offsets into dwords 200 and 201 at the next switch-in when the window changed, which only makes sense if the engine has filled it.
After the write the driver reads back one thing, bit 31 of `0x10018`, which it treats as "the save is finished and the engine has stopped". The interrupt handler (`@0x17bee`) reads the whole register and tests it for non-zero (`@0x17c06`), masks only the two cause bits `0x60000000`, and writes 0 to the whole register (`@0x17c1c`), which also clears bit 31. The `0x7fffffff` write is consistent with an acknowledge that clears bit 31 and leaves the two cause bits for the handler. Inferred.
Not known: which registers the engine writes, in what packet form, and how many of the 208 dwords it covers. The driver rewrites only the long chain packet and dwords 200 and 201 itself, so everything else in the image has to come from the engine's dump to be correct after the first switch-out.

## How the OpenGL driver builds and submits packets (`sgcoicd.dll` 5.2.4)

Read in Ghidra from the shipped 5.2.4 NT 4.0 package with SGI's symbols (the ICD `sgcoicd.dll`, the display driver `sgcodd.dll`
and `sgcomp.sys`), checked against the miniport reading above. Grades: *established* is what the driver code does, read in full for
the routine named; the hardware's reading of any word is inferred, as above.

**The path, end to end. Established.**

1. The OpenGL driver runs in the application's process. It writes packets straight into packet-buffer memory that the kernel
   side mapped for it (it never touches an engine register; those are at physical addresses only the miniport maps).
2. To make work visible to the engine it calls `ExtEscape` (through `__glCoExtEscape`, `0x69018322`) with one of the escape codes
   below. The display driver's `DrvOpenGLEscape` checks the arguments (`checkargs`, with the escape's name and input size) and
   `IsUserPktOffset` verifies that the position the application passed lies inside the context's own packet buffer, then
   allocates a "drop" (`CoGrmAllocateDrop`) on the context's render node and calls the driver's queue entry.
3. The miniport's scheduler runs the drops in order (`CoExecuteDrop`, `CoScheduleDrop`): drop types 1 finish, 2 screen-to-screen copy,
   3 pixel DMA, 5 serialisation mode, 6 kick (only clears a "pending" flag on the render node), 7 and 8 bind and unbind buffers, 9
   unsynchronised swap; types 0 and 4 complete at once. The engine itself is started by the context switch described above (the
   `0x10000` write), not by the escape.
4. Completion is learned by the application polling the packet buffer (below), blocking on a kernel event, or both.

**ICD escape codes, 5.2.4 build. Established (`DrvOpenGLEscape` in `sgcodd.dll` 5.2.4, and the call sites in `sgcoicd.dll` 5.2.4; name, input size in bytes).**
The display driver tests the code and the sizes with `checkargs` before it acts. Every input size below was read in the display driver
and equals the size the ICD passes at each of its 33 call sites of `__glCoExtEscape` (all read from the disassembly); the output size
the ICD supplies equals the driver's requirement, or is larger where the driver asks for none. An escape also takes an output size,
listed after the table. The Windows 2000 5.2.4 display driver (`sgcodd.dll`, 339,040 bytes, a different binary from this one) was read the same way and carries the identical table, sizes and all.

| Code | Name | In | Code | Name | In |
|---|---|---|---|---|---|
| `0x40` | SETCONTEXT | `0x54` | `0x52` | SETUP_SCR2SCR | `0x7c` |
| `0x41` | FIRSTSETCONTEXT | `0x2c` | `0x53` | CREATE_BUFFER | `0x28` |
| `0x42` | CREATECONTEXT | `0x20` | `0x54` | FREE_BUFFERS | `0x40` |
| `0x43` | DELETECONTEXT | `0x14` | `0x55` | BIND_BUFFERS | `0x70` |
| `0x44` | RELEASECONTEXT | `0x14` | `0x56` | UNBIND_BUFFERS | `0x6c` |
| `0x46` | SHARELISTS | `0x1c` | `0x57` | CREATE_TEXTLBPKT | `0x44` |
| `0x47` | DESCRIBELAYERPLANE | `0x20` | `0x58` | FREE_TEXTLBPKT | `0x40` |
| `0x48` | SWAPBUFFERS | `0x7c` | `0x59` | SET_READY_TEXTLBPKT | `0x40` |
| `0x4b` | SETLAYERPALETTEENTRIES | `0x28` | `0x5a` | CREATE_PBUFFER | `0x50` |
| `0x4c` | FINISH | `0x18` | `0x5b` | DELETE_PBUFFER | `0x1c` |
| `0x4d` | KICKSCHEDULER | `0x7c` | `0x5d` | PIXELFORMATEXTDESCRIBE | `0x20` |
| `0x4e` | CLEARBUFFER | `0x7c` | `0x60` | GET_GAMMA | none (output only) |
| `0x4f` | PIXELDMA | `0x7c` | `0x61` | SET_GAMMA | none (output only) |
| `0x50` | CACHEOP | `0x24` | `0x62` | WAIT_FOR_HARDWARE | `0x18` |
| `0x51` | PIXELDMA_CLEANUP | `0x7c` | `0x63` | ALLOC_PACKET_BUFFER | `0x18` |
| | | | `0x64` | FREE_PACKET_BUFFER | `0x18` |

Output sizes (bytes the display driver requires the caller to provide): SETCONTEXT 8, FIRSTSETCONTEXT `0x18`, CREATECONTEXT `0x10`,
DELETECONTEXT 4, RELEASECONTEXT 4, DESCRIBELAYERPLANE `0x24`, SETLAYERPALETTEENTRIES 8, PIXELDMA `0x10`, CREATE_BUFFER `0x14`,
CREATE_TEXTLBPKT `0x18`, CREATE_PBUFFER 8, DELETE_PBUFFER 4, PIXELFORMATEXTDESCRIBE `0x30`, GET_GAMMA and SET_GAMMA `0x1c`,
WAIT_FOR_HARDWARE 4, ALLOC_PACKET_BUFFER `0x18`; the rest need none. The codes `0x45`, `0x49`, `0x4a`, `0x5c`, `0x5e`, `0x5f` and
anything above `0x64` return 0 (not supported) without doing anything. Two further codes, `0x1100` and `0x1101`, and the low codes `8`
and `0x3f`, belong to the OpenGL-presence query (`OPENGL_GETINFO`, input 4, output `0x214`) and are not packet-path codes. Of the 33
call sites in the ICD, none sends `0x60` or `0x61`: the gamma pair is sent by another component. ALLOC_PACKET_BUFFER and
FREE_PACKET_BUFFER forward to the miniport as video IOCTLs `0x2320b4` and `0x2320b8`. An earlier version of this table gave the
three codes `0x60` to `0x62` as WAIT_FOR_HARDWARE, ALLOC_PACKET_BUFFER and FREE_PACKET_BUFFER in an unmatched order, and had no
gamma pair: the list above is the corrected assignment. Established.

**ICD escape codes, 1999 build (`sgcodd.dll` 334,704 bytes, build 1999-04-27; byte-identical on the SGI 320 NT 4.0 install CD, Disk 1, and
Recovery CD-2 v1.1). Established, read in Ghidra at the jump table of the dispatch routine (`0x1a6e0`, table `0x1c02c`, 36 entries from
`0x40`).** The codes are different, not just the sizes: the 1999 build has no FINISH, no texture-table packets and no escape at `0x48`,
but has DISPATCHCHUNK and SERIALIZE, and every code from `0x4c` up is shifted by one or two. Name, input size, output size:

| Code | Name | In / out | Code | Name | In / out |
|---|---|---|---|---|---|
| `0x40` | SETCONTEXT | `0x50` / 8 | `0x53` | PIXELDMA_CLEANUP | `0x70` |
| `0x41` | FIRSTSETCONTEXT | `0x30` / `0x18` | `0x54` | SETUP_SCR2SCR | `0x70` |
| `0x42` | CREATECONTEXT | `0x20` / `0xc` | `0x55` | CREATE_BUFFER | `0x28` / `0x14` |
| `0x43` | DELETECONTEXT | `0x14` / 4 | `0x56` | FREE_BUFFERS | `0x3c` |
| `0x44` | RELEASECONTEXT | `0x14` / 4 | `0x57` | BIND_BUFFERS | `0x64` |
| `0x46` | SHARELISTS | `0x1c` | `0x58` | UNBIND_BUFFERS | `0x64` |
| `0x47` | DESCRIBELAYERPLANE | `0x20` / `0x24` | `0x59` | CREATE_PBUFFER | `0x50` / 8 |
| `0x49` | SWAPBUFFERS | `0x70` | `0x5a` | DELETE_PBUFFER | `0x1c` / 4 |
| `0x4c` | SETLAYERPALETTEENTRIES | `0x28` / 8 | `0x5c` | PIXELFORMATEXTDESCRIBE | `0x20` / `0x30` |
| `0x4d` | DISPATCHCHUNK | `0x1c` | `0x5f` | GET_GAMMA | out `0x1c` |
| `0x4e` | SERIALIZE | `0x1c` | `0x60` | SET_GAMMA | out `0x1c` |
| `0x4f` | KICKSCHEDULER | `0x70` | `0x61` | WAIT_FOR_HARDWARE | `0x18` / 4 |
| `0x50` | CLEARBUFFER | `0x70` | `0x62` | ALLOC_PACKET_BUFFER | `0x18` / `0x18` |
| `0x51` | PIXELDMA | `0x70` / `0x10` | `0x63` | FREE_PACKET_BUFFER | `0x18` |
| `0x52` | CACHEOP | `0x24` | | | |

Codes `0x45`, `0x48`, `0x4a`, `0x4b`, `0x5b`, `0x5d` and `0x5e` fall to the default, which returns 0 (not supported). The 1999 ICD (`sgcoicd.dll`, 1,245,456 bytes, disc 2 of the OEM set) was read at all 31 of its call sites: the code and input
size at each agree with this table, except two sites in `__glCoCreateTexLevel` that send code `0x64`, which the table does not
have ([the 1999 build](../../software/windows/nt-opengl-icd.md#the-april-1999-build)). The two builds therefore must not be mixed: an ICD and display driver of one build share one numbering. A
model that implements only the miniport's video IOCTLs is unaffected, since the miniport never sees these codes.

**Packet buffers. Established.** `InitCoPktBuffer` (`0x690186f2`) links a ring of nodes (a 40-byte descriptor each, count and size from the
context) over one block of memory. In each node's buffer the data starts `0x20` bytes in, the last `0x180` bytes are held back as slack,
and the descriptor carries the next node and the node's position in 32-byte units counted from a base the context records (whether that
base is physical address zero or the pool's start was not determined; the units are 20 bits wide in the words below). The first words of
each node buffer are a header: the link word (below) at dword 2 and again at the dword index `pktFlushOffset`, which the kernel returns in the reply to
`FIRSTSETCONTEXT` (the earlier text put the second copy at dword 0 without following where the value comes from), and a running sequence number at dword 3
(`InitCoPktBuffer`, `DispatchCurrentPkt`).

**Packet header word and the headers the ICD emits.** A packet is a header dword followed by payload dwords; bits 11:5 are
the payload count (checked against 17 of 18 entries of the context shadow table, whose last entry runs to a padded end),
bits 31:21 are the start address of a state or register block, and the packet set is now classified in full in
[packet-set.md](packet-set.md): 52 constant header values in `sgcoicd.dll` 5.2.4, each with its emitting routine, and
the computed forms (vertex port plus attribute mask, light index, second clear bank). The packets' combined use is in
[draw-and-buffers.md](draw-and-buffers.md). Three statements made here earlier were wrong and are corrected there:
`0x932ac040` (polygon offset) carries the factor first and the units times a scale second; `0x97400020` writes the
constant 0 (it is not "one texture-level word"); and `0x9400014f` is not a header the driver emits. The list of
20 headers and the remark that the pass-state targets were "two fields that step together" are superseded by the
address reading, in which the first field is an address and the step is the previous block's payload count.

**Closing a buffer and chaining. Established (`DispatchCurrentPkt` `0x6901a83a`, `ClosePacket` `0x69062118`).** The ICD ends the data in a node with a
three-dword packet: header `0x97000040`, a dword with the next node's position in bits 31:12 and, in the low bits, a field built from the running sequence
number, and a dword with this node's position in bits 31:12 and the low twelve bits of the sequence number. (The display-list builder's variant writes
the same header and the two positions without the sequence number.) The node is then padded with zero dwords to a 32-byte boundary, and the link word
below is stored. A fixed packet (`0x09280080` plus four dwords the context keeps together) is appended to the new node after a dispatch.

**Node layout and link words. Established as software writes; the engine's use is inferred.**
A node is one 32-byte header unit followed by packet data in whole 32-byte units. The OpenGL driver, the miniport and the display driver all
start the data at node byte `0x20` (the display driver puts a three-dword chain packet first, so its own packets start at `0x2c`). The link word is
`(next node position << 12) | (units << 3) | flags | 1`: bit 0 set means the engine owns the node, bits 2:1 are the interrupt and sync flags the caller chooses
(the display driver uses 1, 3 or 7: ready, ready and interrupt, ready and interrupt and sync), bits 11:3 count the units of
data and bits 31:12 give the next node's position. `units` counts the data after the header unit, padded to a whole unit, and includes the chain packet that ends the node.

*Every writer counts the same way* (the display driver has one undecoded mode: when its flag at `+0x77c` is non-zero the count becomes `0xffffffff`, `sgcodd.dll@0x2789d`..`0x278c5`; what sets that flag was not traced). The OpenGL driver's closers (the ordinary dispatch at `0x6901a83a`, the display-list close at `0x6901d5e5` and the
display-list compiler's close near `0x69062687`) count bytes from the start of the data. The miniport's per-packet writer (`sgcomp.sys@0x1725c`), the three
link stores that build a context-switch page (`@0x168d5`, `@0x16904`, `@0x16934`) and the user-texture table writer (`@0x17a2f`) take the end of the
padded data minus the node's own position minus one, which is the same number once the header unit is left out. The display driver's packet dispatcher
(`sgcodd.dll@0x2786c`) takes the distance from the node start in units and subtracts one: the same again. The earlier reading that the miniport
stores the count "minus one" and so disagrees with the OpenGL driver counted from the wrong end; no pair of writers differs.
Two layouts the miniport builds show that the header is not counted. The saved-state node of a context-switch page has its header at page byte `0x80`, data
from `0xa0` and a stored length of 26 units, which ends exactly at `0x3e0` where the next node's header starts; a count that included the header would leave the last
unit uncovered, and that unit holds the long chain packet (below). The OpenGL driver's display-list jump node holds one three-dword packet after its header and stores length 1 (link `(next << 12) | 9`, `0x6901d791`).
Sources are the 5.2.4 driver pack's `sgcoicd.dll`, `sgcomp.sys` and `sgcodd.dll` (the evidence-directory copies of the last two are other builds and were not used). Established for what the drivers write. That the engine reads exactly `units` units after the header is inferred from this agreement (any other rule would cut off the chain packet that
ends each node in the OpenGL driver and the miniport).

*Where the word is stored.* The packet-buffer allocator turns one pool page number into three values (`sgcomp.sys@0x15b14`): the byte offset in the pool and two CPU addresses for it, the id 0 aperture window at `0x80000000` ([above](#cpu-aperture-windows-at-0x80000000)) and a second window at `0xc0000000` plus the same offset, which no other page lists (the aperture list has only windows at `0x80000000 + (id << 24)`). The OpenGL driver's buffers are mapped through the first; a second mapping through the second, the "flush view", is returned to it as a dword distance from the buffer start (the reply of the first-context escape, built by the packet-buffer allocation at `sgcomp.sys@0x1514c`; the display driver gets the same from its own allocation call; that the first-context reply's field is this distance is unverified, I traced it through the display driver's pass-through only). The miniport keeps the same two views for its own pool and for every context-switch page. Every writer stores the link word, with its owner bit, through the flush view at the
node's first dword. The OpenGL driver and the display driver also store a copy in the node's own dword 2 and the running serial in dword 3; the miniport writes neither in any
builder I read. The display driver first zeroes dword 0 and fills dword 1, then stores the link word through the flush view. Every wait in all three
drivers tests bit 0 of dword 0, and the only store that can set that bit is the flush-view one, so the two windows must reach the same memory: the link word and
owner bit sit in node dword 0. Inferred (the drivers are the witness; what the extra window does differently, such as bypassing a cache, is unknown). So "link word in
dword 2" describes only the OpenGL and display drivers' extra copy; the word the miniport's chains depend on, and the word every driver waits on, is dword 0. I found no code that reads dword 2 back in the routines
examined; the rest were not searched.

**Waiting. Established (OpenGL driver `0x6901aa0b`, `0x6901b50c` and `0x690614c2`).** All three loops exit when bit 0 of node dword 0 reads clear, and on nothing else: not dword 2, not dword 3.
They step through an 18-entry table of busy-wait counts. At the last step, unless the variable that disables blocking is set, the loop asks the kernel (escape `0x62`, wait for hardware) whether the engine is still
busy with a serial and, if so, sleeps on an event the miniport signals from its stop interrupt; then it restarts the table. The serial it sends is dword 3 of the node, read just before the call
(sixth input dword of the escape). The two loops inside the dispatch and display-list code reset their remembered value to all ones each time the table step is reloaded (`0x6901a9e9`, `0x690614a0`), so the compare against it skips the call only
when the serial is `0xffffffff`; the loop at `0x6901b50c` keeps the value across passes and asks once per distinct serial. No loop waits for dword 3 to change.

*Who writes dword 3.* Only drivers: the OpenGL driver at each node close (a running count that starts at 0 for the ring and is the same number it folds into the chain packet), the display driver at
each dispatch from a global counter, and both at ring set-up. The engine is not expected to write it. The miniport reads it in one case: a stop whose chain-register serial lacks the
`0x800000` marker (the chain packets of the display-list builders lack it), where it takes dword 3 of the node whose position the stop reports. The kernel's wait compares the low 20 bits of the serial the OpenGL
driver sends with the last serial its stop handler recorded for the render node, so dword 3, the chain packet and the escape must carry one series. A replacement engine therefore has to clear bit 0 of node dword 0
when it finishes a node, write the chain registers when it executes a chain packet, and raise the stop interrupt. Inferred for the engine, established for the drivers' side.
`Finish` is escape `0x4c` followed by a wait on a second event. `__glCoIcdKickScheduler` is escape `0x4d` with the
closed node's position as its one argument.

## What is not known

The engine's use of every header in [packet-set.md](packet-set.md) (the register-file rows are tied to registers only by three matches); the contents of `0x00000` to `0x07fff` and `0x0c200` to `0x17fff`; what `0x8000` entries
and `0xc000` entries are in hardware terms; what the engine does on each interrupt cause; whether the engine reads the
packet buffers by physical address or through the `0x8000` table; reset values; the Z and stencil buffer layout; how
the engine writes pixels into the display back end's tile lists (the miniport hands it the same lists it gives the
display, see `pAllocateTilesForBuffer`). The route is the OpenGL driver `sgcoicd.dll`, whose setup code is a software
implementation of the geometry pipeline that ends by writing packets; reading its packet emitters in full (see
[the open questions](../../questions/open-questions.md)).

## Review

Independent review of the OpenGL packet-path section (chips commits 7990dc9..7a40091) could not open the InstallShield cabinet of
`sgcoicd.dll` 5.2.4 and checked the claims against the 1999 `sgcodd.dll` and `sgcomp.sys` from the SGI 320 NT 4.0 install CD. Confirmed
there: the shadow-table rule that bits 11:5 of a header are the payload count (every table distance matches count plus one except
the last entry, which runs to a padded end); the chain packet `0x97000040` with a next-position dword and a position-and-sequence
dword; the escape dispatch on a code range from `0x40` upward. The count for `0x944c42c0` was corrected from six to twenty-two.
The escape table was then read in both builds (above): the 5.2.4 sizes in the table the review could not check were all confirmed
against the 5.2.4 display driver and the 5.2.4 ICD's own call sites; what the review took for a transcription difference is a
different numbering in the 1999 build, listed in full. The gamma pair and the three-code `0x60` to `0x62` row were corrected.
