#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	pid_t child;
	
	if((child = fork())<0)
		perror("fork");
	
	if(child==0){
		if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) perror("ptrace@child");
		execvp(argv[1], argv+1);
		perror("execvp");
	}
	else{
		long long counter = 0LL;
		int status;
		int enter = 0x01;
		if(waitpid(child, &status, 0) < 0) perror("wait@parent");
		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);
		while(WIFSTOPPED(status)){
			if(ptrace(PTRACE_SYSCALL, child, 0, 0)!=0)perror("ptrace@parent");
			if(waitpid(child, &status, 0) < 0) perror("wait@parent");

			//check whether stopped by syscall
			if(!WIFSTOPPED(status) || !(WSTOPSIG(status)& 0x80)) continue;
			if(enter)
				counter++;
			enter ^= 0x01;
		}
		fprintf(stderr, "Flag: ASM{%lld}\n", counter);
	}
	return 0;
}
