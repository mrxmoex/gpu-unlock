# NVIDIA 610.43.03 open kernel modules vs kernel 7.x: diagnosis

Honest note: this is NVIDIA's code (MIT/GPL dual-licensed open modules), not this project's. Verdict: **NOT FIXED.** There is no safe one-hunk or compiler-flag fix.

Observed (F43/F44/rawhide, kernel 7.2.11 / 7.4.0-rc0):
1. First error: `nvidia/os-interface.c:773: implicit declaration of function 'strncpy'`. The 7.2 kernel headers no longer declare `strncpy()` at all. `KCFLAGS="-include linux/string.h"` therefore does nothing (tested: same error).
2. A shim header (`-include` of a header defining strncpy) made it worse (765 errors) and was dropped.
3. A local patch replacing the call with `strscpy_pad()` (fedora-port/nvidia-610.43.03-strncpy-to-strscpy_pad.patch, one site only) fixes that one line, and the build then fails elsewhere (nvswitch). NVIDIA main (615.78.08) already uses `strscpy` at that site (logs in staging folder).
4. Running `make -k` (all errors), F44: 95 `error:` lines. Besides strncpy, the kernel 7.2 renamed/reworked DRM atomic state: `struct drm_atomic_state` -> `struct drm_atomic_commit` (`drm_atomic_state_alloc` -> `drm_atomic_commit_alloc`, `drm_atomic_state_put` -> `drm_atomic_commit_put`). That breaks nvidia-drm (color-pipeline, connector, crtc, drv, helper), plus nvswitch and nvidia-modeset sites. Rawhide (7.4-rc0) has 96 errors, one more `os-interface.c:2818` site.
5. F42 (6.19.14) builds NVIDIA 610.43.03 with zero errors.

Inferred (not tested): the fix is a newer NVIDIA source drop (the main branch already tracks these API changes, per its strscpy use), not a patch on 610.43.03. Bumping the pinned NVIDIA input would also change the sha256 checks and every firmware-generation assumption, so it needs a maintainer decision and hardware validation.
