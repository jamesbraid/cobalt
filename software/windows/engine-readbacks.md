# What the Cobalt drivers read back from the engine

Every place the OpenGL driver (`sgcoicd.dll`), the display driver (`sgcodd.dll`) and the miniport (`sgcomp.sys`) read
something the engine produced: registers, words the engine writes into packet memory, and pixels. For each, what is read
and why, so a replacement engine knows which state has to be exact and which can be approximated.

Source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 (patch SG0020054) and for Windows 2000 (patch SG0020052). The
NT 4.0 files were read in Ghidra with the vendor's symbols and types; the miniport's register reads were also taken from a
linear disassembly of both builds (the `mov reg, [base + offset]` sites, below). Grades as in [AGENTS.md](../../AGENTS.md).
"Established" here means the routine was read and the access is in the disassembly; what the engine does to make the value
is inferred or unknown, as always.

## Summary

The drivers read very little back. Four items need engine behaviour:

1. Bit 0 of dword 0 of every packet node (the owner bit), polled by all three drivers: the engine must clear it when it has
   finished the node.
2. Bit 31 of the same dword, read by the OpenGL driver for the occlusion test: the engine must set it in the same write.
3. The stop registers the miniport reads at each stop (`0x10000`, `0x10018`, `0x185c0`, `0x185c8`, `0x185d0`).
4. The context dump the engine writes when the miniport writes `0x18800`.

Nothing in any driver reads a colour or depth value back out of an engine register, a FIFO, a status word or a counter other
than these, and no read-back packet exists in the set of 52 headers. Pixels come back by host transfer (a packet-driven
engine copy into host memory) or by CPU reads of the aperture windows, listed last.

## 1. The packet node's first dword

**Owner bit, bit 0. Established.**

- OpenGL driver: the three wait loops (`DispatchCurrentPkt@0x6901a83a`, `CoWaitForPacket@0x6901b50c`, `waitForPacket` and
  `waitForHWDLPacket` for display lists) read the first dword of the next node in the ring and loop while bit 0 is set. This
  is how it learns a node can be reused, and the only way it learns the engine has finished.
- Display driver: `pulGetPacket@0x27748` spins on bit 0 of the next ring node before writing into it. `DrvSynchronize@0x2b2ea`
  sends a flush-marker packet (`0x98804000`, no payload), waits for the miniport's last-retired serial, then spins on bit 0 of the last
  node it dispatched. The wait on the serial is `WaitForEvent`; the spin is a plain read.
- Miniport: none; it writes the bit (sets it by the flush-view store that hands a node over) and never polls it.

**Occlusion result, bit 31. Established for the driver, inferred for the engine.**

The vendor's own type for a returned node word, `CoPktRespType`, has an `occlusionTest` bit at 31 beside the `ready` bit 0, and
its type for the word as the driver writes it, `CoPktDescType`, has `ready`, `intr`, `sync`, a 9-bit size and a 20-bit next
position. The OpenGL driver's occlusion path:

1. `glStartInstrumentsSGIX@0x690434e2` makes the depth-test word carry the occlusion-count bits (`init` 28 and `en` 27, both
   set for the first update, `en` alone afterwards; see [opengl-expectations.md](opengl-expectations.md)).
2. `glStopInstrumentsSGIX@0x6904353c` (and `glReadInstrumentsSGIX@0x6904366a` while a measurement runs) turns the bits off
   with a depth-test update (stop only), sets the tag byte at `+0x20` of the current node record (`occlusionTag`), dispatches
   that node, and appends a four-word entry to the application's instrument buffer at the active position: the instrument id
   `0x8310` (`GL_DEPTH_PASS_INSTRUMENT_SGIX`), the size 4, a result word left for later, and the caller's marker. A full
   buffer sets the instrument machine's overflow flag instead. So the engine's answer for everything drawn since the bits
   went on is owed to that entry.
3. The driver does not look at the answer until the node is retired:
   when it next waits for that node to be free and the loop above ends, `DispatchCurrentPkt` sees the tag and calls
   `__glCoProcessOcclusion@0x690437e6`, which reads **bit 31 of the node's first dword**, stores 0 or 1 into the result word
   of the oldest unretired entry (the retire position advances by 4 words) and clears the tag. `glPollInstrumentsSGIX@0x6904371a` returns the marker of the next entry that
   has a result; `glGetInstrumentsSGIX@0x69043798` returns how many entries have retired since the last call.

So the count is one bit per dispatched node: whether any fragment passed the depth test while the occlusion bits were on in
that node. The engine returns it in the node's first dword when it retires the node, in the same write that clears the owner
bit; no register, counter or FIFO is involved. A node with the tag set and no pass reads as 0. This answers the occlusion row
of the open questions. Whether the hardware writes the word (rather than the kernel) is inferred from the type name and from
the miniport never touching it; the miniport writes the word only by the link-word store at hand-over.
An emulator that always returns bit 31 clear reports "no samples passed" for every instrument, which is a valid answer.

## 2. Engine registers the miniport reads

