#ifndef PTRACE_H
#define PTRACE_H

#include <sys/ptrace.h>
#include <sys/types.h>

#define CPSR_T_MASK (1u << 5)

#if defined(__arm__)
    #define REGS_ARG_NUM 4
#endif

#if defined(__aarch64__)
    #define REGS_ARG_NUM 6
#endif

#if defined(__aarch64__)
    #define pt_regs user_pt_regs
    #define uregs regs
    #define ARM_r0 regs[0]
    #define ARM_lr regs[30]
    #define ARM_sp sp
    #define ARM_pc pc
    #define ARM_cpsr pstate
#endif

void ptrace_attach(pid_t pid);
void ptrace_detach(pid_t pid);
void ptrace_get_regs(pid_t pid, struct pt_regs *regs);
void ptrace_set_regs(pid_t pid, struct pt_regs *regs);
void ptrace_cont(pid_t pid);
void ptrace_write(pid_t pid, uint8_t *addr, uint8_t *data, size_t size);
long call_remote_func(pid_t pid, long func_addr, long *args, size_t argc);
long call_remote_func_from_namespace(pid_t pid, long func_addr,
                                     long return_addr, long *args, size_t argc);

#endif
