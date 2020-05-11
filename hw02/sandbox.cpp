
#include <utility>
#include <dlfcn.h>
#include <cstdio>
#include <cerrno>
#include <string>
#include "utils.h"

using namespace std;

#define LIBC "libc.so.6"

extern "C" {

//    static int (*old_system)(const char*) = nullptr;
//    int system(const char *command) {
//        fprintf(stderr,"[sandbox] system(%s): not allowed\n", command);
//        return EACCES;
//    }
    // ------------------------------------------------------

    static int (*old_execl)(const char*, const char *) = nullptr;
    int execl(const char *path, const char *arg, ...) {
        fprintf(stderr,"[sandbox] execl(%s): not allowed\n", path);
        return EACCES;
    }
    // ------------------------------------------------------

//    static int (*old_execle)(const char*, const char *, ..., char * const) = nullptr;
//    int execle(const char *path, const char *arg, ..., char * const envp[]) {
//        fprintf(stderr,"[sandbox] execl(%s): not allowed\n", path);
//        return EACCES;
//    }
    // ------------------------------------------------------

    static int (*old_execlp)(const char*, const char *) = nullptr;
    int execlp(const char *file, const char *arg, ...) {
        fprintf(stderr,"[sandbox] execlp(%s): not allowed\n", file);
        return EACCES;
    }
    // ------------------------------------------------------

    static int (*old_execv)(const char*, char *const) = nullptr;
    int execv(const char *path, char *const argv[]) {
        fprintf(stderr,"[sandbox] execv(%s): not allowed\n", path);
        return EACCES;
    }
    // ------------------------------------------------------

    static int (*old_execve)(const char*, char *const, char *const) = nullptr;
    int execve(const char *pathname, char *const argv[], char *const envp[]) {

        if (old_execve == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_execve = reinterpret_cast<int (*)
                    (const char*, char *const, char *const)>(dlsym(handle, "execve"));
        }
//        fprintf(stderr, "%s\n", pathname);

        if (by_laucher(envp)) {
            old_execve(pathname, *argv, *envp);
            fprintf(stderr, "errrrrr\n");
        } else {
            fprintf(stderr,"[sandbox] execve(%s): not allowed\n", pathname);
        }
        return 0;
    }

    static int (*old_execvp)(const char*, char *const) = nullptr;
    int execvp(const char *file, char *const argv[]) {
        fprintf(stderr,"[sandbox] execvp(%s): not allowed\n", file);
        return EACCES;
    }



    // ----------------- FILE and DIR --------------------------

    static int (*old_chdir)(const char*) = nullptr;
    int chdir(const char *path, string x) {
        if (old_chdir == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if (handle != nullptr) old_chdir = reinterpret_cast<int (*)(const char *)>(dlsym(handle, "chdir"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: chdir: access to %s is not allowed", path);
            return EACCES;
        } else {
            if (old_chdir != nullptr) old_chdir(path);
            return 0;
        }
    }

    // ------------------------------------------------------

    static int (*old_chmod)(const char*, mode_t) = nullptr;
    int chmod(const char *path, mode_t mode, string x) {
        if(old_chmod == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_chmod = reinterpret_cast<int (*)(const char*, mode_t)>(dlsym(handle, "chmod"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: chmod: access to %s is not allowed", path);
            return EACCES;
        } else {
            if(old_chmod != nullptr) old_chmod(path, mode);
            return 0;
        }
    }

    // ------------------------------------------------------

    static int (*old_chown)(const char*, mode_t) = nullptr;
    int chown(const char *path, mode_t mode, string x) {
        if(old_chown == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_chown = reinterpret_cast<int (*)(const char*, mode_t)>(dlsym(handle, "chown"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: chown: access to %s is not allowed", path);
            return EACCES;
        } else {
            if(old_chown != nullptr) old_chown(path, mode);
            return 0;
        }
    }

    // ------------------------------------------------------

    static int (*old_creat)(const char*, mode_t) = nullptr;
    int creat(const char *pathname, mode_t mode, string x) {
        if(old_creat == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_creat = reinterpret_cast<int (*)(const char*, mode_t)>(dlsym(handle, "creat"));
        }
        if (!check_valid(pathname, std::move(x))) {
            fprintf(stderr, "[sandbox]: creat: access to %s is not allowed", pathname);
            return EACCES;
        } else {
            if(old_creat != nullptr) old_creat(pathname, mode);
            return 0;
        }
    }

    // ------------------------------------------------------
//    static FILE (*old_fopen)(const char*, const char*) = nullptr;
//    FILE *fopen(const char *pathname, const char *mode, int gap, string x) {
//        if(old_fopen == nullptr) {
//            void *handle = dlopen(LIBC, RTLD_LAZY);
//            if(handle != nullptr) old_fopen = reinterpret_cast<FILE (*)(const char*, const char*)>(dlsym(handle, "fopen"));
//        }
//        if (!check_valid(pathname, std::move(x))) {
//            fprintf(stderr, "[sandbox]: fopen: access to %s is not allowed", pathname);
//            return nullptr;
//        } else {
//            if(old_fopen != nullptr) return old_fopen(pathname, mode);
//        }
//    }


    // ------------------------------------------------------

    static int (*old_mkdir)(const char*, mode_t) = nullptr;
    int mkdir(const char *path, mode_t mode, string x) {
        if(old_mkdir == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_mkdir = reinterpret_cast<int (*)(const char*, mode_t)>(dlsym(handle, "mkdir"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: mkdir: access to %s is not allowed", path);
            return EACCES;
        } else {
            if(old_mkdir != nullptr) old_mkdir(path, mode);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_open)(const char *, int, mode_t) = nullptr;
    int open(const char *pathname, int flags, mode_t mode, string x) {
        if(old_open == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_open = reinterpret_cast<int (*)(const char*, int, mode_t)>(dlsym(handle, "open"));
        }
        if (!check_valid(pathname, std::move(x))) {
            fprintf(stderr, "[sandbox]: open: access to %s is not allowed", pathname);
            return EACCES;
        } else {
            if(old_open != nullptr) old_open(pathname, flags, mode);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_opendir)(const char *) = nullptr;
    int opendir(const char *name, string x) {
        if (old_opendir == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if (handle != nullptr) old_opendir = reinterpret_cast<int (*)(const char *)>(dlsym(handle, "opendir"));
        }
        if (!check_valid(name, std::move(x))) {
            fprintf(stderr, "[sandbox]: opendir: access to %s is not allowed", name);
            return EACCES;
        } else {
            if (old_opendir != nullptr) old_opendir(name);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_readlink)(const char *, char*, size_t) = nullptr;
    int readlink(const char *path, char *buf, size_t bufsize, string x) {
        if(old_readlink == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_readlink = reinterpret_cast<int (*)(const char *, char*, size_t)>(dlsym(handle, "readlink"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: readlink: access to %s is not allowed", path);
            return EACCES;
        } else {
            if(old_readlink != nullptr) old_readlink(path, buf, bufsize);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_rmdir)(const char *) = nullptr;
    int rmdir(const char *pathname, string x) {
        if(old_rmdir == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_rmdir = reinterpret_cast<int (*)(const char*)>(dlsym(handle, "rmdir"));
        }
        if (!check_valid(pathname, std::move(x))) {
            fprintf(stderr, "[sandbox]: rmdir: access to %s is not allowed", pathname);
            return EACCES;
        } else {
            if(old_rmdir != nullptr) old_rmdir(pathname);
            return 0;
        }
    }

    //-------------------------------------------------
//    static int (*old_remove)(const char *) = nullptr;
//    int remove(const char *pathname, string x) {
//        if(old_remove == nullptr) {
//            void *handle = dlopen(LIBC, RTLD_LAZY);
//            if(handle != nullptr) old_remove = reinterpret_cast<int (*)(const char*)>(dlsym(handle, "remove"));
//        }
//        if (!check_valid(pathname, std::move(x))) {
//            fprintf(stderr, "[sandbox]: remove: access to %s is not allowed", pathname);
//            return EACCES;
//        } else {
//            if(old_remove != nullptr) old_rmdir(pathname);
//            return 0;
//        }
//    }

    //-------------------------------------------------
    static int (*old_xstat)(const char *, struct stat*) = nullptr;
    int __xstat(const char *path, struct stat *buf, string x) {
        if(old_xstat == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_xstat = reinterpret_cast<int (*)(const char *, struct stat*)>(dlsym(handle, "__xstat"));
        }
        if (!check_valid(path, std::move(x))) {
            fprintf(stderr, "[sandbox]: __xstat: access to %s is not allowed", path);
            return EACCES;
        } else {
            if(old_xstat != nullptr) old_xstat(path, buf);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_unlink)(const char *) = nullptr;
    int unlink(const char *pathname, string x) {
        if(old_unlink == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_unlink = reinterpret_cast<int (*)(const char*)>(dlsym(handle, "unlink"));
        }
        if (!check_valid(pathname, std::move(x))) {
            fprintf(stderr, "[sandbox]: unlink: access to %s is not allowed", pathname);
            return EACCES;
        } else {
            if(old_unlink != nullptr) old_unlink(pathname);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_link)(const char *, const char *) = nullptr;
    int link(const char *existingpath, const char *newpath, string x) {
        if(old_link == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_link = reinterpret_cast<int (*)(const char*,  const char *)>(dlsym(handle, "link"));
        }
        if (!check_valid(existingpath, x)) {
            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", existingpath);
            return EACCES;
        } else if (!check_valid(newpath, x)) {
            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", newpath);
            return EACCES;
        } else {
            if(old_link != nullptr) old_link(existingpath, newpath);
            return 0;
        }
        }
    //-------------------------------------------------
//    static int (*old_rename)(const char *, const char *) = nullptr;
//    int rename(const char *oldname, const char *newname, const string& x) {
//        if(old_rename == nullptr) {
//            void *handle = dlopen(LIBC, RTLD_LAZY);
//            if(handle != nullptr) old_rename = reinterpret_cast<int (*)(const char*, const char *)>(dlsym(handle, "rename"));
//        }
//        if (!check_valid(oldname, x)) {
//            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", oldname);
//            return EACCES;
//        } else if (!check_valid(newname, x)) {
//            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", newname);
//            return EACCES;
//        } else {
//            if(old_rename != nullptr) old_rename(oldname, newname);
//            return 0;
//        }
//    }

    //-------------------------------------------------
    static int (*old_symlink)(const char *, const char *) = nullptr;
    int symlink(const char *actualpath, const char *sympath, string x) {
        if(old_symlink == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_symlink = reinterpret_cast<int (*)(const char*, const char *)>(dlsym(handle, "symlink"));
        }
        if (!check_valid(actualpath, x)) {
            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", actualpath);
            return EACCES;
        } else if (!check_valid(sympath, x)) {
            fprintf(stderr, "[sandbox]: link: access to %s is not allowed", sympath);
            return EACCES;
        } else {
            if(old_symlink != nullptr) old_symlink(actualpath, sympath);
            return 0;
        }
    }

    //-------------------------------------------------
    static int (*old_openat)(int, const char *, int, mode_t) = nullptr;
    int openat(int dirfd, const char *pathname, int flags, mode_t mode, string x) {
        if(old_openat == nullptr) {
            void *handle = dlopen(LIBC, RTLD_LAZY);
            if(handle != nullptr) old_openat = reinterpret_cast<int (*)(int, const char *, int, mode_t)>(dlsym(handle, "openat"));
        }
        if (!check_valid(pathname, std::move(x))) {
            fprintf(stderr, "[sandbox]: openat: access to %s is not allowed", pathname);
            return EACCES;
        } else {
            if(old_openat != nullptr) old_openat(dirfd, pathname, flags, mode);
            return 0;
        }
    }

}
