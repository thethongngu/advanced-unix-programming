#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>

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
        fprintf(stderr, "usage: %s program [args ...]\n", argv[0]);
        return -1;
    }

    if ((child = fork()) < 0) errquit("fork");
    if (child == 0) {
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("prace@child");
        execvp(argv[1], argv + 1);
        errquit("execvp");
    } else {
        int wait_status;
        int enter = 0x01;
        long long counter = 0;
        if (waitpid(child, &wait_status, 0) < 0) errquit("waitpid");
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);
        while (WIFSTOPPED(wait_status)) {
            struct user_regs_struct regs;
            if (ptrace(PTRACE_SYSCALL, child, 0, 0) != 0) errquit("ptrace@parent");
            if (waitpid(child, &wait_status, 0) < 0) errquit("waidpid");
            if (!WIFSTOPPED(wait_status) || !(WSTOPSIG(wait_status) & 0x80)) continue;
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
        fprintf(stderr, "## %lld syscall(s) executed\n", counter);
    }
}