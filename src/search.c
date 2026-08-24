#include "search.h"

entry search(char* wantedFile){
    char *home = getenv("HOME");
    if (home == NULL)
    {
        perror("Could not find the HOME directory");
        exit(EXIT_FAILURE);        
    }

    char dbPath[1024];
    snprintf(dbPath, sizeof(dbPath), "%s/.local/share/cnav/db.txt", home);

    FILE* mydb = fopen(dbPath, "r");
    if (mydb == NULL)
    {
        perror("Could not open the database");
        exit(EXIT_FAILURE);
    }

    entry* data = NULL;

    char line[1024];
    int i = 0;
    while (fgets(line, sizeof(line), mydb) != NULL) {
        char* linePath = strtok(line, ",");
        char* lineName = strtok(NULL, ",");
        
        if (linePath == NULL || lineName == NULL) continue;;
        

        if (!checkStrings(lineName, wantedFile)) continue;

        entry* tmpData = realloc(data, (i + 1) * sizeof(entry));
        if (tmpData == NULL)
        {
            free(data);
            perror("Error: realloc failed");
            exit(EXIT_FAILURE);
        }
        
        data = tmpData;

        char* lineProgram = strtok(NULL, ",");
        char* lineCallNo = strtok(NULL, ",");
        char* linelastCall = strtok(NULL, ",");

        if (lineCallNo == NULL || linelastCall == NULL || lineProgram == NULL) exit(EXIT_FAILURE);

        strncpy(data[i].program, lineProgram, sizeof(data[i].program) - 1);
        data[i].program[sizeof(data[i].program) - 1] = '\0';        
        strncpy(data[i].path, linePath, sizeof(data[i].path) - 1);
        data[i].path[sizeof(data[i].path) - 1] = '\0';
        strncpy(data[i].name, lineName, sizeof(data[i].name) - 1);
        data[i].name[sizeof(data[i].name) - 1] = '\0';
        
        data[i].callNo = atoi(lineCallNo);
        data[i].lastCall = strtoull(linelastCall, NULL, 10);

        i++;
    }

    entry result = {0};

    if (i > 0)
    {
        result = findMax(data, i);
    }
    

    fclose(mydb);
    free(data);
    return result;
}

bool checkStrings(const char* str1,const char* str2){
    //The program will be case insensitiv
    
    if (str1 == NULL || str2 == NULL) return false;
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2); 
    if (len1 < len2) return false;
    
    if (str2[0] == '\0') return true;

    for (size_t i = 0; i < len1; i++)
    {
        size_t j = 0;

        while (str1[i + j] != '\0' && tolower((unsigned char)str1[i+j]) == tolower((unsigned char)str2[j]))
        {
            if (str2[++j] == '\0')
            {
                return true;
            }
        }
        
    }
    return false;
}

int currLineNo(FILE* dbFile){
    int i = 0;
    char ch;
    while ((ch = fgetc(dbFile)) != EOF)
    {
        if (ch == '\n')
        {
            i++;
        }
    }
    return i;
}

double calcScore(int callNo, unsigned long long lastcall) {
    unsigned long long deltaTime = (unsigned long long)time(NULL) - lastcall;
    if (deltaTime == 0) deltaTime = 1; 
    
    double score = (double)callNo / ((double)deltaTime / 3600.0 + 1.0);
    return score;
}

entry findMax(entry* entryList, int listSize) {
    entry finalEntry = {0};
    double maxScore = -1.0; 
    double currScore = 0.0;

    for (int i = 0; i < listSize; i++) {
        currScore = calcScore(entryList[i].callNo, entryList[i].lastCall);
        if (currScore > maxScore) {
            maxScore = currScore;
            finalEntry = entryList[i];
        }
    }
    
    return finalEntry; 
}