All reads are in `sgcomp.sys` and use the engine window base kept in the device record (`coBase`). Established from the
disassembly in both builds: the Windows 2000 build has the same reads at the same offsets in the corresponding routines
(`FUN_00017752` for the NT 4.0 `CoProcessCobaltEvent`, `FUN_00016630` for `CoBuildWinparamsPacket`, and so on).

| Register | Read site (NT 4.0) | What it reads and why |
|---|---|---|
| `0x10000` | `CoInterruptHandler@0x17be8`, first instruction after the device record load | A read whose result is thrown away (the decompiler drops it; it is a plain `mov` into a register that is never used). It comes straight before the cause read below. Inferred purpose: to flush earlier posted writes, or to order the cause read after the engine's own state update. |
| `0x10018` | `CoInterruptHandler@0x17be8` | The stop-cause register. The handler reads it whole: zero counts a spurious interrupt; otherwise it keeps bits `0x60000000` as the cause, writes 0 to the register, and re-reads until the cause bits no longer equal what it kept (the clear has landed), then queues an event with the cause for the deferred handler. Why: to learn why the engine stopped (end of the chain, or the second cause) and to acknowledge it. |
| `0x10018` | `pCoContextSwitch@0x16610`, wait before the start-word write | Polls bit 31 until set. Why: the engine sets it when the context dump it was asked to make (a write of the dump position to `0x18800`) is complete and the engine has stopped; the miniport then writes `0x7fffffff` to it and restarts. See [graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md). |
| `0x10000` | `CoProcessCobaltEvent@0x17f2c` | The start word, as the engine left it: its upper 20 bits are the position of the node the engine had reached. Why: to record where the context stopped (`savedPktOffset`) and to resume it there. If it reads 0 the handler falls back to `0x185c0`. A position that is not inside the stopped context's own buffer (`CoIsUserPktOffset`) causes the handler to rewrite `0x10000` to an idle word and leave the node alone. |
| `0x185c0` | the same routine | Chain register one: used in place of `0x10000` when that is 0, and for the 12-bit serial field of the stop. |
| `0x185c8` | the same routine | Chain register two: bits 31:12 are the position of the node whose chain packet stopped the engine (non-zero means a node was retired; zero means the engine idled), bits 11:0 the serial. When the first register's 12-bit field lacks the marker bit `0x800` (display-list chain packets lack it) the handler instead reads dword 3 of that node from memory at `node position * 0x20 + 0xc` and keeps its low 20 bits as the serial. Why: this is `serialOnRemove`, the value the wait escape compares against. |
| `0x185d0` | the same routine, only for nodes of the OpenGL type | The current texture id. Why: saved in the node so the next switch-in can restore the binding (the miniport writes the same register at `0x185d0` on a restart, and clears it when a texture packet is gone or a bind says "reset"). |
| `0x18138`, `0x18140` | `CoBuildWinparamsPacket@0x16e0a` | The scissor minimum and maximum corner, read when the incoming node is restarted without having been dumped. Why: copied into a scissor packet (`0x84e44040`) at the head of the node so the scissor survives a switch the dump did not cover. |

Established for the engine window by scanning both builds for every load addressed as a base register plus a constant displacement between `0x4000` and `0x20000`: the rows above are the whole list. Registers reached through a pointer the code holds in a register (the interrupt handler's `devext+0xbdc`, the context switch's `lea` for `0x10018`) appear in the routines named. Not read from any register: the colour, depth and stencil values; the vertex or fragment counters; any FIFO; any texture
state beyond the id above; the window parameters (the miniport builds those from its own drawable record). The stores to the
translation tables at `0x8000` and `0xc000` and the writes listed in the other pages are not reads.

### The context dump

The engine writes the outgoing context into the memory the miniport names at `0x18800` (the "dump offset"); the miniport
afterwards reads that memory in two ways: it chains the incoming node's long chain packet over the saved image and it stores
two dwords (window texture offsets, dwords 200 and 201) into the dump when the texture window moved. It reads nothing else
from the dump. So the engine's dump must be in the packet form the image already has (the default image in
[the engine page](../../devices/cobalt-graphics/graphics-engine.md)), because it is replayed as packets.
See also open question 16.

### Display block reads in the miniport

Reads at the display block's base (`asBase`), not the engine: the colour map FIFO status at `+0x58000` (waits for its low six
bits), the live copy of the frame-control register at `+0x30008` (waits for bit 0 to clear in `asStopNormal`), the vertical
retrace interrupt status and enable registers (`CoVRetraceHandler`: it reads the status through a pointer in the device
record and writes back its complement, which acknowledges it), the scan-line counter at `+0x10000`
(`asGetScanline` returns bits 23:12), and the read-modify-write forms at `+0x10018`, `+0x10020` and `+0x10024` (power state and
scan-line interrupts). Their meanings are in [display-engine.md](../../devices/cobalt-graphics/display-engine.md). Note that
`0x10000`, `0x10018` and `0x10020` are offsets in **both** the engine window and the display block; the routines named
`as*` use the display block base.

