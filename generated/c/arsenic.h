/* generated from devices/arsenic/registers.toml by defs-gen - do not edit,
 * change the source */
/* SPDX-License-Identifier: BSD-3-Clause */
/* defs-gen 3 */
/* input sha256 8125592e1294c4b2309e70f52ec7a8ce510f33fe6751ca63d12a1cd6e8d9276a */
/* Arsenic display back end register block */
#ifndef ARS_GEN_H
#define ARS_GEN_H

#include <stdint.h>


/* enum did_buffer_mode */
#define ARS_DID_BUFFER_MODE_BOTH                     3 /* inferred */

/* enum did_pixel_type: Pixel type of a window in the per-window mode table: the
 * values the Linux header names. Which hardware path each selects is inferred
 * from the O2 display back end. */
#define ARS_DID_PIXEL_TYPE_INDEX8                    0 /* inferred */
#define ARS_DID_PIXEL_TYPE_INDEX12                   1 /* inferred */
#define ARS_DID_PIXEL_TYPE_RG3B2                     2 /* inferred */
#define ARS_DID_PIXEL_TYPE_RGB4                      3 /* inferred */
#define ARS_DID_PIXEL_TYPE_ARGB5                     4 /* inferred */
#define ARS_DID_PIXEL_TYPE_RGB8                      5 /* inferred */
#define ARS_DID_PIXEL_TYPE_RGBA5                     6 /* inferred */
#define ARS_DID_PIXEL_TYPE_RGB10                     7 /* inferred */

/* enum frame_depth */
#define ARS_FRAME_DEPTH_BPP8                         0 /* established */
#define ARS_FRAME_DEPTH_BPP16                        1 /* established */
#define ARS_FRAME_DEPTH_BPP32                        2 /* established */
#define ARS_FRAME_DEPTH_RESERVED                     3 /* unknown */

/*
 * General control and status. The mode-set routine of the NT miniport writes
 * the value with the clock selector at 3 as its last step; Linux writes the
 * selector at 2 before programming and 3 afterwards when bit 25 reads set; the
 * firmware does the same, and with a flat panel ORs in bit 26 instead of
 * writing the selector 3. The firmware reads the low nibble as the chip
 * revision (a real 320 shows display engine 002). Linux treats a clock selector
 * below 2 as not scanning out from the internal dot clock.
 */
/* ctrlstat: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_CTRLSTAT_OFFSET                          0x0u /* inferred */
#define ARS_CTRLSTAT_PCLKSEL_SHIFT                   28 /* inferred */
#define ARS_CTRLSTAT_PCLKSEL_MASK                    0x3u
#define ARS_CTRLSTAT_PCLKSEL_INPLACE (ARS_CTRLSTAT_PCLKSEL_MASK << ARS_CTRLSTAT_PCLKSEL_SHIFT)
#define ARS_CTRLSTAT_BIT26_SHIFT                     26 /* unknown */
#define ARS_CTRLSTAT_BIT26_MASK                      0x1u
#define ARS_CTRLSTAT_BIT26_INPLACE (ARS_CTRLSTAT_BIT26_MASK << ARS_CTRLSTAT_BIT26_SHIFT)
#define ARS_CTRLSTAT_BIT25_SHIFT                     25 /* unknown */
#define ARS_CTRLSTAT_BIT25_MASK                      0x1u
#define ARS_CTRLSTAT_BIT25_INPLACE (ARS_CTRLSTAT_BIT25_MASK << ARS_CTRLSTAT_BIT25_SHIFT)
#define ARS_CTRLSTAT_SENSE_N_SHIFT                   4 /* unknown */
#define ARS_CTRLSTAT_SENSE_N_MASK                    0x1u
#define ARS_CTRLSTAT_SENSE_N_INPLACE (ARS_CTRLSTAT_SENSE_N_MASK << ARS_CTRLSTAT_SENSE_N_SHIFT)
#define ARS_CTRLSTAT_CHIPID_SHIFT                    0 /* inferred */
#define ARS_CTRLSTAT_CHIPID_MASK                     0xfu
#define ARS_CTRLSTAT_CHIPID_INPLACE (ARS_CTRLSTAT_CHIPID_MASK << ARS_CTRLSTAT_CHIPID_SHIFT)

/*
 * Dot clock PLL. Software writes M-1 and N-1. The output is 27 MHz times M over
 * N, divided by 2 to the P; that reproduces the clock column of 26 of the 27
 * rows of the NT mode table to within 1.4 percent. No lock bit is read and
 * every wait is a plain delay. The NT miniport clears everything above bit 15,
 * restarts the PLL through a fixed sequence with RUN set and cleared and 40 ms,
 * one second and 100 ms delays, then writes the final value without RUN, waits
 * 40 ms and sets RUN; Linux and the firmware write the final value with RUN and
 * wait 11 ms; the NT 4.0 HAL clears the bits above 15 and writes the value with
 * RUN with no wait.
 */
/* dotclock: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_DOTCLOCK_OFFSET                          0x4u /* inferred */
#define ARS_DOTCLOCK_RUN_SHIFT                       20 /* inferred */
#define ARS_DOTCLOCK_RUN_MASK                        0x1u
#define ARS_DOTCLOCK_RUN_INPLACE (ARS_DOTCLOCK_RUN_MASK << ARS_DOTCLOCK_RUN_SHIFT)
#define ARS_DOTCLOCK_P_SHIFT                         14 /* inferred */
#define ARS_DOTCLOCK_P_MASK                          0x3u
#define ARS_DOTCLOCK_P_INPLACE (ARS_DOTCLOCK_P_MASK << ARS_DOTCLOCK_P_SHIFT)
#define ARS_DOTCLOCK_N_SHIFT                         8 /* inferred */
#define ARS_DOTCLOCK_N_MASK                          0x3fu
#define ARS_DOTCLOCK_N_INPLACE (ARS_DOTCLOCK_N_MASK << ARS_DOTCLOCK_N_SHIFT)
#define ARS_DOTCLOCK_M_SHIFT                         0 /* inferred */
#define ARS_DOTCLOCK_M_MASK                          0xffu
#define ARS_DOTCLOCK_M_INPLACE (ARS_DOTCLOCK_M_MASK << ARS_DOTCLOCK_M_SHIFT)

/*
 * CRT I2C lines (DDC). One register is both the write and the read-back
 * location. Writing a bit as 1 pulls the line low and writing 0 lets it float
 * high; a read returns 1 when the line is low. With no device both lines read
 * 0. The EDID reader tries device write addresses 0xa2, 0xa6 and 0xa0 in that
 * order.
 */
/* i2c: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for Windows NT
 * 4.0, display miniport, bit-bang routines and the EDID reader */
#define ARS_I2C_OFFSET                               0x8u /* established */
#define ARS_I2C_SCL_SHIFT                            1 /* established */
#define ARS_I2C_SCL_MASK                             0x1u
#define ARS_I2C_SCL_INPLACE (ARS_I2C_SCL_MASK << ARS_I2C_SCL_SHIFT)
#define ARS_I2C_SDA_SHIFT                            0 /* established */
#define ARS_I2C_SDA_MASK                             0x1u
#define ARS_I2C_SDA_INPLACE (ARS_I2C_SDA_MASK << ARS_I2C_SDA_SHIFT)

/*
 * System clock PLL. Neither driver, the firmware nor the HAL touches it; the
 * Linux header names it as the system clock PLL control. Nothing shipped
 * depends on its contents.
 */
/* sysclk: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only) */
#define ARS_SYSCLK_OFFSET                            0xcu /* unknown */

/*
 * Flat-panel I2C lines, the same layout and sense as the CRT register. The
 * flat-panel status device answers at write address 0x70 and read address 0x71;
 * bits 7:5 of its status byte are the panel id (5 is the 1600 by 1024 digital
 * panel) and bit 2 is power-save. The firmware reads it twice when the first
 * try gets no answer, and when the CRT EDID reader finds a flat panel it uses
 * this register instead of the CRT one.
 */
/* i2cfp: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for Windows NT
 * 4.0, display miniport, bit-bang routines and the flat-panel status routines
 */
#define ARS_I2CFP_OFFSET                             0x10u /* established */
#define ARS_I2CFP_SCL_SHIFT                          1 /* established */
#define ARS_I2CFP_SCL_MASK                           0x1u
#define ARS_I2CFP_SCL_INPLACE (ARS_I2CFP_SCL_MASK << ARS_I2CFP_SCL_SHIFT)
#define ARS_I2CFP_SDA_SHIFT                          0 /* established */
#define ARS_I2CFP_SDA_MASK                           0x1u
#define ARS_I2CFP_SDA_INPLACE (ARS_I2CFP_SDA_MASK << ARS_I2CFP_SDA_SHIFT)

/*
 * Device id. The NT miniport compares the whole dword with the value in the ID
 * constant and does not attach otherwise; the Windows 2000 drivers compare the
 * low 16 bits; the firmware self-test compares the whole dword. A read of that
 * value passes every reader (established as the check; what the value means is
 * inferred).
 */
/* id: r32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0,
 * display miniport, adapter attach check; the firmware self-test */
#define ARS_ID_OFFSET                                0x14u /* inferred */

/*
 * Output configuration. The NT miniport writes bits 7, 6, 5, 2 and 0, sets bit
 * 0 when the mode asks for sync on green (the sync-on-green bit), always sets
 * bit 3, and clears bit 0 when a flat panel answered unless the mode carries
 * the key flag. Linux defines a frame-buffer value that sets bits 7, 5, 2, 1
 * and 0 but never writes the register; the firmware writes that value; the NT
 * 4.0 HAL writes the same and ORs in bit 3. The Windows 2000 frame buffer
 * driver writes bits 7, 5, 2 and 0, with bit 3 added for a flat panel, and its
 * boot video driver one further value. Nothing shipped writes bit 6 or bit 1 on
 * its own. A model should accept any value. The firmware self-test walks bits
 * 7:5 and 3:0 (bit 4 is not tested).
 */
