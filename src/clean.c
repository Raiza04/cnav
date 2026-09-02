#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void clean_database() {
    char *home = getenv("HOME");
    if (home == NULL) return;

    char db_path[1024];
    char tmp_path[1024];
    snprintf(db_path, sizeof(db_path), "%s/.local/share/cnav/db.txt", home);
    snprintf(tmp_path, sizeof(tmp_path), "%s/.local/share/cnav/tmp.txt", home);

    FILE *db = fopen(db_path, "r");
    if (db == NULL) {
        printf("Database not found, nothing cleaned. \n");
        return;
    }

    FILE *tmp = fopen(tmp_path, "w");
    if (tmp == NULL) {
        perror("Error with creating temporary database");
        fclose(db);
        return;
    }

    char line[1024];
    int removed_count = 0;

    while (fgets(line, sizeof(line), db) != NULL) {
        char line_copy[1024];
        strcpy(line_copy, line);

        char *path = strtok(line_copy, ",");
        
        if (path != NULL) {
            if (access(path, F_OK) == 0) {
                fputs(line, tmp);
            } else {
                removed_count++;
            }
        }
    }

    fclose(db);
    fclose(tmp);

    remove(db_path);
    rename(tmp_path, db_path);

    printf("Cleanup finished! %d entries deleted.\n", removed_count);
}