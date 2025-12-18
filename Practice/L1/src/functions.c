#include "functions.h"

#include <stdio.h>
#include <stdlib.h>

void Merge(int *arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;


  int *L = (int *)malloc(n1 * sizeof(int));
  int *R = (int *)malloc(n2 * sizeof(int));


  for (int i = 0; i < n1; i++)
    L[i] = arr[left + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j];


  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k++] = L[i++];
    } else {
      arr[k++] = R[j++];
    }
  }


  while (i < n1) arr[k++] = L[i++];
  while (j < n2) arr[k++] = R[j++];


  free(L);
  free(R);
}


void MergeSort(int *arr, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    MergeSort(arr, left, mid);
    MergeSort(arr, mid + 1, right);

    Merge(arr, left, mid, right);
  }
}


void SortArray(int *arr, int size) {
  MergeSort(arr, 0, size - 1);
}




double CalculateMedian(int *arr, int size) {
  int middle_of_array = size/2;

  if (size % 2 == 0) {
    return(arr[middle_of_array-1] + arr[middle_of_array]) / 2.0;
  } else {
    return(arr[middle_of_array]);
  }
}

double CalculateMean(int *arr, int size) {
  int i = 0;
  double media = 0;
  while (i < size) {
    media += arr[i];
    i++;
  }
  return(media/size);
}