/* config: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_CONFIG_OFFSET                            0x18u /* inferred */
#define ARS_CONFIG_EXT_ADDR_SHIFT                    7 /* inferred */
#define ARS_CONFIG_EXT_ADDR_MASK                     0x1u
#define ARS_CONFIG_EXT_ADDR_INPLACE (ARS_CONFIG_EXT_ADDR_MASK << ARS_CONFIG_EXT_ADDR_SHIFT)
#define ARS_CONFIG_TILEHIST_SHIFT                    6 /* inferred */
#define ARS_CONFIG_TILEHIST_MASK                     0x1u
#define ARS_CONFIG_TILEHIST_INPLACE (ARS_CONFIG_TILEHIST_MASK << ARS_CONFIG_TILEHIST_SHIFT)
#define ARS_CONFIG_LENDIAN_SHIFT                     5 /* inferred */
#define ARS_CONFIG_LENDIAN_MASK                      0x1u
#define ARS_CONFIG_LENDIAN_INPLACE (ARS_CONFIG_LENDIAN_MASK << ARS_CONFIG_LENDIAN_SHIFT)
#define ARS_CONFIG_FPENABLE_SHIFT                    3 /* inferred */
#define ARS_CONFIG_FPENABLE_MASK                     0x1u
#define ARS_CONFIG_FPENABLE_INPLACE (ARS_CONFIG_FPENABLE_MASK << ARS_CONFIG_FPENABLE_SHIFT)
#define ARS_CONFIG_VDAC_PBLANK_SHIFT                 2 /* inferred */
#define ARS_CONFIG_VDAC_PBLANK_MASK                  0x1u
#define ARS_CONFIG_VDAC_PBLANK_INPLACE (ARS_CONFIG_VDAC_PBLANK_MASK << ARS_CONFIG_VDAC_PBLANK_SHIFT)
#define ARS_CONFIG_VDAC_GSYNC_SHIFT                  1 /* inferred */
#define ARS_CONFIG_VDAC_GSYNC_MASK                   0x1u
#define ARS_CONFIG_VDAC_GSYNC_INPLACE (ARS_CONFIG_VDAC_GSYNC_MASK << ARS_CONFIG_VDAC_GSYNC_SHIFT)
#define ARS_CONFIG_VDAC_ENABLE_SHIFT                 0 /* inferred */
#define ARS_CONFIG_VDAC_ENABLE_MASK                  0x1u
#define ARS_CONFIG_VDAC_ENABLE_INPLACE (ARS_CONFIG_VDAC_ENABLE_MASK << ARS_CONFIG_VDAC_ENABLE_SHIFT)

/*
 * Built-in self-test. Neither driver reads it. The firmware self-test writes 0,
 * polls bit 31 for completion and reports a failure when any of bits 27:24 is
 * set. Release 1.0006 also compares the register with a mask of 0x0ff0ffff for
 * zero. The layout of the failure bits is unknown.
 */
/* bist: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code */
#define ARS_BIST_OFFSET                              0x1cu /* inferred */
#define ARS_BIST_DONE_SHIFT                          31 /* inferred */
#define ARS_BIST_DONE_MASK                           0x1u
#define ARS_BIST_DONE_INPLACE (ARS_BIST_DONE_MASK << ARS_BIST_DONE_SHIFT)
#define ARS_BIST_FAIL_SHIFT                          24 /* inferred */
#define ARS_BIST_FAIL_MASK                           0xfu
#define ARS_BIST_FAIL_INPLACE (ARS_BIST_FAIL_MASK << ARS_BIST_FAIL_SHIFT)

/*
 * Current dot coordinates. The live line is read from bits 23:12 (the scan-line
 * poll); bit 31 is the timing counter freeze: it reads back what software set
 * by writing the top bit and clears when software writes 0. Software tests the
 * freeze bit with a byte load of the top byte, and the NT miniport, the Windows
 * 2000 frame buffer driver, the NT 4.0 HAL and the firmware all do. The
 * firmware's non-graphics console path writes the freeze bit alone.
 */
/* vt_xy: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, scan-line and mode-set routines; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_VT_XY_OFFSET                             0x10000u /* inferred */
#define ARS_VT_XY_VT_FREEZE_SHIFT                    31 /* inferred */
#define ARS_VT_XY_VT_FREEZE_MASK                     0x1u
#define ARS_VT_XY_VT_FREEZE_INPLACE (ARS_VT_XY_VT_FREEZE_MASK << ARS_VT_XY_VT_FREEZE_SHIFT)
#define ARS_VT_XY_Y_SHIFT                            12 /* inferred */
#define ARS_VT_XY_Y_MASK                             0xfffu
#define ARS_VT_XY_Y_INPLACE (ARS_VT_XY_Y_MASK << ARS_VT_XY_Y_SHIFT)
#define ARS_VT_XY_X_SHIFT                            0 /* inferred */
#define ARS_VT_XY_X_MASK                             0xfffu
#define ARS_VT_XY_X_INPLACE (ARS_VT_XY_X_MASK << ARS_VT_XY_X_SHIFT)

/*
 * Totals: horizontal total in 11:0 and vertical total in 23:12. Software writes
 * 0 while it programs the other windows and the real totals afterwards.
 */
/* vt_xymax: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_XYMAX_OFFSET                          0x10004u /* inferred */
#define ARS_VT_XYMAX_VT_MAXY_SHIFT                   12 /* inferred */
#define ARS_VT_XYMAX_VT_MAXY_MASK                    0xfffu
#define ARS_VT_XYMAX_VT_MAXY_INPLACE (ARS_VT_XYMAX_VT_MAXY_MASK << ARS_VT_XYMAX_VT_MAXY_SHIFT)
#define ARS_VT_XYMAX_VT_MAXX_SHIFT                   0 /* inferred */
#define ARS_VT_XYMAX_VT_MAXX_MASK                    0xfffu
#define ARS_VT_XYMAX_VT_MAXX_INPLACE (ARS_VT_XYMAX_VT_MAXX_MASK << ARS_VT_XYMAX_VT_MAXX_SHIFT)

/*
 * Vertical sync window, in lines.
 */
/* vt_vsync: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_VSYNC_OFFSET                          0x10008u /* inferred */
#define ARS_VT_VSYNC_VT_VSYNC_ON_SHIFT               12 /* inferred */
#define ARS_VT_VSYNC_VT_VSYNC_ON_MASK                0xfffu
#define ARS_VT_VSYNC_VT_VSYNC_ON_INPLACE (ARS_VT_VSYNC_VT_VSYNC_ON_MASK << ARS_VT_VSYNC_VT_VSYNC_ON_SHIFT)
#define ARS_VT_VSYNC_VT_VSYNC_OFF_SHIFT              0 /* inferred */
#define ARS_VT_VSYNC_VT_VSYNC_OFF_MASK               0xfffu
#define ARS_VT_VSYNC_VT_VSYNC_OFF_INPLACE (ARS_VT_VSYNC_VT_VSYNC_OFF_MASK << ARS_VT_VSYNC_VT_VSYNC_OFF_SHIFT)

/*
 * Horizontal sync window, in dots.
 */
/* vt_hsync: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_HSYNC_OFFSET                          0x1000cu /* inferred */
#define ARS_VT_HSYNC_VT_HSYNC_ON_SHIFT               12 /* inferred */
#define ARS_VT_HSYNC_VT_HSYNC_ON_MASK                0xfffu
#define ARS_VT_HSYNC_VT_HSYNC_ON_INPLACE (ARS_VT_HSYNC_VT_HSYNC_ON_MASK << ARS_VT_HSYNC_VT_HSYNC_ON_SHIFT)
#define ARS_VT_HSYNC_VT_HSYNC_OFF_SHIFT              0 /* inferred */
#define ARS_VT_HSYNC_VT_HSYNC_OFF_MASK               0xfffu
#define ARS_VT_HSYNC_VT_HSYNC_OFF_INPLACE (ARS_VT_HSYNC_VT_HSYNC_OFF_MASK << ARS_VT_HSYNC_VT_HSYNC_OFF_SHIFT)

/*
 * Vertical blank window, in lines.
 */
/* vt_vblank: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_VBLANK_OFFSET                         0x10010u /* inferred */
#define ARS_VT_VBLANK_VT_VBLANK_ON_SHIFT             12 /* inferred */
#define ARS_VT_VBLANK_VT_VBLANK_ON_MASK              0xfffu
#define ARS_VT_VBLANK_VT_VBLANK_ON_INPLACE (ARS_VT_VBLANK_VT_VBLANK_ON_MASK << ARS_VT_VBLANK_VT_VBLANK_ON_SHIFT)
#define ARS_VT_VBLANK_VT_VBLANK_OFF_SHIFT            0 /* inferred */
#define ARS_VT_VBLANK_VT_VBLANK_OFF_MASK             0xfffu
#define ARS_VT_VBLANK_VT_VBLANK_OFF_INPLACE (ARS_VT_VBLANK_VT_VBLANK_OFF_MASK << ARS_VT_VBLANK_VT_VBLANK_OFF_SHIFT)

/*
 * Horizontal blank window. NT and the firmware write ON as the blank start less
 * 5 (wrapped by the total) and OFF as the blank end less 3; Linux writes ON as
 * the blank start.
 */
/* vt_hblank: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_VT_HBLANK_OFFSET                         0x10014u /* inferred */
#define ARS_VT_HBLANK_VT_HBLANK_ON_SHIFT             12 /* inferred */
#define ARS_VT_HBLANK_VT_HBLANK_ON_MASK              0xfffu
#define ARS_VT_HBLANK_VT_HBLANK_ON_INPLACE (ARS_VT_HBLANK_VT_HBLANK_ON_MASK << ARS_VT_HBLANK_VT_HBLANK_ON_SHIFT)
#define ARS_VT_HBLANK_VT_HBLANK_OFF_SHIFT            0 /* inferred */
#define ARS_VT_HBLANK_VT_HBLANK_OFF_MASK             0xfffu
#define ARS_VT_HBLANK_VT_HBLANK_OFF_INPLACE (ARS_VT_HBLANK_VT_HBLANK_OFF_MASK << ARS_VT_HBLANK_VT_HBLANK_OFF_SHIFT)

