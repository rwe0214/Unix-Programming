#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    char path[1024] = "./sandbox.so", base[1024] = ".";
    int opt;
    int path_flag = 0, base_flag = 0, arg_flag = 0;

    while ((opt = getopt(argc, argv, "p:d:h")) != -1) {
        switch (opt) {
        case 'p':
            strncpy(path, optarg, sizeof(path));
            path_flag = 1;
            break;
        case 'd':
            strncpy(base, optarg, sizeof(base));
            base_flag = 1;
            break;
        default:
            fprintf(stderr,
                    "usage: %s [-h] [-p sopath] [-d basedir] [--] cmd [cmd "
                    "args ...]\n",
                    argv[0]);
			fprintf(stderr, "\t-h: this help text\n");
            fprintf(
                stderr,
                "\t-p: set the path to sandbox.so, default = ./sandbox.so\n");
            fprintf(
                stderr,
                "\t-d: the base directory that is allowed to access, default "
                "= .\n");
            fprintf(stderr,
                    "\t--: separate the arguments for sandbox and for the "
                    "executed command\n");
            exit(EXIT_FAILURE);
        }
    }


    if (argc < 2)
        printf("no command given.\n");
    else {
        /* use optind to process the system call typed by user */
        char sopath[1024];
        char basepath[1024];
        realpath(path, sopath);
        realpath(base, basepath);
        setenv("BASE_DIR", basepath, 1);
        setenv("LD_PRELOAD", sopath, 1);
        int status = execvp(argv[optind], argv + optind);
        unsetenv("LD_PRELOAD");
        unsetenv("BASE_DIR");
    }
    return 0;
}
