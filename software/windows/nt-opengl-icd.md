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
pages cite. The 4.2 MB `.pdb` was not
needed for names. The `.pdb` carries
types and local variable names the project has not used. Any claim about a routine therefore cites a vendor name and an
address, for example `__glCoSetScissor` at `0x69018ebe`.

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
| `Generate*` and `__glGenerate*` (the `__glCoGenerate*` ones are in the row above) | 19 | run-time code generators for vertex handling (below) |
| the rest | 2,297 | the generic OpenGL machinery from the vendor's base code, including the span, store and image-pipe routines used for fallbacks: evaluators, texture state, clipping, lighting maths, colour tables |

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
render procedure (offset `0x3164`) otherwise. They contain no header constants.

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

## The April 1999 build

The OpenGL driver of the NT 4.0 OEM disc set (disc 2, April 1999, file version 1.1.4.0; the files list gives the sizes and
hashes) was read against 5.2.4 with the same scan. It is in the disc's top-level InstallShield 5 cabinet, which `unshield`
opens; an earlier note that no tool opened it had the wrong archive in mind.

- **Names.** Its `.dbg` holds a CodeView table (the NB09 form) of 3,964 public names and no COFF symbol table, so the
  Ghidra importer for `.dbg` files refuses it ("has no COFF symbol table") and names came from a direct parse of the
  publics: 3,785 names in `.text` once the import slots are left out, at 3,784 distinct addresses. File-static routines
  such as the material and light loaders have no public record, so 1999 routine names are those of the nearest public,
  not the routine's own. The counts are not comparable with the 3,633 of 5.2.4. Established (the table read in full).
- **Header scan.** The 5.2.4 scan found 57 distinct immediates in the known set (52 base headers and their instance
  variants); the Windows 2000 build gives the same 57. The 1999 build gives 56. It has no `0x97400020` (the packet-pointer
  reset) and no `0x99ce00a0` (the rectangle clear), and it sends the vertex port `0x18c33068`, which 5.2.4 does not
  (a texture-coordinate mask on a port known already). No header outside the known 52 plus `0x91ca1020` occurs as an
  immediate in `.text`. The 25 dwords in 5.2.4 and 43 in 1999 whose top 20 bits match a known address and register but
  whose value is not in the set are not immediates (they straddle instruction bytes or sit in data). The scan this round
  also matches the exact header values byte by byte over `.text`, because the linear sweep had lost the viewport header
  `0x0362a0c0` in the 1999 image; the 5.2.4 counts did not change. Established as a scan result; a header the code
  computes is not found by it.
- **Clears.** `__glCoClearBuffers@0x6904fcb0` in the 1999 build never writes the rectangle clear: every header it stores is
  among `0x18e84020`, `0x936ae020`, `0x938af040`, `0x98804000`, `0x80003020`, `0x18002020` and the vertex port
  `0x18c33060`, so a clear is always a drawn rectangle. Established (routine read for its stores). The engine's
  rectangle clear exists for the miniport's own clears in the 1999 pack.
- **Packet-pointer reset.** Without `0x97400020` the 1999 `__glCoCreateTexLevel` has no pointer reset before it frees a
  bound texture's pages. The 1999 `__glCoCreateTexLevel@0x69021f60` and `@0x690220c1` instead call `__glCoExtEscape`
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