/*
 * Polarity and blanking flags of the timing signals. The mode set writes bit 2
 * for the horizontal flag of the mode record and bit 0 for the vertical one,
 * after writing 0x40 and then 0. The power-state routine edits bits 1 and 3 for
 * the standby, suspend and off states (bit 1 for standby, bit 3 for suspend,
 * both for the rest); which signal each gates is inferred from the VESA states,
 * bit 1 horizontal and bit 3 vertical. The retrace handler sets bit 6 in stereo
 * modes and clears it again when free-running. The firmware self-test walks
 * bits 6:0. The firmware does not write the register.
 */
/* vt_flags: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set, power-state and retrace routines
 */
#define ARS_VT_FLAGS_OFFSET                          0x10018u /* inferred */
#define ARS_VT_FLAGS_BIT6_SHIFT                      6 /* unknown */
#define ARS_VT_FLAGS_BIT6_MASK                       0x1u
#define ARS_VT_FLAGS_BIT6_INPLACE (ARS_VT_FLAGS_BIT6_MASK << ARS_VT_FLAGS_BIT6_SHIFT)
#define ARS_VT_FLAGS_VSYNC_OFF_SHIFT                 3 /* inferred */
#define ARS_VT_FLAGS_VSYNC_OFF_MASK                  0x1u
#define ARS_VT_FLAGS_VSYNC_OFF_INPLACE (ARS_VT_FLAGS_VSYNC_OFF_MASK << ARS_VT_FLAGS_VSYNC_OFF_SHIFT)
#define ARS_VT_FLAGS_HDRV_INVERT_SHIFT               2 /* inferred */
#define ARS_VT_FLAGS_HDRV_INVERT_MASK                0x1u
#define ARS_VT_FLAGS_HDRV_INVERT_INPLACE (ARS_VT_FLAGS_HDRV_INVERT_MASK << ARS_VT_FLAGS_HDRV_INVERT_SHIFT)
#define ARS_VT_FLAGS_HSYNC_OFF_SHIFT                 1 /* inferred */
#define ARS_VT_FLAGS_HSYNC_OFF_MASK                  0x1u
#define ARS_VT_FLAGS_HSYNC_OFF_INPLACE (ARS_VT_FLAGS_HSYNC_OFF_MASK << ARS_VT_FLAGS_HSYNC_OFF_SHIFT)
#define ARS_VT_FLAGS_VDRV_INVERT_SHIFT               0 /* inferred */
#define ARS_VT_FLAGS_VDRV_INVERT_MASK                0x1u
#define ARS_VT_FLAGS_VDRV_INVERT_INPLACE (ARS_VT_FLAGS_VDRV_INVERT_MASK << ARS_VT_FLAGS_VDRV_INVERT_SHIFT)

/*
 * Frame-to-field and frame-lock line. The NT miniport writes the blank-start
 * line in 23:12 and 0xfff in 11:0 for a mono mode, and the blank-start line in
 * both halves for a stereo mode. Linux does not write it. The retrace handler
 * reads it in stereo and discards the value; the read may have a side effect.
 * Which half is which is inferred from the order of the names in the Linux
 * header comment.
 */
/* vt_f2rf_lock: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set and retrace routines */
#define ARS_VT_F2RF_LOCK_OFFSET                      0x1001cu /* inferred */
#define ARS_VT_F2RF_LOCK_F2RF_Y_SHIFT                12 /* inferred */
#define ARS_VT_F2RF_LOCK_F2RF_Y_MASK                 0xfffu
#define ARS_VT_F2RF_LOCK_F2RF_Y_INPLACE (ARS_VT_F2RF_LOCK_F2RF_Y_MASK << ARS_VT_F2RF_LOCK_F2RF_Y_SHIFT)
#define ARS_VT_F2RF_LOCK_FRAMELOCK_Y_SHIFT           0 /* inferred */
#define ARS_VT_F2RF_LOCK_FRAMELOCK_Y_MASK            0xfffu
#define ARS_VT_F2RF_LOCK_FRAMELOCK_Y_INPLACE (ARS_VT_F2RF_LOCK_FRAMELOCK_Y_MASK << ARS_VT_F2RF_LOCK_FRAMELOCK_Y_SHIFT)

/*
 * Scan-line interrupt lines 1 (11:0) and 0 (23:12). The NT miniport parks both
 * at 0xfff and then arms line 0 at the screen height less an allowance, and
 * line 1 at the height plus one (plus two with flat-panel timing). The
 * firmware's hand-over routine parks both and writes 0x1fff. No shipped handler
 * for these interrupts was found.
 */
/* vt_intr01: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for Windows
 * NT 4.0, display miniport, interrupt routines; SGI Visual Workstation PROM
 * 1.1005, display start-up code */
#define ARS_VT_INTR01_OFFSET                         0x10020u /* inferred */
#define ARS_VT_INTR01_INTR0_Y_SHIFT                  12 /* inferred */
#define ARS_VT_INTR01_INTR0_Y_MASK                   0xfffu
#define ARS_VT_INTR01_INTR0_Y_INPLACE (ARS_VT_INTR01_INTR0_Y_MASK << ARS_VT_INTR01_INTR0_Y_SHIFT)
#define ARS_VT_INTR01_INTR1_Y_SHIFT                  0 /* inferred */
#define ARS_VT_INTR01_INTR1_Y_MASK                   0xfffu
#define ARS_VT_INTR01_INTR1_Y_INPLACE (ARS_VT_INTR01_INTR1_Y_MASK << ARS_VT_INTR01_INTR1_Y_SHIFT)

/*
 * The same for scan-line interrupts 3 (11:0) and 2 (23:12). The NT miniport
 * accepts an interrupt number below 4 and a line below 4096.
 */
/* vt_intr23: rw32, reset unknown; SGI Cobalt Graphics Driver 5.2.4 for Windows
 * NT 4.0, display miniport, interrupt routines */
#define ARS_VT_INTR23_OFFSET                         0x10024u /* inferred */
#define ARS_VT_INTR23_INTR2_Y_SHIFT                  12 /* inferred */
#define ARS_VT_INTR23_INTR2_Y_MASK                   0xfffu
#define ARS_VT_INTR23_INTR2_Y_INPLACE (ARS_VT_INTR23_INTR2_Y_MASK << ARS_VT_INTR23_INTR2_Y_SHIFT)
#define ARS_VT_INTR23_INTR3_Y_SHIFT                  0 /* inferred */
#define ARS_VT_INTR23_INTR3_Y_MASK                   0xfffu
#define ARS_VT_INTR23_INTR3_Y_INPLACE (ARS_VT_INTR23_INTR3_Y_MASK << ARS_VT_INTR23_INTR3_Y_SHIFT)

/*
 * Flat-panel horizontal drive window. With a panel the NT miniport sets OFF to
 * the width; the firmware writes the panel's value for panel id 5 and leaves
 * the low 12 bits at the width.
 */
/* fp_hdrv: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation
 * PROM 1.1005, display start-up code */
#define ARS_FP_HDRV_OFFSET                           0x10028u /* inferred */
#define ARS_FP_HDRV_FP_HDRV_ON_SHIFT                 12 /* inferred */
#define ARS_FP_HDRV_FP_HDRV_ON_MASK                  0xfffu
#define ARS_FP_HDRV_FP_HDRV_ON_INPLACE (ARS_FP_HDRV_FP_HDRV_ON_MASK << ARS_FP_HDRV_FP_HDRV_ON_SHIFT)
#define ARS_FP_HDRV_FP_HDRV_OFF_SHIFT                0 /* inferred */
#define ARS_FP_HDRV_FP_HDRV_OFF_MASK                 0xfffu
#define ARS_FP_HDRV_FP_HDRV_OFF_INPLACE (ARS_FP_HDRV_FP_HDRV_OFF_MASK << ARS_FP_HDRV_FP_HDRV_OFF_SHIFT)

/*
 * Flat-panel vertical drive window. ON is 1 and OFF is the row count plus one,
 * so a 1024-row panel leaves 0x401 in the low 12 bits, which the Windows 2000
 * drivers read back to recognise a panel. Without a panel OFF is 0.
 */
/* fp_vdrv: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation
 * PROM 1.1005, display start-up code */
#define ARS_FP_VDRV_OFFSET                           0x1002cu /* inferred */
#define ARS_FP_VDRV_FP_VDRV_ON_SHIFT                 12 /* inferred */
#define ARS_FP_VDRV_FP_VDRV_ON_MASK                  0xfffu
#define ARS_FP_VDRV_FP_VDRV_ON_INPLACE (ARS_FP_VDRV_FP_VDRV_ON_MASK << ARS_FP_VDRV_FP_VDRV_ON_SHIFT)
#define ARS_FP_VDRV_FP_VDRV_OFF_SHIFT                0 /* inferred */
#define ARS_FP_VDRV_FP_VDRV_OFF_MASK                 0xfffu
#define ARS_FP_VDRV_FP_VDRV_OFF_INPLACE (ARS_FP_VDRV_FP_VDRV_OFF_MASK << ARS_FP_VDRV_FP_VDRV_OFF_SHIFT)

/*
 * Flat-panel data enable window. The NT miniport writes 0xffffff; the firmware
 * writes the panel's value, 0x640 in 11:0 for panel id 5.
 */
/* fp_de: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine; SGI Visual Workstation
 * PROM 1.1005, display start-up code */
