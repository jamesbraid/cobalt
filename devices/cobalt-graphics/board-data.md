# Cobalt graphics: what SGI's white paper and a real machine's registry settle

Two sources that were not in the first round. Back to the [overview](README.md).

- **The white paper**: "The Silicon Graphics 320 and Silicon Graphics 540 Visual Workstation Graphics Subsystem",
  Silicon Graphics, form J10043 (2351 6/99), five pages. Facts it states are **established** for the document.
  It is a marketing white paper: features and peak numbers, no registers, packets or formats.
- **A registry export of `HKEY_LOCAL_MACHINE\HARDWARE`** from a running install on a 320 (the machine is
  not identified; the 5.00 export format and a PnP resource map mean Windows 2000 or later). What it lists is **established** as published output from a real machine: it is what
  that machine's registry held. What it implies about the hardware is **inferred**, because a registry reports what the
  firmware and the drivers told Windows, not what the chips do.

Neither source is copied here; the facts are in my words, with the page or key.

## The white paper

Page numbers are the printed ones.

| Fact | Page | Notes |
|---|---|---|
| The Cobalt chip holds the rendering and geometry pipelines, the memory controller, and the interfaces to the P6 bus, the memory, the I/O subsystem and the display subsystem | 3 | The display back end is a separate part of the system ("display subsystem"); fits the three-ASIC reading in the overview |
| Package: 1521-ball flip-chip, five layers, 0.25 micron, 13.6 mm square die, 10 million transistors | 3 | |
| The engine takes primitives described by window-space vertex coordinates and attributes (normal, colour, texture coordinates, fog, z); it also takes connected line and triangle meshes | 3 | Matches the vertex packets and the strip and fan types ([draw-and-buffers.md](draw-and-buffers.md)) |
| Vertex-attribute interface for point, line, line strip, triangle, triangle strip and rectangle primitives | 3 | The ICD's mode word has types for lines, line strips and loops, triangles, strips and fans; no rectangle type was seen. A rectangle is not a separate primitive in the driver |
| Engine does vertex transform object to window, view-frustum clip check, normal transform and normalisation, rasteriser and interpolation set-up, anti-aliased line set-up, front and back face culling, per-vertex lighting for up to four lights plus a host-supplied contribution | 3 | The ICD sends matrices, light and material blocks and a cull field ([packet-set.md](packet-set.md)), and also keeps host paths for transform, clipping and lighting. Which the driver uses when is not settled |
| Rasterisation: Gouraud shading; depth buffering with 16-bit floating point and 24-bit fixed point formats; 8-bit stencil; nearest, bilinear and trilinear mip-mapped textures; anti-aliased points and lines; fog; scissor; line and polygon stipple; alpha and chroma test; alpha blend; logical operations; colour plane mask; dithering for 5-bit RGB components | 3 | Every one has a packet in the ICD except chroma test, which was not found |
| Window clipping through screen masks and window clip IDs; linear framebuffer addressing; occlusion testing; post-texture specular highlights | 3 | The ICD has occlusion handling (`__glCoProcessOcclusion`); window parameters are built by the miniport |
| Colour formats 8, 16 and 32 bits, and 16/16 and 32/32 double-buffer formats; pixel transfers through the pipeline with format conversion; subsampled YUV; a 4 by 4 colour matrix for colour-space conversion, scale and bias and component swizzle; clears at full memory bandwidth; framebuffers and textures up to 4k by 4k; memory-management hardware that allocates graphics buffers in system memory | 3 | Agreement: three colour sizes in the draw-buffer descriptor, the colour-matrix packet, the 4096 scissor limit, the rectangle clear |
| Memory: one pool, up to 1 GB on the 320 and 2 GB on the 540, with up to about 900 MB and 1.9 GB for textures; 32-bit double-buffered colour, 24-bit z, 8-bit overlay, 8-bit stencil | 3 | The overlay is the 8-bit layer plane the ICD describes through `DESCRIBELAYERPLANE` |
| Memory is allocated in tiled pages, and to avoid fragmentation and reduce DRAM page crossings "the pages are organized linearly (without tiles)" | 3 | Read literally the sentence contradicts itself. Unresolved; the miniport's tile lists and `0x8000` table are the software side ([graphics-engine.md](graphics-engine.md)) |
| Largest mode 1920 by 1200 at 66 Hz in 32-bit RGBA | 3 | Already in the overview |
| Main memory: 288-bit bus (32 bits of it ECC), up to 3.2 GB/s, SDRAM; graphics to memory 3.2 GB/s, CPU to memory 800 MB/s peak | 2, 3 | 288 minus 32 is 256 data bits, one 32-byte memory row; the clear packet works in 32-byte rows ([draw-and-buffers.md](draw-and-buffers.md#clears)) |
| Sustained rates (32-bit colour, 24-bit z, 500 MHz Pentium III): 180 M smooth-shaded z-buffered pixels/s; 150 M bilinear and 120 M trilinear textured; 7 M 10-pixel vectors/s (3.5 M anti-aliased); 3.5 M 25-pixel smooth triangles/s; 1.5 M textured; clear 2.7 GB/s | 2 | Performance claims; the clear figure is 84 percent of the stated peak bandwidth |
| Software: OpenGL 1.2, Direct3D, DirectDraw; library tuning for Windows NT applications | 4 | |

**What it does not settle:** every register, packet format and address; which functions run on the host; the unit that
decodes `0xc8000000`; the engine's use of any header. Nothing in it contradicts a page already here.

## The registry export

Keys are given relative to `HKEY_LOCAL_MACHINE\HARDWARE`. The text is Windows registry 5.00 export; binary values were
decoded by hand and checked against the resource-list layout (a count, then per list the interface type, bus number,
version, revision and descriptor count, then 16-byte descriptors).

**Identity of the machine (established as listed).**

- `DESCRIPTION\System`: identifier `SGI-320_ARCx86_mp`; firmware ("SystemBiosVersion") `PROM Version 1.1005.SGI`,
  date `07/06/00`; "VideoBiosVersion" strings `Main BIOS*v1.0` and `005.SGI`, date `01/01/99`. One processor, x86 Family 6
  Model 5 Stepping 2, about 451 MHz (`~Mhz` = `0x1c3`).
- `DESCRIPTION\System\MultifunctionAdapter\4` ("SGI REV INFO") carries the revision text the firmware passes to
  Windows: `Motherboard: 320 fab 006 rev F (100MHz CPU Bus)`, `Cobalt:a4`, `Cobalt I/O: 005`, `PCI Bridge: 01`,
  `Serial I/O: 366c3dfb`, `1394: 00000001`, `Display Engine: 002`. Its children repeat the numbers as dwords:
  `SYS TYPE` 0, `BOARD REV` `0x11`, `COBALT` `0xa4`, `COBALT I/O` 5, `DISPLAY ENGINE` 2. Established as what this machine reported.

**Reconciliation with the existing pages.**

| Existing statement | Registry | Result |
|---|---|---|
| The Cobalt CPU block's revision register (`0xc2000008`) has high nibble `0xa`; NT names the part "Cobalt Graphics ChipSet A0" to "AF" ([README](README.md#physical-map-graphics-related-windows-only)) | `Cobalt:a4` | **A real machine carries Cobalt revision A4.** The same value is what a public emulator's model of the machine returns for that register (it also accepts A5). |
| The display back end (Arsenic) exposes an ID register that reads `0x666` and NT refuses to attach otherwise | `Display Engine: 002` | The firmware names the display back end a "Display Engine", revision 2. A relation between that revision and anything the driver reads is not known. |
| The NT miniport claims `0xd0000000` to `0xd008001f` and maps `0x80020` bytes (inferred) | the resource list of the device the miniport owns: one memory range, start `0xd0000000`, length `0x80020`, interface type "processor internal", raw and translated alike (`RESOURCEMAP\PnP Manager\PnpManager`, the `\Device\Resource…` value) | **The claim is established for this install**, so the extent `0x80020` is now more than an inference for the driver that this install ran (the export is not from NT 4.0; the NT 4.0 miniport's own claim was read in the binary). The engine window at `0xc8000000` is not in the list: the miniport maps it without claiming it, as the first round read. |
| The graphics is in the chip set, not on a PCI card | `DESCRIPTION\System\MultifunctionAdapter\1` is `Internal` with `DisplayController` "Cobalt Graphics" and a monitor peripheral `1024x 768`; the PCI adapters (`2`, `3`) list only the network controller; no PCI display device appears in the resource map | Established as listed; inferred as a statement about the wiring. The only graphics resource is the internal-bus one above. |
| One video device | `DEVICEMAP\VIDEO`: `\Device\Video0` is service `Sgcomp` (the SGI miniport) and `\Device\Video1` is service `mnmdd`; the second claims only the legacy VGA ranges (ports `0x3b0` to `0x3bb`, `0x3c0` to `0x3df`, `0x1ce`, `0x1cf`, `0x2e8` to `0x2ef`, memory `0xa0000` to `0xbffff`) | A second video device exists on this install and is not SGI's. It does not tell anything about the Cobalt hardware. |

**Physical memory as Windows saw it (established as listed).** `RESOURCEMAP\System Resources\Physical Memory` lists twelve ranges
of RAM, 455.1 MiB in all; `Reserved` lists nine more ranges of 4, 12, 12, 8, 4, 4, 4, 4 and 4 MiB, 56 MiB in all.
Together they make 511 MiB, so the install had 512 MiB less the legacy hole below 1 MiB and one 512 KiB gap above
`0x1100000`. RAM runs from 0 to `0x10000000` and again from `0x20000000` to `0x30000000`, with nothing in between.
The reserved ranges sit at the start of 32 MiB-aligned blocks: `0x2000000` (4), `0x4000000` (12), `0x6000000` (12),
`0x8000000` (8), `0xa000000`, `0xc000000`, `0xe000000` (4 each), `0x20000000` and `0x22000000` (4 each). Whether
they are the graphics memory the firmware sets aside is **inferred** only from the sizes being whole multiples of
4 MiB and nothing else in the export being in that place; the open question about the memory split
([open questions](../../questions/open-questions.md)) stays open. The memory banks are the platform's business
([chips](https://github.com/jamesbraid/chips/blob/main/devices/cobalt/README.md)).

**Other resources, for the platform pages.** The HAL's raw and translated lists show interrupts 0 to `0x2f`, `0x3d`,
`0x41`, `0xc1`, `0xe1`, `0xf1`, `0xf2`, `0xfd`, `0xfe` and 11 port ranges (`0x0`, `0x20`, `0x40`, `0x48`, `0x70`,
`0x80`, `0x92`, `0xa0`, `0xc0`, `0xf0`, `0xcf8`); the first PCI bus has ports `0x10` to `0x7fff` and a 64 MiB
memory window at `0x80000000` (translated `0xf4000000`), the second a port window `0x8000` to `0xffff` and a 64 MiB
memory window at the same raw address (translated `0xf0000000`); the network controller (Intel 82557) is on the
second bus with a 4 KiB register range, a 1 MiB range and 32 ports. None of it concerns the graphics.

**What the registry does not settle:** any register, the engine window, the packet set, the memory split, the
meaning of the `Display Engine` revision, which chip decodes `0xd0000000`.
