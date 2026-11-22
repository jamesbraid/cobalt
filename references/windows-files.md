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

Windows 2000 also carries its own, different builds of the SGI Cobalt graphics driver set; only the display driver's OpenGL escape table has been compared so far.
