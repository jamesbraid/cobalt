<!-- generated from devices/arsenic/registers.toml by defs-gen - do not edit, change the source -->
<!-- SPDX-License-Identifier: BSD-3-Clause -->
<!-- defs-gen 4 -->
<!-- input sha256 3411604497bffbc7c3d72c4372b429ff18bf72d9de9013f9bf6bc1fc3a0cb6e8 -->

# Arsenic display back end register block

The display back end of the graphics subsystem: the register block the NT 4.0 miniport and display driver, the Windows 2000 display drivers, the firmware and the Linux frame buffer driver program. Offsets are from the start of the block, which the display miniport maps; all accesses are 32 bits wide and uncached, with a few byte loads of live words noted per register. The retrace latch is a separate 32-bit window outside the block and sits in its own address space. Access records what the drivers do to the register. Reset values are unknown for every register: no shipped software reads one before writing it, and both drivers only ever see a post-firmware state. A register's grade is the grade of its hardware meaning; what the drivers write is recorded in its doc, and where a doc says the driver writes are established, that holds whatever the grade says. Register names are the ones the public Linux header uses, which the miniport symbol tables also carry; field names are the Linux header names where it defines one and our own plain description otherwise.

Unit `arsenic`, macro prefix `ARS`, register block `ARS`.
Default source: SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions).

Every item has a grade: established, inferred or unknown. A grade says how the item was derived from its source; it does not claim the item was measured on silicon.

## Address spaces

| Space | Offset unit | Description |
|---|---|---|
| retrace | byte | The 32-bit retrace latch window, a mapping separate from the main block; its offset 0 is the window base. |

## Registers

| Register | Offset | Access | Width | Count | Space | Reset | Grade | Source |
|---|---|---|---|---|---|---|---|---|
| `ctrlstat` | 0x0 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `dotclock` | 0x4 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `i2c` | 0x8 | rw | 32 |  |  | unknown | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the EDID reader |
| `sysclk` | 0xc | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only) |
| `i2cfp` | 0x10 | rw | 32 |  |  | unknown | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the flat-panel status routines |
| `id` | 0x14 | r | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, adapter attach check; the firmware self-test |
| `config` | 0x18 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `bist` | 0x1c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code |
| `vt_xy` | 0x10000 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, scan-line and mode-set routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vt_xymax` | 0x10004 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_vsync` | 0x10008 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_hsync` | 0x1000c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_vblank` | 0x10010 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_hblank` | 0x10014 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vt_flags` | 0x10018 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines |
| `vt_f2rf_lock` | 0x1001c | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set and retrace routines |
| `vt_intr01` | 0x10020 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vt_intr23` | 0x10024 | rw | 32 |  |  | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines |
| `fp_hdrv` | 0x10028 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `fp_vdrv` | 0x1002c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `fp_de` | 0x10030 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vt_hpixen` | 0x10034 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_vpixen` | 0x10038 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_hcmap` | 0x1003c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `vt_vcmap` | 0x10040 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine |
| `did_start_xy` | 0x10044 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `crs_start_xy` | 0x10048 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vc_start_xy` | 0x1004c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `ovr_width_tile` | 0x20000 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `ovr_inhwctrl` | 0x20004 | r | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, overlay stop routine |
| `ovr_control` | 0x20008 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, display driver overlay routines |
| `frm_size_tile` | 0x30000 | rw | 32 |  |  | unknown | established | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `frm_size_pixel` | 0x30004 | rw | 32 |  |  | unknown | established | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `frm_inhwctrl` | 0x30008 | r | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, page-flip and stop routines |
| `frm_control` | 0x3000c | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, start, flip and stop routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `did_inhwctrl` | 0x40000 | r | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, id fetch stop routine |
| `did_control` | 0x40004 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, id fetch routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `mode_regs` | 0x48000 | w | 32 | 32 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Visual Workstation PROM 1.1005, display start-up code; Windows 2000 boot video and frame buffer drivers |
| `cmap` | 0x50000 | w | 32 | 6144 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `cm_fifo` | 0x58000 | r | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code |
| `gmap` | 0x60000 | rw | 32 | 256 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, gamma routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `gmap10` | 0x68000 | rw | 32 | 1024 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, gamma routines |
| `crs_pos` | 0x70000 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `crs_ctl` | 0x70004 | rw | 32 |  |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `crs_cmap` | 0x70008 | rw | 32 | 3 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `crs_glyph` | 0x78000 | rw | 32 | 64 |  | unknown | inferred | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |
| `vc_0` | 0x80000 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_1` | 0x80004 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_2` | 0x80008 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_3` | 0x8000c | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_4` | 0x80010 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_5` | 0x80014 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_6` | 0x80018 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_7` | 0x8001c | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `vc_8` | 0x80020 | rw | 32 |  |  | unknown | unknown | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code (self-test masks) |
| `retrace_status` | 0x0 | rw | 32 |  | retrace | unknown | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, retrace interrupt routine; SGI Visual Workstation PROM 1.1005, display start-up code |

