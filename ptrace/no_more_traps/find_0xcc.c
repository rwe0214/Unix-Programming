#include <assert.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t child;
    if ((child = fork()) < 0)
        perror("fork");

    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
        perror("execvp");
    } else {
        int status;
        struct user_regs_struct regs;
        if (waitpid(child, &status, 0) < 0)
            perror("wait@parent");
        assert(WIFSTOPPED(status));
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
        ptrace(PTRACE_CONT, child, 0, 0);
        waitpid(child, &status, 0);
        assert(WIFSTOPPED(status));
        long long count = 0;
        FILE *fp = fopen("no_more_traps.txt", "r");

        while (WSTOPSIG(status) == SIGTRAP) {
            char buf[2];
            fread(buf, 2, 1, fp);
            unsigned long opcode;
            sscanf(buf, "%lx", &opcode);

            long long trap_addr;
            if (ptrace(PTRACE_GETREGS, child, 0, &regs) != 0)
                perror("ptrace(GETREGS)");
            trap_addr = regs.rip - 1;

            unsigned long code = ptrace(PTRACE_PEEKTEXT, child, trap_addr, 0);
            code = (code & 0xffffffffffffff00) | opcode;
            if (ptrace(PTRACE_POKETEXT, child, trap_addr,
                       ((code & 0xffffffffffffff00) | opcode)) != 0)
                perror("ptrace(POKETEXT)");
            regs.rip = trap_addr;
            if (ptrace(PTRACE_SETREGS, child, 0, &regs) != 0)
                perror("ptrace(SETREGS)");

            ptrace(PTRACE_CONT, child, 0, 0);
            waitpid(child, &status, 0);
        }
        fclose(fp);
    }
}
