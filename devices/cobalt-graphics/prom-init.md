# Cobalt display back end: what the system firmware programs

The firmware of the 320 and 540 drives the display back end (Arsenic, `0xd0000000`) itself, for its text console, menus and
boot screens, before any driver runs. This page records what two shipped releases, PROM 1.0006 (July 1999) and PROM 1.1005
(July 2000), write and check, in order. It is the only account here of a driver other than SGI's NT miniport and the Linux
frame buffer that sets the block up from nothing, so the values are a third, independent witness for the registers in
[display-engine.md](display-engine.md). Back to the [overview](README.md).

Grades here are the repository's three: **established** (the whole routine or table read in the shipped image, cited by
address), **inferred** (a reading, with its reasoning), **unknown**. The images are not committed; how they are laid out and
how addresses are cited is in [the firmware page](https://github.com/jamesbraid/chips/blob/main/software/visws/firmware.md):
`1.0006@0xf3ec30` is a virtual address in the main module of 1.0006 (loaded at `0xf00000`), `1.1005@0x1131949` the same
routine in 1.1005 (loaded at `0x1100000`), and `1.0006:test+0x96f7` an offset in the firmware's self-test module.
The routines were read in Ghidra (x86, 32-bit flat) and cross-checked with objdump. Nothing here was run on hardware or in a
model; a register value that the firmware writes is what the firmware writes, not what the chip needs.

## When it runs

`1.0006@0xf3e9b0` (1.1005: `0x1131702`) is the display start-up. It reads the firmware environment variable `console`: a value
starting with `g` or `G` (`1.1005@0x1131525`; the console-selection routine `0x1113ca1` is in [the PROM boot path](https://github.com/jamesbraid/chips/blob/main/software/visws/boot-path.md#console-selection-main-module): a serial console needs a lowercase `d`, an unset or any other value, including `D`, still gets `video()` but with the display frozen; the empty-flash default is `g`) selects the graphics console and runs the full sequence below; anything else only stops the display
(below) and writes `+0x10000 = 0x80000000`, which freezes the timing counter (the `FREEZE` bit of `vt_xy`).
Every path first writes `ctrlstat` (`+0x000`) = `0x20000000`.

## Register writes, in order (graphics console)

Offsets are from `0xd0000000`; all accesses are 32-bit. 1.0006 and 1.1005 write the same values in the same order (the 1.1005 timing routine makes the same 29 stores in
the same order, but its generated code differs by more than operand reordering). The console start-up also calls `0xf3e770`,
which computes the frame-buffer stride, and `0xf3e7f0`, which is a bare `ret`. The mode record `R` is the one chosen by the probe in step 2 ([layout below](#the-mode-records)).

| Step | Routine (1.0006) | Writes | What |
|---|---|---|---|
| 1 | `0xf3e700` | `+0x000 = 0x20000000` | The value Linux writes before it programs the block. |
| 2 | `0xf3e8d0` (stop) | if `(+0x000 & 0x30000000) == 0x30000000` and `vt_xy` is not frozen: clear bit 0 of `+0x20008`, wait 1 ms, clear bit 0 of `+0x3000c`, wait 1 ms, clear bit 16 of `+0x40004`, wait 1 ms; then poll up to 10,000 times 10 microseconds for `+0x30008` bit 0, `+0x20004` bit 0 and `+0x40000` bit 16 to read **1** | Stops overlay, normal-plane and DID fetch. The poll waits for the live bits to read set, the opposite of NT's wait for them to clear ([display-engine.md](display-engine.md)); as written it would simply time out after 100 ms. Established as code; which sense the hardware has is unknown. |
| 3 | `0xf3ea10` | I2C read of one byte from device `0x71` through `+0x10` (the flat-panel status device), two tries | See [panel probe](#the-panel-probe). |
| 4 | `0xf3e860` | after 10 microseconds, `+0x60000` to `+0x603fc`: 256 words `i * 0x01010100` | Identity gamma map, the same ramp as Linux and the Windows 2000 boot video driver. |
| 5 | `0xf3e8a0` | if `vt_xy` has `FREEZE` set: `+0x10000 = 0`, wait 1 microsecond; otherwise stop as in step 2 | Unfreezes the timing counter. |
| 6 | `0xf3e800` | `+0x018 = 0xa7` | `config` = Linux's frame-buffer value (`0x80 | 0x20 | 0x04 | 0x02 | 0x01`). Never changed afterwards except for a panel (step 9). |
| 7 | `0xf3eac0` | `+0x48000` to `+0x4807c` (all 32 `mode_regs`) = 3 if the console flag in the main module is set; zero fill of the frame buffer | The frame buffer is 8 bits per pixel, linear, with the row stride rounded up to whole 512-byte tile widths: `stride = ceil(W / 512) * 512` (computed at `1.1005@0x113154e`; the console's fill, text and zero-fill routines all step rows by it, e.g. `1.1005@0x1140296`), at physical `0xa00000` (1.0006) or `0x2000000` (1.1005), 2 MiB mapped uncached. The value 3 (both buffers) is what the Windows 2000 boot video driver writes; the NT miniport does not write the table. The 2 MiB mapping is uncached (inferred from the page-table attribute the routine sets). |
| 8 | `0xf3ec30` (timing) | see below | Mode set. |
| 9 | `0xf3e810` | for entries 0 and 1 of the colour map: wait while `(+0x58000 & 0x3f)` is neither 0 nor above `0x10` (up to 1,000 times 10 microseconds), then `+0x50000 = 0`, `+0x50004 = 0` | Only two colour-map words are touched; the text console's colours are written elsewhere. |
| 10 | `0xf3ebb0` | `+0x70008 = 0xffffff00`, `+0x7000c = 0xff000000`, `+0x70000 = 0`; `+0x78000` to `+0x7807c` copied from a 128-byte table in the second data module, `+0x78080` to `+0x780fc` zero, `+0x70004 = 0` | Cursor colours and glyph; the position/control word `+0x70000` is written 0 (off, as Linux does), so the cursor is never enabled. |

The timing step (`0xf3ec30`) in the order the writes happen, with `R` the mode record and `Ht, hbs, hbe, hss, hse, Vt, vbs, vbe, vss, vse`
its totals, blanking and sync fields:

| Order | Register | Value |
|---|---|---|
| 1 | `+0x004` `dotclock` | `old & 0xffff` (clears `RUN` and everything above bit 15) |
| 2 | `+0x10004` `vt_xymax` | 0 |
| 3 | `+0x10008` `vt_vsync` | `vss << 12 \| vse` |
| 4 | `+0x1000c` `vt_hsync` | `hss << 12 \| hse` |
| 5 | `+0x10010` `vt_vblank` | `vbs << 12 \| vbe` |
| 6 | `+0x10014` `vt_hblank` | `((hbs - 5 + Ht) mod Ht) << 12 \| (hbe - 3)` |
| 7 | `+0x1003c` `vt_hcmap` | `hbs << 12 \| hbe` |
| 8 | `+0x10040` `vt_vcmap` | `vbs << 12 \| vbe` |
| 9 | `+0x10030` `fp_de`, `+0x10028` `fp_hdrv`, `+0x1002c` `fp_vdrv` | see [flat panel](#flat-panel-registers) |
| 10 | `+0x10044` `did_start_xy` | `Y << 12 \| X`, `Y = vbs - vbe - 1` (negative for any real mode, written as 12-bit two's complement; the routine negates it if it is positive); `X = hbe - 20`, plus `Ht` if negative |
| 11 | `+0x10048` `crs_start_xy` | the same `Y` plus one; `X = hbe - 54`, plus `Ht` if negative |
| 12 | `+0x1004c` `vc_start_xy` | the same `Y`; `X = hbe - 4` (no wrap) |
| 13 | `+0x30000` `frm_size_tile` | whole tile columns in `[12:5]`: `(columns - 1) << 5 \| (remainder / 32)` for a width that is not a multiple of 512, `columns << 5` for one that is |
| 14 | `+0x30004` `frm_size_pixel` | `H << 16` |
| 15 | `+0x004` `dotclock` | `((N - 1) & 0x3f) << 8 \| ((P & 3) \| 0x40) << 14 \| ((M - 1) & 0xff)`: `RUN` (bit 20) is set in the same write |
| 16 | wait 11 ms | the same wait Linux uses |
| 17 | `+0x10038`, `+0x10034` | both `0xffffff` (pixel-enable windows off) |
| 18 | `+0x10004` `vt_xymax` | `Vt << 12 \| Ht` |
| 19 | `+0x30000` | the step-13 value `\| 0x8000`, then the plain value (`FIFO_RESET` pulse) |
| 20 | `+0x20000` `ovr_width_tile` | `0x2000`, then 0 (overlay width zero; the `FIFO_RESET` pulse) |
| 21 | `+0x3000c` `frm_control` | frame-buffer physical address `\| 3` (linear and enable) |
| 22 | `+0x40004` `did_control` | 0 |
| 23 | poll `+0x30008` bit 0 | until it reads 1, one microsecond between reads, no time-out |
| 24 | `+0x10034` `vt_hpixen` | `((W - 2 + ON) mod Ht) \| ON << 12`, `ON = hbe - 19` wrapped by `Ht` |
| 25 | `+0x10038` `vt_vpixen` | `vbe << 12 \| vbs` |
| 26 | `+0x000` `ctrlstat` | with a flat panel ([below](#the-panel-probe)): `+0x018 \|= 8`, `+0x000 \|= 0x04000000`, and no `0x30000000`. Without one: if bit 25 reads set, write `0x30000000` (`PCLKSEL` 3, the internal dot clock), as Linux does. |

Not written by the firmware: `+0x00c` (system clock PLL), `+0x008` and `+0x010` (it only reads the panel I2C), `+0x10018` `vt_flags`
(so the sync polarities in the mode record's flags are not applied), `+0x1001c`, `+0x10020`-`+0x10024` at this point (they are written
by the hand-over below), the capture block `+0x80000` and up. Established (no store to them in the routines read).
All of this is established: both routines read in full in both images.

A later routine (`1.0006@0xf081c0`, 1.1005 `0x1108040`; it also masks the thirty Cobalt interrupt entries, resets the floppy
controller and fills the legacy BIOS data area) writes `+0x10020` and `+0x10024` as `0xffffff` (scanline interrupts parked, as NT's stop does)
and writes the complement of the word at `0xca000000` back to it, the same acknowledge NT's retrace handler uses. Established.

## The mode records

A record is `0x30` bytes. Fields (little endian): `+0x00` u32 index, `+0x04` u32 flags (`0x02` unknown, `0x10`, `0x1000` flat-panel
timing, `0x3000` flat-panel with the "magic key" of [display-engine.md](display-engine.md)), `+0x08` u16 width, `+0x0a` u16 height, `+0x0c`
u32 refresh in thousandths of a hertz, `+0x10` u32 pixel clock in kHz, then thirteen u16 values from `+0x14`: `Ht`, `hbs`, `hbe`, `hss`,
`hse`, `Vt`, `vbs`, `vbe`, `vss`, `vse`, `M`, `N`, `P`. This is the layout of Linux's timing record with a leading index word and a 32-bit
refresh field. The firmware ignores `flags` and the refresh and clock fields (it only uses the geometry, `M`, `N` and `P`).

1.1005's table (`1.1005@0x11d1ea0`, 26 rows) agrees with the NT 5.2.4 miniport's table in [display-engine.md](display-engine.md) on clock,
totals and `M N P` in 24 of its 27 rows. The differences: the miniport's 1376 by 768 row is not in the firmware, and the 1024 by 768
row at 119.8 Hz is clock 135000, 1392 x 809 in the miniport and clock 133195, 1376 x 808 in the firmware (both 1.0006 and 1.1005);
established (compared by value). The 1.0006 table (`1.0006@0xfe36a0`, 24 rows) is an older edition: nine of the rows above differ from it. Rows, with what 1.0006 had where it differs:

| Index | Mode | Hz | Clock kHz | Flags | Ht x Vt | M N P | 1.0006 |
|---|---|---|---|---|---|---|---|
| 0 | 640 x 480 | 59.940 | 25175 | 0x10 | 800 x 525 | 14 15 0 | flags 0, `M N P` 15 2 3 |
| 1 | 800 x 600 | 60.317 | 40000 | 0x0 | 1056 x 628 | 3 1 1 | |
| 2 | 800 x 600 | 75.000 | 49500 | 0x0 | 1056 x 625 | 11 3 1 | |
| 3 | 800 x 600 | 119.851 | 81000 | 0x2 | 1056 x 640 | 3 1 0 | 119.800 Hz, clock 82978, 1040 x 666, `M N P` 31 5 1, other timings differ |
| 4 | 1024 x 768 | 50.000 | 54163 | 0x0 | 1344 x 806 | 4 1 1 | |
| 5 | 1024 x 768 | 60.004 | 65000 | 0x10 | 1344 x 806 | 12 5 0 | flags 0 |
| 6 | 1024 x 768 | 75.029 | 78750 | 0x0 | 1312 x 800 | 35 6 1 | `M N` 29 5 |
| 7 | 1024 x 768 | 84.997 | 94500 | 0x0 | 1376 x 808 | 7 2 0 | |
| 8 | 1024 x 768 | 119.800 | 133195 | 0x2 | 1376 x 808 | 5 1 0 | |
| 9 | 1280 x 1024 | 50.000 | 89460 | 0x0 | 1680 x 1065 | 10 3 0 | vsync 1027-1030 |
| 10 | 1280 x 1024 | 60.020 | 108000 | 0x0 | 1688 x 1066 | 4 1 0 | vsync 1025-1028 |
| 11 | 1280 x 1024 | 75.025 | 135000 | 0x0 | 1688 x 1066 | 5 1 0 | |
| 12 | 1280 x 1024 | 85.024 | 157500 | 0x0 | 1728 x 1072 | 35 6 0 | `M N` 29 5 |
| 13 | 1600 x 1000 | 75.397 | 171000 | 0x0 | 2160 x 1050 | 19 3 0 | not present |
| 14 | 1600 x 1000 | 85.317 | 193500 | 0x0 | 2160 x 1050 | 36 5 0 | not present |
| 15 | 1600 x 1024 | 53.000 | 107447 | 0x3000 | 1900 x 1067 | 4 1 0 | |
| 16 | 1600 x 1024 | 60.000 | 106913 | 0x1000 | 1670 x 1080 | 4 1 0 | 1670 x 1067 |
| 17 | 1600 x 1200 | 50.000 | 130500 | 0x0 | 2088 x 1250 | 29 6 0 | `M N` 24 5 |
| 18 | 1600 x 1200 | 59.940 | 162000 | 0x0 | 2160 x 1250 | 6 1 0 | |
| 19 | 1600 x 1200 | 75.000 | 202500 | 0x0 | 2160 x 1250 | 15 2 0 | |
| 20 | 1920 x 1080 | 50.000 | 133200 | 0x0 | 2368 x 1125 | 5 1 0 | |
| 21 | 1920 x 1080 | 59.940 | 159840 | 0x0 | 2368 x 1125 | 6 1 0 | |
| 22 | 1920 x 1080 | 72.000 | 216023 | 0x0 | 2560 x 1172 | 8 1 0 | |
| 23 | 1920 x 1200 | 50.000 | 161500 | 0x0 | 2584 x 1250 | 6 1 0 | |
| 24 | 1920 x 1200 | 59.940 | 193606 | 0x0 | 2584 x 1250 | 36 5 0 | clock 193800, `M N` 29 4 |
| 25 | 1920 x 1200 | 66.000 | 213180 | 0x0 | 2584 x 1250 | 8 1 0 | |

(In 1.0006 the indices 13 to 23 are one lower from the 1600 x 1000 insertion on; rows are matched here by mode.)

## The panel probe

`0xf3ea10` (1.1005 `0x113175c`) reads the status byte of the flat-panel I2C device at `0x71` (`1.0006@0xf4e3d0`, the same bit-banged
`+0x10` protocol as NT's `mli2cFlatPanelStatus`), twice if the first try gets no answer. The mode record is chosen from bits `[7:5]` of
the answer:

| Panel id | Record | "Flat panel" flag (`R.panel`) |
|---|---|---|
| 3 | 1024 x 768, 60 Hz (index 5) | 1 |
| 4 | 1280 x 1024, 60 Hz (index 10) | 1 |
| 5 (the 1600SW) | 1600 x 1024, 60 Hz, flags `0x1000` (1.0006 index 14, 1.1005 index 16) | 1; the status byte is also written back with bit 2 set (`0xf4e4f0`; NT calls this bit power-save, so the sense is unknown) |
| anything else, or no answer | 1024 x 768, 75 Hz (index 6) | 0 |

Established. That id 5 is the 1600SW agrees with [display-engine.md](display-engine.md). The timing routine also has branches for ids `0x82` and `0x83` (next section); the id is stored only by this probe, so values above 7 cannot occur and
those branches are dead in both images.

## Flat panel registers

With the panel flag set, the timing step writes the three panel registers; without it they get whatever the register `ecx` holds on entry
(the routine takes its argument in `ecx` and the callers read were not traced: unknown).

| Panel id | `+0x10030` `fp_de` | `+0x10028` `fp_hdrv` | `+0x1002c` `fp_vdrv` |
|---|---|---|---|
| 5 | `0x000640` | `0x640` | `0x1401` (`(0x400 + 1) \| 0x1000`) |
| `0x82` (dead) | `0x303 << 12 \| 0x326` | `0x400` | `0x1301` |
| `0x83` (dead) | `0x400 << 12 \| 0x429` | `0x500` | `0x1401` |
| any other with the flag | `0xfff << 12 \| 0xfff` | `0xfff` | `0x1000` |

Established (`1.0006@0xf3ec30`, `1.1005@0x1131949`). For a 1600SW this leaves `fp_hdrv` low 12 bits `0x640` and `fp_vdrv` low 12 bits `0x401`,
which is the state the Windows 2000 boot video and frame buffer drivers test to recognise a panel the firmware set up
([display-engine.md](display-engine.md#overlay-normal-plane-did)): **their inference that the firmware leaves those two registers set up is now established.**

## The ID check and the register self-test

The firmware's self-test module checks the chip before the display is used. Table `1.0006:test+0x96f7` (1.1005 `test+0x9775`) holds triples
(address, mask, expected) that fail the test when `(*address & mask) != expected`:

| Address | Mask | Expected | In |
|---|---|---|---|
| `0xd0000014` | `0xffffffff` | `0x666` | both (the ID NT and the Windows 2000 drivers require) |
| `0xd000001c` | `0x0ff0ffff` | 0 | 1.0006 only |

`+0x1c` is the `bist` register that neither driver reads. The test code at `test+0x966a` (1.1005 `test+0x96e4`) writes 0 to it, spins
about 67 million loop iterations, polls up to 134 million reads for bit 31 to become 1, masks the value with `0x0ff0ffff` and
reports a failure when bits `[27:24]` are non-zero; if bit 31 never sets it reports the failure code `0x7777777`. So writing
`+0x1c` starts a built-in self-test and bit 31 reports completion (inferred; the failure bits' layout is unknown).

A second table, forty (address, mask) pairs starting at `test+0x9713` (1.1005 `test+0x9785`), lists the registers the self-test walks bit by bit
(it sets the masked bits, reads, clears them, reads, then tries `0x55555555` and `0xaaaaaaaa` through the mask). The masks are the bits
that read back as written, so they are a readable statement of which bits are implemented (inferred: a bit outside the mask might be read-only, not
implemented, or merely not tested). Identical in both versions:

| Register | Mask | Note |
|---|---|---|
| `+0x018` `config` | `0xef` | bit 4 is not tested |
| `+0x10004` to `+0x1004c`, every timing word except `+0x10018` | `0x00ffffff` | |
| `+0x10018` `vt_flags` | `0x7f` | seven writable bits |
| `+0x20000` `ovr_width_tile` | `0x3fff` | |
| `+0x20004`, `+0x20008` | `0xffffffe3` | bits 2 to 4 not tested |
| `+0x30000` `frm_size_tile` | `0xffff` | |
| `+0x30004` `frm_size_pixel` | `0xffff0000` | height only |
| `+0x30008`, `+0x3000c` | `0xfffffe03` | pointer `[31:9]` and bits 1 and 0 |
| `+0x40000`, `+0x40004` | `0xffffffff` | |
| `+0x70000` | `0xffffffff` | cursor position and control |
| `+0x70004` | `0x3` | |
| `+0x70008`, `+0x7000c` | `0xffffff00` | cursor colours, 24 bits in `[31:8]` |
| `+0x80000`, `+0x80004` | `0x00ffffff` | video capture words |
| `+0x80008` | `0x1f` | |
| `+0x80010`, `+0x80014`, `+0x80018`, `+0x80020` | `0xffffffff` | |

The live copies `+0x20004` and `+0x30008` appear in the walk, so the walk writes them; whether they hold the written bits is not known.
These are the first values for the capture block's width: neither driver touches it ([display-engine.md](display-engine.md#colour-map-gamma-cursor-video-capture)).

## What changed between the two releases

- Frame buffer at `0xa00000` becomes `0x2000000`.
- The mode table gains two rows, replaces one and changes nine, converging on the NT miniport's table (above).
- The register sequence, the panel probe and the flat-panel values are unchanged.
- The self-test's ID check is unchanged; its triple for the `bist` register is dropped (the test code remains).
- The text console's other uses of the block (cursor, gamma, colour map) are unchanged.

Unknown: the sense of the stop-time live-bit wait; the reset values of every register (before the console start-up the firmware reads only the low nibble of `+0x000`, the
chip revision that a real machine's registry shows as "Display Engine: 002", the ID at `+0x14` and the `bist` word, and writes none of them first); the panel-id values other than 3, 4 and 5 and what a 1600 x 1024 flat panel's bit 2 does; the meaning of record flag `0x02`;
the failure bit layout of `+0x1c`.