### Register `ctrlstat`

General control and status. The mode-set routine of the NT miniport writes the value with the clock selector at 3 as its last step; Linux writes the selector at 2 before programming and 3 afterwards when bit 25 reads set; the firmware does the same, and with a flat panel ORs in bit 26 instead of writing the selector 3. The firmware reads the low nibble as the chip revision (a real 320 shows display engine 002). Linux treats a clock selector below 2 as not scanning out from the internal dot clock.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 29:28 | `pclksel` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | dot clock source: 3 selects the internal dot clock in every writer |
| 26 | `bit26` | unknown |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | set by the firmware and the NT 4.0 HAL when a flat panel answered; meaning unknown |
| 25 | `bit25` | unknown |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | meaning unknown; the firmware writes the selector 3 only when this reads set, so a model should read it as 1 |
| 4 | `sense_n` | unknown |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | named in the Linux header; no shipped driver reads or writes it |
| 3:0 | `chipid` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | chip revision as the firmware reads it; a real 320 reports 2 (see the real-machine revision constant). Reset value unknown |

### Register `dotclock`

Dot clock PLL. Software writes M-1 and N-1. The output is 27 MHz times M over N, divided by 2 to the P; that reproduces the clock column of 26 of the 27 rows of the NT mode table to within 1.4 percent. No lock bit is read and every wait is a plain delay. The NT miniport clears everything above bit 15, restarts the PLL through a fixed sequence with RUN set and cleared and 40 ms, one second and 100 ms delays, then writes the final value without RUN, waits 40 ms and sets RUN; Linux and the firmware write the final value with RUN and wait 11 ms; the NT 4.0 HAL clears the bits above 15 and writes the value with RUN with no wait.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 20 | `run` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | PLL run |
| 15:14 | `p` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | output divider as a power of two |
| 13:8 | `n` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | reference divider minus 1 |
| 7:0 | `m` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | feedback multiplier minus 1 |

### Register `i2c`

CRT I2C lines (DDC). One register is both the write and the read-back location. Writing a bit as 1 pulls the line low and writing 0 lets it float high; a read returns 1 when the line is low. With no device both lines read 0. The EDID reader tries device write addresses 0xa2, 0xa6 and 0xa0 in that order.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 1 | `scl` | established |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the EDID reader | clock line |
| 0 | `sda` | established |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the EDID reader | data line |

### Register `sysclk`

System clock PLL. Neither driver, the firmware nor the HAL touches it; the Linux header names it as the system clock PLL control. Nothing shipped depends on its contents.

### Register `i2cfp`

Flat-panel I2C lines, the same layout and sense as the CRT register. The flat-panel status device answers at write address 0x70 and read address 0x71; bits 7:5 of its status byte are the panel id (5 is the 1600 by 1024 digital panel) and bit 2 is power-save. The firmware reads it twice when the first try gets no answer, and when the CRT EDID reader finds a flat panel it uses this register instead of the CRT one.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 1 | `scl` | established |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the flat-panel status routines | clock line |
| 0 | `sda` | established |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, bit-bang routines and the flat-panel status routines | data line |

### Register `id`

Device id. The NT miniport compares the whole dword with the value in the ID constant and does not attach otherwise; the Windows 2000 drivers compare the low 16 bits; the firmware self-test compares the whole dword. A read of that value passes every reader (established as the check; what the value means is inferred).

### Register `config`

Output configuration. The NT miniport writes bits 7, 6, 5, 2 and 0, sets bit 0 when the mode asks for sync on green (the sync-on-green bit), always sets bit 3, and clears bit 0 when a flat panel answered unless the mode carries the key flag. Linux defines a frame-buffer value that sets bits 7, 5, 2, 1 and 0 but never writes the register; the firmware writes that value; the NT 4.0 HAL writes the same and ORs in bit 3. The Windows 2000 frame buffer driver writes bits 7, 5, 2 and 0, with bit 3 added for a flat panel, and its boot video driver one further value. Nothing shipped writes bit 6 or bit 1 on its own. A model should accept any value. The firmware self-test walks bits 7:5 and 3:0 (bit 4 is not tested).

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 7 | `ext_addr` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | extended addressing |
| 6 | `tilehist` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | tile history |
| 5 | `lendian` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | little-endian pixel memory; not settled |
| 3 | `fpenable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | flat panel enable |
| 2 | `vdac_pblank` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | pixel blank |
| 1 | `vdac_gsync` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | sync on green |
| 0 | `vdac_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | video DAC enable |

