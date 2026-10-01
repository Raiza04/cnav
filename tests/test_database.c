#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../inc/add.h"
#include "../inc/search.h"

void purge(void);
void clean_database(void);
void delete_entry(char *line);
void init();

void get_app_dir(char *path, size_t max_size) {
  (void)snprintf(path, max_size, ".");
}

int main(void) {
  // ==========================================
  // SETUP: create new environment
  // ==========================================
  // Make sure cnav really is created new
  (void)remove("cnav.db");

  // ==========================================
  // TEST-BLOCK 1: initilize
  // ==========================================
  init();
  FILE *f = fopen("tools.txt", "r");
  assert(f != NULL && "init() should create tools.txt");
  if (f)
    (void)fclose(f);
  (void)remove("tools.txt");

  // ==========================================
  // TEST-BLOCK 2: adding files to database
  // ==========================================
  add("file.c", (Prog){"vim"});
  add("file.c", (Prog){"vim"});
  add("test.c", (Prog){"vim"});

  // ==========================================
  // TEST-BLOCK 3: searching files from database
  // ==========================================
  entry res = search("fil");
  assert(strcmp(res.name, "file.c") == 0 &&
         "Search should find the added file");
  assert(res.callNo == 2 && "CallNo should be 2 after two additions");

  // ==========================================
  // TEST-BLOCK 4:Purge && Clean
  // ==========================================
  (void)ungetc('\n', stdin);
  (void)ungetc('y', stdin);

  delete_entry("fil");
  res = search("file.c");
  assert(strcmp(res.name, "file.c") != 0 &&
         "The file should not be in database after delete");

  purge();
  res = search("");
  assert(res.name[0] == '\0' &&
         "Purge should resets the whole database with no entry left in it");

  // ==========================================
  // TEST-BLOCK 5: clean_database
  // ==========================================
  add("non-existing.txt", (Prog){"vim"});
  clean_database();
  res = search("non-existing.txt");
  assert(res.path[0] == '\0' &&
         "clean_database should remove non-existing files");

  // ==========================================
  // TEARDOWN: Cleanup
  // ==========================================
  (void)remove("cnav.db");

  return 0;
}
