#include <assert.h>
#include <stdbool.h>

#include "../src/search.c" //NOLINT

int main(void) {
  // ==========================================
  // Tests for Levenshtein-distance
  // ==========================================

  // Test 1: Identical strings
  assert(levenshtein("main.c", "main.c") == 0 &&
         "Identical strings should have a distance of 0");

  // Test 2: Single typo (Distance 1)
  assert(levenshtein("main.c", "maan.c") == 1 &&
         "A single typo should result in a distance of exactly 1");

  // Test 3: Completely different strings
  assert(levenshtein("tools.txt", "readme.md") > 5 &&
         "Completely different words should return a high distance score");

  // ==========================================
  // Tests for checkStrings (match check)
  // ==========================================

  // Test 1: Exact match
  assert(checkStrings("main.c", "main.c") &&
         "Exact file name matches should be evaluated as valid");

  // Test 2: Prefix-match (User enters only one part of the string)
  assert(checkStrings("main.c", "ma") &&
         "Partial prefix searches must be evaluated as valid matches");

  // Test 3:Searching word is longer than the actual file-name
  assert(!checkStrings("ma", "main.c") &&
         "Search input longer than target file cannot be a match");

  // Test 4: No match at all
  assert(!checkStrings("main.c", "test.c") &&
         "If completely different strings then there is no match");

  // ==========================================
  // Tests for Score calculation
  // ==========================================

  // Test 1: Faktor Time:
  assert(calcScore((CallNo){1}, (Distance){0}, (LastCall){1}) <
             calcScore((CallNo){1}, (Distance){0}, (LastCall){2}) &&
         "Newer files should have a higher score than older files");

  // Test 2: Faktor the number of called times
  assert(calcScore((CallNo){1}, (Distance){0}, (LastCall){1}) <
             calcScore((CallNo){2}, (Distance){0}, (LastCall){1}) &&
         "Files that are called more often shoulg have higher score than those "
         "that are called less");

  // Test 3: Faktor Penalty application
  assert(calcScore((CallNo){1}, (Distance){1}, (LastCall){1}) <
             calcScore((CallNo){1}, (Distance){0}, (LastCall){1}) &&
         "Files that have higher distance (levenshtein) Should have a higher "
         "score than those with a smaller distance");

  return 0;
}