#define ARS_FP_DE_OFFSET                             0x10030u /* inferred */
#define ARS_FP_DE_FP_DE_ON_SHIFT                     12 /* inferred */
#define ARS_FP_DE_FP_DE_ON_MASK                      0xfffu
#define ARS_FP_DE_FP_DE_ON_INPLACE (ARS_FP_DE_FP_DE_ON_MASK << ARS_FP_DE_FP_DE_ON_SHIFT)
#define ARS_FP_DE_FP_DE_OFF_SHIFT                    0 /* inferred */
#define ARS_FP_DE_FP_DE_OFF_MASK                     0xfffu
#define ARS_FP_DE_FP_DE_OFF_INPLACE (ARS_FP_DE_FP_DE_OFF_MASK << ARS_FP_DE_FP_DE_OFF_SHIFT)

/*
 * Internal horizontal pixel enable window. ON is the blank end less 19 (wrapped
 * by the total) and OFF is ON plus the width less 2 (wrapped). Software writes
 * 0xffffff first and the real value last.
 */
/* vt_hpixen: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_HPIXEN_OFFSET                         0x10034u /* inferred */
#define ARS_VT_HPIXEN_VT_HPIXEN_ON_SHIFT             12 /* inferred */
#define ARS_VT_HPIXEN_VT_HPIXEN_ON_MASK              0xfffu
#define ARS_VT_HPIXEN_VT_HPIXEN_ON_INPLACE (ARS_VT_HPIXEN_VT_HPIXEN_ON_MASK << ARS_VT_HPIXEN_VT_HPIXEN_ON_SHIFT)
#define ARS_VT_HPIXEN_VT_HPIXEN_OFF_SHIFT            0 /* inferred */
#define ARS_VT_HPIXEN_VT_HPIXEN_OFF_MASK             0xfffu
#define ARS_VT_HPIXEN_VT_HPIXEN_OFF_INPLACE (ARS_VT_HPIXEN_VT_HPIXEN_OFF_MASK << ARS_VT_HPIXEN_VT_HPIXEN_OFF_SHIFT)

/*
 * Internal vertical pixel enable window. ON is the vertical blank end and OFF
 * the vertical blank start.
 */
/* vt_vpixen: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_VPIXEN_OFFSET                         0x10038u /* inferred */
#define ARS_VT_VPIXEN_VT_VPIXEN_ON_SHIFT             12 /* inferred */
#define ARS_VT_VPIXEN_VT_VPIXEN_ON_MASK              0xfffu
#define ARS_VT_VPIXEN_VT_VPIXEN_ON_INPLACE (ARS_VT_VPIXEN_VT_VPIXEN_ON_MASK << ARS_VT_VPIXEN_VT_VPIXEN_ON_SHIFT)
#define ARS_VT_VPIXEN_VT_VPIXEN_OFF_SHIFT            0 /* inferred */
#define ARS_VT_VPIXEN_VT_VPIXEN_OFF_MASK             0xfffu
#define ARS_VT_VPIXEN_VT_VPIXEN_OFF_INPLACE (ARS_VT_VPIXEN_VT_VPIXEN_OFF_MASK << ARS_VT_VPIXEN_VT_VPIXEN_OFF_SHIFT)

/*
 * Colour-map write window, horizontal: the blank start and end (Linux writes
 * the end less 3).
 */
/* vt_hcmap: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_HCMAP_OFFSET                          0x1003cu /* inferred */
#define ARS_VT_HCMAP_VT_HCMAP_ON_SHIFT               12 /* inferred */
#define ARS_VT_HCMAP_VT_HCMAP_ON_MASK                0xfffu
#define ARS_VT_HCMAP_VT_HCMAP_ON_INPLACE (ARS_VT_HCMAP_VT_HCMAP_ON_MASK << ARS_VT_HCMAP_VT_HCMAP_ON_SHIFT)
#define ARS_VT_HCMAP_VT_HCMAP_OFF_SHIFT              0 /* inferred */
#define ARS_VT_HCMAP_VT_HCMAP_OFF_MASK               0xfffu
#define ARS_VT_HCMAP_VT_HCMAP_OFF_INPLACE (ARS_VT_HCMAP_VT_HCMAP_OFF_MASK << ARS_VT_HCMAP_VT_HCMAP_OFF_SHIFT)

/*
 * Colour-map write window, vertical: the blank start and end.
 */
/* vt_vcmap: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, mode-set routine */
#define ARS_VT_VCMAP_OFFSET                          0x10040u /* inferred */
#define ARS_VT_VCMAP_VT_VCMAP_ON_SHIFT               12 /* inferred */
#define ARS_VT_VCMAP_VT_VCMAP_ON_MASK                0xfffu
#define ARS_VT_VCMAP_VT_VCMAP_ON_INPLACE (ARS_VT_VCMAP_VT_VCMAP_ON_MASK << ARS_VT_VCMAP_VT_VCMAP_ON_SHIFT)
#define ARS_VT_VCMAP_VT_VCMAP_OFF_SHIFT              0 /* inferred */
#define ARS_VT_VCMAP_VT_VCMAP_OFF_MASK               0xfffu
#define ARS_VT_VCMAP_VT_VCMAP_OFF_INPLACE (ARS_VT_VCMAP_VT_VCMAP_OFF_MASK << ARS_VT_VCMAP_VT_VCMAP_OFF_SHIFT)

/*
 * Start position for the per-window id fetch. Y is the vertical blank start
 * less the end less 1, a small negative number written as 12-bit two's
 * complement; X is the horizontal blank end less 20, wrapped by the total. The
 * Windows 2000 boot video driver writes 0xffffffff.
 */
/* did_start_xy: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_DID_START_XY_OFFSET                      0x10044u /* inferred */
#define ARS_DID_START_XY_DID_STARTY_SHIFT            12 /* inferred */
#define ARS_DID_START_XY_DID_STARTY_MASK             0xfffu
#define ARS_DID_START_XY_DID_STARTY_INPLACE (ARS_DID_START_XY_DID_STARTY_MASK << ARS_DID_START_XY_DID_STARTY_SHIFT)
#define ARS_DID_START_XY_DID_STARTX_SHIFT            0 /* inferred */
#define ARS_DID_START_XY_DID_STARTX_MASK             0xfffu
#define ARS_DID_START_XY_DID_STARTX_INPLACE (ARS_DID_START_XY_DID_STARTX_MASK << ARS_DID_START_XY_DID_STARTX_SHIFT)

/*
 * Start position for the cursor. Y is one more than the id fetch's Y; X is the
 * horizontal blank end less 54 (the Linux header calls 54 the cursor magic
 * number), wrapped by the total.
 */
/* crs_start_xy: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_CRS_START_XY_OFFSET                      0x10048u /* inferred */
#define ARS_CRS_START_XY_CRS_STARTY_SHIFT            12 /* inferred */
#define ARS_CRS_START_XY_CRS_STARTY_MASK             0xfffu
#define ARS_CRS_START_XY_CRS_STARTY_INPLACE (ARS_CRS_START_XY_CRS_STARTY_MASK << ARS_CRS_START_XY_CRS_STARTY_SHIFT)
#define ARS_CRS_START_XY_CRS_STARTX_SHIFT            0 /* inferred */
#define ARS_CRS_START_XY_CRS_STARTX_MASK             0xfffu
#define ARS_CRS_START_XY_CRS_STARTX_INPLACE (ARS_CRS_START_XY_CRS_STARTX_MASK << ARS_CRS_START_XY_CRS_STARTX_SHIFT)

/*
 * Start position for the video capture. Y is the same as the id fetch's; X is
 * the horizontal blank end less 4 (not wrapped).
 */
/* vc_start_xy: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_VC_START_XY_OFFSET                       0x1004cu /* inferred */
#define ARS_VC_START_XY_VC_STARTY_SHIFT              12 /* inferred */
#define ARS_VC_START_XY_VC_STARTY_MASK               0xfffu
#define ARS_VC_START_XY_VC_STARTY_INPLACE (ARS_VC_START_XY_VC_STARTY_MASK << ARS_VC_START_XY_VC_STARTY_SHIFT)
#define ARS_VC_START_XY_VC_STARTX_SHIFT              0 /* inferred */
#define ARS_VC_START_XY_VC_STARTX_MASK               0xfffu
#define ARS_VC_START_XY_VC_STARTX_INPLACE (ARS_VC_START_XY_VC_STARTX_MASK << ARS_VC_START_XY_VC_STARTX_SHIFT)

/*
 * Overlay plane width. Whole 512-byte tile columns in 12:5 and the remainder in
 * 32-byte units in 4:0; FIFO reset in bit 13 is pulsed set then clear. The
 * overlay is always 8 bits per pixel. The NT miniport writes the full width
 * then pulses; the firmware writes the FIFO reset bit alone, then 0, with the
 * width zero. The firmware self-test walks bits 13:0.
 */
/* ovr_width_tile: rw32, reset unknown; Linux frame buffer driver for the
 * display back end, register block header and driver; SGI Cobalt Graphics
 * Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI
 * Visual Workstation PROM 1.1005, display start-up code */
#define ARS_OVR_WIDTH_TILE_OFFSET                    0x20000u /* inferred */
#define ARS_OVR_WIDTH_TILE_OVR_FIFO_RESET_SHIFT      13 /* inferred */
#define ARS_OVR_WIDTH_TILE_OVR_FIFO_RESET_MASK       0x1u
#define ARS_OVR_WIDTH_TILE_OVR_FIFO_RESET_INPLACE (ARS_OVR_WIDTH_TILE_OVR_FIFO_RESET_MASK << ARS_OVR_WIDTH_TILE_OVR_FIFO_RESET_SHIFT)
#define ARS_OVR_WIDTH_TILE_OVR_WIDTH_TILE_SHIFT      5 /* inferred */
#define ARS_OVR_WIDTH_TILE_OVR_WIDTH_TILE_MASK       0xffu
#define ARS_OVR_WIDTH_TILE_OVR_WIDTH_TILE_INPLACE (ARS_OVR_WIDTH_TILE_OVR_WIDTH_TILE_MASK << ARS_OVR_WIDTH_TILE_OVR_WIDTH_TILE_SHIFT)
#define ARS_OVR_WIDTH_TILE_OVR_RHS_SHIFT             0 /* inferred */
#define ARS_OVR_WIDTH_TILE_OVR_RHS_MASK              0x1fu
#define ARS_OVR_WIDTH_TILE_OVR_RHS_INPLACE (ARS_OVR_WIDTH_TILE_OVR_RHS_MASK << ARS_OVR_WIDTH_TILE_OVR_RHS_SHIFT)

