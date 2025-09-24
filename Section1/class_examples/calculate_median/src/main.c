#include "array_function.h"

#include <stdio.h>
#include <stdlib.h>


int main() {
  int size_of_array;
  printf("How many numbers do you want to add? \n");
  scanf("%d", &size_of_array);

  int arr[size_of_array];
  for(int i = 0; i < size_of_array; i++){
    printf("Add a number %d: ", i+1);
    scanf("%d", &arr[i]);
  }
  sort_array(arr, size_of_array);
  double median = calculate_median(arr, size_of_array);

  printf("Median is: %.2f\n", median);

  return EXIT_SUCCESS;
}
