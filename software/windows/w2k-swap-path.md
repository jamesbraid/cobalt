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
| Which routine is the OpenGL driver's per-window swap procedure (window record `+0x64`)? | `__glCoIcdSwapBuffers@8` (`0x69019b62`), stored in `+0x64` (at `0x69019266`) and `+0x68` (at `0x69019263`) by `__wglCoInitDrawable@8`, which clears `+0x6c` at `0x6901925f`. None of the four candidate stores `0x69016f7b`, `0x69017902`, `0x6901fe3c`, `0x6901fe86` is it. | established |
| Key of the record lookup (`0x6901638e`) | the window handle from `WindowFromDC(hdc)`; records of memory and pixel-buffer drawables key on the DC and cannot be found by a swap | established |
| Does `opengl32` call the OpenGL driver's `DrvSwapBuffers`? | Only when bit 0 of the driver's registry "Flags" is set. The Supplemental Software Kit (SSK) v5.1 installer writes a subkey with `Flags` = 1, so on a machine set up the way SGI shipped it, `SwapBuffers` reaches the driver. The 5.2.x graphics patches write only the plain value, and a guest given a patch alone has no `Flags` | established for the code and for what the SSK installer's tables carry; inferred that the installer ran as written (not run) |
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

**What the installs give it.** The display driver's escape handler (`DrvOpenGLEscape@24`, `0x18520`, the escape slot of its table) answers
`OPENGL_GETINFO` with `Version` 2, `DriverVersion` 2 and the name `sgi` (the wide string at `0x32934`). Established (read in full). Two kinds of
SGI installer then register the driver, and they differ:

