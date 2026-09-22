#include "platform.h"

#include <sqlite3.h>
#include <stdio.h>

void clean_database(void) {
  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  int ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);
  if (ret < 0) {
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
