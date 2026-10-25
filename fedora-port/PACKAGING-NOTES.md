# Fedora packaging notes (dkms vs akmods, Secure Boot/MOK, dracut)

Labels: OBSERVED = read from the Fedora 44 packages/repo this run; INFERRED = reasoning, not tested.

## Packages inspected (OBSERVED; downloaded and unpacked in a sandbox, nothing installed)
dkms-3.4.4-1.fc44.noarch, akmods-0.6.2-14.fc44.noarch, kmodtool-1.2.1-1.fc44.noarch.

## dkms (what the repo's spec uses today)
- OBSERVED: dkms 3.4.4 /etc/dkms/framework.conf documents `try_sign_modules` (default `not_in_chroot`), `mok_signing_key=/var/lib/dkms/mok.key`, `mok_certificate=/var/lib/dkms/mok.pub`, `sign_file`.
- INFERRED: with Secure Boot on, a DKMS-built gpu_unlock.ko loads only if the user's MOK key/cert pair is enrolled (`mokutil --import /var/lib/dkms/mok.pub`, reboot, Enroll MOK). The spec does not do this; users must.
- OBSERVED (spec): %post/%posttrans run `dkms add/build/install` for every kernel with headers, then `dracut --force` unless framework.conf already has a `post_transaction=...dracut` hook.

## akmods (alternative for Fedora)
- OBSERVED: akmods ships /usr/bin/akmods, akmodsbuild, kmodgenca, services akmods.service / akmods@.service / akmods-keygen, a kernel-install hook `95-akmodsposttrans.install`, and README.secureboot: keys in /etc/pki/akmods/{certs,private}, enrol with `mokutil --import /etc/pki/akmods/certs/public_key.der`.
- OBSERVED: akmods Requires `kernel-devel-matched` (if kernel-core), rpmdev-vercmp, etc.
- INFERRED: a Fedora-native package would be a `gpu-unlock-kmod` SRPM built with kmodtool (akmod-gpu-unlock + gpu-unlock-kmod-common). It is how RPM Fusion ships out-of-tree modules and gives automatic signing with the akmods key. It needs an RPM-Fusion-style packaging effort and a licence review; not done here. The existing dkms spec stays valid on Fedora; akmods is optional extra work, not a replacement.

## dracut drop-in (issues #54, #66)
- OBSERVED: packaging/boot/dracut.conf (`force_drivers+=" gpu_unlock "`) installs to /usr/lib/dracut/dracut.conf.d/gpu-unlock.conf and is listed in `%files dkms`.
- OBSERVED: the spec only regenerates initramfs when the dkms framework.conf lacks a dracut post_transaction hook.
- INFERRED / needs hardware validation: ordering of gpu_unlock before nvidia in the initramfs on Fedora, and #66 (RTX 3080 LHR 10de:2216) behaviour. Not claimed here.

## Secure Boot summary
Module signing is the user's responsibility with both dkms (MOK key in /var/lib/dkms) and akmods (/etc/pki/akmods). Document it in the README for Fedora; do not attempt to enrol keys from a package scriptlet.
