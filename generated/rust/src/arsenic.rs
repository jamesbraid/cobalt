// @generated from devices/arsenic/registers.toml by defs-gen - do not edit, change the source
// SPDX-License-Identifier: BSD-3-Clause
// defs-gen 4
// input sha256 7af1e2c234fa5437a7357c991d8a1158ad7c0f3aa54c6d8f579a3017611e8ff6
// Arsenic display back end register block
#![allow(dead_code)]
#![allow(clippy::doc_markdown)]
#![cfg_attr(rustfmt, rustfmt_skip)]

pub mod enums {
    pub mod did_buffer_mode {
        pub const BOTH: u32 = 3; // inferred
        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        pub enum Value {
            Both,
            Other(u32),
        }
        impl Value {
            pub const fn from_raw(v: u32) -> Self {
                match v {
                    3 => Self::Both,
                    _ => Self::Other(v),
                }
            }
            pub const fn raw(self) -> u32 {
                match self {
                    Self::Both => 3,
                    Self::Other(v) => v,
                }
            }
        }
    }
    pub mod did_pixel_type {
        pub const INDEX8: u32 = 0; // inferred
        pub const INDEX12: u32 = 1; // inferred
        pub const RG3B2: u32 = 2; // inferred
        pub const RGB4: u32 = 3; // inferred
        pub const ARGB5: u32 = 4; // inferred
        pub const RGB8: u32 = 5; // inferred
        pub const RGBA5: u32 = 6; // inferred
        pub const RGB10: u32 = 7; // inferred
        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        pub enum Value {
            Index8,
            Index12,
            Rg3b2,
            Rgb4,
            Argb5,
            Rgb8,
            Rgba5,
            Rgb10,
            Other(u32),
        }
        impl Value {
            pub const fn from_raw(v: u32) -> Self {
                match v {
                    0 => Self::Index8,
                    1 => Self::Index12,
                    2 => Self::Rg3b2,
                    3 => Self::Rgb4,
                    4 => Self::Argb5,
                    5 => Self::Rgb8,
                    6 => Self::Rgba5,
                    7 => Self::Rgb10,
                    _ => Self::Other(v),
                }
            }
            pub const fn raw(self) -> u32 {
                match self {
                    Self::Index8 => 0,
                    Self::Index12 => 1,
                    Self::Rg3b2 => 2,
                    Self::Rgb4 => 3,
                    Self::Argb5 => 4,
                    Self::Rgb8 => 5,
                    Self::Rgba5 => 6,
                    Self::Rgb10 => 7,
                    Self::Other(v) => v,
                }
            }
        }
    }
    pub mod frame_depth {
        pub const BPP8: u32 = 0; // established
        pub const BPP16: u32 = 1; // established
        pub const BPP32: u32 = 2; // established
        pub const RESERVED: u32 = 3; // unknown
        #[derive(Debug, Clone, Copy, PartialEq, Eq)]
        pub enum Value {
            Bpp8,
            Bpp16,
            Bpp32,
            Reserved,
            Other(u32),
        }
        impl Value {
            pub const fn from_raw(v: u32) -> Self {
                match v {
                    0 => Self::Bpp8,
                    1 => Self::Bpp16,
                    2 => Self::Bpp32,
                    3 => Self::Reserved,
                    _ => Self::Other(v),
                }
            }
            pub const fn raw(self) -> u32 {
                match self {
                    Self::Bpp8 => 0,
                    Self::Bpp16 => 1,
                    Self::Bpp32 => 2,
                    Self::Reserved => 3,
                    Self::Other(v) => v,
                }
            }
        }
    }
}

