#include "platform.h"
#include <sqlite3.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define W_NAME 16
#define W_PROG 8
#define W_CALLS 5
#define W_DATE 19
#define W_PATH 30

static char *compress_path(char *path);

void list_database(void) {
  int ret;
  char app_dir[1024];
  get_app_dir(app_dir, sizeof(app_dir));
  char mydb[2048];
  ret = snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", app_dir);
  if (ret < 0 || (size_t)ret >= sizeof(mydb)) {
    perror("Error: Path to the database is either too long or unvalid");
    return;
  }

  sqlite3 *db;
  if (sqlite3_open(mydb, &db) != SQLITE_OK) {
    perror("Error: failed to open the database.\n");
    return;
  }

  const char *sql = "SELECT name, program, callNo, lastCall, path FROM history "
                    "ORDER BY lastCall DESC;";
  sqlite3_stmt *stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    perror("Database is empty. Use cnav to fill it!\n");
    sqlite3_close(db);
    return;
  }

  printf("\n%-*s | %-*s | %-*s | %-*s | %-*s\n", W_NAME, "NAME", W_PROG,
         "PROGRAM", W_CALLS, "CALLS", W_DATE, "LAST CALL", W_PATH, "PATH");

  for (int i = 0; i < W_NAME + 1; i++)
    printf("-");
  printf("+");
  for (int i = 0; i < W_PROG + 2; i++)
    printf("-");
  printf("+");
  for (int i = 0; i < W_CALLS + 2; i++)
    printf("-");
  printf("+");
  for (int i = 0; i < W_DATE + 2; i++)
    printf("-");
  printf("+");
  for (int i = 0; i < W_PATH + 1; i++)
    printf("-");

  printf("\n");

  int count = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const char *name = (const char *)sqlite3_column_text(stmt, 0);
    const char *program = (const char *)sqlite3_column_text(stmt, 1);
    int callNo = sqlite3_column_int(stmt, 2);
    time_t rawtime = (time_t)sqlite3_column_int64(stmt, 3);
    char *path = (char *)sqlite3_column_text(stmt, 4);

    struct tm *info = localtime(&rawtime);
    char time_buf[80];
    size_t(ret) =
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", info);
    if (ret == 0)
      continue;

    char *currPath = compress_path(path);
    if (currPath == NULL)
      continue;

    printf("%-*s | %-*s | %*d | %-*s | %s\n", W_NAME, name, W_PROG, program,
           W_CALLS, callNo, W_DATE, time_buf, currPath);

    free(currPath);
    count++;
  }

  if (count == 0) {
    printf("(No entry found)\n");
  } else {
    printf("\nFound %d entries in total.\n\n", count);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

char *compress_path(char *path) {
  size_t path_len = strlen(path);
  size_t sep_len = strlen(PATH_SEP);

  unsigned short counter = 0;
  size_t start = 0;

  for (size_t i = path_len - sep_len; i <= path_len; i--) {
    if (strncmp(&path[i], PATH_SEP, sep_len) == 0) {
      counter++;
    }

    if (counter == 3) {
      start = i;
      break;
    }
  }

  if (counter < 3)
    return path;

  unsigned short cp_len = (unsigned short)(path_len - start);
  // +2 for .. and +1 \0
  unsigned short total_length = 2 + cp_len + 1;

  char *res = malloc(total_length);
  if (res == NULL)
    return NULL;

  int ret = snprintf(res, total_length, "..%.*s", (int)cp_len, &path[start]);
  if (ret < 0 || (size_t)ret >= total_length) {
    return NULL;
  }

  return res;
}
