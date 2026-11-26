# The Windows 2000 and NT 4.0 builds of the Cobalt OpenGL and display drivers

Both operating systems have a 5.2.4 build of the three Cobalt graphics files, released on the same day with the same
change list. This page compares them: what differs, what does not, and which one a port should read first. The engine-facing
behaviour is the same in both; the differences are in the layer that faces the operating system.

Source: the SGI Cobalt Graphics Driver 5.2.4 packs for Windows NT 4.0 (patch SG0020054) and Windows 2000 (patch SG0020052),
both dated 12 December 2001; hashes in [the files list](../../references/windows-files.md). The NT 4.0 files were read with
the vendor's symbols; the Windows 2000 files carry no names (their `.dbg` files hold no symbols and Ghidra 12.1.3 cannot read
their `.pdb`, see the retools notes), so they were compared by size, instruction shape and constants. Grades as in
[AGENTS.md](../../AGENTS.md); what is "established" below is a comparison fact about the two files, not a claim about hardware.

## The two builds side by side

| | NT 4.0 | Windows 2000 |
|---|---|---|
| `sgcoicd.dll` | 1,069,328 bytes, file version 1.1.5.24, built 2001-12-13 03:12 UTC | 1,120,336 bytes, 1.1.5.24, built 2001-12-12 22:25 UTC |
| `sgcodd.dll` | 299,696 bytes, 5.2.4, built 2001-12-13 03:12 | 339,040 bytes, 5.2.4, built 2001-12-13 01:42 |
| `sgcomp.sys` | 62,992 bytes, 5.2.4 | 65,808 bytes, 5.2.4 |
| Linker, minimum OS in the header | 6.0, OS 4.0 | 5.12, OS 5.0 (ICD: subsystem 4.0, OS 5.0) |
| Display driver DDI version it reports | `0x20000` | `0x20000` (the NT 4.0 value, not the Windows 2000 one) |
| Drawing hooks in the DDI table | 32 | 35 |
| Release note | supersedes the 4.1.x, 5.2.1 to 5.2.3 updates and "MR Software v1.8" | supersedes 5.2.1 to 5.2.3 and the "SGI Supplemental Software Kit v5.1" |

Both release notes list the same three fixes and nothing else (a display-list bounding-box test that overflowed for floats
above 2^24, a negated point-light effect introduced in 5.2.3, and corrupted RGB textures wider than 640 pixels), so neither
build adds a feature in the engine path. Established (release notes of the two packs, read in full).

The three kernel and user files of the Windows 2000 pack were linked with a different linker (5.12) but compiled alike:
see the ICD comparison.

## OpenGL driver `sgcoicd.dll`

### Exported entry points. Established.

| NT 4.0 (15) | Windows 2000 (18) |
|---|---|
| `DllMain`, `DrvCopyContext`, `DrvCreateContext`, `DrvCreateLayerContext`, `DrvDeleteContext`, `DrvDescribeLayerPlane`, `DrvGetLayerPaletteEntries`, `DrvGetProcAddress`, `DrvRealizeLayerPalette`, `DrvReleaseContext`, `DrvSetContext`, `DrvSetLayerPaletteEntries`, `DrvShareLists`, `DrvSwapLayerBuffers`, `DrvValidateVersion` | the same fifteen plus **`DrvDescribePixelFormat`, `DrvSetPixelFormat`, `DrvSwapBuffers`** |

Two of the three additions are small forwarders. `DrvDescribePixelFormat` and `DrvSetPixelFormat` load `gdi32` on first use, look up
`GdiDescribePixelFormat` or `GdiSetPixelFormat` and call it (bodies at `0x69016d28` and `0x69016d71`, wrapped by the exports at `0x690175b3` and
`0x690175cb`). `DrvSwapBuffers` (`0x690175db`) is not a forwarder: it finds the window's record, calls the record's swap procedure, and only when
that returns 1 calls `GdiSwapBuffers` (`0x69016ceb`), which does load `gdi32` and forward
([the swap path](w2k-swap-path.md) reads it in full). The display driver's own hooks (`DrvDescribePixelFormat`, `DrvSetPixelFormat`,
`DrvSwapBuffers` in its DDI table, which both builds keep) are reached through GDI in the Windows 2000 build.

### Swap is wired differently. Established.