### Register `bist`

Built-in self-test. Neither driver reads it. The firmware self-test writes 0, polls bit 31 for completion and reports a failure when any of bits 27:24 is set. Release 1.0006 also compares the register with a mask of 0x0ff0ffff for zero. The layout of the failure bits is unknown.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `done` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code | self-test complete |
| 27:24 | `fail` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver (name only); SGI Visual Workstation PROM 1.1005, display start-up code | failure bits; layout unknown |

### Register `vt_xy`

Current dot coordinates. The live line is read from bits 23:12 (the scan-line poll); bit 31 is the timing counter freeze: it reads back what software set by writing the top bit and clears when software writes 0. Software tests the freeze bit with a byte load of the top byte, and the NT miniport, the Windows 2000 frame buffer driver, the NT 4.0 HAL and the firmware all do. The firmware's non-graphics console path writes the freeze bit alone.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31 | `vt_freeze` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, scan-line and mode-set routines; SGI Visual Workstation PROM 1.1005, display start-up code | timing counter frozen |
| 23:12 | `y` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, scan-line and mode-set routines; SGI Visual Workstation PROM 1.1005, display start-up code | live line |
| 11:0 | `x` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, scan-line and mode-set routines; SGI Visual Workstation PROM 1.1005, display start-up code | live horizontal position |

### Register `vt_xymax`

Totals: horizontal total in 11:0 and vertical total in 23:12. Software writes 0 while it programs the other windows and the real totals afterwards.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_maxy` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | vertical total |
| 11:0 | `vt_maxx` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | horizontal total |

### Register `vt_vsync`

Vertical sync window, in lines.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_vsync_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_vsync_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_hsync`

Horizontal sync window, in dots.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_hsync_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_hsync_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_vblank`

Vertical blank window, in lines.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_vblank_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_vblank_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_hblank`

Horizontal blank window. NT and the firmware write ON as the blank start less 5 (wrapped by the total) and OFF as the blank end less 3; Linux writes ON as the blank start.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_hblank_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start of the window (ON) |
| 11:0 | `vt_hblank_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | end of the window (OFF) |

### Register `vt_flags`

Polarity and blanking flags of the timing signals. The mode set writes bit 2 for the horizontal flag of the mode record and bit 0 for the vertical one, after writing 0x40 and then 0. The power-state routine edits bits 1 and 3 for the standby, suspend and off states (bit 1 for standby, bit 3 for suspend, both for the rest); which signal each gates is inferred from the VESA states, bit 1 horizontal and bit 3 vertical. The retrace handler sets bit 6 in stereo modes and clears it again when free-running. The firmware self-test walks bits 6:0. The firmware does not write the register.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 6 | `bit6` | unknown |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines | written 0x40 then 0 at mode set; set by the stereo retrace handler; meaning unknown |
| 3 | `vsync_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines | power state: stop vertical sync; which signal is inferred |
| 2 | `hdrv_invert` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines | invert horizontal sync drive |
| 1 | `hsync_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines | power state: stop horizontal sync; which signal is inferred |
| 0 | `vdrv_invert` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set, power-state and retrace routines | invert vertical sync drive |

### Register `vt_f2rf_lock`

Frame-to-field and frame-lock line. The NT miniport writes the blank-start line in 23:12 and 0xfff in 11:0 for a mono mode, and the blank-start line in both halves for a stereo mode. Linux does not write it. The retrace handler reads it in stereo and discards the value; the read may have a side effect. Which half is which is inferred from the order of the names in the Linux header comment.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `f2rf_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set and retrace routines | frame-to-field line |
| 11:0 | `framelock_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set and retrace routines | frame-lock line |

### Register `vt_intr01`

Scan-line interrupt lines 1 (11:0) and 0 (23:12). The NT miniport parks both at 0xfff, then `CoSetMode` arms line 0 at the screen height less an allowance less one, and line 1 at the height plus one (plus two with the flat-panel timing flag of the mode record). The allowance is (field rate / 1000) * vertical total * 300 / 1,000,000, in integers: 9 lines for the built-in 640 by 480 mode at 59.94 Hz, so line 0 is 470 and line 1 is 481 (established). The firmware's hand-over routine parks both and writes 0x1fff. The only shipped consumer is the miniport's retrace handler, which sees the interrupts through retrace_status; no routine reads these lines back except the writer's own read-modify-write. That scan-line interrupt 0 and 1 are what set bits 0 and 1 of retrace_status is inferred from the order and position of the two lines.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `intr0_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines; SGI Visual Workstation PROM 1.1005, display start-up code | line of scan-line interrupt 0 |
| 11:0 | `intr1_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines; SGI Visual Workstation PROM 1.1005, display start-up code | line of scan-line interrupt 1 |

### Register `vt_intr23`

The same for scan-line interrupts 3 (11:0) and 2 (23:12). The NT miniport accepts an interrupt number below 4 and a line below 4096.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `intr2_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines | line of scan-line interrupt 2 |
| 11:0 | `intr3_y` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, interrupt routines | line of scan-line interrupt 3 |

