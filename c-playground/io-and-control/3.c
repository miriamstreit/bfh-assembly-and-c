#include <stdio.h>

int main(void) {
    char input[] = "1a";
    unsigned int x = 0;

    if(1 == sscanf(input, "%u", &x))
        fprintf(stdout, "Got a number %u\n", x);
    return 0;
}