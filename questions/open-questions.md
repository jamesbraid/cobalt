# Cobalt graphics (Visual Workstation 320 and 540): what the sources do not settle

Question. What are the packet formats and register meanings of the Cobalt rendering engine, and which blocks sit on
which of the chip set's ASICs?

Status: **unknown** for everything below. What is established and what software expects is in
[the Cobalt graphics pages](../devices/cobalt-graphics/README.md).

- **Packet semantics.** The headers are catalogued ([packet-set.md](../devices/cobalt-graphics/packet-set.md)): 52
  constant values in the OpenGL driver plus one in the display driver, each with its emitting routine and payload layout,
  and the computed forms. The `B` field is the register's index in the engine's own file, the vertex ports, window
  parameters, table entries and mode-word bits are decoded as the drivers write them
  ([draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md)). Open: what the engine does with each (the
  arithmetic of set-up, interpolation, texturing, blending, depth and stencil formats and the tile layout), what
  `0x98804000` does, mode-word bits 28, 19 and 12:0, the instance flag on the diffuse blocks, the third word of the
  long chain packet, the miniport's window record fields at `+0x194` and the two record fields in the base-pointer entry,
  and the register at `B` = `0xa3` (`A` = `0x490`), which no binary writes. Routes tried: the OpenGL driver (every
  constant header and its routine), the display driver and miniport (the packet builders read in disassembly, 2D routines
  and their mode words), the Windows 2000 packs, the April 1999 display driver and miniport, SGI's graphics white paper
  (features only), a registry export (identity only).
  Next: the April 1999 OpenGL driver (inside an InstallShield 3 archive on disc 2 of the NT 4.0 OEM set, which neither 7z
  nor unshield opens), the Windows 2000 program-database files for symbol names in the Windows 2000 ICD, and the display
  driver's DirectDraw and video routines (`DdBlt`, `CoYUVBlt_S2S`).
- **Host setup versus engine setup.** Answered for the driver's side
  ([draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)): transform is the engine's only
  when the clip-volume hint is `GL_FASTEST`, the user enables it or a variable forces it, and the primitive is not drawn by
  a host fill routine; lighting is the engine's for up to four lights unless a light, colour material or the soft-lighting
  switch puts it on the host. Open: whether the engine clips when it transforms, the texture-coordinate rule
  (`GLCOSIMFORCECHIPTEXXFORM` is read and never used, `__glCoPickCalcTextureProcs` unread), and `__glCoPickPhase1Procs`.
- **Engine register window.** The `0x08000` entries (a 4 KB page frame number) and `0x0c000` entries (the buffer's
  first translation slot in 31:21, a size class in 12:10) are decoded as the miniport writes them
  ([packet-set.md](../devices/cobalt-graphics/packet-set.md#texture-table-packets-class-11)). Open: the meaning of the
  `0x10000` write fields, `0x10008`, the two interrupt causes and `0x18800`; contents of `0x00000` to `0x07fff` and
  `0x0c200` to `0x17fff`. Route: `pCoContextSwitch` and `CoProcessCobaltEvent` read in full, and the Windows 2000
  miniport as a cross-check (its names need the program-database file).
- **Second interrupt register.** The 32-bit register at `0xca000000` that the retrace handler reads and complements, and
  the register at `0xc0000020`. Route: the Linux Cobalt interrupt code (`visws_quirks.c` and the Lithium and PIIX4
  paths) and the HAL image for the PROM's use.
- **Which ASIC.** The kernel's notes name Cobalt, Lithium and Arsenic; the rendering engine is in Cobalt and the display
  back end is inferred to be Arsenic (NT's `as` routine prefix and `ArsenicPixelFormats`). Unconfirmed: which chip
  decodes `0xd0000000`, `0xca000000` and `0xc0000020`, and what the link between Cobalt and Arsenic carries. Route:
  a board photograph with chip markings, the PROM's device tables, or a boot log naming the resources.
- **Reset values.** For every register. Neither driver reads one before writing. Route: the PROM's initialisation of
  the display (the PROM image and the SGI "Software Support Kit" if held), or a hardware read, which needs the
  repository owner's approval and a read-only plan.
- **Cursor, gamma and video capture.** The `0x70000` cursor block, the `0x60000` and `0x68000` gamma maps and
  the `0x80000` capture words are named by Linux and untouched by Linux and by the NT miniport routines read. Where NT
  puts gamma and how the 2D cursor is drawn is unresolved. Route: the display driver `sgcodd.dll` (it may draw the
  cursor itself and send gamma as engine packets) and the Windows 2000 driver.
- **PLL and clock.** The `sysclk` register (`+0x0c`), the purpose of the one-second PLL restart in NT's mode set, and
  `ctrlstat` bit 25.
- **Pixel order.** Whether 32-bit pixels are red-first in memory (Linux) or red in the top byte of the word (NT's
  `ArsenicPixelFormats`), and what the config bit `LENDIAN` does. Route: a Linux boot of the real hardware, or the
  pixel-format table's users in `sgcodd.dll`.
- **Memory banks.** How the firmware chooses and passes the graphics memory split ("profile") and the bank list NT reads
  from `SGICtrl\Memory`. A real machine's registry shows 56 MiB of "reserved" ranges at the start of 32 MiB blocks in
  two 256 MiB banks ([board-data.md](../devices/cobalt-graphics/board-data.md)); whether that is the graphics memory is not shown. Route: the PROM and the NT loader on a system disk.
