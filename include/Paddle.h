
#ifndef INC_PADDLE_
#define INC_PADDLE_

#include "Theme.h"
#include "mathutils.h"

#define PADDLE_MAX_WIDTH ((THEME_PADDLE_WIDTH) * 8)
#define PADDLE_MIN_WIDTH (THEME_PADDLE_WIDTH)
#define PADDLE_INIT_WIDTH (AVERAGE((PADDLE_MIN_WIDTH), (PADDLE_MAX_WIDTH)) / (THEME_PADDLE_WIDTH) * (THEME_PADDLE_WIDTH))

typedef struct {
  int x, width, health;
  const int y, height, speed, player;
  Theme_Shield shield; // warning: to weaken the shield, ++ it (see Theme_Shield enum)
} Paddle;

/**
 * enlarges the paddle and centers its position correctly according to its new width
 * makes sure if doesn't move out of the screen when enlarged
 */
extern void paddle_enlarge(Paddle* paddle);

/**
 * @param offset
 * either 1 (to go right) or -1 (to go left)
 *
 * moves the paddle and makes sure it doesn't move off the screen
 */
extern void paddle_move(Paddle* paddle, int offset);

extern Paddle paddle_new(int player, unsigned int ball_speed);

/**
 * shrinks the paddle and centers its position correctly according to its new width
 */
extern void paddle_shrink(Paddle* paddle);

#endif

