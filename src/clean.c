#include "main.h"
#include "platform.h"
#include "search.h"

#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

void clean_database(void) {
  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  int ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);
  if (ret < 0 || (size_t)ret >= sizeof(mydb)) {
    perror("Failed to resolve the path to the database to clean\n");
  }

  sqlite3 *db;
  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
    perror("Database not found or could not be opened.\n");
    return;
  }

  const char *sql_select = "SELECT DISTINCT path FROM history;";
  sqlite3_stmt *stmt_select;

  const char *sql_delete = "DELETE FROM history WHERE path = ?;";
  sqlite3_stmt *stmt_delete;

  if (sqlite3_prepare_v2(db, sql_select, -1, &stmt_select, NULL) != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt_delete, NULL) != SQLITE_OK) {
    sqlite3_finalize(stmt_select);
    sqlite3_close(db);
    return;
  }

  int removed_count = 0;

  while (sqlite3_step(stmt_select) == SQLITE_ROW) {
    const char *path = (const char *)sqlite3_column_text(stmt_select, 0);

    if (!FILE_EXISTS(path)) {
      sqlite3_bind_text(stmt_delete, 1, path, -1, SQLITE_TRANSIENT);

      sqlite3_step(stmt_delete);

      sqlite3_reset(stmt_delete);

      removed_count++;
    }
  }

  sqlite3_finalize(stmt_select);
  sqlite3_finalize(stmt_delete);
  sqlite3_close(db);

  printf("Cleanup finished! %d entries deleted.\n", removed_count);
}

void purge(void) {
  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  int ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);
  if (ret < 0 || (size_t)ret >= sizeof(mydb)) {
    (void)fprintf(stderr,
                  "Error: Failed to resolve the path to the database\n");
    return;
  }

  sqlite3 *db;

  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
    (void)fprintf(stderr, "Error: Failed to open the database\n");
    sqlite3_close(db);
    return;
  }

  const char *sql_purge = "DELETE FROM history; VACUUM;";
  char *errmsg = NULL;

  if (sqlite3_exec(db, sql_purge, NULL, NULL, &errmsg) != SQLITE_OK) {
    (void)fprintf(stderr, "Error: Failed to execute purge: %s\n", errmsg);
    sqlite3_free(errmsg);
  } else {
    printf("Database successfully emptied\n");
  }

  sqlite3_close(db);
}

void delete_entry(char *line) {
  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  int ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);

  if (ret < 0 || (size_t)ret >= sizeof(mydb)) {
    (void)fprintf(stderr,
                  "Error: Failed to resolve the path to the database\n");
    return;
  }

  entry match = search(line);
  if (match.path[0] == '\0') {
    printf("No match found for %s\n", line);
    return;
  }

  printf("You are about to delete %s\n"
         "Do you want to continue? (y) (n)\n",
         match.path);

  int choice = getchar();

  if (choice != 'y' && choice != 'Y') {
    printf("Operation canceled\n"
           "Note: If " ANSI_COLOR_RED "%s" ANSI_COLOR_RESET
           " is not the one you want to delete try to enter the "
           "whole name or path instead of %s\n",
           match.path, line);
    return;
  }

  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;

  sqlite3 *db;

  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
    (void)fprintf(stderr, "Error: Failed to open the database. %s\n",
                  sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  const char *sql_delete = "DELETE FROM history WHERE path = ?; ";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql_delete, -1, &stmt, NULL) != SQLITE_OK) {
    (void)fprintf(stderr, "Error: Failed to prepare the operation %s\n",
                  sqlite3_errmsg(db));
    sqlite3_close(db);
    return;
  }

  if (sqlite3_bind_text(stmt, 1, match.path, -1, SQLITE_STATIC) != SQLITE_OK) {
    (void)fprintf(stderr, "Error: Failed to bind path: %s\n",
                  sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    (void)fprintf(stderr, "Error: Failed to delete entry: %s\n",
                  sqlite3_errmsg(db));
  } else {
    printf(ANSI_COLOR_GREEN "Successfully deleted: %s\n" ANSI_COLOR_RESET,
           match.path);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

void comp(char *wanted) {
  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  int ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);
  if (ret < 0 || (size_t)ret >= sizeof(mydb))
    return;

  sqlite3 *db;
  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  const char *sql_comp = "SELECT name FROM history WHERE name LIKE ?";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql_comp, -1, &stmt, NULL) != SQLITE_OK) {
    sqlite3_close(db);
    return;
  }

  char search_pattern[512];
  (void)snprintf(search_pattern, sizeof(search_pattern), "%s%%", wanted);

  if (sqlite3_bind_text(stmt, 1, search_pattern, -1, SQLITE_TRANSIENT) !=
      SQLITE_OK) {
    sqlite3_finalize(stmt); // Hier fehlte das finalize
    sqlite3_close(db);
    return;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    // FEHLER 5 BEHOBEN: Spalte 0 abfragen
    const char *name = (const char *)sqlite3_column_text(stmt, 0);
    printf("%s\n", name);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}
