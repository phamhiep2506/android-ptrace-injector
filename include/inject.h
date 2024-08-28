#ifndef INJECT_H
#define INJECT_H

#include <sys/types.h>

#if defined(__aarch64__)
    #define LIBC_PATH "/apex/com.android.runtime/lib64/bionic/libc.so"
    #define LINKER_PATH "/apex/com.android.runtime/lib64/bionic/libdl.so"
    #define LIBC_PATH_OLD "/system/lib64/libc.so"
    #define LINKER_PATH_OLD "/system/lib64/libdl.so"
    #define VNDK_PATH "/system/lib64/libRS.so"
#endif

#if defined(__arm__)
    #define LIBC_PATH "/apex/com.android.runtime/lib/bionic/libc.so"
    #define LINKER_PATH "/apex/com.android.runtime/lib/bionic/libdl.so"
    #define LIBC_PATH_OLD "/system/lib/libc.so"
    #define LINKER_PATH_OLD "/system/lib/libdl.so"
    #define VNDK_PATH "/system/lib/libRS.so"
#endif

const char *get_libc_path();
const char *get_linker_path();
long call_mmap(pid_t pid, size_t length);
long call_munmap(pid_t pid, long addr, size_t length);
long call_dlopen(pid_t pid, const char *library_path);
void inject_library(pid_t pid, const char *library_path);

#endif
