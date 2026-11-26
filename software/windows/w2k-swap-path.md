# How a Windows 2000 SwapBuffers reaches the Cobalt drivers

What happens between an application's `SwapBuffers` and the engine on Windows 2000 with the SGI 5.2.4 graphics pack: which Microsoft
routines run, which of the OpenGL driver's exports are called, what the display driver's `DrvSwapBuffers` does, and why a double-buffered
window can stay empty. The answer is mostly in Microsoft's `opengl32.dll`, which decides whether the OpenGL driver's own
`DrvSwapBuffers` exists as far as it is concerned.

Source: Windows 2000 RTM (build 2195) `\I386\gdi32.dll` (234,256 bytes), `opengl32.dll` (691,984) and `win32k.sys`; Windows 2000 SP4
(build 2195.6717) `gdi32.dll` (233,744) and `opengl32.dll` (692,496); and the SGI Cobalt Graphics 5.2.4 pack for Windows 2000
([files list](../../references/windows-files.md)): `sgcoicd.dll`, `sgcodd.dll` and the installer's tables. The two Windows 2000 SGI files
have no symbols in their `.dbg`, but their `.pdb` files (PDB 2.00) carry the public names and the C type declarations of the OpenGL
driver, so the names and structure fields below are the vendor's own; the Microsoft files have exports only, so their routine
names are descriptions. Every address is an image address (the load address of `sgcoicd.dll` is `0x69000000`, of `sgcodd.dll`
`0x10000`, of `opengl32.dll` `0x69510000`). Grades as in [AGENTS.md](../../AGENTS.md); nothing here was observed on hardware.
A reading marked established covers every instruction of the named routine.

## Answers in one table

| Question | Answer | Grade |
|---|---|---|
| Which routine is the OpenGL driver's per-window swap procedure (window record `+0x64`)? | `__glCoIcdSwapBuffers@8` (`0x69019b62`), stored with the same pointer in `+0x68`, by `__wglCoInitDrawable@8` at `0x69019266`. None of the four candidate stores `0x69016f7b`, `0x69017902`, `0x6901fe3c`, `0x6901fe86` is it. | established |
| Key of the record lookup (`0x6901638e`) | the window handle from `WindowFromDC(hdc)`; records of memory and pixel-buffer drawables key on the DC and cannot be found by a swap | established |
| Does `opengl32` call the OpenGL driver's `DrvSwapBuffers`? | Only when bit 0 of the driver's registry "Flags" is set. The installed pack leaves it clear, so `SwapBuffers` goes to `GdiSwapBuffers` alone and the driver sends nothing | established for the code, inferred for the registry |
| Does `wglSwapLayerBuffers` reach the driver? | Always, through `DrvSwapLayerBuffers`, for every driver-owned pixel format | established |
| What does the display driver's `DrvSwapBuffers` do for a window with a current context? | Returns TRUE and does nothing; it copies only for a window with no context current | established |

## Microsoft's side: `SwapBuffers` and `wglSwapLayerBuffers`

**`gdi32!SwapBuffers`. Established** (RTM `0x77f53859`). It loads `opengl32.dll`, takes `wglSwapBuffers` with `GetProcAddress` and calls it. There is
no other path. `GdiSwapBuffers` (`0x77f69b2e`) is a separate export that goes to the kernel (`win32k`).

**`opengl32!wglSwapBuffers`. Established** (RTM `0x69534068`, SP4 `0x69534086`, the same logic). In order:

1. A device context that a registered callback claims (the routine at `0x6952932f`, inferred to be a DirectDraw surface check) fails with last error 1; a
   context that is neither a display context (type 3) nor a memory context (type `0xa`) fails with last error 6. A memory context returns TRUE at once and
   does nothing.
2. It finds `opengl32`'s own record for the window from the DC (a window handle key) and fails with last error 6 when there is none.
3. The record holds the pixel format index at `+0x3c` and a count at `+0x40`. Index 0 or less returns FALSE with no error code. An index up to
   the count is a driver-owned format (inferred from the set-pixel-format path, which stores both and takes the driver route for an index in
   range). That case calls the helper at `0x6952a098` (SP4 `0x6952a09a`) with finish requested. An index above the count is a software format that
   draws with GDI.
4. **The helper decides.** It finds the driver table of the DC (`0x69528da4`). If the table exists and bit 0 of its flags word is set, it
   calls `glFinish` (unless bit 1 is also set) and then the table's slot `+0x54`, which is the driver's `DrvSwapBuffers`. If bit 0 is clear
   or there is no table, it calls `glFinish` and `GdiSwapBuffers`.

