# Cobalt graphics (Visual Workstation 320 and 540): what the sources do not settle

Question. What are the packet formats and register meanings of the Cobalt rendering engine, and which blocks sit on
which of the chip set's ASICs?

Status: **unknown** for everything below. What is established and what software expects is in
[the Cobalt graphics pages](../devices/cobalt-graphics/README.md).

- **Packet semantics.** The headers are catalogued ([packet-set.md](../devices/cobalt-graphics/packet-set.md)): 52
  constant values in the OpenGL driver plus one in the display driver, each with its emitting routine and payload layout,
  and the computed forms; the payload words carry the vendor's own field names from the drivers' program databases
  ([the register definitions](../devices/rendering-engine/registers.md)). Open: what the engine does with each (the
  arithmetic of set-up, interpolation, texturing, blending, depth and stencil formats and the tile layout), what
  `0x98804000` (the flush marker) does, what the `0x06c6d081` instance flag on the diffuse blocks does, whether
  `0x91ca1020` is the colour of the cleared pixels (inferred from the opaque-stipple bit) and which of the two cause bits
  the link word's interrupt and sync flags raise. Routes tried: the OpenGL driver in all three builds (every constant
  header and its routine), the display driver and miniport, the program databases of the OpenGL driver and the miniport, SGI's graphics
  white paper (features only), a registry export (identity only). Next: nothing in the shipped software decides these; a
  model chooses and documents a value.
- **Host setup versus engine setup.** Answered for the driver's side
  ([draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)): transform is the engine's only
  when the clip-volume hint is `GL_FASTEST`, the user enables it or a variable forces it, and the primitive is not drawn by
  a host fill routine; lighting is the engine's for up to four lights unless a light, colour material or the soft-lighting
  switch puts it on the host; texture coordinates are always the host's. Open: whether the engine clips when it transforms
  (the transform word has a clip-check bit that no routine sets).
- **Engine register window.** The `0x08000` entries (the first 4 KB page frame number of a tile, the tile size coming from the buffer's descriptor) and `0x0c000` entries (buffer descriptors),
  the `0x10000` start word (idle count, bit 11, node position), `0x10008`, the two interrupt causes, `0x185d0` and
  `0x18800` are decoded as the drivers use them ([graphics-engine.md](../devices/cobalt-graphics/graphics-engine.md)).
  Open: what the engine writes at the dump offset it is given through `0x18800`; what separates the two causes in
  hardware; the idle count's effect; contents of `0x00000` to `0x07fff` and `0x0c200` to `0x17fff`.
- **Second interrupt register.** The 32-bit register at `0xca000000` that the retrace handler reads and complements (the
  miniport names the pointer `CAI_InterruptReg`), and the register at `0xc0000020`. Route: the Linux Cobalt interrupt code (`visws_quirks.c` and the Lithium and PIIX4
  paths) and the HAL image for the PROM's use.
- **Which ASIC.** The kernel's notes name Cobalt, Lithium and Arsenic; the rendering engine is in Cobalt and the display
  back end is inferred to be Arsenic (NT's `as` routine prefix and `ArsenicPixelFormats`). Unconfirmed: which chip
  decodes `0xd0000000`, `0xca000000` and `0xc0000020`, and what the link between Cobalt and Arsenic carries. Route:
  a board photograph with chip markings, the PROM's device tables, or a boot log naming the resources.
- **Reset values.** For every register. Neither driver reads one before writing, and the PROM reads only
  the revision nibble of `+0x000`, the ID and `bist` before it writes ([prom-init.md](../devices/cobalt-graphics/prom-init.md)). Route: a hardware read, which needs the repository owner's approval and a read-only plan.
- **Cursor, gamma and video capture.** The `0x70000` cursor block, the `0x60000` and `0x68000` gamma maps and
  the `0x80000` capture words are named by Linux and untouched by Linux and by the NT miniport routines read (the PROM
  writes an identity gamma, a cursor glyph left disabled, and read-back tests the capture words; see
  [prom-init.md](../devices/cobalt-graphics/prom-init.md)). Where NT
  puts gamma and how the 2D cursor is drawn is unresolved. Route: the display driver `sgcodd.dll` (it may draw the
  cursor itself and send gamma as engine packets) and the Windows 2000 driver.
- **PLL and clock.** The `sysclk` register (`+0x0c`), the purpose of the one-second PLL restart in NT's mode set, and
  `ctrlstat` bit 25.
- **Pixel order.** Whether 32-bit pixels are red-first in memory (Linux) or red in the top byte of the word (NT's
  `ArsenicPixelFormats`), and what the config bit `LENDIAN` does. Route: a Linux boot of the real hardware, or the
  pixel-format table's users in `sgcodd.dll`.
