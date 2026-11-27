# The escapes between the OpenGL driver and the display driver

Every request the OpenGL installable client driver (`sgcoicd.dll`) makes of the kernel side goes through one GDI call,
`ExtEscape`, to the display driver (`sgcodd.dll`), which either answers it itself or turns it into a request for the
miniport (`sgcomp.sys`). This page lists each escape code with its input and output layouts, what the display driver does
for it, and what the private escapes next to it are. The code table and sizes were first recorded in
[graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524);
this page adds the layouts, the behaviour and the Windows 2000 build.

Source: SGI Cobalt Graphics Driver 5.2.4, Windows NT 4.0 pack (patch SG0020054) and Windows 2000 pack (patch SG0020052),
read in Ghidra with the vendor's symbols and type declarations applied to the NT 4.0 files. The Windows 2000 program
database cannot be read by Ghidra 12.1.3 (its debug-info stream is version 19960307, see the retools notes), so the
Windows 2000 files were compared with the NT 4.0 ones by size, constants and instruction shape. Decompiler output was
used to navigate; the field names, offsets and sizes below are SGI's own type declarations (`_CO_*` structures and the
`CO_CMD_ESCAPE` enumeration in `sgcodd.pdb`) confirmed against the consumer routine's reads. Grades as in
[AGENTS.md](../../AGENTS.md).

## Transport

- **Established.** The ICD sends every escape from one wrapper (`__glCoExtEscape@0x69018322`, NT 4.0; same body at
  `0x69019924` in the Windows 2000 ICD) as `ExtEscape(hdc, openglcmd, ...)`. `openglcmd` is `OPENGL_CMD` (`0x1100`),
  or `0x3f` when the environment variable `CO_OPENGL_CMD` is set. The ICD never sends `OPENGL_GETINFO` (`0x1101`).
- **Established.** GDI hands the input to the display driver's escape entry (`DrvOpenGLEscape@0x18630` in the NT 4.0
  build, the function at `0x18520` in the Windows 2000 build; DDI index 24, the driver's `DrvEscape` hook). A request
  shorter than `0x14` bytes is rejected with `-1`. A code outside the list below is handled as a private escape (next
  section).
- **Established.** Input starts with the 20-byte `_CO_OPENGLCMD_HEADER`: `+0x00` the escape code (`ulSubEsc`), `+0x04`
  flags (the ICD sends 0), `+0x08` and `+0x0c` a window object and a translation object (`pwo`, `pxo`; by their types
  GDI supplies them for `OPENGL_CMD`, not checked in `win32k`), `+0x10` the driver's context handle (`hcx`, the value `CREATECONTEXT` returned, 0 before
  it). The display driver looks the handle up (`__glFindDrvContext`) for every code.
- **Established, return values.** The escape returns `1` for success, `-1` when `checkargs` finds the input or output
  length wrong or an argument invalid, and `0` for an unsupported code (the ICD's wrapper retries a `0` return every
  millisecond, against the screen device context, until it is non-zero).
  Output structures carry an NT status in a `status` field: `0` success, `0xc000000d` invalid parameter, `0xc0000017` no
  memory.
- **Established, ICD side.** The wrapper writes the code and the context handle into the header and refuses codes outside `0x40`
  to `0x64`. For the queued codes it counts escapes per packet node (`+0x1c` of the node record); past `droplimit` (10 by
  default, `CO_GL_DROPLIMIT` from 5 to 100) it sends a finish (`0x4c`, through `__glCoIcdFinish` at `0x6901849c`) and waits on the finish
  event, and resets the count; this is back-pressure, not a kick (the earlier text said kick). After a
  successful call it clears bit 0 of the node record's flag word at `+0x18` (bits 0 and 1 for `0x4d`, `0x4f`, `0x52` and
  `0x55`); the kick path sets bit 0 first. What the bits mean to the packet writer was not traced.