**Where the flags word comes from. Established for the code** (`0x69528da4`, the reader `0x69528b37`, the escape wrapper `0x69528cb8`). The display
driver is asked for a name with the `OPENGL_GETINFO` escape (`0x1101`). The name is looked up under
`HKLM\Software\Microsoft\Windows NT\CurrentVersion\OpenGLDrivers` in one of two layouts:

- a **subkey** with that name holding `DLL` (the file name), `Version`, `DriverVersion` and `Flags`; the flags word is `Flags`, and `Version`
  and `DriverVersion` must equal what the display driver returned;
- a **value** with that name whose data is the file name (the layout of the NT 4.0 era). The flags word is then 0.

In a driver table the entry points load in this order from the start of the table at `+8`: create context, delete context, set context,
release context, copy context, create layer context, share lists, get proc address, describe layer plane, set and get layer palette entries,
realize layer palette, swap layer buffers (`+0x38`), three DirectDraw entries, swap multiple buffers, describe pixel format (`+0x4c`), set pixel
format (`+0x50`), swap buffers (`+0x54`). When bit 0 of the flags is set the three last ones must all be present, or the driver is rejected.

**What the pack gives it. Established for the display driver, inferred for the installer.** The display driver's escape handler (`DrvOpenGLEscape@24`,
`0x18520`, the escape slot of its table) answers `OPENGL_GETINFO` with `Version` 2, `DriverVersion` 2 and the name `sgi` (the wide string at
`0x32934`). The installer's string table carries the key `...\OpenGLDrivers`, the name `SGI` and `sgcoicd.dll`, and none of the value names
`DLL`, `Version`, `DriverVersion` or `Flags`. A Windows 2000 guest installed from the pack holds exactly one value, `SGI`, with the data
`sgcoicd.dll`, and no subkey. That is the value layout, so the flags word is 0.

**Consequence. Established for the code, inferred for the install.** With this registry content the OpenGL driver's `DrvSwapBuffers`,
`DrvSetPixelFormat` and `DrvDescribePixelFormat` exports are never called by `opengl32` (`wglSetPixelFormat`'s helper at `0x6952a053` takes
the `GdiSetPixelFormat` route for a clear bit 0 as well). A plain `SwapBuffers` of a double-buffered window reaches only the display driver
through `GdiSwapBuffers`, which does nothing while a context is current (below). **No swap escape is sent.** SP4's `opengl32` has the same
logic. Whether SGI's hardware ever ran Windows 2000 this way is unknown: no other file in the pack writes these values, but nothing here shows
what a field machine held.

**`opengl32!wglSwapLayerBuffers`. Established** (RTM `0x69527125`). After a `glFinish` when a context is current, it asks GDI for the format index. For a
driver-owned format it finds the same driver table and calls slot `+0x38`, the driver's `DrvSwapLayerBuffers`, **without testing bit 0**. When the
slot is empty it falls back to `SwapBuffers` for the main plane. So the layer call reaches the OpenGL driver for every format of the pack, and the
plain call does not.

## The OpenGL driver's two swap exports

**The window record. Established** (the PDB declares `__WGLdrawablePrivateRec`, 0x7bc bytes, and `__WGLcontextRec`, 0x70 bytes). In the record:
`+0xc` is the DC, `+0x10` the window handle, `+0x38` the pixel format, `+0x3c` the swap rectangle list (`swapRgn`), `+0x40` its size, `+0x44` the swap interval,
`+0x64` `swapBuffers`, `+0x68` `swapBuffersMain`, `+0x6c` `swapBuffersFallback` (cleared), `+0x70` the generic drawable (`glPriv`), `+0x378` its mutex and
`+0x3b4` a reference count. The context record's `+0x64` is `pfnSetProcTable` (the callback `opengl32` supplies, stored by `DrvSetContext` at
`0x69016f7b` and by `wglMakeCurrentReadSGI` at `0x69017902`), which is why those two stores are not the swap routine. The two stores at `0x6901fe3c` and
`0x6901fe86` are in `__glCoCreateProxyTexLevel@36` and fill a texture level.

