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
hits. The immediates were attributed to the routine that contains them by the symbol table. Raw hits in data (export
and import tables, jump tables, string pools) occur: the earlier `0x9400014f` was one, in the export address table at
`0x690f2681` where it straddles two entries, which is why every row also rests on a routine whose body was read. The
scan is repeatable with any script that does the same arithmetic; no scanner or listing is committed.