### Register `fp_hdrv`

Flat-panel horizontal drive window. With a panel the NT miniport sets OFF to the width; the firmware writes the panel's value for panel id 5 and leaves the low 12 bits at the width.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `fp_hdrv_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start of the window (ON) |
| 11:0 | `fp_hdrv_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | end of the window (OFF) |

### Register `fp_vdrv`

Flat-panel vertical drive window. ON is 1 and OFF is the row count plus one, so a 1024-row panel leaves 0x401 in the low 12 bits, which the Windows 2000 drivers read back to recognise a panel. Without a panel OFF is 0.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `fp_vdrv_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start of the window (ON) |
| 11:0 | `fp_vdrv_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | end of the window (OFF) |

### Register `fp_de`

Flat-panel data enable window. The NT miniport writes 0xffffff; the firmware writes the panel's value, 0x640 in 11:0 for panel id 5.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `fp_de_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start of the window (ON) |
| 11:0 | `fp_de_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | end of the window (OFF) |

### Register `vt_hpixen`

Internal horizontal pixel enable window. ON is the blank end less 19 (wrapped by the total) and OFF is ON plus the width less 2 (wrapped). Software writes 0xffffff first and the real value last.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_hpixen_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_hpixen_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_vpixen`

Internal vertical pixel enable window. ON is the vertical blank end and OFF the vertical blank start.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_vpixen_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_vpixen_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_hcmap`

Colour-map write window, horizontal: the blank start and end (Linux writes the end less 3).

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_hcmap_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_hcmap_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `vt_vcmap`

Colour-map write window, vertical: the blank start and end.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vt_vcmap_on` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | start of the window (ON) |
| 11:0 | `vt_vcmap_off` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine | end of the window (OFF) |

### Register `did_start_xy`

Start position for the per-window id fetch. Y is the vertical blank start less the end less 1, a small negative number written as 12-bit two's complement; X is the horizontal blank end less 20, wrapped by the total. The Windows 2000 boot video driver writes 0xffffffff.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `did_starty` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start line |
| 11:0 | `did_startx` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start dot |

### Register `crs_start_xy`

Start position for the cursor. Y is one more than the id fetch's Y; X is the horizontal blank end less 54 (the Linux header calls 54 the cursor magic number), wrapped by the total.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `crs_starty` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start line |
| 11:0 | `crs_startx` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start dot |

### Register `vc_start_xy`

Start position for the video capture. Y is the same as the id fetch's; X is the horizontal blank end less 4 (not wrapped).

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 23:12 | `vc_starty` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start line |
| 11:0 | `vc_startx` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | start dot |

### Register `ovr_width_tile`

Overlay plane width. Whole 512-byte tile columns in 12:5 and the remainder in 32-byte units in 4:0; FIFO reset in bit 13 is pulsed set then clear. The overlay is always 8 bits per pixel. The NT miniport writes the full width then pulses; the firmware writes the FIFO reset bit alone, then 0, with the width zero. The firmware self-test walks bits 13:0.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 13 | `ovr_fifo_reset` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | FIFO reset, pulsed |
| 12:5 | `ovr_width_tile` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | whole tile columns |
| 4:0 | `ovr_rhs` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | remainder in 32-byte units |

### Register `ovr_inhwctrl`

Live copy of the overlay control word. The NT miniport, the firmware and the Windows 2000 frame buffer driver poll bit 0 with a byte load until it clears (the firmware polls it until it reads set); the NT 4.0 HAL reads it as a dword. Bit 0 must follow its shadow within the 200 ms the drivers allow.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 0 | `ovr_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, overlay stop routine | overlay fetch active |

### Register `ovr_control`

Overlay control, the shadow word taken at the start of vertical blank. Bit 0 enables fetch; the rest is the overlay's tile-list pointer. The miniport loads the pointer without bit 0; the display driver writes the pointer with bit 0 set to start the fetch and the pointer alone to stop it. The firmware self-test walks bits 31:5 and 1:0.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:9 | `ovr_tile_ptr` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, display driver overlay routines | address bits 31:9 of the overlay tile list, 512-byte aligned; the span is by analogy with the normal plane |
| 0 | `ovr_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, display driver overlay routines | overlay fetch enable |

### Register `frm_size_tile`

