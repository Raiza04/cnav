#pragma once

// we are declaring this struct to solve the argument scrambling
// so that we dont mistakenly swap path and program as argument
#include <string.h>
typedef struct {
  char s[64];
} Prog;

inline Prog prog_init(char *program) {
  Prog p;
  strncpy(p.s, program, sizeof(p.s) - 1);
  p.s[sizeof(p.s) - 1] = '\0';
  return p;
}

/**
 * @brief It adds new entries to the database. If already there the callno is
 * incremented
 *
 * @param tmp_path path to the file that is to be added to the database
 * @param program the program with which the file was opend
 */
void add(char *tmp_path, Prog program);
