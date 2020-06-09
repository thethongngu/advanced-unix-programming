#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <sys/user.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>

static void err_doit(int errnoflag, int error, const char *fmt, va_list ap) {
	char buf[1024];
	vsnprintf(buf, 1024-1, fmt, ap);
	if (errnoflag) snprintf(buf+strlen(buf), 1024 - strlen(buf)-1, ": %s",strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);
}

void errquit(const char *fmt, ...){
	va_list	ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}

int main(int argc, char *argv[]) {
    pid_t child;
    if (argc < 2) {
        fprintf(stderr, "usage: %s program\n", argv[0]);
        return -1;
    }

    child = fork();
    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        execvp(argv[1], argv + 1);
        fprintf(stdout, "execvp\n");
    } else {
        int wait_status;
        waitpid(child, &wait_status, 0);
        assert(WIFSTOPPED(wait_status));
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);

        ptrace(PTRACE_CONT, child, 0, 0);        
        waitpid(child, &wait_status, 0);

        int enter = 0x01;
        long long counter = 0;
        while (WIFSTOPPED(wait_status)) {
            printf("asdf");
            struct user_regs_struct regs;
            if (ptrace(PTRACE_SYSCALL, child, 0, 0) != 0) errquit("ptrace@parent");
            if (waitpid(child, &wait_status, 0) < 0) errquit("waidpid");
            // if (!WIFSTOPPED(wait_status) || !(WSTOPSIG(wait_status) & 0x80)) continue;
            if (ptrace(PTRACE_GETREGS, child, 0, &regs) != 0) errquit("ptrace@parent");
            if (enter) {
                fprintf(stderr, "0x%llx: rax=%llx rdi=%llx rsi=%llx rdx=%llx r10=%llx r8=%llx r9=%llx", regs.rip - 2, regs.orig_rax, regs.rdi, regs.rsi, regs.rdx, regs.r10, regs.r8, regs.r9);
                if (regs.orig_rax == 0x3c || regs.orig_rax == 0xe7) fprintf(stderr, "\n");
                counter++;
            } else {
                fprintf(stderr, "0x%llx: ret= 0x%llx\n", regs.rip - 2, regs.rax);
            }
            enter ^= 0x01;
        }

        waitpid(child, &wait_status, 0);

        printf("%d\n", WEXITSTATUS(wait_status));
        printf("%d\n", wait_status);
        fprintf(stdout, "done\n");
    }
    return 0;
}