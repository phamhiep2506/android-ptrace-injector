#include "utils.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/system_properties.h>
#include <sys/types.h>
#include <unistd.h>

int get_android_version() {
    char version[PROP_VALUE_MAX + 1];

    __system_property_get("ro.build.version.release", version);

    int android_version = atoi(version);

    return android_version;
}

pid_t get_pid(const char *package_name) {
    DIR *dir = opendir("/proc");

    if (package_name == NULL || dir == NULL) {
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        int pid = atoi(entry->d_name);

        if (pid != 0) {
            char file_name[30];
            char temp_name[50];

            snprintf(file_name, 30, "/proc/%d/cmdline", pid);
            FILE *fp = fopen(file_name, "r");

            if (fp != NULL) {
                fgets(temp_name, 50, fp);
                fclose(fp);

                if (strcmp(package_name, temp_name) == 0) {
                    return pid;
                }
            }
        }
    }

    return -1;
}

void set_selinux(bool is_selinux) {
    char buffer[2];
    char file_line[1024];

    FILE *fp = fopen("/proc/mounts", "r");

    while (fgets(file_line, sizeof(file_line), fp)) {
        if (strstr(file_line, "selinuxfs")) {
            strtok(file_line, " ");
            char *selinux_dir  = strtok(NULL, " ");
            char *selinux_path = strcat(selinux_dir, "/enforce");

            FILE *fp_selinux = fopen(selinux_path, "w");

            if (is_selinux) {
                strcpy(buffer, "1"); // set selinux to enforce
            } else {
                strcpy(buffer, "0"); // set selinux to permissive
            }

            fwrite(buffer, strlen(buffer), 1, fp_selinux);
            fclose(fp_selinux);
            break;
        }
    }

    fclose(fp);
}

long get_module_base_addr(pid_t pid, const char *module_name) {
    long base_addr = 0;
    char file_name[50];
    char file_line[1024];

    if (pid == -1) {
        return 0;
    }

    snprintf(file_name, sizeof(file_name), "/proc/%d/maps", pid);
    FILE *fp = fopen(file_name, "r");

    if (fp != NULL) {
        while (fgets(file_line, sizeof(file_line), fp)) {
            if (strstr(file_line, module_name)) {
                char *addr = strtok(file_line, "-");
                base_addr  = strtoul(addr, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }

    return base_addr;
}

long get_remote_func_addr(pid_t remote_pid, const char *module_name,
                          long local_func_addr) {
    pid_t pid = getpid();

    long local_module_addr  = get_module_base_addr(pid, module_name);
    long remote_module_addr = get_module_base_addr(remote_pid, module_name);

    if (local_module_addr == 0 || remote_module_addr == 0) {
        return 0;
    }

    long remote_func_addr =
        local_func_addr + (remote_module_addr - local_module_addr);

    return remote_func_addr;
}
