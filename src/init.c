#include "main.h"

void init(){
    char *home = getenv("HOME");
    if (home == NULL) {
        perror("Could not find the HOME directory");
        return;
    }

    char mainDir[1024];
    snprintf(mainDir, sizeof(mainDir), "%s/.local/share/cnav", home);    
    mkdir(mainDir, 0700);

    char progs[1024];
    snprintf(progs, sizeof(progs), "%s/.local/share/cnav/tools.txt", home);

    FILE *myfile = fopen(progs, "r");
    if (myfile == NULL)
    {
        myfile = fopen(progs, "w");
        fprintf(myfile, "vim\nnano\ncat\ncode\nxdg-open\nopen");
        fclose(myfile);
    }

    myfile = fopen(progs, "r");

    char line[64];
        
    while (fgets(line, sizeof(line), myfile) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        printf(
            "%s() {\n"
            "    command %s \"$@\"\n"
            "    if [ $? -eq 0 ]; then\n"
            "        n --add %s \"$@\"\n"
            "    fi\n"
            "}\n\n", 
            line, line, line
        );
    }

    fclose(myfile);
}