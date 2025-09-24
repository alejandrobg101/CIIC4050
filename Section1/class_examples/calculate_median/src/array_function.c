#include "array_function.h" 

#include <stdio.h>
#include <stdlib.h>

// Merge two halves of an array
void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary arrays
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    // Copy data
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    // Merge the temp arrays back into arr
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    // Copy remaining elements
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

// Merge sort function
void merge_sort(int *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Recursively sort both halves
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);

        // Merge sorted halves
        merge(arr, left, mid, right);
    }
}

void sort_array(int *arr, int size) {
  merge_sort(arr, 0, size - 1);
  // for(int i = 0; i < size-1; i++) {
  //   for(int j = 0; j < size-1-i; j++) {
  //     if(arr[j] > arr[j+1]) {
  //       int temp = arr[j];
  //       arr[j] = arr[j+1];
  //       arr[j+1] = temp;
  //     }
  //   }
  // }
}


double calculate_median(int *arr, int size){
    int middle_of_array = size/2;  

  if (size % 2 == 0){
      return(arr[middle_of_array-1] + arr[middle_of_array]) / 2.0; 
  }
  else {
      return(arr[middle_of_array]) ;

  }

  
}