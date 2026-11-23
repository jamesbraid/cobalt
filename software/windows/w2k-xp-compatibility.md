# Whether the Windows 2000 Cobalt drivers can load on Windows XP

The 5.2.4 Windows 2000 graphics pack is the newest SGI graphics software for the Visual Workstation 320 and 540, and the
only one built for an operating system version XP can host. This page records what the files, the release note and the
public record say about running it on XP, and what they leave open. Nothing here was run: an emulator run would show model
behaviour, not hardware, and none was made.

Source: the SGI Cobalt Graphics Driver 5.2.4 packs for Windows 2000 (patch SG0020052) and Windows NT 4.0 (patch SG0020054);
the Windows XP SP3 kernel (`ntoskrnl.exe`, 5.1.2600.5512) and HAL (`hal.dll`, 5.1.2600.5512) export tables; two newsgroup
replies listed in [the public sources](../../references/documents/INDEX.md). Grades as in [AGENTS.md](../../AGENTS.md).

## Answer

**Inferred: nothing in the files stops the Windows 2000 pack loading on XP, and SGI did not support it.** Every kernel and HAL
import of the miniport exists in the XP kernel and HAL; no file checks for an exact operating-system version; the one version
test is "major version above 4". Two things were not checkable here: XP's `win32k` and video port were not available, so the
display driver's `win32k` imports and the video port's acceptance of the miniport and of an NT 4.0 DDI version are by
documentation only.

## The documented support position. Established as statements.

- The release note of the Windows 2000 pack (12 December 2001) names Windows 2000 and "SGI Version 5.1" of the Cobalt graphics
  driver as the supported configuration and says the software "should not be installed on other configurations". It says the
  update is released as is and not supported by SGI Customer Service. XP reached retail on 25 October 2001, about seven weeks earlier.
- Replies from an SGI address in comp.sys.sgi.misc: in April 2001, asked whether XP would run on a 320 or 540, "Short answer:
  no."; on 26 October 2001, again asked about a dual 320, "Windows XP does not support the 320 or 540." A reader's reply in
  comp.sys.sgi.hardware in January 2002 repeated that Windows 2000 "is and will be the last version to run".
- These are statements about the whole machine (the firmware and the HAL among them), not tests of the graphics
  drivers, and none says what fails. They fix the support position: SGI built, tested and shipped nothing for XP.

## The files

All checks below are on both `sgcodd.dll`, `sgcomp.sys` and `sgcoicd.dll` of the Windows 2000 pack, with the NT 4.0 pack for
contrast.

