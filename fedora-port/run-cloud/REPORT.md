# Fedora rpmbuild of gpu-unlock (issue #92)

REPRODUCED: only-with-emulation

PATCH WORKS: yes

NOARCH-FIRMWARE: KEEP-NOARCH

The `Arch dependent binaries in noarch package` error did not appear on ext4. It appeared when every libc `chmod` was forced to mode `0755`. With `%defattr(0644,root,root,0755)` on both `%files` sections, `make rpm` exits 0 under that emulation and on ext4. `rpm -qlpv` of both binary RPMs on ext4 is identical before and after the spec change (`logs/qlpv-diff.txt`: `before_lines=33 after_lines=33 identical=True`).

## Versions

Sandbox image: `registry.fedoraproject.org/fedora:44`, pulled without docker/podman.

- index `sha256:40005ed606ff7bbdc5344f3625e043f64493358b3dfaf2cc6b1995282189f3f5`
- amd64 manifest `sha256:2d894246f63e15be3557d103c50e5fdef67bf50b34e35d54753966b9c1854608`
- layer `sha256:b07077454b630a045041b5d6444937a0d429c478655ede9aa9ba1fc06cba785f`
- annotation `org.opencontainers.image.created` `2026-10-25T06:48:02.717176821Z`

Inside the rootfs (`logs/rpmbuild-normal-fs.log`):