**Creation and lookup. Established** (`__wglGetDrawablePrivate@12` `0x690164d7`, `__wglFindDrawablePrivate@8` `0x6901638e`, `__wglCreateDrawablePrivate@12`
`0x69016112`). The records live in one list under a global mutex. A record is created only when a context is made current
(`DrvSetContext`, `wglMakeCurrentReadSGI`, the layer palette routines and pixel-buffer creation call the getter; `DrvSwapBuffers` and `DrvSwapLayerBuffers`
call only the finder). The finder takes a DC key and a window handle. A record whose window handle is nonzero matches on the handle alone;
a record with handle 0 (a memory or pixel-buffer drawable) matches on the DC. Both swap exports pass `-1` as the DC and `WindowFromDC(hdc)` as the
handle, so they find only windowed records. `DrvSetContext` takes the handle from `WindowFromDC` for a display context, so the same window
finds its own record. The record is destroyed when its reference count reaches zero: `DrvReleaseContext` (`0x69017003`) or `DrvDeleteContext` of the last
context, or the window's destruction (the driver subclasses the window procedure with a stub that calls `__wglDestroyHwnd`).

**A lookup miss therefore needs one of three things.** No context was ever made current on the window with this driver (the format having gone
through `GdiSetPixelFormat` does not matter, since the driver's `DrvSetPixelFormat` export is only a forwarder to `GdiSetPixelFormat`), every context has
since been released, or `WindowFromDC` returned a different or a null handle for the DC passed to the swap. A miss makes `DrvSwapBuffers` and
`DrvSwapLayerBuffers` return FALSE with no error code. The thread's current context being null is a second FALSE source (below).

**`DrvSwapBuffers` (`0x690175db`) and `DrvSwapLayerBuffers` (`0x69017511`). Established.** They lock the global mutex, find the record, lock its mutex and set bit 2 of
`pendingState` of every context on the record, unlock, and call `swapBuffers(record, planes)`: `DrvSwapBuffers`
always passes 1, `DrvSwapLayerBuffers` passes its plane mask. **If the procedure returns 1 they then call `GdiSwapBuffers` (`0x69016ceb`, which loads `gdi32` and
forwards) and return its result; any other return is returned as it is.** Then they clear the swap rectangle list and unlock. So the OpenGL
driver swaps in its own procedure and calls GDI afterwards, for the window system's bookkeeping.

**`__glCoIcdSwapBuffers@8` (`0x69019b62`). Established.** It takes the thread's current OpenGL context from the thread block slot at `+0xbf0` and
returns **0 without sending anything when there is none**. Otherwise it dispatches the open node with request 2 (flush) and then:

- **single-buffered context** (`modes.doubleBufferMode`, `+0x245a`, is 0): it calls the context's `finish` and **returns 1 without an escape**;
- **double-buffered context:** it builds the 0x7c-byte block (the swap rectangle hint if the record's list is non-empty, then clears the list; the swap
  interval; flags: 1, becoming 3 when bit 0 of the plane argument is set, with 4 added for bit 1) and sends **escape `0x48` itself** through
  `__glCoExtEscape`. It returns 1 when the escape returns a positive value. `DrvSwapBuffers` and `wglSwapLayerBuffers(hdc, 1)` therefore put the same block
  on the wire, the same escape and the same drop type 4 ([escapes](opengl-escapes.md#the-table)).

## The display driver's side

**Pixel format and window object. Established** (`DrvSetPixelFormat@12` `0x30f60`, `___glDrvCreateDrawable@20` `0x307ce`, `CoGrmWndObjAllocate@4` `0x1d568`). It is
the only caller of `EngCreateWnd`, with flags `0x42` and the callback `CoGrmWndObjCallBack@8` (`0x1d5e0`). It builds a 0x40-byte drawable object
(window handle `+4`, the window object `+8`, id `+0xc`, the **context list head `+0x10`**, pixel format `+0x14`, a copy of the 0x28-byte pixel format descriptor `+0x18`), puts it on a
global list, and stores it as the first word of a 0x30-byte consumer record that `WNDOBJ_vSetConsumer` attaches. A second set for the same window fails (the
object already exists). `___glCoDrvSetContext@16` (`0x1db40`) fails with `0xc000000d` when it cannot find the object for the window, so a context
cannot be made current on a window that never went through `DrvSetPixelFormat`.

**Escape `0x40` is the one at `0x18708`. Established** (the table of 0x25 handlers at `0x197ae` is indexed by the escape code minus `0x40`; the entry for `0x40`
starts at `0x186e5`). It checks a 0x54-byte input and an 8-byte output and calls `___glCoDrvSetContext`: it **makes a context current on a window**,
and `___glDrvSetDrawable` (`0x3087c`) puts the context on the `+0x10` list of the draw window's object and, when the read window differs, on that of the
read window's. Its neighbours: `0x41` first set context (`___glCoDrvFirstSetContext`), `0x42` create layer context, `0x43` delete context, `0x44` release
context, `0x48` swap (a drop of type 4 on the context's rendering node).

**Where contexts leave the list. Established.** `___glDrvReleaseDrawable@8` (`0x308ec`) removes the entry and is called from:
`___glCoDrvReleaseContext@4` (`0x1d8ec`) at `0x1d913` for the draw window and `0x1d924` for the read window (escape `0x44`, and `___glCoDrvDeleteContext`
runs it first when the context is still current); and `___glDrvReleaseContext@4` (`0x3093c`) at `0x3094a` for the window recorded in the context, which the
window object's deletion callback (`___glDrvWndObj@8`, flag `0x20`) calls once for every context still on the list before it frees the object. Nothing
else removes an entry. A context stays on the list from its make-current until its release, delete or the window's destruction.

**`DrvSwapBuffers@8` (`0x312ae`, DDI index 56). Established.** It returns TRUE and does nothing when the window object is missing, the consumer record is missing, **or
the object's `+0x10` list is non-empty, that is whenever any OpenGL context is current on the window.** Otherwise it copies: for a trivial clip it runs
`vCopyBackToFront@8` (`0x12b3a`) once on the window's bounding rectangle when it has area, and for any other clip once per rectangle from `WNDOBJ_bEnum`.
So **the ICD is expected to do the swap** for a window with a current context; the display driver's own swap is for a window whose contexts are all released.

**`vCopyBackToFront`. Established for the packets, inferred for the word meanings.** It builds one chain on the display driver's own node: when the draw buffer
differs from the cached one it writes the draw-buffer word `pdev+0x6dc`; a mode word `0x80003020` and mode `0xc0000`; the texture word `pdev+0x6e4` and three zero
words; the environment word `0x62000000`; the bias pair 0.5, 0.5; and a primitive per rectangle (the `0x18002020` mode word, then vertices `x y s t` with the texture
coordinates equal to the pixel position). The two words are set in `bInitializeModeFields@16` (`0x21364`, `0x21381`) from a per-depth base word
(`0x24000000` for 32 bits per pixel, `0x12000000` in the 16-bit branch) or'd with `0x100000` and `0x180000`: **the global front buffer (descriptor 2) as draw buffer and
the global back buffer (descriptor 3) as the texture**, the same pair the miniport's swap copy uses
([swap](opengl-swap-points-formats-faces.md#buffer-swap-an-engine-copy-or-a-flip-of-the-displayed-tiles)). The copy does not go through the miniport's retrace hold.
It is **never used for a window that has a current context**, and so for no OpenGL window while the application renders.

## What an emulator must do

- Nothing for the plain `SwapBuffers` of a double-buffered window with the pack as installed: the guest sends no swap drop and the window keeps what it
  had. The engine's copy and flip are reached only by escape `0x48`, which the OpenGL driver sends from its swap procedure. A black or unchanged window after
  `SwapBuffers` on Windows 2000 is what the shipped software asks for, not an engine fault. Inferred (the install's registry decides).
- `wglSwapLayerBuffers(hdc, 1)` is the path that sends the escape. It must work as the NT 4.0 swap does.
- To exercise the OpenGL driver's `DrvSwapBuffers`, give the guest the subkey layout: under `OpenGLDrivers`, a key `sgi` with `DLL` = `sgcoicd.dll`,
  `Version` = 2, `DriverVersion` = 2 and `Flags` = 1 (and no `SGI` value of the same name). That is the configuration the exports exist for. Inferred; not
  tried.
- The display driver's escape for `OPENGL_GETINFO` must keep answering 2 and 2; the registry check compares them.

## Corrections to earlier pages

[The comparison page](w2k-opengl-driver-comparison.md) called the three added exports small forwarders. Two are (`DrvDescribePixelFormat`, `DrvSetPixelFormat`).
`DrvSwapBuffers` is the full routine above, and it is the thing that sends the escape. The `GdiSwapBuffers` forwarder it calls is at `0x69016ceb`; the one at
`0x69016d71` is `GdiSetPixelFormat`.
