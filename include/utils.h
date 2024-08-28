#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <sys/types.h>

int get_android_version();
pid_t get_pid(const char *package_name);
long get_module_base_addr(pid_t pid, const char *module_name);
long get_remote_func_addr(pid_t remote_pid, const char *module_name,
                          long local_func_addr);
void set_selinux(bool is_selinux);

#endif
