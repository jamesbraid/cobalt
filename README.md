# cobalt

Notes on the graphics of SGI's 320 and 540 Visual Workstations (1999, Pentium II and Pentium II Xeon, Windows NT 4.0):
the Cobalt rendering engine, the Arsenic display back end, and the Windows drivers that program them. The machines have
no graphics card. Display, 3D rasteriser and video ports are part of SGI's Cobalt chip set and share the 100 MHz main
memory with the processors.

No register manual was ever published. What is here comes from SGI's own Windows graphics stack (the OpenGL driver,
display driver and miniport, and the Windows 2000 frame buffer drivers), the public Linux frame buffer driver, and the
published datasheet, product guide and trade press. Every fact notes where it came from and how firmly: established,
inferred or unknown.

## What's covered

- **Display back end:** timing, frame buffer scan-out, palette, gamma, cursor, I2C and the mode table.
- **Rendering engine:** its register window, the packet queue in main memory, the packet headers the OpenGL driver
  emits, interrupts.
- **Windows drivers:** what the NT 4.0 miniport, display driver and OpenGL driver do, and the Windows 2000 boot video
  and frame buffer drivers.

Pages are under [devices/](devices/README.md) and [software/](software/README.md). The platform around the graphics
(memory controller, interrupt controller, Lithium bridges, HAL, PROM, boot path) is in
[chips](https://github.com/jamesbraid/chips). [COVERAGE.md](COVERAGE.md) lists what is still missing.

## Using it

To run the checks yourself:

```sh
git clone https://github.com/jamesbraid/cobalt && cd cobalt
tools/setup-clone.sh
retools/tools/re-run retools/tools/defs/ci-check
```

You'll need Podman, git and python3. The SGI software is not included.
[references/windows-files.md](references/windows-files.md) says which files the notes were read from.

## How far along

The display back end's registers and the NT driver's use of them are mapped. The rendering engine's packet set is
mostly open. [questions/](questions/README.md) has the open questions.
If you spot a mistake or have a manual, a board photograph or a configuration dump we don't, [open an issue](https://github.com/jamesbraid/cobalt/issues).

## License

[BSD 3-Clause](LICENSE). The `retools/` scripts started life as gr4's and keep its notice. No SGI material in here;
bring your own.

## Credits

James Braid, plus a lot of Claude reading x86 disassembly so I didn't have to.

Thanks to -

* Dominik Behr ([@techomancer](https://github.com/techomancer)) for [gr2](https://github.com/techomancer/gr2)
  and [gr4](https://github.com/techomancer/gr4), where the layout and working style come from
* The Linux visws maintainers, whose public code is the only open description of the display back end
* Everyone keeping SGI manuals and period software online
