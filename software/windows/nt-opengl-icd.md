# The NT 4.0 OpenGL driver `sgcoicd.dll` 5.2.4

How the OpenGL installable client driver (ICD) is built, how its symbols were applied, and where its packet emitters
are. The packets themselves are in [packet-set.md](../../devices/cobalt-graphics/packet-set.md) and
[draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md). The file is the one the
[files list](../../references/windows-files.md) names (1,069,328 bytes, image base `0x69000000`, 4,096-byte pages).
Grades as in [AGENTS.md](../../AGENTS.md): what a routine does is read from the code; what the hardware does with the
result is not claimed here.

## Symbols

The `.dbg` file that ships next to it holds the vendor's own names twice: a COFF symbol table and a reference to a
CodeView `.pdb` (`NB10`, Microsoft program database version 2.00). The COFF table was parsed directly (the layout is the
Windows `.dbg` header, the debug directory, and the table with its string pool). It has 13,109 records, of which 8,883
are symbols and the rest are auxiliary records; 3,929 are function-typed symbols in `.text`, at 3,633 distinct
addresses (some routines carry two names), and those names, with their stdcall and fastcall decorations, are what the
pages cite. Any claim about a routine therefore cites a vendor name and an
address, for example `__glCoSetScissor` at `0x69018ebe`. The `.pdb` (4.2 MB) carries the vendor's type declarations, read
in [the next section](#the-program-databases-types).

The static locals of `__glGenerateSetup` (names such as `affine_texture_bias`, `mmx_q_bias`, `alpha_correction_factor`,
at `0x69013810` to `0x690138a0`) are in the image but nothing in `.text` addresses them, and no function of that name
exists in this build. Established by scanning every dword of `.text` for four of those addresses (`0x69013810`, `0x69013858`,
`0x69013888`, `0x690138a0`) and finding none; the routine is probably leftover from the vendor's generic software
rasteriser (inferred, from the names). The brief's `__glGenerateSetup` therefore has no packet emitter to classify.

## Shape of the driver

3,633 distinct function addresses, in these families (by symbol prefix):

| Family | Count | Role |
|---|---|---|
| `___glim_*` | 310 | immediate-mode GL entry points |
| `___gllc_*` and `___glcolc_*` | 321 | display-list compile entry points |
| `___glcoim_*` | 114 | the Cobalt versions of the immediate calls; they set the context's state and call a `__glCoPass*` routine to write the packet |
| `__glCo*`, `co*`, `Co*` | 572 | the Cobalt back end: begin and restart routines, fill and render routines, buffer and texture management, pixel transfer, display-list conversion, validation |
| `Generate*` and `__glGenerate*` (the `__glCoGenerate*` ones are in the row above) | 21 | run-time code generators for vertex handling (below); an earlier count gave 19 |
| the rest | 2,295 | the generic OpenGL machinery from the vendor's base code, including the span, store and image-pipe routines used for fallbacks: evaluators, texture state, clipping, lighting maths, colour tables |

**The driver compiles code at run time.** `__glCoGeneratePrimitiveHandler`, `GenerateSetAttribs`,
`GenerateTransformer` (and a Pentium III `KNI` version of each), `GenerateHostCullHandler`,
`__glCoGenerateNoXformHandler`, `GenerateQuadsHandler`, `__glCoGeneratePolygonMode`, `GenerateCalcLightVectors` and
the others write x86 machine code into a code cache (`__glCoCreateVertexCodeCache`) for each combination of state and
attributes, through a small emit helper (`emit_code`, `_value_8`). The generators emit code that stores the packet
headers: each header is an immediate in the generator, which is why a scan of the generators' bytes finds them, and the
store happens when the generated code runs. The generated code itself is data in memory
and is not in the image.

`__glDrawVertexes_*` (point, line, line strip, line loop, triangle, quad, polygon, triangle strip, triangle fan and
quad strip, plus a wrapper that asks `__glGenerateDrawVerticesVcache` for a code body) are the generic OpenGL
primitive loops. They read vertex records of 0x170 bytes, test the clip-code word at record offset `0x30` against the
mask `0xfff20000`, call `__glDoClipTriangle` when a triangle straddles the frustum and call the context's triangle
render procedure (`procs.renderTriangle`, offset `0x3164`) otherwise. They contain no header constants.

## Where the packets come from

| Routine class | Headers it writes |
|---|---|
| `__glCoPass*` (Fog, AlphaFunc, Blend, LogicOp, ColorMask, DepthTest, Stencil, PolygonOffset, CullFace, MVPMatrix, NormalMatrix), `__glCoSetViewMatrix`, `__glCoSetScissor`, `__glCoSetDrawBuffer`, `__glCoSetSzBuffer`, `__glCoEnable*` | the per-state rows |
| `__glCoBegin*`, `Restart*`, `__glCoFill*Triangle`, `__glCoRender*` | mode word, vertex ports |
| `__glCoSetVertexAttribsMask`, `GenerateSetAttribs` | the attribute packets and the vertex ports |
| `LoadLightSource`, `LoadLightPath`, `ChangeMaterial*`, `__glCoValidateMaterial` | light and material blocks |
| `coSetTexParam`, `__glCoSetXferParam`, `coSetColorMatrixRegisters`, `coVhdlTexImage`, `__glCoCreateTexLevel`, `coCreateTexBindPkt`, `coDispatchTexBindPkt` | texture and transfer rows |
| `coClearBuffers`, `__glCoClearBuffers` | clear rows |
| `DispatchCurrentPkt`, `ClosePacket`, `__glCoDlistExecutePackets`, `InitCoPktBuffer` | chain, flush marker, node set-up ([graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md)) |
| `__glCoDlistToPackets` | converts a compiled display list into recorded packets, with the same headers |

The display driver `sgcodd.dll` uses the same headers for 2D, plus one the OpenGL driver never sends (`0x91ca1020`); a
scan of both drivers' immediates for every header form outside the list found nothing else. The miniport `sgcomp.sys`
builds the chain and window-parameter packets, the texture table loads and the hardware clear, copy and
context-switch packets ([packet-set.md](../../devices/cobalt-graphics/packet-set.md)).

## How a header scan was done

Two passes, both over the whole of `.text` and `.data`. First, a linear disassembly sweep collected every 32-bit
immediate operand, which finds the headers the code stores and the generators emit. Second, every dword, aligned or not,
whose bits 31:12 equal a known `(A, B)` pair was collected from the raw bytes, which also finds table entries and false
hits; in `.text` the exact known header values are also matched byte by byte, because a sweep that meets data in the
code loses its place (it missed the viewport header in the April 1999 image). The immediates were attributed to the routine that contains them by the symbol table. Raw hits in data (export
and import tables, jump tables, string pools) occur: the earlier `0x9400014f` was one, in the export address table at
`0x690f2681` where it straddles two entries, which is why every row also rests on a routine whose body was read. The
scan is repeatable with any script that does the same arithmetic; no scanner or listing is committed.

## The program databases' types

Every 5.2.4 pack ships a `.pdb` (Microsoft program database 2.00, Visual C 6) for each driver, NT 4.0 and Windows 2000
alike. The type streams give the vendor's own declarations of the context, the vertex, the window record and the engine
register words. Read in full this round; the NT 4.0 and Windows 2000 `sgcoicd.pdb` files declare identical layouts
(`__GLcontextRec` 185,720 bytes, `__GLCOcontextRec` 189,600, the same member offsets), so one table serves both builds.

**How they were read.** Ghidra's PDB reader (`--pdb` of the export tool) applies the NT 4.0 `sgcomp.pdb`, `sgcodd.pdb` and `sgcoicd.pdb`
(function names, prototypes and structure types; the decompiler shows the vendor's names for callees, fields and globals) and fails on
the Windows 2000 `sgcoicd.pdb` and `sgcomp.pdb` with "Needed data is not available" in the module-information stream, so no
Windows 2000 routine got a PDB name. The type streams were first read with a small parser of the PDB 2.00 container
and its type records (the member lists, bit fields and array sizes); the layouts below were then checked a second time
against the structures Ghidra builds from the same streams (every offset and size in this section matches). The prototypes
Ghidra applies say `__cdecl` for routines that take the context in ECX, so by default the decompiler leaves the context's fields as raw
offsets. A project that sets `__fastcall` on the routines whose first parameter is a pointer to a vendor structure and whose body reads ECX
(1,247 of them, 962 with the context first), and types a context parameter as `__GLCOcontextRec` (the vendor's context with the generic part first), makes the decompiler print every field by name; that is how the
other pages' offsets into the context were resolved.

**The context, from `gc+0x2d578` (the `__GLCOcontextRec` part after the generic context).** Established (declaration read in
full; the offsets match the routines cited in [draw-and-buffers.md](../../devices/cobalt-graphics/draw-and-buffers.md)).

| Offset | Vendor name | Notes |
|---|---|---|
| `+0x2d578` | `modes` (16 bytes of flags) | `+6` `softLighting`, `+7` `useOG`, `+8` `forceClip`, `+9` `clipToBigFrustum`, `+0xa` `forceChipXForm` (`GLCOSIMFORCECHIPXFORM`), `+0xb` `allowChipXForm` (`GLCOALLOWCHIPXFORM`, at `+0x2d583`), `+0xc` `forceChipTexXForm` (`GLCOSIMFORCECHIPTEXXFORM`, at `+0x2d584`), `+0xd` `fastSetAttribs`, `+0xe` `objectClipCheck`, `+0xf` `hardwareDisplayListsEnabled` |
| `+0x2d5a0` | `light` | `lightingMode` (the path word) at `+0x2d5c0`, `controlWord` (per-light bits: `__glCoSetVertexAttribsMask` tests `1 << light` shifted left 8 to decide whether a light's scalar packet is sent) at `+0x2d5cc`, `lightModeReg` (the lighting-mode packet's word, a `CoLightModeReg`) at `+0x2d5e4`, `calcColor` at `+0x2d608`; the out-of-range flags `cOutOfRange` at `+0x2d5a8` |
| `+0x2d60c` | `vertex` (868 bytes) | `noNeedToClip` at `+0x2d960` (a four-byte word: bit 0 `forceChipXForm` from `GLCOSIMFORCECHIPXFORM`, bit 1 the clip-volume hint while it is `GL_FASTEST`, bit 2 lock-array, bit 3 display list); `allTuples` `+0x2d964`; `hostCulling` `+0x2d968`; `enableTexMatrix` byte at `+0x2d96e` |
| `+0x2d9b0` | `texture` (204 bytes) | `coParam` (the four texture-format words) `+0x2d9b0`, `coEnv` `+0x2d9c0`; `controlWord` at `+0x2da78` is the texture-generation state word of `__glCoPickCalcTextureProcs` |
| `+0x2da7c` | `pixel` (284 bytes) | `yCrCbToRGBA` `+0xc`, `RGBAToyCrCb` `+0xd`, `enableHwCM` (colour-matrix flag) `+0xd6`, `xferParam` `+0x104` (the five words `descId`, `pixDepth`, `pixType`, `pixMode`, `linear`), `colorFormat` `+0x118` |
| `+0x2db98` | `displayBank` | |
| `+0x2db9c`, `+0x2dba0` | `drawmode`, `drawModeReg` | the pixel-pipe mode word and the last value sent, a `CoDrawModeReg` |
| `+0x2dba4`, `+0x2dba8`, `+0x2dbac` | `linePrimMode`, `polyPrimMode`, `primitiveReg` | `CoPrimModeReg` |
| `+0x2dbb0` | `xformmodeReg` | a `CoXformModeReg`: the draw flags word |
| `+0x2dbb4`, `+0x2dbb8` | `flatColorReg`, `scissor` | the colour last sent (reset to all ones by a node dispatch); the scissor corners (`CoClipRectReg`) |
| `+0x2dbc4` | `tlbDescTable` | 64 descriptors of 24 bytes (id, type, slots, base, offset, descriptor word) |
| `+0x2e284` to `+0x2e2c8` | `pktSequence`, `pktCount`, `pktSize`, `baseOffset`, `pktFlushOffset`, `currentPkt`, `currentLevelPtr`, `currentHighPtr`, `pktBuffers`, `pktBufferDesc`, `dispatchCurrentPkt`, `forceDispatchPrim`, `forceDispatchAll`, `forceFinish` | the packet-buffer state the dispatch routines keep |
| `+0x2e2cc`, `+0x2e2cd` | `fastClear`, `bigScissor` | `fastClear` picks the second bank of the rectangle-clear register (`0x9e7200a0` instead of `0x99ce00a0`) |
| `+0x2e2d0` | `fbDesc` (76 bytes) | `colorFormat` at `+0x2e30c` (the draw-buffer descriptor word) and `szFormat` at `+0x2e310` (the depth descriptor word) |
| `+0x2e31c` | `colorBitMask` | the colour write mask |
| `+0x2e460`, `+0x2e474`, `+0x2e494` | `pendingState`, `validEnables`, `glCoStateFlags1` | the validation shadow and the flags `GetBoardInfo` sets |

The generic context part holds, among much else, the vertex record: **`__GLvertexRec`, 368 bytes**, with `obj` at `+0x00`, `userNormal` `+0x10`,
`clip` `+0x20`, `flags` `+0x30`, `color` and `secondColor` pointers `+0x34` and `+0x38`, `fog` `+0x3c`, `window` `+0x40`,
`eye` `+0x50`, `texture[2]` `+0x60`, `colors[2]` `+0x80`, `secondColors[2]` `+0xa0`, `normal` `+0xc0`, `materialColor`
`+0xd0`, `lightAtt[4]` `+0xe0`, `lightVPli[4]` `+0xf0` and `lightHAngle[4]` `+0x130`. So the three coordinate sets the
draw-flags word chooses from are object (`+0x00`), clip (`+0x20`) and window (`+0x40`) coordinates, not the unnamed word
ranges the draw-and-buffers page described. Established (declaration), the same page gives the packet routine's use.

**The window record.** The miniport's `_GRMDRAWABLEINFO` is 472 bytes, the size `pCoAllocateDrawable` clears: the drawable's
client and clip regions (`rgnClient` `+0x08`, `rgnClips` `+0x1c`, with the previous pair after them), `pixelFormatId`
`+0x58`, `numTiles` `+0x84` and a 256-byte `tileList`, `drawLock` `+0x188`, `drawIncarn` `+0x18c`, `winOffset`
`+0x190`, **`cidMode` `+0x194`**, **`scrMaskMode` `+0x198`**, `scrMask[5]` of two dwords at `+0x19c`, **`texWinoffsetS`
`+0x1c4`**, **`texWinoffsetT` `+0x1c8`**, `nextSwapFrame` `+0x1cc` and two list links. The three window-parameter words of
the miniport are therefore the valid mask (`scrMaskMode`), five (minimum, maximum) pairs (`scrMask`) and the clip-ID
word (`cidMode`), and the two bias floats are the texture window offsets. The rendering node `_RENDERINGNODE` (1,464 bytes)
names `+0x108` `savedPktOffset`, `+0x4e0` `currentTexId`, `+0x4e4` to `+0x4f0` the context-switch packet page and its offset,
`+0x500` `cxswDumpOffset`, `+0x514` `gfxType`, `+0x518` `nodeStatus`, `+0x560` `waitingEvents`, `+0x58c`
`DrawableInfo`, `+0x5a8` `chunkCount` and `+0x5b0` `serialOnRemove`. In the adapter extension `+0xbd0` is `pktIdleCount`,
`+0xbd8` `CAI_InterruptReg` (the register at the second interrupt address), `+0xbdc` `coBase` and `+0xbe0` `asBase`.
Established (declarations).

**The engine register words.** The program database declares 28 `Co*` types: 23 whose names end in `Reg`, and `CoColor4i`, `CoRgbaType`, `CoFbAddrType`,
`CoPktDescType` and `CoPktRespType`. The 23 are the word types for the registers the packets
load (`CoDrawModeReg`, `CoPrimModeReg`, `CoXformModeReg`, `CoColorFormatReg`, `CoSzFormatReg`, `CoTlbDescReg`,
`CoBlendReg`, `CoColorTestReg`, `CoDepthTestReg`, `CoStencilModeReg`, `CoLogicOpReg`, `CoLightModeReg`, `CoTexGenParamReg`,
`CoTexEnvParamReg`, `CoClearModeReg` and the rest), with bit-field names and positions. They are compared
with every writer in [packet-set.md](../../devices/cobalt-graphics/packet-set.md). The register definitions carry 18 words;
`CoColor4i`, `CoPktRespType`, `CoStippleModeReg` and `CoClearModeReg` are not in them yet.

## The April 1999 build

The OpenGL driver of the NT 4.0 OEM disc set (disc 2, April 1999, file version 1.1.4.0; the files list gives the sizes and
hashes) was read against 5.2.4 with the same scan. It is in the disc's top-level InstallShield 5 cabinet, which `unshield`
opens; an earlier note that no tool opened it had the wrong archive in mind.

- **Names.** Its `.dbg` holds a CodeView table (the NB09 form) of 3,964 public names and no COFF symbol table. The
  Ghidra importer for `.dbg` files used to refuse it ("has no COFF symbol table"); the current one reads NB09 and
  reports the same 3,964 records (3,389 functions and 573 other labels, 80 functions it could not create), and Ghidra finds 3,716 functions in the image. The page's own
  names came from a direct parse of the publics: 3,785 names in `.text` once the import slots are left out, at 3,784 distinct addresses. File-static routines
  such as the material and light loaders have no public record, so 1999 routine names are those of the nearest public,
  not the routine's own. The counts are not comparable with the 3,633 of 5.2.4. Established (the table read in full).
- **Header scan.** The 5.2.4 scan found 57 distinct immediates in the known set (52 base headers and their instance
  variants); the Windows 2000 build gives the same 57. The 1999 build gives 56. It has no `0x97400020` (the texture-id
  reset), no `0x99ce00a0` as an immediate (the rectangle clear is built by arithmetic, see Clears below) and no `0x04a40000`, and it sends the vertex port `0x18c33068`, which 5.2.4 does not
  (a texture-coordinate mask on a port known already). A second pass over the decompiled C of both builds (the literals in the typed
  decompile) agrees on the other differences; it shows `0x9e7200a0` as an immediate in the clear routine only because the decompiler folds the subtraction of `0x618dff60`. No header outside the known 52 plus `0x91ca1020` occurs as an
  immediate in `.text`. The 25 dwords in 5.2.4 and 43 in 1999 whose top 20 bits match a known address and register but
  whose value is not in the set are not immediates (they straddle instruction bytes or sit in data). The scan this round
  also matches the exact header values byte by byte over `.text`, because the linear sweep had lost the viewport header
  `0x0362a0c0` in the 1999 image; the 5.2.4 counts did not change. Established as a scan result; a header the code
  computes is not found by it.
- **Clears.** `__glCoClearBuffers@0x6904fcb0` in the 1999 build does not write the rectangle clear itself: every header it stores is
  among `0x18e84020`, `0x936ae020`, `0x938af040`, `0x98804000`, `0x80003020`, `0x18002020` and the vertex port
  `0x18c33060`, plus the lighting mode word `0x05260020` (`@0x69050413`, `@0x6905093a`) and the draw flags `0x00210020` (`@0x69050657`, `@0x690508ef`). It
  does call the static routine at `0x69050cb0` (no public name; the 5.2.4 `coClearBuffers`), which builds the rectangle clear: the header is
  `(fastClear == 0 ? 0xfb5c0000 : 0) - 0x618dff60`, which is `0x99ce00a0` when the byte at context `+0x311ec` is clear and `0x9e7200a0` otherwise
  (`sbb`, `and eax,0xfb5c0000`, `sub eax,0x618dff60` at `0x69050e52` to `0x69050e60`; `0x618dff60` is the negation of `0x9e7200a0`). The image holds the
  two immediates `0xfb5c0000` and `0x618dff60` and neither header as a raw dword, so a search for the headers finds neither. The same routine queues a kernel clear (escape `0x50`, input `0x70`) when its mode flag is set. The
  earlier statement that a 1999 clear is always a drawn rectangle, and that the engine's rectangle clear was for the miniport's own clears only, is withdrawn.
  Established (both routines read).
- **Texture-id reset.** Without `0x97400020` the 1999 `__glCoCreateTexLevel@0x690219a0` has no texture-id reset before it frees a
  bound texture's pages. Its call sites at `0x69021f60` and `0x690220c1` instead call `__glCoExtEscape`
  with code `0x64`, a 0x20-byte input and a 4-byte output, a code the 1999 display driver's table (`0x40` to `0x63`) does
  not contain; the caller prints its "glCoExtEscape failed" message when the call fails. What the call is for is unknown.
- **The escape call sites.** All 31 call sites of `__glCoExtEscape` in the 1999 ICD were read; the code and the input size
  at each agree with the 1999 table in [graphics-engine.md](../../devices/cobalt-graphics/graphics-engine.md) except the
  two above. Codes `0x4e` (serialise), `0x5f` and `0x60` (gamma) have no call site. Established.
- **The viewport packet** has the same six payload words in the same order (context offsets `0x1a10` to `0x1a28`, the x and
  y offsets less a constant) as in 5.2.4. Established (routine read in full).
- **Configuration variables.** The 1999 image names the simulation variables `GLCOSIMVHDLTEXLOAD`, `GLCOSIMPKTWRITE`,
  `GLCOSIMCOMMONPKT`, `GLCOSIMDEBUG`, `GLCOSIMDEBUGCTXSWITCH`, `GLCOSIMTEXNODEALLOC` and four `GLCOSIMFORCE...BITS`
  and `NOACCUM` ones that 5.2.4 does not; 5.2.4 adds `GLCONOUSERLEVELBINDS`, `GLCOUSERBINDTHRESHOLD`,
  `GLCOUSERLEVELBINDLEVEL` and `GLCONODLISTTEXOBJS`, which fit the texture-table packets the 1999 pack lacks. The
  simulation names back the reading of the image-transfer packet `0x9ed380e0` as a hardware-simulation hook:
  `GLCOSIMVHDLTEXLOAD` is its name in the 1999 build. Inferred (names).
