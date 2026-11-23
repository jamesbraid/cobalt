# Cobalt Reverse Engineering Project

This project reverse-engineers the graphics of the SGI Visual Workstation 320 and 540 (1999, x86, Windows NT 4.0): the **Cobalt graphics engine** (its command and packet set, register window and semantics), the **Arsenic display back end** (timing, scan-out, palette, gamma, cursor), and the way SGI's software drives them. The goal is to document the hardware as register definitions and notes that emulator authors, FPGA builders and the vintage SGI community can work from. Layout and working style follow techomancer's [gr2](https://github.com/techomancer/gr2) and [gr4](https://github.com/techomancer/gr4). Where this file is silent, do what gr4 does.

## 1. Scope and sources

The primary source is SGI's Windows graphics stack: the OpenGL driver `sgcoicd`, the display driver `sgcodd`, the miniport `sgcomp`, and from Windows 2000 the frame buffer pair `sglfb` and `sgibvid`. Linux (`sgivwfb`, the visws platform code) is secondary. Public documents are the SGI datasheet and product guide and the trade-press articles listed in [references/documents/INDEX.md](references/documents/INDEX.md).

Out of scope here, covered in the sibling [chips](https://github.com/jamesbraid/chips) repository: the platform around the graphics (the Cobalt memory controller, interrupt controller and CPU interface, the Lithium bridges, the Windows HAL, the PROM and the boot path). Cite those pages by link; do not copy them.

Facts are labelled with one of three ratings. **established**: read in full from a shipped binary (the whole function, table or code path), a published document, or a public driver, cited with the release, function, page or file. **inferred**: a reasoned reading, with the reasoning. A reading built from call lists, store summaries, a name, a sibling chip or a partial read is inferred, and so is any approximation, with its limit stated. An emulator run is inferred, never established: it shows model behaviour, not hardware. **unknown**: nothing published or shipped decides it. Anything unlabelled is unknown. Do not fill gaps from memory, other graphics boards or other emulators.

The pages moved in from chips were written with five grades (established, software expectation, observed, inferred, unknown) and have been regraded to the three above; [their overview](devices/cobalt-graphics/README.md#grades) says how. New and revised text uses the three. The regrade by mapping is listed in [COVERAGE.md](COVERAGE.md).

## 2. Binaries

Proprietary SGI binaries are not tracked. Copy the files [references/windows-files.md](references/windows-files.md) lists from your own copy of the SGI Cobalt Graphics 5.2.4 driver pack (and the Windows 2000 CD) into the ignored `root/`. Disassembly, decompiled C, headers and notes derived from them are committed as findings in your own words; instruction listings and firmware images are not.

## 3. Deliverables

Register and word layouts live in `devices/<unit>/registers.toml`, one file per unit, with a grade on every item. `generated/` and `devices/<unit>/registers.md` are written from it by `retools/tools/defs/defs-gen` and never edited by hand. To change a layout, edit the TOML, run `defs-gen`, then `retools/tools/defs/defs-check --build`, and commit both. The rendering engine window (`devices/rendering-engine/`) and the display back end (`devices/arsenic/`) each have one. Everything else goes in pages under `devices/` and `software/`, in your own words, each finding labelled with its rating and derivation.

## 4. Toolchain

The host needs Podman, git and python3; everything else runs in the container built from [retools/tools/Containerfile](retools/tools/Containerfile) via `retools/tools/re-run COMMAND`. [tools/setup-clone.sh](tools/setup-clone.sh) prepares a fresh clone. Run shared tools by their submodule path from the repository root (`retools/tools/<name>`). A change to a tool is made in retools and the pin here moves with a commit. `tools/` keeps only the setup script and `tools/config/`. The shared tools descend from gr4's and carry its BSD-3 notice ([retools/tools/LICENSE-gr4](retools/tools/LICENSE-gr4)).

## 5. Evidence: software only, any source

No hardware is available. Do not plan, request or wait for measurements. Every question is answered from software and published material, and a row is not finished while either still has something to read. Readers may open every file of the vendor package, the SGI datasheet and product guide, public drivers and emulators, manuals and man pages, and published output from real machines (`hinv`, X logs, screenshots in posts and archives).

Running the shipped software under an emulator shows the model's behaviour and the driver's expectations. It is never hardware evidence; label it that way. Write findings in your own words. Never paste vendor source, firmware or binaries; firmware images and driver packages stay in `root/`, so describe their format and cite offsets.

Use "established" only for what you read in full: the whole function body or table, or the passage in a document. A reading built from call lists, register-store summaries, a sibling chip or a name is "inferred". Never reword an entry to change its grade. An entry written quickly and not yet checked ends with "Unverified." and grades at most inferred. Trade press and the datasheet's feature list are cross-checks. A fact about what the chips do, or about their wiring, needs the vendor software or an SGI document.

When every route is exhausted, label the fact "unknown: no shipped software or published source depends on it" and list the routes tried; an emulator author then chooses a value and documents it. Never write "needs capture". An open question is not a stopping point: record it in `questions/open-questions.md` with the routes tried and the next one, and carry on. When it is answered, move it into the owning page and delete the entry.

## Evidence order

For a register or behaviour fact, take it from the highest source that states it and grade it by that source:

1. SGI's own published documents (datasheet, product guide, owner's guides) and any datasheet of a part that is shown to be on the board: write the fact in your own words and cite document, revision and page. A fact one of them defines is `established` for that part.
2. Shipped binaries: confirm what the software does, graded established where read in full, and note which release.
3. The public Linux visws code (`sgivwfb`, `visws_quirks.c`, the `asm/visws` headers): `established` once a document or binary confirms them. It covers the display back end only; no public driver for the rendering engine exists.
4. Notes from other projects (other projects' datasheets, manual pages and emulator code): may confirm or contradict, never supply a fact. A fact resting only on one stays `unknown`, with the note named.

Reset values: `established` when a datasheet states a power-on default; `inferred` when the software depends on a value before its first write; a read after firmware initialisation is not a reset value and can only rule values out; otherwise `unknown`. A physical hardware read is a last resort, needs the repository owner's approval, and counts as a reset value only for a register firmware provably never writes. Emulator runs are model behaviour, never hardware evidence.

Full text and the checks that enforce it: `retools/tools/README-analysis.md#evidence-order`.

## 6. When work is finished

A coverage row is done when every item in its area is `established` or labelled as having no software dependency, each with a derivation (release, binary, install path under `root/`, offset, callee) in the same entry. Hand off only when your area meets that definition or the session ends; then say in `questions/open-questions.md` where you stopped.

## 7. Rules

- Software-only reverse engineering from public sources and the shipped package. Nothing else is an input.
- Name no emulator projects, private tooling, hosts, people or local storage paths in committed files, and do not record where your copy of the files came from. Give no capture, trace or machine a nickname. Do not quote internal documents. Quote only public sources, briefly, with title and URL in `references/documents/INDEX.md`. `retools/tools/leakcheck.py` enforces this (allow-list `tools/config/leak-allow.txt`) and refuses commit trailers.
- Update `COVERAGE.md` in the same commit as the finding. Commit disassembly and traces you produce as data; keep the proprietary binaries in `root/`.
- No trailers in commit messages: no attribution, sign-off or generated-with lines.
- Commit small and often, straight to `main`. Subjects are lowercase, component-prefixed imperatives (`devices/cobalt-graphics: record the clear packet header`); bodies say why. Write everything for a stranger.

## Where new work goes

There is no `evidence/` directory. File each page by what it is about:

- `devices/<board>/`, `platforms/<board>/`, `formats/<name>/`, `software/<area>/`: a note about one device, platform, format or software component.
- `references/`: what we hold and cite: binary lists with checksums, scope lists, source lists. Code submodules stay under `references/code/`.
- `generated/`: tool output that is regenerated and never hand-edited (register output; inventory tables once a root exists).
- `COVERAGE.md` at the root: progress tallies and the areas still open.
- `questions/`: open questions.

`retools/tools/defs/ci-check` fails when an `evidence/` directory comes back.
