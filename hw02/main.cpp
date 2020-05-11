//
// Created by thong on 5/11/20.
//

#define _GNU_SOURCE
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

string so_path, base_dir;

int main(int argc, char **argv) {
    so_path = "./sandbox.so";
    base_dir = ".";
    int c;

    while ((c = getopt(argc, argv, "p:d:")) != -1) {
        print(char(c));
        if (c == 'p') {
            so_path = optarg;
        } else if (c == 'd') {
            base_dir = optarg;
            base_dir = get_full_path(base_dir);
        } else {
            fprintf(stderr, "Usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n \
            -p: set the path to sandbox.so, default = ./sandbox.so\n\
            -d: the base directory that is allowed to access, default = .\n\
            --: separate the arguments for sandbox and for the executed command\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "no command given.\n");
        exit(EXIT_FAILURE);
    }

    char *cmd_args[100];
    for(int i = optind; i < argc; i++) {
        cmd_args[i - optind] = argv[i];
        printf("argv[%d] = %s\n", i, cmd_args[i]);
    }

    char so_path_env[sizeof("LD_PRELOAD=") + so_path.size()];
    char base_dir_env[sizeof("BASE_DIR=") + base_dir.size()];

    sprintf(so_path_env, "LD_PRELOAD=%s", so_path.c_str());
    sprintf(base_dir_env, "BASE_DIR=%s", base_dir.c_str());

    char *envp[] = {so_path_env, base_dir_env, nullptr};
    if (execvpe(cmd_args[0], cmd_args, envp) == -1) perror(cmd_args[0]);
}