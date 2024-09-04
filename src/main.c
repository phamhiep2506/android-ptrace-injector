#include <bits/get_device_api_level_inlines.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "app.h"
#include "inject.h"
#include "utils.h"

void show_usage(char *execute_name) {
    printf("Usage: %s [Options]\n", execute_name);
    printf("Options:\n");
    printf("-p <package> Set the package name\n");
    printf("-l <library> Set the library path\n");
    printf("-a Enable auto launch\n");
    printf("-h Show usage\n");
}

int main(int argc, char *argv[]) {
    int opt;
    bool auto_launch         = false;
    const char *package_name = NULL;
    const char *library_path = NULL;

    // Set value arguments
    while ((opt = getopt(argc, argv, "p:l:ah")) != -1) {
        switch (opt) {
        case 'p':
            package_name = optarg;
            break;
        case 'l':
            library_path = optarg;
            break;
        case 'a':
            auto_launch = true;
            break;
        case 'h':
        default:
            show_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }
    }

    // Check value arguments
    if (!package_name || !library_path) {
        printf("Missing required argument, see -h\n");
        exit(EXIT_FAILURE);
    }

    // Check android version
    if (get_android_version() < 10) {
        printf("[-] Android devices below 10 are not supported\n");
        exit(EXIT_FAILURE);
    }

    // Show info
    printf("[+] Package: %s\n", package_name);
    printf("[+] Library: %s\n", library_path);
    printf("[+] Auto launch: %s\n", (auto_launch ? "enable" : "disable"));

    // Launch app
    if (auto_launch) {
        launch_application(package_name);
    }

    // Wait pid
    pid_t pid = -1;
    while (pid <= 0) {
        pid = get_pid(package_name);
        sleep(1);
    }

    // Disable selinux
    set_selinux(false);

    // Inject library
    inject_library(pid, library_path);

    // Enable selinux
    set_selinux(true);

    return 0;
}
