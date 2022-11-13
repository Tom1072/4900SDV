#include <stdio.h>
#include <assert.h>
#include "backend/includes/tests.h"
#include "backend/includes/commons.h"

int main()
{
  printf("SDV\n");

  // char comm_result  = test_comm();
  // char simulator_result = testSimulator();
  char view_result = testView();

  // assert(comm_result);
  // assert(simulator_result);
  assert(view_result);

  return 0;
}