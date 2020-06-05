#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	pid_t child;
	if((child = fork()) < 0) perror("fork");
	if(child==0){
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)perror("ptrace@child");
		execvp(argv[1], argv+1);
		perror("execv");
	}
	else{
		int status;
		long long counter = 0LL;
		if(waitpid(child, &status, 0) < 0) perror("wait");
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
		while (WIFSTOPPED(status)){
			counter++;
			if(ptrace(PTRACE_SINGLESTEP, child, 0, 0)<0)perror("ptrace@parent");
			if(waitpid(child, &status, 0)<0)perror("wait");
		}
		fprintf(stderr, "Flag: ASM{%lld}\n", counter);
		perror("done");
	}
	return 0;
}

