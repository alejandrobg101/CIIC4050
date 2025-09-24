#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: Incorrect number of arguments\n");
        return EXIT_FAILURE;
    }

    const char *command = argv[1];
    char *endptr;
    double radius = strtod(argv[2], &endptr);
    if (*endptr != '\0' || radius < 0) {
        fprintf(stderr, "Error: Invalid radius\n");
        return EXIT_FAILURE;
    }

    if (strcmp(command, "circle_area") == 0) {
        printf("%.6f\n", CalculateCircleArea(radius));
    } else if (strcmp(command, "circle_perimeter") == 0) {
        printf("%.6f\n", CalculateCirclePerimeter(radius));
    } else if (strcmp(command, "sphere_volume") == 0) {
        printf("%.6f\n", CalculateSphereVolume(radius));
    } else {
        fprintf(stderr, "Error: Invalid command\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}