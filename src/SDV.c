#include <stdio.h>
#include <assert.h>
#include "backend/includes/tests.h"
#include "backend/includes/commons.h"

int main() {
    printf("SDV\n");

    // char commResult  = testComm();
    char simulatorResult = testSimulator();
    char viewTestResult = testView();

    // assert(commResult);
    assert(simulatorResult);
    assert(viewTestResult);


    return 0;
}