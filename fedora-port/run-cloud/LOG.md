# Run log

Sandbox root: `/tmp/gpu-unlock-f44-sandbox` (not in the git tree). Branch worked on: `cursor/kit-gpu-unlock-cloud-34fb` at `af28cbb` before this change. No pull request. No push to `fedora-port-20261025` or `main`. No force-push.

Durations below are the wall times reported for each shell invocation, not a separate timer inside the sandbox.

## What happened

1. Host is Ubuntu 24.04. `docker`, `podman`, `rpm`, `dnf`, `bwrap`, and `mkfs.vfat` are absent. `unshare --user --map-root-user` works. `/proc/filesystems` has no vfat/exfat/ntfs. A vfat mount was not attempted beyond the failed `mkfs.vfat` / `mount -t vfat` probe. No `modprobe`.

2. Pulled `registry.fedoraproject.org/fedora:44` amd64 with Python `urllib` (no docker). First extract failed on absolute symlinks (`AbsoluteLinkError`). Second extract failed on a mode-`0555` directory (`PermissionError`). Third extract, which temporarily adds owner write, succeeded: 9034 members, 0 skipped. Rootfs says Fedora 44 Container Image.

3. First `dnf -y install` of the rpm build set returned `DNF_EXIT:1` (`Transaction failed`) because scriptlets could not open the bind-mounted host `/dev/null` (`O_TRUNC` is `Permission denied` in this user namespace). The packages were nevertheless present afterward. A second `dnf -y install` returned `DNF_EXIT:0` (`Complete!`) with `/dev/null` as a regular file. `rpmbuild`, `gcc` 16.2.1, `rpmlint` 2.8.0, `git`, and `binutils` are installed inside the rootfs only.

4. `git clone --local` of this repo into the rootfs. First `make rpm` failed in `%build`: `tar` tried to `chown` the NVIDIA source to uid 1000, which is not mapped (`MAKE_EXIT:2`). `%prep` had already printed both `sha256sum -c` lines as `OK`. Retry with `TAR_OPTIONS=--no-same-owner` on ext4: `MAKE_EXIT:0`, no shebang warnings. Saved those RPMs under `/tmp/gpu-unlock-f44-sandbox/baseline-rpms/` (not in git).

5. `file` / `readelf -h` / `readelf -S` on `gsp_ga10x.bin` from `rpm2cpio`. First `readelf -S -W` missed the extract because `/tmp` in that namespace was a tmpfs that had gone away. Repeated later into `logs/gsp-identify.txt` and deleted the extracted tree.

6. Mode emulation, first library: interposed `stat` and made `chmod` a no-op. `stat` segfaulted (libc `stat` and `fstatat` re-entered the library). Rebuilt on the `newfstatat` syscall. `stat(1)` then showed `755` while disk stayed `644`. `bash` builtin `test -x` did not see the preload; `bash -c 'test -x'` did. rpm is linked to `stat64`/`fstatat64`, so those aliases were added. A `make rpm` with that library printed 19 shebang warnings and then `%check` failed `PermissionError` on the test binaries (execve uses the real `0644`). That log was overwritten by the next run.

7. Final emulation (`modeignore.c` in this directory): `chmod`/`fchmod`/`fchmodat` perform a real syscall with mode `0755`. `install -Dm644` leaves disk mode `755`. Unmodified `make rpm` under `LD_PRELOAD`: 19 shebang warnings, `error: Arch dependent binaries in noarch package`, `MAKE_EXIT:2`.

8. Variants under that preload: `%defattr` exit 0 and package mode `0644`; macro mute exit 0 with a warning and package mode `0755`; drop `BuildArch` exit 2 in `find-debuginfo` (`No build ID note found`); per-file `%attr` exit 0 and mode `0644`. Dropping noarch again on ext4 without the preload failed with `Empty %files file .../debugsourcefiles.list` for `gpu-unlock-debugsource-...x86_64`. The same spec plus `%global debug_package %{nil}` wrote `RPMS/x86_64/*.x86_64.rpm`. The `RPMS/noarch/*.noarch.rpm` glob matched nothing.

9. Committed spec change rebuilt on ext4 without `LD_PRELOAD`: `FIX_EXIT:0`, 0 shebang warnings. `rpm -qlpv` before vs after: 33 lines, identical. `rpmlint` exit 64 both before and after, same `arch-independent-package-contains-binary-or-object` error. The `rpmlintrc` filters are unused. `rpmlint` was not installed on the host; it was installed inside the sandbox.

## Commands and exit codes

Host checks:

