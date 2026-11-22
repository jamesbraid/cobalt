# Cobalt graphics: the Visual Workstation 320 and 540 graphics subsystem

The Silicon Graphics 320 and 540 Visual Workstations (1999, Pentium II and Pentium II Xeon, Windows NT 4.0) have no
graphics card. Their display, 3D rasteriser and video ports are part of SGI's "Cobalt" core-logic chip set and use the
same 100 MHz main memory as the processors (a unified memory design, like the O2's). This page is the overview and the
emulator brief. The registers and the software that drives them are in three companion pages:

- [display-engine.md](display-engine.md): the display back end (timing, frame buffer scan-out, palette, gamma, cursor,
  I2C) and the mode table.
- [graphics-engine.md](graphics-engine.md): the rendering engine's window, the packet queue in main memory, interrupts.
- [nt-software.md](nt-software.md): what the Windows NT miniport, display driver and OpenGL driver do, and the Linux
  frame buffer driver.
- [packet-set.md](packet-set.md): the header words the OpenGL driver puts in the packet stream, one row each, with the
  address-field reading and the engine-register class.
- [draw-and-buffers.md](draw-and-buffers.md): how those packets draw, clear, set buffers and bind textures.
- [board-data.md](board-data.md): what SGI's graphics white paper and a real machine's registry export settle.
- The OpenGL driver itself, its symbols and its run-time code generators: [nt-opengl-icd.md](../../software/windows/nt-opengl-icd.md).
- [framebuffer-driver-floor.md](framebuffer-driver-floor.md): the smallest set of registers and of HAL and driver-model facts a
  frame-buffer-only driver needs, how well each is known, and the drivers' hot paths.

The system side is covered by pages in the [chips repository](https://github.com/jamesbraid/chips): [the platform](https://github.com/jamesbraid/chips/blob/main/platforms/visws/README.md), [the Cobalt system ASIC](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md)
(processor interface, interrupt controller, timer) and [Lithium](https://github.com/jamesbraid/chips/blob/main/devices/lithium/README.md) (PCI bridges, audio). This one stops
at the graphics blocks and says where it leans on them.

## Grades

Every fact carries one of the repository's three ratings ([AGENTS.md](../../AGENTS.md)). Nothing is unlabelled.

- **established**: read in full from a shipped binary (the whole function, table or code path, cited as file and
  address), or stated in a published SGI document or a public driver.
- **inferred**: a reasoned reading, with the reasoning given. What a driver does is established; what the hardware does
  in response is inferred, because the driver is the only witness. A reading from call lists, a name, trade press, a
  sibling chip or a partial read is inferred. So is anything another project's model says.
- **unknown**: nothing published or shipped decides it. The route that would settle it is given, and it is listed in
  [the open questions](../../questions/open-questions.md).

These pages were first written with five grades. The two extra ones were *software expectation* (what one shipped
driver does) and *observed* (what trade press, a public post or a registry export reports). Round 2 regraded them to the
three above: both became **inferred** unless the entry says otherwise, and a statement of what a named routine does that
cites a routine read in full is **established**. The regrade is by mapping, not a fresh reading of every row; a row that
was **established** before is as it was.

## Sources

| Source | Kind | Used for |
|---|---|---|
| SGI 320 Visual Workstation datasheet, Silicon Graphics 1999 | published specification | feature list, supported modes, driver list |
| SGI 320 Visual Workstation Product Guide, Silicon Graphics 1999 | published brochure | architecture overview, chip names |
| Microprocessor Report, 15 February 1999, "SGI Chip Set Powers NT Workstations" | trade press | bandwidth split, fill and triangle rates, UMA rationale |
| Design News, "SGI reinvents NT workstations" (1999) | trade press | three-ASIC structure, static memory split |
| SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 (patch SG0020054, 12 December 2001): `sgcomp.sys`, `sgcodd.dll`, `sgcoicd.dll`, `sgcotab.cpl`, with the matching `.dbg` symbol files | shipped binaries | everything marked inferred |
| Same driver for Windows 2000 (also v5.2.4; different binaries) | shipped binaries | the display driver's OpenGL escape table, read and identical; the OpenGL driver, display driver and miniport header scans, identical sets ([packet-set.md](packet-set.md#coverage)) |
| Windows NT 4.0 OEM disc set, disc 1, and Silicon Graphics Recovery CD-1 v1.1: April 1999 `sgcodd.dll` and `sgcomp.sys` | shipped binaries | header scan only; same set as 5.2.4 |
| Windows 2000 RTM `sgibvid.dll`, `sglfb.sys`, `sglfb.dll` | shipped binaries (the Windows 2000 CD) | the display back end as the OS's own drivers program it; the "Arsenic" naming |
| Linux `drivers/video/sgivwfb.c`, `include/video/sgivw.h`, `arch/x86/kernel/visws_quirks.c`, `arch/x86/include/asm/visws/cobalt.h` (Linux history tree, 2.6.x era) | public driver | display back end registers, memory reservation |
| Usenet posts in comp.sys.sgi.admin (2003 to 2004) | public posts | history, absence of Linux 3D support |
| "The Silicon Graphics 320 and Silicon Graphics 540 Visual Workstation Graphics Subsystem", white paper J10043, 1999 | published document | features and peak rates of the engine, memory pool, formats ([board-data.md](board-data.md)) |
| `HKEY_LOCAL_MACHINE\HARDWARE` export of a running 320 | published output from a real machine | firmware and chip revisions, resource claims, memory ranges ([board-data.md](board-data.md)) |

The NT driver package is the file `SGICoGraphics524nt4.exe` (SHA-256 `5097ff4e…`; the Windows 2000 one is
`SGICoGraphics524w2k.exe`, `c09ea6c6…`; full hashes in [INDEX.md](../../references/documents/INDEX.md#software)). Each is a
self-extracting InstallShield archive; its `data1.cab` holds the files above (`7z x`, then `unshield x data1.hdr`). SGI
Support Folio, the original host, is gone. The driver symbols ship in the package, so function and many variable names in
`sgcomp.sys` are the vendor's own and are cited here by name.

## What the subsystem is

- **A graphics, memory and I/O chip set of three ASICs.** The Linux kernel's own Visual Workstation notes name them Cobalt,
  Lithium and Arsenic, with the 3D engine inside Cobalt (see [the Cobalt page](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md)). Design News counts
  the same three as a graphics engine, a display engine and an I/O engine, and the Product Guide names two custom parts, a
  "graphics and memory controller" and an "I/O coprocessor". Inferred (trade press). Mapping them: the rendering engine is in Cobalt
  (established by the kernel notes); the display back end at `0xd0000000` is named Arsenic by SGI's own software (the Windows 2000 boot video driver prints "Failed to find SGI Arsenic chip ID" when the
  `0x666` ID does not read; established as the vendor's name for the chip), and the NT driver agrees by prefixing every display-back-end routine `as` (`asStartFrm`, `asSetConfigAndTiming`, `asSetInterrupt`) and
  names its pixel-format table `ArsenicPixelFormats`; Lithium is the I/O chip. Nothing states where the register
  blocks physically are, so the mapping of blocks to chips is inferred.
- **Unified memory.** The memory array is 100 MHz, 256 bits wide and not interleaved, 3.2 GB/s in total. Per the trade
  press, up to 800 MB/s of that goes to the processors' front-side bus, up to 1.6 GB/s to the I/O side, and the rest,
  plus whatever the others leave unused, to graphics. Inferred (trade press). The datasheet states 256-bit, 3.2 GB/s, ECC SDRAM and up
  to about 80 percent of system memory for textures. Established (published datasheet).
- **Memory split is static.** Design News says the video share is fixed by a profile, not allocated on demand. The
  Linux driver agrees: it reserves the top 8 MB of RAM as the frame buffer and never tells the kernel about it
  (`visws_memory_setup`), and the NT miniport takes its bank list from the loader (see
  [nt-software.md](nt-software.md)). Inferred (driver behaviour).
- **Pipeline stages the datasheet lists for the rendering engine.** Transform and lighting of vertices (up to four
  lights), culling, clipping to screen masks and clip IDs, setup of rasteriser and attribute interpolation, Gouraud
  shading, anti-aliased lines, point, line, triangle and rectangle rasterisation, stippling, scissor, texture mapping
  (nearest, bilinear, trilinear mip-mapped, up to 4096 by 4096), fog, alpha and chroma key, blending, dithering,
  logical operations, plane mask, stencil (8-bit), depth (16-bit float or 24-bit fixed), full-scene anti-aliasing at 4x,
  pixel transfer with format conversion and a 4x4 colour matrix (YCrCb to RGB and back), p-buffers, an overlay buffer,
  occlusion instruments, and frame-lock video sync. Established as a feature list (published datasheet); the order of
  the stages is **unknown**; what the OpenGL driver does on the host and what it leaves to the engine is in
  [draw-and-buffers.md](draw-and-buffers.md#who-does-what). Trade press quotes 7.4 Mtriangles/s and 176 Mpixels/s
  with trilinear filtering as peaks. Inferred (trade press).
- **Frame buffer formats.** The datasheet lists 8-, 16- and 32-bit colour, 16/16 and 32/32 double buffering, and
  1920 by 1200 at 66 Hz in 32-bit RGBA as the largest mode. Established. The display back end exposes three pixel depths
  (8, 16, 32 bits) as a two-bit field; see [display-engine.md](display-engine.md).

## Physical map (graphics-related windows only)

| Physical range | What | Source | Grade |
|---|---|---|---|
| `0xc2000000` | Cobalt CPU block. Register `+0x08` is the revision (high nibble `0xa`). NT refuses the part if the low nibble is 0 or 8 and names it "Cobalt Graphics ChipSet A0" to "AF" from the low nibble. Also holds the timer (Linux). Described in [the Cobalt page](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md#cpu-block-revision-and-system-timer-established) | `sgcomp.sys` `CobaltFindAdapter`, Linux `cobalt.h` | inferred (revision), established (timer block exists) |
| `0xc4000000` | Cobalt interrupt controller ("APIC" in Linux). Graphics pins below; registers and line assignments in [the Cobalt page](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md#interrupt-controller-the-cobalt-apic) and [the interrupt routing page](https://github.com/jamesbraid/chips/blob/main/platforms/visws/interrupts.md) | Linux `cobalt.h` | established |
| `0xc8000000`, `0x19000` bytes | Rendering engine register window | `sgcomp.sys` `CobaltInitialize` maps exactly this length | inferred |
| `0xca000000`, 4 bytes | Vertical-retrace interrupt register, read and then rewritten with its complement in the retrace handler. Mapped only if the display back end's ID register reads `0x666` | `sgcomp.sys` `CoVRetraceHandler` | inferred |
| `0xc0000020`, 4 bytes | Mapped on the same condition. The mapped kernel address (`FlushKvAddr`) is returned to the display driver in word 15 of the global-information reply of `CobaltStartIO`, and the display driver stores it as `ulFlushAddr` in its shared device record; no routine read by name uses it afterwards. Purpose unknown | `sgcomp.sys`, `sgcodd.dll` | unknown |
| `0xd0000000`, 16 MB | Display back end (DBE). Linux reserves `0x01000000` bytes; the NT miniport claims `0xd0000000` to `0xd008001f` and maps `0x80020` bytes. Mapping extent by software, each read from the disassembly: NT 4.0 HAL `0xa0` pages (`halborg.dll@0x8002d39c`, largest offset used `+0x70004`); NT miniport `sgcomp.sys` `0x80020` bytes (`@0x1175b`, largest `+0x68ffc`: the 0x1000-byte gamma-table copy at `+0x68000`); NT display driver `sgcodd.dll` maps nothing, it uses the miniport's kernel mapping returned in the global-information reply (`sgcomp.sys@0x13215`), largest `+0x780fc` (cursor glyphs); Windows 2000 boot video `sgibvid.dll` `0x80020` (`@0x800106a6`, largest `+0x70004`); `sglfb.sys` `0x80020` for itself (`@0x10561`) and `0x80100` only for the public-access-ranges reply to its display driver (`@0x109f1`, `@0x109f8`; largest `+0x70004`); the SGI 5.2.4 Windows 2000 `sgcomp.sys` `0x80020` (`@0x111aa`); PROM 1.1005 main module `0x81` pages (`1.1005@0x1103892`, largest `+0x780fc`); Linux `DBE_REG_SIZE` `0x01000000` (`include/video/sgivw.h`, `drivers/video/sgivwfb.c`) | `sgivwfb.h`, `sgcomp.sys`, a real machine's resource list ([board-data.md](board-data.md)) | established (base; the claim, as listed by a Windows 2000 or later install) |
| Top of RAM, 8 MB default | Linux frame buffer. NT takes its own bank list from the loader | `visws_quirks.c`, `sgcomp.sys` | inferred |

Interrupt pins on the Cobalt controller (numbering from Linux `cobalt.h`): 16 and 17 video out, 18 and 19 video in, 22
audio, 24 "AS", 25 "RE", 28 CPU timer and cache. The NT miniport asks the HAL for the vectors of pins 24 and 25 and
connects the engine's handler to pin 25 and the retrace handler to pin 24. So pin 25 carries the rendering engine's
interrupt and pin 24 the display's vertical retrace. Inferred (driver behaviour). That "RE" stands for rendering engine and
"AS" for Arsenic is inferred, not published: it agrees with the `as` prefix, the Arsenic pixel-format table above, the boot video driver's
own naming of the chip at `0xd0000000` and the kernel notes naming Arsenic as the video chip.

## What an emulator needs for a console and for the NT or Linux display

Enough to show text and a desktop:

1. **The DBE register window** at `0xd0000000`, 32-bit uncached accesses only. Reads must return the ID `0x666` at
   `+0x14` (NT refuses to attach otherwise). Inferred (driver behaviour).
2. **A timing generator state machine** good enough that the status bits NT and Linux poll change: the live DMA-enable
   words (`+0x20004`, `+0x30008`, `+0x40000`) must follow the shadow words (`+0x20008`, `+0x3000c`, `+0x40004`)
   within a frame, and the colour map FIFO word (`+0x58000`) must report free entries. Both drivers spin on these
   with a time limit and carry on regardless, so a fixed "ready" answer is enough to boot. Inferred (driver behaviour).
3. **Scan-out from main memory.** Either a linear frame buffer (Linux: tile pointer is the physical address shifted
   right by 9, linear bit set, 8 MB at the top of RAM) or a tile list (NT: 64 KB tiles named by a list in memory).
   Both are needed. The pixel formats are 8-bit indexed through the colour map, 16-bit and 32-bit RGB. Both drivers
   leave the DID (per-pixel window ID) fetch disabled by writing zero; the enable bit is bit 16 in NT and in the
   O2's GBE, and the Linux constant names bit 0, which looks like a slip. See [display-engine.md](display-engine.md).
4. **A dot clock that reports stable.** NT and Linux program a PLL and wait fixed times (40 ms, 200 ms, and a one-second
   dance); they never poll a lock bit, so none is needed.
5. **Interrupt pins 24 and 25** can stay idle for a Linux text console, which uses neither. The NT miniport connects
   both handlers at attach, so the pins must exist for NT. Whether NT waits for an engine interrupt before the
   first draw is not established.

Enough to run the NT display driver's 2D and the OpenGL driver:

6. **The rendering engine window**, the packet chain in main memory and its ownership bit (bit 0 of a header word,
   set by software and cleared by the engine), and the context-switch and doorbell registers. See
   [graphics-engine.md](graphics-engine.md). The header words, their payload layouts and the way the OpenGL driver
   combines them are now catalogued ([packet-set.md](packet-set.md), [draw-and-buffers.md](draw-and-buffers.md)). What
   the engine does with them, to the level needed to rasterise (the arithmetic of set-up, interpolation, texturing,
   blending and the memory layout of the buffers) is **unknown**: no source describes it and no public code drives the
   engine other than SGI's own driver.

Not needed to show a console: video capture and video out, the cursor (the miniport does not program it; the NT display driver does,
[display-engine.md](display-engine.md#colour-map-gamma-cursor-video-capture)), I2C, flat-panel logic, DDC, the audio, 1394 and USB blocks.

## Open

The unknowns are in [the open questions](../../questions/open-questions.md): the packet format and the rendering
engine's unread register range, the cursor and video-capture programming, what the chip set's second interrupt
register at `0xca000000` carries, the reset values of every register, and which ASIC hosts the display
back end (Arsenic is inferred).
