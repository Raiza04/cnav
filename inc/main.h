#pragma once

#include "add.h"
#include "platform.h"
#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add(char *tmp_path, Prog program);
void init(void);
void clean_database(void);
void list_database(void);
void purge(void);

Prog prog_init(char *program);

typedef struct {
  char *program;
  char *path;
  char *delete_target;
  char *query;
} Flags;

typedef int (*CommandFunc)(Flags *);

static inline int run_list(Flags *flag __attribute__((unused))) {
  list_database();
  return EXIT_SUCCESS;
}

static inline int run_init(Flags *flag __attribute__((unused))) {
  init();
  return EXIT_SUCCESS;
}

static inline int run_clean(Flags *flag __attribute__((unused))) {
  clean_database();
  return EXIT_SUCCESS;
}

static inline int run_purge(Flags *flag __attribute__((unused))) {
  purge();
  return EXIT_SUCCESS;
}

static inline int run_delete(Flags *flag __attribute__((unused))) {
  printf("This flag is under development\n");
  return EXIT_FAILURE;
}

static inline int run_add(Flags *flag) {
  add(flag->path, prog_init(flag->program));
  return EXIT_SUCCESS;
}

static inline int run_search(Flags *flag) {
  entry result = search(flag->query);

  if (result.path[0] == '\0') {
    printf("No match found for %s\n", flag->query);
    return EXIT_FAILURE;
  }

  add(flag->path, prog_init(flag->program));

  if (EXEC_PROG(result.program, result.program, result.path, (char *)NULL) ==
      -1) {
    printf("Error opening the file\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

static inline int parse_and_run(int argc, char *argv[]) {
  Flags opts = {0};
  CommandFunc active = NULL;

  struct {
    const char *flag;
    CommandFunc func;
  } dispatch_table[] = {{"--list", run_list},
                        {"--clean", run_clean},
                        {"--init", run_init},
                        {"--purge", run_purge}};

  int numCommand = sizeof(dispatch_table) / sizeof(dispatch_table[0]);

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--add") == 0) {

      if (active != NULL) {
        (void)fprintf(stderr,
                      "Error: '--add' conflicts with another action.\n");
        return EXIT_FAILURE;
      }

      active = run_add;
      if (i + 2 < argc) {
        opts.program = argv[++i];
        opts.path = argv[++i];
      } else {
        (void)fprintf(
            stderr,
            "Error: '--add' needs to be called with a program and a file\n");
        return EXIT_FAILURE;
      }
    } else if (strcmp(argv[i], "-d") == 0) {

      if (active != NULL) {
        (void)fprintf(stderr, "Error: '-d' conflicts with another action.\n");
        return EXIT_FAILURE;
      }

      active = run_delete;

      if (i + 1 < argc) {
        opts.delete_target = argv[++i];
      } else {
        (void)fprintf(stderr, "Error: '-d' need an entry name to delete\n");
        return EXIT_FAILURE;
      }
    } else if (argv[i][0] == '-') {
      int found = 0;
      for (int j = 0; j < numCommand; j++) {
        if (strcmp(argv[i], dispatch_table[j].flag) == 0) {
          if (active != NULL) {
            (void)fprintf(stderr, "Error: '%s' conflicts with another action\n",
                          argv[i]);
            return EXIT_FAILURE;
          }

          active = dispatch_table[j].func;
          found = 1;
          break;
        }
      }

      if (!found) {
        (void)fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
        return EXIT_FAILURE;
      }
    } else {
      opts.query = argv[i];
      if (!active) {
        active = run_search;
      };
    }
  }

  if (!active && argc == 1) {
    (void)fprintf(stderr, "Usage: n [option] or <file to open>\n");
    return EXIT_FAILURE;
  }

  return active(&opts);
}
