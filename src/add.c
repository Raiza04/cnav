#include "add.h"
#include "platform.h"

#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
    We want to add lines to the database in this form
    example:
    ~/Download/test.txt,test.txt,vim,<how many times was this exact line
   called>,<last call this line was called>
*/

static inline char *cnav_dirname(char *path);
static inline char *cnav_basename(char *path);

void add(char *tmp_path, Prog program) {
  char *path = realpath(tmp_path, NULL);
  int ret; // return value for sprintf

  if (path == NULL) {

    char *dir_path = STRDUP(tmp_path);
    char *base_name_str = STRDUP(tmp_path);

    char *dir = cnav_dirname(dir_path);
    char *base = cnav_basename(base_name_str);

    char *resolved_dir = realpath(dir, NULL);

    if (resolved_dir != NULL) {
      path = malloc(strlen(resolved_dir) + strlen(base) + 2);
      if (path != NULL) {
        size_t dir_len = strlen(resolved_dir);

        if (dir_len > 0 && (resolved_dir[dir_len - 1] == '/' ||
                            resolved_dir[dir_len - 1] == '\\')) {
          ret = sprintf(path, "%s%s", resolved_dir, base);
        } else {
          ret = sprintf(path, "%s" PATH_SEP "%s", resolved_dir, base);
        }

        if (ret < 0) {
          perror("Failed to resolve the provided path to add");
          return;
        }
      }
      free(resolved_dir);
    }

    free(dir_path);
    free(base_name_str);
  }

  if (path == NULL) {
    perror("Problem with the provided path");
    exit(EXIT_FAILURE);
  }

  char *path_copy = STRDUP(path);
  char *name = cnav_basename(path_copy);

  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048];
  ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);
  if (ret < 0) {
    perror("Could not find the database");
    return;
  }

  sqlite3 *db;

  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
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

  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    perror("Error while creating the table\n");
    sqlite3_close(db);
    return;
  }

  const char *upsert =
      "INSERT INTO history (path, name, program, callNo, lastCall) "
      "VALUES (?, ?, ?, 1, ?) "
      "ON CONFLICT(path, program) "
      "DO UPDATE SET callNo = callNo + 1, lastCall = excluded.lastCall;";

  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, upsert, -1, &stmt, NULL) != SQLITE_OK) {
    perror("Error with preparing the upsert\n");
    sqlite3_close(db);
    return;
  }

  long long currTime = time(NULL);

  sqlite3_bind_text(stmt, 1, path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, program.s, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int64(stmt, 4, currTime);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    perror("Error with executing the upsert\n");
  }

  sqlite3_finalize(stmt);

  sqlite3_close(db);
  free(path_copy);
  free(path);
}

/**
 * @brief Cross-platform alternative to POSIX basename.
 * @details Finds and returns a pointer to the filename component of a path.
 * This function does not modify the original string.
 *
 * @param path The path string to parse.
 * @return Pointer to the base name, or "." if the path is empty or invalid.
 */
static inline char *cnav_basename(char *path) {
  if (path == NULL || *path == '\0')
    return ".";

  char *base = path;
  for (char *p = path; *p != '\0'; p++) {
    if (*p == '/' || *p == '\\') {
      base = p + 1;
    }
  }

  // Handle edge case where path ends with a separator (e.g., "dir/")
  if (*base == '\0')
    return ".";

  return base;
}

/**
 * @brief Cross-platform alternative to POSIX dirname.
 * @details Returns the directory component of a path.
 * WARNING: This function modifies the input string by replacing the last
 * separator with a null terminator. Pass a modifiable copy (e.g., via strdup).
 *
 * @param path The path string to parse.
 * @return Pointer to the directory name string.
 */
static inline char *cnav_dirname(char *path) {
  if (path == NULL || *path == '\0')
    return ".";

  char *last_slash = NULL;
  for (char *p = path; *p != '\0'; p++) {
    if (*p == '/' || *p == '\\') {
      last_slash = p;
    }
  }

  // No directory separator found -> file is in current directory
  if (last_slash == NULL)
    return ".";

  // Edge case: file is in the root directory (e.g., "/test.txt" or
  // "C:\test.txt")
  if (last_slash == path || (last_slash == path + 2 && path[1] == ':')) {
    *(last_slash + 1) = '\0';
    return path;
  }

  *last_slash = '\0';
  return path;
}