Normal plane width and depth. Whole tile columns in 12:5 and the remainder in 32-byte units in 4:0, depth in 14:13, FIFO reset in bit 15 pulsed. A tile is 512 bytes by 128 lines, so a 32-bit mode has 128 pixels per tile column and a 16-bit mode 256. The NT miniport writes the value twice around the reset pulse; the firmware does the same. Linux, NT and the O2 display back end page agree.  The writers disagree on the width: the NT miniport, the Windows 2000 frame buffer miniport and the firmware write the exact line length in bytes (whole columns plus the remainder in 32-byte units, 0x24 for 640 pixels at 8 bits), while Linux and the Windows 2000 boot video driver write the length rounded up to whole columns with a zero remainder (0x40 for 640 and for 800 pixels at 8 bits; 0x60 for 1280). All of these are read from the writers. The register is not the visible width: every writer also programs the pixel-enable windows (`vt_hpixen`, `vt_vpixen`), whose lengths are the visible width less 2 and the visible height. What the fetch does with the bytes past the exact length is inferred (shown pixels come from the windows). The firmware self-test walks bits 15:0.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 15 | `frm_fifo_reset` | established |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | FIFO reset, pulsed set then clear |
| 14:13 | `frm_depth` | established | frame_depth (bpp8=0, bpp16=1, bpp32=2, reserved=3) | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | pixel depth; the value 3 is not used by any driver |
| 12:5 | `frm_width_tile` | established |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | whole 512-byte tile columns |
| 4:0 | `frm_rhs` | established |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | remainder in 32-byte units |

### Register `frm_size_pixel`

Normal plane height in lines, in 31:16. The firmware self-test walks only those bits.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:16 | `fb_height_pix` | established |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation PROM 1.1005, display start-up code | height in lines |

### Register `frm_inhwctrl`

Live copy of the normal plane control word. Completion of a page flip is this word matching the value written; stop waits for bit 0 to clear. Software reads bit 0 with a byte load in the NT miniport, the Windows 2000 frame buffer driver and the firmware, and as a dword in the Windows 2000 boot video driver and the NT 4.0 HAL; a model must answer a byte read with the matching byte. The firmware waits forever for bit 0 to read set after it starts the plane. Bit 0 must clear within 200 ms of its shadow and follow it at the start of vertical blank.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:9 | `frm_tile_ptr` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, page-flip and stop routines | address bits 31:9, copy of the shadow's address field; by analogy with the normal plane |
| 1 | `frm_linear` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, page-flip and stop routines | linear form, copy of the shadow; by analogy with the normal plane |
| 0 | `frm_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, page-flip and stop routines | normal plane fetch active |

### Register `frm_control`

Normal plane control, the shadow word taken at the start of vertical blank. Bit 0 enables fetch; bit 1 selects the linear form, in which the pointer is the frame buffer's physical address shifted right 9 and the line pitch is the tile-column count rounded up times 512 bytes (Linux, both Windows 2000 drivers and the firmware set it, the SGI 5.2.4 miniport does not and gives its tile list instead). The pitch rule is inferred from the two writers whose frames are not Linux: the firmware (exact width register, 1024-byte stride at 640 pixels) and the Windows 2000 boot video driver (rounded-up register, 1024-byte stride at 640). Linux lays its lines out at the visible width in bytes (640 and 800, not 1024), which disagrees with that pitch for any width that is not a multiple of 512; whether the hardware then shears is unknown. In the tile-list form the pointer is the physical address of a 512-byte aligned list of 16-bit entries. Both stop DMA by clearing bit 0 and wait up to 20000 ten-microsecond steps for the live copy to follow. The firmware self-test walks bits 31:9 and 1:0.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:9 | `frm_tile_ptr` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, start, flip and stop routines; SGI Visual Workstation PROM 1.1005, display start-up code | address bits 31:9: the tile list address, or the frame buffer address in the linear form, shifted right 9 |
| 1 | `frm_linear` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, start, flip and stop routines; SGI Visual Workstation PROM 1.1005, display start-up code | linear form |
| 0 | `frm_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, start, flip and stop routines; SGI Visual Workstation PROM 1.1005, display start-up code | normal plane fetch enable |

### Register `did_inhwctrl`