- `git status && git rev-parse --abbrev-ref HEAD && git log -1 --oneline && git branch -vv && ls -la` — 0
- `type docker podman rpmbuild rpm dnf yum bwrap chroot fakeroot fakechroot debootstrap` plus `ls` of those paths, `id` — 0 (commands not found; `chroot` is `/usr/sbin/chroot`)
- `df -h`, `unshare --user --map-root-user --mount echo unshare-ok`, `/proc/filesystems` grep, `python3 --version`, `command -v curl file readelf gcc make tar xz zstd python3` — 0
- `cat /proc/filesystems`; user-ns `mount -t tmpfs` OK; `mkfs.vfat` missing (`MKFS_EXIT:127`); `mount -t vfat` failed (`VFAT_FAIL:32`) — shell 0
- `curl -fsSI` registry `/v2/`, dl.fedoraproject.org Container/44 listing, src.fedoraproject.org linux-firmware.spec — 0
- `curl` OCI index for `fedora:44` — 0. Digest recorded in REPORT.md
- Python download of the amd64 manifest — 0. `LAYER` download — 0. Extract with `tarfile` data filter — 1 (`AbsoluteLinkError`). Custom extract — 1 (`PermissionError`). Custom extract with parent mode fix — 0
- user-ns `mount -t sysfs` and bind `/sys` — shell 32
- user-ns mount probe and `chroot rpm --version` — 0 (`RPM version 6.0.2`)
- user-ns `dnf -y install rpm-build make gcc python3 zstd rpmlint file binutils git xz gzip findutils which diffutils patch redhat-rpm-config` — wrapper 0, `DNF_EXIT:1`
- same `dnf` with a regular `/dev/null` — wrapper 0, `DNF_EXIT:0`
- `git clone --local --branch cursor/kit-gpu-unlock-cloud-34fb` into the rootfs — 0
- user-ns `make rpm` without `TAR_OPTIONS` — wrapper 0, `MAKE_EXIT:2`
- `stat -f` / `findmnt` of the work tree; user-ns `make rpm` with `TAR_OPTIONS=--no-same-owner` — wrapper 0, `MAKE_EXIT:0`
- user-ns `rpm -qlpv`, `rpmlint`, `rpmlint -r`, `rpm2cpio | cpio`, `file`, `readelf -h`, `readelf -S` — wrapper 0. `rpmlint` exits inside that log are 64
- Python scan of `librpmbuild.so` for the error string — 0
- `curl` linux-firmware.spec — 0. `curl` `build/files.c` at two tag names — 22 (404). GitHub contents API for `build?ref=rpm-6.0.2-release` — 0. `curl` `build/files.cc` — 0. `curl` `build/rpmfc.cc` — 0
- `nm -D` libc and GitHub tags API — 0
- copy baseline RPMs; `readelf -S -W` — wrapper 1 (extract missing, `/dev/null` trunc denied)
- compile first `modeignore.c` inside the sandbox — `COMPILE:0`, wrapper 0
- preload probe — wrapper 139 (segfault)
- syscall rewrite probe — `LIE_STAT:755`, `DISK_AFTER:644`, wrapper 0
- `bash -c test -x`, `find -executable`, Python `os.stat` — wrapper 0 (`PY_MODE 0o644` on that library)
- `nm -D` librpm and C `stat`/`fstatat`/`statx` — wrapper 0 (`stat 755`)
- compile `stat64` C aliases — `COMPILE:1` (type conflict), wrapper 127
- `sizeof(struct stat)` vs `struct stat64` — wrapper 0 (both 144, `st_mode` offset 24)
- asm `.set stat64, stat` and `callstat64` — wrapper 0 (`stat64 755`, disk `644`)
- `make rpm` with the stat-lie library — wrapper 0, `MAKE_EXIT:2` (`%check` `PermissionError`). Log not kept
- rewrite to chmod-forces-0755; `install -Dm644` and `gcc` probe — `INSTALL_MODE:755`, `GCC_MODE:755`, `RUN:0`, wrapper 0
- `make rpm` unmodified spec with that preload — wrapper 0, `MAKE_EXIT:2` (`logs/rpmbuild-emulation-unmodified.log`)
- four spec variants under the preload — wrapper 0. Exits: a 0, b 0, c 2, d 0
- ext4 drop-noarch, drop-noarch plus `debug_package %{nil}`, and the committed spec — wrapper 64 because `rpmlint` returned 64 under `set -e` after `FIX_EXIT:0`. `C_NORMAL_EXIT:2`, `C_NODEBUG_EXIT:0`, `FIX_EXIT:0`
- qlpv compare, `rpmlint -r` on the fixed RPMs, `readelf` — wrapper 0. Diff file: `identical=True`. `RPMLINT_RC_EXIT:64`. `READELF_H:0`, `READELF_S:0`
- copy logs and `git diff` of the spec — 0

Not run: `dnf install` of the built RPMs, `insmod`/`modprobe`/`depmod`, dracut, any command on a real exFAT/NTFS mount, `sudo`, printing `env` or `/proc/*/environ`.
