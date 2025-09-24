#include "functions.h"

#define PI 3.14159

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double CalculateCircleArea(double radius) {
  return PI * radius * radius;
}
double CalculateCirclePerimeter(double radius) {
  return 2 * PI * radius;
}
double CalculateSphereVolume(double radius) {
  return (4.0 / 3.0) * PI * pow(radius, 3);
}