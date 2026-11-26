# The setup loader block: Windows 2000 and Windows XP layouts

Windows text-mode Setup is told what the boot loader found through a block that the loader hangs off the loader parameter
block (`LoaderBlock + 0x54`). SGI's Windows 2000 setup loader (`ARCSETUP.EXE`, on the Windows 2000 CD's SGI files) builds the
Windows 2000 shape of that block. Windows XP's kernel and its Setup driver read a larger, rearranged one. This page records
both layouts as the shipped readers use them, side by side, so that a loader for the Visual Workstation can build the XP one.
Back to [software](../README.md); the boot video side of the same loaders is in
[w2k-display-drivers.md](w2k-display-drivers.md) and [w2k-xp-compatibility.md](w2k-xp-compatibility.md).

Source: Windows 2000 RTM (build 2195) `\I386\setupdd.sys` (384,880 bytes), `ntkrnlmp.exe` (1,611,712) and `setupldr.bin`
(229,264); Windows XP SP3 (5.1.2600.5512) `\I386\setupdd.sys` (493,696), `ntkrnlmp.exe` (2,145,280) and `setupldr.bin`
(260,288); SGI's `ARCSETUP.EXE` (162,816 bytes, in the SGI files added to the Windows 2000 CD). The paths are in
[the files list](../../references/windows-files.md). The XP functions are named with Microsoft's public symbol files (the
public names only, no types); the Windows 2000 files were read without names and matched to the XP functions by structure.
Offsets into images are relative virtual addresses. Read with a linear disassembler; every function cited was read in full.
Grades as in [AGENTS.md](../../AGENTS.md): a field the shipped reader dereferences is established for its offset and size,
what it means is inferred from how it is used unless a public header states it, and the public header agrees with every
offset below (the name column is that header's, see the last section).

## Answer

**Established: our earlier notes did not hold the XP layout.** SGI's loader allocates 0x32c bytes and sets the flags dword at
`+0x218`; XP's loader allocates 0x388 and its kernel and Setup read the flags at `+0x22c`. Everything from the second
embedded device node onward is shifted by 4, 8, 0xc or 0x14 bytes. So an XP kernel given the Windows 2000 block sees no text-mode flag
(and stops with `SYSTEM_LICENSE_VIOLATION`, code `0x9a`, when it goes looking for Setup's registry hive), and, with the flag
forced, XP's Setup driver reads Windows 2000's input-devices list pointer where it expects the boot font's length.

## Who reads the block

- **The XP kernel** (`ExpInitializeExecutive`, RVA `0x1e7aa0`): if `LoaderBlock + 0x54` is non-null it tests the byte at
  `+0x22c`: bit 1 (value 2) sets the kernel's text-mode-Setup flag, bit 0 (value 1) the remote-boot-client flag (code at
  `0x1f4b22`). The Windows 2000 kernel does the same at `+0x218` (RVA `0x141e3c`, `0x141e4f`). Nothing else sets either flag. A
  second reader, the routine that records remote-boot values in the registry (`IopAddRemoteBootValuesToRegistry`, XP RVA
  `0x203cfb`; Windows 2000 RVA `0x1471e2`), runs the same text-mode test and then reads the string pointer at `+0x230`
  (Windows 2000 `+0x21c`), appended to the redirector device name to make the value `RemoteBootMachineDirectory`; it is reached
  only for a remote boot. Established.
- **XP's Setup driver, start-up** (`SpInitialize0`, RVA `0x35a80`): reads the loader parameter block
  from the exported kernel variable, the setup block from it, and then the fields in the table below, once. The Windows 2000
  routine is at RVA `0x25750`. Its video part (`SpvidInitialize0`, XP RVA `0x43fd4`, Windows 2000 `0x33002`) takes the
  loader parameter block itself, and the virtual-OEM-device routine takes the setup block. After start-up the driver works from
  its own copies: the only other use of the kernel's loader-block variable in either image is a drive-letter call that
  passes it on unread. Established.
- **The XP setup loader** (`setupldr.bin`, the image embedded after the real-mode stub) is the producer. It allocates
  0x388 bytes from its heap (RVA `0x1f20f`, `push 0x388`), stores the pointer at `LoaderBlock + 0x54`, ORs 2 into the flags
  at `+0x22c` straight away, clears `+0x58`, `+0x60`, `+0x98`, `+0x9c` and `+0xa0`, and clears the low six bits of `+0x64`; it
  never zeroes the rest, so it depends on its heap handing out zeroed memory (inferred). The Windows 2000 loader and SGI's
  `ARCSETUP.EXE` (RVA `0xc45` to `0xc9f` of its image) do the same with 0x32c bytes and the offsets in the Windows 2000 column.
  Established.

## Side by side

Offsets in bytes from the start of the block. "Node" is an embedded device node (a `next` pointer, an id string and the file and
driver bookkeeping Setup turns into a hardware component); it is 0x28 bytes in XP and 0x24 in Windows 2000, because XP added a
hardware-id list pointer at node `+0x24`. Field names are the public header's (see the last section); "read" says what
the start-up routine does with the value.

| XP | W2K | Field | What Setup's start-up does with it |
|---|---|---|---|
| `0x000` | `0x000` | char pointer, the ARC name of the setup device | Not read by Setup's start-up or by the kernel. Unknown consumer. |
| `0x004` | `0x004` | video device (node) | Converted into hardware component slot 1, the display (the slot's meaning inferred from the order the component table is used in and the header's name) |
| `0x02c` | `0x028` | keyboard devices (list pointer) | Slot 2 |
| `0x030` | `0x02c` | computer device (node) | Slot 0 |
| `0x058` | `0x050` | SCSI devices (list pointer) | The list is walked for migrated drivers (XP only: string at node `+0x1c`, flag byte at `+0x20`), turned into a component list and then into driver registry entries |
| `0x05c` | none | OEM source devices (list pointer) | XP only. Nodes: `next`, an ARC name string at `+4`, a dword at `+8` and a 64-bit size at `+0x10`; Setup creates a virtual OEM source device for each |
| `0x060` | `0x054` | first scalar, the setup operation | Copied to a global |
| `0x064` | `0x058` | second scalar, bit flags (the loader clears the low six) | Copied to a global |
| `0x068`, `0x06c` | `0x05c`, `0x060` | setup information file (`txtsetup.sif`): pointer, length | Length stored, a buffer allocated and the bytes copied; always present |
| `0x070`, `0x074` | `0x064`, `0x068` | `winnt.sif`: pointer, length | Copied if the length is non-zero; parsed (an upgrade is detected from its contents) |
| `0x078`, `0x07c` | `0x06c`, `0x070` | `migrate.inf`: pointer, length | Copied if non-zero and processed (inferred for Windows 2000 from the order and the processing call) |
| `0x080`, `0x084` | `0x074`, `0x078` | `unsupdrv.inf`: pointer, length | Same |
| `0x088`, `0x08c` | none | boot font file: pointer, length | XP only. If the pointer and the length are both non-zero, a buffer of that length is allocated and the font copied |
| `0x090` | `0x07c` | monitor configuration data (pointer to 0x1a bytes) | If non-zero, 0x1a bytes are copied; not interpreted at start-up |
| `0x094` | `0x080` | monitor firmware id (char pointer) | If non-zero, duplicated as a string |
| `0x098` | `0x084` | boot bus extenders (list) | Converted to a component list |
| `0x09c` | `0x088` | bus extenders (list) | Same |
| `0x0a0` | `0x08c` | input devices support (list) | Same |
| `0x0a4` | `0x090` | hardware-id database (pointer to a list of id, driver, class-GUID strings) | Converted into Setup's own database |
| `0x0a8` to `0x12f` | `0x094` to `0x11b` | computer name (64 wide characters) and the first network scalars | Not read at start-up. Unknown |
| `0x130` | `0x11c` | server IP address | Copied, remote boot only |
| `0x134` to `0x223` | `0x120` to `0x20f` | netboot card ids and names | Not read at start-up |
| `0x224` | `0x210` | netboot card information (pointer; 5 dwords copied) | Remote system-preparation setup only |
| `0x228` | `0x214` | its length | Not read |
| **`0x22c`** | **`0x218`** | **flags** | See below |
| `0x230` | `0x21c` | machine directory (char pointer) | Read by the kernel, remote boot only |
| `0x234` | `0x220` | net-boot answer-file path (char pointer) | Duplicated as a string, remote boot only |
| `0x238` | `0x224` | net-boot secret (pointer) | Not read |
| `0x23c` | `0x228` | image-mirror file path (260-byte buffer) | Duplicated as a string, remote system-preparation only |
| `0x340`, `0x344` | none | ASR plug-and-play file: pointer, length | XP only. Copied if the length is non-zero; Setup then enters ASR mode |
| `0x348` | none | administrator password for a net boot (64-byte buffer) | XP only; remote boot only; a fixed default is used when the buffer is empty and its last byte is `0xff` |
| size `0x388` | size `0x32c` | | The loader's allocation |

Separately, the loader parameter block itself: `+0x10` (the boot driver list), `+0x34` (the ARC name of the boot device),
`+0x3c` (the boot path), `+0x4c` (the disk signature list), `+0x50` (the OEM font file) and `+0x54` (the setup block) are
read by Setup at the same offsets in both systems. The setup block's other pointers (every list head and string pointer
above) are ordinary 32-bit addresses in the loader's mapping; Setup converts the strings from 8-bit to wide characters
itself.