- **Established.** The escapes that queue work (`0x48` swap, `0x4c` finish, `0x4d` kick, `0x4e` clear, `0x4f` pixel DMA,
  `0x51` pixel DMA clean-up, `0x52` screen copy, `0x55` bind, `0x56` unbind) all carry the position of a closed packet node
  at `+0x14` (`pktOffset`). The display driver refuses it unless it lies inside that context's own packet buffer
  (`IsUserPktOffset`), then allocates a *drop* (`CoGrmAllocateDrop`, a typed work item on the context's rendering node)
  and hands it to the miniport's scheduler (`SetChunk`). Drop types are the `_CO_DROP_TYPES` enumeration: 0 clear, 1
  finish, 2 screen copy, 3 pixel DMA set-up, 4 swap, 5 serialise, 6 kick, 7 bind buffers, 8 unbind buffers, 9 unsynchronised
  swap.

## The table

`In` and `Out` are bytes. Offsets are from the start of the input or output buffer. A field marked "unchecked" is read
but not range-checked by the display driver. Row text is what the display driver does, read in the routine named.

| Code | Name | In / Out | Input fields (after the header) | What the display driver does |
|---|---|---|---|---|
| `0x40` | SETCONTEXT | `0x54` / 8 | `+0x14` `hWnd`, `+0x18` `hWndRead`, `+0x1c` `layer`, `+0x20` `isStereoPFD`, `+0x24` to `+0x48` descriptor ids for front, back, front-right, back-right, depth, depth-right, front-read, back-read, depth-read and overlay buffers, `+0x4c` `pbufID`, `+0x50` `readpbufID`. Out: `nTlbSlots`, `status` | Makes the context current. Finds the window's drawable (or the pixel-buffer's), links the context to it, and for a pixel buffer allocates its buffers on first use. Binds each buffer to the descriptor id the ICD chose (`BindBufferToDescId`, which tracks free descriptor slots). Binds the rendering node to the window's clip regions (`CoGrmBindRnToClip`: it creates or finds the drawable record, unbinds the old one, binds the new one, resumes the node and moves it to the run queue). A second bind is made for the read window when it differs. Returns the number of descriptor slots still free. `__glCoDrvSetContext@0x1dc12` |
| `0x41` | FIRSTSETCONTEXT | `0x2c` / `0x18` | `+0x14` `hWnd`, `+0x18` `layer` (0, or 1 for the overlay), `+0x1c` `waitobj` (an event handle), `+0x20` `packetSize`, `+0x24` `pbufID`, `+0x28` `ctxLoadObj` (an event handle). Out: `packetAddress`, `packetFlushOffset`, `packetSize`, `baseOffset`, `endOffset`, `status` | Creates the context's rendering node (miniport request `0x232058`, with the two event handles) and its first packet buffer pool (`CoGrmAllocatePacketBuffer`, miniport request `0x23204c`). If the GDI queue was in use (flag `0x40` set, `1` clear) it first drains it: `DrvSynchronize`, then a serialise drop and a wait for its serial. Returns the CPU address of the pool, the dword distance to the flush view, and the pool's start and end positions in the packet-position units the link words use. Creates the table that tracks buffers shared between contexts. `__glCoDrvFirstSetContext@0x1defa` |
| `0x42` | CREATECONTEXT | `0x20` / `0x10` | `+0x14` `dhrc` (the GDI rendering-context handle), `+0x18` `iPixelFmt`, `+0x1c` `iLayer`. Out: `hcx`, `stereoDisplay`, `rightDepthEnabled`, `canSetOverlayPalette` | Allocates a driver context record, registers a driver object so the record is released if the process dies (`EngCreateDriverObj`), copies the pixel format and, for a layer, the layer-plane descriptor. No engine work. |
| `0x43` | DELETECONTEXT | `0x14` / 4 | header only. Out: `status` | Deletes the driver object, which runs the release path below, then frees the context's buffers (each by reference count, miniport request `0x232054`), the shared-buffer table, and the rendering node (`0x23205c`). |
| `0x44` | RELEASECONTEXT | `0x14` / 4 | header only. Out: `status` | Makes the node not current: suspends it, unbinds its drawable and readable, moves it to the idle queue, unbinds descriptors 2 to `0x3f` and zeroes the node's current-texture id. |
| `0x46` | SHARELISTS | `0x1c` / none | `+0x14` source handle, `+0x18` destination handle | Looks both handles up and shares the source context's shared-buffer table with the destination (`__glCoDrvShareLists`; the table carries a reference count, so inferred to be a share by count). |
| `0x47` | DESCRIBELAYERPLANE | `0x20` / `0x24` | `+0x14` `hdc`, `+0x18` `iPixelFmt`, `+0x1c` `iLayerPlane`. Out: size, `LAYERPLANEDESCRIPTOR` | Fills the descriptor from the device's overlay support. |
| `0x48` | SWAPBUFFERS | `0x7c` / none | `+0x14` `pktOffset`, then `_CO_SWAPBUFFERS_ARGS` at `+0x18`: `rectCount`, `RectSwapHint` (16 bytes), `swapFlags`, `swapInterval` | Drop type 4. The miniport (`CoScheduleSwap@0x1934a`) turns it into engine copies of the drawable's rectangles with `CoCopyRect`: flag value 2 copies the drawable's copy region, flag value 4 then copies its clip region, and with stereo enabled a second pass copies the right eye. The default swap is a copy of the partly covered tiles plus a flip of the whole tiles inside the window, which the miniport does when it releases the held chunk ([the tile flip](nt-swap-tile-flip.md)). The unsynced form is drop type 9 (`CoScheduleUnsyncedSwap`: the flip and no copy); no escape queues it, and the only candidate is the DirectDraw flip request `0x232080`, not traced. |
| `0x4b` | SETLAYERPALETTEENTRIES | `0x28` / 8 | `+0x14` `hwnd`, `+0x18` `iLayerPlane`, `+0x1c` `iStart`, `+0x20` `cEntries`, `+0x24` pointer to entries. Out: `status`, `nEntries` | Writes the overlay palette (`__glSetOvrPalette`). |
| `0x4c` | FINISH | `0x18` / none | `+0x14` `pktOffset` | Drop type 1. Refused unless the low nibble of the node's status is 0 or 8. The miniport sets bit 1 of the node's waiting-events mask (`CoScheduleFinish`) and `CoPostDropCleanup` signals event 1, the `waitobj` the ICD passed to `FIRSTSETCONTEXT`, when the drop completes; `__glCoIcdFinish` waits on it. |
| `0x4d` | KICKSCHEDULER | `0x7c` / none | `+0x14` `pktOffset`, `_DROPARGS` with `flags` | Drop type 6: clears the node's "pending" flag so the scheduler will run it. |
| `0x4e` | CLEARBUFFER | `0x7c` / none | `+0x14` `pktOffset`, `_CO_CLEARBUFFER_ARGS` at `+0x18`: six `color` words, six `bytemask` words, six descriptor ids, six `bufdepth` codes, `nBuffers`, a rectangle (`xStart`, `yStart`, `xEnd`, `yEnd`), `flags` | Drop type 0. Validates that every descriptor id is in 2 to `0x3f` and every `bufdepth` is 0, 1 or 2 (`+0x66` is `nBuffers`). The miniport runs it as `CoScheduleClear@0x1928e` (not read in full). |
| `0x4f` | PIXELDMA | `0x7c` / `0x10` | `+0x14` `pktOffset`, `_CO_DMASETUP_ARGS` at `+0x18`: `bufAddr`, `bufSize`, `bufStride`, `bufWidth`, `tlbDesc` (`+0x28`, 2 to `0x3f`), `pcgb`, `ddFlags`, `glFlags`. Out: `bufSizeMapped`, `nTlbSlots`, `x`, `y`, `fbDepth`, `status` | Sets up a host transfer. If the user's buffer is the screen or a DirectDraw surface (`GetXYFromLinear` succeeds) it sets the `status` byte at `+0xd` to 1, answers with the screen position and depth and queues nothing: the ICD then writes packets that address the screen directly. Otherwise it locks and maps the user's linear buffer into translation-table pages (miniport `CoGrmMapLinearBuffer`, request `0x232050`, at most `0x800` slots per node), binds it at `tlbDesc`, and queues drop type 3 carrying the buffer and three flag bits. |
| `0x50` | CACHEOP | `0x24` / none | `+0x14` `addr`, `+0x18` `nbytes`, `+0x1c` `descId` (2 to `0x3f`, must be bound), `+0x20` `flags` (bit 0 or bit 1) | Checks `[addr, addr+nbytes)` lies inside the bound linear buffer, then asks the miniport (request `0x2320a8`, `0x10` bytes: address, length, the buffer record, mode 3 or 2) to flush or invalidate the CPU cache over that range of the pinned pages. Cache maintenance around host DMA; no engine work. |
| `0x51` | PIXELDMA_CLEANUP | `0x7c` / 8 | `+0x14` `pktOffset`, `+0x18` `tlbDesc`. Out: `nTlbSlots`, `status` | Accepted only if the node is drained (`chunkCount` 0, status bit `0x20`) and the engine stopped exactly at `pktOffset` (`savedPktOffset`): the host buffer is then unbound and freed (request `0x232054`) and the slot count returned. |
| `0x52` | SETUP_SCR2SCR | `0x7c` / none | `+0x14` `pktOffset`, then `ddFlags` `+0x18`, `glFlags` `+0x1c`, two more words | Drop type 2 (screen-to-screen copy set-up); the three flag bits are folded into the first drop word. |
| `0x53` | CREATE_BUFFER | `0x28` / `0x14` | `+0x14` `width`, `+0x18` `height` (each at most `0x4000`), `+0x1c` `pageSize` (at most 4), `+0x20` `bufType` (1 or 2), `+0x24` `memAllocHint` (low half 0 or 5 to 7, high half 0 or 1). Out: `bufId`, `numTiles`, `tilesPerRow`, `pageSize` class, `flags` | Rounds the width to a tile multiple and allocates a tiled buffer from the miniport's tile pool (`CoGrmAllocateTiledBuffer`, request `0x232048`); if that fails it retries with the allocation chunk shrunk by four each time down to one page. Adds the buffer to the context's table and returns its id (the table key, with bit 0 set when the buffer record's flag 1 is set). |
| `0x54` | FREE_BUFFERS | `0x40` / none | `+0x14` `count` (at most 9), `+0x18` ids | Allowed only on a drained node. Removes each from the context and frees those whose reference count reaches zero. |
| `0x55` | BIND_BUFFERS | `0x70` / 8 | `+0x14` `pktOffset`, `+0x18` `count` (at most 9), `+0x1c` `resetCurrentTexture`, `+0x20` ids, `+0x48` descriptor ids (2 to `0x3f`). Out: `nTlbSlots`, `status` | Drop type 7. Binds each buffer to its descriptor id in the driver's bookkeeping now. When the drop runs (`CoScheduleBindBuffers@0x19662`) the miniport records the buffers in the node, adjusts its slot count, marks each buffer's translation entries for reload and sets the node flag that makes the next switch-in re-send the table; it clears the current-texture register (and the node's copy) if `resetCurrentTexture` is set or a rebound descriptor lies in the current texture's range. |
| `0x56` | UNBIND_BUFFERS | `0x6c` / 8 | `+0x14` `pktOffset`, `+0x18` `count`, `+0x1c` ids, `+0x44` descriptor ids | Drop type 8, the converse. |
| `0x57` | CREATE_TEXTLBPKT | `0x44` / `0x18` | `+0x14` `descId` (above 10, with `descId + count` at most `0x40`), `+0x18` `count` (at most 9), `+0x1c` ids. Out: `tlbPktOffset`, `rtrnPktAddr`, `rtrnOffset`, `rtrnFlushOffset`, `nTlbSlots`, `texPktId` | Checks every id is in the context's shared-buffer table and asks the miniport (request `0x2320bc`, `0x38` bytes in, `0x18` out) to build a prebuilt table-load packet in a packet page and file it under a new id in the node's texture-packet table (`CoBuildUserTexTlbPacket@0x17a46`). The ICD chains this packet between its own nodes to bind a texture. |
| `0x58` | FREE_TEXTLBPKT | `0x40` / none | `+0x14` `count`, `+0x18` ids | Miniport request `0x2320c0` per id. |
| `0x59` | SET_READY_TEXTLBPKT | `0x40` / none | `+0x14` `count`, `+0x18` ids | For each id, copies a stored link field into the packet's own record in the display driver's table. No miniport call. What it arms is inferred to be the packet's chaining; not traced further. |
| `0x5a` | CREATE_PBUFFER | `0x50` / 8 | `+0x14` `hWnd`, `+0x18` `width`, `+0x1c` `height`, `+0x20` `pageSize`, `+0x24` `pfdID`, `+0x28` a `PIXELFORMATDESCRIPTOR` (40 bytes). Out: `pbufID`, `status` | Creates the pixel-buffer record; its buffers are allocated at the first `SETCONTEXT` that uses it. |
| `0x5b` | DELETE_PBUFFER | `0x1c` / 4 | `+0x14` `pbufID`, `+0x18` `hWnd`. Out: `status` | Frees the record and its buffers. |
| `0x5d` | PIXELFORMATEXTDESCRIBE | `0x20` / `0x30` | `+0x14` `screenFormats`, `+0x18` `iPixelFormat`, `+0x1c` `nBytes`. Out: `status`, one `PIXELFORMATDESCRIPTOR`, `iNumPixelInfoEXT` | Builds the extended format list for the current screen formats (`DrvOpenGLBppEXT`) and describes entry `iPixelFormat`. The list itself is [question 12](../../questions/open-questions.md). |
| `0x60` | GET_GAMMA | none / `0x1c` | output `CO_GAMMA`: header, `TableSize`, a table | `__glGetGamma`. The ICD never sends it. |
| `0x61` | SET_GAMMA | none (not checked) / `0x1c` | the same structure, read from the input buffer, whose length the driver does not check | `__glSetGamma`. The ICD never sends it; another component does. |
| `0x62` | WAIT_FOR_HARDWARE | `0x18` / 4 | `+0x14` `pktSerial`. Out: `goToSleep` | Asks the miniport whether to sleep (`pCoWaitForHardwareLoad@0x1a1ee`): answer 0 when the node is already running on the engine (its saved position is the marker `0xbabeface`), when the serial is not newer than the last one the stop handler recorded, or on a small wrap case; otherwise it sets bit 8 in the node's waiting-events mask and answers 1. The miniport signals the event when it next **loads this node onto the engine** (`pCoContextSwitch`), not when it stops. See the correction below. |
| `0x63` | ALLOC_PACKET_BUFFER | `0x18` / `0x18` | `+0x14` `packetSize`. Out: as `FIRSTSETCONTEXT` | Another pool for the same node (miniport request `0x2320b4`, `0x1c` bytes in and out), mapped with the node's mapping mode. |
| `0x64` | FREE_PACKET_BUFFER | `0x18` / none | `+0x14` `baseOffset` | Miniport request `0x2320b8`: node and base offset. |

Codes the display driver does not act on (the default branch returns 0): `0x45` (copy context), `0x49` (get layer
palette), `0x4a` (realize layer palette), `0x5c`, `0x5e`, `0x5f` (extended pixel-format attribute, choose and set/get) and
`0x65` (a simulator load-buffer code). The enumeration in the vendor's symbols names all seven; the ICD sends none of
them. Established.

### What `WAIT_FOR_HARDWARE` really waits for

[graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)
says the sleeper is woken from the stop interrupt. The routines say otherwise. `pCoContextSwitch` tests bit 8 of the node's
waiting-events mask after it has written the start word for the node (`pCoContextSwitch@0x16610`)
and signals event 8. The miniport's node set-up (`pCoAllocateRNode`) fills event 1 from the `waitobj` handle and event 8 from the
`ctxLoadObj` handle of `FIRSTSETCONTEXT` and creates the rest itself. The ICD keeps `ctxLoadObj` at `+0x2e324` of its context
and sleeps on it in the loop that sends `0x62`; `waitobj` is at `+0x2e320` and is what `0x4c` waits on. So the busy-wait ends in a sleep only to wait for the scheduler to **start**
the node; completion of the node is always seen by polling bit 0 of node dword 0. Established for the drivers. An emulator
that runs every queued node to completion before returning from the escape can answer `goToSleep` = 0 always.

## Private escapes and the other entry points

`DrvOpenGLEscape` hands every code that is not `OPENGL_CMD`, `0x3f` or `OPENGL_GETINFO` to `DrvEscape@0x21854`.
Established, NT 4.0 build; the Windows 2000 build's equivalent carries the same codes (instruction shape compared, not read
line by line).

- `QUERYESCSUPPORT` (8) answers 1 for `0x1100` and `0x1101` and 0 otherwise.
- `OPENGL_GETINFO` (`0x1101`) with a zero input checks the output is `0x214` bytes, writes the version words `2`, `2` and
  the wide string `sgi` at `+8`. This is how `opengl32` finds the ICD's registry name (not a file name: the file name `sgcoicd.dll` is the registry value's
  data); both builds answer 2, 2 and `sgi` ([registration](nt-opengl-icd.md#registration-and-the-version-handshake)).
- `0x232088`, `0x23208c`, `0x232090`, `0x232094`, `0x232098`, `0x23209c`, `0x2320a0`, `0x2320a4`, `0x2320ac`, `0x2320b0` and
  `0x2320c8` are passed unchanged to the miniport through `EngDeviceIoControl`, so the control-panel applet can reach its
  video requests. The OpenGL driver never sends them. `0x2320ac` returns the number of tile banks less one, `0x2320b0` a
  table of bank base and size, `0x2320c8` a power state (to `asSetPowerState`).
- `0xcace` writes 0 to the first output dword and `0xcaca` writes `0xe0` at `+4`; both return success. Their purpose is not
  known.
- `0x11` is a private control code with a sub-code in the first input dword: 1 and `0xb` only check the input length
  (`0x601c`, `0xc`); 3 sets a tile-flip flag (`bFlipTiles`); 5 sets the enable flags of the off-screen-bitmap creation and move
  paths and can free all non-permanent off-screen bitmaps; 6 runs the driver's line-to fix-up for a test; 7 stores a debug
  value; 9 stores the vertical-blank fudge. Developer switches; no shipped caller found among the OpenGL escapes.
- The display driver's own requests to the miniport (not escapes) are numbered `0x230400` to `0x230478` (video modes, memory
  mapping) and `0x232048` to `0x2320c8` (buffers, rendering nodes, drawables, packet pools). The ones the escapes above use
  are named in the table; `0x232058` and `0x23205c` allocate and free a rendering node, `0x232064` and `0x232068` allocate
  and free a drawable record, `0x23207c` is sent once at start-up, and `0x232080` is the DirectDraw flip.

## Window and clip information

The ICD sends no window geometry at all. The display driver learns it from GDI: it creates a window object for the
drawable (`EngCreateWnd`, `WNDOBJ_vSetConsumer`) and receives clip changes through a callback
(`CoGrmWndChangeProc@0x19e36`). On a change it suspends every rendering node bound to that window, rebuilds the
drawable's client and clip regions from the window object (`pCoGrmUpdateDrawable`), refreshes the clip identifiers and the
overlay layer (`vUpdateCIDs`, `vValidateLayer`), and resumes the scheduler (a final notification calls `DrvSynchronize`
first). On window destruction the nodes are rebound to a "no clip" drawable. The window parameters the engine sees are
packets the miniport builds at each context switch (`CoBuildWinparamsPacket`), from the drawable record. Established for
the NT 4.0 build.

An emulator or port with no GDI window objects needs to supply, per context: the clip rectangle list, the window origin
and the clip-identifier mode of its drawable, refreshed whenever the window moves, resizes or is covered, with the node
suspended while it changes. Which of these the engine needs is in [the engine page](../../devices/cobalt-graphics/graphics-engine.md).

## The Windows 2000 build

- **Established.** `DrvOpenGLEscape` in the Windows 2000 display driver has the same switch with the same 31 codes, and every
  `checkargs` call carries the same name, input size and output size (compared mechanically across all 31 cases). The
  default branch and the pass-through to the private handler are the same.
- **Established.** The Windows 2000 ICD has 33 call sites of its escape wrapper with the same (input size, output size) pairs
  as the NT 4.0 ICD, and the code of each site read from the Windows 2000 file equals the table above.
- **Inferred.** The field offsets the Windows 2000 routine reads for each code are the same: the decompiler shows them as
  dword indices and the sets agree where visible, but the Windows 2000 build's type declarations could not be loaded, so the
  structures were not diffed field by field.
- The two builds share one numbering, unlike the April 1999 build (see the table in graphics-engine.md).

## What an emulator author can leave out

Unknowns are the author's to choose; none of these has a software dependency beyond what is stated.

- `GET_GAMMA` and `SET_GAMMA`, `0xcace`, `0xcaca` and the `0x11` group: the OpenGL driver does not send them.
- The drop types behind `CACHEOP`, `SETCONTEXT` and the pixel-buffer codes need no engine behaviour beyond the table and
  packets already recorded; the choice is whether a port keeps the kernel-side bookkeeping (descriptor slots, shared-buffer
  table, node queue) or replaces it with its own.
- What `SET_READY_TEXTLBPKT` arms inside the display driver's packet record is unknown; see
  [the open questions](../../questions/open-questions.md).
