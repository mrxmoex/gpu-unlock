# Fedora RPM build of gpu-unlock (upstream issue 92)

REPRODUCED: only-with-emulation

PATCH WORKS: yes

NOARCH-FIRMWARE: keep noarch. `gsp_ga10x.bin` is ELF64 RISC-V firmware, not a host program. rpm 6.0.2 rejects it in a noarch package only when the packaged mode is executable. Declare modes with `%defattr`. Do not drop `BuildArch: noarch`.

Nothing from this run was installed on the host, loaded into a kernel, or sent upstream. The applied change is `packaging/rpm/gpu-unlock.spec` (`fedora-port/run-machine/fix-spec.patch`). Raw logs are in `fedora-port/run-machine/logs/`. Built RPMs and NVIDIA firmware were not copied into the repository.

## What was reproduced

On a normal filesystem, unpatched `make rpm` (which is `rpmbuild -ba`) exits 0. The `error: Arch dependent binaries in noarch package` line appears only when regular-file `chmod` is forced to mode 0755. That emulation also prints 19 `executable but has no shebang` warnings, the same count the report described.

The emulation is an `LD_PRELOAD` library (`noperm_shim.c`, scratch only). It intercepts `chmod`, `fchmod`, `fchmodat`, and `fchmodat2` and, for a regular file, stores mode 0755. A direct check inside the sandbox:

```
normal:644
shim:755
install_shim:755
install_normal:644
```

After the failing build, the generated asset and the buildroot copy of the same file were:

```
644 84277400 build/rpm/BUILD/gpu-unlock-1.0.0-build/gpu-unlock-1.0.0/assets/gsp_ga10x.bin
755 84277400 build/rpm/BUILD/gpu-unlock-1.0.0-build/BUILDROOT/usr/lib/firmware/gpu-unlock/610.43.03/gsp_ga10x.bin
```

(`logs/D.gsp-modes.txt`). Python created the asset as 0644. `install -m644` is what the Makefile uses, and under the shim the buildroot copy is 0755.

This is weaker than a real exFAT or NTFS mount. The shim does not implement a filesystem: `open()` creation modes are unchanged, directories are unchanged, symlinks and long names still work, and `chmod` returns success. A real removable mount synthesizes mode from its driver (udisks `fmask` / `dmask` or NTFS permissions) and may return `EPERM` from `chmod`. No FUSE device and no sudo were available, so exFAT/NTFS were not mounted. The reporter's filesystem type was not observed. The shim matches the symptom that matters here: at classification time the RISC-V blob is a regular file with an execute bit.

Fedora's `/usr/lib/rpm/redhat/brp-mangle-shebangs` (read from the sandbox) only considers `find -executable` files whose MIME type is `text/` or `application/javascript`, then warns and runs `chmod -x`. The ELF blob is not text, so that script never clears its execute bit. Text sources do get the warning. With the shim, `chmod -x` is also stored as 0755, so the warnings and the leftover execute bit happen together.

## Versions

Sandbox userspace is Fedora 44. The host kernel is what `uname -r` printed inside the sandbox (`6.12.94+`). kernel-devel was not installed. This is not a boot of `7.2.11-200.fc44`.

| item | value |
|---|---|
| image | `registry.fedoraproject.org/fedora:44` tag `44`. Index `sha256:40005ed606ff7bbdc5344f3625e043f64493358b3dfaf2cc6b1995282189f3f5`. amd64 manifest `sha256:2d894246f63e15be3557d103c50e5fdef67bf50b34e35d54753966b9c1854608`. Config `sha256:d0b49678b365ee557c2be5206645869e2ed913fe50e400a683c8de6fe365bbd2`. Layer `sha256:b07077454b630a045041b5d6444937a0d429c478655ede9aa9ba1fc06cba785f` (72037545 bytes). |
| os | Fedora Linux 44 (Container Image) |
| uname -r | 6.12.94+ |
| rpm / rpm-build | 6.0.2-1.fc44 |
| rpmbuild | RPM version 6.0.2 |
| gcc | 16.2.1-2.fc44 |
| file | 5.46-10.fc44 |
| binutils | 2.46.1-1.fc44 |
| python3 | 3.14.8-1.fc44 |
| make | 4.4.1-12.fc44 |
| git | 2.56.0-1.fc44 |
| zstd | 1.5.7-5.fc44 |
| rpmlint | 2.8.0-3.fc44 |
| repo HEAD | `af28cbbfd3d922a659227ecfb7369adfd924cf03` |
| `%{_binaries_in_noarch_packages_terminate_build}` | `1` |
| `%{?_binaries_in_noarch_data_ok}` | empty (macro not set) |

