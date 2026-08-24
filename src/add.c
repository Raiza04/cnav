#ifdef _WIN32
    // Windows-Variante
    #include <windows.h>
    #define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
#else
    // Linux/macOS-Variante
    #define _XOPEN_SOURCE 700
#endif

#include "add.h"
#include "search.h"

/*
    We want to add lines to the database in this form
    example:
    ~/Download/test.txt,test.txt,vim,<how many times was this exact line called>,<last call this line was called>,score

*/

void add(char* tmp_path, char* program) {
    char * path = realpath(tmp_path, NULL);

    if (path == NULL) {

        char *dir_path = strdup(tmp_path);
        char *base_name_str = strdup(tmp_path);
        
        char *dir = dirname(dir_path);
        char *base = basename(base_name_str);
        
        char *resolved_dir = realpath(dir, NULL);
        
        if (resolved_dir != NULL) {
            path = malloc(strlen(resolved_dir) + strlen(base) + 2);
            if (path != NULL) {
                if (strcmp(resolved_dir, "/") == 0) {
                    sprintf(path, "/%s", base);
                } else {
                    sprintf(path, "%s/%s", resolved_dir, base);
                }
            }
            free(resolved_dir);
        }
        
        free(dir_path);
        free(base_name_str);
    }

    if (path == NULL)
    {
        perror("Problem with the provided path");
        exit(EXIT_FAILURE);
    }
    
    char *home = getenv("HOME");
    if (home == NULL) {
        perror("Could not find the HOME directory");
        return;
    }

    char mainDir[1024];
    snprintf(mainDir, sizeof(mainDir), "%s/.local/share/cnav", home);    
    mkdir(mainDir, 0700);

    char path_name[64];
    strcpy(path_name, path);

    char* name = basename(path_name);

    char olddb[1024];
    char newdb[1024];
    snprintf(olddb, sizeof(olddb), "%s/.local/share/cnav/db.txt", home);
    snprintf(newdb, sizeof(newdb), "%s/.local/share/cnav/tmp.txt", home);

    FILE *myolddb = fopen(olddb, "r");
    if (myolddb == NULL)
    {
        printf("Brah");
    }
    
    FILE *mynewdb = fopen(newdb, "w");
    
    if (mynewdb == NULL) {
        perror("Error: Could not create temporary database");
        if (myolddb) fclose(myolddb);
        return;
    }

    bool found = false;

    if (myolddb != NULL) {
        char line[1024];

        while (fgets(line, sizeof(line), myolddb) != NULL) {
            char line_copy[1024];
            strcpy(line_copy, line);

            char* currLinePath = strtok(line_copy, ",");
            if (currLinePath != NULL)
            {
                if (strcmp(currLinePath, path) == 0) {
                    found = true;
                    
                    char* nameStr = strtok(NULL, ",");
                    char* programStr = strtok(NULL, ",");
                    char* callNoStr = strtok(NULL, ",");
                    char* lastCallStr = strtok(NULL, ",");

                    int currLineCallNo = atoi(callNoStr);

                    if (callNoStr != NULL && lastCallStr != NULL){
                        unsigned long long currLinelastCall = strtoull(lastCallStr, NULL, 10);
                        
                        currLineCallNo += 1;
                        currLinelastCall = (unsigned long long)time(NULL);
    
                        fprintf(mynewdb, "%s,%s,%s,%d,%llu\n", path, nameStr, programStr, currLineCallNo, currLinelastCall);
                    }
    
                } else {
                    fputs(line, mynewdb);
                }
            }
             
        }
        fclose(myolddb);
    }

    if (!found) {
        unsigned long long currTime = (unsigned long long)time(NULL);
        fprintf(mynewdb, "%s,%s,%s,1,%llu\n", path, name, program, currTime);
    }

    fclose(mynewdb);
    free(path);
    
    remove(olddb);
    rename(newdb, olddb);
}