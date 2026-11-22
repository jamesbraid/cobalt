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

The Windows 2000 packs carry their own, different builds of the three drivers. The header scan of the packet set
([packet-set.md](../devices/cobalt-graphics/packet-set.md#coverage)) covers both packs and the April 1999 builds.
The April 1999 OpenGL driver is inside an InstallShield 3 archive on disc 2 that neither 7z nor unshield opens; only its
`.dbg` file was seen.