/*
 * Live copy of the overlay control word. The NT miniport, the firmware and the
 * Windows 2000 frame buffer driver poll bit 0 with a byte load until it clears
 * (the firmware polls it until it reads set); the NT 4.0 HAL reads it as a
 * dword. Bit 0 must follow its shadow within the 200 ms the drivers allow.
 */
/* ovr_inhwctrl: r32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, overlay stop routine */
#define ARS_OVR_INHWCTRL_OFFSET                      0x20004u /* inferred */
#define ARS_OVR_INHWCTRL_OVR_DMA_ENABLE_SHIFT        0 /* inferred */
#define ARS_OVR_INHWCTRL_OVR_DMA_ENABLE_MASK         0x1u
#define ARS_OVR_INHWCTRL_OVR_DMA_ENABLE_INPLACE (ARS_OVR_INHWCTRL_OVR_DMA_ENABLE_MASK << ARS_OVR_INHWCTRL_OVR_DMA_ENABLE_SHIFT)

/*
 * Overlay control, the shadow word taken at the start of vertical blank. Bit 0
 * enables fetch; the rest is the overlay's tile-list pointer. The miniport
 * loads the pointer without bit 0; the display driver writes the pointer with
 * bit 0 set to start the fetch and the pointer alone to stop it. The firmware
 * self-test walks bits 31:5 and 1:0.
 */
/* ovr_control: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, display driver overlay routines */
#define ARS_OVR_CONTROL_OFFSET                       0x20008u /* inferred */
#define ARS_OVR_CONTROL_OVR_TILE_PTR_SHIFT           9 /* inferred */
#define ARS_OVR_CONTROL_OVR_TILE_PTR_MASK            0x7fffffu
#define ARS_OVR_CONTROL_OVR_TILE_PTR_INPLACE (ARS_OVR_CONTROL_OVR_TILE_PTR_MASK << ARS_OVR_CONTROL_OVR_TILE_PTR_SHIFT)
#define ARS_OVR_CONTROL_OVR_DMA_ENABLE_SHIFT         0 /* inferred */
#define ARS_OVR_CONTROL_OVR_DMA_ENABLE_MASK          0x1u
#define ARS_OVR_CONTROL_OVR_DMA_ENABLE_INPLACE (ARS_OVR_CONTROL_OVR_DMA_ENABLE_MASK << ARS_OVR_CONTROL_OVR_DMA_ENABLE_SHIFT)

/*
 * Normal plane width and depth. Whole tile columns in 12:5 and the remainder in
 * 32-byte units in 4:0, depth in 14:13, FIFO reset in bit 15 pulsed. A tile is
 * 512 bytes by 128 lines, so a 32-bit mode has 128 pixels per tile column and a
 * 16-bit mode 256. The NT miniport writes the value twice around the reset
 * pulse; the firmware does the same. Linux, NT and the O2 display back end page
 * agree. The firmware self-test walks bits 15:0.
 */
/* frm_size_tile: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, mode-set routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_FRM_SIZE_TILE_OFFSET                     0x30000u /* established */
#define ARS_FRM_SIZE_TILE_FRM_FIFO_RESET_SHIFT       15 /* established */
#define ARS_FRM_SIZE_TILE_FRM_FIFO_RESET_MASK        0x1u
#define ARS_FRM_SIZE_TILE_FRM_FIFO_RESET_INPLACE (ARS_FRM_SIZE_TILE_FRM_FIFO_RESET_MASK << ARS_FRM_SIZE_TILE_FRM_FIFO_RESET_SHIFT)
#define ARS_FRM_SIZE_TILE_FRM_DEPTH_SHIFT            13 /* established */
#define ARS_FRM_SIZE_TILE_FRM_DEPTH_MASK             0x3u
#define ARS_FRM_SIZE_TILE_FRM_DEPTH_INPLACE (ARS_FRM_SIZE_TILE_FRM_DEPTH_MASK << ARS_FRM_SIZE_TILE_FRM_DEPTH_SHIFT)
#define ARS_FRM_SIZE_TILE_FRM_WIDTH_TILE_SHIFT       5 /* established */
#define ARS_FRM_SIZE_TILE_FRM_WIDTH_TILE_MASK        0xffu
#define ARS_FRM_SIZE_TILE_FRM_WIDTH_TILE_INPLACE (ARS_FRM_SIZE_TILE_FRM_WIDTH_TILE_MASK << ARS_FRM_SIZE_TILE_FRM_WIDTH_TILE_SHIFT)
#define ARS_FRM_SIZE_TILE_FRM_RHS_SHIFT              0 /* established */
#define ARS_FRM_SIZE_TILE_FRM_RHS_MASK               0x1fu
#define ARS_FRM_SIZE_TILE_FRM_RHS_INPLACE (ARS_FRM_SIZE_TILE_FRM_RHS_MASK << ARS_FRM_SIZE_TILE_FRM_RHS_SHIFT)

/*
 * Normal plane height in lines, in 31:16. The firmware self-test walks only
 * those bits.
 */
/* frm_size_pixel: rw32, reset unknown; Linux frame buffer driver for the
 * display back end, register block header and driver; SGI Cobalt Graphics
 * Driver 5.2.4 for Windows NT 4.0, display miniport, mode-set routine; SGI
 * Visual Workstation PROM 1.1005, display start-up code */
#define ARS_FRM_SIZE_PIXEL_OFFSET                    0x30004u /* established */
#define ARS_FRM_SIZE_PIXEL_FB_HEIGHT_PIX_SHIFT       16 /* established */
#define ARS_FRM_SIZE_PIXEL_FB_HEIGHT_PIX_MASK        0xffffu
#define ARS_FRM_SIZE_PIXEL_FB_HEIGHT_PIX_INPLACE (ARS_FRM_SIZE_PIXEL_FB_HEIGHT_PIX_MASK << ARS_FRM_SIZE_PIXEL_FB_HEIGHT_PIX_SHIFT)

/*
 * Live copy of the normal plane control word. Completion of a page flip is this
 * word matching the value written; stop waits for bit 0 to clear. Software
 * reads bit 0 with a byte load in the NT miniport, the Windows 2000 frame
 * buffer driver and the firmware, and as a dword in the Windows 2000 boot video
 * driver and the NT 4.0 HAL; a model must answer a byte read with the matching
 * byte. The firmware waits forever for bit 0 to read set after it starts the
 * plane. Bit 0 must clear within 200 ms of its shadow and follow it at the
 * start of vertical blank.
 */
/* frm_inhwctrl: r32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, page-flip and stop routines */
#define ARS_FRM_INHWCTRL_OFFSET                      0x30008u /* inferred */
#define ARS_FRM_INHWCTRL_FRM_TILE_PTR_SHIFT          9 /* inferred */
#define ARS_FRM_INHWCTRL_FRM_TILE_PTR_MASK           0x7fffffu
#define ARS_FRM_INHWCTRL_FRM_TILE_PTR_INPLACE (ARS_FRM_INHWCTRL_FRM_TILE_PTR_MASK << ARS_FRM_INHWCTRL_FRM_TILE_PTR_SHIFT)
#define ARS_FRM_INHWCTRL_FRM_LINEAR_SHIFT            1 /* inferred */
#define ARS_FRM_INHWCTRL_FRM_LINEAR_MASK             0x1u
#define ARS_FRM_INHWCTRL_FRM_LINEAR_INPLACE (ARS_FRM_INHWCTRL_FRM_LINEAR_MASK << ARS_FRM_INHWCTRL_FRM_LINEAR_SHIFT)
#define ARS_FRM_INHWCTRL_FRM_DMA_ENABLE_SHIFT        0 /* inferred */
#define ARS_FRM_INHWCTRL_FRM_DMA_ENABLE_MASK         0x1u
#define ARS_FRM_INHWCTRL_FRM_DMA_ENABLE_INPLACE (ARS_FRM_INHWCTRL_FRM_DMA_ENABLE_MASK << ARS_FRM_INHWCTRL_FRM_DMA_ENABLE_SHIFT)

/*
 * Normal plane control, the shadow word taken at the start of vertical blank.
 * Bit 0 enables fetch; bit 1 selects the linear form, in which the pointer is
 * the frame buffer's physical address shifted right 9 and the line pitch is the
 * tile-column count rounded up times 512 bytes (Linux, both Windows 2000
 * drivers and the firmware set it, the SGI 5.2.4 miniport does not and gives
 * its tile list instead). In the tile-list form the pointer is the physical
 * address of a 512-byte aligned list of 16-bit entries. Both stop DMA by
 * clearing bit 0 and wait up to 20000 ten-microsecond steps for the live copy
 * to follow. The firmware self-test walks bits 31:9 and 1:0.
 */
/* frm_control: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, start, flip and stop routines; SGI
 * Visual Workstation PROM 1.1005, display start-up code */
