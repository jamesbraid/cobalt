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
  the link word's interrupt and sync flags raise. The software's side of those flags is read
  ([link word flags](../devices/cobalt-graphics/graphics-engine.md#link-word-flags-what-the-software-expects-of-the-engine)): an
  interrupt flag stops the engine at the next node and the driver restarts it; the sync flag alone is only the occlusion closer.
  What the hardware does on the sync flag, and which cause bit each flag raises, is unknown: no shipped software depends on more.
  Emulator choice: stop on the interrupt flag only, raise cause `0x40000000` for it and `0x20000000` when the ring runs dry, and
  treat the sync flag as a drain of earlier writes that posts the occlusion bit without stopping. Routes tried: the OpenGL driver in all three builds (every constant
  header and its routine), the display driver and miniport, the program databases of the OpenGL driver and the miniport, SGI's graphics
  white paper (features only), a registry export (identity only). Next: nothing in the shipped software decides these; a
  model chooses and documents a value.
- **CPU aperture windows.** The NT 4.0 miniport maps physical `0x80000000 + (id << 24)` for ids 0, 2, 5 and 6 and returns the id 2 window as the
  frame buffer ([format](../devices/cobalt-graphics/graphics-engine.md#cpu-aperture-windows-at-0x80000000)). Open: which block decodes the
  range (no register of the memory controller or the CPU block is touched for it), whether the descriptor's tile geometry or 8 KiB steps
  index the table, what a line beyond the buffer's row length reads, and what a read does. Routes tried: the miniport, display driver and
  both NT 4.0 HALs, the Windows 2000 HAL and the PROM (no other user of the range). Next: nothing in the shipped software decides
  these; a model chooses and documents a value, and the first text-mode screen shows whether the layout is right.
- **Host setup versus engine setup.** Answered for the driver's side
  ([draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md#who-does-what)): transform is the engine's only
  when the clip-volume hint is `GL_FASTEST`, the user enables it or a variable forces it, and the primitive is not drawn by
  a host fill routine; lighting is the engine's for up to four lights unless a light, colour material or the soft-lighting
  switch puts it on the host; texture coordinates are always the host's. Open: whether the engine clips when it transforms
  (the transform word has a clip-check bit that no routine sets).
- **Engine register window.** The `0x08000` entries (the first 4 KB page frame number of a tile, the tile size coming from the buffer's descriptor) and `0x0c000` entries (buffer descriptors),
  the `0x10000` start word (idle count, bit 11, node position), `0x10008`, the two interrupt causes, `0x185d0` and
  `0x18800` are decoded as the drivers use them ([graphics-engine.md](../devices/cobalt-graphics/graphics-engine.md)).
  The driver's expectation of `0x18800` is read (a save of the outgoing context into the saved-state node, then a wait on bit 31
  of `0x10018`). Open: which registers the engine writes at the dump offset and how much of the 208-dword image it covers; whether
  the engine follows the link word, the chain packet's position or both; whether the two address windows that reach the packet pages
  (`+0x80000000`, `+0xc0000000`) differ in anything but caching; what separates the two causes in
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
- **Pixel order and the scan-out expansion.** Whether 32-bit pixels are red-first in memory (Linux's declaration: red 7:0) or red in
  the top byte of the word (NT's `ArsenicPixelFormats`, Windows 2000's `sglfb.sys`), and what the config bit `LENDIAN` does; and how
  a 15-bit pixel is widened to eight bits per component on the way out. Routes tried: every shipped writer's window-table word and
  colour-map, gamma and config set-up at each depth, the miniport's pixel-format table, the Linux driver and header, and the public
  report of X11 on the real machine (it works at depth 15, 32 bits was broken for another reason); the 16-bit layout agrees everywhere,
  the 32-bit one does not, and the config bit is the same in every writer. Written up in
  [framebuffer-driver-floor.md](../devices/cobalt-graphics/framebuffer-driver-floor.md#5-status-what-must-be-bit-exact-and-what-may-be-approximated).
  Emulator choice: red in the top byte of the word at 32 bits, replication of the top bits for the expansion. Settles it: a chip
  description, or a shipped routine that reads the screen back and uses the bytes.
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
  - Why the linear form's line pitch follows the width register's remainder. Whole columns apart when the register has a
    remainder (the firmware, `sglfb.sys` and `sglfb.dll`) and the visible width apart when it has none (`sgibvid.dll`, whose row
    step is the mode width, and Linux) fits every shipped writer
    ([display-engine.md](../devices/cobalt-graphics/display-engine.md#frame-width-line-pitch-and-the-visible-rectangle));
    what is not known is the mechanism, where the hardware takes the visible width from when there is no remainder (the
    `vt_hpixen` window is the only per-mode width in the block), and whether the shipped Windows 2000 boot video and the Linux
    console displayed correctly on the machine (no report says so for `sgibvid.dll`). Routes tried: every shipped writer (NT
    miniport, `sglfb.sys` and `sglfb.dll`, `sgibvid.dll`, firmware 1.1005), the public Linux driver and header, the public mailing
    list report of Linux X11 modes (ambiguous about the mode). One firmware path (`1.1005@0x1140542`, the region copy in the console table) indexes destination rows by the visible width
    and mixes the two layouts (source rows by the stride); whether the console reaches it is the next route (find the callers of table slot `0x11ddf8c`). Settles it: a chip description, or a report of `sgibvid.dll`'s
    boot screens on the machine. Emulator choice: take the pitch from the remainder as above and show the width given by the
    `vt_hpixen` window and the height by `vt_vpixen`.
  - The write semantics of the register at `0xca000000` (write-0-to-clear is inferred from the complement write) and what its other
    bits are. Settles it: the Cobalt interrupt code in the Windows 2000 HAL for pin 24, or a chip description.

## 2D, blit and video: what the shipped software does not settle

Routes tried for all of these: the NT 4.0 5.2.4 display driver and miniport read in the typed decompile with every immediate
checked in the disassembly, the Windows 2000 5.2.4 display driver by header immediates, the digital media pack's video
driver. Written up in [2d-blit-video.md](../devices/cobalt-graphics/2d-blit-video.md).

- **Engine side of every 2D word.** What the engine does with the descriptor, byte mask and columns of the rectangle clear when
  the window origin is non-zero; with primitive type 6 (`vFillRectSpans`); with the direction bits of a copy; with the
  third-of-pixel field of the styled-line repeat word; with mode-word bits 26, 5 and 4 of the pixel-transfer window packet. Settles it: a model
  that chooses and documents a value, or a published description of the engine.
- **What the engine does with span mode, a luminance draw buffer, and draws past a descriptor's tile columns.** Span mode (bit 18 of
  a colour-format word) is set only on the host-buffer row transfers; no shipped path shows a difference. Emulator choice: draw every
  textured pixel-transfer line as an end-exclusive span either way. A draw buffer of colour type 6 (text at 8 bpp) takes its index from bits
  15:8 of the colour in the driver's use; whether the engine converts a full colour to luminance for any other use is unknown. Emulator choice: take
  bits 15:8 for a type 6 draw buffer. A fill or clear past the descriptor's tile columns (the driver sends one only
  when a descriptor still describes the previous mode, in its whole-buffer clears at a mode change) may clamp, wrap into the next tile
  row or be ignored. Emulator choice: write the columns the descriptor has and drop the rest. Written up in
  [2d-blit-video.md](../devices/cobalt-graphics/2d-blit-video.md#host-buffer-transfers-span-mode-and-the-and-and-xor-passes) and
  [the clip page](../software/windows/engine-clip-degenerate-and-line-ends.md#5-do-fills-and-clears-ever-run-past-the-descriptors-tile-columns).
  Settles it: a published description of the engine.
- **Why NT 4.0 radio buttons draw as black discs after a mode change from an 8 bpp boot.** Each button is two host-buffer passes,
  AND with a mask bitmap and XOR with an image bitmap ([2d-blit-video.md](../devices/cobalt-graphics/2d-blit-video.md#host-buffer-transfers-span-mode-and-the-and-and-xor-passes)),
  so a black disc means the image bitmap held zero in its interior. In model runs a cold boot at 1024 by 768 and 32 bits draws correct buttons with the
  same packets, and a boot at 640 by 480 and 8 bits followed by a mode change draws black ones at every depth. Inferred: the bitmaps were
  built while the screen was 8 bpp (the 4 bpp to 8 bpp translation of the open question above) and the window system keeps them across the
  mode change. Next: boot at 8 bpp and at 16 bpp cold and compare the button, and look at the image bitmap's interior at the second pass.
- **Which way a model should treat mask bit 27 clipping.** The copy, stretch and transfer routines send a valid mask of 0 and
  draw exact rectangles, so clipping by the valid mask is exercised only by the solid and stipple fills, and then
  redundantly. Settles it: nothing in the shipped drivers depends on it.
- **The map of the CPU window.** The display driver writes the screen with a row pitch of `0x2000` bytes; the miniport routine that
  builds that mapping (video IOCTL `0x230458` and the display driver's `ulScreenBase`) was not read for the way it lays 64 KiB tiles
  into 8 KiB rows. Next: read `CobaltStartIO` and the map-memory path with this in mind.
- **Video input and output.** Four windows (`0xf8000000` to `0xfb000000`, `0x80` bytes mapped each, plus an 8-byte pair each and
  the 8-byte system time counter at `0xff001200`, both in the Lithium audio page) are driven by `sgilivid.sys` (digital media pack 5.2.2, NT 4.0 and Windows 2000 builds). The register
  layout, the DMA descriptor format, the 10-bit field table at `.data` `0x135e0`, the interrupt pins and the video formats were not
  decoded; `viddiag.exe` and `sgivcp.cpl` reach the hardware through `sgidmedia.dll` or `sgival.dll`, not directly. Settles it: reading `sgilivid.sys` entry points and
  its IOCTL table in Ghidra.
- **Texture type codes.** Answered for what the drivers write: the full table of GL formats to type codes, the filter, wrap
  and level-of-detail codes and the environment word are in [textures.md](../devices/cobalt-graphics/textures.md); what is left
  is under [the texture and pixel pipeline](#texture-and-pixel-pipeline-what-the-shipped-software-does-not-settle).
- **Windows 2000 display driver 2D routines.** The 2D header immediates occur 5 to 10 percent more often in the Windows 2000 build
  (draw-buffer sets 38 against 35, flush markers 100 against 89, window parameters 125 against 116); the routines were not
  matched one by one, so whether the extra sites are new operations is unknown. Next: export the build with the vendor's
  function names of the NT 4.0 program database mapped by constants.
- **Trapezoid and path fills, glyph fast paths.** `vSolidTrapezoid`, `vStipTrapezoid`, `vPatTrapezoid`, `bFillSetup`, `bFastFill`,
  `DrvFillPath`, `vTextOutFW_P2C` to `P5C` and `vXferDMA_*` were summarised by their packet sequence and not read in full.
- **Alpha blend in 2D.** `ulAlphaBlend`, its mode and value are tested by the copy routines but no routine read sets them; whether any
  DirectDraw or GDI path ever turns blending on is unknown.

## Texture and pixel pipeline: what the shipped software does not settle

Routes tried for all of these: the NT 4.0 5.2.4 OpenGL driver read in disassembly (texture, blend, depth, stencil, viewport,
context-creation and transfer routines), its program-database types, the Windows 2000 5.2.4 OpenGL driver by string and
table search, the display driver's YUV and fill paths, the white paper (features only). Written up in
[textures.md](../devices/cobalt-graphics/textures.md), [draw-and-buffers.md](../devices/cobalt-graphics/draw-and-buffers.md)
and [opengl-expectations.md](../software/windows/opengl-expectations.md). A model chooses a value for each and documents it.

- **Texel order inside a tile, and the order of tiles.** Unknown: no shipped software depends on it. Every texel reaches texture
  memory through an engine transfer, no routine of the OpenGL driver forms an address inside a tile (the texture store,
  transfer, small-tile and read-back routines were all read), and the CPU aperture window is the only CPU-visible view
  ([opengl-texture-levels.md](../software/windows/opengl-texture-levels.md#question-5-texel-order-inside-a-tile-and-tile-order-in-a-level)).
  A model chooses row-major tiles and row-major rows inside a tile and documents it.
- **How the engine finds mip level k.** Inferred, not read: descriptor `id + min(k, kc)` and the small-level rows of the shared
  tile, from the driver's consecutive descriptors and its offset table
  ([opengl-texture-levels.md](../software/windows/opengl-texture-levels.md#question-4-how-the-engine-finds-level-k)). Open: level
  numbers beyond the lod word's range, bordered levels, and a texture with a missing level. A model chooses.
- **The unused transfer type codes 1 and 11 to 13**, and what type 5's decoder does with the packed 16-bit types. The channel order
  of types 4 and 5 is answered in [opengl-pixel-transfers.md](../software/windows/opengl-pixel-transfers.md#channel-order-of-types-4-and-5).
- **Linear surfaces.** The driver addresses a pinned host buffer as a surface 2048 bytes wide through `(s, t)` and gives the
  engine a page list; the descriptor holds only the slot and bit 19, which is a flat marker, not a row length
  ([decode](../software/windows/nt-display-driver-video.md#flat-host-buffers-row-pitch-and-the-staging-area)). The engine's own
  address rule is inferred only. A model chooses `t * 2048 + s * bytesPerTexel` through the page list.
- **The colour matrix's clamp words** (`GLCOPIXPOSTCMCLAMPMIN`, `GLCOPIXPOSTCMCLAMPMAX`, default 0 and `0xffffffff`): their
  meaning is from the names; no driver path sets them.
- **Primitive-mode bits 8 and 10:9** (set only by an overlapping copy) and **bit 3 of the render node's flags** (set by the
  screen-to-screen escape's argument `0x18`, never read in the window packet builder). Read as a scan direction and an
  unknown; a model may ignore the first when source and destination do not overlap.
- **Sampling arithmetic.** Filter precision, the rounding of level selection, the effect of `lodFracSize`, how a border is
  filtered, and perspective correction. No shipped routine depends on them.
- **Rasterisation rules.** Coverage, fill convention, rounding of the 1/64 pixel position, attribute interpolation precision,
  line and point rules. The 0.492 and 0.508 viewport constants hint at a tie-break but do not state it.
- **Blend, dither and write-mask arithmetic.** Rounding of the blend products, the dither pattern, behaviour of the 4-bit
  colour formats under blend. Nothing in the driver depends on them beyond the codes. Whether the dither bit changes a 32-bit colour buffer is unknown: the driver sets it from `GL_DITHER` alone
  at every depth ([packet-set.md](../devices/cobalt-graphics/packet-set.md#pixel-pipe-mode-word-0x80003020)), so an emulator may leave 32-bit buffers undithered and apply a pattern only to the narrower formats.
- **Depth.** Answered for the driver's side in [opengl-depth-stencil-fog.md](../software/windows/opengl-depth-stencil-fog.md): the
  16-bit word is a three-bit-exponent code the driver decodes with a table, the 24-bit depth is fixed point in the low three bytes
  with stencil in the top byte, and the clears write raw values. Open: the engine's encode rounding, the 16-bit depth with stencil,
  and the `equal_range` field of the depth word, never set. A model encodes on the depth path only and stores raw words as sent.
- **Fog arithmetic.** The driver sends `trunc(255 * f)` per vertex with OpenGL's `f` (1 is no fog); the engine's interpolation,
  rounding and treatment of alpha are unknown. A model blends `(F * C + (255 - F) * C_fog) / 255` per colour channel.
- **The occlusion bit's writer and exact meaning.** The driver side is read ([read-backs](../software/windows/engine-readbacks.md#1-the-packet-nodes-first-dword)):
  one bit, bit 31 of the retired node's first dword, set when a fragment passed the depth test with bits 28:27 of the depth-test
  word on. Unknown: no shipped software depends on more, and the engine's rule for setting it (any pixel, any sample after
  early rejection, count of at most one) is not stated anywhere. Routes tried: the ICD and miniport (the miniport never writes
  the word), the type declarations (`CoPktRespType`). An emulator author chooses: a constant 0 satisfies every caller. No fragment count exists in the software's view: nothing in any driver reads a counter, a status word or a register for one ([read-backs](../software/windows/engine-readbacks.md#summary)), so an emulator may expose only this bit, and any count it adds is its own.
- **Which mode-word bit the opaque stipple and styled-line fields read in 3D.** Unchanged from the 2D list.
- **Clip coordinates and mask 0 against flat buffers; degenerate fans; line ends.** The software's side is read
  ([page](../software/windows/engine-clip-degenerate-and-line-ends.md)). Unknown, no shipped software depends on more: which
  coordinates (before or after the origin) the window clip rectangles use; whether the engine draws a zero-area fan; whether the
  end vertex of a line that is not horizontal is exclusive. Routes tried: every transfer, fill and triangle routine of the OpenGL
  and display drivers and the miniport's window packet builders. Emulator choices: no window clip on flat destinations and mask 0
  unclipped; draw nothing for a degenerate fan and log it; a horizontal line with integer ends covers `x0` to `x1 - 1`.
- **The display driver's pixel-format list** (escape `0x5d`): sizes read ([page](../software/windows/opengl-swap-points-formats-faces.md#the-display-drivers-pixel-format-list-escape-0x5d-and-the-accumulation-buffer)); open: the `dwFlags` bits per entry, and the per-mode list's exact count.
- **Windows 2000 builds, routine by routine.** The OpenGL driver is compared ([comparison](../software/windows/w2k-opengl-driver-comparison.md)):
  2,592 of 2,637 size-aligned routines are instruction-identical and the packet headers, escape sites and extension strings
  are the same. Not done: the display driver and miniport routine by routine, the roughly 270 unaligned ICD runs, and the
  engine path of the added alpha-blend hook. Next route: an own reader for the Windows 2000 program database (PDB 2.00,
  debug-info stream 19960307, which Ghidra 12.1.3 refuses) to name every routine, then the same alignment.
- **What `SET_READY_TEXTLBPKT` (escape `0x59`) arms.** The display driver copies a stored link field in its own record of the
  texture packet; the miniport is not called and no packet word is written. Unknown: no shipped path shows what reads the
  field afterwards. An emulator author keeps the bookkeeping or drops the escape.
- **How NT 4.0's `opengl32.dll` reads the driver registration, and the command-memory mapping on Windows 2000.** The NT 4.0 and
  Windows 2000 patches both write one `OpenGLDrivers` value (`SGI` = `sgcoicd.dll`); only the Windows 2000 `opengl32.dll` is read
  here, so what NT 4.0's does with that layout (and whether it has any `Flags` notion) is inferred
  ([registration](../software/windows/nt-opengl-icd.md#registration-and-the-version-handshake)). The Windows 2000 miniport holds
  the `\Device\PhysicalMemory` name the NT 4.0 packet-pool mapping uses, but its allocator was not read, so the two-view mapping
  of the pool is established for NT 4.0 only. Routes tried: the NT 4.0 files with the vendor's names, the installers' tables, the Windows
  2000 `opengl32.dll` and the Windows 2000 SGI files by constants. Next: the Windows 2000 program-database reader (see the entry
  on Windows 2000 builds) to name `sgcomp.sys`, then read its pool allocator; an NT 4.0 `opengl32.dll` or a setup log from an
  NT 4.0 machine would settle the first, and none is held.
- **Escapes `0xcace`, `0xcaca` and the `0x11` group of the display driver.** Reachable only by a caller that is not the OpenGL
  driver; the OpenGL driver sends none, so no OpenGL path depends on them. The control-panel applet was not read for them.
- **Whether Windows XP's graphics stack accepts the Windows 2000 pack.** Settled only as far as the imports and headers
  go ([the XP page](../software/windows/w2k-xp-compatibility.md)); `win32k` and the video port of XP are not held. An emulator
  author assumes it loads and observes.
- **Why the NT 4.0 display driver at 8 bits per pixel can draw only black, teal and grey for colour icons.** A model run of the
  installed April 1999 driver at 640 by 480 shows every opaque icon pixel, the Display Properties monitor picture and the Internet
  Explorer start page background written through CPU aperture window 2 with the values 0, 6 and 7 only, while a Start button
  bitmap copied by the engine from off-screen memory carries `0xf8` to `0xff`. The driver reads no palette hardware and builds
  its translation tables without device input ([nt-display-driver-video.md](../software/windows/nt-display-driver-video.md#the-8-bits-per-pixel-palette-colour-translation-and-the-cpu-drawn-paths)).
  Next: check whether the engine pixel DMA of `vXferDMA_SB` returns screen contents and honours the logic operation word that
  `vXferDMA_BS` sends, and where GDI's 4 bpp to 8 bpp translation vector first becomes zero (the `XLATEOBJ` handed to `DrvCopyBits` and
  `bPuntBlt`).
  A second 8 bpp symptom is explained by the engine's side: white text drawn black (selected list and combo items, the white
  labels of the display applet's test bitmap) is the cached-glyph route's type 6 draw buffer with the index in bits 15:8 of the colour
  ([2d-blit-video.md](../devices/cobalt-graphics/2d-blit-video.md#text)); an engine that takes bits 7:0 writes 0. What is left is the
  test bitmap's gradient bars, which draw black while its flat colour bars are right. A flat colour GDI cannot
  map to a static entry is a dithered brush (`DrvDitherColor`, sixteen static indices), drawn by the pattern route: `vPatRealize`
  (64 point draws of the pattern byte into the brush cache), `vTexRealize` (a screen-as-texture copy into the pattern texture) and
  `vFillPat` (a textured fill with the 8 by 8 map). At 16 and 32 bits the same bars are solid fills, so this route is exercised
  only at 8 bpp. Next: check each of the three stages at 8 bpp (the cache points land with the index, the texture copy keeps it, the fill
  reads it), and read the colour map after the test bitmap's palette is realised: `DrvSetPalette` ignores the start index GDI passes and
  writes the run at entry 0.

## Rendering engine arithmetic and vertex semantics

What [the triangle-path reading](../software/windows/opengl-triangle-path.md) leaves for an emulator author to choose.
No shipped routine or published document decides these; routes tried: the OpenGL driver's vertex, viewport, matrix,
lighting and colour-material routines, the display driver and miniport (draw flags, viewport and default context image).
Next: the Windows 2000 build of the OpenGL driver, by header constants, only repeats the driver side.

- **Missing z and w on a vertex.** Port A has no z; under the object-coordinate mode neither z nor w is sent. The model
  must supply z = 0 and w = 1 (inferred from matching the host path); what the engine does under the other two modes
  with the missing z is unknown (depth is off there).
- **Clip, w at or below zero.** Whether the engine clips, rejects or flags; the clip-check bit is never set.
- **Order and rounding of viewport offset, window origin and the 1/64-pixel conversion.** The sum is inferred; where it
  rounds is unknown. The bias pair is inferred to be a texture-coordinate offset; its interplay with the miniport's
  switch-in patch for a 3D context is unknown.
- **Host colour with lit vertices.** Added before or after the engine's clamp; whether ambient is scaled by the light
  scalar; the lighting-word specular-only bit (never set by the driver).
- **Diffuse block instance 1.** Inferred to mean "also set the ambient colour"; the byte that selects it in the code
  generator is not traced.
