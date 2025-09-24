
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "functions.h"

int main() {
  char in_array[100];

  printf("type a text: \n");

  fgets(in_array, sizeof(in_array), stdin);

  ModifyArray(in_array);

  printf("You typed: %s\n", in_array);



  return EXIT_SUCCESS;
}

