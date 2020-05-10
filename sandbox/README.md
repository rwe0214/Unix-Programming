# Play in a Sandbox!?
< API hijacking, library injection >

## How to Compile
``` shell
$ make
gcc -g -fPIC -shared -o sandbox.so sandboxso.c -ldl
gcc -g -o sandbox sandbox.c
```

## How to Run
```shell
$ ./sandbox
no command given.

$ ./sandbox -h
usage: ./sandbox [-h] [-p sopath] [-d basedir] [--] cmd [cmd args ...]
	-h: this help text
	-p: set the path to sandbox.so, default = ./sandbox.so
	-d: the base directory that is allowed to access, default = .
	--: separate the arguments for sandbox and for the executed command

$ ./sandbox -- ls /
[sandbox] __xstat: access to / is not allowed
ls: cannot access '/': permission denied

$ ./sandbox -d / -- ls /
addons	cfg   initrd.img      lib64	  mnt	     root  srv	var
bin	dev   initrd.img.old  libx32	  opt	     run   sys	vmlinuz
boot	etc   lib	      lost+found  platforms  sbin  tmp	vmlinuz.old
cdrom	home  lib32	      media	  proc	     snap  usr

$ ./sandbox -- ls -la / Makefile >log 2>&1
$ cat log
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] opendir: access to / is not allowed
-rw-rw-r--  1 1000 1000  435  5月 10 14:13 Makefile

ls: cannot open directory '/'

```

## Error Messages
The return value of each rejected functions is `-1` or `NULL` depends on the return type of the rejected function.

Besides, the function always rejected will set `errno` to `EACCES`.

## The List of Monitored Functions
1. `chdir`
2. `chmod`
3. `chown`
4. `creat`
5. `fopen`
6. `link`
7. `mkdir`
8. `open`
9. `openat`
11. `opendir`
12. `readlink`
13. `remove`
14. `rename`
15. `rmdir`
16. `stat`
    * `__xstat` and `__xsata64`
17. `symlink`
18. `unlink`

## The List of Rejected Functions
1. `execl`
2. `execle`
3. `execlp`
4. `execv`
5. `execve`
6. `execvp`
7. `system`
