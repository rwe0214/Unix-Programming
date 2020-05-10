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

/*Check whether the requested directory is sub directory of base directory*/
int check_permission(const char *subdir_name)
{
    char my_name[1024];
    strncpy(my_name, ((subdir_name) ? subdir_name : "."), sizeof(my_name));

    char abs_base[1024];
    char abs_subdir[1024];

    strncpy(abs_base, getenv("BASE_DIR"), sizeof(abs_base));
    realpath(my_name, abs_subdir);

    if (strncmp(abs_base, abs_subdir, strlen(abs_base)) == 0)
        return 1;
    else
        return 0;
}

#define HOOK_VAR(func_type, func_name, func_paremeter) \
    func_type(*orig_##func_name) func_paremeter = NULL;

#define MONITORED_WRAP(func_type, func_name, func_para) \
    HOOK_VAR(func_type, func_name, func_para);          \
    func_type func_name func_para

#define ORIGIN_FUNC(func_name, func_para)                               \
    ({                                                                  \
        void *handle =                                                  \
            dlopen("/lib/x86_64-linux-gnu/libc-2.27.so", RTLD_LAZY);    \
        if (!handle)                                                    \
            fprintf(stderr, RED_BOLD "[sandbox] dlopen error\n" RESET); \
        else if (orig_##func_name == NULL)                              \
            orig_##func_name = dlsym(handle, #func_name);               \
        return orig_##func_name func_para;                              \
    })

#define PRINT_ERR(func_name, request_path)                                    \
    ({                                                                        \
        fprintf(stderr,                                                       \
                RED_BOLD "[sandbox] %s: access to %s is not allowed\n" RESET, \
                #func_name, request_path);                                    \
    })

#define MONITORED_FUNC(func_type, reject_ret, func_name, func_para, para_name) \
    MONITORED_WRAP(func_type, func_name, func_para)                            \
    {                                                                          \
        if (check_permission(request_path)) {                                  \
            ORIGIN_FUNC(func_name, para_name);                                 \
        } else {                                                               \
            PRINT_ERR(func_name, request_path);                                \
            return reject_ret;                                                 \
        }                                                                      \
    }
#define MONITORED_FUNC2(func_type, reject_ret, func_name, func_para, \
                        para_name)                                   \
    MONITORED_WRAP(func_type, func_name, func_para)                  \
    {                                                                \
        if (check_permission(request_path1) &&                       \
            check_permission(request_path2)) {                       \
            ORIGIN_FUNC(func_name, para_name);                       \
        } else {                                                     \
            if (check_permission(request_path1))                     \
                PRINT_ERR(func_name, request_path1);                 \
            if (check_permission(request_path2))                     \
                PRINT_ERR(func_name, request_path2);                 \
            return reject_ret;                                       \
        }                                                            \
    }

#define MONITORED_FUNC_LONG(func_type, reject_ret, func_name, func_para, \
                            para_name)                                   \
    MONITORED_WRAP(func_type, func_name, func_para)                      \
    {                                                                    \
        va_list args;                                                    \
        va_start(args, flags);                                           \
        if (check_permission(request_path)) {                            \
            ORIGIN_FUNC(func_name, para_name);                           \
        } else {                                                         \
            PRINT_ERR(func_name, request_path);                          \
            return reject_ret;                                           \
        }                                                                \
    }

MONITORED_FUNC(int, -1, chdir, (const char *request_path), (request_path));
MONITORED_FUNC(int,
               -1,
               chmod,
               (const char *request_path, mode_t mode),
               (request_path, mode));
MONITORED_FUNC(int,
               -1,
               chown,
               (const char *request_path, uid_t owner, gid_t group),
               (request_path, owner, group));
MONITORED_FUNC(int,
               -1,
               creat,
               (const char *request_path, mode_t mode),
               (request_path, mode));
MONITORED_FUNC(int,
               -1,
               mkdir,
               (const char *request_path, mode_t mode),
               (request_path, mode));
MONITORED_FUNC(DIR *,
               NULL,
               opendir,
               (const char *request_path),
               (request_path));
MONITORED_FUNC(ssize_t,
               -1,
               readlink,
               (const char *request_path, char *buf, size_t bufsiz),
               (request_path, buf, bufsiz));
MONITORED_FUNC(int, -1, remove, (const char *request_path), (request_path));
MONITORED_FUNC(int, -1, rmdir, (const char *request_path), (request_path));
MONITORED_FUNC(int,
               -1,
               __xstat,
               (int __ver, const char *request_path, struct stat *statbuf),
               (__ver, request_path, statbuf));
MONITORED_FUNC(int,
               -1,
               __xstat64,
               (int ver, const char *request_path, struct stat64 *stat_buf),
               (ver, request_path, stat_buf));
MONITORED_FUNC(int, -1, unlink, (const char *request_path), (request_path));

MONITORED_FUNC(FILE *,
               NULL,
               fopen,
               (const char *request_path, const char *mode),
               (request_path, mode));
/*function which have two request path*/
MONITORED_FUNC2(int,
                -1,
                link,
                (const char *request_path1, const char *request_path2),
                (request_path1, request_path2));
MONITORED_FUNC2(int,
                -1,
                rename,
                (const char *request_path1, const char *request_path2),
                (request_path1, request_path2));
MONITORED_FUNC2(int,
                -1,
                symlink,
                (const char *request_path1, const char *request_path2),
                (request_path1, request_path2));
/*Function which has variable arguments*/
MONITORED_FUNC_LONG(int,
                    -1,
                    open,
                    (const char *request_path, int flags, ...),
                    (request_path, flags, args));
MONITORED_FUNC_LONG(int,
                    -1,
                    open64,
                    (const char *request_path, int flags, ...),
                    (request_path, flags, args));
MONITORED_FUNC_LONG(int,
                    -1,
                    openat,
                    (const char *request_path, int flags, ...),
                    (request_path, flags, args));

#define FORBID_MSG(func_name, request_process)                            \
    ({                                                                    \
        fprintf(stderr, RED_BOLD "[sandbox] %s(%s): not allowed\n" RESET, \
                #func_name, request_process);                             \
        errno = EACCES;                                                   \
    })

#define FORBID_FUNC(func_type, func_name, func_para, request_path) \
    func_type func_name func_para                                  \
    {                                                              \
        FORBID_MSG(func_name, request_path);                       \
        return -1;                                                 \
    }

FORBID_FUNC(int, execl, (const char *path, const char *arg, ...), path);
FORBID_FUNC(int, execle, (const char *path, const char *arg, ...), path);
FORBID_FUNC(int, execlp, (const char *path, const char *arg, ...), path);
FORBID_FUNC(int, execv, (const char *path, char *const arg[]), path);
FORBID_FUNC(int,
            execve,
            (const char *filename, char *const argv[], char *const envp[]),
            filename);
FORBID_FUNC(int, execvp, (const char *file, char *const argv[]), file);
FORBID_FUNC(int, system, (const char *command), command);
