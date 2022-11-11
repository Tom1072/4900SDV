#include <stdio.h>
#include <assert.h>
#include "backend/includes/tests.h"
#include "backend/includes/commons.h"

int main() {
    printf("Hello, World!\n");
    char commResult  = testComm();
    // char simulatorResult = testSimulator();
    // char viewTestResult = testView();

    assert(commResult == TRUE);
    // assert(simulatorResult == TRUE);
    // assert(viewTestResult == TRUE);

    return 0;
}

