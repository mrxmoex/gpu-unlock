# fedora-port (groundwork notes, 2026-10-25)
Docs and a partial patch only. No GPU IDs added or changed. Nothing here was loaded on real hardware.
- MATRIX.md - compile matrix F42/F43/F44/rawhide
- NVIDIA-610-COMPILE.md - why NVIDIA 610.43.03 does not build on kernel 7.x (not fixed)
- nvidia-610.43.03-strncpy-to-strscpy_pad.patch - partial local patch, one site only; does NOT make the build pass
- PACKAGING-NOTES.md - dkms vs akmods, Secure Boot/MOK, dracut drop-in
- gpu-id-coverage.md - supported_ids[] vs README tested table
- RUN-SUMMARY.md - the #92 rerun result
