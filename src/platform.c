#include "platform.h"

#include <stdio.h>

void get_app_dir(char *buffer, size_t max_size) {
    #ifdef _WIN32
        // on Windows we save in %LOCALAPPDATA%\cnav
        char *base = getenv("LOCALAPPDATA");
        if (base == NULL) {
            perror("LOCALAPPDATA nicht gefunden");
            exit(EXIT_FAILURE);
        }
        snprintf(buffer, max_size, "%s" PATH_SEP "cnav", base);
    #else
        // on Linux/macOS we save in ~/.local/share/cnav
        char *base = getenv("HOME");
        if (base == NULL) {
            perror("HOME nicht gefunden");
            exit(EXIT_FAILURE);
        }
        snprintf(buffer, max_size, "%s" PATH_SEP ".local" PATH_SEP "share" PATH_SEP "cnav", base);
    #endif
    MAKE_DIR(buffer);
}