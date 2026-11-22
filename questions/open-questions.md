# Cobalt graphics (Visual Workstation 320 and 540): what the sources do not settle

Question. What are the packet formats and register meanings of the Cobalt rendering engine, and which blocks sit on
which of the chip set's ASICs?

Status: **unknown** for everything below. What is established and what software expects is in
[the Cobalt graphics pages](../devices/cobalt-graphics/README.md).

- **Packet semantics.** The headers are catalogued ([packet-set.md](../devices/cobalt-graphics/packet-set.md)): 52 constant
  values in the OpenGL driver, each with its emitting routine and payload layout, and the computed forms. The vertex
  and mode packets, the buffer descriptors, the clear, scissor and texture packets are described in
  [draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md). Open: what the engine does with each
  (the arithmetic of set-up, interpolation, texturing, blending, depth and stencil formats and the tile layout), the `B`
  field of the header, what the instance bits mean for each block, the difference between the two vertex ports
  `0xc5` and `0xc6`, the bits of the pixel-pipe mode word other than 13 and 22 to 25, the window parameter packets,
  the translation-table entry format and the two link-word builders' one-off length difference. Routes tried: the OpenGL
  driver (every constant header and its routine), the display driver and miniport (header scan and the packet builders
  read), SGI's graphics white paper (features only), a registry export (identity only), a public emulator's model of
  the machine (decodes no packet). Next: the miniport's texture-table packet and window packets in full, the display
  driver's 2D routines, and a straight disassembly read of the vertex-port routine to settle the payload order.
- **Host setup versus engine setup.** Answered in part: the OpenGL driver has both. It transforms, clips and (optionally)
  lights on the host with run-time generated code, and it also sends the matrices, light and material blocks, a cull
  field and, for replayed display lists, vertex packets in a coordinate mode the engine transforms
  ([draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)). Open: which path each
  GL state selects. Route: the validation routines (`__glCoValidateLighting`, `__glCoValidateSoftLighting`,
  `__glCoPickVertexProcs`, `__glCoPickOGVertexProcs`).
- **Engine register window.** Meaning of `0x08000` entries, `0x0c000` entries, `0x10000` write fields, `0x10008`, the two
  interrupt causes and `0x185c0` to `0x18800`; contents of `0x00000` to `0x07fff` and `0x0c200` to `0x17fff`. Route:
  the rest of `sgcomp.sys` (`pCoContextSwitch`, `CoBuildTlbPacket`, `CoProcessCobaltEvent`) read in full, and the same
  driver for Windows 2000 as a cross-check.
- **Second interrupt register.** The 32-bit register at `0xca000000` that the retrace handler reads and complements, and
  the register at `0xc0000020`. A public emulator's model keeps the low four bits of a write and returns them, and never
  raises the interrupt; it is no evidence for the write-back semantics. Route: the Linux Cobalt interrupt code (`visws_quirks.c` and the Lithium and PIIX4
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
