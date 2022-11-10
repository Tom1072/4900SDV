#include <stdio.h>
#include "backend/includes/tests.h"

int main() {
    printf("Hello, World!\n");
    // char result = testComm();
    char result = testActuators();
    printf("Result: %d", result);

    return 0;
}