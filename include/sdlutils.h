
#ifndef INC_SDLUTILS_
#define INC_SDLUTILS_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

/**
 * r, g, b are comprised between 0 and 255
 */
extern SDL_Color sdlutils_color(int r, int g, int b);

#endif

