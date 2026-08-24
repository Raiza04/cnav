#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct
{
    char path[1024];
    char name[256];
    char program[32];
    int callNo;
    unsigned long long lastCall;
} entry;

entry search(char* wantedFile);
bool checkStrings(const char* str1,const char* str2);
int currLineNo(FILE* dbFile);
entry findMax(entry* entryList, int listSize);
double calcScore(int callNo, unsigned long long lastcall);