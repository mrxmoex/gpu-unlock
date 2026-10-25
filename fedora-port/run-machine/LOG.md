# Log

Scratch was `/workspace/gpu-unlock-rerun-scratch/m1/`. The earlier tree `/workspace/gpu-unlock-rpm/` was read and not modified. Times are UTC on 2026-10-25. Durations are the tool's elapsed times when it printed one. Exit codes are the process status unless a line inside the output says otherwise.

## Branch

`git checkout -b cursor/gpu-unlock-machine` from `fedora-port-20261025` at `af28cbbfd3d922a659227ecfb7369adfd924cf03`. Exit 0. No commit was made on `fedora-port-20261025`.

## Sandbox bring-up

1. `python3 .../pull_rootfs.py fedora:44 .../rootfs` exited 1. `urllib.error.HTTPError: HTTP Error 400: Bad Request`. The script's URL is `/v2/fedora/manifests/<tag>`, so the tag is `44`, not `fedora:44`.
2. `python3 .../pull_rootfs.py 44 .../rootfs` exited 0 in 12582 ms. Digests are in REPORT.md. Layer 72037545 bytes.
3. First `dnf install` inside bwrap used the image's metalink. It sat on slow mirrors (`mirror.23m.com` curl error 28, timeout 120s) for more than 13 minutes and had not started an RPM transaction. I killed it. `rpmdb.sqlite` mtime stayed `2026-10-25 07:48:01`.
4. I pointed the sandbox's `fedora.repo` and `fedora-updates.repo` `baseurl` at `https://mirrors.kernel.org/fedora/...` and disabled `countme`. Retried dnf. Metadata came from cache and the transaction started downloading, then blocked on `ciscobinary.openh264.org` (curl 28). The tool then killed that shell at its own timeout. rpmdb mtime was still `07:48:01`. 221 RPM files were in the libdnf5 cache. No transaction had committed.
5. Set `enabled=0` on the cisco openh264 repo and reran dnf with `--disable-repo=fedora-cisco-openh264`. Exit 0. Wall time about 11 s because the packages were already cached. Tail is `logs/dnf-install.tail.txt` (`Complete!` / `DNF_EXIT:0`, 223 packages). Toolchain versions are in REPORT.md.

`kernel-devel` was not installed. The RPM build does not compile `gpu_unlock`.

## Shim and clone

`git clone --branch cursor/gpu-unlock-machine file://<repo> rootfs/work/gpu-unlock`. `git rev-parse HEAD` printed `af28cbbfd3d922a659227ecfb7369adfd924cf03`.

Compiled `/work/emulate/noperm_shim.so` with the sandbox gcc. `GCC_EXIT:0`. Mode probe printed `normal:644`, `shim:755`, `install_shim:755`, `install_normal:644`. `rpm --eval '%{_binaries_in_noarch_packages_terminate_build}'` printed `1`. `rpm --eval '%{?_binaries_in_noarch_data_ok}'` printed an empty value. A byte search found `removing executable bit` in `/usr/lib/rpm/redhat/brp-mangle-shebangs`.

## Matrix

`bash /work/run-all.sh` inside bwrap. Console `START 2026-10-25T12:47:51Z`, `END 2026-10-25T12:50:43Z`. The wrapper exited 0 (`RUN_ALL_EXIT:0`). Per-build exits are the `*.exit` files: A 0, D 2, B 0, C 0, E 0, F 2, G 0. Checksums in A: both `sha256sum -c` lines are `OK`, and `%check` printed `OK`.

I then compared payload listings with a host python script. The first redirect used the wrong filename (`A.qlpv` instead of `A.qlpv.txt`) and exited 1 with `FileNotFoundError`, leaving an empty `payload-compare.txt`. I reran it. Exit 0. Result is `logs/payload-compare.txt`.

## Follow-ups that failed and were retried

A combined sandbox script extracted the firmware RPM with `rpm2cpio` (`CPIO_EXIT:0`) and printed `file` / `readelf -h`, then tried to build F2. `cp` of the spec failed: `cannot create regular file 'packaging/rpm/gpu-unlock.spec': No such file or directory`, because the shell was still in `/tmp/payload`. The script had no `set -e`, so it still ran `rm -rf build/rpm` and printed `F2_EXIT:2`. That F2 result is void. The `file`/`readelf` text from that attempt matched the later saved run. Overall script exit was 0 because the version commands at the end succeeded.

Retried F2 from `/work/gpu-unlock`. `F2_EXIT:0` in 23691 ms. Packages are under `RPMS/x86_64/`. The noarch glob exited 2.

Re-ran `file` and `readelf -h` on the F2 firmware RPM and saved `logs/file-readelf-f2.txt`. `TEE_EXIT:0`. `uname -r` printed `6.12.94+`.

`grep -n` of the firmware macros in `kernel/gpu_unlock/unlock_610.c` exited 0 (`logs/grep-firmware.txt`).

## Upstream text fetches

- `curl` of rawhide `linux-firmware.spec`: exit 0.
- `curl` of `macros.in` and `rpmfc.cc` and `files.cc` from `rpm-software-management/rpm` tag `rpm-6.0.2-release`: `macros.in` and `rpmfc.cc` and `files.cc` exit 0. The first `files.c` URL (not `.cc`) was HTTP 404.
- `curl` of `https://docs.fedoraproject.org/en-US/packaging-guidelines/`: exit 0. No firmware section in that HTML.
- `curl` of `https://docs.fedoraproject.org/en-US/packaging-guidelines/Firmware/`: exit 22, HTTP 404.

## Not done

Module compile, `dnf install` of the built packages, `insmod` / `modprobe` / `depmod`, dracut, reboot, sudo, docker, a real exFAT/NTFS mount, any write to Zagitta/gpu-unlock. After these notes were written, `/workspace/gpu-unlock-rerun-scratch/m1/rootfs` was deleted. The text logs above were already copied into this directory.
