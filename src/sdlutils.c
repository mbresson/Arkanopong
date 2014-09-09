
#include <assert.h>

#include "sdlutils.h"

SDL_Color sdlutils_color(int r, int g, int b) {
  assert(r >= 0 && r <= 255);
  assert(g >= 0 && g <= 255);
  assert(b >= 0 && b <= 255);

  SDL_Color color = {r, g, b, 255};

  return color;
}
