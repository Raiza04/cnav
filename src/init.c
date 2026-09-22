#include "platform.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void init(void) {
  char app_dir[1024];
  get_app_dir(app_dir, sizeof(app_dir));

  char progs[1048];
  int ret = snprintf(progs, sizeof(progs), "%s" PATH_SEP "tools.txt", app_dir);
  if (ret < 0 || (size_t)ret >= sizeof(progs)) {
    perror("Error: Application directory path is too long for the buffer.\n");
    return;
  }

  FILE *myfile = fopen(progs, "r");
  if (myfile == NULL) {
    myfile = fopen(progs, "w+");
    if (myfile == NULL) {
      perror("Error: Could not create the tools.txt configuration file");
      return;
    }
    (void)fprintf(myfile, "vim\nnano\ncat\ncode\nxdg-open\nopen\n");

    if (fseek(myfile, 0, SEEK_SET) != 0) {
      perror("Error: Could not reset file pointer");
      (void)fclose(myfile);
      return;
    }
  }

  char line[64];

  while (fgets(line, sizeof(line), myfile) != NULL) {
    line[strcspn(line, "\r\n")] = '\0';

    if (line[0] == '\0') {
      continue;
    }

#ifdef _WIN32
    // Windows PowerShell Syntax
    printf("function %s {\n"
           "    & (Get-Command %s -CommandType Application) $args\n"
           "    if ($LASTEXITCODE -eq 0) {\n"
           "        n --add %s $args $null 2>&1\n"
           "    }\n"
           "}\n\n",
           line, line, line);
#else
    // Linux/macOS Bash Syntax
    printf("%s() {\n"
           "    command %s \"$@\"\n"
           "    if [ $? -eq 0 ]; then\n"
           "        n --add %s \"$@\" > /dev/null 2>&1\n"
           "    fi\n"
           "}\n\n",
           line, line, line);
#endif
  }

  (void)fclose(myfile);
}
