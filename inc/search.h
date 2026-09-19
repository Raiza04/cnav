#pragma once

typedef struct
{
    char path[1024];
    char name[256];
    char program[32];
    int callNo;
    unsigned long long lastCall;
} entry;

/**
 * @brief Searchs the data base based on a given file name
 * @details Uses a modified levenshtein-algorithm combined with frecency-scoring
 * 
 * @param wantedFile The name of the file that the function should seach for
 * @return It returns a entry with information about the seaching file
 */
entry search(char *wantedFile);