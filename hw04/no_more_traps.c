#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>


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
        int fd = open("./no_more_traps.txt", O_RDONLY);

        waitpid(child, &wait_status, 0);
        ptrace(PTRACE_CONT, child, 0, 0);

        long code;
        struct user_regs_struct regs;
        while (waitpid(child, &wait_status, 0) > 0) {
            if (WSTOPSIG(wait_status) != SIGTRAP) continue;

            ptrace(PTRACE_GETREGS, child, 0, &regs);

            char *override_byte = malloc(2 * sizeof(char));
            read(fd, override_byte, 2);
            unsigned long patch_code;
            sscanf(override_byte, "%lx", &patch_code);

            code = ptrace(PTRACE_PEEKTEXT, child, regs.rip - 1, 0);
            ptrace(PTRACE_POKETEXT, child, regs.rip - 1, (code & 0xffffffffffffff00 | patch_code));

            regs.rip = regs.rip - 1;
            ptrace(PTRACE_SETREGS, child, 0, &regs);
            ptrace(PTRACE_CONT, child, 0, 0);
       }
    }
}