The NT 4.0 ICD has no `DrvSwapBuffers` export. Instead its `glFinish` entry looks up `wglSwapBuffers` in `opengl32`
(`GetProcAddress`, first falling back to a module whose name it builds) and, when its caller's return address lies within
`0x100` bytes of that function, runs the swap itself (`__wglSwapBuffers` to `__wglSwapLayerBuffers`). That is how the NT 4.0
build catches `SwapBuffers`, which `opengl32` implements as a `glFinish` followed by a call into the display driver. The
Windows 2000 ICD has no such string or lookup (`wglSwapBuffers` and the `GetModuleHandle`, `GetClassInfo`, `GetClassName`
and `GetWindowsDirectory` imports are the NT 4.0 file's alone) and swaps from its `DrvSwapBuffers` and `DrvSwapLayerBuffers` exports.
The escape sent for a swap is the same (`0x48`, [the escapes](opengl-escapes.md)). Whether `SwapBuffers` reaches the `DrvSwapBuffers`
export at all is `opengl32`'s decision, from a registry flag that the pack's installer does not set: as installed, only
`wglSwapLayerBuffers` does ([the swap path](w2k-swap-path.md)).

### Imports. Established.

The NT 4.0 file imports 80 functions and the Windows 2000 file 75: the five only the NT 4.0 file has are `GetModuleHandleA`,
`GetWindowsDirectoryA`, `GetClassInfoA`, `GetClassNameA` and `MSVCRT!_except_handler3`. The Windows 2000 file imports nothing the
NT 4.0 file does not. The escape call is `GDI32!ExtEscape` in both, and neither imports `OPENGL32`.

### Function-level comparison. Established for the counts, not for the unaligned remainder.

The routines of the two ICDs sit in the same source order. After dropping stubs of six bytes or fewer, the NT 4.0 file has
3,549 routines (named from the vendor's symbols) and the Windows 2000 file 2,814 (found by analysis alone). Aligning the two
lists by routine size, in address order:

- 2,637 pairs have equal size, and **2,592 of them are instruction-for-instruction identical** once addresses inside the image
  are masked (disassembled with a x86 disassembler, all operands compared). The 45 that differ are all short: 43 are the
  pixel-span generator tables (`__glGenColorsRGB*`, `__glCombinePixelsRGB*`, and similar, a few operands each, no routine
  more than 57 operands different), the `__glle_Begin_*` and `SetTEBDispatchVertex*` dispatch stubs (one operand), `__glSetError`
  (one operand) and `DrvDescribeLayerPlane` (all 42 instructions differ; same size).
- About 270 runs do not align. Most are tiny NT 4.0-only entries (a few bytes to a few dozen: vertex copy helpers, the
  `Color4*v` entry stubs, matrix push and pop helpers, `__glDoStore_*`, `ClipC`...) that the analysis of the Windows 2000 file,
  which has no symbols to seed function starts, did not split out. They are not shown to be absent. A real difference is
  confined to the window-system glue: `DrvSetContext` (`0x1ba` to `0x198` bytes), `DrvCreateLayerContext` (`0x227` to `0x1ef`),
  `DrvSwapLayerBuffers` (`0x22` to `0x7a`), `wglMakeCurrentReadSGI` (`0x433` to `0x40d`), `wglChoosePixelFormatEXT` (`0x3d1`
  to `0x3c2`), the window procedure (`0x113` to `0x66`), the format-modes helper (`0x117` to `0x114`), the pointer-capture and
  buffer-region helpers (`fbCaptureUsingReads`, `FbCapWriteBuffer`), and the three added exports.
- The engine-facing routines that were read for [the triangle path](opengl-triangle-path.md) and the texture packets
  (`coSetTexParam` `0xd07` to `0xce5` bytes, `__glCoInitTextureManager` `0x276` to `0x26a`, `__glCoTileToLinearXfer`
  `0x870` to `0x86b`, `__glCoResetAvailTlbSlots`) differ in register allocation and one switch layout only; their constants
  are the same set (`coSetTexParam` has `0x2900`, `GL_CLAMP`, as a switch compare where the NT 4.0 file compares differently;
  `__glCoTileToLinearXfer` loses one debug-print argument push).

### Which differences touch geometry. Established for the counts; inferred where marked.

A second alignment, by instruction-identical routines in order (all operands compared, addresses inside the image masked, branches
compared by position), finds the same picture from the other side. Of the NT 4.0 file's 3,633 routines, 2,747 are identical one for one,
717 more are identical inside runs of neighbouring routines that the analysis of the Windows 2000 file did not split (small
routines), and 169 differ. The 169 are 110 software pixel-span generators (`__glGenColors*`, `__glGenPixels*`, `__glCombinePixels*`), 18 span, depth-test and blend
tables, 14 window-system routines, 6 texture and buffer-management routines, 3 thread-block dispatch routines and 18 others.

- **Draw flags, vertex mode and the primitive mode word are not among them. Established.** All 19 routines that carry the draw-flags
  header `0x00210020` and all 20 that touch the context's draw-flags word (`gc+0x2dbb0`) are identical, except `__glCoTileToLinearXfer`,
  which loses one pushed debug-print argument. All 59 routines that carry the primitive mode header `0x18002020` (the `__glCoBegin*` and
  `RestartDispatch*` routines, `__glCoRenderPoint`) are identical. Of the 94 header constants of [the packet set](../../devices/cobalt-graphics/packet-set.md)
  only three differing routines carry any: `coSetTexParam` (texture parameter headers; the Windows 2000 file compares `0x2900` where the NT 4.0
  file compares differently, a switch layout), `__glCoDrawBufferRegion` (one alignment `nop`) and `__glCoTileToLinearXfer`.
- **The geometry-looking names are layout, not logic. Inferred.** `__glcoim_CullFace`, `__glcoim_Lighti`, `__glle_Begin_LineLoop` and `__glcole_BoundingBox`
  differ only in the first few instructions of the Windows 2000 region, where that file keeps function-pointer tables in the code stream; the
  remaining instructions match. `__glCoResetAvailTlbSlots` differs in register allocation, `__glSetError` in a tail call, and `Finish` by the NT 4.0
  file's `wglSwapBuffers` lookup ([above](#swap-is-wired-differently-established)).
- **The thread-block dispatch routines differ by operating system.** `SetTEBDispatchVertex2fv`, `3fv` and `4fv` store their pointer at thread-block
  offsets `0x880`, `0x8a0` and `0x8c0` in the NT 4.0 file and `0x930`, `0x950` and `0x970` in the Windows 2000 file: the two `opengl32` dispatch tables place `glVertex*fv` at different slots.

So the driver code that builds vertices, draw flags and primitive modes is the same program in both builds; a difference in what the screen saver draws on the
two systems does not come from it. What the 3D Pipes screen saver (`sspipes.scr`, 163,600 bytes, 1996-10-14 on the NT 4.0 disk) draws differs because of what each system sends:

- It imports `glNewList`, `glCallList`, `glMap2f` and `glEvalMesh2`, so its pipes are compiled display lists of evaluated surfaces. Established (imports).
- An emulator run of the NT 4.0 pack (screen saver, then the five test programs) records 410,375 draw-flags words of `0xc0000000` (object coordinates,
  engine transform) among 5.2 million port B vertices. The decoded first 3D nodes of the five test programs carry only `0x80000000` (host transform, clip coordinates) and `0`.
  A display list is replayed through the engine transform only after the bounding-box test of [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md).
  Inferred (model behaviour, not hardware; which of the two sources sent the `0xc0000000` words was not separated, since the trace has no time stamps).
- The same run of the Windows 2000 pack records no `0xc0000000` at all (196 words of `0x80000000`, 73 of `0`) and 3,440 port B vertices in total, so the picture
  the saver draws correctly there was not drawn by the Cobalt engine. Inferred (model behaviour).

The comparison therefore does not test the engine-transform path at all: the NT 4.0 shards are what that path produces under the model, and nothing here shows
a driver difference behind them. A test of that path needs a known-good reference: `GLCOSIMFORCECHIPXFORM` (read once in `GetBoardInfo`,
bit 0 of `noNeedToClip`) forces the engine transform for ordinary primitives, so the test programs can be drawn in both modes and compared.

### Packet headers and constants. Established.

- The set of 32-bit constants of header shape (`0x8` and `0x9` in the top nibbles and the others of
  [packet-set.md](../../devices/cobalt-graphics/packet-set.md)) that the code carries is the same in both ICDs: a comparison
  of the hexadecimal constants in the decompiled text of the two files, minus stack offsets and image addresses, finds no
  header-shaped constant in one file and not in the other (the constants that differ are stack offsets, NTSTATUS values and
  float constants). The vertex port, draw-flags, lighting and pixel-transfer packets
  therefore carry the same words in both.
- The 33 call sites of the escape wrapper carry the same (input size, output size) pairs, and the Windows 2000 sites send the
  same 29 codes (compared from the call arguments).
- The extension string of the hardware context (28 names) and of the software context, the version and renderer strings and
  every `GLCO*`, `GL_*` and `__GL_*` environment-variable name in the file strings are the same.

So no new packet, no new extension and no new escape appears in the Windows 2000 OpenGL driver.

## Display driver `sgcodd.dll`

- **Escape table.** Same 31 codes, names and sizes ([escapes](opengl-escapes.md)). Established.
- **DDI table.** Both report `iDriverVersion` `0x20000` and hook the same drawing entries (bit-blit, copy, text, stroke and
  fill path, line-to, stretch-blit, pointer, palette, the pixel-format group, `DrvSetPixelFormat`, `DrvDescribePixelFormat`,
  `DrvSwapBuffers`, DirectDraw entry points) except: the NT 4.0 file also hooks index 17 (its `DrvPaint` wrapper), which the
  Windows 2000 file does not, and the Windows 2000 file adds hooks at indices 67, 68, 71 and 88, with the table count 35
  against 32. Established for the table. The roles below come from what each routine calls, not from index names. Whether
  Windows 2000's `win32k` honours hooks that a driver reporting the NT 4.0 DDI version carries is not established here.
  The Windows 2000 file also calls `EngModifySurface` at surface and device-bitmap creation (four sites).
- **New features.** Hook 67 (format 1, a 256-entry ramp) copies the ramp into the 8-bit table the driver's colour-register
  routine takes and calls it. Hook 68 is a wrapper around one routine that calls GDI's `EngGradientFill` (so the gradient is drawn by GDI into the
  aperture surface), possibly twice with two clip fields swapped. Hook 71 runs an own routine (`0x319ac`, about 1.4 KB) when the source and destination
  rectangles match in size, a surface is a device surface and a device field is above 1, and falls back to `EngAlphaBlend`
  otherwise. Neither of the two own routines contains a packet-header constant, so by that evidence both are CPU paths, not
  engine ones (inferred). The Windows 2000 file's header immediates are more numerous than the NT 4.0 file's (a linear sweep
  counts `0x98250160` 116 against 125 and `0x9845b020` 114 against 123, operands not sites), which fits the extra 2D routines
  the earlier comparison noted ([2d-blit-video.md](../../devices/cobalt-graphics/2d-blit-video.md)); no header is new.
- **Imports.** 41 imports from `WIN32K` (NT 4.0) and 45 (Windows 2000). The Windows 2000 file adds `EngModifySurface`,
  `EngGradientFill`, `EngAlphaBlend`, `EngSaveFloatingPointState` and `EngRestoreFloatingPointState` and drops
  `EngFindImageProcAddress`. All five additions exist from Windows 2000 on.

## Miniport `sgcomp.sys`

- 65 imports (NT 4.0) and 64 (Windows 2000). The NT 4.0 file alone imports `HalDisplayString` and `RtlUnwind`; the Windows 2000
  file alone imports `ntoskrnl!_except_handler3` (structured exceptions, compiled differently). All of its `ntoskrnl` and `HAL` imports exist in the Windows XP SP3 kernel and HAL ([XP check](w2k-xp-compatibility.md)).
- The engine register reads and writes of the stop path (`0x10000`, `0x10018`, `0x185c0`, `0x185c8`, `0x185d0`, `0x18800`,
  `0x18138`, `0x18140`) are at the same offsets in the same routines ([read-backs](engine-readbacks.md)).
- The miniports differ in the video-port initialisation record they pass: `0x28` bytes in the NT 4.0 file, `0x50` (the
  Windows 2000 layout) in the Windows 2000 file, with the same adapter-find, initialise and start-I/O routines. Established.
- Both read the CPU block revision at `0xc2000008` and require its high nibble `0xa` and its low nibble neither 0 nor 8, and
  both take the resource-assignment path when the operating system major version is above 4 (`PsGetVersion`), so a
  version-5 kernel of any minor number takes the same branch.

## Which build should the port follow

Follow the NT 4.0 file for reading and the Windows 2000 file for the shape of the operating-system layer.

- For everything the engine sees (packets, escapes, state words, texture and transfer paths) the two ICDs are one program,
  so a port written from the NT 4.0 reading, which has the vendor's names and types, already matches the Windows 2000 build.
  No Windows 2000 reading is needed to get the engine side right, and a trace of either under an emulator is a valid
  reference for the other.
- For the window-system layer the Windows 2000 build is the right model: it exports the pixel-format and swap entries an
  XP `opengl32` expects of an installable client, and it does not depend on `opengl32`'s internal `wglSwapBuffers` address.
  The NT 4.0 build's call-site hack would not survive a different `opengl32`.
- For a reference run on XP only the Windows 2000 pack applies ([XP check](w2k-xp-compatibility.md)).

## Not compared

- The Windows 2000 display driver's routine bodies and the miniport's, beyond the tables, escape cases and register reads
  above; the 45 near-identical pairs' exact operands; the alpha-blend routine at `0x319ac` beyond its lack of packet constants; whether the 270 unaligned runs hide
  other engine-facing changes. The next routes are an own reader for the Windows 2000 program database (the format is the
  PDB 2.00 public stream), which would name every routine, and the same size-alignment applied to the display driver and
  miniport once they are named.
