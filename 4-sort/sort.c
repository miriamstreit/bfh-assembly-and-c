#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    bool reverse = false;

    if (argc == 1 || argc == 2) {
        if (argc == 2) {
            if (strcmp(argv[1], "-r") == 0) reverse = true;
            else {
                puts("Arguments invalid: use none or -r for a reversed sort");
                return EXIT_FAILURE;
            }
        }
    } else {
        puts("Arguments invalid: use none or -r for a reversed sort");
        return EXIT_FAILURE;
    }

    // char **strings = calloc(1, sizeof(char*));
    // if (strings == NULL) {
    //     return EXIT_FAILURE;
    // }

    char strings[50][64];

    int numberOfStrings = 0;
    while (1 == scanf("%s", strings[numberOfStrings])) {
        numberOfStrings++;
        // strings = realloc(strings, (numberOfStrings+1) * sizeof(char*));
        // if (strings == NULL) {
        //     return EXIT_FAILURE;
        // }
    }

    if (numberOfStrings == 0) {
        puts("no strings to be sorted");
        // free(strings);
        return EXIT_SUCCESS;
    }

    // @todo test if this works
    // declare loop variables
    int i, j;

    // bubble sort
    for (i = 0; i < numberOfStrings-1; i++) {
        for (j = 0; j < numberOfStrings-i-1; j++) {
            // create pointers for array positions
            char *currentString = strings[j];
            char *nextString = strings[j+1];

            // use string compare for sort
            // if nextString < currentString, the positions need to be swapped
            if (strcmp(currentString, nextString) > 0) {
                // define a temp variable for swapping
                char *temp = calloc(1, sizeof(char));
                if (temp == NULL) {
                    return 1;
                }
                strcpy(temp, currentString);
                strcpy(strings[j], nextString);
                strcpy(strings[j + 1], temp);

                free(temp);
            }
        }
        
    }
    
    int k;
    if (reverse) {
        // print all of the strings descending
        for (k = numberOfStrings-1; k >= 0; k--) {
            puts(strings[k]);
        }
    } else {
        // print all of the strings ascending
        for (k = 0; k < numberOfStrings; k++) {
            puts(strings[k]);
        }
    }

    // free(strings);
    return EXIT_SUCCESS;
}