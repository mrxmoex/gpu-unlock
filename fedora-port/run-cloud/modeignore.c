#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
 * Sandbox-only emulation of a filesystem whose chmod cannot clear the
 * execute bit. Every libc chmod/fchmod/fchmodat is performed as mode 0755.
 * The kernel stores that mode, so stat, execve, and rpm agree. This is not
 * an exFAT or NTFS mount: only processes with this object loaded are
 * affected, and open(O_CREAT) still uses the caller's mode.
 */

static void note_load(void)
{
    static int done;
    FILE *out;
    char exe[256];
    ssize_t n;

    if (done)
        return;
    done = 1;
    out = fopen("/tmp/modeignore-loaded.txt", "a");
    if (!out)
        return;
    n = readlink("/proc/self/exe", exe, sizeof exe - 1);
    if (n < 0)
        n = 0;
    exe[n] = '\0';
    fprintf(out, "loaded %s\n", exe);
    fclose(out);
}

int fchmodat(int dirfd, const char *path, mode_t mode, int flags)
{
    (void)mode;
    note_load();
    return syscall(SYS_fchmodat, dirfd, path, 0755, flags);
}

int chmod(const char *path, mode_t mode)
{
    return fchmodat(AT_FDCWD, path, mode, 0);
}

int fchmod(int fd, mode_t mode)
{
    (void)mode;
    note_load();
    return syscall(SYS_fchmod, fd, 0755);
}
