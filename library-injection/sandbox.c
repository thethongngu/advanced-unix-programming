#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <dirent.h>

#define LIB "libc.so.6"

static int (*old_chdir)(const char *path) = NULL;
static int (*old_chmod)(const char *pathname, mode_t mode) = NULL;
static int (*old_chown)(const char *pathname, uid_t owner, gid_t group) = NULL;
static int (*old_creat)(const char *pathname, mode_t mode) = NULL;
static FILE* (*old_fopen)(const char *pathname, const char *mode) = NULL;
static int (*old_link)(const char *path1, const char *path2) = NULL;
static int (*old_mkdir)(const char *path, mode_t mode) = NULL;
static int (*old_open)(const char *path, int oflag, ... ) = NULL;
static int (*old_openat)(int fd, const char *path, int oflag, ...) = NULL;
static DIR* (*old_opendir)(const char *name) = NULL;
static ssize_t (*old_readlink)(const char *restrict path, char *restrict buf, size_t bufsize) = NULL;
static int (*old_remove)(const char *pathname) = NULL;
static int (*old_rmdir)(const char *path) = NULL;
static int (*old___xstat)(int ver, const char *path, struct stat *stat_buf) = NULL;
static int (*old_symlink)(const char *path1, const char *path2) = NULL;
static int (*old_unlink)(const char *path) = NULL;

// ------------- macro -------------

