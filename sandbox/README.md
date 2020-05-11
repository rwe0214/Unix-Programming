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
    
$ ls
Makefile  README.md  sandbox  sandbox.c  sandbox.so  sandboxso.c

$ ./sandbox ls
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
Makefile  README.md  sandbox  sandbox.c  sandbox.so  sandboxso.c

$ ./sandbox ls -a
./sandbox: invalid option -- 'a'
usage: ./sandbox [-h] [-p sopath] [-d basedir] [--] cmd [cmd args ...]
	-h: this help text
	-p: set the path to sandbox.so, default = ./sandbox.so
	-d: the base directory that is allowed to access, default = .
	--: separate the arguments for sandbox and for the executed command
    
$ ./sandbox -- ls -a
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
.  ..  Makefile  README.md  sandbox  sandbox.c	sandbox.so  sandboxso.c

$ ls /
addons  cdrom  etc         initrd.img.old  lib64       media  platforms  run   srv  usr      vmlinuz.old
bin     cfg    home        lib             libx32      mnt    proc       sbin  sys  var
boot    dev    initrd.img  lib32           lost+found  opt    root       snap  tmp  vmlinuz

$ ./sandbox -- ls /
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
[sandbox] __xstat: access to / is not allowed
[sandbox] opendir: access to / is not allowed
ls: cannot open directory '/'

$ ./sandbox -d / ls /
addons	cdrom  etc	   initrd.img.old  lib64       media  platforms  run   srv  usr      vmlinuz.old
bin	cfg    home	   lib		   libx32      mnt    proc	 sbin  sys  var
boot	dev    initrd.img  lib32	   lost+found  opt    root	 snap  tmp  vmlinuz

$ ./sandbox -- ls -la / Makefile
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
-rw-rw-r--  1 1000 1000  435  5月 10 14:13 Makefile

[sandbox] opendir: access to / is not allowed
ls: cannot open directory '/'

$ ./sandbox -- ls -la / Makefile >/dev/null
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] opendir: access to / is not allowed
ls: cannot open directory '/'

$ ./sandbox -- ls -la / Makefile >/dev/null 2>&1
[sandbox] fopen: access to /proc/filesystems is not allowed
[sandbox] fopen: access to /proc/mounts is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] fopen: access to /etc/passwd is not allowed
[sandbox] fopen: access to /etc/group is not allowed
[sandbox] opendir: access to / is not allowed

$ ./sandbox -- sh -c 'ls'
[sandbox] __xstat64: access to /home/swchiu/.local/bin/ls is not allowed
[sandbox] __xstat64: access to /usr/local/sbin/ls is not allowed
[sandbox] __xstat64: access to /usr/local/bin/ls is not allowed
[sandbox] __xstat64: access to /usr/sbin/ls is not allowed
[sandbox] __xstat64: access to /usr/bin/ls is not allowed
[sandbox] __xstat64: access to /sbin/ls is not allowed
[sandbox] __xstat64: access to /bin/ls is not allowed
[sandbox] __xstat64: access to /usr/games/ls is not allowed
[sandbox] __xstat64: access to /usr/local/games/ls is not allowed
[sandbox] __xstat64: access to /snap/bin/ls is not allowed
sh: 1: ls: not found
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
9. `open64`
10. `openat`
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