- `RPM version 6.0.2`
- `gcc (GCC) 16.2.1 20260819 (Red Hat 16.2.1-2)`
- `Python 3.14.8`
- `uname -r` printed `6.12.94+` (this VM's kernel, not `7.2.11-200.fc44.x86_64`)
- `rpm --eval '%{_binaries_in_noarch_packages_terminate_build}'` printed `1`
- `/etc/os-release` in the rootfs: `Fedora Linux 44 (Container Image)`, `VERSION_ID=44`

Host mount of the build tree: `/dev/vdc / ext4 rw,relatime`. `stat -f` prints `TYPE=ext2/ext3` for that same ext4 filesystem. `findmnt -T` printed `FSTYPE ext4`.

`/proc/filesystems` has no `vfat`, `exfat`, or `ntfs`. `mkfs.vfat` is absent (`MKFS_EXIT:127`). `mount -t vfat` failed (`VFAT_FAIL:32`). No `modprobe`. This is not a real removable-media mount.

Sandbox limits that are not part of the rpm bug:

- User namespace maps only uid 0. `tar -xf` of the NVIDIA source as that root failed with `Cannot change ownership to uid 1000` (`logs/rpmbuild-normal-fs-tar-chown-excerpt.txt`,  ownership error count in that file). Later builds set `TAR_OPTIONS=--no-same-owner`. SHA-256 checks still ran and passed.
- `/dev/null` from the host cannot be opened with `O_TRUNC` in this user namespace (`Permission denied`). The sandbox uses a regular mode-`0666` file as `/dev/null`.

## What the 84 MB file is

From the fixed noarch firmware RPM, extracted with `rpm2cpio | cpio` and then deleted (`logs/gsp-identify.txt`):

`file` on `gsp_ga10x.bin`:

`ELF 64-bit LSB relocatable, UCB RISC-V, soft-float ABI, version 1 (SYSV), BuildID[sha1]=3609758e15d41b2720db2ffb49df623c4ac21c62, not stripped`

`readelf -h`: `Type: REL (Relocatable file)`, `Machine: RISC-V`, entry point 0, 0 program headers. Size in `rpm -qlpv` is `84277400` bytes, mode `0644`, owner `root:root`.

`readelf -S -W` section `.fwimage` is `PROGBITS` flags `WA` (not `X`). Section `.fwsignature_ga10x` is present. The other firmware files are `data` or `SIMH tape data`, not ELF.

`kernel/gpu_unlock/unlock_610.c` requests `gpu-unlock/610.43.03/gsp_ga10x.bin` via `request_firmware` (`GSP_FILE`, `MODULE_FIRMWARE`, `gpu_unlock_610_load_shared`). It then calls `gpu_unlock_elf_section` for `.fwimage` and `.fwsignature_ga10x` and copies those spans into DMA buffers. That is what the source does. This run did not load the module and did not touch a GPU.

rpm 6.0.2 colors a regular file as an ELF only when its payload mode has an execute bit, then rejects a noarch package whose header color is non-zero. RISC-V is not the BPF exception. Excerpt: `logs/rpm-6.0.2-rpmfc-excerpt.txt` (from tag `rpm-6.0.2-release`, `build/rpmfc.cc`). Macro default: `logs/rpm-macro-binaries-in-noarch.txt`.

`brp-mangle-shebangs` warns only for executable text files and then runs `chmod -x`. It does not strip the execute bit from an ELF. Excerpt: `logs/brp-mangle-shebangs-excerpt.txt`.

## Reproduction

Unmodified spec, ext4, `TAR_OPTIONS=--no-same-owner`, no mode emulation:

- `make rpm` ended `MAKE_EXIT:0`
- wrote `RPMS/noarch/gpu-unlock-dkms-1.0.0-1.fc44.noarch.rpm` and `gpu-unlock-firmware-1.0.0-1.fc44.noarch.rpm`
- `grep -c 'executable but has no shebang'` on that log is 0
- both packages' files are mode `0644`, directories `0755`, `root:root` (`logs/qlpv-rpmlint-before.txt`)
- spec `%prep` `sha256sum -c` printed `OK` for both NVIDIA inputs

Emulation (`fedora-port/run-cloud/modeignore.c`): `LD_PRELOAD` replaces `chmod`, `fchmod`, and `fchmodat` with a real `fchmod`/`fchmodat` syscall of mode `0755`. `install -Dm644` then leaves disk mode `755`. `stat` is not hooked, so rpm and the kernel see the same mode. Compared with exFAT/NTFS this is weak: only libc callers in processes that keep the library loaded are affected; `open(O_CREAT)` still honors its mode; symlinks and long names still work because the backing store is ext4; chmod returns success instead of `EPERM`.

Unmodified spec under that preload (`logs/rpmbuild-emulation-unmodified.log`):

- 19 lines `executable but has no shebang, removing executable bit`
- `error: Arch dependent binaries in noarch package`
- `MAKE_EXIT:2`
- no `Wrote:` RPM lines

An earlier preload that only lied in `stat`/`access` and ignored chmod also printed those 19 warnings, then `%check` died with `PermissionError` executing the test binaries (the kernel still saw mode `0644`). That log was overwritten by the chmod-to-0755 run. The saved emulation log is the chmod-to-0755 run.

## Alternatives

All four were attempted under the chmod-to-0755 preload unless noted. Summary: `logs/variants-emulation.log`.

| Variant | Result |
| --- | --- |
| (a) `%defattr(0644,root,root,0755)` on both `%files` | `MAKE_EXIT:0`. Still 19 shebang warnings. `gsp_ga10x.bin` in the package is `-rw-r--r--` size `84277400`. |
| (b) `%define _binaries_in_noarch_packages_terminate_build 0` | `MAKE_EXIT:0`. Log contains `warning: Arch dependent binaries in noarch package` (not `error:`). `gsp_ga10x.bin` is `-rwxr-xr-x` size `84277400`. |
| (c) delete `BuildArch: noarch` | Under the preload, `%install` runs `find-debuginfo` and fails: `No build ID note found in .../gsp_ga10x.bin`. `MAKE_EXIT:2`. No RPMs. On ext4 without the preload, the failure is later: `error: Empty %files file .../debugsourcefiles.list` while processing `gpu-unlock-debugsource-1.0.0-1.fc44.x86_64`. `C_NORMAL_EXIT:2`. |
| (c) plus `%global debug_package %{nil}`, ext4, no preload | `C_NODEBUG_EXIT:0`. Wrote `RPMS/x86_64/gpu-unlock-dkms-1.0.0-1.fc44.x86_64.rpm` and `gpu-unlock-firmware-1.0.0-1.fc44.x86_64.rpm`. The Makefile glob `RPMS/noarch/gpu-unlock-*-1.0.0-*.noarch.rpm` printed `(no match)`. `dnf install` from `install-rpm` was not run. |
| (d) `%attr(0644,root,root)` on each file | `MAKE_EXIT:0`. `gsp_ga10x.bin` is `-rw-r--r--` size `84277400`. |

Fedora `linux-firmware.spec` from `https://src.fedoraproject.org/rpms/linux-firmware/raw/f44/f/linux-firmware.spec` (51640 bytes) has `BuildArch: noarch` and `%define _binaries_in_noarch_packages_terminate_build 0` (`logs/linux-firmware-spec-head.txt`). That is the distribution's way to keep ELF-looking firmware in a noarch package. It does not pin the payload mode.

## Fix that was committed

`%defattr(0644,root,root,0755)` on `%files dkms` and `%files firmware`. On ext4, `make rpm` ended `FIX_EXIT:0`, shebang warning count `0`, and `rpm -qlpv` matched the pre-change packages.

`rpmlint` 2.8.0 before and after, without the rc file, both end with `3 errors, 18 warnings` and `gpu-unlock-firmware.noarch: E: arch-independent-package-contains-binary-or-object .../gsp_ga10x.bin` (exit 64). With `packaging/rpm/gpu-unlock.rpmlintrc`, both runs also report the two filters as unused and end with `4 errors, 18 warnings` (exit 64). The rc file looks for `unstripped-binary-or-object`, `statically-linked-binary`, `missing-PT_GNU_STACK-section`, and `no-binary`. The message rpmlint actually emits is `arch-independent-package-contains-binary-or-object`. The rc file was not edited.

## Recommendation

Keep `BuildArch: noarch`. `gsp_ga10x.bin` is a RISC-V relocatable firmware image with no program headers; the module source loads it through `request_firmware` and copies `.fwimage` and `.fwsignature_ga10x` into DMA buffers. It is not an x86-64 host program, and labeling the package `x86_64` makes `find-debuginfo` fail and misses `install-rpm`'s `RPMS/noarch/*.noarch.rpm` glob. `%defattr(0644,...)` is the change that was committed: under a tree that cannot clear the execute bit it still packages mode `0644`, and a real host executable would still fail the noarch check. Muting `%_binaries_in_noarch_packages_terminate_build` also builds, which is what `linux-firmware` does, but the emulation then shipped `gsp_ga10x.bin` as mode `0755`. Per-file `%attr` also forces `0644` and is easier to forget when a file is added.

## Unknown

- Not run on a real exFAT or NTFS mount, and not run on kernel `7.2.11-200.fc44.x86_64`.
- If that mount returns `EPERM` from `chmod`, `brp-mangle-shebangs` (`set -eu`) would stop at the first text file instead of printing 19 warnings. The emulation returns success and forces `0755`.
- Packages were not installed. The module was not loaded. No GPU was used.
- `docs.fedoraproject.org` was not fetched. The noarch precedent used here is `linux-firmware.spec` on `src.fedoraproject.org` plus the rpm 6.0.2 sources and the sandbox macros.
- `gpu-unlock.rpmlintrc` does not match the rpmlint error this blob produces. It was left unchanged.
