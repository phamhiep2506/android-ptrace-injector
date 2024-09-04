#include "inject.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "ptrace.h"
#include "utils.h"

long call_mmap(pid_t pid, size_t length) {
    long func_addr = get_remote_func_addr(pid, LIBC_PATH, ((long)(void *)mmap));

    printf("[+] Call mmap in process %d\n", pid);

    long params[6];
    params[0] = 0;
    params[1] = length;
    params[2] = PROT_READ | PROT_WRITE;
    params[3] = MAP_PRIVATE | MAP_ANONYMOUS;
    params[4] = 0;
    params[5] = 0;

    return call_remote_func(pid, func_addr, params, 6);
}

long call_munmap(pid_t pid, long addr, size_t length) {
    long func_addr =
        get_remote_func_addr(pid, LIBC_PATH, ((long)(void *)munmap));

    printf("[+] Call munmap address %p in process %d\n", (void *)addr, pid);

    long params[2];
    params[0] = addr;
    params[1] = length;

    return call_remote_func(pid, func_addr, params, 2);
}

long call_dlopen(pid_t pid, const char *library_path) {
    long func_addr =
        get_remote_func_addr(pid, LINKER_PATH, ((long)(void *)dlopen));

    printf("[+] Call dlopen library %s in process %d\n", library_path, pid);

    long mmap_ret = call_mmap(pid, 0x400);

    ptrace_write(pid, (uint8_t *)mmap_ret, (uint8_t *)library_path,
                 strlen(library_path) + 1);

    long params[2];
    params[0] = mmap_ret;
    params[1] = RTLD_NOW | RTLD_LOCAL;

    long vndk_addr = get_module_base_addr(pid, VNDK_PATH);
    long re =
        call_remote_func_from_namespace(pid, func_addr, vndk_addr, params, 2);

    call_munmap(pid, mmap_ret, 0x400);

    return re;
}

void inject_library(pid_t pid, const char *library_path) {
    ptrace_attach(pid);

    long handle = call_dlopen(pid, library_path);

    if (handle) {
        printf("[+] Inject library succesfully\n");
    } else {
        printf("[-] Inject library failed\n");
    }

    ptrace_detach(pid);
}
