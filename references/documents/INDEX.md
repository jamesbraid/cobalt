# Public sources

Public documents this workspace reads, by title and public URL only. Nothing here is a copy. A claim that rests on one of them cites it by title, revision and page in the owning page.

## The workstations

- SGI 320 Visual Workstation datasheet (Silicon Graphics, 1999): feature list, supported modes, driver list.
- SGI 320 Visual Workstation Product Guide (Silicon Graphics, 1999): architecture overview, chip names.
- "SGI Chip Set Powers NT Workstations", Microprocessor Report, 15 February 1999: bandwidth split, fill and triangle rates, unified memory rationale.
- "SGI reinvents NT workstations", Design News, 1999: three-ASIC structure, static memory split.

## Graphics and board data

- "The Silicon Graphics 320 and Silicon Graphics 540 Visual Workstation Graphics Subsystem", Silicon Graphics white paper, form J10043 (2351 6/99), five pages: https://sgi320.wordpress.com/wp-content/uploads/2013/01/320_gfx_subsystem.pdf
- "Contents of the HKLM\HARDWARE registry key", a registry export of a running SGI 320, linked from the ReactOS wiki page "SGI 320/540": https://pastebin.com/raw/dnE1xn5q and https://reactos.org/wiki/SGI_320/540

## Software

- SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0 and for Windows 2000: Library cache `drivers/visws/SGICoGraphics524nt4.exe` (SHA-256 `5097ff4e911a73b892e3072d0db439afb42e9e5db099c8937ee8d38ed810d5e6`) and file `SGICoGraphics524w2k.exe` (SHA-256 `c09ea6c6e059b0a887b48dd8ffa66703783d5ba7308efccf994dc7e7ef675363`).
- SGI Visual Workstation 320 and 540 Windows NT 4.0 OEM disc set, discs 1 and 2 (April 1999 builds of the display driver and miniport on disc 1, `I386`; the April 1999 OpenGL driver in the top-level `data1.cab` of disc 2, whose image has SHA-256 `a382074fb24d3884e82d84aab4788921e305fda854b7759818ccba0d020b1900`, and the OpenGL driver's symbol files and the display driver's loose on disc 2, `SYMBOLS`), and Silicon Graphics Recovery CD-1 v1.1 (the same display driver and miniport builds in `I386`).
- Linux visws code (history tree): `drivers/video/sgivwfb.c`, `include/video/sgivw.h`, `arch/x86/kernel/visws_quirks.c`, `arch/x86/include/asm/visws/cobalt.h`.
- Usenet, comp.sys.sgi.admin, 2003 to 2004: history and the absence of Linux 3D support.
