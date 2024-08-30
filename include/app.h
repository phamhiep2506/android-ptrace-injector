#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>

void launch_application(const char *package_name) {
    char cmd[1024];
    sprintf(cmd,
            "am start $(cmd package resolve-activity --brief %s | tail -n 1)",
            package_name);
    system(cmd);
}

#endif
