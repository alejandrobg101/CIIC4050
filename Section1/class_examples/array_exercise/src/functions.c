#include <ctype.h>
#include <unistd.h>

#include "functions.h"

//definicion
void ModifyArray(char *array){
  for (int i =0; array[i] != '\0'; i++){
    array[i] = toupper(array[i]);
  }
}