### Flags

| Bit | Value | Meaning | Read by |
|---|---|---|---|
| 0 | 1 | remote boot | kernel (`IoRemoteBootClient`), Setup (selects the whole network branch above). Both versions. |
| 1 | 2 | text-mode Setup | kernel (`ExpInitializeExecutive` and the remote-boot routine). Both versions. The loader sets it in the allocation routine. |
| 7 | `0x80` | force console | Setup (`ForceConsole`). Both versions. |
| 10 | `0x400` | remote system-preparation setup | Setup, in the remote branch (at `+0x22d` bit 2, Windows 2000 `+0x219` bit 2). Both versions. |
| 12 | `0x1000` | Windows 9x rollback | Setup (at `+0x22d` bit 4). XP only. |
| 9 | `0x200` | set by XP's loader (a single `or`, `+0x22d` bit 1) | No reader found in the kernel or Setup. Unknown. |

The public header (see below) defines the text-mode and remote-boot bits the other way round (text mode as 1, remote boot as
2). The kernel's own tests and the loader's stores above agree with the table, not with the header, so the binaries decide.

## The video and font fields

- **OEM font** (`LoaderBlock + 0x50`, both versions, mandatory). `SpvidInitialize0` checks it before use and stops Setup
  with a bug check if the check fails. The image must start with the word `0x200` (a version-2 Windows bitmap font); its
  size is the dword at image `+2` and is the size copied; the word at `+0x42` must be 0, the bytes at `+0x50`, `+0x51` and
  `+0x52` must be 0, the byte at `+0x55` must be `0xff` and the byte at `+0x5a` must be `0x30`; the character width at
  `+0x56` must be at most 32 and the height is at `+0x58`. The same checks are in the Windows 2000 routine. Established.
  Where the loader gets the image is inferred: XP's setup loader contains the names `vgaoem.fon` and `OemHalFont` next to each other.
