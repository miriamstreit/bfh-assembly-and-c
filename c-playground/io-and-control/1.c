#include <stdio.h>

#define TYPE char

int  main (void) {
    int ret = getchar();
    TYPE c = (TYPE) ret;
    fprintf(stdout, "%d\n", (EOF == c));
    return 0;
}