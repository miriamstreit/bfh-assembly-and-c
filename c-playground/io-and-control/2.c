#include <stdio.h>

int main(void) {
    char x[30] = {0};
    char y[30] = {0};

    scanf("%29[^\n]%*c", x);
    scanf("%29[^\n]", y);

    fprintf(stdout, "%s, %s\n", x, y);
    return 0;
}