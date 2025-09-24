#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "functions.h"

int main() {
  int n;
  if (scanf("%d", &n) != 1 || n < 10 || n > 50) {
    fprintf(stderr,
        "Error: Invalid input.  Please enter an integer between 10 and 50.\n");
    return EXIT_FAILURE;
  }

  int *arr = (int*)malloc(n * sizeof(int));
  if (arr == NULL) {
    fprintf(stderr, "Error: Memory allocation failed.\n");
    return EXIT_FAILURE;
  }

  FillArray(arr, n);
  long sum = SumArray(arr, n);
  printf("%ld\n", sum);

  free(arr);
  return EXIT_SUCCESS;
}