bubblewrap 0.12.0. No docker daemon. The sandbox is `scripts/bwrap-run.sh` from the earlier read-only tree, copied into scratch and not modified: user namespace, uid 0 mapped to the invoking user, rootfs bind, `/proc`, `/dev`, tmpfs `/tmp` and `/run`, read-only host `/etc/resolv.conf`.

## Build matrix

Each row is `make rpm` in `/work/gpu-unlock` inside the sandbox. `make rpm` runs `firmware/fetch_artifacts.py` and the spec's `sha256sum -c`. Both checksum lines in `logs/A.build.log` are `OK`. The unit tests in `%check` printed `OK` (5 tests).

| tag | spec | chmod shim | exit | shebang warnings |
|---|---|---|---|---|
| A | pristine `BuildArch: noarch` | no | 0 | 0 |
| D | pristine | yes | 2 | 19 |
| B | `%defattr(0644,root,root,0755)` on both `%files` | no | 0 | 0 |
| C | same `%defattr` | yes | 0 | 19 |
| E | `%global _binaries_in_noarch_packages_terminate_build 0` | yes | 0 | 19 |
| F | delete `BuildArch: noarch` only | no | 2 | 0 |
| G | per-file `%attr` list | yes | 0 | 19 |
| F2 | delete `BuildArch: noarch` and add `%global debug_package %{nil}` | no | 0 | not counted |

D's failure, from `logs/D.build.log`:

```
error: Arch dependent binaries in noarch package
RPM build errors:
    Arch dependent binaries in noarch package
make: *** [Makefile:138: rpm] Error 1
```

E prints `warning: Arch dependent binaries in noarch package` and still writes the noarch RPMs. F prints:

```
Processing files: gpu-unlock-firmware-1.0.0-1.fc44.x86_64
Processing files: gpu-unlock-debugsource-1.0.0-1.fc44.x86_64
error: Empty %files file /work/gpu-unlock/build/rpm/BUILD/gpu-unlock-1.0.0-build/gpu-unlock-1.0.0/debugsourcefiles.list
```

F2 writes only:

```
build/rpm/RPMS/x86_64/gpu-unlock-dkms-1.0.0-1.fc44.x86_64.rpm
build/rpm/RPMS/x86_64/gpu-unlock-firmware-1.0.0-1.fc44.x86_64.rpm
```

`ls build/rpm/RPMS/noarch/gpu-unlock-*-1.0.0-*.noarch.rpm` then exits 2 (`logs/F2.glob.txt`). That is the glob in `Makefile` `install-rpm` (line 144). F2 was built only to see the directory. It is not the committed spec.

## Payload identity of the committed fix

On the normal filesystem, binary RPMs from A and from B (`%defattr`) have identical `rpm -qlpv`, `rpm -qlp`, and mode/owner/size/color queries (`logs/payload-compare.txt`: `qlpv A vs B binary_equal True`, `modes A vs B binary_equal True`, `qlp A vs B binary_equal True`). The same is true for C (the fix under the shim) versus A. The nosrc RPM's `gpu-unlock.spec` member changes size from 4689 to 4749 bytes because the spec text changed. Paths in the nosrc RPM stay the same (`qlp` srpm_equal True).

Packaged `gsp_ga10x.bin` on A, B, and C:

```
-rw-r--r-- root root 84277400 /usr/lib/firmware/gpu-unlock/610.43.03/gsp_ga10x.bin mode=100644 color=0
```

Under E (macro mute, shim on), every regular payload file is mode `100755`, and the blob is `color=2` (`logs/E.modes.txt`).

Under G (per-file `%attr`, shim on) the blob is `mode=100644 color=0`, but the file list is not identical to A: G adds `/usr/lib/dracut` and `/usr/lib/dracut/dracut.conf.d`, and the dkms license directory is `mode=40644` instead of `40755`. That `%attr` list is not the committed fix.

## What the 84 MB file is

Extracted from `gpu-unlock-firmware-1.0.0-1.fc44.x86_64.rpm` (the F2 package; same `%build` firmware generation). `file` and `readelf -h` are in `logs/file-readelf-f2.txt`.

