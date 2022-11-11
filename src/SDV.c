#include <stdio.h>
#include <assert.h>
#include "backend/includes/tests.h"
#include "backend/includes/commons.h"

int main() {
    printf("Hello, World!\n");
    char result  = testComm();
    char result2 = testSimulator();
    char viewTestResult = testView();
    printf("Result: %d", result);

    assert(result == TRUE);
    assert(result2 == TRUE);
    assert(viewTestResult == TRUE);

    return 0;
}

