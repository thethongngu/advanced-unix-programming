//
// Created by thong on 5/11/20.
//

#include <getopt.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>

#include "utils.h"

using namespace std;

#define print_array(a, len) \
    cout << #a << " = ["; \
    for(int i = 0; i < len; i++) cout << a[i] << ", "; \
    cout << "]" << endl

#define print(a) cout << #a << " = " << a << endl

#define execute(a) \
    call_args[0]

string so_path, base_dir;
vector<string> call_args;

void get_arguments(int argc, char **argv) {
//    print(argc);

    int c;
    while ((c = getopt(argc, argv, "p:d:")) != -1) {
        if (c == 'p') {
            so_path = optarg;
        }
        if (c == 'd') {
            base_dir = optarg;
            base_dir = get_full_path(base_dir);
        }
    }

    for(int i = optind; i < argc; i++) {
        call_args.emplace_back(argv[i]);
    }
//    print_array(call_args, call_args.size());
}

static void init() __attribute__((constructor));

void init() {
    void *handle = dlopen(so_path.c_str(), RTLD_LAZY);
    static auto a = reinterpret_cast<int (*)(const char*, mode_t)>(dlsym(handle, "mkdir"));
//    a("a", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

int main(int argc, char **argv) {
    get_arguments(argc, argv);

    char *newargv[] = { "/usr/bin/mkdir a", nullptr };
//    char *newenviron[] = { BY_LAUCHER, nullptr };

//    int fd = open("~/Desktop/test.log", O_CREAT|O_WRONLY, 0600);
//    dup2(fd, 1);
//    dup2(fd, 2);
//    close(fd);

//    extern char **environ;
//    char **new_envp = copyenv(environ);

//    putenv(BY_LAUCHER);
//    execvpe("/usr/bin/mkdir", newargv, new_envp);
//    execve("mkdir", newargv);

    system("mkdir");

//    execvp("mkdir", reinterpret_cast<char *const *>("a"));
}