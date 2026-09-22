#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

void get_app_dir(char *buffer, size_t max_size) {
  int ret;

#ifdef _WIN32
  // on Windows we save in %LOCALAPPDATA%\cnav
  char *base = getenv("LOCALAPPDATA");
  if (base == NULL) {
    perror("LOCALAPPDATA not found");
    exit(EXIT_FAILURE);
  }
  ret = snprintf(buffer, max_size, "%s" PATH_SEP "cnav", base);
#else
  // on Linux/macOS we save in ~/.local/share/cnav
  char *base = getenv("HOME");
  if (base == NULL) {
    perror("HOME not found");
    exit(EXIT_FAILURE);
  }
  ret = snprintf(buffer, max_size,
                 "%s" PATH_SEP ".local" PATH_SEP "share" PATH_SEP "cnav", base);
#endif
  if (ret < 0) {
    perror("Failed to resolve the path to cnav folder");
    exit(EXIT_FAILURE);
  }
  MAKE_DIR(buffer);
}