#define ARS_FRM_CONTROL_OFFSET                       0x3000cu /* inferred */
#define ARS_FRM_CONTROL_FRM_TILE_PTR_SHIFT           9 /* inferred */
#define ARS_FRM_CONTROL_FRM_TILE_PTR_MASK            0x7fffffu
#define ARS_FRM_CONTROL_FRM_TILE_PTR_INPLACE (ARS_FRM_CONTROL_FRM_TILE_PTR_MASK << ARS_FRM_CONTROL_FRM_TILE_PTR_SHIFT)
#define ARS_FRM_CONTROL_FRM_LINEAR_SHIFT             1 /* inferred */
#define ARS_FRM_CONTROL_FRM_LINEAR_MASK              0x1u
#define ARS_FRM_CONTROL_FRM_LINEAR_INPLACE (ARS_FRM_CONTROL_FRM_LINEAR_MASK << ARS_FRM_CONTROL_FRM_LINEAR_SHIFT)
#define ARS_FRM_CONTROL_FRM_DMA_ENABLE_SHIFT         0 /* inferred */
#define ARS_FRM_CONTROL_FRM_DMA_ENABLE_MASK          0x1u
#define ARS_FRM_CONTROL_FRM_DMA_ENABLE_INPLACE (ARS_FRM_CONTROL_FRM_DMA_ENABLE_MASK << ARS_FRM_CONTROL_FRM_DMA_ENABLE_SHIFT)

/*
 * Live copy of the per-window id fetch control word. The NT miniport tests bit
 * 16 with a byte load of the third byte. The live bit is bit 16. Linux names
 * bit 0 and writes 0, which looks wrong against NT and the O2 display back end.
 */
/* did_inhwctrl: r32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, id fetch stop routine */
#define ARS_DID_INHWCTRL_OFFSET                      0x40000u /* inferred */
#define ARS_DID_INHWCTRL_DID_DMA_ENABLE_SHIFT        16 /* inferred */
#define ARS_DID_INHWCTRL_DID_DMA_ENABLE_MASK         0x1u
#define ARS_DID_INHWCTRL_DID_DMA_ENABLE_INPLACE (ARS_DID_INHWCTRL_DID_DMA_ENABLE_MASK << ARS_DID_INHWCTRL_DID_DMA_ENABLE_SHIFT)

/*
 * Per-window id fetch control, the shadow word. The NT miniport writes 0 at
 * mode set and clears bit 16 to stop; the firmware writes 0. The firmware
 * self-test walks all 32 bits.
 */
/* did_control: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display miniport, id fetch routines; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_DID_CONTROL_OFFSET                       0x40004u /* inferred */
#define ARS_DID_CONTROL_DID_DMA_ENABLE_SHIFT         16 /* inferred */
#define ARS_DID_CONTROL_DID_DMA_ENABLE_MASK          0x1u
#define ARS_DID_CONTROL_DID_DMA_ENABLE_INPLACE (ARS_DID_CONTROL_DID_DMA_ENABLE_MASK << ARS_DID_CONTROL_DID_DMA_ENABLE_SHIFT)

/*
 * Per-window mode table of 32 words, the window id table. Linux fills all 32
 * identically (8 bits per pixel as index, 16 as RGBA5, 32 as RGB8); the
 * firmware and the Windows 2000 boot video driver write 3 into all 32; the
 * Windows 2000 frame buffer driver writes only the first word, 3 for 8 bits per
 * pixel and 0x17 for 32. The SGI 5.2.4 miniport does not write the table.
 */
/* mode_regs: w32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Visual Workstation PROM 1.1005,
 * display start-up code; Windows 2000 boot video and frame buffer drivers */
#define ARS_MODE_REGS_OFFSET                         0x48000u /* inferred */
#define ARS_MODE_REGS_COUNT                          32
#define ARS_MODE_REGS_STRIDE                         4
#define ARS_MODE_REGS_WID_GAMMA_BYPASS_SHIFT         10 /* inferred */
#define ARS_MODE_REGS_WID_GAMMA_BYPASS_MASK          0x1u
#define ARS_MODE_REGS_WID_GAMMA_BYPASS_INPLACE (ARS_MODE_REGS_WID_GAMMA_BYPASS_MASK << ARS_MODE_REGS_WID_GAMMA_BYPASS_SHIFT)
#define ARS_MODE_REGS_WID_CM_BANK_SHIFT              5 /* inferred */
#define ARS_MODE_REGS_WID_CM_BANK_MASK               0x1fu
#define ARS_MODE_REGS_WID_CM_BANK_INPLACE (ARS_MODE_REGS_WID_CM_BANK_MASK << ARS_MODE_REGS_WID_CM_BANK_SHIFT)
#define ARS_MODE_REGS_WID_TYP_SHIFT                  2 /* inferred */
#define ARS_MODE_REGS_WID_TYP_MASK                   0x7u
#define ARS_MODE_REGS_WID_TYP_INPLACE (ARS_MODE_REGS_WID_TYP_MASK << ARS_MODE_REGS_WID_TYP_SHIFT)
#define ARS_MODE_REGS_WID_BUF_SHIFT                  0 /* inferred */
#define ARS_MODE_REGS_WID_BUF_MASK                   0x3u
#define ARS_MODE_REGS_WID_BUF_INPLACE (ARS_MODE_REGS_WID_BUF_MASK << ARS_MODE_REGS_WID_BUF_SHIFT)

/*
 * Colour map, 6144 entries in the Linux declaration (the O2 display back end
 * specification says 4608), one dword each. Both drivers write an entry as red
 * in 31:24, green in 23:16 and blue in 15:8. Before every 32nd entry the NT
 * miniport waits on the FIFO status; the Windows 2000 boot video driver waits
 * before every entry.
 */
/* cmap: w32, reset unknown; Linux frame buffer driver for the display back end,
 * register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, colour register routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_CMAP_OFFSET                              0x50000u /* inferred */
#define ARS_CMAP_COUNT                               6144
#define ARS_CMAP_STRIDE                              4
#define ARS_CMAP_RED_SHIFT                           24 /* inferred */
#define ARS_CMAP_RED_MASK                            0xffu
#define ARS_CMAP_RED_INPLACE (ARS_CMAP_RED_MASK << ARS_CMAP_RED_SHIFT)
#define ARS_CMAP_GREEN_SHIFT                         16 /* inferred */
#define ARS_CMAP_GREEN_MASK                          0xffu
#define ARS_CMAP_GREEN_INPLACE (ARS_CMAP_GREEN_MASK << ARS_CMAP_GREEN_SHIFT)
#define ARS_CMAP_BLUE_SHIFT                          8 /* inferred */
#define ARS_CMAP_BLUE_MASK                           0xffu
#define ARS_CMAP_BLUE_INPLACE (ARS_CMAP_BLUE_MASK << ARS_CMAP_BLUE_SHIFT)

/*
 * Colour map FIFO status. Linux waits until it reads non-zero and then writes
 * that many entries. The NT miniport, before every 32nd entry, spins until the
 * low six bits read zero or the value reaches 0x30. The firmware waits while
 * the low six bits are non-zero and above 0x10 is not reached. The Windows 2000
 * boot video driver needs the low six bits above 0x30. A value of 0x3f
 * satisfies all four readers and 0x40 fails the boot video driver. Meaning
 * inferred as a free-entry count.
 */
/* cm_fifo: r32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, colour register routine; SGI Visual
 * Workstation PROM 1.1005, display start-up code */
#define ARS_CM_FIFO_OFFSET                           0x58000u /* inferred */
#define ARS_CM_FIFO_FREE_ENTRIES_SHIFT               0 /* inferred */
#define ARS_CM_FIFO_FREE_ENTRIES_MASK                0x3fu
#define ARS_CM_FIFO_FREE_ENTRIES_INPLACE (ARS_CM_FIFO_FREE_ENTRIES_MASK << ARS_CM_FIFO_FREE_ENTRIES_SHIFT)

/*
 * Gamma map, 256 dwords. Linux, the firmware and the Windows 2000 boot video
 * driver write the identity ramp (i times 0x01010100). The NT miniport copies
 * dwords from the registry value and from the private gamma control request,
 * and reads the table back for that request. What the hardware does with the
 * words is inferred. The driver writes are established; the grade is for what
 * the hardware does with them.
 */
/* gmap: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display miniport, gamma routines; SGI Visual Workstation PROM
 * 1.1005, display start-up code */
#define ARS_GMAP_OFFSET                              0x60000u /* inferred */
#define ARS_GMAP_COUNT                               256
#define ARS_GMAP_STRIDE                              4
#define ARS_GMAP_RED_SHIFT                           24 /* inferred */
#define ARS_GMAP_RED_MASK                            0xffu
#define ARS_GMAP_RED_INPLACE (ARS_GMAP_RED_MASK << ARS_GMAP_RED_SHIFT)
#define ARS_GMAP_GREEN_SHIFT                         16 /* inferred */
#define ARS_GMAP_GREEN_MASK                          0xffu
#define ARS_GMAP_GREEN_INPLACE (ARS_GMAP_GREEN_MASK << ARS_GMAP_GREEN_SHIFT)
#define ARS_GMAP_BLUE_SHIFT                          8 /* inferred */
#define ARS_GMAP_BLUE_MASK                           0xffu
#define ARS_GMAP_BLUE_INPLACE (ARS_GMAP_BLUE_MASK << ARS_GMAP_BLUE_SHIFT)

/*
 * 10-bit gamma map, 1024 dwords. The same two NT paths write it when the size
 * selector is 10 or the registry value is 4096 bytes long. The field layout of
 * an entry is unknown. The driver writes are established; the grade is for what
 * the hardware does with them.
 */
/* gmap10: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Cobalt Graphics Driver
 * 5.2.4 for Windows NT 4.0, display miniport, gamma routines */
#define ARS_GMAP10_OFFSET                            0x68000u /* inferred */
#define ARS_GMAP10_COUNT                             1024
#define ARS_GMAP10_STRIDE                            4

/*
 * Cursor position. The display driver writes the pointer position less the hot
 * spot, masked to 12 bits, x in 11:0 and y in 27:16; a hidden cursor is parked
 * with the value of the cursor park constant. Every other writer only writes 0.
 * What the hardware does with the hot-spot-adjusted position is inferred from
 * the O2 display back end. The firmware self-test walks all 32 bits. The driver
 * writes are established; the grade is for what the hardware does with them.
 */
/* crs_pos: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM
 * 1.1005, display start-up code */