Live copy of the per-window id fetch control word. The NT miniport tests bit 16 with a byte load of the third byte. The live bit is bit 16. Linux names bit 0 and writes 0, which looks wrong against NT and the O2 display back end.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 16 | `did_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, id fetch stop routine | id fetch active, bit 16; the Linux header names bit 0, which looks wrong |

### Register `did_control`

Per-window id fetch control, the shadow word. The NT miniport writes 0 at mode set and clears bit 16 to stop; the firmware writes 0. The firmware self-test walks all 32 bits.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 16 | `did_dma_enable` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, id fetch routines; SGI Visual Workstation PROM 1.1005, display start-up code | id fetch enable, bit 16 (the NT driver clears it to stop); the Linux header names bit 0, which looks wrong |

### Register `mode_regs`

Per-window mode table of 32 words, the window id table. Linux fills all 32 identically (8 bits per pixel as index, 16 as RGBA5, 32 as RGB8); the firmware and the Windows 2000 boot video driver write 3 into all 32; the Windows 2000 frame buffer driver writes only the first word, 3 for 8 bits per pixel and 0x17 for 32. The SGI 5.2.4 miniport does not write the table.

Array of 32 registers, stride 4.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 10 | `wid_gamma_bypass` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Visual Workstation PROM 1.1005, display start-up code; Windows 2000 boot video and frame buffer drivers | gamma bypass: 1 disables the gamma map, 0 enables it. From the O2 display back end specification; no Arsenic driver sets it |
| 9:5 | `wid_cm_bank` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Visual Workstation PROM 1.1005, display start-up code; Windows 2000 boot video and frame buffer drivers | colour map bank select: the upper five bits of the colour map index for an 8-bit index pixel, 256 entries to a bank. From the O2 display back end specification; no Arsenic driver or firmware writes a non-zero value (every value written has these bits clear) |
| 4:2 | `wid_typ` | inferred | did_pixel_type (index8=0, index12=1, rg3b2=2, rgb4=3, argb5=4, rgb8=5, rgba5=6, rgb10=7) | Linux frame buffer driver for the display back end, register block header and driver; SGI Visual Workstation PROM 1.1005, display start-up code; Windows 2000 boot video and frame buffer drivers | pixel type |
| 1:0 | `wid_buf` | inferred | did_buffer_mode (both=3) | Linux frame buffer driver for the display back end, register block header and driver; SGI Visual Workstation PROM 1.1005, display start-up code; Windows 2000 boot video and frame buffer drivers | buffer mode; 3 selects both buffers |

### Register `cmap`

Colour map, 6144 entries in the Linux declaration (the O2 display back end specification says 4608), one dword each. Both drivers write an entry as red in 31:24, green in 23:16 and blue in 15:8. Before every 32nd entry the NT miniport waits on the FIFO status; the Windows 2000 boot video driver waits before every entry.

Array of 6144 registers, stride 4.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:24 | `red` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 23:16 | `green` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 15:8 | `blue` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code |  |

### Register `cm_fifo`

Colour map FIFO status. Linux waits until it reads non-zero and then writes that many entries. The NT miniport, before every 32nd entry, spins until the low six bits read zero or the value reaches 0x30. The firmware waits while the low six bits are non-zero and above 0x10 is not reached. The Windows 2000 boot video driver needs the low six bits above 0x30. A value of 0x3f satisfies all four readers and 0x40 fails the boot video driver. Meaning inferred as a free-entry count.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 5:0 | `free_entries` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, colour register routine; SGI Visual Workstation PROM 1.1005, display start-up code | free entries in the colour map write FIFO |

### Register `gmap`

Gamma map, 256 dwords. Linux, the firmware and the Windows 2000 boot video driver write the identity ramp (i times 0x01010100). The NT miniport copies dwords from the registry value and from the private gamma control request, and reads the table back for that request. What the hardware does with the words is inferred. The driver writes are established; the grade is for what the hardware does with them.

Array of 256 registers, stride 4.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:24 | `red` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, gamma routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 23:16 | `green` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, gamma routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 15:8 | `blue` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, gamma routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |

### Register `gmap10`

10-bit gamma map, 1024 dwords. The same two NT paths write it when the size selector is 10 or the registry value is 4096 bytes long. The field layout of an entry is unknown. The driver writes are established; the grade is for what the hardware does with them.

Array of 1024 registers, stride 4.

### Register `crs_pos`

Cursor position. The display driver writes the pointer position less the hot spot, masked to 12 bits, x in 11:0 and y in 27:16; a hidden cursor is parked with the value of the cursor park constant. Every other writer only writes 0. What the hardware does with the hot-spot-adjusted position is inferred from the O2 display back end. The firmware self-test walks all 32 bits. The driver writes are established; the grade is for what the hardware does with them.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 27:16 | `y` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code | vertical position less hot spot, 12-bit two's complement as for x |
| 11:0 | `x` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code | horizontal position less hot spot, a 12-bit two's complement number: the display driver masks x minus the hot spot with 0xfff. Sign extension from bit 11 by the hardware is inferred from the park value, which puts 0xfe0 (minus 32) in both fields to move the cursor off screen |

### Register `crs_ctl`

Cursor control. The display driver writes 1 to show and 0 to hide; the NT 4.0 HAL's stop clears bit 0 with a read-modify-write; every other writer writes 0. The firmware self-test walks bits 1:0, so bit 1 is implemented and unused. The driver writes are established; the grade is for what the hardware does with them.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 1 | `bit1` | unknown |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code | stored but no shipped writer sets it |
| 0 | `show` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code | cursor visible |

### Register `crs_cmap`

Cursor colours, three entries. The display driver writes each as red in 31:24, green in 23:16 and blue in 15:8 (the colour list's entries 1 to 3 go to these three words) and at start-up sets the second to 0 and the third to 0xffffff00; the firmware writes 0xffffff00 and 0xff000000 to the first two. The firmware self-test walks bits 31:8. The driver writes are established; the grade is for what the hardware does with them.

Array of 3 registers, stride 4.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:24 | `red` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 23:16 | `green` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 15:8 | `blue` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |

### Register `crs_glyph`

Cursor glyph, 64 dwords: a 32 by 32 pointer, two dwords per row, 2 bits per pixel packed from the top of the dword, 0 for transparent and 1 to 3 for the entry of the cursor colour list. The display driver rewrites all 64 for a pointer of at most 32 by 32 pixels and three colours and draws any other pointer in software. Pixel x of row y is field px(x mod 16) of dword 2y + x div 16, so px0 is the leftmost pixel of a dword and sits in bits 31:30. The scan order and the colour index are inferred from the O2 display back end. The driver writes are established; the grade is for what the hardware does with them.

Array of 64 registers, stride 4.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:30 | `px0` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 29:28 | `px1` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 27:26 | `px2` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 25:24 | `px3` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 23:22 | `px4` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 21:20 | `px5` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 19:18 | `px6` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 17:16 | `px7` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 15:14 | `px8` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 13:12 | `px9` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 11:10 | `px10` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 9:8 | `px11` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 7:6 | `px12` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 5:4 | `px13` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 3:2 | `px14` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |
| 1:0 | `px15` | inferred |  | Linux frame buffer driver for the display back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM 1.1005, display start-up code |  |

### Register `vc_0`

Video capture control word 0. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0x00ffffff, the only statement of how many bits are implemented.

### Register `vc_1`

Video capture control word 1. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0x00ffffff, the only statement of how many bits are implemented.

### Register `vc_2`

Video capture control word 2. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0x1f, the only statement of how many bits are implemented.

### Register `vc_3`

Video capture control word 3. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test does not walk it.

### Register `vc_4`

Video capture control word 4. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0xffffffff, the only statement of how many bits are implemented.

### Register `vc_5`

Video capture control word 5. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0xffffffff, the only statement of how many bits are implemented.

### Register `vc_6`

Video capture control word 6. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0xffffffff, the only statement of how many bits are implemented.

### Register `vc_7`

Video capture control word 7. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test does not walk it.

### Register `vc_8`

Video capture control word 8. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt pins 16 to 19 of the system interrupt controller. The firmware self-test walks the bits of mask 0xffffffff, the only statement of how many bits are implemented.

### Register `retrace_status`

Retrace latch in its own 32-bit window, wired to line 24 of the system interrupt controller (latched). The retrace handler reads it, writes back the bitwise complement of what it read and does not read it again. That only clears the bits if a written 0 clears a bit and a written 1 leaves it, so the register is write-0-to-clear (inferred). The handler counts a frame when bit 1 was set (in a stereo mode only on the fields where the low bit of its field counter is 1) and queues the whole word, not only the bits it counted: it takes a record from the free list, stores the word in it and links it behind the pending ones, and queues the deferred routine; if a record is already pending, the word is dropped and only a counter grows (established). The deferred routine tests bit 0 and, only when bit 0 is clear, bit 1, and no other bit; what the other bits are is unknown. The firmware's hand-over routine writes the complement of the word the same way. The name is ours: the register has no public name.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 1 | `frame_end` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, retrace interrupt routine; SGI Visual Workstation PROM 1.1005, display start-up code | a frame has ended; the deferred routine flips the display tile list (in a mono mode only while a swap copy is pending) and signals the vertical-blank wait event; tested only when bit 0 is clear |
| 0 | `release_copies` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, retrace interrupt routine; SGI Visual Workstation PROM 1.1005, display start-up code | the deferred routine releases the held swap copies, only while the live line is in [line 0, height - 3) or the mode is stereo (established for the software); that the bit is the scan-line interrupt 0 latch is inferred |

## Words

| Word | Width | Grade | Source |
|---|---|---|---|
| `PIXEL_RGB8` | 32 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |
| `PIXEL_RGBA5` | 16 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |
| `TILE_LIST_ENTRY` | 16 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, frame start and flip routines |

### Word `PIXEL_RGB8`

A 32-bit frame buffer pixel of the RGB8 type. The miniport's pixel format table and the Windows 2000 frame buffer driver give red in the top byte, green below it and blue in 15:8; the Linux frame buffer driver puts red in the low byte, so the byte order is not settled for the hardware. The low byte is not described. The colour map and gamma entries use the same red, green, blue positions.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 31:24 | `red` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |
| 23:16 | `green` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |
| 15:8 | `blue` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |

### Word `PIXEL_RGBA5`

A 16-bit frame buffer pixel of the RGBA5 type: five bits of red on top, then green and blue, and one alpha bit at the bottom. The miniport's format table and the Linux driver agree on these masks.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 15:11 | `red` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |
| 10:6 | `green` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |
| 5:1 | `blue` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |
| 0 | `alpha` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport pixel format table; Linux frame buffer driver |  |

### Word `TILE_LIST_ENTRY`

One entry of a tile list in main memory: the tile's first 4 KB page frame number shifted right 4 and truncated to 16 bits, so bit 15 is physical address bit 31 and no bit is an attribute. All four routines that write a list store exactly this. A list is 512-byte aligned, non-cached and (height + 127) / 128 rows of one entry per 64 KB tile; lists 0 to 3 belong to the normal plane (the current one flips by exclusive-or with 1) and list 4 to the overlay. That the engine reads an entry this way is inferred from the O2 display back end, whose tile pointer is 16 bits. The driver writes are established; the grade is for what the hardware does with them.

| Bits | Field | Grade | Values | Source | Description |
|---|---|---|---|---|---|
| 15:0 | `tile_number` | inferred |  | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport, frame start and flip routines | 64 KB tile number |

## Enums

### Enum `did_buffer_mode`

| Label | Value | Grade | Source |
|---|---|---|---|
| `both` | 3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |

### Enum `did_pixel_type`

Pixel type of a window in the per-window mode table: the values the Linux header names. Which hardware path each selects is inferred from the O2 display back end.

| Label | Value | Grade | Source |
|---|---|---|---|
| `index8` | 0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `index12` | 1 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `rg3b2` | 2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `rgb4` | 3 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `argb5` | 4 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `rgb8` | 5 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `rgba5` | 6 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `rgb10` | 7 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |

### Enum `frame_depth`

| Label | Value | Grade | Source |
|---|---|---|---|
| `bpp8` | 0 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `bpp16` | 1 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `bpp32` | 2 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |
| `reserved` | 3 | unknown | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) |

## Constants

| Constant | Value | Grade | Source | Description |
|---|---|---|---|---|
| `BLOCK_SIZE_DECLARED` | 0x80024 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | size of the register structure in the miniport's program database, the last capture word included; no source gives a larger size such as a page-rounded 0x81000, which is a model choice |
| `BLOCK_SIZE_LINUX` | 0x1000000 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | size the Linux frame buffer driver reserves for the block |
| `BLOCK_SIZE_MAPPED` | 0x80020 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | bytes the NT miniport maps and claims for the block, ending at the last capture word |
| `CHIPID_REAL_320` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | chip revision a real 320 reports (registry export: Display Engine 002); that it is the low nibble of the control and status register is inferred |
| `CLOCK_REF_KHZ` | 0x6978 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | dot clock reference, from the Linux header and the formula that fits the NT mode table |
| `CMAP_BANK_ENTRIES` | 0x100 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | entries in one colour map bank, from the eight index bits of an 8-bit pixel and the O2 display back end specification; the NT drivers write indices 0 to 255 |
| `COLOR_MAP_FIFO_FREE` | 0x3f | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | value that satisfies every shipped reader of the colour map FIFO status |
| `CRS_MAGIC` | 0x36 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | Linux's name for the cursor start offset |
| `CURSOR_GLYPH_PIXELS_PER_DWORD` | 0x10 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | glyph pixels in one dword |
| `CURSOR_GLYPH_PIXEL_BITS` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | bits per glyph pixel; 16 pixels to a dword, the first in bits 31:30 |
| `CURSOR_GLYPH_ROWS` | 0x20 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | rows in the cursor glyph |
| `CURSOR_GLYPH_ROW_DWORDS` | 0x2 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | dwords per glyph row |
| `CURSOR_PARK_POS` | 0xfe00fe0 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | value the display driver writes to the cursor position to hide it |
| `ID_VALUE` | 0x666 | inferred | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | value the id register must read; the check is established, what the value means is inferred |
| `NORMAL_TILE_LISTS` | 0x4 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | tile lists the NT miniport keeps for the normal plane |
| `TILE_BYTES` | 0x10000 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | memory per tile, one list entry each |
| `TILE_HEIGHT_LINES` | 0x80 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | tile height |
| `TILE_LIST_ALIGN` | 0x200 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | alignment of a tile list |
| `TILE_WIDTH_BYTES` | 0x200 | established | SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0, display miniport and its program database (the gberegs structure); Linux frame buffer driver header (struct asregs and the DBE definitions) | tile width |