/// General control and status. The mode-set routine of the NT miniport writes the value with the clock selector at 3 as its last step; Linux writes the s (inferred)
pub mod ctrlstat {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x0;
    pub mod pclksel { // bits 29:28, inferred
        pub const SHIFT: u32 = 28;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod bit26 { // bits 26:26, unknown
        pub const SHIFT: u32 = 26;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod bit25 { // bits 25:25, unknown
        pub const SHIFT: u32 = 25;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod sense_n { // bits 4:4, unknown
        pub const SHIFT: u32 = 4;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod chipid { // bits 3:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xf;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Dot clock PLL. Software writes M-1 and N-1. The output is 27 MHz times M over N, divided by 2 to the P; that reproduces the clock column of 26 of the  (inferred)
pub mod dotclock {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x4;
    pub mod run { // bits 20:20, inferred
        pub const SHIFT: u32 = 20;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod p { // bits 15:14, inferred
        pub const SHIFT: u32 = 14;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod n { // bits 13:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0x3f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod m { // bits 7:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// CRT I2C lines (DDC). One register is both the write and the read-back location. Writing a bit as 1 pulls the line low and writing 0 lets it float high (established)
pub mod i2c {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x8;
    pub mod scl { // bits 1:1, established
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod sda { // bits 0:0, established
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// System clock PLL. Neither driver, the firmware nor the HAL touches it; the Linux header names it as the system clock PLL control. Nothing shipped depe (unknown)
pub mod sysclk {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0xc;
}
/// Flat-panel I2C lines, the same layout and sense as the CRT register. The flat-panel status device answers at write address 0x70 and read address 0x71; (established)
pub mod i2cfp {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10;
    pub mod scl { // bits 1:1, established
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod sda { // bits 0:0, established
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Device id. The NT miniport compares the whole dword with the value in the ID constant and does not attach otherwise; the Windows 2000 drivers compare  (inferred)
pub mod id {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x14;
}
/// Output configuration. The NT miniport writes bits 7, 6, 5, 2 and 0, sets bit 0 when the mode asks for sync on green (the sync-on-green bit), always se (inferred)
pub mod config {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x18;
    pub mod ext_addr { // bits 7:7, inferred
        pub const SHIFT: u32 = 7;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod tilehist { // bits 6:6, inferred
        pub const SHIFT: u32 = 6;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod lendian { // bits 5:5, inferred
        pub const SHIFT: u32 = 5;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod fpenable { // bits 3:3, inferred
        pub const SHIFT: u32 = 3;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vdac_pblank { // bits 2:2, inferred
        pub const SHIFT: u32 = 2;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vdac_gsync { // bits 1:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vdac_enable { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Built-in self-test. Neither driver reads it. The firmware self-test writes 0, polls bit 31 for completion and reports a failure when any of bits 27:24 (inferred)
pub mod bist {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1c;
    pub mod done { // bits 31:31, inferred
        pub const SHIFT: u32 = 31;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod fail { // bits 27:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0xf;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Current dot coordinates. The live line is read from bits 23:12 (the scan-line poll); bit 31 is the timing counter freeze: it reads back what software  (inferred)
pub mod vt_xy {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10000;
    pub mod vt_freeze { // bits 31:31, inferred
        pub const SHIFT: u32 = 31;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod y { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod x { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Totals: horizontal total in 11:0 and vertical total in 23:12. Software writes 0 while it programs the other windows and the real totals afterwards. (inferred)
pub mod vt_xymax {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10004;
    pub mod vt_maxy { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_maxx { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Vertical sync window, in lines. (inferred)
pub mod vt_vsync {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10008;
    pub mod vt_vsync_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_vsync_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Horizontal sync window, in dots. (inferred)
pub mod vt_hsync {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1000c;
    pub mod vt_hsync_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_hsync_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Vertical blank window, in lines. (inferred)
pub mod vt_vblank {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10010;
    pub mod vt_vblank_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_vblank_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Horizontal blank window. NT and the firmware write ON as the blank start less 5 (wrapped by the total) and OFF as the blank end less 3; Linux writes O (inferred)
pub mod vt_hblank {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10014;
    pub mod vt_hblank_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_hblank_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Polarity and blanking flags of the timing signals. The mode set writes bit 2 for the horizontal flag of the mode record and bit 0 for the vertical one (inferred)
pub mod vt_flags {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10018;
    pub mod bit6 { // bits 6:6, unknown
        pub const SHIFT: u32 = 6;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vsync_off { // bits 3:3, inferred
        pub const SHIFT: u32 = 3;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod hdrv_invert { // bits 2:2, inferred
        pub const SHIFT: u32 = 2;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod hsync_off { // bits 1:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vdrv_invert { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Frame-to-field and frame-lock line. The NT miniport writes the blank-start line in 23:12 and 0xfff in 11:0 for a mono mode, and the blank-start line i (inferred)
pub mod vt_f2rf_lock {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1001c;
    pub mod f2rf_y { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod framelock_y { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Scan-line interrupt lines 1 (11:0) and 0 (23:12). The NT miniport parks both at 0xfff, then `CoSetMode` arms line 0 at the screen height less an allow (inferred)
pub mod vt_intr01 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10020;
    pub mod intr0_y { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod intr1_y { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// The same for scan-line interrupts 3 (11:0) and 2 (23:12). The NT miniport accepts an interrupt number below 4 and a line below 4096. (inferred)
pub mod vt_intr23 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10024;
    pub mod intr2_y { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod intr3_y { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Flat-panel horizontal drive window. With a panel the NT miniport sets OFF to the width; the firmware writes the panel's value for panel id 5 and leave (inferred)
pub mod fp_hdrv {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10028;
    pub mod fp_hdrv_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod fp_hdrv_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Flat-panel vertical drive window. ON is 1 and OFF is the row count plus one, so a 1024-row panel leaves 0x401 in the low 12 bits, which the Windows 20 (inferred)
pub mod fp_vdrv {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1002c;
    pub mod fp_vdrv_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod fp_vdrv_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Flat-panel data enable window. The NT miniport writes 0xffffff; the firmware writes the panel's value, 0x640 in 11:0 for panel id 5. (inferred)
pub mod fp_de {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10030;
    pub mod fp_de_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod fp_de_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Internal horizontal pixel enable window. ON is the blank end less 19 (wrapped by the total) and OFF is ON plus the width less 2 (wrapped). Software wr (inferred)
pub mod vt_hpixen {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10034;
    pub mod vt_hpixen_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_hpixen_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Internal vertical pixel enable window. ON is the vertical blank end and OFF the vertical blank start. (inferred)
pub mod vt_vpixen {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10038;
    pub mod vt_vpixen_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_vpixen_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Colour-map write window, horizontal: the blank start and end (Linux writes the end less 3). (inferred)
pub mod vt_hcmap {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1003c;
    pub mod vt_hcmap_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_hcmap_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Colour-map write window, vertical: the blank start and end. (inferred)
pub mod vt_vcmap {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10040;
    pub mod vt_vcmap_on { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vt_vcmap_off { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Start position for the per-window id fetch. Y is the vertical blank start less the end less 1, a small negative number written as 12-bit two's complem (inferred)
pub mod did_start_xy {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10044;
    pub mod did_starty { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod did_startx { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Start position for the cursor. Y is one more than the id fetch's Y; X is the horizontal blank end less 54 (the Linux header calls 54 the cursor magic  (inferred)
pub mod crs_start_xy {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x10048;
    pub mod crs_starty { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod crs_startx { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Start position for the video capture. Y is the same as the id fetch's; X is the horizontal blank end less 4 (not wrapped). (inferred)
pub mod vc_start_xy {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x1004c;
    pub mod vc_starty { // bits 23:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod vc_startx { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Overlay plane width. Whole 512-byte tile columns in 12:5 and the remainder in 32-byte units in 4:0; FIFO reset in bit 13 is pulsed set then clear. The (inferred)
#[allow(clippy::module_inception)]
pub mod ovr_width_tile {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x20000;
    pub mod ovr_fifo_reset { // bits 13:13, inferred
        pub const SHIFT: u32 = 13;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod ovr_width_tile { // bits 12:5, inferred
        pub const SHIFT: u32 = 5;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod ovr_rhs { // bits 4:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Live copy of the overlay control word. The NT miniport, the firmware and the Windows 2000 frame buffer driver poll bit 0 with a byte load until it cle (inferred)
pub mod ovr_inhwctrl {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x20004;
    pub mod ovr_dma_enable { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Overlay control, the shadow word taken at the start of vertical blank. Bit 0 enables fetch; the rest is the overlay's tile-list pointer. The miniport  (inferred)
pub mod ovr_control {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x20008;
    pub mod ovr_tile_ptr { // bits 31:9, inferred
        pub const SHIFT: u32 = 9;
        pub const MASK: u32 = 0x7fffff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod ovr_dma_enable { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Normal plane width and depth. Whole tile columns in 12:5 and the remainder in 32-byte units in 4:0, depth in 14:13, FIFO reset in bit 15 pulsed. A til (established)
pub mod frm_size_tile {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x30000;
    pub mod frm_fifo_reset { // bits 15:15, established
        pub const SHIFT: u32 = 15;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_depth { // bits 14:13, established
        pub const SHIFT: u32 = 13;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
        #[allow(unused_imports)]
        pub use super::super::enums::frame_depth::Value;
    }
    pub mod frm_width_tile { // bits 12:5, established
        pub const SHIFT: u32 = 5;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_rhs { // bits 4:0, established
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Normal plane height in lines, in 31:16. The firmware self-test walks only those bits. (established)
pub mod frm_size_pixel {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x30004;
    pub mod fb_height_pix { // bits 31:16, established
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xffff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Live copy of the normal plane control word. Completion of a page flip is this word matching the value written; stop waits for bit 0 to clear. Software (inferred)
pub mod frm_inhwctrl {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x30008;
    pub mod frm_tile_ptr { // bits 31:9, inferred
        pub const SHIFT: u32 = 9;
        pub const MASK: u32 = 0x7fffff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_linear { // bits 1:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_dma_enable { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Normal plane control, the shadow word taken at the start of vertical blank. Bit 0 enables fetch; bit 1 selects the linear form, in which the pointer i (inferred)
pub mod frm_control {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x3000c;
    pub mod frm_tile_ptr { // bits 31:9, inferred
        pub const SHIFT: u32 = 9;
        pub const MASK: u32 = 0x7fffff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_linear { // bits 1:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod frm_dma_enable { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Live copy of the per-window id fetch control word. The NT miniport tests bit 16 with a byte load of the third byte. The live bit is bit 16. Linux name (inferred)
pub mod did_inhwctrl {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x40000;
    pub mod did_dma_enable { // bits 16:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Per-window id fetch control, the shadow word. The NT miniport writes 0 at mode set and clears bit 16 to stop; the firmware writes 0. The firmware self (inferred)
pub mod did_control {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x40004;
    pub mod did_dma_enable { // bits 16:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Per-window mode table of 32 words, the window id table. Linux fills all 32 identically (8 bits per pixel as index, 16 as RGBA5, 32 as RGB8); the firmw (inferred)
pub mod mode_regs {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x48000;
    pub const COUNT: u32 = 32;
    pub const STRIDE: u64 = 4;
    pub mod wid_gamma_bypass { // bits 10:10, inferred
        pub const SHIFT: u32 = 10;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod wid_cm_bank { // bits 9:5, inferred
        pub const SHIFT: u32 = 5;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod wid_typ { // bits 4:2, inferred
        pub const SHIFT: u32 = 2;
        pub const MASK: u32 = 0x7;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
        #[allow(unused_imports)]
        pub use super::super::enums::did_pixel_type::Value;
    }
    pub mod wid_buf { // bits 1:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
        #[allow(unused_imports)]
        pub use super::super::enums::did_buffer_mode::Value;
    }
}
/// Colour map, 6144 entries in the Linux declaration (the O2 display back end specification says 4608), one dword each. Both drivers write an entry as re (inferred)
pub mod cmap {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x50000;
    pub const COUNT: u32 = 6144;
    pub const STRIDE: u64 = 4;
    pub mod red { // bits 31:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod green { // bits 23:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod blue { // bits 15:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Colour map FIFO status. Linux waits until it reads non-zero and then writes that many entries. The NT miniport, before every 32nd entry, spins until t (inferred)
pub mod cm_fifo {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x58000;
    pub mod free_entries { // bits 5:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x3f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Gamma map, 256 dwords. Linux, the firmware and the Windows 2000 boot video driver write the identity ramp (i times 0x01010100). The NT miniport copies (inferred)
pub mod gmap {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x60000;
    pub const COUNT: u32 = 256;
    pub const STRIDE: u64 = 4;
    pub mod red { // bits 31:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod green { // bits 23:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod blue { // bits 15:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// 10-bit gamma map, 1024 dwords. The same two NT paths write it when the size selector is 10 or the registry value is 4096 bytes long. The field layout  (inferred)
pub mod gmap10 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x68000;
    pub const COUNT: u32 = 1024;
    pub const STRIDE: u64 = 4;
}
/// Cursor position. The display driver writes the pointer position less the hot spot, masked to 12 bits, x in 11:0 and y in 27:16; a hidden cursor is par (inferred)
pub mod crs_pos {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x70000;
    pub mod y { // bits 27:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod x { // bits 11:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xfff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Cursor control. The display driver writes 1 to show and 0 to hide; the NT 4.0 HAL's stop clears bit 0 with a read-modify-write; every other writer wri (inferred)
pub mod crs_ctl {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x70004;
    pub mod bit1 { // bits 1:1, unknown
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod show { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Cursor colours, three entries. The display driver writes each as red in 31:24, green in 23:16 and blue in 15:8 (the colour list's entries 1 to 3 go to (inferred)
pub mod crs_cmap {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x70008;
    pub const COUNT: u32 = 3;
    pub const STRIDE: u64 = 4;
    pub mod red { // bits 31:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod green { // bits 23:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod blue { // bits 15:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Cursor glyph, 64 dwords: a 32 by 32 pointer, two dwords per row, 2 bits per pixel packed from the top of the dword, 0 for transparent and 1 to 3 for t (inferred)
pub mod crs_glyph {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x78000;
    pub const COUNT: u32 = 64;
    pub const STRIDE: u64 = 4;
    pub mod px0 { // bits 31:30, inferred
        pub const SHIFT: u32 = 30;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px1 { // bits 29:28, inferred
        pub const SHIFT: u32 = 28;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px2 { // bits 27:26, inferred
        pub const SHIFT: u32 = 26;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px3 { // bits 25:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px4 { // bits 23:22, inferred
        pub const SHIFT: u32 = 22;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px5 { // bits 21:20, inferred
        pub const SHIFT: u32 = 20;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px6 { // bits 19:18, inferred
        pub const SHIFT: u32 = 18;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px7 { // bits 17:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px8 { // bits 15:14, inferred
        pub const SHIFT: u32 = 14;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px9 { // bits 13:12, inferred
        pub const SHIFT: u32 = 12;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px10 { // bits 11:10, inferred
        pub const SHIFT: u32 = 10;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px11 { // bits 9:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px12 { // bits 7:6, inferred
        pub const SHIFT: u32 = 6;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px13 { // bits 5:4, inferred
        pub const SHIFT: u32 = 4;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px14 { // bits 3:2, inferred
        pub const SHIFT: u32 = 2;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod px15 { // bits 1:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x3;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// Video capture control word 0. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_0 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80000;
}
/// Video capture control word 1. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_1 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80004;
}
/// Video capture control word 2. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_2 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80008;
}
/// Video capture control word 3. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_3 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x8000c;
}
/// Video capture control word 4. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_4 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80010;
}
/// Video capture control word 5. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_5 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80014;
}
/// Video capture control word 6. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_6 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80018;
}
/// Video capture control word 7. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_7 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x8001c;
}
/// Video capture control word 8. Neither driver touches the capture block; the Linux header names nine words. The video in and out ports raise interrupt  (unknown)
pub mod vc_8 {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x80020;
}
/// Retrace latch in its own 32-bit window, wired to line 24 of the system interrupt controller (latched). The retrace handler reads it, writes back the b (inferred)
pub mod retrace_status {
    pub const WIDTH: u32 = 32;
    pub const OFFSET: u64 = 0x0;
    pub mod frame_end { // bits 1:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod release_copies { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// A 32-bit frame buffer pixel of the RGB8 type. The miniport's pixel format table and the Windows 2000 frame buffer driver give red in the top byte, gre (inferred)
pub mod pixel_rgb8 {
    pub const WIDTH: u32 = 32;
    pub mod red { // bits 31:24, inferred
        pub const SHIFT: u32 = 24;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod green { // bits 23:16, inferred
        pub const SHIFT: u32 = 16;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod blue { // bits 15:8, inferred
        pub const SHIFT: u32 = 8;
        pub const MASK: u32 = 0xff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// A 16-bit frame buffer pixel of the RGBA5 type: five bits of red on top, then green and blue, and one alpha bit at the bottom. The miniport's format ta (inferred)
pub mod pixel_rgba5 {
    pub const WIDTH: u32 = 16;
    pub mod red { // bits 15:11, inferred
        pub const SHIFT: u32 = 11;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod green { // bits 10:6, inferred
        pub const SHIFT: u32 = 6;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod blue { // bits 5:1, inferred
        pub const SHIFT: u32 = 1;
        pub const MASK: u32 = 0x1f;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
    pub mod alpha { // bits 0:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0x1;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
/// One entry of a tile list in main memory: the tile's first 4 KB page frame number shifted right 4 and truncated to 16 bits, so bit 15 is physical addre (inferred)
pub mod tile_list_entry {
    pub const WIDTH: u32 = 16;
    pub mod tile_number { // bits 15:0, inferred
        pub const SHIFT: u32 = 0;
        pub const MASK: u32 = 0xffff;
        pub const fn get(v: u32) -> u32 {
            (v >> SHIFT) & MASK
        }
        pub const fn set(v: u32, x: u32) -> u32 {
            (v & !(MASK << SHIFT)) | ((x & MASK) << SHIFT)
        }
    }
}
pub const BLOCK_SIZE_DECLARED: u32 = 0x80024; // established
pub const BLOCK_SIZE_LINUX: u32 = 0x1000000; // established
pub const BLOCK_SIZE_MAPPED: u32 = 0x80020; // established
pub const CHIPID_REAL_320: u32 = 0x2; // inferred
pub const CLOCK_REF_KHZ: u32 = 0x6978; // inferred
pub const CMAP_BANK_ENTRIES: u32 = 0x100; // inferred
pub const COLOR_MAP_FIFO_FREE: u32 = 0x3f; // inferred
pub const CRS_MAGIC: u32 = 0x36; // inferred
pub const CURSOR_GLYPH_PIXELS_PER_DWORD: u32 = 0x10; // inferred
pub const CURSOR_GLYPH_PIXEL_BITS: u32 = 0x2; // inferred
pub const CURSOR_GLYPH_ROWS: u32 = 0x20; // inferred
pub const CURSOR_GLYPH_ROW_DWORDS: u32 = 0x2; // inferred
pub const CURSOR_PARK_POS: u32 = 0xfe00fe0; // inferred
pub const ID_VALUE: u32 = 0x666; // inferred
pub const NORMAL_TILE_LISTS: u32 = 0x4; // established
pub const TILE_BYTES: u32 = 0x10000; // established
pub const TILE_HEIGHT_LINES: u32 = 0x80; // established
pub const TILE_LIST_ALIGN: u32 = 0x200; // established
pub const TILE_WIDTH_BYTES: u32 = 0x200; // established
