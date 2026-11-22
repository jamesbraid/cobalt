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
- **Cursor, gamma and video capture.** The NT miniport writes the `0x60000` and `0x68000` gamma maps and the NT display
  driver programs the `0x70000` cursor block (position, control, three colours, a 32 by 32 two-bit glyph; see
  [display-engine.md](../devices/cobalt-graphics/display-engine.md#colour-map-gamma-cursor-video-capture)). Open: the hardware's
  reading of the cursor words (inferred from the O2's GBE layout), what the 10-bit gamma map's entries mean, and the
  `0x80000` capture words, which Linux names and neither NT driver touches (the PROM writes an identity gamma, leaves a
  cursor glyph disabled and read-back tests the capture words; see [prom-init.md](../devices/cobalt-graphics/prom-init.md)).
  Route: the Windows 2000 drivers' own cursor use, and the video capture software on the OEM discs.
- **PLL and clock.** The `sysclk` register (`+0x0c`), the purpose of the one-second PLL restart in NT's mode set, and
  `ctrlstat` bit 25.
- **Pixel order.** Whether 32-bit pixels are red-first in memory (Linux) or red in the top byte of the word (NT's
  `ArsenicPixelFormats`), and what the config bit `LENDIAN` does. Route: a Linux boot of the real hardware, or the
  pixel-format table's users in `sgcodd.dll`.
- **Frame-buffer floor: what the hardware does that the five boot-time writers cannot show.** The writers (the NT 4.0 miniport and
  HAL, the Windows 2000 boot video driver and frame buffer miniport, the firmware) accept different orders and waits, so the
  contract is written ([framebuffer-driver-floor.md](../devices/cobalt-graphics/framebuffer-driver-floor.md#5-status-what-must-be-bit-exact-and-what-may-be-approximated)).
  Open, each with the route tried (every shipped writer read from the disassembly) and what would settle it:
  - What `ctrlstat` bit 25 is and when it reads set. Settles it: a software reader of the bit other than the firmware's
    one test, or a published chip description.
  - What clearing `dotclock` `RUN` does. The NT 4.0 HAL clears it and writes the new value with `RUN` set in the next
    store with no wait; the miniport waits 40 ms to 1 s around the same pair. Settles it: a chip description, or a shipped
    routine that depends on the blanking.
  - The live-copy delay (the vertical-blank rule is inferred from the O2's display back end) and why the firmware waits for the
    live bits to read set when it stops the display. Settles it: a chip description.
  - The pitch of the linear form (`frm_control` bit 1) for widths that are not a multiple of 512 bytes. The firmware and the
    boot video driver round the pitch up, the frame buffer miniport's own surface pitch was not read. Settles it:
    `sglfb.dll`'s surface setup read for 32 bits per pixel, a width of 640 and the pitch it reports.
  - The write semantics of the register at `0xca000000` (write-0-to-clear is inferred from the complement write) and what its other
    bits are. Settles it: the Cobalt interrupt code in the Windows 2000 HAL for pin 24, or a chip description.
