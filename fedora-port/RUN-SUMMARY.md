# Run summary (2026-10-25): issue #92 rerun, machine vs cloud agent

Both agents reproduced the same finding independently; the fix is the two-line `%defattr(0644,root,root,0755)` in each `%files` section (commit "pin noarch RPM file modes with %defattr", in this branch).
- REPRODUCED: only with emulation (unmodified `make rpm` exits 0 on ext4; exits 2 with "Arch dependent binaries in noarch package" when chmod is forced to 0755, which is what a mode-less filesystem like exFAT/NTFS would give). No real exFAT/NTFS mount was tested.
- PATCH WORKS: yes (exit 0 under emulation and on ext4; `rpm -qlpv` of the built RPMs identical to the unpatched ext4 build).
- gsp_ga10x.bin (84,277,400 bytes) is an ELF64 RISC-V relocatable (GSP firmware), not host code: keep it in the noarch package.
- Full agent reports and logs live on their branches: `cursor/gpu-unlock-machine` (self-hosted worker) and `cursor/kit-gpu-unlock-cloud-34fb` (cloud agent), under `fedora-port/run-machine/` and `fedora-port/run-cloud/`.
- Open item both noticed: `packaging/rpm/gpu-unlock.rpmlintrc` filters do not match the rpmlint message actually emitted (`arch-independent-package-contains-binary-or-object`); left unchanged.
