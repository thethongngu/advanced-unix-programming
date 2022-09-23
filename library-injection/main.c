//
// Created by thong on 5/11/20.
//

#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>
#include <string.h>

int main(int argc, char **argv) {
    char *so_path = "./sandbox.so";
    char *base_dir = ".";
    int c;

    while ((c = getopt(argc, argv, "p:d:")) != -1) {
        if (c == 'p') {
            so_path = optarg;
        } else if (c == 'd') {
            base_dir = optarg;
        } else {
            fprintf(stderr, "Usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n \
            -p: set the path to sandbox.so, default = ./sandbox.so\n \
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
    int i, cnt = 0;
    for(i = optind; i < argc; i++) {
        cmd_args[i - optind] = argv[i];
        cnt++;
        // printf("argv[%d] = %s\n", i - optind, cmd_args[i - optind]);
    }
    
    if (cnt == 1) {
        cmd_args[i - optind] = ".";
        i++;
        cnt++;
    }
    cmd_args[i - optind] = (char *)(0);

    char so_path_env[sizeof("LD_PRELOAD=") + strlen(so_path)];
    char base_dir_env[sizeof("BASE_DIR=") + strlen(base_dir)];

    sprintf(so_path_env, "LD_PRELOAD=%s", so_path);
    sprintf(base_dir_env, "BASE_DIR=%s", base_dir);
    
    // printf("%s\n", cmd_args[0]);
    // printf("%s\n", cmd_args[1]);

    char *envp[] = {so_path_env, base_dir_env, NULL};
    if (execvpe(cmd_args[0], cmd_args, envp) == -1) perror(cmd_args[0]);
}