#define ARS_CRS_POS_OFFSET                           0x70000u /* inferred */
#define ARS_CRS_POS_Y_SHIFT                          16 /* inferred */
#define ARS_CRS_POS_Y_MASK                           0xfffu
#define ARS_CRS_POS_Y_INPLACE (ARS_CRS_POS_Y_MASK << ARS_CRS_POS_Y_SHIFT)
#define ARS_CRS_POS_X_SHIFT                          0 /* inferred */
#define ARS_CRS_POS_X_MASK                           0xfffu
#define ARS_CRS_POS_X_INPLACE (ARS_CRS_POS_X_MASK << ARS_CRS_POS_X_SHIFT)

/*
 * Cursor control. The display driver writes 1 to show and 0 to hide; the NT 4.0
 * HAL's stop clears bit 0 with a read-modify-write; every other writer writes
 * 0. The firmware self-test walks bits 1:0, so bit 1 is implemented and unused.
 * The driver writes are established; the grade is for what the hardware does
 * with them.
 */
/* crs_ctl: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM
 * 1.1005, display start-up code */
#define ARS_CRS_CTL_OFFSET                           0x70004u /* inferred */
#define ARS_CRS_CTL_BIT1_SHIFT                       1 /* unknown */
#define ARS_CRS_CTL_BIT1_MASK                        0x1u
#define ARS_CRS_CTL_BIT1_INPLACE (ARS_CRS_CTL_BIT1_MASK << ARS_CRS_CTL_BIT1_SHIFT)
#define ARS_CRS_CTL_SHOW_SHIFT                       0 /* inferred */
#define ARS_CRS_CTL_SHOW_MASK                        0x1u
#define ARS_CRS_CTL_SHOW_INPLACE (ARS_CRS_CTL_SHOW_MASK << ARS_CRS_CTL_SHOW_SHIFT)

/*
 * Cursor colours, three entries. The display driver writes each as red in
 * 31:24, green in 23:16 and blue in 15:8 (the colour list's entries 1 to 3 go
 * to these three words) and at start-up sets the second to 0 and the third to
 * 0xffffff00; the firmware writes 0xffffff00 and 0xff000000 to the first two.
 * The firmware self-test walks bits 31:8. The driver writes are established;
 * the grade is for what the hardware does with them.
 */
/* crs_cmap: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4 for
 * Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation PROM
 * 1.1005, display start-up code */
#define ARS_CRS_CMAP_OFFSET                          0x70008u /* inferred */
#define ARS_CRS_CMAP_COUNT                           3
#define ARS_CRS_CMAP_STRIDE                          4
#define ARS_CRS_CMAP_RED_SHIFT                       24 /* inferred */
#define ARS_CRS_CMAP_RED_MASK                        0xffu
#define ARS_CRS_CMAP_RED_INPLACE (ARS_CRS_CMAP_RED_MASK << ARS_CRS_CMAP_RED_SHIFT)
#define ARS_CRS_CMAP_GREEN_SHIFT                     16 /* inferred */
#define ARS_CRS_CMAP_GREEN_MASK                      0xffu
#define ARS_CRS_CMAP_GREEN_INPLACE (ARS_CRS_CMAP_GREEN_MASK << ARS_CRS_CMAP_GREEN_SHIFT)
#define ARS_CRS_CMAP_BLUE_SHIFT                      8 /* inferred */
#define ARS_CRS_CMAP_BLUE_MASK                       0xffu
#define ARS_CRS_CMAP_BLUE_INPLACE (ARS_CRS_CMAP_BLUE_MASK << ARS_CRS_CMAP_BLUE_SHIFT)

/*
 * Cursor glyph, 64 dwords: a 32 by 32 pointer, two dwords per row, 2 bits per
 * pixel packed from the top of the dword, 0 for transparent and 1 to 3 for the
 * entry of the cursor colour list. The display driver rewrites all 64 for a
 * pointer of at most 32 by 32 pixels and three colours and draws any other
 * pointer in software. Pixel x of row y is field px(x mod 16) of dword 2y + x
 * div 16, so px0 is the leftmost pixel of a dword and sits in bits 31:30. The
 * scan order and the colour index are inferred from the O2 display back end.
 * The driver writes are established; the grade is for what the hardware does
 * with them.
 */
/* crs_glyph: rw32, reset unknown; Linux frame buffer driver for the display
 * back end, register block header and driver; SGI Cobalt Graphics Driver 5.2.4
 * for Windows NT 4.0, display driver, pointer routines; SGI Visual Workstation
 * PROM 1.1005, display start-up code */
#define ARS_CRS_GLYPH_OFFSET                         0x78000u /* inferred */
#define ARS_CRS_GLYPH_COUNT                          64
#define ARS_CRS_GLYPH_STRIDE                         4
#define ARS_CRS_GLYPH_PX0_SHIFT                      30 /* inferred */
#define ARS_CRS_GLYPH_PX0_MASK                       0x3u
#define ARS_CRS_GLYPH_PX0_INPLACE (ARS_CRS_GLYPH_PX0_MASK << ARS_CRS_GLYPH_PX0_SHIFT)
#define ARS_CRS_GLYPH_PX1_SHIFT                      28 /* inferred */
#define ARS_CRS_GLYPH_PX1_MASK                       0x3u
#define ARS_CRS_GLYPH_PX1_INPLACE (ARS_CRS_GLYPH_PX1_MASK << ARS_CRS_GLYPH_PX1_SHIFT)
#define ARS_CRS_GLYPH_PX2_SHIFT                      26 /* inferred */
#define ARS_CRS_GLYPH_PX2_MASK                       0x3u
#define ARS_CRS_GLYPH_PX2_INPLACE (ARS_CRS_GLYPH_PX2_MASK << ARS_CRS_GLYPH_PX2_SHIFT)
#define ARS_CRS_GLYPH_PX3_SHIFT                      24 /* inferred */
#define ARS_CRS_GLYPH_PX3_MASK                       0x3u
#define ARS_CRS_GLYPH_PX3_INPLACE (ARS_CRS_GLYPH_PX3_MASK << ARS_CRS_GLYPH_PX3_SHIFT)
#define ARS_CRS_GLYPH_PX4_SHIFT                      22 /* inferred */
#define ARS_CRS_GLYPH_PX4_MASK                       0x3u
#define ARS_CRS_GLYPH_PX4_INPLACE (ARS_CRS_GLYPH_PX4_MASK << ARS_CRS_GLYPH_PX4_SHIFT)
#define ARS_CRS_GLYPH_PX5_SHIFT                      20 /* inferred */
#define ARS_CRS_GLYPH_PX5_MASK                       0x3u
#define ARS_CRS_GLYPH_PX5_INPLACE (ARS_CRS_GLYPH_PX5_MASK << ARS_CRS_GLYPH_PX5_SHIFT)
#define ARS_CRS_GLYPH_PX6_SHIFT                      18 /* inferred */
#define ARS_CRS_GLYPH_PX6_MASK                       0x3u
#define ARS_CRS_GLYPH_PX6_INPLACE (ARS_CRS_GLYPH_PX6_MASK << ARS_CRS_GLYPH_PX6_SHIFT)
#define ARS_CRS_GLYPH_PX7_SHIFT                      16 /* inferred */
#define ARS_CRS_GLYPH_PX7_MASK                       0x3u
#define ARS_CRS_GLYPH_PX7_INPLACE (ARS_CRS_GLYPH_PX7_MASK << ARS_CRS_GLYPH_PX7_SHIFT)
#define ARS_CRS_GLYPH_PX8_SHIFT                      14 /* inferred */
#define ARS_CRS_GLYPH_PX8_MASK                       0x3u
#define ARS_CRS_GLYPH_PX8_INPLACE (ARS_CRS_GLYPH_PX8_MASK << ARS_CRS_GLYPH_PX8_SHIFT)
#define ARS_CRS_GLYPH_PX9_SHIFT                      12 /* inferred */
#define ARS_CRS_GLYPH_PX9_MASK                       0x3u
#define ARS_CRS_GLYPH_PX9_INPLACE (ARS_CRS_GLYPH_PX9_MASK << ARS_CRS_GLYPH_PX9_SHIFT)
#define ARS_CRS_GLYPH_PX10_SHIFT                     10 /* inferred */
#define ARS_CRS_GLYPH_PX10_MASK                      0x3u
#define ARS_CRS_GLYPH_PX10_INPLACE (ARS_CRS_GLYPH_PX10_MASK << ARS_CRS_GLYPH_PX10_SHIFT)
#define ARS_CRS_GLYPH_PX11_SHIFT                     8 /* inferred */
#define ARS_CRS_GLYPH_PX11_MASK                      0x3u
#define ARS_CRS_GLYPH_PX11_INPLACE (ARS_CRS_GLYPH_PX11_MASK << ARS_CRS_GLYPH_PX11_SHIFT)
#define ARS_CRS_GLYPH_PX12_SHIFT                     6 /* inferred */
#define ARS_CRS_GLYPH_PX12_MASK                      0x3u
#define ARS_CRS_GLYPH_PX12_INPLACE (ARS_CRS_GLYPH_PX12_MASK << ARS_CRS_GLYPH_PX12_SHIFT)
#define ARS_CRS_GLYPH_PX13_SHIFT                     4 /* inferred */
#define ARS_CRS_GLYPH_PX13_MASK                      0x3u
#define ARS_CRS_GLYPH_PX13_INPLACE (ARS_CRS_GLYPH_PX13_MASK << ARS_CRS_GLYPH_PX13_SHIFT)
#define ARS_CRS_GLYPH_PX14_SHIFT                     2 /* inferred */
#define ARS_CRS_GLYPH_PX14_MASK                      0x3u
#define ARS_CRS_GLYPH_PX14_INPLACE (ARS_CRS_GLYPH_PX14_MASK << ARS_CRS_GLYPH_PX14_SHIFT)
#define ARS_CRS_GLYPH_PX15_SHIFT                     0 /* inferred */
#define ARS_CRS_GLYPH_PX15_MASK                      0x3u
#define ARS_CRS_GLYPH_PX15_INPLACE (ARS_CRS_GLYPH_PX15_MASK << ARS_CRS_GLYPH_PX15_SHIFT)

