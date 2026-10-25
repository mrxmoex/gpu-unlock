# Compile matrix: gpu_unlock vs Fedora kernel-devel (build only)

Run 2026-10-25 in bubblewrap user-namespace sandboxes over digest-verified official `fedora:<tag>` rootfs images (no install/load on any real machine, no sudo, build only).
Command per tag: `make -C /usr/src/kernels/<ver> M=<repo>/kernel/gpu_unlock modules` (module sources from this repo at af28cbb).

| Fedora | kernel-devel | gcc | gpu_unlock builds? | NVIDIA 610.43.03 open modules build? |
|---|---|---|---|---|
| 42 | 6.19.14-108.fc42 | 15.2.1 | **yes** (exit 0) | **yes** (exit 0, all 5 .ko built) |
| 43 | 7.2.11-100.fc43 | - | **yes** (exit 0) | **no** (exit 2) |
| 44 | 7.2.11-200.fc44 | - | **yes** (exit 0) | **no** (exit 2) |
| rawhide (F46) | 7.4.0-0.rc0.261023g734bef548a96.6.fc46 | - | **yes** (exit 0) | **no** (exit 2) |

Observed: gpu_unlock itself (the module in this repo) compiles cleanly on all four kernels. Only the separate NVIDIA 610.43.03 source (the firmware-generation input, tarball sha256 `7e118923...c1dc`, verified before use) fails on 7.x kernels. See NVIDIA-610-COMPILE.md.
Not tested: loading, running on hardware, Secure Boot signing, actual `rpmbuild` of dkms-built modules.
Raw per-tag logs are kept in the staging folder, not pushed (1.7 MB).
