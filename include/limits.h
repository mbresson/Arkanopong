
#ifndef INC_LIMITS_
#define INC_LIMITS_

#include "Block.h"
#include "Window.h"

#define LIM_STR_LEN 255

#define LIM_LEVEL_WIDTH (WINDOW_WIDTH / BLOCK_WIDTH)
#define LIM_PLAYER_SPACE_HEIGHT ((THEME_SHIELD_HEIGHT + THEME_PADDLE_HEIGHT) * 2)
#define LIM_LEVEL_HEIGHT (((WINDOW_HEIGHT) - (LIM_PLAYER_SPACE_HEIGHT*2)) / BLOCK_HEIGHT)

#endif

