# Files needed for the Cobalt graphics reverse engineering

This lists the proprietary files a reader copies from their own copies of SGI's Windows software into a local `root/` directory. They are not tracked in source control. Paths and roles only: this repository holds no file contents.

From the SGI Cobalt Graphics 5.2.4 driver pack for Windows NT 4.0 (patch SG0020054, 12 December 2001; the pack is a self-extracting InstallShield archive whose `data1.cab` holds the files):

```files
sgcomp.sys
sgcodd.dll
sgcoicd.dll
sgcotab.cpl
```

The matching `.dbg` symbol files ship in the pack and give the vendor's own function and variable names.

From the Windows 2000 install CD (build 2195), `\I386\`:

```files
sgibvid.dll
sglfb.sys
sglfb.dll
```

From the same Windows 2000 install CD, `\I386\`, the Microsoft files the [swap path page](../software/windows/w2k-swap-path.md) reads (exports only, no symbols):

```files
opengl32.dll
gdi32.dll
win32k.sys
```

The same page also reads the Windows 2000 Service Pack 4 (build 2195.6717) `opengl32.dll` (692,496 bytes) and `gdi32.dll` (233,744 bytes); they share the RTM files' names, so keep them in a separate directory.

From the SGI Cobalt Graphics 5.2.4 driver pack for Windows 2000 (`SGICoGraphics524w2k.exe`, the same InstallShield layout; its `.dbg` files hold no symbol table):

```files
sgcomp.sys
sgcodd.dll
sgcoicd.dll
```

From the SGI Visual Workstation 320 and 540 Windows NT 4.0 OEM disc set, disc 1 (identical to the same two files on Silicon Graphics Recovery CD-1 v1.1), `\I386\`, April 1999 builds:

```files
SGCODD.DLL
SGCOMP.SYS
```

From the same disc set, disc 2: the top-level `data1.cab` is an InstallShield 5 cabinet (`unshield x data1.cab` opens it; 7z does not) that holds the April 1999 OpenGL driver and the same two display files in the language-specific directory:

```files
Open GL\sgcoicd.dll
Language Specfic - ENU\system32\sgcodd.dll
Language Specfic - ENU\system32\drivers\sgcomp.sys
```

Loose on the same disc, the matching symbol files (`Symbols\cpl\sgcoicd.dbg`, `Symbols\dll\sgcodd.dbg`, `Symbols\sys\sgcomp.dbg`). These three carry a CodeView table of public names and no COFF symbol table, unlike the 5.2.4 `.dbg` files.

## Files read, with SHA-256 prefixes

| Product and file | Size | SHA-256 (first 16 digits) |
|---|---:|---|
| 5.2.4 NT 4.0 `sgcoicd.dll` (file version 1.1.5.24) | 1,069,328 | `ca772f62c1a1553c` |
| 5.2.4 NT 4.0 `sgcodd.dll` | 299,696 | `aa932e96561ec739` |
| 5.2.4 NT 4.0 `sgcomp.sys` | 62,992 | `872607c081b0d1ca` |
| 5.2.4 NT 4.0 `sgcoicd.dbg` (COFF symbols) / `sgcoicd.pdb` (PDB 2.00) | 365,904 / 4,293,632 | `ac0ca0770ecfe8c9` / `2364380191a47bda` |
| 5.2.4 NT 4.0 `sgcodd.pdb`, `sgcomp.pdb` | 795,648, 410,624 | `273bfeaa4386db44`, `982839e30ea220c5` |
| 5.2.4 Windows 2000 `sgcoicd.dll` (file version 1.1.5.24) | 1,120,336 | `ee1b8202d9399af2` |
| 5.2.4 Windows 2000 `sgcodd.dll`, `sgcomp.sys` | 339,040, 65,808 | `24b07c7e6a69e7ac`, `d472cb91bae2ecdf` |
| 5.2.4 Windows 2000 `sgcoicd.pdb` (PDB 2.00) | 4,400,128 | `268d1132ef9df236` |
| 5.2.4 Windows 2000 `sgcodd.pdb`, `sgcomp.pdb` (PDB 2.00) | 1,033,216, 492,544 | `5a5c3803f411b88d`, `ca316cf989c80cae` |
| Windows 2000 RTM (build 2195) `opengl32.dll` | 691,984 | `38671d6ee82e68ff` |
| Windows 2000 RTM `gdi32.dll` | 234,256 | `c82bf3ca428b7858` |
| Windows 2000 RTM `win32k.sys` | 1,726,256 | `cf96bd76273e29d6` |
| April 1999 `sgcoicd.dll` (file version 1.1.4.0, built 1999-04-27) | 1,245,456 | `8fc6a926fb699014` |
| April 1999 `sgcodd.dll` (file version 4.1.1) | 334,704 | `0a6be6e30120dc74` |
| April 1999 `sgcomp.sys` | 65,296 | `77ca10cf9b13b93a` |
| April 1999 `sgcoicd.dbg` | 304,936 | `e79955dafd9f5d17` |

The Windows 2000 pack's `.dbg` files hold frame-pointer data and a reference to the `.pdb` only. The NT 4.0 pack ships a `.pdb` for every driver as well; the NT 4.0 and Windows 2000 `sgcoicd.pdb` files declare the same engine register types and the same context layout (the OpenGL driver page says how they were read).

From the SGI Digital Media Software 5.2.2 pack for Windows NT 4.0 (`SGIDMediaSW522nt4.exe`, the same InstallShield layout; `data1.cab`), the video capture driver:

```files
sgilivid.sys
```

`sgilivid.sys` is 17,648 bytes, built 2001-10-29. Its installed path is `Livid_Driver\System32\Drivers\`. The 2D, blit and video page reads its mappings and entry points only.

## Image identity: the NT 4.0 5.2.4 miniport

The pages cite the miniport as `sgcomp.sys@ADDRESS`. The image behind those addresses is the NT 4.0 file of the 5.2.4 pack:

| | |
|---|---|
| File | `sgcomp.sys` from the NT 4.0 pack (patch SG0020054), 62,992 bytes |
| SHA-256 | `872607c081b0d1cab9cdaead053c90f7d6b25b62d83345e74b5fcbfe123d1e84` |
| PE header | time stamp 2001-12-13 03:12:07 UTC, linker 6.0, image base `0x10000`, size of image `0xf500`, entry `0x10b18`, checksum field `0x197e1` |
| Sections (virtual address, size) | `.text` `0x10360`, `0xd12c`; `.data` `0x1d4a0`, `0xfb4`; `INIT` `0x1e460`, `0x6ec`; `.rsrc` `0x1eb60`; `.reloc` `0x1ef00` |
| Symbols | `sgcomp.pdb` from the same pack, 410,624 bytes, SHA-256 `982839e30ea220c5c0e91ef55508f3509bf21f60a05eaa84bc7dd4e4764b925b`; the routine names below are its |
| Address form | the virtual address at the preferred base; for every section of this file it is the file offset plus `0x10000` |

Routines the retrace and scan-line reading uses, with the vendor's names:

| Address | Name | Role |
|---|---|---|
| `0x11e24`, `0x11e59` | `CobaltInitialize` (`0x11828`), the two `IoConnectInterrupt` calls | connects `CoInterruptHandler` (`0x17be8`) to bus interrupt level `0x19` and the retrace handler to level `0x18` |
| `0x191ab` | `CoVRetraceHandler` | the retrace interrupt service routine |
| `0x18314` | `CoDPC` | the deferred routine; dispatches the queued records |
| `0x17dae` | `CoProcessRetraceEvent` | the deferred routine's handler for a retrace record |
| `0x17d12` | `CoMoveChunksFromRetraceToSerializedQueue` | releases the held chunks |
| `0x17ce1` | `CoAllocIdleEvent` | takes a record from the free list |
| `0x1a293` | `CoSetMode` | arms the scan-line interrupts (the line is stored at `0x1a312`; the two `asSetInterrupt` calls are at `0x1a318` and `0x1a338`) |
| `0x1c378`, `0x1c3f8` | `asSetInterrupt`, `asGetScanline` | the writer of `vt_intr01` and `vt_intr23`, and the reader of the live line |
| `0x1d4a0` | the built-in mode table | 27 consecutive records of `0x30` bytes (`cobalt_timing_info`) up to `0x1d9b0`, the first 640 by 480 at 59.94 Hz |
| `0x1de84` | `cmcfrtsqOverhead` | the constant 3; only read |
| `0x14715` | `CoSetupGlobalData` (`0x146c4`) | the only write of `asPreblankInterval`, 300 |
