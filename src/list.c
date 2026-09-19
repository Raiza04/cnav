#include <sqlite3.h>
#include "platform.h"

#include <stdio.h>
#include <time.h>

void list_database()
{
    char app_dir[1024];
    get_app_dir(app_dir, sizeof(app_dir));
    char mydb[2048];
    snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", app_dir);

    sqlite3 *db;
    if (sqlite3_open(mydb, &db) != SQLITE_OK)
    {
        perror("Error: failed to open the database.\n");
        return;
    }

    // Wir holen nur das Wichtigste und sortieren direkt nach dem letzten Aufruf
    const char *sql = "SELECT name, program, callNo, lastCall, path FROM history ORDER BY lastCall DESC;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        perror("Database is empty. Use cnav to fill it!\n");
        sqlite3_close(db);
        return;
    }

    printf("\n%-10s | %-10s | %-7s | %-19s | %s\n","NAME", "PROGRAM", "CALLS", "LAST CALL", "PATH");
    printf("-----------+-----------+---------+---------------------+-----------------------------------\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        const char *program = (const char *)sqlite3_column_text(stmt, 1);
        int callNo = sqlite3_column_int(stmt, 2);
        time_t rawtime = (time_t)sqlite3_column_int64(stmt, 3);
        const char *path = (const char *)sqlite3_column_text(stmt, 4);

        // Unix-Zeitstempel in lesbares Datum umwandeln
        struct tm *info = localtime(&rawtime);
        char time_buf[80];
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", info);

        printf("%-10s | %-10s | %-7d | %-19s | %s\n", name, program, callNo, time_buf, path);
        count++;
    }

    if (count == 0)
    {
        printf("(No entry found)\n");
    }
    else
    {
        printf("\nFound %d entries in total.\n\n", count);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}