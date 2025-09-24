#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

Hurricane CreateHurricane(int category, double latitude, double longitude) {
    Hurricane h;
    h.category = category;
    h.latitude = latitude;
    h.longitude = longitude;
    // Optionally initialize wind_speeds to 0
    for (int i = 0; i < 100; i++) h.wind_speeds[i] = 0;
    return h;
}

void FillWindSpeeds(Hurricane* hurricane) {
    int min = MIN_WIND_SPEED;
    int max;
    switch (hurricane->category) {
        case 1: max = MAX_SPEED_CAT_1; break;
        case 2: max = MAX_SPEED_CAT_2; break;
        case 3: max = MAX_SPEED_CAT_3; break;
        case 4: max = MAX_SPEED_CAT_4; break;
        case 5: max = MAX_SPEED_CAT_5; break;
        default: max = MIN_WIND_SPEED; break;
    }
    for (int i = 0; i < 100; i++) {
        hurricane->wind_speeds[i] = min + rand() % (max - min + 1);
    }
}
