# Windows 2000 display drivers for the Visual Workstation: sgibvid, sglfb

The two SGI-specific video files on the Windows 2000 CD: the boot video driver `sgibvid.dll` and the "Cobalt Graphics"
frame buffer pair `sglfb.sys` and `sglfb.dll`. They program the display back end (the chip SGI's own software calls
Arsenic) and nothing else, so they are the smallest display path in any shipped binary. The register pages are
[the display back end](../../devices/cobalt-graphics/display-engine.md); the NT 4.0 drivers are in
[nt-software.md](../../devices/cobalt-graphics/nt-software.md).

Source: Windows 2000 RTM (build 2195, CD dated 7 December 1999) `\I386\sgibvid.dll` (10,176 bytes, built 1999-11-04),
`sglfb.sys` (6,992, 1999-10-11) and `sglfb.dll` (9,136, 1999-11-30). Windows 2000 SP4 (build 2195.6717) carries
byte-identical copies (SHA-256 prefixes compared). The text-mode setup map lists them under the SGI machine type
`sgiborg_mp`: `sgibvid.dll` is copied as `bootvid.dll`, and the `sglfb` pair is the display type "Cobalt Graphics".
Ratings here are the page's original five (see [the overview](../../devices/cobalt-graphics/README.md#grades)).

**`sgibvid.dll`: the boot video driver. Established (read in Ghidra; the build has a debug-file name but no symbol file was found, so routine names
other than the eleven exports are mine).** It exports exactly the eleven `BOOTVID.dll` entry points (`VidInitialize`, `VidResetDisplay`, `VidCleanUp`,
`VidDisplayString`, `VidDisplayStringXY`, `VidBitBlt`, `VidScreenToBufferBlt`, `VidBufferToScreenBlt`, `VidSetScrollRegion`, `VidSetTextColor`, `VidSolidColorFill`), so NT's
loader and kernel use it in place of the stock VGA `bootvid.dll`. Its own messages are prefixed "SGI> BOOTVID.DLL". What it does that matters here:

- It maps `0xd0000000` for `0x80020` bytes ("Failed to map Arsenic Space") and requires the low 16 bits at `+0x14` to read `0x666` ("Failed to find SGI Arsenic chip ID").
  So the vendor's own software calls the chip behind the display back end Arsenic; that agrees with the `as` prefix and the pixel-format table name, and turns
  the earlier inference that the block is Arsenic into the vendor's naming (the chip's name in SGI software; hardware-level identity still comes from no document).
- It does not use the firmware's frame buffer. It allocates its own 8-bit-per-pixel buffer from contiguous memory (rows rounded up to 512 bytes, 64 KiB aligned,
  filled with `0xff`), allocates a second small row buffer, and programs the display back end from scratch. It tells a 1600 by 1024 flat panel
  apart from a CRT by what the firmware left behind: the flat-panel horizontal drive's low 12 bits equal `0x640` (1600) and the vertical drive's `0x401`. With the
  panel it uses the 1600 by 1024 timing (1670 by 1080 total); otherwise 640 by 480 (800 by 525 total). The text is drawn into a 640 by 480 virtual screen
  and scaled to the mode by an integer factor, centred.
- The sequence it writes is the one [the display page](../../devices/cobalt-graphics/display-engine.md) lists for the NT 4.0 miniport and Linux, and adds three things
  neither of those did: it sets the "linear" bit (bit 1) of the frame control with a 64 KiB-aligned physical address, writes the window table at `+0x48000` (all 32 words `3`), and
  writes an identity ramp (`i * 0x01010100`) into the gamma map at `+0x60000`. It loads 16 palette entries (the VGA colours, scaled to six-bit components) through the
  colour map at `+0x50000`, waiting until the FIFO status at `+0x58000` shows its low six bits above `0x30` ("Setting color map" on timeout). It waits 20 ms between its first writes
  and the timing, and clears the frame-control DMA-enable bit, then waits for the live copy at `+0x30008`, first.
- It writes the configuration word at `+0x18` as `0xad` with a CRT and `0xac` with the panel (DAC on or off, panel enable always set).

**`sglfb.sys` and `sglfb.dll`: the Windows 2000 "Cobalt Graphics" display driver. Established (read in Ghidra, no symbols).** The text-mode setup map names the display
"Cobalt Graphics" and loads `sglfb.sys` for it; the INF-less pair is Microsoft's, built for this machine, and is only a frame buffer driver: the display driver
imports nothing but the GDI engine's bitmap, palette and I/O-control calls and draws in software; neither file touches the Cobalt rendering engine (`0xc8000000`) or the
CPU block revision at `0xc2000000`. The miniport:

- claims `0xd0000000` to `0xd008001f` through `IoAssignResources`, maps `0x80020` bytes, requires the same ID `0x666` at `+0x14`, and finds the flat panel by the same
  `0x640` and `0x401` test. If the ID or the claim fails it reports "no such device".
- gets its memory from the registry value pair `GraphicsMemory` (base, must be non-zero) and `GraphicsMemorySize` (at least 4 MiB) under `SGICtrl\Memory`: the two values that the HAL's
  `HalpAddGbeDisplayToRegistry` writes first ([nt-hal.md](https://github.com/jamesbraid/chips/blob/main/software/visws/nt-hal.md)). It does not read the bank list (`NumberOfBanks`, `BankAddrNN`, `BankSizeNN`) that the SGI 5.2.4 miniport demands; a Windows 2000
  HAL that wrote only the first two would satisfy this driver and fail the SGI one.
- offers only 8 and 32 bits per pixel, filtered by how many 64 KiB tiles fit in the graphics memory; the 32-bit mode reports colour masks `0xff000000`, `0xff0000` and
  `0xff00`: **red in the top byte of the pixel**, and the window table value for 32 bpp is `0x17` (RGB8 type, both buffers). This is a second NT driver agreeing with the
  `ArsenicPixelFormats` table's byte order, against Linux's low-byte red ([nt-software.md](../../devices/cobalt-graphics/nt-software.md)). Software expectation.
- sets the mode with the display page's sequence, writing the frame control with the "linear" bit and a pointer from the tile list it is given, the window table at `+0x48000`
  (`3` for 8 bits per pixel, `0x17` for 32), the configuration word `0xa5` (`0xad` with a panel), and `0x30000000` into the control and status word last. It writes the colour map
  (`+0x50000`) with the same FIFO wait as the other drivers, and sets the chip name strings "Cobalt Graphics ChipSet" and "Cobalt Graphics Backend" with no revision letter.
- answers the standard video-port control codes (available modes `0x230400`, mode count `0x230404`, current mode `0x230408`, set mode `0x23040c`, reset `0x230410`, colour registers
  `0x23041c`, map and unmap video memory `0x230458` and `0x23045c`, public access ranges `0x230460`, which maps `0xd0000000` for `0x80100` bytes into the display driver), the same
  codes the SGI 5.2.4 miniport dispatches at the base `0x230400`.

An emulator that provides the display back end of [display-engine.md](../../devices/cobalt-graphics/display-engine.md) and the two registry values gets a working Windows 2000 console from
`sglfb` and `sgibvid` without any engine model. That is the smallest known Cobalt display path in any shipped binary. Software expectation (it is what the drivers do on real
hardware that was never traced).
