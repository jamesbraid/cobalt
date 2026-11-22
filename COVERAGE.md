# cobalt coverage map

The areas of the Visual Workstation 320 and 540 graphics and where each stands. There is no measured inventory yet:
`retools/tools/inventory.py` needs the driver pack unpacked under `root/` and an inventory configuration. Until then the
rows below are the tally, graded as in [AGENTS.md](AGENTS.md).

| Area | State | Where |
|---|---|---|
| Display back end registers (Arsenic, `0xd0000000`) and mode table | graded from the Linux driver and the NT driver; cursor, gamma and capture blocks and every reset value unknown; no `registers.toml` yet | [devices/cobalt-graphics/display-engine.md](devices/cobalt-graphics/display-engine.md) |
| Rendering engine window and packet queue | window extent, queue ownership bit and doorbell read from the miniport; packet header word count field, chaining and link words read from the OpenGL driver; opcode set and window parameter and table packets in the packet-set page; most of the window unknown | [devices/cobalt-graphics/graphics-engine.md](devices/cobalt-graphics/graphics-engine.md) |
| Rendering engine packet set | 52 constant header values of the OpenGL driver plus one of the display driver, classified with emitting routine and payload layout; the `B` field is the register index, the mode word, instance bits, vertex ports, window parameters and table entries are decoded as the drivers write them; engine behaviour unknown | [devices/cobalt-graphics/packet-set.md](devices/cobalt-graphics/packet-set.md), [draw-and-buffers.md](devices/cobalt-graphics/draw-and-buffers.md) |
| SGI white paper and a real machine's registry | read in full; features, identity, resource claims and memory ranges recorded | [devices/cobalt-graphics/board-data.md](devices/cobalt-graphics/board-data.md) |
| NT 4.0 OpenGL driver structure | symbols from the `.dbg` COFF table (counts reproduced); run-time code generators identified; header scan done on the NT 4.0 and Windows 2000 packs; engine-versus-host rules for transform and lighting read | [software/windows/nt-opengl-icd.md](software/windows/nt-opengl-icd.md) |
| NT 4.0 miniport, display and OpenGL drivers | miniport attach, display back end programming and the OpenGL escape table read; the OpenGL driver's packet emitters classified in the packet-set page (the engine's use of them is unknown) | [devices/cobalt-graphics/nt-software.md](devices/cobalt-graphics/nt-software.md) |
| Windows 2000 boot video and frame buffer drivers | read in Ghidra, no symbols | [software/windows/w2k-display-drivers.md](software/windows/w2k-display-drivers.md) |
| Regrading to three ratings | the moved pages still use five grades; mapping in the overview | [devices/cobalt-graphics/README.md](devices/cobalt-graphics/README.md#grades) |
| Open questions | the packet set, engine window, second interrupt register, reset values, cursor and capture | [questions/open-questions.md](questions/open-questions.md) |
