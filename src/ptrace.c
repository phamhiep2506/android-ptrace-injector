#include "ptrace.h"

#include <errno.h>
#include <linux/elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

void ptrace_attach(pid_t pid) {
    printf("[+] Ptrace attach to process %d\n", pid);

    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) {
        printf("[-] Ptrace attach error %s\n", strerror(errno));
    }

    waitpid(pid, NULL, WUNTRACED);
}

void ptrace_detach(pid_t pid) {
    printf("[+] Ptrace detach from process %d\n", pid);

    if (ptrace(PTRACE_DETACH, pid, NULL, NULL) == -1) {
        printf("[-] Ptrace detach error %s\n", strerror(errno));
    }
}

void ptrace_get_regs(pid_t pid, struct pt_regs *regs) {
    struct iovec iov;
    iov.iov_base = regs;
    iov.iov_len  = sizeof(*regs);

    if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov) == -1) {
        printf("[-] Ptrace get regs error %s\n", strerror(errno));
    }
}

void ptrace_set_regs(pid_t pid, struct pt_regs *regs) {
    struct iovec iov;
    iov.iov_base = regs;
    iov.iov_len  = sizeof(*regs);

    if (ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iov) == -1) {
        printf("[-] Ptrace set regs error %s\n", strerror(errno));
    }
}

void ptrace_cont(pid_t pid) {
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
        printf("[-] Ptrace cout error %s\n", strerror(errno));
    }
}

void ptrace_write(pid_t pid, uint8_t *addr, uint8_t *data, size_t size) {
    printf("[+] Ptrace write %zu byte to address %p in process %d\n", size,
           addr, pid);

    for (size_t i = 0; i < size; i += sizeof(long)) {
        long buffer = 0;

        memcpy(&buffer, data + i, sizeof(buffer));

        if (ptrace(PTRACE_POKEDATA, pid, addr + i, buffer) == -1) {
            printf("[-] Ptrace write error %s\n", strerror(errno));
        }
    }
}

long call_remote_func(pid_t pid, long func_addr, long *args, size_t argc) {
    return call_remote_func_from_namespace(pid, func_addr, 0, args, argc);
}

long call_remote_func_from_namespace(pid_t pid, long func_addr,
                                     long return_addr, long *args,
                                     size_t argc) {
    struct pt_regs regs, original_regs;

    ptrace_get_regs(pid, &regs);

    // Backup regs
    memcpy(&original_regs, &regs, sizeof(regs));

    // Put the first 4 args to r0-r3
    for (int i = 0; i < argc && i < REGS_ARG_NUM; ++i) {
        regs.uregs[i] = args[i];
    }

    // Push the remainder to stack
    if (argc > REGS_ARG_NUM) {
        regs.ARM_sp -= (argc - REGS_ARG_NUM) * sizeof(long);
        long *data = args + REGS_ARG_NUM;
        ptrace_write(pid, (uint8_t *)regs.ARM_sp, (uint8_t *)data,
                     (argc - REGS_ARG_NUM) * sizeof(long));
    }

    regs.ARM_lr = return_addr;
    regs.ARM_pc = func_addr;

    if (regs.ARM_pc & 1) {
        // thumb
        regs.ARM_pc &= (~1u);
        regs.ARM_cpsr |= CPSR_T_MASK;
    } else {
        // arm
        regs.ARM_cpsr &= ~CPSR_T_MASK;
    }

    ptrace_set_regs(pid, &regs);
    ptrace_cont(pid);

    waitpid(pid, NULL, WUNTRACED);

    // To get return value;
    ptrace_get_regs(pid, &regs);
    ptrace_set_regs(pid, &original_regs);

    return regs.ARM_r0;
}
