#pragma once

#include <stddef.h>

// Windows
#ifdef _WIN32
    #include <direct.h>
    #include <io.h>
    #include <process.h>
    
    #define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
    #define STRDUP _strdup
    #define MAKE_DIR(path) _mkdir(path)
    #define PATH_SEP "\\"
    #define GETCWD(buffer, size) _getcwd(buffer, size)
    #define FILE_EXISTS(path) (_access(path, 0) == 0)
    #define EXEC_PROG _execlp
#else
    // Linux/macOS
    #include <sys/stat.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <libgen.h>
    #define _XOPEN_SOURCE 700
    
    #define MAKE_DIR(path) mkdir(path, 0700)
    #define STRDUP strdup
    #define PATH_SEP "/"
    #define GETCWD(buffer, size) getcwd(buffer, size) 
    #define FILE_EXISTS(path) (access(path, F_OK) == 0)
    #define EXEC_PROG execlp
#endif

void get_app_dir(char *buffer, size_t max_size);