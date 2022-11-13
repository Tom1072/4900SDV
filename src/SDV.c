#include <stdio.h>
#include "backend/includes/tests.h"

int main() {
    printf("Hello, World!\n");
    // char result  = testComm();
     char result2 = testSimulator();
//     char result3 = testActuators();
     printf("Result: %d", result2);


    return 0;
}
