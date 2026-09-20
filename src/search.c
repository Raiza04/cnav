#include "search.h"
#include "platform.h"
#include "sqlite3.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define min3(a, b, c) min(a, min(b, c))

/**
 * @brief checks if a string is a substring of another string
 * @details It is also case-insensitive. All chars are transformed to lowercase
 *
 * @param str1 The complete name that is in the database as string
 * @param str2 The name of the wanted file
 * @return returns true if str2 is in str1 false otherwise
 */
static bool checkStrings(const char *str1, const char *str2);

/**
 * @brief calculates the frecency score
 *
 * @param callNo How many times was the file called in total (its a member of
 * the entry struct)
 * @param lastcall The last time the file was called or seached for (also member
 * of the entry struct)
 * @param dist The calculated levenshtein distance for the penalty
 * @return returns a score as double
 */
static double calcScore(int callNo, unsigned long long lastcall, int dist);

/**
 * @brief calculates the levenshtein distance with wagner-fisher algorithm
 *
 * @param str1 The first string for the wagner-fisher algo
 * @param str2 The second string for the wagner-fisher algo
 * @return It returns the absolut number of required changes (levenshtein
 * distance) as int
 */
static int levenshtein(const char *str1, const char *str2);

entry search(char *wantedFile)
{

  char cwd[1024];
  if (GETCWD(cwd, sizeof(cwd)) == NULL)
  {
    perror("Error getting current directory\n");
    cwd[0] = '\0';
  }

  entry result = {0};

  char tmp[1024];
  get_app_dir(tmp, sizeof(tmp));

  char mydb[1048]; // change to 1048 to resolve the compiler warning (snprintf
                   // truncation)
  snprintf(mydb, sizeof(mydb), "%s" PATH_SEP "cnav.db", tmp);

  sqlite3 *db;
  if (sqlite3_open(mydb, &db) != SQLITE_OK)
  {
    perror("Could not open the database for search\n");
    sqlite3_close(db);
    return result;
  }

  // Wir lassen SQL grob vorfiltern: 'LIKE' ist case-insensitive und sucht
  // Teilstrings
  const char *sql =
      "SELECT path, name, program, callNo, lastCall FROM history; ";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
  {
    perror("Tabel <history> could not be prepared for operation\n");
    sqlite3_close(db);
    return result;
  }

  double maxScore = -1.0;

  while (sqlite3_step(stmt) == SQLITE_ROW)
  {
    const char *path = (const char *)sqlite3_column_text(stmt, 0);
    const char *name = (const char *)sqlite3_column_text(stmt, 1);
    const char *program = (const char *)sqlite3_column_text(stmt, 2);
    int callNo = sqlite3_column_int(stmt, 3);
    unsigned long long lastCall =
        (unsigned long long)sqlite3_column_int64(stmt, 4);
    int dist = 0;

    int len_name = (int)strlen(name);
    int len_wanted = (int)strlen(wantedFile);

    if (len_name < len_wanted)
      continue;

    if (!checkStrings(name, wantedFile))
    {
      dist = levenshtein(wantedFile, name);

      // We want to ignore not typed chars from the dist
      // (eg. name=test.c and wanted=te => dist = 4)
      int diff = len_name - len_wanted;
      dist = dist - diff;

      if (dist > 3)
      {
        continue;
      }
    }

    double currScore = calcScore(callNo, lastCall, dist);

    size_t cwd_len = strlen(cwd);

    if (strcmp(path, cwd) == 0)
    {
      // checkign the case (path=~/docs/project/ and cwd=~/docs/proj/) so we are
      // not in the directory we check the next char in path at index cwd_len
      if (path[cwd_len] == '/' || path[cwd_len] == '\\' ||
          path[cwd_len] == '\0')
      {
        currScore *= 2.0;
      }
    }

    if (currScore > maxScore)
    {
      maxScore = currScore;

      // finalEntry überschreiben (ersetzt dein altes findMax)
      strncpy(result.path, path, sizeof(result.path) - 1);
      result.path[sizeof(result.path) - 1] = '\0';

      strncpy(result.name, name, sizeof(result.name) - 1);
      result.name[sizeof(result.name) - 1] = '\0';

      strncpy(result.program, program, sizeof(result.program) - 1);
      result.program[sizeof(result.program) - 1] = '\0';

      result.callNo = callNo;
      result.lastCall = lastCall;
    }
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return result;
}

double calcScore(int callNo, unsigned long long lastcall, int dist)
{
  unsigned long long deltaTime = (unsigned long long)time(NULL) - lastcall;
  if (deltaTime == 0)
    deltaTime = 1;

  double penalty = (dist + 1.0) * (dist + 1.0);
  double score = (double)callNo / ((double)deltaTime / 3600.0 + 1.0);
  return score / penalty;
}

bool checkStrings(const char *str1, const char *str2)
{
  // The program will be case insensitiv

  if (str1 == NULL || str2 == NULL)
    return false;

  size_t len1 = strlen(str1);
  size_t len2 = strlen(str2);
  if (len1 < len2)
    return false;

  if (str2[0] == '\0')
    return true;

  for (size_t i = 0; i < len1; i++)
  {
    size_t j = 0;

    while (str1[i + j] != '\0' && tolower((unsigned char)str1[i + j]) ==
                                      tolower((unsigned char)str2[j]))
    {
      if (str2[++j] == '\0')
      {
        return true;
      }
    }
  }
  return false;
}

int levenshtein(const char *str1, const char *str2)
{
  int n = (int)strlen(str1);
  int m = (int)strlen(str2);
  int arr[m + 1][n + 1];

  arr[0][0] = 0;

  for (int i = 1; i <= m; i++)
  {
    arr[i][0] = i;
  }

  for (int j = 1; j <= n; j++)
  {
    arr[0][j] = j;
  }

  for (int i = 1; i <= m; i++)
  {
    for (int j = 1; j <= n; j++)
    {
      int cost = (tolower(str1[j - 1]) == tolower(str2[i - 1])) ? 0 : 1;

      int upleft = arr[i - 1][j - 1];
      int up = arr[i - 1][j];
      int left = arr[i][j - 1];

      arr[i][j] = min3(up + 1, left + 1, upleft + cost);
    }
  }
  return arr[m][n];
}