#define PATH_PERMISSION(func, path)\
    if(!check_path(path)){\
        fprintf(stderr, "[sandbox] %s: access to %s is not allowed\n", #func, path); \
        errno = EACCES; \
        return returnedValue;\
    }

#define KEEP_FUNC(name)\
    if(old_##name == NULL){\
        void *handle = dlopen(LIB, RTLD_LAZY);\
        if(handle != NULL){\
            if ((old_##name = dlsym(handle, #name)) == NULL){\
                fprintf(stderr, "[sandbox] " #name " failed\n");\
                exit(EXIT_FAILURE);\
            }\
        }\
        else{\
            fprintf(stderr, "[sandbox] cannot open %s\n", LIB);\
            exit(EXIT_FAILURE);\
        }\
    }

#define HOOK1(name, formatRetStr, param1, formatStr1)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1);\
    return returnedValue;

#define HOOK2(name, formatRetStr, param1, formatStr1, param2, formatStr2)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2);\
    return returnedValue;

#define HOOK3(name, formatRetStr, param1, formatStr1, param2, formatStr2, param3, formatStr3)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2, param3);\
    return returnedValue;

#define HOOK4(name, formatRetStr, param1, formatStr1, param2, formatStr2, param3, formatStr3, param4, formatStr4)\
    if(old_##name != NULL)\
        returnedValue = old_##name(param1, param2, param3, param4);\
    return returnedValue;


bool check_path(const char *pathname){
    char *base_dir, *path;
    if((base_dir = realpath(getenv("BASE_DIR"), NULL)) == NULL) {
        fprintf(stderr, "[sandbox] cannot access to the base directory \"%s\"\n", getenv("BASE_DIR"));
        exit(EXIT_FAILURE);
    }

    if((path = realpath(pathname, NULL)) == NULL){
        fprintf(stderr, "[sandbox] cannot access to %s\n", pathname);
        exit(EXIT_FAILURE);
    }

    if (strlen(base_dir) > strlen(path)) return false;
    for(int i = 0; i < strlen(base_dir); i++)
        if (base_dir[i] != path[i]) return false;

    return true;
}


int chdir(const char *path){
    int returnedValue = -1;
    PATH_PERMISSION(chdir, path);
    KEEP_FUNC(chdir);
    HOOK1(chdir, %d, path, %s);
}

int chmod(const char *pathname, mode_t mode){
    int returnedValue = -1;
    PATH_PERMISSION(chmod, pathname);
    KEEP_FUNC(chmod);
    HOOK2(chmod, %d, pathname, %s, mode, %u);
}

int chown(const char *pathname, uid_t owner, gid_t group){
    int returnedValue = -1;
    PATH_PERMISSION(chown, pathname);
    KEEP_FUNC(chown);
    HOOK3(chown, %d, pathname, %s, owner, %u, group, %u);
}

int creat(const char *pathname, mode_t mode){

    int returnedValue = -1;

    char tmp[100];

    if (strchr(pathname, '/') == NULL) {
        tmp[0] = '.';  tmp[1] = '\0';
    } else {
        strcpy(tmp, pathname);
        for(int i = strlen(tmp) - 1; i > 0; i--) 
        if (tmp[i] != '/') tmp[i] = '\0';
        else break;
    }

    PATH_PERMISSION(creat, pathname);
    KEEP_FUNC(creat);
    HOOK2(creat, %d, pathname, %s, mode, %u);
}

FILE* fopen(const char *pathname, const char *mode){
    FILE* returnedValue = NULL;
    PATH_PERMISSION(fopen, pathname);
    KEEP_FUNC(fopen);
    HOOK2(fopen, %p, pathname, %s, mode, %s);
}

int link(const char *path1, const char *path2){
    int returnedValue = -1;
    PATH_PERMISSION(link, path1);
    PATH_PERMISSION(link, path2);
    KEEP_FUNC(link);
    HOOK2(link, %d, path1, %s, path2, %s);
}

int mkdir(const char *path, mode_t mode){
    int returnedValue = -1;

    char tmp[100];
    strcpy(tmp, path);
    if (strchr(path, '/') == NULL) {
        tmp[0] = '.';  tmp[1] = '\0';
    } else {
        for(int i = strlen(tmp) - 1; i > 0; i--) 
        if (tmp[i] != '/') tmp[i] = '\0';
        else break;
    }

    PATH_PERMISSION(mkdir, tmp);
    KEEP_FUNC(mkdir);
    HOOK2(mkdir, %d, path, %s, mode, %u);
}

int open(const char *path, int oflag, ... ){
    int returnedValue = -1;
    PATH_PERMISSION(open, path);
    KEEP_FUNC(open);

    if(oflag != O_CREAT){
        HOOK2(open, %d, path, %s, oflag, %d);
    }

    if(oflag == O_CREAT){
        mode_t mode = 0;
        va_list arg;
        va_start(arg, oflag);
        mode = va_arg(arg, mode_t);
        va_end (arg);
        HOOK3(open, %d, path, %s, oflag, %d, mode, %u);
    }
}

int openat(int fd, const char *path, int oflag, ...){
    int returnedValue = -1;
    PATH_PERMISSION(openat, path);
    KEEP_FUNC(openat);

    if(oflag != O_CREAT){
        HOOK3(openat, %d, fd, %d, path, %s, oflag, %d);
    }

    if(oflag == O_CREAT){
        mode_t mode = 0;
        va_list arg;
        va_start(arg, oflag);
        mode = va_arg(arg, mode_t);
        va_end (arg);
        HOOK4(openat, %d, fd, %d, path, %s, oflag, %d, mode, %u);
    }
}

DIR* opendir(const char *name){
    DIR* returnedValue = NULL;
    PATH_PERMISSION(opendir, name);
    KEEP_FUNC(opendir);
    HOOK1(opendir, %p, name, %s);
}

ssize_t readlink(const char *restrict path, char *restrict buf, size_t bufsize){
    ssize_t returnedValue = -1;
    PATH_PERMISSION(readlink, path);
    KEEP_FUNC(readlink);
    HOOK3(readlink, %ld, path, %s, buf, %s, bufsize, %lu);
}

int remove(const char *pathname){
    int returnedValue = -1;
    PATH_PERMISSION(remove, pathname);
    KEEP_FUNC(remove);
    HOOK1(remove, %d, pathname, %s);
}

int rmdir(const char *path){
    int returnedValue = -1;
    PATH_PERMISSION(rmdir, path);
    KEEP_FUNC(rmdir);
    HOOK1(rmdir, %d, path, %s);
}

int __xstat(int ver, const char *path, struct stat *stat_buf){
    int returnedValue = -1;
    PATH_PERMISSION(__xstat, path);
    KEEP_FUNC(__xstat);
    if(old___xstat != NULL) returnedValue = old___xstat(ver, path, stat_buf);
    return returnedValue;
}

int symlink(const char *path1, const char *path2){
    int returnedValue = -1;
    PATH_PERMISSION(symlink, path1);
    PATH_PERMISSION(symlink, path2);
    KEEP_FUNC(symlink);
    HOOK2(symlink, %d, path1, %s, path2, %s);
}

int unlink(const char *path){
    int returnedValue = -1;
    PATH_PERMISSION(unlink, path);
    KEEP_FUNC(unlink);
    HOOK1(unlink, %d, path, %s);
}


// ----------- exec func ---------------

#define HOOK_EXEC(name, var1, ...) \
    int name(const char *var1, ##__VA_ARGS__) { \
        fprintf(stderr, "[sandbox] %s(%s): not allowed\n", #name, var1); \
        return EACCES; \
    }

HOOK_EXEC(execl, path, const char *arg, ...);
HOOK_EXEC(execlp, file, const char *arg, ...);
HOOK_EXEC(execle, path, const char *arg, ...);
HOOK_EXEC(execv, path, char *const argv[]);
HOOK_EXEC(execve, filename, char *const argv[], char *const envp[]);
HOOK_EXEC(execvp, file, char *const argv[]);
HOOK_EXEC(system, command);