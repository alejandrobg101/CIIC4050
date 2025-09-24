#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "functions.h"

void FillArray(int* arr, int size) {
  for (int i = 0; i < size; i++) {
    arr[i] = i + 1;
  }
}

int SumArray(int* arr, int size) {
  int sum = 0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum;
}