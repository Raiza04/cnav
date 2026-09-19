#include "add.h"
#include "search.h"
#include <sqlite3.h>
#include "platform.h"

/*
    We want to add lines to the database in this form
    example:
    ~/Download/test.txt,test.txt,vim,<how many times was this exact line called>,<last call this line was called>
*/

void add(char *tmp_path, char *program)
{
    char *path = realpath(tmp_path, NULL);

    if (path == NULL)
    {

        char *dir_path = strdup(tmp_path);
        char *base_name_str = strdup(tmp_path);

        char *dir = dirname(dir_path);
        char *base = basename(base_name_str);

        char *resolved_dir = realpath(dir, NULL);

        if (resolved_dir != NULL)
        {
            path = malloc(strlen(resolved_dir) + strlen(base) + 2);
            if (path != NULL)
            {
                size_t dir_len = strlen(resolved_dir);

                if (dir_len > 0 && (resolved_dir[dir_len - 1] == '/' || resolved_dir[dir_len - 1] == '\\'))
                {
                    sprintf(path, "%s%s", resolved_dir, base);
                }
                else
                {
                    sprintf(path, "%s" PATH_SEP "%s", resolved_dir, base);
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

    char *path_copy = strdup(path);
    char *name = basename(path_copy);

    char tmp[1024];
    get_app_dir(tmp, sizeof(tmp));

    char mydb[1048];
    snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);

    sqlite3 *db;

    if (sqlite3_open(mydb, &db) != SQLITE_OK)
    {
        perror("Error while opening the database to add\n");
        sqlite3_close(db);
        return;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS history("
                      "path TEXT NOT NULL,"
                      "name TEXT NOT NULL,"
                      "program TEXT NOT NULL,"
                      "callNo INTEGER NOT NULL,"
                      "lastCall INTEGER NOT NULL,"
                      "PRIMARY KEY(path, program));";

    if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK)
    {
        perror("Error while creating the table\n");
        sqlite3_close(db);
        return;
    }

    const char *upsert = "INSERT INTO history (path, name, program, callNo, lastCall) "
                         "VALUES (?, ?, ?, 1, ?) "
                         "ON CONFLICT(path, program) "
                         "DO UPDATE SET callNo = callNo + 1, lastCall = excluded.lastCall;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, upsert, -1, &stmt, NULL) != SQLITE_OK)
    {
        perror("Error with preparing the upsert\n");
        sqlite3_close(db);
        return;
    }

    unsigned long long currTime = (unsigned long long)time(NULL);

    sqlite3_bind_text(stmt, 1, path, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, program, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, currTime);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        perror("Error with executing the upsert\n");
    }

    sqlite3_finalize(stmt);

    sqlite3_close(db);
    free(path_copy);
    free(path);
}