- **The 5.2.1, 5.2.3 and 5.2.4 graphics patches** (`SGICoGraphics521Win2KJPN.exe`, `SGICoGraphics523Win2KJPN.exe`, `SGICoGraphics524w2k.exe`;
  their release notes say they update a system that already has the SSK v5.1) register the key `HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\OpenGLDrivers` with
  **one value**, name `SGI`, data `sgcoicd.dll`, and no `DLL`, `Version`, `DriverVersion` or `Flags` name anywhere in the header (all three read: the string tables hold the
  name once, in the key's record). A guest given a patch alone ends up in the value layout and `Flags` is 0. Established for the header's
  records (the record layout was checked against the same table's service keys); inferred that the installer writes them as the records say (never run here).
- **The SSK v5.1 installer** (`sgi_ssk.exe`, English, and `sgi_ssk-sgi-jp.exe`; the release notes are `ssk_relnotes.rtf` in the same pack: it is the SGI
  upgrade kit for a 320 or 540 moving to Windows 2000, installed after Windows 2000 itself) carries the OpenGL driver as the component `Sgcoicd`
  (build date 9 December 1999, 1,119,808 bytes) with registry entries under **`...\OpenGLDrivers\SGI`**: `DLL` = `sgcoicd.dll`, `Flags` = 1, `DriverVersion` = 2,
  `Version` = 2. In that table's record format the string type code is 2 and the number type code is 3, the codes the same table gives
  `Group` = `Base` (string) and `Start`, `Type`, `ErrorControl`, `Interrupt` (numbers, decimal text) of its service keys, so `DLL` is a string and the other three are numbers
  (inferred from the codes; the type constant itself is not documented). Both kits' tables carry the same four entries.
  This is the layout the exports exist for: `opengl32` reads the subkey first and its type test wants a string for `DLL` and a 32-bit number for the three others.

**Consequence.** A Windows 2000 machine installed the way SGI's notes describe (Windows 2000, then the SSK, then the 5.2.x patch) has both the subkey
and the value, and `opengl32` takes the subkey: bit 0 of `Flags` is set, `SwapBuffers` runs the OpenGL driver's `DrvSwapBuffers`, the swap escape is sent, and the
display driver's `DrvSwapBuffers` runs afterwards through `GdiSwapBuffers` and does nothing while a context is current. So **the field behaviour is the
escape-`0x48` path of the layer swap below**, not a swap that does nothing. A machine with a graphics patch and no SSK (the Windows 2000 drivers on a
guest, say) has `Flags` 0: `opengl32` never calls the OpenGL driver's `DrvSwapBuffers`, `DrvSetPixelFormat` or `DrvDescribePixelFormat`, `wglSetPixelFormat`'s helper at
`0x6952a053` takes the `GdiSetPixelFormat` route, and a plain `SwapBuffers` of a double-buffered window reaches only the display driver, which does nothing while a
context is current. SP4's `opengl32` has the same logic. Established for the code; inferred for what an installed SSK writes (the installer was read, not run).

**`opengl32!wglSwapLayerBuffers`. Established** (RTM `0x69527125`). After a `glFinish` when a context is current, it asks GDI for the format index. For a
driver-owned format it finds the same driver table and calls slot `+0x38`, the driver's `DrvSwapLayerBuffers`, **without testing bit 0**. When the
slot is empty it falls back to `SwapBuffers` for the main plane. So the layer call reaches the OpenGL driver for every format of the pack, and the
plain call does not.

## What the kernel's `NtGdiSwapBuffers` does

`GdiSwapBuffers` in `gdi32` (RTM `0x77f69b2e`) pushes its argument and calls a system-call stub (`mov eax, 0x111e`, table 1, index `0x11e`) and returns the kernel's
result unchanged. Windows 2000 RTM `win32k.sys` (1,726,256 bytes, preferred base `0xa0000000`) registers its service table at `0xa01859f0` (the `push` at
`0xa01945b4`); entry `0x11e` is `NtGdiSwapBuffers` at **`0xa0134500`** (the neighbour at `0xa013432d`, entry `0x111`, is `NtGdiSetPixelFormat`, called by
`GdiSetPixelFormat`). The file has no symbols, so every name below is a description. SP4's `win32k.sys` was not available to read. Established (read in full).

Calls, in order, with what returns FALSE (the routine's results are 1 or 0):

| Step | FALSE or TRUE | Last error |
|---|---|---|
| Look up and lock the device context from the handle (`0xa001478c`). Fails | FALSE | 6 (invalid handle) |
| The context's surface pointer (`+0x1dc`) is 0, a context with no device surface behind it | FALSE | 6 |
| The context's type (`+0x14`) is 1, taken to be a memory context (inferred from the value; it matches what `wglSwapBuffers` does for one) | TRUE, no driver call | |
| Lock the context for window access (`0xa007dc61`, which wraps `0xa0014917`). That lock **fails** when the context has the synchronise flag (bit 8 of the flags word at `+0x18`) and, after waiting for its semaphore, either bit 11 (`0x800`) is set or bit 4 is set and the region update `0xa0017006` returns 0. Then the routine `0xa0027568` decides: it returns TRUE if bit 11 is set, FALSE if not (the context has a surface here, so that is the only test left) | TRUE if bit 11, else FALSE | none set |
| Find the window object. For a display context of the direct type the lock routine searches a global list (head at `[0xa0186dbc]+0x24`) for an entry keyed by the first dword of the context object and takes the object it points to; a miss, an empty entry, or an object that cannot be referenced leaves the object pointer 0. The swap then fails | FALSE | 6 |
| The window object's bounds (the `CLIPOBJ` rectangle at `+4`): left equals right, or top equals bottom (an empty client area) | TRUE, no driver call | |
| Call the driver's `DrvSwapBuffers` through the PDEV's table slot `+0xc6c` (the slot `NtGdiSetPixelFormat` tests before it calls `DrvSetPixelFormat` at `+0xc64`) with the driver's surface and the window object. Slot empty, or the driver returned FALSE | FALSE | none set |
| Driver returned TRUE | TRUE | |

So with the display driver returning TRUE, the call can still return FALSE on four paths that run before it: an invalid context (6), a context with no surface (6),
a failed window lock whose context lacks flag `0x800` (no error), and a window object that is missing (6). The routine's last error tells two groups apart:
**6 is the lookup group, 0 is the lock or table-slot group.** Which one a real run took is decided by one breakpoint on the return of `0xa0134500` plus `GetLastError`
after the failed call; no run has examined it (unknown, and an emulator trace that reaches the display driver's `DrvSwapBuffers` has already passed the lookup, because
the window object is the second argument that call receives).

The kernel never sees the application's draw buffer: nothing in this routine reads a context's `glDrawBuffer` state, so a failure that appears only with the front buffer
selected is not a kernel decision about the buffer.

**Quake II 3.20.** The renderer DLL of the Quake II build used here (`ref_gl.dll`, 234,496 bytes) calls `wglSwapBuffers` in `GLimp_EndFrame` only when the cvar `gl_drawbuffer` is
the string `GL_BACK` (the string compare against `GL_BACK`, its address pushed at `0x1000e70b`, the error string `GLimp_EndFrame() - SwapBuffers() failed!` at `0x1002f62c`), and calls `Sys_Error` when
it returns 0. So a "SwapBuffers() failed" with `gl_drawbuffer` at `GL_FRONT` means the failing call ran in a frame where the cvar still read `GL_BACK` (the first frames, before the
setting was applied), not in the front-buffer frames. Established (the function read in full); that this is what happened in a given run is inferred, since the frame number of the
failure was not looked at.

## The OpenGL driver's two swap exports

**The window record. Established** (the PDB declares `__WGLdrawablePrivateRec`, 0x7bc bytes, and `__WGLcontextRec`, 0x70 bytes). In the record:
`+0xc` is the DC, `+0x10` the window handle, `+0x38` the pixel format, `+0x3c` the swap rectangle list (`swapRgn`), `+0x40` its size, `+0x44` the swap interval,
`+0x64` `swapBuffers`, `+0x68` `swapBuffersMain`, `+0x6c` `swapBuffersFallback` (cleared), `+0x70` the generic drawable (`glPriv`), `+0x378` its mutex and
`+0x3b4` a reference count. The context record's `+0x64` is `pfnSetProcTable` (the callback `opengl32` supplies, stored by `DrvSetContext` at
`0x69016f7b` and by `wglMakeCurrentReadSGI` at `0x69017902`), which is why those two stores are not the swap routine. The two stores at `0x6901fe3c` and
`0x6901fe86` are in `__glCoCreateProxyTexLevel@36` and fill a texture level.

**Creation and lookup. Established** (`__wglGetDrawablePrivate@12` `0x690164d7`, `__wglFindDrawablePrivate@8` `0x6901638e`, `__wglCreateDrawablePrivate@12`
`0x69016112`). The records live in one list under a global mutex. A record is created only when a context is made current
(`DrvSetContext`, `wglMakeCurrentReadSGI` and `__wglSetLayerPaletteEntries` call the getter, which calls the finder and then the creator;
`wglCreatePBufferSGIX` calls the creator directly; `DrvSwapBuffers`, `DrvSwapLayerBuffers` and the other layer palette routines call only the finder). The finder takes a DC key and a window handle. A record whose window handle is nonzero matches on the handle alone;
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
window object's deletion callback (`___glDrvWndObj@8`, flag `0x20`) calls once for every context still on the list before it frees the object. The only other
caller of `___glDrvReleaseContext` is `___glCoDrvDeletePBuffer` (at `0x1e3cd`), which does the same for a pixel buffer's own drawable object. Nothing else
removes an entry from a window object's list. A context stays on the list from its make-current until its release, delete or the window's destruction.

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

- Give the guest the registry SGI's installer writes (below), then a plain `SwapBuffers` sends the same swap escape `0x48` the layer swap does and must work as the NT 4.0 swap does.
  A guest with a graphics patch but no SSK has a plain `SwapBuffers` that sends no swap drop: the window keeps what it had, which is what that registry asks for, not an engine fault.
  Established for both registry states (code and installer tables); the SSK has not been run here.
- `wglSwapLayerBuffers(hdc, 1)` is the path that sends the escape. It must work as the NT 4.0 swap does.
- The subkey to give the guest, the one the SSK installer writes: under `OpenGLDrivers`, a key `SGI` with `DLL` = `sgcoicd.dll` (a string),
  and `Version`, `DriverVersion` and `Flags` as 32-bit values 2, 2 and 1. `opengl32` tries the subkey first, so an `SGI` value beside it is ignored, and a
  subkey with any of the four missing or mistyped makes the lookup fail instead of reading the flags as 0. An emulator run with exactly this subkey made
  plain `SwapBuffers` work for the five GL tests (model output, not hardware), and `DriverVersion` 1 made context creation fail, as the code says.
- The display driver's escape for `OPENGL_GETINFO` must keep answering 2 and 2. `opengl32` rejects a driver whose `Version` is not 2, passes `DriverVersion`
  to the OpenGL driver's `DrvValidateVersion` (`0x69017598`, true only for 2), and in the subkey layout also compares both with the registry values.

## Corrections to earlier pages

[The comparison page](w2k-opengl-driver-comparison.md) called the three added exports small forwarders. Two are (`DrvDescribePixelFormat`, `DrvSetPixelFormat`).
`DrvSwapBuffers` is the full routine above, and it is the thing that sends the escape. The `GdiSwapBuffers` forwarder it calls is at `0x69016ceb`; the one at
`0x69016d71` is `GdiSetPixelFormat`.
