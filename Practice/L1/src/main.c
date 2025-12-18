#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  int size_of_array;
  fprintf(stderr, "How many numbers do you want to add? \n");
  scanf("%d", &size_of_array);

  int *numbers = malloc(size_of_array * sizeof(int));
  if (numbers == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return EXIT_FAILURE;
  }
  fprintf(stderr, "Enter %d numbers:\n", size_of_array);
  for (int i = 0; i < size_of_array; i++) {
    scanf("%d", &numbers[i]);
  }

  SortArray(numbers, size_of_array);

  double median = CalculateMedian(numbers, size_of_array);
  printf("Median: %.2f\n", median);

  double mean = CalculateMean(numbers, size_of_array);
  printf("Mean: %.2f\n", mean);

  return EXIT_SUCCESS;
}