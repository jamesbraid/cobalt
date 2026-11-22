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
| April 1999 `sgcoicd.dll` (file version 1.1.4.0, built 1999-04-27) | 1,245,456 | `8fc6a926fb699014` |
| April 1999 `sgcodd.dll` (file version 4.1.1) | 334,704 | `0a6be6e30120dc74` |
| April 1999 `sgcomp.sys` | 65,296 | `77ca10cf9b13b93a` |
| April 1999 `sgcoicd.dbg` | 304,936 | `e79955dafd9f5d17` |

The Windows 2000 pack's `.dbg` files hold frame-pointer data and a reference to the `.pdb` only. The NT 4.0 pack ships a `.pdb` for every driver as well; the NT 4.0 and Windows 2000 `sgcoicd.pdb` files declare the same engine register types and the same context layout (the OpenGL driver page says how they were read).
