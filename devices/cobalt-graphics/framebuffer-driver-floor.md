# Frame-buffer-only driver: the smallest set of facts a Windows display driver needs

What someone writing a display driver without 3D for the Visual Workstation 320 and 540 (a replacement HAL and miniport
for Windows XP, or a model that has to run SGI's own drivers) has to get right, how well each item is known, and where
the answer comes from. Back to the [overview](README.md). The display back end's registers are in
[display-engine.md](display-engine.md), the engine in [graphics-engine.md](graphics-engine.md); this page only picks
from them and adds what the NT drivers ask of the operating system.

Ratings are the repository's three ([AGENTS.md](../../AGENTS.md)): a statement about what a shipped routine does is
**established** when the routine was read in full (the NT 4.0 5.2.4 miniport `sgcomp.sys` and display driver `sgcodd.dll`,
read in Ghidra with the vendor's symbols and types applied); what the hardware does in response is **inferred**; anything
not stated is **unknown**. No register has a known reset value. The Windows 2000 CD drivers (`sgibvid.dll`, `sglfb.sys`,
`sglfb.dll`) are described in [their own page](../../software/windows/w2k-display-drivers.md) and were not re-read for this one.

## 1. Registers a frame-buffer driver needs

All in the display back end at physical `0xd0000000` (the NT miniport maps `0x80020` bytes), unless the row says
otherwise. Every access the NT drivers make is a 32-bit load or store, with one exception: the stop routines poll single
bytes of three live words (`+0x30008`, `+0x20004` and `+0x40002`, the last being bit 16 of `+0x40000`).

| Need | Registers and what the NT driver does | Known | Source |
|---|---|---|---|
| Identify the part | `id` (`+0x14`) must read `0x666` (low 16 bits); anything else and the driver does not attach | established (the check), inferred (what the chip is) | `CobaltInitialize`; both Windows 2000 drivers |
| Mode set, order | stop the interrupt, normal, overlay and DID fetches; dot clock sequence; the timing window words; `config`; the tile and frame size words; `ctrlstat` = `0x30000000` last | established (the order the NT driver writes), inferred (hardware effect) | `asSetConfigAndTiming`, [display-engine.md](display-engine.md#video-timing-block-0x10000) |
| Pixel clock | `dotclock` (`+0x04`): `((N-1) << 8) \| (P << 14) \| (M-1)`, then `RUN` (bit 20) after 40 ms; 27 MHz times `M/N` divided by `2^P` fits 26 of the 27 table rows to 1.4 percent. No lock bit is read. The one-second restart dance is reproduced, its purpose unknown | established (writes), inferred (formula) | `asSetConfigAndTiming`; table `cobaltVTimings` |
| Timing | `vt_xymax`, `vt_vsync`, `vt_hsync`, `vt_vblank`, `vt_hblank`, `vt_hpixen`, `vt_vpixen`, `vt_hcmap`, `vt_vcmap`, `did_start_xy`, `crs_start_xy`, `vc_start_xy`, `fp_*`, `vt_f2rf_lock`, from the 27-row mode record | established (every field written), inferred (field meanings from the O2's display back end) | `asSetConfigAndTiming`, [mode table](display-engine.md#mode-table) |
| Configuration | `config` (`+0x18`): `0xe5`, `0xe7` for sync on green, `0x08` always set, bit 0 cleared when a flat panel answered; a model should accept any value | established (writes), inferred (bits) | `asSetConfigAndTiming` |
| Scan-out base and stride | `frm_size_tile` (whole 512-byte tile columns in `[12:5]`, remainder in 32-byte units in `[4:0]`, depth in `[14:13]`, FIFO reset bit 15, written twice around the pulse), `frm_size_pixel` (height in `[31:16]`), `frm_control` (`+0x3000c`): tile-list physical address, bit 0 DMA enable, optionally bit 1 linear (physical address >> 9 when set) | established (NT writes the tile list form; Linux and the Windows 2000 drivers write the linear form), inferred (hardware reading) | `asStartFrm`; [display-engine.md](display-engine.md#overlay-normal-plane-did) |
| Tile list in memory | five non-cached lists of 16-bit entries, 512-byte aligned, one entry per 64 KB tile (`tileAndBank >> 4`), `(height + 127) / 128` rows of `widthintiles` entries; lists 0 to 3 are the normal plane's (the current index is flipped with an XOR of 1), list 4 is the overlay's | established (what is built), inferred (the engine's reading, taken from the O2's display back end) | `asStartFrm`, `CobaltInitialize` |
| Page flip and its completion | rewrite the list, write `frm_control`; completion is the live copy `frm_inhwctrl` (`+0x30008`) matching the value written, polled by `ddrvalUpdateFlipStatus` | established, inferred | `asFlipTiles`, `DdFlip`, `ddrvalUpdateFlipStatus` |
| Stop fetch | clear bit 0 of the control word (bit 16 for the DID), poll the live word up to 20,000 times with a 10 microsecond stall | established | `asStopNormal`, `asStopOverlay`, `asStopDID` |
| Colour map (8-bit modes) | `+0x50000` entries, one dword each as `R << 24 \| G << 16 \| B << 8`; before every 32nd entry, spin until `+0x58000` shows a low six bits of 0 or a value of `0x30` or more | established (writes), inferred (FIFO meaning) | `asSetColorRegisters` |
| Gamma (optional) | `+0x60000` (256 dwords) and `+0x68000` (1,024 dwords), plain dword stores; identity ramp `i * 0x01010100` is what the Windows 2000 boot driver writes | established (NT writes), inferred (meaning) | `GammaDataCallback`, IOCTL `0x23208c` |
| Hardware cursor (optional) | `+0x70000` position `((y - hotY) & 0xfff) << 16 \| ((x - hotX) & 0xfff)`, `+0x70004` control (1 on, 0 off, parked at `0x0fe00fe0`), `+0x70008` three colours, `+0x78000` 64 dwords of 2-bit pixels, 32 by 32; a driver may draw the pointer in software instead, as `sgcodd` does for pointers that do not fit | established (display driver writes), inferred (meaning from the O2 layout) | `vMovePointer`, `bCopyColorPointerIntoHardware`, [display-engine.md](display-engine.md#colour-map-gamma-cursor-video-capture) |
| Scan line and vertical blank | read `vt_xy` (`+0x10000`) bits 23:12; the NT driver treats a 48-line window starting at the screen height less a fudge as "in blank". For a retrace interrupt, the register at `0xca000000` (read, then write its complement) and the Cobalt interrupt line 24 | established (reads and the ISR), unknown (what the `0xca000000` bits are beyond bit 1) | `asGetScanline`, `VBLANK_IS_ACTIVE`, `CoVRetraceHandler` |
| Blanking and DPMS | `vt_flags` (`+0x10018`) bits 1 and 3 (standby, suspend); with a flat panel, the panel's power-save bit over I2C | established (writes), inferred (which signal each gates) | `asSetPowerState` |
| Monitor and panel identity | CRT: DDC over `i2c` (`+0x08`, bit 0 SDA, bit 1 SCL, 1 pulls low), device write addresses `0xa2`, `0xa6`, `0xa0`, three tries each (when a flat panel was found the same EDID reader uses `i2cfp` instead); flat panel: `i2cfp` (`+0x10`), device `0x70`/`0x71`, panel id in bits 7:5 of the status byte; the Windows 2000 drivers instead read back `fp_hdrv` and `fp_vdrv` | established | `i2cHW*`, `mli2cFlatPanelStatus`, `CoDDC_GetEDID` |
| Not needed | the engine window (`0xc8000000`), video capture (`+0x80000`), `mode_regs` for the SGI 5.2.4 miniport (the Windows 2000 drivers do write it), the system clock PLL (`+0x0c`) | | |

Two cautions. The `fp_vdrv` row was wrong before this re-check: the miniport writes `0x1000 | (H + 1)`, so a panel probe
by readback sees `0x401` in the low 12 bits on a 1024-row panel in both drivers. And the pixel byte order is not settled:
the NT table and the Windows 2000 drivers put red in the top byte, Linux in the low byte
([nt-software.md](nt-software.md#linux-sgivwfb)).

## 2. What `sgcomp` and `sgcodd` need from the HAL and the NT driver model

**The HAL.** `DriverEntry` ends in `VideoPortInitialize`, so the driver is a video miniport. `CobaltFindAdapter` (system I/O
bus 0 only) calls `HalDispatchTable->HalQuerySystemInformation(0x8100001, 16, ...)` and prints "cobalt: Not an SGI HAL" and
stops when the status is not zero. The answer fills the device extension's `CacheOpInfo`: three cache-flush routines (for a
memory descriptor list, a physical buffer and a virtual buffer) and a one-byte invalidate flag; the miniport calls them
through that record and nothing else of the HAL's. Established (the HAL's side is read in
[the chips HAL page](https://github.com/jamesbraid/chips/blob/main/software/visws/nt-hal.md#the-sgi-specific-hal-interface)).
A HAL that returns success for that class and supplies the three routines satisfies the check; the 3D path depends on them
for cache coherence, a frame-buffer-only driver does not call them (inferred).

| What | How the miniport asks | Source |
|---|---|---|
| Interrupt vectors | `HalGetInterruptVector(Internal, bus 0, level 0x19, vector 0x19)` for the engine and `(0x18, 0x18)` for retrace; each `IoConnectInterrupt` is latched (mode 1), not shared, with one shared spin lock and the larger level as the synchronise level; no floating-point save. A frame-buffer driver needs only line 24 | `CobaltInitialize`; [interrupt delivery in the HAL](https://github.com/jamesbraid/chips/blob/main/software/visws/nt-hal.md#interrupt-delivery-in-the-hal), [the Cobalt controller](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md#interrupt-controller-the-cobalt-apic), [line assignments](https://github.com/jamesbraid/chips/blob/main/platforms/visws/interrupts.md#cobalt-lines-established-from-the-header) |
| Interrupt acknowledge | the driver does no end-of-interrupt write to the controller; its handlers clear only their own source (below). The controller's acknowledge sequence is the HAL's | `CoInterruptHandler`, `CoVRetraceHandler` |
| Mapped ranges | `MmMapIoSpace` (uncached) of `0xd0000000` `0x80020`; `0xc8000000` `0x19000` (engine); `0xca000000` 4; `0xc0000020` 4 (both only after the ID check); `0xc2000008` 8 (revision, mapped and released in `CobaltFindAdapter`) | `CobaltInitialize`, `CobaltFindAdapter` |
| Resource claim | `IoAssignResources` for `0xd0000000` to `0xd008001f`, only when `PsGetVersion` reports a major version above 4 | `CobaltFindAdapter` |
| Revision check | high nibble `0xa`, low nibble neither 0 nor 8; the revision read at `0xc2000008` is read once, in `CobaltFindAdapter`, before anything else; no routine reads it after an interrupt handler | `CobaltFindAdapter` |
| Memory | the registry key `SGICtrl\Memory` under `Control`: `NumberOfBanks`, `BankAddr00`..`31`, `BankSize00`..`31`; fewer than two banks and the driver prints "GRAPHICS: no preallocated memory" and bug-checks (`0x5a`). The HAL writes them ([the HAL's side](https://github.com/jamesbraid/chips/blob/main/software/visws/boot-path.md)). A Windows 2000 `sglfb` needs only `GraphicsMemory` and `GraphicsMemorySize` | `pCoBankLoadBanks`; [w2k-display-drivers.md](../../software/windows/w2k-display-drivers.md) |
| Persisting the memory profile | `HalSetEnvironmentVariable("cogfxmem", "count,pages,...")` from a private IOCTL (`0x232094`) | `CobaltStartIO` |
| Aperture and physical memory | the pool and surfaces are mapped with `MmMapIoSpace` at `0x80000000`, `0x82000000`, `0x85000000`, `0x87000000` (16 MB each), and the miniport opens `\Device\PhysicalMemory` with `ZwOpenSection` and `ZwMapViewOfSection` to map packet buffers into the calling process | `pCoInitializePacketPool`, `pCoMapPktMem` |
| Memory for tile lists | `MmAllocateNonCachedMemory` of `0xbff` bytes, carved into five 512-byte lists, `MmGetPhysicalAddress` for each | `CobaltInitialize` |
| Registry | `...\Software\Silicon Graphics\Control Panel\Display` (`GammaData`, `EnableDDCSupport`, `15Is16BitColor`, timing records) and `...\Graphics` (`GdiOnOverlay`, the scheduler tuning values) | `CobaltInitialize` |

**The video port IOCTLs.** The miniport answers the stock codes at `0x230400`: available modes, mode count, current mode,
set mode, reset, colour registers (`0x23041c`), map and unmap video memory (`0x230458`, `0x23045c`), and the public access
range codes (`0x230460`, `0x230478`). Its private range is `0x23204c` to `0x2320c8` (packet buffers, render nodes, texture
table packets, flip `0x232080`, vertical-blank event `0x232084`, gamma `0x23208c`, memory profile `0x232094`, power state
`0x2320c8`); a frame-buffer-only driver needs only the stock codes plus flip and power if it wants them. Established (the
dispatch in `CobaltStartIO`); the names of the stock codes are inferred from their numbers and what each branch does.

**The display driver.** `sgcodd.dll` is a GDI display driver (45 imports from `WIN32K.SYS` in the Windows 2000 build: the
Eng* surface, bitmap, palette, clip and driver-object calls, and `EngSaveFloatingPointState` and
`EngRestoreFloatingPointState`, which the NT 4.0 build replaces by `EngFindImageProcAddress`). It talks to the miniport with
`EngDeviceIoControl`. A frame-buffer-only driver replaces it; the Windows 2000 `sglfb` pair is Microsoft's and draws in software.

## 3. Would the Windows 2000 builds load on Windows XP (32-bit) unchanged?

Inferred at best: nothing here was tried. What the binaries say.

- **Imports.** The Windows 2000 `sgcomp.sys` (65,808 bytes) imports 7 functions from `VIDEOPRT.SYS`, 53 from `ntoskrnl.exe` and
  4 from `HAL.dll`. It differs from the NT 4.0 build only in dropping `HalDisplayString` and `RtlUnwind` and adding
  `_except_handler3`. The ones that decide whether it loads and runs: `VideoPortInitialize` (the miniport model
  itself; the NT 4.0 build passes a 0x28-byte `HwInitDataSize` and the Windows 2000 build a 0x50-byte one, with the same
  find, initialise, start-I/O and reset callbacks and a zero-filled remainder; whether XP's video port accepts either size
  was not checked); `HalDispatchTable` (the SGI HAL query); `IoAssignResources`
  and `IoConnectInterrupt` with `HalGetInterruptVector` (legacy resource and interrupt claiming, which an ACPI HAL may not
  serve for fixed Cobalt addresses); `MmMapIoSpace` of the fixed ranges above; `ExInitializeZone` and `ExExtendZone`
  (the old zone allocator); `ZwOpenSection` of `\Device\PhysicalMemory`; `HalSetEnvironmentVariable`. Whether each is still exported
  by the Windows XP kernel and HAL, and whether the physical-memory section may be opened by a kernel caller on the XP service
  packs in question, was not checked.
- **Driver model.** XP still runs XPDM video miniports and GDI display drivers, which is what these are. The 3D side is
  separate: the OpenGL ICD (`sgcoicd.dll`) passes escape codes through GDI `ExtEscape` and links nothing from the kernel.
  The two builds of the escape numbering differ (the April 1999 and the 5.2.4 tables are not interchangeable,
  [graphics-engine.md](graphics-engine.md#how-the-opengl-driver-builds-and-submits-packets-sgcoicddll-524)); the Windows 2000 and NT 4.0
  5.2.4 display drivers carry the same table.
- **Platform.** The drivers are 32-bit; a 64-bit Windows would need new ones written from the register facts above.
  The miniport also needs an SGI HAL (the query above) and the HAL's `SGICtrl\Memory` values, so "load unchanged" is
  about the files, not the machine: a stock XP HAL does not answer class `0x8100001`.

## 4. Hot paths

All accesses are 32-bit and uncached; the miniport never uses a write barrier or a read-back other than those listed. An
emulator that sequences these correctly passes the NT drivers' loops.

| Path | Accesses, in order | Notes |
|---|---|---|
| Start a packet chain | node's context-switch packets written in memory; for a switch from another node: `0x18800` write (its dump offset), poll `0x10018` until bit 31 reads set, write `0x10018` = `0x7fffffff`; for a restart of the node already running: write `0` to `0x185c8`; then `0x10000` = `pktIdleCount \| (position << 12) \| 0x800` | `pCoContextSwitch`; the start word is the last write |
| Doorbell | `0x10008` = `0x400` after work is appended or a node is resumed | three routines, conditions in [graphics-engine.md](graphics-engine.md) |
| Engine interrupt (pin 25) | read `0x10018` (zero means not ours, counted as spurious); mask with `0x60000000`; write `0`; re-read `0x10018` until its masked bits differ from the value read; queue a deferred routine | the one read-after-write: the clear must be visible before the handler returns |
| Stop-event deferred routine | read `0x10000` (position in bits 31:12), or `0x185c0` when that reads 0; read `0x185c0`, `0x185c8` and, for an OpenGL node, `0x185d0`; for a stop inside a state-restore page rewrite `0x10000` with the same position | `CoProcessCobaltEvent` |
| Retrace interrupt (pin 24) | read the dword at `0xca000000`; write its complement; if bit 1 was set count a frame; in stereo also read-modify-write `vt_flags` | no re-read |
| Enable and park | `CoInitHw`: `0x10000` = `0xfffff040`, `0x10018` = 0, `0x10020` = `0x60000000`; `asStopIntr`: `vt_intr01` and `vt_intr23` = `0xffffff` | |
| Scan line poll | read `vt_xy`, bits 23:12 | `VBLANK_IS_ACTIVE` |
| Flip | write `frm_control`; poll `frm_inhwctrl` against the value written | |

**DMA into and out of system memory** (all inferred from the driver's side; no document describes the engine's bus
behaviour): the display back end reads the five tile lists and then the frame, overlay and DID data they name; the
rendering engine reads packet nodes from the pool (bank 0 memory, 16 tiles of 1 MB, named through the engine's translation
table, [packet-set.md](packet-set.md#texture-table-packets-class-11)) and reads and writes the colour, depth and texture
buffers in the graphics banks and in user memory the driver locks (`MmProbeAndLockPages`) and loads into table entries for pixel
transfers; the engine writes the 24-bit sequence and node registers the stop routine reads back and stores context-switch state at the
dump offset it is handed. The video capture and output ports have no driver in the NT 4.0 pack's display path.
