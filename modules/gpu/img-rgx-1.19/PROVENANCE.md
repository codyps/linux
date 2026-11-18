# Provenance

This repository contains the PowerVR Rogue DDK 1.19 kernel module (KM)
source, extracted from a publicly available Linux kernel fork. PocketForge
maintains this fork to build `pvrsrvkm.ko` and `dc_sunxi.ko` from source
against the PocketForge-owned kernel (`pocketforge-os/kernel-tsp`).

## Upstream source

| Field            | Value |
|------------------|-------|
| Repository       | [DC-DeepComputing/fml13v01-linux](https://github.com/DC-DeepComputing/fml13v01-linux) |
| Branch           | `fm7110-6.6` (default branch) |
| Commit SHA       | `97c64fe2832b6826914b6da7aa4febcdd4d3d444` |
| Commit date      | 2025-05-13 |
| Commit message   | `riscv: configs: fml13v01_defconfig: undefined CONFIG_LOCALVERSION_AUTO` |
| Extraction path  | `drivers/gpu/drm/img/img-rogue/` |
| Extraction date  | 2026-06-14 |

## DDK version

| Field               | Value |
|----------------------|-------|
| DDK branch           | `1.19` |
| DDK build            | `6345021` |
| DDK version string   | `Rogue_DDK_Linux_WS rogueddk 1.19@6345021` |
| Source file          | `include/pvrversion.h` |

## Target silicon

| Field   | Value |
|---------|-------|
| GPU     | Imagination GE8300 (PowerVR Rogue) |
| BVNC    | 22.102.54.38 |
| SoC     | Allwinner A133 (sun50iw10) |
| Device  | TrimUI Smart Pro |
| Header  | `hwdefs/rogue/km/cores/rgxcore_km_22.102.54.38.h` |

## What was extracted

The entire `drivers/gpu/drm/img/img-rogue/` subtree from the upstream
repository, containing:

- `generated/` -- auto-generated bridge code
- `hwdefs/` -- hardware definitions for all supported Rogue cores
- `include/` -- public and private DDK headers
- `services/` -- kernel services (server, common, system layers)
- `Kconfig`, `Makefile`, `config_kernel.h`, `config_kernel.mk`

The checked-in `config_kernel.h` and `config_kernel.mk` target the
StarFive FM7110 (RISC-V, BVNC 36.50.54.182). PocketForge replaces these
with A133-specific configuration (BVNC 22.102.54.38, ARM64).

## What was NOT extracted

Everything outside `drivers/gpu/drm/img/img-rogue/` in the upstream
repository: the Linux kernel tree, device trees, other drivers, build
infrastructure, etc.

## Upstream license

All Imagination Technologies source files carry `@License Dual MIT/GPLv2`
headers referencing `MIT-COPYING` and `GPL-COPYING`. A small number of
public API headers (`include/public/powervr/`) are MIT-only. Platform
system-layer files contributed by third parties (e.g. MediaTek `mt8173`)
carry their own GPL-2.0 headers.

See `LICENSE`, `MIT-COPYING`, and `GPL-COPYING` in this repository.

## PocketForge modifications

PocketForge's fork adds:

1. **`services/system/rgx_sunxi/`** -- A133/sun50iw10 system layer,
   ported from the DDK 1.11 sunxi system layer in
   `CrealityTech/sonic_pad_os` (GPL-2.0).
2. **Build configuration** -- replacement `config_kernel.h` and
   `config_kernel.mk` targeting BVNC 22.102.54.38, ARM64, nullws.
3. **Build-options strict-mask** -- `RGX_BUILD_OPTIONS_KM` constant
   override matching the vendor `pvrsrvkm.ko` bitmask (`0x0060d13d`).

All PocketForge-authored files are dual MIT/GPLv2, matching the upstream
convention.

## Upstream tracking

The `upstream` branch tracks the upstream repository for visibility.
Periodic fetches (monthly target) from
`DC-DeepComputing/fml13v01-linux:fm7110-6.6` are merged into `upstream`.
Cherry-picks to `main` are made only when absorbing a specific upstream
change.

## TG3040 reconstruction changes

The reconstruction changes the A133 build to the generic handle backend, enables
the stock physical-memory tests, and disables the separate RGX kick-sync bridge.
These choices follow the compilation-unit inventory retained in the original
TG3040 #860 module DWARF, rather than matching the reported DDK version alone.
`services/server/common/handle_generic.c` initially used the related public
Creality DDK 1.11 implementation. Comparing its error paths with stock identified
the matching generic backend in the same Semidrive DDK 1.18 tree cited below.
The replacement retains the matching handle API and dual MIT/GPLv2 license;
its callback table and error handling restore ten more comparable stock bodies.

The stock display-class ABI differs from the imported modern kernel port.
Its DWARF describes a 208-byte `_DC_DEVICE_` with a physical heap pointer and
list node, plus the 176-byte callback table with `pfnResetDevice`. The matching
DC implementation and generated bridge were recovered from the related public
`licy007/myir-semidrive-linux` commit
`c96cc8f345885b92d84ac52fc917b4329f8ee791`, path `drivers/gpu/rogue_km/`.
That source tree reports DDK 1.18@6276027; this is a component-level source
match, not a claim that the entire stock module is version 1.18. The imported
DC files are adapted to this tree's PhysHeapAcquireByID and void PMR-finalize
interfaces. Restoring the callback signatures also removes the extra heap-ID
argument from the Sunxi buffer-allocation callback. All 141 shared layouts
compared in the DC compilation unit match the stock module's DWARF.


Further TG3040 comparisons recover client-connect device initialization, the
stock module power lifecycle, two A133 clocks (PLL/core), a value-taking DVFS
frequency callback, a 48-bit DMA mask, and two dynamically allocated physical
heaps. The platform layer was restored from the related Creality source and
adapted to the DDK 1.19 callback API; the heap/configuration initialization was
reconstructed from stock ARM64 code. The unusual static gsDevices platform
back-pointer is preserved because the shipped relocations identify that object.

The private SCP_CONTEXT has a trailing synchronization-requester ID in stock.
The matching scp.c from the same pinned Semidrive tree restores the 48-byte
context and its allocation/error paths. This is another component import, not
a change to the reported DDK version. Original file hashes are retained in the
companion workbench's semidrive-gpu-source.json.

Stock uses vm_map_ram/vm_unmap_ram, non-NULL device DMA-mask accesses, a live
RGXSetComputeContextProperty bridge implementation, RGB565 enum 86, and bounded
one/two-buffer allocation in the Sunxi display driver. The port's TRACK_FW_BOOT
option is absent in stock. Whole-union zero-initializers in six power-command
routines are also absent; the recovered code writes the active command fields
and leaves unused bytes unspecified, as the shipped implementation does.

After these changes, 1958/2063 comparable pvrsrvkm function bodies and all 18
comparable dc_sunxi bodies match raw code/relocations. The companion workbench
separates diagnostic-line/address differences, differential execution checks,
and unresolved code differences. Matching instruction bodies does not establish
all referenced-data, firmware or physical hardware behavior.


A separate Linaro GCC 7.4.1-2019.02 candidate identifies remaining RISC-V cache
workarounds: two do_invalid_range calls, its A133 no-op shim, and private
riscv_cache/rv_cache mapping choices. Stock has none of these. Removing them
restores the four-argument _MemsetPageArray and its callers. The GCC 7.4.1
comparison matches 1974/2063 raw instruction bodies/relocations; all other 89
have classified changes confined to PVR diagnostic source-line arguments after
resolving named address targets. No source padding or #line directives were
added to manufacture diagnostic equality. The mirrored compiler is not asserted
to be the original OpenWrt-customized compiler. This code comparison still does
not validate every referenced object or physical GPU behavior.


Named-data comparison additionally identifies a false EnablePageFaultDebug
initial apphint in both stock apphint tables. The generic fallback macro remains
true in stock PVRSRVInit/PVRSRVDeviceCreate instruction bodies, so only the list
initializer is changed. Do not globally change the fallback macro to make those
separately compiled defaults agree. Both stock table values and code are retained.


The stock module also exports the external physical-heap interface, including
six PhysHeap functions, PVRSRVGetDeviceInstance and PVRSRVGetErrorString.
Enable SUPPORT_EXTERNAL_PHYSHEAP_INTERFACE to restore those eight public
exports; matching the corresponding function bodies alone does not restore
module linkage compatibility.
