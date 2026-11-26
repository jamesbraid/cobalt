# Cobalt display back end (DBE): registers, scan-out and modes

The display half of the Cobalt graphics subsystem is a register block at physical `0xd0000000` that matches, offset for
offset, the display back end of the O2 (see [GBE](https://github.com/jamesbraid/chips/blob/main/devices/gbe/README.md)): same timing generator, same tiled frame buffer
fetch, same colour map, gamma, cursor and video-capture blocks. That identity is established for every offset and field
both drivers touch; for the blocks neither drives (cursor, capture, 10-bit gamma) it is inferred from the identical
layout, and the GBE page's field tables are the best available description of them, untested on Cobalt. Linux calls
the block DBE and the NT miniport prefixes its routines with `as`. Back to the [overview](README.md). The registers below are also defined, with offsets, widths, fields and grades, in [the Arsenic register definitions](../arsenic/registers.md), from which the generated C, Rust and Python outputs are built. A scan of the NT miniport's and display driver's disassembly for displacements into the block found no access outside the registers on this page.

All accesses are 32 bits wide and uncached (Linux `sgivwfb.h`: "all loads and stores must be 32 bits"). Offsets below
are from `0xd0000000`.

Evidence per row: **L** = Linux `sgivwfb.c` and `sgivw.h` (public); **N** = the routine in NT `sgcomp.sys` v5.2.4 that
writes it (read in full); both agree where both are named. Grades are the overview's. No reset value is established for
any register: neither driver reads one before writing, and both only ever see a post-firmware state.

## General block

| Offset | Name (Linux) | What | Evidence | Grade |
|---|---|---|---|---|
| `0x000` | ctrlstat | chip ID in `[3:0]` (read by Linux as a constant field), `SENSE_N` `[4]`, `PCLKSEL` `[29:28]`. NT writes `0x30000000` as the last step of every mode set; Linux writes `0x20000000` before programming and `0x30000000` afterwards if bit 25 reads set. Linux treats `PCLKSEL` below 2 as "not scanning out from the internal dot clock". Meaning of bit 25 unknown | L, N | inferred |
| `0x004` | dotclock | PLL: `M` `[7:0]`, `N` `[13:8]`, `P` `[15:14]`, `RUN` `[20]`. Both drivers write `M-1` and `N-1`. Output is 27 MHz times `M` over `N`, divided by 2 to the `P`; reproduces the clock column of 26 of the 27 NT table rows to within 1.4 percent (see below) | L, N | inferred (fields), inferred (formula) |
| `0x008` | i2c | CRT I2C lines (DDC). NT's `CoDDC_GetEDID` reads an EDID over it, or over `i2cfp` when a flat panel answered the probe. Bit 0 SDA, bit 1 SCL, 1 pulls the line low (see below) | L, N | established (NT) |
| `0x00c` | sysclk | system clock PLL. Neither driver touches it | L (name only) | unknown |
| `0x010` | i2cfp | flat-panel I2C lines, bit-banged. See below | L, N | established (NT) |
| `0x014` | id | NT refuses to attach unless it reads `0x666` | N | inferred |
| `0x018` | config | see below | L, N | inferred |
| `0x01c` | bist | internal self-test. Neither driver reads it; the PROM's self-test writes 0 and polls bit 31 for completion, then reports a failure when bits 27:24 are set ([prom-init.md](prom-init.md#the-id-check-and-the-register-self-test)) | L (name only), PROM | inferred |

**Dot clock start sequence (NT `asSetConfigAndTiming`).** Clear everything above bit 15, wait 40 ms, write `0x4101`
(`M=2, N=2, P=1`), wait 40 ms, write `0x104101` (adds `RUN`), wait one second, write `0x4101`, wait 100 ms. Then, after
the timing registers below, write the real value without `RUN`, wait 40 ms, set `RUN`, wait 200 ms. No lock bit is
read. What the driver does; the purpose of the one-second dance is unknown. Linux writes the real value
with `RUN` and waits 11 ms.

**Config register (`0x018`).** Bits from Linux names: `0x01` video DAC enable, `0x02` sync on green, `0x04` pixel
blank, `0x08` flat-panel enable, `0x20` little-endian pixel memory, `0x40` tile history, `0x80` extended addressing.
NT writes `0xe5` (all of `0x80`, `0x40`, `0x20`, `0x04`, `0x01`), adds `0x02` when the mode asks for sync on green or a
boot option says so, always adds `0x08`, and clears `0x01` when a flat panel answered the probe unless the mode carries
the "magic key" flag (`0x2000`). Linux defines a frame-buffer value of `0xa7` (`0x80`, `0x20`, `0x04`, `0x02`, `0x01`) but its mode-set code never
writes the register; the PROM writes that value ([prom-init.md](prom-init.md)). Inferred (driver behaviour); a model should accept any value. Windows 2000's `sglfb.sys` writes `0xa5` (`0xad` with a flat panel, adding bit 3 and keeping the DAC on) and its boot video driver `0xad` with a CRT and `0xac` with a panel, so a third and fourth value are seen; none writes bit 6 (tile history) or bit 1.

**What the two Windows 2000 drivers add (`sgibvid.dll`, `sglfb.sys`; established as their code, inferred as hardware).** Both are described in
[NT drivers](../../software/windows/w2k-display-drivers.md). They write the same timing, tile and PLL registers in the same order as the SGI
5.2.4 miniport above, and both (and the earlier Linux driver) set the linear bit. Both find a flat panel by reading back what the firmware left in the panel drive registers:
`fp_hdrv` low 12 bits `0x640` and `fp_vdrv` low 12 bits `0x401` (1600 columns, 1025 rows). That the firmware leaves those two registers set for the 1600 by 1024 panel before the OS starts is established: the PROM's mode set writes `0x640` and `0x1401` for panel id 5 ([prom-init.md](prom-init.md#flat-panel-registers)); the SGI miniport writes the row end as `H + 1`, 1025, and so leaves the same `0x401` in the low 12 bits that these drivers test for (an earlier version of this page had it write `H` and called the value different; the disassembly of `asSetConfigAndTiming` increments the row count before the mask). Both require `id` (`+0x14`) to read `0x666`. The boot video driver's own table has two timing records, 640 by 480 (800 by 525 total) and the 1600 by 1024 panel (1670 by 1080
total), in the same record layout as `cobaltVTimings`; it is the only place a Windows driver programs the display with no SGI pack present, so a model that satisfies it
satisfies the Windows 2000 setup display path.

## Video timing block (`0x10000`)

Every window register is `ON` in bits `[23:12]` and `OFF` in bits `[11:0]`. The right-hand column gives what NT
writes from its mode record (width `W`, height `H`, totals `Ht` and `Vt`, blank, sync and PLL fields; the record is the
same layout as Linux's `dbe_timing_info`). Linux differs only where noted. All entries: inferred.

| Offset | Name | NT writes |
|---|---|---|
| `0x10000` | vt_xy | live `x` `[11:0]`, live `y` `[23:12]`, `FREEZE` `[31]`. NT writes 0 first if frozen; its `asGetScanline` returns `[23:12]`. Linux treats `FREEZE` as "counter stopped" |
| `0x10004` | vt_xymax | `Ht` in `[11:0]`, `Vt` in `[23:12]` (zero while programming) |
| `0x10008` | vt_vsync | vsync start and end |
| `0x1000c` | vt_hsync | hsync start and end |
| `0x10010` | vt_vblank | vblank start and end |
| `0x10014` | vt_hblank | `ON = (hblank_start - 5) mod Ht`, `OFF = hblank_end - 3`. Linux writes `ON = hblank_start` |
| `0x10018` | vt_flags | `[2]` invert hsync drive when the mode flag `0x10` is set, `[0]` invert vsync when `0x20`. NT's `asSetPowerState` also drives `[1]` and `[3]` (below). NT writes `0x40` then `0` during programming, and its retrace handler sets bit 6 in stereo modes |
| `0x1001c` | vt_f2rf_lock | frame-to-field and frame-lock Y: `(vblank_start << 12) \| 0xfff` (mono), `(vblank_start << 12) \| vblank_start` (stereo). Linux does not write it |
| `0x10020` | vt_intr01 | scanline interrupts 1 (`[11:0]`) and 0 (`[23:12]`). NT parks both at `0xfff` (`0xffffff` written) |
| `0x10024` | vt_intr23 | the same for interrupts 3 and 2 |
| `0x10028` | fp_hdrv | flat-panel horizontal drive; NT sets `OFF = W` when a panel is present |
| `0x1002c` | fp_vdrv | `ON = 1`, `OFF = H + 1` (NT writes `0x1000 \| ((H + 1) & 0xfff)`, that is `0x1401` for the 1024-row panel; with no panel `OFF` is 0). The disassembly increments the row count before the mask |
| `0x10030` | fp_de | flat-panel data enable; NT writes `0xffffff` |
| `0x10034` | vt_hpixen | `ON = hblank_end - 19` (wrapped by `Ht`), `OFF = (ON + W - 2) mod Ht`. Same in Linux |
| `0x10038` | vt_vpixen | `ON = vblank_end`, `OFF = vblank_start`. Same in Linux |
| `0x1003c` | vt_hcmap | colour-map write window, horizontal: `hblank_start`, `hblank_end` (Linux: `hblank_end - 3`) |
| `0x10040` | vt_vcmap | vertical: `vblank_start`, `vblank_end` |
| `0x10044` | did_start_xy | `Y = vblank_start - vblank_end - 1` (a small negative number), `X = hblank_end - 20` (wrapped) |
| `0x10048` | crs_start_xy | `Y` one more than the DID `Y`, `X = hblank_end - 54` (wrapped) (Linux calls 54 `DBE_CRS_MAGIC`) |
| `0x1004c` | vc_start_xy | same `Y` as DID, `X = hblank_end - 4` |

The NT code writes `vt_hpixen`, `vt_vpixen` first as `0xffffff` (off), starts the PLL, then writes the real values
last. The scanline-interrupt registers are written only through `asSetInterrupt(n, y)`, which accepts `n` below 4 and
`y` below 4096. Inferred (driver behaviour).

**Blanking and power (DPMS).** NT's `asSetPowerState` takes states 1 to 6 and edits bits `[3]` and `[1]` of `vt_flags`:
state 1 (on) clears both, 2 (standby) sets `[1]`, 3 (suspend) sets `[3]`, 4 to 6 set both. Which signal each bit
gates is inferred (bit `[1]` horizontal, bit `[3]` vertical, from the VESA states). With a flat panel present NT
instead sets the panel's power-save bit over I2C.

## Overlay, normal plane, DID

| Offset | Name | What | Grade |
|---|---|---|---|
| `0x20000` | ovr_width_tile | overlay plane width: whole 512-byte tile columns in `[12:5]`, remainder in 32-byte units in `[4:0]`, `FIFO_RESET` `[13]` pulsed set then clear (NT and Linux). Overlay is always 8 bits per pixel | N, L: inferred |
| `0x20004` | ovr_inhwctrl | live copy; bit 0 is the DMA-enable that NT polls until it reads clear | inferred |
| `0x20008` | ovr_control | shadow; bit 0 DMA enable, rest the overlay's tile-list pointer. The miniport's `asStartFrm` loads the list pointer without bit 0; the display driver's `vSetOvrFetch` writes the pointer with bit 0 set to start the fetch and the pointer alone to stop it | inferred |
| `0x30000` | frm_size_tile | normal plane. `[12:5]` whole tile columns, `[4:0]` remainder in 32-byte units, `[14:13]` depth (0 = 8 bpp, 1 = 16, 2 = 32), `[15]` `FIFO_RESET` pulsed. A tile is 512 bytes by 128 lines, so a 32-bit mode has 128 pixels per tile column and a 16-bit mode 256. NT computes exactly this and writes the register twice around the reset pulse | established (Linux and NT agree, GBE page agrees) |
| `0x30004` | frm_size_pixel | height in `[31:16]` | established |
| `0x30008` | frm_inhwctrl | live copy; bit 0 live DMA enable, polled by NT | inferred |
| `0x3000c` | frm_control | shadow. Bit 0 DMA enable, bit 1 "linear" (Linux and both Windows 2000 drivers set it, the SGI 5.2.4 miniport does not), `[31:9]` pointer: Linux gives the frame buffer's physical address shifted right 9, NT gives its tile list. Both stop DMA by clearing bit 0 and wait up to 20 000 ten-microsecond steps for the live copy to follow | inferred |
| `0x40000` | did_inhwctrl | live copy; NT tests bit 16 (byte at `+0x40002`, bit 0) | inferred |
| `0x40004` | did_control | shadow. NT writes 0 at mode set and clears bit 16 to stop. Linux names bit 0 and writes 0 | inferred; Linux's bit looks wrong, NT and the O2's GBE use bit 16 |
| `0x48000` | mode_regs[32] | per-DID window table: buffer mode `[1:0]` (3 = both), type `[4:2]` (0 = 8-bit index, 1 = 12-bit index, 2 = RG3B2, 3 = RGB4, 4 = ARGB5, 5 = RGB8, 6 = RGBA5, 7 = RGB10). Linux fills all 32 identically (8 bpp = index, 16 bpp = RGBA5, 32 bpp = RGB8). The SGI 5.2.4 miniport's mode-set code does not write this table. Windows 2000's boot video driver writes `3` into all 32 words, and its `sglfb.sys` writes only the first word: `3` for 8 bits per pixel and `0x17` for 32 (buffer mode 3, type 5 = RGB8) | L, W2K: inferred; SGI miniport: not touched in the routines read |

**The cursor, as the NT display driver drives it.** `sgcodd.dll` (not `sgcomp.sys`) writes the cursor block through its `gberegs`
pointer: `vMovePointer` and `DrvSetPointerShape` set `crs_pos` to `((y - hotY) & 0xfff) << 16 | ((x - hotX) & 0xfff)` and `crs_ctl` to 1
to show it, and to `0x0fe00fe0` and 0 to hide it (`ulCursorState` bit 0 means the hardware cursor holds the pointer, bit 1 that it is shown, bit 2 that
a software pointer is in use); The two position fields are 12-bit two's complement numbers: the code subtracts the hot spot and masks with `0xfff` (`vMovePointer@0x283b3` to `0x283d8`, `DrvSetPointerShape@0x28f4c` to `0x28f6b`), and the park value `0x0fe00fe0` (`vMovePointer@0x282ce`) is `0xfe0` in each field, minus 32, which moves the cursor off screen when read as a signed number. Hardware sign extension from bit 11 is inferred from that park value. PROM 1.1005 only writes position 0 (`1.1005@0x11318cb`, read in an earlier pass and not re-read here); `sglfb.sys` writes only the control word (`0x10f33`); neither does any hot-spot arithmetic. `bEnablePointer` sets `crs_cmap[1]` to 0 and
`crs_cmap[2]` to `0xffffff00` (`sgcodd.dll@0x28b4d`, `0x28b57`); `bCopyColorPointerIntoHardware` rewrites all 64 glyph dwords and the 3 colour dwords for a pointer of at most 32 by 32
pixels and three colours (the routine tests both limits at its top; the bodies of those tests are empty in the decompile and are read as a return
to the software path), packing each pixel into 2 bits from the top of the dword, two dwords per row (0 for a transparent pixel, 1 to 3 for the
colour list's entry), and writing the colours as `R<<24 | G<<16 | B<<8`. Any other pointer is drawn in software: the driver keeps three 32 by 32
bitmaps (the pointer and two save-unders) and blits them into the 8-bit overlay plane. Established (`vMovePointer`, `DrvSetPointerShape`,
`bEnablePointer` and `bCopyColorPointerIntoHardware` read in the typed decompile; the colour-list index that the 2-bit value selects, and what the
hardware does with the hot-spot-adjusted position, are inferred from the GBE's cursor). The display driver also starts and stops the overlay plane
itself: `vSetOvrFetch` writes `ovr_control` with the overlay's tile list and bit 0 to start the fetch (for a software pointer or an OpenGL window)
and with the tile list alone to stop it.

**Frame buffer memory.** Both the live and the shadow word are read by software; the engine takes the shadow at the
start of vertical blank. This is how NT page-flips (`asFlipTiles`, `asFlipFrontBack`): it rewrites the tile list or the
list pointer, writes `frm_control`, and relies on the copy. Inferred (driver behaviour).

**Frame width, line pitch and the visible rectangle.** Three separate things, and the shipped writers set them in different
ways.

- *The width register* (`frm_size_tile`, `+0x30000`) is whole 512-byte tile columns in `[12:5]`, a remainder in 32-byte units in
  `[4:0]`, the depth in `[14:13]` and the FIFO reset in bit 15. The value `0x40` is two whole columns, no remainder, 8 bits per
  pixel: 1024 bytes a line. `frm_size_pixel` (`+0x30004`) `0x01e00000` is a height of `0x1e0` = 480. `frm_control` (`+0x3000c`)
  `0x07800003` is linear (bit 1) and enabled (bit 0) at physical `0x07800000` (`0x07800000 >> 9` in `[31:9]`). The NT miniport
  (`sgcomp.sys@0x1be99` to `0x1bf1c`), the firmware (step 13 of [the PROM's display start-up](prom-init.md)) and `sglfb.sys`
  write the exact line length: whole columns plus `(bytes mod 512) / 32`. Linux (`sgivwfb_set_par`) and the Windows 2000 boot
  video driver (`sgibvid.dll`) write the length rounded up to whole columns with remainder 0. Established for what each writes.
- *The line pitch* in the linear form. The shipped writers lay their frames out in two ways, and which one goes with a writer
  follows the remainder field of the width register it writes.
  - With a remainder (the exact form), lines are whole tile columns apart. The firmware's console code stores
    `ceil(W / 512) * 512` as its stride (kept at `0x116dbb0`) and its fill, pixel, text and forward-copy routines step rows by
    it (1024 bytes at 640 pixels, with register `0x24`; the stride is computed at `1.1005@0x113154e` and first used at
    `1.1005@0x1140295`). The register is built at `1.1005@0x11317f6` from the whole columns and the remainder floored to
    32-byte units. One copy path breaks the pattern (see the counter-witness below). `sglfb.sys` reports the same
    number as each mode's screen stride (`ceil(W / 512) * 512` at 8 bits and `ceil(W / 128) * 512` at 32: 1024 at 640 and 800,
    1536 at 1280, 2048 at 1600), and `sglfb.dll` hands that stride unchanged to the engine as the bitmap's line delta. Established
    for both (the stride computation, the mode record and the bitmap creation read in full).
  - With no remainder (the rounded form), lines are the visible width in bytes apart. `sgibvid.dll` steps every row by the mode
    width, 640 at 640 by 480 and 1600 at the flat panel's 1600 by 1024. The two instructions that index a row in memory are
    the pixel read at `sgibvid.dll@0x80010d78` and the pixel write at `0x80010e09`; the fills, the text drawing, the
    scroll-region move and the screen save and restore all go through them. The move also computes a start and a per-row step
    from the same variable (`0x80011349`, `0x80011385`) but never dereferences them. All four multiply or add the one variable that
    the mode width is stored in (`0x80011fdc`, set at `0x80010707`; its six reads are those four, the allocation size and
    the centring offsets). Only the allocation is rounded up to whole 512-byte
    columns (1024 bytes a row at 640, 2048 at 1600), at `0x800107c9` to `0x800107da`. Linux lays its lines out at `xres_virtual * bytes per pixel`
    (`get_line_length`) and writes a register of whole columns, rounded up from the selected timing's width, with remainder 0.
    Established for both.
  - The two layouts are the same whenever the visible width in bytes is a multiple of 512, so only `sgibvid.dll` (at 640 and
    1600) and Linux (at 640, 800 and 1280 pixels at 8 bits) tell them apart. Every writer's frame fits the rule "remainder set: lines
    are whole columns apart; remainder clear: lines are the visible width apart": a single
    pitch of whole columns for every register value would turn `sgibvid.dll`'s and Linux's frames into noise, and a pitch
    read from the exact register as bytes (`0x24` is 640 bytes) would do the same to the firmware's and `sglfb`'s.
    One firmware code path does not fit. The region-copy entry of the console table (`1.1005@0x1140492`, listed in the
    second data module at `0x11ddf8c`) indexes the source rows by the stride but, in the branch whose row loop counts
    down from the last row and whose horizontal shift is not positive, the destination rows by the visible width (`1.1005@0x1140542` to `0x1140577`: width times row, against
    stride times row in the other three paths). At 640 pixels that copy would shear against every other firmware path, so it
    reads as a firmware defect, not as the hardware pitch (inferred; whether the console ever calls it in that branch was
    not traced).
    Inferred: it assumes the shipped Windows 2000 boot video and the Linux console displayed correctly on the machine, and no
    document or register states the rule. A 2005 posting on the public mailing lists reports Linux X11 working at 15 bits on
    the 1600 SW panel, where the two layouts differ (3200 bytes against 3584), but does not say which mode it meant. Not
    settled: the mechanism, and where the hardware takes the visible width from on the no-remainder branch (the only
    per-mode width in the block is the `vt_hpixen` window, which equals the mode width for every writer above). Linux also
    differs from that window when the requested width is below the selected timing's: its line is the requested width, its
    window the timing's. No shipped software depends on that case.
- *The visible rectangle* is not in the tile registers. Every writer programs the internal pixel-enable windows: `vt_hpixen` ON
  is the horizontal blank end less 19 (wrapped by the total) and OFF is ON plus the active width less 2 (wrapped), so the active
  width is `(OFF - ON) mod total + 2`; `vt_vpixen` runs from the vertical blank end to the vertical blank start, the active
  height. NT (`sgcomp.sys@0x1bfb2` to `0x1bfec`), Linux and the firmware write the same two formulas. There is no horizontal
  or vertical start-offset register in the block: Linux forces `xoffset` and `yoffset` to 0 and no other writer pans. The frame
  starts at its first byte at the first active pixel. Inferred that the fetch pitch and the shown width are independent (a
  1024-byte fetch under a 640-pixel window shows the first 640 pixels of each line), because the PROM's mode 0 writes 640
  active pixels against a 1024-byte stride.

Per mode, as each writer sets the width register (value with depth bits; columns is the rounded-up count; the exact-register
pitch is whole columns, and the rounded-register pitch is the visible width, as the rule above has it):

| Mode | Depth | Exact register (NT, firmware, `sglfb`) | Rounded register (Linux, `sgibvid`) | Columns | Pitch with the exact register | Pitch with the rounded register | `vt_hpixen` window |
|---|---|---|---|---|---|---|---|
| 640 x 480 | 8 | `0x24` (1 + 4/32) | `0x40` | 2 | 1024 | 640 | 638 |
| 800 x 600 | 8 | `0x29` (1 + 9/32) | `0x40` | 2 | 1024 | 800 | 798 |
| 1024 x 768 | 8 | `0x40` | `0x40` | 2 | 1024 | 1024 | 1022 |
| 1280 x 1024 | 8 | `0x48` (2 + 8/32) | `0x60` | 3 | 1536 | 1280 | 1278 |
| 640 x 480 | 32 | `0x40a0` | `0x40a0` | 5 | 2560 | 2560 | 638 |
| 800 x 600 | 32 | `0x40c4` (6 + 4/32) | `0x40e0` | 7 | 3584 | 3200 | 798 |
| 1024 x 768 | 32 | `0x4100` | `0x4100` | 8 | 4096 | 4096 | 1022 |
| 1280 x 1024 | 32 | `0x4140` | `0x4140` | 10 | 5120 | 5120 | 1278 |

Only the 8 bits per pixel rows are written by the firmware and `sgibvid.dll`; NT and `sglfb` offer 8 and 32; Linux 8, 16 and 32.
The 32-bit exact values use the NT miniport's per-depth arithmetic (128 pixels per column at 32 bits). All four modes are in the
Linux timing table, the firmware table and the NT table with these active sizes. The two pitch columns are the established layouts
of the writers that use each register form (only 8 bits per pixel is written by `sgibvid.dll`; the rest follow the writers'
formulas); that the hardware reads each as the table says is inferred. The other columns are computed from the formulas the writers use. A run of the SGI Linux kernels under an emulator (model behaviour, not hardware
evidence) agrees with the arithmetic: at 640 by 480 the three registers read `0x40`, `0x01e00000` and `0x07800003`. The 2.2.10
kernel's 800 by 600 console showed the same height of 480 in that run, so its console mode and programmed timing disagreed there;
the 2.2.10 source is not held (the 2.6 file was read), so that case is unverified.

**Tile lists.** NT keeps its lists in main memory as arrays of 16-bit entries, one per 64 KB tile, built by copying a
list of dwords from the memory manager shifted right 4. If those dwords are 4 KB page-frame numbers, the entry is the
64 KB tile number, which is the GBE's 16-bit tile pointer. Inferred. How the hardware turns an entry into an address
for a tile that is not contiguous is unknown but follows the GBE's.

## Colour map, gamma, cursor, video capture

| Offset | What | Evidence | Grade |
|---|---|---|---|
| `0x50000` | colour map, 6144 dwords in Linux's declaration (the GBE specification says 4608), one dword per entry, written as `R<<24 \| G<<16 \| B<<8` (both drivers). NT's `asSetColorRegisters` takes a count, a first index and four-byte entries whose first three bytes go to bits `[31:24]`, `[23:16]` and `[15:8]` | L, N | inferred |
| `0x58000` | colour-map FIFO status. Linux waits until it reads non-zero, then writes that many entries. NT before every 32nd entry spins until the low six bits read zero or the value reaches `0x30`. The firmware waits only while the low six bits are non-zero and at most `0x10`, and the Windows 2000 boot video driver needs the low six bits above `0x30`, so a model that returns `0x3f` satisfies all four readers and `0x40` fails the boot video driver. Meaning inferred as a free-entry count | L, N | inferred |
| `0x60000` | gamma map, 256 dwords. Linux writes identity (`i<<24 \| i<<16 \| i<<8`). NT's miniport copies dwords straight into it, from the registry value `GammaData` (`GammaDataCallback`, 0x400 bytes, skipped when `BaseVideo` is set) and from the private video IOCTL `0x23208c` (input and output 0x1008 bytes: direction 0 reads the table back, 1 writes it; size selector 8 picks this table, 10 the next). An earlier version of this page said the miniport was not found writing this range; it is. Windows 2000's boot video driver writes the identity ramp (`i * 0x01010100`, 256 words) here | L, N, W2K boot video | established (NT miniport writes), inferred (what the hardware does with the words) |
| `0x68000` | 10-bit gamma map, 1024 dwords. The same two NT paths write it when the selector is 10 or the registry value is 0x1000 bytes long | L (name), N | established (NT miniport writes), inferred (meaning) |
| `0x70000` to `0x78100` | hardware cursor: position (`crs_pos`), control (`crs_ctl`), 3 colours at `0x70008`, 64-dword glyph at `0x78000`; same offsets as the GBE's cursor. Programmed by the NT display driver, not the miniport (below) | L (names), N (display driver) | established (display driver writes), inferred (meaning, from the GBE layout) |
| `0x80000` to `0x80020` | video capture control words (9 dwords, as in the GBE). Neither driver touches them. The video in and out ports raise Cobalt interrupt pins 16 to 19 | L (names), Linux `cobalt.h` | unknown |

The NT miniport also claims exactly `0xd0000000` to `0xd008001f`, ending at the last video-capture word, which agrees with
the block's extent. Inferred (driver behaviour). The miniport's program database declares the whole block as `gberegs`
(`0x80024` bytes): a 6144-dword colour map, a 256-dword gamma map and a 1024-dword 10-bit gamma map, the cursor position,
control and three colour words with a 64-dword glyph, and nine capture words `vc_0` to `vc_8`, under the Linux names. That fixes
the array sizes (declared by the vendor); what the registers do stays as above. Established (declaration), the rest as before.

## I2C and the flat panel

Flat-panel presence and type are read over I2C through `i2cfp` (`+0x10`): `mli2cFlatPanelStatus` reads one byte from
device `0x71` (read address). The panel ID is bits `[7:5]` of that byte. `5` is the SGI 1600SW digital panel (1600 by
1024), which Linux uses as `FLATPANEL_SGI_1600SW`. Bit 2 of the status byte is power-save: NT's
`mli2cFlatPanelPowerSave` sets it when the panel is to sleep. The routines drive SCL and SDA by write-ahead masks held
in a context record that the callers fill in, which is where the bit positions come from. Established (`sgcomp.sys` 5.2.4,
the bit-bang routines `i2cHWSetSCL`, `i2cHWSetSDA`, `i2cHWGetSCL`, `i2cHWGetSDA` at `0x1cae6` to `0x1cb7a` and their two
context builders `mli2cFlatPanelStatus` and `CoDDC_GetEDID`, read in full): in both I2C registers, `+0x08` for the monitor
and `+0x10` for the flat panel, **bit 0 is SDA and bit 1 is SCL**; writing a bit as 1 pulls its line low and writing 0 lets
it float high, and reading a bit gives 1 when the line is low (inverted sense, open-drain). The same register is both the
write and the read-back location. The DDC reader (`CoDDC_GetEDID`, `0x1d182`) tries the three device write addresses
`0xa2`, `0xa6` and `0xa0` in that order (reading `0x100`, `0x100` and `0x80` bytes), each up to three times, so an EDID of
one or two blocks is accepted; the flat-panel status device is at write address `0x70` and read address `0x71`
(`mli2cFlatPanelSetStatus`, `mli2cFlatPanelStatus`). Reset values stay unknown.

## Mode table

NT's table `cobaltVTimings` (27 rows of `0x30` bytes at `sgcomp.sys` data offset `0xd4a0`) holds one record per
resolution and refresh, in the same layout as Linux's `dbe_timing_info` (Linux's header calls it the O2's video timing
structure). Inferred (driver behaviour) for the contents; the 27 rows cover everything in the datasheet's mode list. Clock is
in kHz, `Ht`/`Vt` are totals, `M N P` are the PLL values before the `-1`.

| Mode | Hz | Clock | Ht x Vt | M N P | Note |
|---|---:|---:|---|---|---|
| 640 x 480 | 59.94 | 25 175 | 800 x 525 | 14 15 0 | neg hsync, neg vsync (flags `0x30`). Linux uses `15 2 3` for the same clock |
| 800 x 600 | 60.3 | 40 000 | 1056 x 628 | 3 1 1 | |
| 800 x 600 | 75 | 49 500 | 1056 x 625 | 11 3 1 | |
| 800 x 600 | 119.9 | 81 000 | 1056 x 640 | 3 1 0 | |
| 1024 x 768 | 50 | 54 163 | 1344 x 806 | 4 1 1 | |
| 1024 x 768 | 60 | 65 000 | 1344 x 806 | 12 5 0 | negative syncs |
| 1024 x 768 | 75 | 78 750 | 1312 x 800 | 35 6 1 | |
| 1024 x 768 | 85 | 94 500 | 1376 x 808 | 7 2 0 | |
| 1024 x 768 | 119.8 | 135 000 | 1392 x 809 | 5 1 0 | |
| 1280 x 1024 | 50 | 89 460 | 1680 x 1065 | 10 3 0 | |
| 1280 x 1024 | 60 | 108 000 | 1688 x 1066 | 4 1 0 | |
| 1280 x 1024 | 75 | 135 000 | 1688 x 1066 | 5 1 0 | |
| 1280 x 1024 | 85 | 157 500 | 1728 x 1072 | 35 6 0 | |
| 1376 x 768 | 59.94 | 162 000 | 1560 x 805 | 11 2 1 | clock column is wrong for this row: the PLL values give 74.25 MHz and the totals at the table's own 59.94 Hz give 75.3 MHz |
| 1600 x 1000 | 75 | 171 000 | 2160 x 1050 | 19 3 0 | |
| 1600 x 1000 | 85 | 193 500 | 2160 x 1050 | 36 5 0 | |
| 1600 x 1024 | 53 | 107 447 | 1900 x 1067 | 4 1 0 | flat panel, magic key (flags `0x3000`) |
| 1600 x 1024 | 60 | 106 913 | 1670 x 1080 | 4 1 0 | flat panel (flags `0x1000`); 1600SW |
| 1600 x 1200 | 50 | 130 500 | 2088 x 1250 | 29 6 0 | |
| 1600 x 1200 | 59.94 | 162 000 | 2160 x 1250 | 6 1 0 | |
| 1600 x 1200 | 75 | 202 500 | 2160 x 1250 | 15 2 0 | |
| 1920 x 1080 | 50 | 133 200 | 2368 x 1125 | 5 1 0 | |
| 1920 x 1080 | 59.94 | 159 840 | 2368 x 1125 | 6 1 0 | |
| 1920 x 1080 | 72 | 216 023 | 2560 x 1172 | 8 1 0 | |
| 1920 x 1200 | 50 | 161 500 | 2584 x 1250 | 6 1 0 | |
| 1920 x 1200 | 59.94 | 193 606 | 2584 x 1250 | 36 5 0 | |
| 1920 x 1200 | 66 | 213 180 | 2584 x 1250 | 8 1 0 | datasheet's maximum |

The datasheet's own list (640 x 480 at 60 up to 1920 x 1200 at 66) is a subset of these rows. NT picks a record by
exact width and height and a refresh rounded to whole hertz (`asMapModeToVTP`). Mode flags in the record: bit 3
(`0x8`) sync on green, bit 4 (`0x10`) invert hsync, bit 5 (`0x20`) invert vsync, `0x1000` flat-panel timing, `0x2000` a
key that keeps the DAC on with a panel attached. The user can also supply timings under a registry key
(`...\Silicon Graphics\Control Panel\Display\Timings`, and a "Video Format Creator" key whose records carry a CRC);
`VTableValidateMode` checks them. Inferred (driver behaviour).
