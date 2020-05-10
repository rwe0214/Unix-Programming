#define _GNU_SOURCE
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RED_BOLD "\x1b[;31;1m"
#define GRN_BOLD "\x1b[;32;1m"
#define RESET "\x1b[0;m"

/*
 *
 *	The list of monitored system call:
 *	*[v] chdir
 *	*[v] chmod
 *	*[v] chown
 *	*[v] creat
 *	*[v] fopen
 *	*[v] link
 *	*[v] mkdir
 *	*[v] open
 *	*[v] openat
 *	*[v] opendir
 *	*[v] readlink
 *	*[v] remove
 *	*[v] rename
 *	*[v] rmdir
 *	*[v] stat
 *	*[v] symlink
 *	*[v] unlink
 *
 *	The list of rejected system call:
 *	*[v] execl
 *	*[v] execle
 *	*[v] execlp
 *	*[v] execv
 *	*[v] execve
 *	*[v] execvp
 *	*[v] system
 *
 */

int check_permission(const char *subdir_name)
{
    char my_name[1024];
    strncpy(my_name, ((subdir_name) ? subdir_name : "."), sizeof(my_name));

    char ab_base[1024];
    char ab_subdir[1024];

    strncpy(ab_base, getenv("HOME"), sizeof(ab_base));
    realpath(my_name, ab_subdir);

    if (strncmp(ab_base, ab_subdir, strlen(ab_base)) == 0)
        return 1;
    else
        return 0;
}

#define HOOK_VAR(func_type, func_name, func_paremeter) \
    func_type(*orig_##func_name) func_paremeter = NULL;

#define MONITORED_FUNC(func_type, func_name, func_parameter) \
    HOOK_VAR(func_type, func_name, func_parameter);          \
    func_type func_name func_parameter


#define ORIGIN_FUNC(func_name)                                          \
    ({                                                                  \
        void *handle =                                                  \
            dlopen("/lib/x86_64-linux-gnu/libc-2.27.so", RTLD_LAZY);    \
        if (!handle)                                                    \
            fprintf(stderr, RED_BOLD "[sandbox] dlopen error\n" RESET); \
        else if (orig_##func_name == NULL)                              \
            orig_##func_name = dlsym(handle, #func_name);               \
    })

#define PRINT_ERR(func_name, request_path)                                    \
    ({                                                                        \
        fprintf(stderr,                                                       \
                RED_BOLD "[sandbox] %s: access to %s is not allowed\n" RESET, \
                #func_name, request_path);                                    \
        errno = EACCES;                                                       \
    })

MONITORED_FUNC(int, chdir, (const char *path))
{
    if (check_permission(path)) {
        ORIGIN_FUNC(chdir);
        return orig_chdir(path);
    } else {
        PRINT_ERR(chdir, path);
        return -1;
    }
}

MONITORED_FUNC(int, chmod, (const char *pathname, mode_t mode))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(chmod);
        return orig_chmod(pathname, mode);
    } else {
        PRINT_ERR(chmod, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, chown, (const char *pathname, uid_t owner, gid_t group))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(chown);
        return orig_chown(pathname, owner, group);
    } else {
        PRINT_ERR(chown, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, creat, (const char *pathname, mode_t mode))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(creat);
        return orig_creat(pathname, mode);
    } else {
        PRINT_ERR(creat, pathname);
        return -1;
    }
}

MONITORED_FUNC(FILE *, fopen, (const char *pathname, const char *mode))
{
    if (check_permission(pathname) ||
        strcmp(pathname, "/proc/filesystems") == 0 ||
        strcmp(pathname, "/proc/mounts") == 0) {
        ORIGIN_FUNC(fopen);
        return orig_fopen(pathname, mode);
    } else {
        PRINT_ERR(fopen, pathname);
        return NULL;
    }
}

MONITORED_FUNC(int, link, (const char *oldpath, const char *newpath))
{
    if (check_permission(oldpath) && check_permission(newpath)) {
        ORIGIN_FUNC(link);
        return orig_link(oldpath, newpath);
    } else {
        if (check_permission(oldpath))
            PRINT_ERR(link, oldpath);
        if (check_permission(newpath))
            PRINT_ERR(link, newpath);
        return -1;
    }
}

MONITORED_FUNC(int, mkdir, (const char *pathname, mode_t mode))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(mkdir);
        return orig_mkdir(pathname, mode);
    } else {
        PRINT_ERR(mkdir, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, open, (const char *pathname, int flags, ...))
{
    va_list args;
    va_start(args, flags);
    if (check_permission(pathname)) {
        ORIGIN_FUNC(open);
        return orig_open(pathname, flags, args);
    } else {
        PRINT_ERR(open, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, open64, (const char *pathname, int flags, ...))
{
    va_list args;
    va_start(args, flags);
    if (check_permission(pathname)) {
        ORIGIN_FUNC(open64);
        return orig_open64(pathname, flags, args);
    } else {
        PRINT_ERR(open64, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, openat, (int dirfd, const char *pathname, int flags, ...))
{
    va_list args;
    va_start(args, flags);
    if (check_permission(pathname)) {
        ORIGIN_FUNC(openat);
        return orig_openat(dirfd, pathname, flags, args);
    } else {
        PRINT_ERR(openat, pathname);
        return -1;
    }
}

MONITORED_FUNC(DIR *, opendir, (const char *name))
{
    if (check_permission(name)) {
        ORIGIN_FUNC(opendir);
        return orig_opendir(name);
    } else {
        PRINT_ERR(opendir, name);
        return NULL;
    }
}

MONITORED_FUNC(ssize_t,
               readlink,
               (const char *pathname, char *buf, size_t bufsiz))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(readlink);
        return orig_readlink(pathname, buf, bufsiz);
    } else {
        PRINT_ERR(readlink, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, remove, (const char *pathname))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(remove);
        return orig_remove(pathname);
    } else {
        PRINT_ERR(remove, pathname);
        return -1;
    }
}

MONITORED_FUNC(int, rename, (const char *oldpath, const char *newpath))
{
    if (check_permission(oldpath) && check_permission(newpath)) {
        ORIGIN_FUNC(rename);
        return orig_rename(oldpath, newpath);
    } else {
        if (check_permission(oldpath))
            PRINT_ERR(rename, oldpath);
        if (check_permission(newpath))
            PRINT_ERR(rename, newpath);
        return -1;
    }
}

MONITORED_FUNC(int, rmdir, (const char *pathname))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(rmdir);
        return orig_rmdir(pathname);
    } else {
        PRINT_ERR(rmdir, pathname);
        return -1;
    }
}

MONITORED_FUNC(int,
               __xstat,
               (int __ver, const char *pathname, struct stat *statbuf))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(__xstat);
        return orig___xstat(__ver, pathname, statbuf);
    } else {
        PRINT_ERR(__xstat, pathname);
        return -1;
    }
}