| Check | Result | Grade |
|---|---|---|
| Image header: minimum operating system, subsystem | Windows 2000 files: OS 5.0; kernel images subsystem 5.0, ICD subsystem 4.0. NT 4.0 files: OS 4.0. No maximum exists in the format. XP is 5.1. | Established |
| `ntoskrnl.exe` imports of `sgcomp.sys` (53 names, e.g. `IoAssignResources`, `ExInitializeZone`, `KeSynchronizeExecution`, `MmAllocateNonCachedMemory`) | All present by name in the XP SP3 `ntoskrnl.exe` export table. The NT 4.0 miniport's set is also all present. | Established |
| `hal.dll` imports of `sgcomp.sys` (`HalGetInterruptVector`, `HalSetEnvironmentVariable`, `KfAcquireSpinLock`, `KfReleaseSpinLock`; the NT 4.0 file also `HalDisplayString`) | All present in the XP SP3 `hal.dll` export table. They are resolved against whatever HAL the guest uses, which here is our HAL, so what matters is that the HAL we ship exports them. | Established for XP's own HAL; open for ours |
| `VIDEOPRT.SYS` imports (`VideoPortInitialize`, `VideoPortMapMemory`, `VideoPortUnmapMemory`, `VideoPortLogError`, `VideoPortStallExecution`, `VideoPortSetRegistryParameters`, `VideoPortZeroMemory`) | The video port of Windows 2000 and later exports all of them; not checked against an XP `videoprt.sys`, which was not available. `HwInitDataSize` is `0x50`, the Windows 2000 layout (the NT 4.0 miniport passes `0x28`), which a later video port is expected to accept. | Inferred |
| `WIN32K.SYS` imports of `sgcodd.dll` (45 `Eng*`, `CLIPOBJ_*`, `PATHOBJ_*`, `WNDOBJ_*`, `XLATEOBJ_*`, `BRUSHOBJ_*`, `PALOBJ_*`, `STROBJ_*` names; the newest are `EngModifySurface`, `EngGradientFill`, `EngAlphaBlend`, `EngSaveFloatingPointState` and `EngRestoreFloatingPointState`, all Windows 2000 additions) | No name later than Windows 2000. Not checked against an XP `win32k.sys`. | Inferred |
| `DrvEnableDriver` | The display driver's DLL entry point fills `iDriverVersion` with `0x20000`, the NT 4.0 DDI version, in **both** the NT 4.0 and the Windows 2000 builds, and the hook count with 32 (NT 4.0) or 35 (Windows 2000). A loader that rejects NT 4.0 DDI drivers would reject the Windows 2000 build too, and it did not (SGI shipped it for Windows 2000). Whether XP's `win32k` still takes `0x20000` was not checked. | Established for the value; inferred for XP |
| Operating-system checks in the miniport | `CobaltFindAdapter` calls `PsGetVersion` and takes the resource-assignment path when the major version is above 4. XP's major version is 5. | Established |
| Operating-system checks in the ICD | One `GetVersionExA`, used only to set a flag when the platform is Windows 95. No build-number or major-version test. | Established |
| Other kernel dependencies | `IoAssignResources` (legacy resource claim, still exported by XP), `HalDispatchTable`, `PsGetVersion`, `ExInitializeZone` and zone calls (still exported). No `Pnp`, `Wmi` or `Po` imports. | Established |

## Dependencies that are ours, not XP's

These are what the same drivers need from the machine, whichever system hosts them; an XP port of the HAL has to supply them
just as the Windows 2000 one does ([the frame-buffer floor](../../devices/cobalt-graphics/framebuffer-driver-floor.md)):
the registry values `GraphicsMemory` and `GraphicsMemorySize` plus the bank list (`NumberOfBanks`, `BankAddrNN`,
`BankSizeNN`) under `SGICtrl\Memory` that the HAL writes, the CPU block revision read at `0xc2000008`, the display back end
at `0xd0000000` and the engine window at `0xc8000000`. None is operating-system specific.

## What is not known

- Whether XP's `win32k` accepts the NT 4.0 DDI version `0x20000` from a display driver, and whether it honours the four
  Windows 2000 hooks the driver reports under that version. Route: a copy of the XP `win32k.sys` and a read of its
  `DrvEnableDriver` handling; none is held. Emulator choice, until then: assume yes and observe.
- Whether XP's video port starts a legacy (no PnP) miniport that claims `0xc8000000` and `0xd0000000` through
  `IoAssignResources` on a machine with our HAL. Same route and same choice.
- Whether XP's `opengl32` accepts the Windows 2000 ICD (it exports the three extra pixel-format and swap entries a later
  `opengl32` asks for, [comparison](w2k-opengl-driver-comparison.md)). Established that those exports exist; not that XP's
  loader finds them sufficient.

## What this means for the reference run

The reference trace of SGI's driver the port is checked against need not come from XP. The Windows 2000 pack is the supported
pairing: Windows 2000 over the same HAL runs the same files, with nothing in the question above in the way, and the OpenGL
and engine-facing code is the same program as the NT 4.0 build ([comparison](w2k-opengl-driver-comparison.md)). An XP run of
the Windows 2000 pack is worth trying as an extra, and a failure to load is not a finding about the port.
