#ifndef INJECT_H
#define INJECT_H

#include <sys/types.h>

#define LIBC_PATH "/apex/com.android.runtime/lib64/bionic/libc.so"
#define LINKER_PATH "/apex/com.android.runtime/lib64/bionic/libdl.so"
#define VNDK_PATH "/system/lib64/libRS.so"

long call_mmap(pid_t pid, size_t length);
long call_munmap(pid_t pid, long addr, size_t length);
long call_dlopen(pid_t pid, const char *library_path);
void inject_library(pid_t pid, const char *library_path);

#endif
