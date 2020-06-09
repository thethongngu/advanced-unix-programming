#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <stdlib.h>
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
        fprintf(stderr, "usage: %s program [args ...]\n", argv[0]);
        return -1;
    }

    if ((child = fork()) < 0) errquit("fork");
    if (child == 0) {
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) errquit("prace@child");
        execvp(argv[1], argv + 1);
        errquit("execvp");
    } else {
        long long counter = 0LL;
        int wait_status;
        if (waitpid(child, &wait_status, 0) < 0) errquit("wait");
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL);
        while (WIFSTOPPED(wait_status)) {
            counter++;
            if (ptrace(PTRACE_SINGLESTEP, child, 0, 0) < 0) errquit("ptrace@parent");
            if (waitpid(child, &wait_status, 0) < 0) errquit("wait");
        }
        fprintf(stderr, "## %lld instruction(s) executed\n", counter);
    }
}