```
gsp_ga10x.bin: ELF 64-bit LSB relocatable, UCB RISC-V, soft-float ABI, version 1 (SYSV), not stripped
Class: ELF64
Type: REL (Relocatable file)
Machine: RISC-V
```

Size 84277400. The other four firmware files are `data` or `SIMH tape data`, not ELF. `readelf` shows 0 program headers and entry point 0.

The module requests the names in `kernel/gpu_unlock/unlock_610.c` (`logs/grep-firmware.txt`):

```
#define GSP_FILE "gpu-unlock/610.43.03/gsp_ga10x.bin"
ret = request_firmware(fw, name, dev);
MODULE_FIRMWARE(GSP_FILE);
```

`unlock_transaction_assets` calls `gpu_unlock_elf_section` on that blob for `.fwimage` and `.fwsignature_ga10x`. The file header comment in `unlock_610.c` describes a halted GSP and Falcon SEC2. `gfw_eval.h` says GFW completion uses Falcon CPUCTL, not RISC-V CPUCTL. That is source text. This run did not execute the module or touch a GPU, so it does not show what a device does with the bytes.

rpm colors an executable regular file via `getElfColor` (upstream tag `rpm-6.0.2-release`, `build/rpmfc.cc` around the `S_ISREG(mode) && is_executable` call). The only machine forced to color 0 there is `EM_BPF`. RISC-V is not exempt. `build/files.cc` logs `Arch dependent binaries in noarch package` when the noarch header color is non-zero, and the log level is an error when `%_binaries_in_noarch_packages_terminate_build` is non-zero. Installed rpm 6.0.2-1.fc44 evaluated that macro as `1`. Those C++ files were fetched from the upstream tag, not from the Fedora SRPM, so Fedora may carry extra patches. The observed error string, the warning downgrade, and `color=2` match that logic.

Fedora `linux-firmware.spec` from `https://src.fedoraproject.org/rpms/linux-firmware/raw/rawhide/f/linux-firmware.spec` (curl exit 0, version `20261019`):

```
%global debug_package %{nil}
%define _binaries_in_noarch_packages_terminate_build 0
BuildArch:	noarch
```

It stays noarch and turns the check into a warning. `https://docs.fedoraproject.org/en-US/packaging-guidelines/Firmware/` returned HTTP 404. The main packaging-guidelines page was downloaded and does not contain a firmware section. `rpmlint` 2.8.0 on both A and B still reports:

```
gpu-unlock-firmware.noarch: E: arch-independent-package-contains-binary-or-object /usr/lib/firmware/gpu-unlock/610.43.03/gsp_ga10x.bin
```

The two runs match (3 errors, 18 warnings without the rc; with `packaging/rpm/gpu-unlock.rpmlintrc`, two filters are reported unused, including the filter aimed at `unstripped-binary-or-object`, which this rpmlint did not emit). The rc file was not edited.

## Recommendation

Keep the package `BuildArch: noarch` and add `%defattr(0644,root,root,0755)` to both `%files` sections. The blob is RISC-V firmware requested through `request_firmware`, and on a normal filesystem the binary RPMs' paths, modes, owners, and sizes do not change. Muting `_binaries_in_noarch_packages_terminate_build` also makes the build exit 0, which is what `linux-firmware` does, but under the mode emulation it ships the blob as mode 0755 with ELF color 2. Dropping noarch does not finish on this Fedora unless debugsource generation is also disabled, and the packages then land in `RPMS/x86_64/`, which `make install-rpm` does not glob. A per-file `%attr` list can also exit 0 and force 0644 on the blob, and the list tried here changed directory ownership and a directory mode. `%defattr` is the small declaration that matches the install modes the spec already relies on.

## Unknown

- The reporter's real mount of `/run/media/.../Softwares`, and whether a real exFAT or NTFS mount reproduces this. Not mounted here.
- Behavior on rpm other than 6.0.2-1.fc44, and whether the Fedora SRPM patches `rpmfc.cc`.
- Installing the RPMs, the `%post` scriptlets, DKMS, dracut, and any GPU. Not run. kernel-devel 7.2.11 was not installed. The module was not compiled.
- Whether a corrected per-file `%attr` list can match A's file list exactly. The list that was built does not.