- **Boot font** (XP `+0x88` pointer, `+0x8c` length; no Windows 2000 counterpart). XP's loader reads a file named
  `bootfont.bin` from the setup source and stores its buffer and size here only when the whole read succeeded (RVA
  `0x1d5b9`, `0x1d5bf`); if it is absent both fields stay 0 and Setup skips the copy. Established. This is the field that a
  Windows 2000 block puts a list pointer into (`+0x8c` is the Windows 2000 input-devices list), which is how XP's Setup came to
  ask for a 2 GB allocation.
- **Video device** (`+0x04` node, both versions): the display component. Setup converts the node's id string (`+4`), optional
  description (`+0xc`), the third-party flag and file-type bits (`+0x10`, `+0x14`), the driver file list (`+0x18`) and the
  base DLL name (`+0x1c`) into its own component record, and the file-list nodes (0x24 bytes in XP) are converted the same
  way. What Setup does with an id that its `txtsetup.sif` display table does not list was not read. Unknown.
- **Monitor data** (XP `+0x90`, `+0x94`; Windows 2000 `+0x7c`, `+0x80`): optional. Where XP's loader fills the pointer from
  was not located (inferred to come from the firmware's monitor configuration component). Setup copies the data and keeps it.

## What a loader for XP must therefore build

Facts only; the choices are the loader author's.

1. Allocate at least 0x388 bytes and zero them (the shipped loader relies on a zeroed heap), store the pointer at
   `LoaderBlock + 0x54`.
2. Set flag bit 1 at `+0x22c` for text-mode Setup. Leave bit 0 clear unless remote booting; leave the remote-boot strings at `+0x230`
   to `+0x344` zero.
3. Fill the computer node (`+0x30`), the video node (`+4`), the keyboard list (`+0x2c`) and the SCSI list (`+0x58`)
   with XP-sized nodes (0x28 bytes, hardware-id pointer 0 if none); a Windows 2000-sized node would overlap its
   neighbour by 4 bytes.
4. Put `txtsetup.sif`'s image and length at `+0x68` and `+0x6c`; `winnt.sif`, `migrate.inf` and `unsupdrv.inf` only if they
   exist (pointer and length at `+0x70`, `+0x78`, `+0x80`; length 0 means absent).
5. Give `LoaderBlock + 0x50` a font that passes the header checks above, and (optional) the boot font at `+0x88` and `+0x8c`.
6. Zero `+0x60`, clear the low six bits of `+0x64`, and zero `+0x58`, `+0x98`, `+0x9c` and `+0xa0` unless the loader has
   SCSI, extender or input-device lists to supply; give `+0xa4` the hardware-id database if it has one. Whether Setup
   copes with a null database was not read. Unknown.

## Not read, and open

- Whether XP needs the larger loader-block extension (SGI's loader gives a size-`0x28` version 5.0 extension). Not traced: neither the kernel's
  extension reads nor Setup's were enumerated. Unknown. Settles it: list the kernel's reads of `LoaderBlock + 0x58` and
  the size each tests, or the first boot of an XP kernel with the Windows 2000 extension past phase 1.
- What consumes flag bit `0x200`, and what the unread areas (`+0x00`, the computer-name and card-name regions) are for in
  Setup's later stages; Setup's later stages were not read. Unknown.
- Where `+0x90` and `+0x94` come from in XP's loader.
- The Windows 2000 file pair at `+0x6c` and `+0x74` was identified by order and by the processing calls, not by a name string.
  Inferred.

## The public header

`sdk/include/reactos/arc/setupblk.h` in the ReactOS tree (public; the ReactOS public source was not in the Library, so it was
fetched once, only to compare) declares the XP shape: field order, array sizes and flags agree with every XP offset in the
table (the video device after the first pointer, the keyboard list, the computer device, the SCSI list, the OEM source
list, two scalars, four file pairs, the boot font pair, the monitor pointer and id, three lists, the database, the 64-character
computer name, five network dwords, the netboot card ids, the flags dword at `0x22c`, then the strings, the ASR pair and the
64-byte password), and its device node is 0x28 bytes. It is a cross-check only; the offsets above come from the binaries.
Its two flag values are the reverse of what the kernel tests, as noted above.
