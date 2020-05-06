# Play in a Sandbox!?
< API hijacking, library injection >

## How to Compile
``` shell
$ make
gcc -g -fPIC -shared -o sandbox.so sandboxso.c -ldl
gcc -g -o sandbox sandbox.c -ldl
```

## How to Run
```shell
$ ./sandbox -h
usage: ./sandbox [-h] [-p sopath] [-d basedir] [--] cmd [cmd args ...]
	-h: this help text
	-p: set the path to sandbox.so, default = ./sandbox.so
	-d: the base directory that is allowed to access, default = .
	--: separate the arguments for sandbox and for the executed command
```

## The List of Monitored Function
1. chdir 
2. chmod 
3. chown 
4. creat 
5. fopen 
6. link 
7. mkdir 
8. open 
9. openat 
11. opendir 
12. readlink 
13. remove 
14. rename 
15. rmdir 
16. stat 
17. symlink 
18. unlink
19. execl 
20. execle 
21. execlp 
22. execv 
23. execve 
24. execvp 
25. system
