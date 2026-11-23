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
otherwise. Every access the NT drivers make is a 32-bit load or store, with one exception: byte loads. The stop routines poll single
bytes of three live words (`+0x30008`, `+0x20004` and `+0x40002`, the last being bit 16 of `+0x40000`), and the mode set, the
firmware, the NT 4.0 HAL and `sglfb.sys` test the freeze bit with a byte load of `+0x10003` (bit 7).

| Need | Registers and what the NT driver does | Known | Source |
|---|---|---|---|
| Identify the part | `id` (`+0x14`) must read `0x666`; the SGI miniport compares the whole dword (`cmp dword [eax+0x14],0x666` at `sgcomp.sys@0x1196f`), the Windows 2000 CD drivers only the low 16 bits; anything else and the driver does not attach | established (the check), inferred (what the chip is) | `CobaltInitialize`; both Windows 2000 drivers ([page](../../software/windows/w2k-display-drivers.md)) |
| Mode set, order | stop the interrupt, normal, overlay and DID fetches; dot clock sequence; the timing window words; `config`; the tile and frame size words; `ctrlstat` = `0x30000000` last | established (the order the NT driver writes), inferred (hardware effect) | `asSetConfigAndTiming`, [display-engine.md](display-engine.md#video-timing-block-0x10000) |
| Pixel clock | `dotclock` (`+0x04`): `((N-1) << 8) \| (P << 14) \| (M-1)`, then `RUN` (bit 20) after 40 ms; 27 MHz times `M/N` divided by `2^P` fits 26 of the 27 table rows to 1.4 percent. No lock bit is read. The one-second restart dance is reproduced, its purpose unknown | established (writes), inferred (formula) | `asSetConfigAndTiming`; table `cobaltVTimings` |
| Timing | `vt_xymax`, `vt_vsync`, `vt_hsync`, `vt_vblank`, `vt_hblank`, `vt_hpixen`, `vt_vpixen`, `vt_hcmap`, `vt_vcmap`, `did_start_xy`, `crs_start_xy`, `vc_start_xy`, `fp_*`, `vt_f2rf_lock`, from the 27-row mode record | established (every field written), inferred (field meanings from the O2's display back end) | `asSetConfigAndTiming`, [mode table](display-engine.md#mode-table) |
| Configuration | `config` (`+0x18`): `0xe5`, `0xe7` for sync on green, `0x08` always set, bit 0 cleared when a flat panel answered; a model should accept any value | established (writes), inferred (bits) | `asSetConfigAndTiming` |
| Scan-out base and stride | `frm_size_tile` (whole 512-byte tile columns in `[12:5]`, remainder in 32-byte units in `[4:0]`, depth in `[14:13]`, FIFO reset bit 15, written twice around the pulse), `frm_size_pixel` (height in `[31:16]`), `frm_control` (`+0x3000c`): tile-list physical address, bit 0 DMA enable, optionally bit 1 linear (physical address >> 9 when set) | established (NT writes the tile list form; Linux and the Windows 2000 drivers write the linear form), inferred (hardware reading) | `asStartFrm`; [display-engine.md](display-engine.md#overlay-normal-plane-did) |
| Tile list in memory | five non-cached lists of 16-bit entries, 512-byte aligned, one entry per 64 KB tile (`tileAndBank >> 4`, the tile's first 4 KB frame number shifted right 4 and truncated to 16 bits, so bit 15 is physical address bit 31 and no other bit is an attribute; all four routines that write a list, `sgcomp.sys@0x1c094`, `0x1c1d8`, `0x1c2c8` and `0x1c386`, store that and nothing else, and none ORs in `0x8000` or any address offset), `(height + 127) / 128` rows of `widthintiles` entries; lists 0 to 3 are the normal plane's (the current index is flipped with an XOR of 1), list 4 is the overlay's | established (what is built), inferred (the engine's reading, taken from the O2's display back end) | `asStartFrm`, `CobaltInitialize` |
| Page flip and its completion | rewrite the list, write `frm_control`; completion is the live copy `frm_inhwctrl` (`+0x30008`) matching the value written, polled by `ddrvalUpdateFlipStatus` | established, inferred | `asFlipTiles`, `DdFlip`, `ddrvalUpdateFlipStatus` |
| Stop fetch | clear bit 0 of the control word (bit 16 for the DID), poll the live word up to 20,000 times with a 10 microsecond stall | established | `asStopNormal`, `asStopOverlay`, `asStopDID` |
| Colour map (8-bit modes) | `+0x50000` entries, one dword each as `R << 24 \| G << 16 \| B << 8`; before every 32nd entry, spin until `+0x58000` shows a low six bits of 0 or a value of `0x30` or more. The Windows 2000 boot video driver is stricter: before every entry it waits for the low six bits to read above `0x30`, up to 1,000 tries of 10 microseconds, then 100 microseconds after the write. A status of `0x3f` satisfies every reader here; `0x40` does not | established (writes), inferred (FIFO meaning) | `asSetColorRegisters` (`sgcomp.sys@0x1c302`); `sgibvid.dll@0x80010c6e` |
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
| Revision check | high nibble `0xa`, low nibble neither 0 nor 8; the register is read once, in `CobaltFindAdapter`, and the copy kept at `+0x1180` of the device extension is read in `CobaltInitialize` (`0x11f5c`) to build the `HardwareInformation.ChipType` string; no routine reads the register after an interrupt handler | `CobaltFindAdapter` |
| Memory | the registry key `SGICtrl\Memory` under `Control`: `NumberOfBanks`, `BankAddr00`..`31`, `BankSize00`..`31`; fewer than two banks and the driver prints "GRAPHICS: no preallocated memory" and bug-checks (`0x5a`). The HAL writes them ([the HAL's side](https://github.com/jamesbraid/chips/blob/main/software/visws/boot-path.md)). A Windows 2000 `sglfb` needs only `GraphicsMemory` and `GraphicsMemorySize` | `pCoBankLoadBanks`; [w2k-display-drivers.md](../../software/windows/w2k-display-drivers.md) |
| Persisting the memory profile | `HalSetEnvironmentVariable("cogfxmem", "count,pages,...")` from a private IOCTL (`0x232094`) | `CobaltStartIO` |
| Aperture and physical memory | the pool and surfaces are mapped with `MmMapIoSpace` at `0x80000000`, `0x82000000`, `0x85000000`, `0x86000000` (16 MB each; descriptor ids 0, 2, 5, 6; the id 2 window is the frame buffer the video port returns, [format and reading](graphics-engine.md#cpu-aperture-windows-at-0x80000000)), and the miniport opens `\Device\PhysicalMemory` with `ZwOpenSection` and `ZwMapViewOfSection` to map packet buffers into the calling process | `pCoInitializePacketPool`, `pCoMapPktMem` |
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
`EngRestoreFloatingPointState`, plus `EngAlphaBlend`, `EngGradientFill` and `EngModifySurface`, which the NT 4.0 build replaces by `EngFindImageProcAddress`). It talks to the miniport with
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

All accesses are 32-bit and uncached; the miniport uses no write barrier, and its only read-backs are those listed (including the discarded `+0x1001c` load and the second `vt_flags` write in the retrace handler). An
emulator that sequences these correctly passes the NT drivers' loops.

| Path | Accesses, in order | Notes |
|---|---|---|
| Start a packet chain | node's context-switch packets written in memory; for a switch from another node: `0x18800` write (its dump offset), poll `0x10018` until bit 31 reads set, write `0x10018` = `0x7fffffff`; for a restart of the node already running: write `0` to `0x185c8`; then `0x10000` = `pktIdleCount \| (position << 12) \| 0x800` | `pCoContextSwitch`; the start word is the last write |
| Doorbell | `0x10008` = `0x400` after work is appended or a node is resumed | three routines, conditions in [graphics-engine.md](graphics-engine.md) |
| Engine interrupt (pin 25) | a load of `0x10000` whose result is discarded (`CoInterruptHandler`, `sgcomp.sys@0x17bfa`), then read `0x10018` (zero means not ours, counted as spurious); mask with `0x60000000`; write `0`; re-read `0x10018` until its masked bits differ from the value read; queue a deferred routine | the one read-after-write: the clear must be visible before the handler returns |
| Stop-event deferred routine | read `0x10000` (position in bits 31:12), or `0x185c0` when that reads 0; read `0x185c0`, `0x185c8` and, for an OpenGL node, `0x185d0`; for a stop inside a state-restore page rewrite `0x10000` with the same position | `CoProcessCobaltEvent` |
| Retrace interrupt (pin 24) | read the dword at `0xca000000`; write its complement; if bit 1 was set count a frame; in stereo also a discarded load of `vt_f2rf_lock` (`+0x1001c`), then a read-modify-write of `vt_flags` and, when `bFreeRun` is set, a second write of `vt_flags` with bit 6 cleared | no re-read of `0xca000000`; the discarded `+0x1001c` load may have side effects |
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

## 5. Status: what must be bit-exact and what may be approximated

Re-read on 2026-11-22 from the disassembly (not the decompile) of the NT 4.0 5.2.4 miniport `sgcomp.sys` and display driver
`sgcodd.dll`, the Windows 2000 boot video driver `sgibvid.dll` and frame buffer miniport `sglfb.sys`, the NT 4.0 HAL
`halborg.dll` (April 1999 OEM disc 1, 142,496 bytes) and PROM 1.1005 (main module loaded at `0x1100000`). Every register
sequence on this page held, with the corrections made above and the differences in the next table. All five programmers
agree on every field layout; they differ in order, waits and which optional registers they touch, which is the evidence that
the hardware accepts more than one order.

**Must be exact** (a shipped reader branches on it, polls it, or lays memory out by it):

- `id` (`+0x14`) reads `0x666` as a 32-bit load. `sgcomp.sys@0x1196f` compares the whole dword; `sgibvid.dll@0x800106cb` and
  `sglfb.sys@0x1057c` mask to the low 16 bits first; the firmware's self-test table compares the whole dword. A read of `0x666` passes all. Established.
- Access widths: 32-bit loads and stores everywhere, plus byte loads. The freeze bit is tested as a byte at `+0x10003` by
  `sgcomp.sys@0x1bd5d`, `sglfb.sys@0x10ef2`, `halborg.dll@0x80014f17` and the firmware (`1.1005@0x1131678`). The live copies
  (`+0x30008`, `+0x20004`, `+0x40002`) are byte loads only in `sgcomp.sys` (`asStopNormal`, `asStopOverlay`, `asStopDID`),
  `sglfb.sys` (`0x10d2f`, `0x10d69`, `0x10da6`) and the firmware (`1.1005@0x11316d3` to `0x11316e5`, `0x1131ccf`);
  `sgibvid.dll` (`0x80010972`) and `halborg.dll` (`0x80014f90` to `0x80014fa1`) read them as dwords. A model must answer a byte
  read with the matching byte, not fault or read as zero. Established.
- Live copies (`+0x30008` bit 0, `+0x20004` bit 0, `+0x40000` bit 16) must follow their shadow words (`+0x3000c`, `+0x20008`,
  `+0x40004`). After a shadow bit is cleared the live bit must clear within 200 ms for the drivers that stall: `sgcomp.sys`, `sgibvid.dll`
  and `sglfb.sys` poll up to 20,000 times with a 10 microsecond stall each. The NT 4.0 HAL's stop (`halborg.dll@0x80014f10`)
  is one loop of at most 20,000 dword loads of the three live words with no stall (a few milliseconds at most), and it skips
  the whole stop when the freeze bit reads set. The firmware's stop runs only when `ctrlstat` bits 29:28 read 3 and the freeze
  bit is clear (`1.1005@0x1131661`): it writes `0x20000000` to `ctrlstat` first (`0x1131547`), so on a chip that reads back
  `0x20000000` the stop and its inverted wait never run. After the firmware writes `frm_control` with bit 0 set, the live bit must read 1 or the firmware waits forever
  (`1.1005@0x1131ccf`, no time-out). When a follows-at-vblank delay is modelled it can be any value inside those bounds.
  Established as the driver contract; the hardware's own delay is unknown.
- Colour-map FIFO status (`+0x58000`) must read a low six bits above `0x30` (use `0x3f`) whenever software writes the colour
  map. `sgibvid.dll@0x80010c85` times out and prints "Setting color map" for anything else, including `0x40`. Established.
- `vt_xy` (`+0x10000`): bits 23:12 are the current line, read live; bit 31 (`FREEZE`) reads back what software set by writing
  `0x80000000` and clears when software writes 0 (`1.1005@0x1131751`, `0x1131647`, `sgcomp.sys@0x1bd5d`). Established.
- `ctrlstat` (`+0x000`): the low nibble is read by the firmware as the chip revision (a real 320 shows "Display Engine: 002");
  bit 25 must read 1 for the firmware to select the internal dot clock (`1.1005@0x1131d0a` to `0x1131d40`, written `0x30000000` only if bit 25 read
  set and no flat panel answered). Bit 25 reading 1 is inferred.
- The retrace register at `0xca000000`: bit 1 set means a frame has ended; the handler writes the bitwise complement of what
  it read (`sgcomp.sys@0x191bb` to `0x191c4`) and does not read it again. The `vt_flags` read-modify-write in that handler runs
  only in stereo, when `[device extension + 0x11c0] & 1` is set (`0x191d8` to `0x191e4`), and clears bit 6 again only when
  the free-run flag is set. That only clears the bits if a written 0 clears a bit
  and a written 1 leaves it, so the register is write-0-to-clear (inferred). The pin is Cobalt line 24 and is latched.
- Mapped ranges and the revision check: `sgcomp.sys` reads the dword at `0xc2000008` once (`0x10cb9`) and attaches only when its
  high nibble is `0xa` and its low nibble is neither 0 nor 8, so revisions `0xa0` and `0xa8` never get the SGI display driver.
  Established.
- Memory layout read by the engine: tile geometry (512 bytes by 128 lines, 64 KB per tile), the 16-bit tile-list entries
  (`PFN >> 4`, from a 512-byte aligned list whose physical address goes in `frm_control` `[31:9]`), the depth field `[14:13]`,
  and for the linear form (`frm_control` bit 1) a raster whose line pitch is the tile-column count rounded up, times 512 bytes
  (the firmware and `sgibvid.dll` both lay the frame buffer out that way, `1.1005@0x113154e`, `sgibvid.dll@0x80010b54`).
  Layout established, hardware reading inferred.
- Pixel byte order: red in the top byte of a 32-bit pixel word (`sglfb.sys` colour masks; `ArsenicPixelFormats`). Inferred,
  and the `config` little-endian bit (`0x20`) is not settled.
- DDC and flat-panel I2C: only if a monitor or panel is to be emulated. Bit 0 is SDA and bit 1 SCL, a 1 pulls the line low and
  a read returns 1 while the line is low. With no device both lines read 0; the NT driver then falls back to its mode table
  and the firmware to 1024 by 768 at 75 Hz. Established.

**May be approximated** (nothing shipped reads the difference):

- The PLL: `dotclock` fields are write-only in effect. No lock bit is read and every wait is a plain delay (`sgcomp.sys@0x1bc13`
  to `0x1bc8d`, `sgibvid.dll@0x800109e5`). A model can derive the refresh rate from the totals and the clock, or ignore the
  clock and use the mode's nominal refresh.
- Scan-out pacing: only the line number (`vt_xy`), the vertical-blank window and the retrace interrupt are observed. They need
  a monotonic line counter that wraps at the vertical total and an interrupt once per frame; the exact horizontal timing,
  the hsync, vsync and blank windows, `did_start_xy`, `crs_start_xy`, `vc_start_xy` and the pixel-enable windows are not read
  back by any shipped software. `sgibvid.dll` writes `0xffffffff` to the last three (`0x80010b36`).
- Colour map, gamma and the 10-bit gamma map: store and return what was written; their effect is display-only.
- Hardware cursor: `sgcodd.dll` programs it (`vMovePointer@0x282a0`, `bCopyColorPointerIntoHardware@0x293ac`, the set-pointer
  path at `0x28f6b`); a replacement driver can draw its pointer in software. Every other writer only switches it off:
  `sgibvid.dll`, `sglfb.sys` and `halborg.dll` write `+0x70004` = 0 (`0x80010bcc`, `0x10f33`, `0x80015043`), and the HAL's stop
  also clears bit 0 of it with a read-modify-write (`0x80014fb7`). The firmware writes the colours (`+0x70008`, `+0x7000c`),
  position 0 and the glyph at `+0x78000`, then control 0 (`1.1005@0x11318cb`). `sgcodd.dll`'s order varies: parking writes the
  position then control (`0x282ce`), showing writes control 1 then the position (`0x283a2`, `0x283d8`), and the set-pointer path
  writes the position then control 1 (`0x28f6b`). A model that stores words and honours control bit 0 satisfies all of them;
  scanning the 32 by 32 two-bit glyph is from the O2's layout.
- The scanline interrupt registers (`+0x10020`, `+0x10024`): the drivers park them at `0xffffff` (`asStopIntr`,
  `sgibvid.dll@0x80010922`), but the SGI miniport then arms them: `asSetInterrupt` (`0x1c378`) is called from `CoSetMode`
  at `0x1a318` (interrupt 0, line = screen height less a computed allowance, less one) and `0x1a338` (interrupt 1,
  line = height plus one, plus two when mode flag `0x1000`, the flat-panel timing, is set). The firmware's later hand-over routine parks both
  (`1.1005@0x1108040`) and then writes `0x1fff` to `+0x10020` (`0x11083d8`). No shipped handler for those two scanline interrupts was found; the
  retrace handler (`CoVRetraceHandler`) acts on pin 24 only, so a model that raises just the once-per-frame retrace interrupt
  satisfies the readers read here (inferred).
- The self-test masks of the firmware (`prom-init.md`) matter only for a model that is to pass the firmware's register
  walk; a failed display test does not stop the boot.
- Reset values: unknown for every register, and nothing reads a register before writing it except `id`, the low nibble of
  `ctrlstat` and `+0x1c`, so any starting state with `FREEZE` clear and the live bits clear is acceptable.

**The four boot-time writers differ.** What each programmer writes, from the disassembly. A model that gives the same result for
all of them has the right contract; one that insists on the SGI miniport's order fails the others.

| Step | SGI 5.2.4 miniport | Windows 2000 `sgibvid.dll` | Windows 2000 `sglfb.sys` | NT 4.0 HAL `halborg.dll` | PROM 1.1005 |
|---|---|---|---|---|---|
| `ctrlstat` | `0x30000000` last | `0x30000000` first (`0x800109cc`), never again | `0x30000000` last (`0x111a1`) | not written, except `\|= 0x04000000` with a panel (`0x80015141`) | `0x20000000` first; `0x30000000` last only if bit 25 read set and no panel |
| PLL | clear `RUN`, 40 ms, `0x4101`, 40 ms, `RUN`, 1 s, `0x4101`, 100 ms, then value, 40 ms, `RUN`, 200 ms | clear `RUN`, 20 ms, value with `RUN`, 20 ms | same as `sgibvid.dll`, and the same value written again after the tile registers, followed by an 11 ms stall (`0x110f5`, stall `0x2af8` microseconds) | clear bits above 15 (`0x80015166`), value with `RUN` in one write, no waits (`0x800153a3`) | value with `RUN` in one write, then 11 ms |
| `config` | `0xe5`, `0xe7`, plus `0x08`, bit 0 cleared with a panel and set again by mode flag `0x2000` (`0x1bcd5`) | `0xad`, `0xac` with a panel; reads `+0x10004` back and discards it (`0x80010b9d`) | `0xa5`, `0xad` with a panel | `0xa7`, then \|`0x08` | `0xa7` (`0xaf` with a panel) |
| `vt_flags` | 4 for hsync flag, 1 for vsync flag (`0x40` then 0 before) | 5 if the hsync flag is set, else 0 | 0 | 0 (`0x8001516d`, `0x8001528a`) | not written |
| `did`/`crs`/`vc` start | computed | `0xffffffff` | computed | `did` and `vc` computed, `crs_start_xy` not written | computed |
| `frm_size_tile` | whole columns, remainder in 32-byte units | width rounded up to whole tile columns, remainder 0 | whole columns plus remainder, depth field | from a precomputed word | whole columns, remainder |
| `ovr_width_tile` | full width then pulse | 0 | full width, then `0x2000`, then 0 | 0 then pulsed | `0x2000` then 0 |
| `frm_control` | tile list \| 1 | base \| 3 | tile list pointer, low bits masked, \| 3 | base \| 3 | base \| 3 |
| Stop wait sense | waits for the live bit to clear | waits to clear | waits to clear | waits to clear, no stall, skipped when frozen | waits for the live bits to read **1**, so with a real stop it times out after 100 ms; runs only if `ctrlstat` bits 29:28 read 3 and the freeze bit is clear |
| `mode_regs` | not written | 32 words of 3 | word 0: 8 bpp 3, 32 bpp `0x17`, 16 bpp `0xf` (unreachable) | 32 words of 3 | 32 words of 3 (graphics console) |
| Colour map | wait before every 32nd entry | 16 entries, wait before each | wait before every 32nd entry | two words (`0xff00`, `0xffffff00`) at `+0x50000`, no FIFO wait (`0x80015059`) | two words zero, wait before each |

Binary addresses in the table: `sgcomp.sys@0x1bbcc` (`asSetConfigAndTiming`), `sgibvid.dll@0x800109b6`, `sglfb.sys@0x10de6`,
`halborg.dll@0x80014f10` to `0x80015511`, `1.1005@0x1131949`. All are established readings of whole routines. What the hardware
does with each difference is inferred; the one the page used to state wrongly was that all drivers wrote the sequence in the same
order.

Display firmware context: the PROM's legacy BIOS placement is established in
[the chips firmware console-path page](https://github.com/jamesbraid/chips/blob/main/software/visws/prom-console-path.md).
The 1.1005 image carries the strings "Enable VGA Mode" (a boot-menu entry) and "Monitor, Mini DB15 VGA with DDC2B Control" (a
device name table entry), neither of which touches the display back end's registers.

Still unknown, with the route tried: reset values (no shipped software reads one before writing it); what `ctrlstat` bit 25
means and when it reads set; what `RUN` going low does to scan-out (the NT 4.0 HAL clears it with one write and sets it with the
next, with no wait); the exact live-copy delay; the linear form's behaviour for a width that is not a multiple of 512 bytes;
the write semantics of `0xca000000` beyond the inference above. [questions/open-questions.md](../../questions/open-questions.md)
lists each with what would settle it.
