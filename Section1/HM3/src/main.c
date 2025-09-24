#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "functions.h"

int main() {
  int category;
  double latitude, longitude;

  printf("Enter category, latitude, longitude: ");
  if (scanf("%d %lf %lf", &category, &latitude, &longitude) != 3) {
    printf("Invalid input.\n");
    return EXIT_FAILURE;
  }

  if (category < 1 || category > 5) {
    printf("Invalid input.\n");
    return EXIT_FAILURE;
  }

  srand(time(NULL));
  Hurricane h = CreateHurricane(category, latitude, longitude);
  FillWindSpeeds(&h);

  printf("Category: %d\n", h.category);
  printf("Coordinates: %.1f, %.1f\n", h.latitude, h.longitude);
  printf("Wind Speeds (mph): ");
  for (int i = 0; i < 100; i++) {
    printf("%d ", h.wind_speeds[i]);
  }
  printf("\n");

  return EXIT_SUCCESS;
}