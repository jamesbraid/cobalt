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
- Public history of the `sgi_320` branch of a PC emulator that models the machine (used as another project's model, cross-check only; commit `96d399ef4`, 21 January 2025): https://github.com/disean/86Box/commits/sgi_320

## Software

- SGI Cobalt Graphics Driver 5.2.4 for Windows NT 4.0: https://archive.org/details/sgi_320_540_drivers (file `SGICoGraphics524nt4.exe`).
- Linux visws code (history tree): `drivers/video/sgivwfb.c`, `include/video/sgivw.h`, `arch/x86/kernel/visws_quirks.c`, `arch/x86/include/asm/visws/cobalt.h`.
- Usenet, comp.sys.sgi.admin, 2003 to 2004: history and the absence of Linux 3D support.
