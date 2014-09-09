
#include <assert.h>
#include <stdlib.h>

#include "Timer.h"

Uint32 timer_get_delay(Timer* timer) {
  assert(timer != NULL);

  Uint32 elapsed = SDL_GetTicks() - timer->start;
  if(elapsed >= timer->ms) {
    return 0;
  }

  return timer->ms - (SDL_GetTicks() - timer->start);
}

Timer timer_new(int freq) {
  assert(freq > 0);

  Timer timer = {
    SDL_GetTicks(),
    1000 / freq
  };

  return timer;
}


void timer_reset(Timer* timer) {
  assert(timer != NULL);

  timer->start = SDL_GetTicks();
}

void timer_sleep(Timer* timer) {
  assert(timer != NULL);

  Uint32 delay = timer_get_delay(timer);
  SDL_Delay(delay);
  timer_reset(timer);
}