/*
 * Video capture control word 0. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0x00ffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_0: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_0_OFFSET                              0x80000u /* unknown */

/*
 * Video capture control word 1. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0x00ffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_1: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_1_OFFSET                              0x80004u /* unknown */

/*
 * Video capture control word 2. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0x1f, the only statement of how many bits are
 * implemented.
 */
/* vc_2: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_2_OFFSET                              0x80008u /* unknown */

/*
 * Video capture control word 3. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test does
 * not walk it.
 */
/* vc_3: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_3_OFFSET                              0x8000cu /* unknown */

/*
 * Video capture control word 4. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0xffffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_4: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_4_OFFSET                              0x80010u /* unknown */

/*
 * Video capture control word 5. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0xffffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_5: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_5_OFFSET                              0x80014u /* unknown */

/*
 * Video capture control word 6. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0xffffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_6: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_6_OFFSET                              0x80018u /* unknown */

/*
 * Video capture control word 7. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test does
 * not walk it.
 */
/* vc_7: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_7_OFFSET                              0x8001cu /* unknown */

/*
 * Video capture control word 8. Neither driver touches the capture block; the
 * Linux header names nine words. The video in and out ports raise interrupt
 * pins 16 to 19 of the system interrupt controller. The firmware self-test
 * walks the bits of mask 0xffffffff, the only statement of how many bits are
 * implemented.
 */
/* vc_8: rw32, reset unknown; Linux frame buffer driver for the display back
 * end, register block header and driver (name only); SGI Visual Workstation
 * PROM 1.1005, display start-up code (self-test masks) */
#define ARS_VC_8_OFFSET                              0x80020u /* unknown */

/*
 * Retrace latch in its own 32-bit window, wired to line 24 of the system
 * interrupt controller (latched). The retrace handler reads it, writes back the
 * bitwise complement of what it read and does not read it again; bit 1 set
 * means a frame has ended and is counted. The firmware's hand-over routine
 * writes the complement of the word the same way. That only clears the bits if
 * a written 0 clears a bit and a written 1 leaves it, so the register is
 * write-0-to-clear (inferred). What the other bits are is unknown. The name is
 * ours: the register has no public name.
 */
/* retrace_status: rw32, space retrace, reset unknown; SGI Cobalt Graphics
 * Driver 5.2.4 for Windows NT 4.0, display miniport, retrace interrupt routine;
 * SGI Visual Workstation PROM 1.1005, display start-up code */
#define ARS_RETRACE_STATUS_OFFSET                    0x0u /* inferred */
#define ARS_RETRACE_STATUS_FRAME_END_SHIFT           1 /* inferred */
#define ARS_RETRACE_STATUS_FRAME_END_MASK            0x1u
#define ARS_RETRACE_STATUS_FRAME_END_INPLACE (ARS_RETRACE_STATUS_FRAME_END_MASK << ARS_RETRACE_STATUS_FRAME_END_SHIFT)

/*
 * A 32-bit frame buffer pixel of the RGB8 type. The miniport's pixel format
 * table and the Windows 2000 frame buffer driver give red in the top byte,
 * green below it and blue in 15:8; the Linux frame buffer driver puts red in
 * the low byte, so the byte order is not settled for the hardware. The low byte
 * is not described. The colour map and gamma entries use the same red, green,
 * blue positions.
 */
/* word PIXEL_RGB8: 32 bits; SGI Cobalt Graphics Driver 5.2.4 for Windows NT
 * 4.0, display miniport pixel format table; Linux frame buffer driver */
#define ARS_PIXEL_RGB8_WIDTH                         32 /* inferred */
#define ARS_PIXEL_RGB8_RED_SHIFT                     24 /* inferred */
#define ARS_PIXEL_RGB8_RED_MASK                      0xffu
#define ARS_PIXEL_RGB8_RED_INPLACE (ARS_PIXEL_RGB8_RED_MASK << ARS_PIXEL_RGB8_RED_SHIFT)
#define ARS_PIXEL_RGB8_GREEN_SHIFT                   16 /* inferred */
#define ARS_PIXEL_RGB8_GREEN_MASK                    0xffu
#define ARS_PIXEL_RGB8_GREEN_INPLACE (ARS_PIXEL_RGB8_GREEN_MASK << ARS_PIXEL_RGB8_GREEN_SHIFT)
#define ARS_PIXEL_RGB8_BLUE_SHIFT                    8 /* inferred */
#define ARS_PIXEL_RGB8_BLUE_MASK                     0xffu
#define ARS_PIXEL_RGB8_BLUE_INPLACE (ARS_PIXEL_RGB8_BLUE_MASK << ARS_PIXEL_RGB8_BLUE_SHIFT)

/*
 * A 16-bit frame buffer pixel of the RGBA5 type: five bits of red on top, then
 * green and blue, and one alpha bit at the bottom. The miniport's format table
 * and the Linux driver agree on these masks.
 */
/* word PIXEL_RGBA5: 16 bits; SGI Cobalt Graphics Driver 5.2.4 for Windows NT
 * 4.0, display miniport pixel format table; Linux frame buffer driver */
#define ARS_PIXEL_RGBA5_WIDTH                        16 /* inferred */
#define ARS_PIXEL_RGBA5_RED_SHIFT                    11 /* inferred */
#define ARS_PIXEL_RGBA5_RED_MASK                     0x1fu
#define ARS_PIXEL_RGBA5_RED_INPLACE (ARS_PIXEL_RGBA5_RED_MASK << ARS_PIXEL_RGBA5_RED_SHIFT)
#define ARS_PIXEL_RGBA5_GREEN_SHIFT                  6 /* inferred */
#define ARS_PIXEL_RGBA5_GREEN_MASK                   0x1fu
#define ARS_PIXEL_RGBA5_GREEN_INPLACE (ARS_PIXEL_RGBA5_GREEN_MASK << ARS_PIXEL_RGBA5_GREEN_SHIFT)
#define ARS_PIXEL_RGBA5_BLUE_SHIFT                   1 /* inferred */
#define ARS_PIXEL_RGBA5_BLUE_MASK                    0x1fu
#define ARS_PIXEL_RGBA5_BLUE_INPLACE (ARS_PIXEL_RGBA5_BLUE_MASK << ARS_PIXEL_RGBA5_BLUE_SHIFT)
#define ARS_PIXEL_RGBA5_ALPHA_SHIFT                  0 /* inferred */
#define ARS_PIXEL_RGBA5_ALPHA_MASK                   0x1u
#define ARS_PIXEL_RGBA5_ALPHA_INPLACE (ARS_PIXEL_RGBA5_ALPHA_MASK << ARS_PIXEL_RGBA5_ALPHA_SHIFT)

/*
 * One entry of a tile list in main memory: the tile's first 4 KB page frame
 * number shifted right 4 and truncated to 16 bits, so bit 15 is physical
 * address bit 31 and no bit is an attribute. All four routines that write a
 * list store exactly this. A list is 512-byte aligned, non-cached and (height +
 * 127) / 128 rows of one entry per 64 KB tile; lists 0 to 3 belong to the
 * normal plane (the current one flips by exclusive-or with 1) and list 4 to the
 * overlay. That the engine reads an entry this way is inferred from the O2
 * display back end, whose tile pointer is 16 bits. The driver writes are
 * established; the grade is for what the hardware does with them.
 */
/* word TILE_LIST_ENTRY: 16 bits; SGI Cobalt Graphics Driver 5.2.4 for Windows
 * NT 4.0, display miniport, frame start and flip routines */
#define ARS_TILE_LIST_ENTRY_WIDTH                    16 /* inferred */
#define ARS_TILE_LIST_ENTRY_TILE_NUMBER_SHIFT        0 /* inferred */
#define ARS_TILE_LIST_ENTRY_TILE_NUMBER_MASK         0xffffu
#define ARS_TILE_LIST_ENTRY_TILE_NUMBER_INPLACE (ARS_TILE_LIST_ENTRY_TILE_NUMBER_MASK << ARS_TILE_LIST_ENTRY_TILE_NUMBER_SHIFT)

#define ARS_BLOCK_SIZE_DECLARED                      0x80024u /* established */
/* established */
#define ARS_BLOCK_SIZE_LINUX                         0x1000000u
#define ARS_BLOCK_SIZE_MAPPED                        0x80020u /* established */
#define ARS_CHIPID_REAL_320                          0x2u /* inferred */
#define ARS_CLOCK_REF_KHZ                            0x6978u /* inferred */
#define ARS_CMAP_BANK_ENTRIES                        0x100u /* inferred */
#define ARS_COLOR_MAP_FIFO_FREE                      0x3fu /* inferred */
#define ARS_CRS_MAGIC                                0x36u /* inferred */
#define ARS_CURSOR_GLYPH_PIXELS_PER_DWORD            0x10u /* inferred */
#define ARS_CURSOR_GLYPH_PIXEL_BITS                  0x2u /* inferred */
#define ARS_CURSOR_GLYPH_ROWS                        0x20u /* inferred */
#define ARS_CURSOR_GLYPH_ROW_DWORDS                  0x2u /* inferred */
#define ARS_CURSOR_PARK_POS                          0xfe00fe0u /* inferred */
#define ARS_ID_VALUE                                 0x666u /* inferred */
#define ARS_NORMAL_TILE_LISTS                        0x4u /* established */
#define ARS_TILE_BYTES                               0x10000u /* established */
#define ARS_TILE_HEIGHT_LINES                        0x80u /* established */
#define ARS_TILE_LIST_ALIGN                          0x200u /* established */
#define ARS_TILE_WIDTH_BYTES                         0x200u /* established */

#endif /* ARS_GEN_H */
