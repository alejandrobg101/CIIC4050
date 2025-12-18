#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_HORSES 5
#define TRACK_LENGTH 50

int track[NUM_HORSES] = {0};
int finish_order[NUM_HORSES] = {0};
int finish_count = 0;

void* horse_run(void* arg) {
  int id = *((int*)arg);
  free(arg);
  while (track[id] < TRACK_LENGTH) {
    int step = rand() % 4;
    track[id] += step;
    if (track[id] > TRACK_LENGTH) track[id] = TRACK_LENGTH;
    printf("Horse %d moves to %d\n", id, track[id]);
    fflush(stdout);
    if (track[id] >= TRACK_LENGTH) {
      finish_order[finish_count] = id;
      finish_count++;
      printf("Horse %d finished the race!\n", id);
      fflush(stdout);
      break;
    }
    usleep(10000);
  }
  return NULL;
  }

  int main() {
  pthread_t horses[NUM_HORSES];
  srand(time(NULL));
  for (int i = 0; i < NUM_HORSES; i++) {
    int* id = malloc(sizeof(int));
    *id = i;
    pthread_create(&horses[i], NULL, horse_run, id);
  }
  for (int i = 0; i < NUM_HORSES; i++) {
    pthread_join(horses[i], NULL);
  }
  printf("\n--- Final results ---\n");
  for (int i = 0; i < NUM_HORSES; i++) {
    printf("Place %d: Horse %d\n", i+1, finish_order[i]);
  }
  return 0;
  }