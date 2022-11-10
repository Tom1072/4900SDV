#include <stdio.h>
#include "backend/includes/tests.h"

int main() {
    printf("Hello, World!\n");
    char result  = testComm();
    char result2 = testSimulator();
    printf("Result: %d", result);

    return 0;
}