## 3. Pixels and surfaces read back

**Host transfers, `glReadPixels` and `glCopyPixels`. Established for the routines, inferred for the engine.**
The packet sequences of the three transfer routines are in [opengl-pixel-transfers.md](opengl-pixel-transfers.md);
this section only says what the CPU reads and when.
`__glCoPickReadPixels@0x69038f2c` chooses one of three ways from the pixel format, type and state:

- *engine transfer to a host buffer* (`__glCoTileToLinearXfer@0x6903c561`): the ICD sends `PIXELDMA` (`0x4f`) to map the
  application's buffer into translation-table pages, writes the transfer packets (starting with the scissor packet
  `0x84e44040` and the draw-flags word `0x00210020` cleared for the transfer and restored afterwards, plus the colour-matrix registers if channel order or size changes), then
  runs a host converter over the result (`coProcessTileToLinear`, `coProcessMmxTileToLinear` for the MMX path). The CPU
  reads the host copy only after the node's owner bit has cleared. Nothing is read from engine memory by the CPU in this
  route. This is the default.
- *direct read*, selected by the variable `__directRead` (set from the environment; the routine at `0x6903c561` is the
  direct-read callee): the same transfer, but the destination is a DirectDraw surface or the screen.
- *host span reads* (`__glCoSpanReadRGBA`, `__glCoSpanReadDepth`, `__glCoSpanReadStencil` and their second variants,
  `ReadRGBA8Span`, `ReadCISpan`): used by the host fill routines (anti-aliased polygons, stippled and wide lines, host
  rasterised bitmaps) that blend or compare against what is already in the buffer. These read the buffer through the CPU
  aperture windows. Whether a node is drained first is not traced; this is [question 14](../../questions/open-questions.md).

`glReadPixels` of depth at `GL_UNSIGNED_SHORT` or `GL_UNSIGNED_INT` and of stencil are handled by the same transfer with
different type codes (`coSZToDepthUint` converts the 24-bit depth word to unsigned int on the host); the packing is
[question 7](../../questions/open-questions.md).

`glCopyPixels` between buffers is `__glCoTileToTileXfer@0x6903ce87`: engine packets only, no host read.

**Buffer regions (`GL_KTX_buffer_region`).** `__glCoReadBufferRegion@0x690229ca` uses the same transfer family into a host
copy; `__glCoDrawBufferRegion` draws it back. No CPU read of engine memory.

**Display driver.** `vXferDMA_SB@0x148d0` and its siblings are the same engine-to-host transfer for GDI `DrvCopyBits` and
`DrvBitBlt` with a screen source; after it, `DrvSynchronize` and a CPU copy of the first and last partial 32-byte lines of the
host buffer through the aperture window at pitch `0x2000`. DirectDraw lock reads go to the aperture directly
(`DdLock@0x1fcec`). The reads of display state are `DdGetScanLine@0x1fe8c` and `VBLANK_IS_ACTIVE@0x1fe54`
(the display block's live x and y counter) and the DirectDraw flip-status routines. The paths are listed in
[nt-display-driver-video.md](nt-display-driver-video.md) and [2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md).

**Context read-back.** The ICD can fetch state with `glGet*`, but it answers from its own shadow copy of the engine state
(the 18-entry shadow table in [graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md)); no packet or register
reads engine state for it.

## 4. Why each read exists, for the exactness decision

| Read | Needs exact behaviour? |
|---|---|
| Owner bit 0 cleared at node completion | Yes; every wait depends on it. |
| Occlusion bit 31 | Only for `GL_SGIX_depth_pass_instrument`; a constant 0 is a valid approximation. |
| `0x10018` cause and acknowledge, `0x10000`, `0x185c0`, `0x185c8` | Yes for any engine model that runs more than one context: they carry the stop position and serial. For one context, an engine that finishes every node and raises the stop interrupt with the last node's position and serial is enough. |
| `0x185d0` current texture | Only across a context switch. |
| `0x18138` and `0x18140` | Only on restart without a dump. A model that always dumps never reads them. |
| Context dump contents | Yes if more than one context is multiplexed; see question 16. |
| Pixel transfers (`PIXELDMA` route) | Yes: the packet sequences are the read-back. |
| Host span reads | The aperture read of tiled pixels must see completed engine writes; whether the drivers drain first is question 14. |
| Display block reads | The retrace and scan-line values affect timing only. |

## 5. The Windows 2000 build

- **Established.** The Windows 2000 miniport has the same engine register reads at the same offsets (table above), in the same
  routines by shape.
- **Established.** The Windows 2000 OpenGL driver is instruction-identical to the NT 4.0 one in 2592 of the 2637 functions
  that align by size (see [the comparison](w2k-opengl-driver-comparison.md)); the wait loops, the occlusion routine and the
  transfer routine are in the identical set, apart from one dropped debug-print argument push in
  `__glCoTileToLinearXfer`.
- **Inferred.** The Windows 2000 display driver's read sites are the same family; its routines carry no names and were
  compared by constants only.