MONITORED_FUNC(int,
               __xstat64,
               (int ver, const char *path, struct stat64 *stat_buf))
{
    if (check_permission(path)) {
        ORIGIN_FUNC(__xstat64);
        return orig___xstat64(ver, path, stat_buf);
    } else {
        PRINT_ERR(__xstat64, path);
        return -1;
    }
}

MONITORED_FUNC(int, symlink, (const char *target, const char *linkpath))
{
    if (check_permission(target) && check_permission(linkpath)) {
        ORIGIN_FUNC(symlink);
        return orig_symlink(target, linkpath);
    } else {
        if (check_permission(target))
            PRINT_ERR(symlink, target);
        if (check_permission(linkpath))
            PRINT_ERR(symlink, linkpath);
        return -1;
    }
}

MONITORED_FUNC(int, unlink, (const char *pathname))
{
    if (check_permission(pathname)) {
        ORIGIN_FUNC(unlink);
        return orig_unlink(pathname);
    } else {
        PRINT_ERR(unlink, pathname);
        return -1;
    }
}

#define FORBID_MSG(func_name, request_process)                            \
    ({                                                                    \
        fprintf(stderr, RED_BOLD "[sandbox] %s(%s): not allowed\n" RESET, \
                #func_name, request_process);                             \
        errno = EACCES;                                                   \
    })

int execl(const char *path, const char *arg, ...)
{
    FORBID_MSG(execl, path);
    return -1;
}

int execle(const char *path, const char *arg, ...)
{
    FORBID_MSG(execle, path);
    return -1;
}

int execlp(const char *path, const char *arg, ...)
{
    FORBID_MSG(execlp, path);
    return -1;
}

int execv(const char *path, char *const argv[])
{
    FORBID_MSG(execv, path);
    return -1;
}

int execve(const char *filename, char *const argv[], char *const envp[])
{
    FORBID_MSG(execve, filename);
    return -1;
}

int execvp(const char *file, char *const argv[])
{
    FORBID_MSG(execvp, file);
    return -1;
}

int system(const char *command)
{
    FORBID_MSG(system, command);
    return -1;
}
