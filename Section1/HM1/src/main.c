#include <stdio.h>
#include <stdlib.h>

#include "functions.h"

int main() {
  int i, joke_num, answer;

  srand(time(NULL));

  for (i = 0; i < 3; i++) {
    joke_num = (rand() % 5) + 1;

    TellJoke(joke_num);

    while (1) {
      printf("Enter the number (1-5) for the punchline: ");
      if (scanf("%d", &answer) != 1) {
        while (getchar() != '\n') {
        }
        printf("Invalid number. Please enter a number between 1 and 5.\n");
        continue;
      }
      if (answer < 1 || answer > 5) {
        printf("Invalid number. Please enter a number between 1 and 5.\n");
      } else {
        break;
      }
    }
    GivePunchline(answer);
    printf("\n");
  }

  return 0;
}