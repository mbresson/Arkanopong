
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "mathutils.h"


int mathutils_pow(int val, int exp) {
  int result = 1;
  int index = 0;
  for(; index < exp; index++) {
    result *= val;
  }

  return result;
}

int mathutils_random_in_range(int min, int max) {
  assert(min < max);
  
  srand(time(NULL));

  return rand() % (max - min) + min;
}

