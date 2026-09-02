#include "main.h"

void init(void);
void clean_database(void);

char *pathfinder(char* arr[], int arrSize){
    for (int i = 3; i < arrSize; i++)
    {
        if (arr[i][0] != '-')
        {
            return arr[i];
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    // Search mode: n <file to search>
    if (argc == 2) {
        if (strcmp(argv[1], "--init") == 0) {
            init();
            return EXIT_SUCCESS;
        }

        if (strcmp(argv[1], "--clean") == 0) {
            clean_database();
            return EXIT_SUCCESS;
        }

        entry result = search(argv[1]);
        
        if (result.path[0] == '\0') {
            printf("No match found for '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }

        add(result.path, result.program);

        if (execlp(result.program, result.program, result.path, (char *)NULL) == -1) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

    // Add mode: n --add <program> <path>
    } else if (argc > 2) {
        if (strcmp(argv[1], "--add") == 0) {
            char* path = pathfinder(argv, argc);
            if (path != NULL)
            {
                add(path, argv[2]);
            }

        } else {
            printf("INVALID OPTION. Use: n --add <program> <path>\n");
            return EXIT_FAILURE;
        }

    }

    return EXIT_SUCCESS;
}
