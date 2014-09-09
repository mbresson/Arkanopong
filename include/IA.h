
#ifndef INC_IA_
#define INC_IA_

#include "Paddle.h"
#include "Ball.h"

/**
 * must be called at each frame
 * this function moves the paddle around to catch the balls
 * the nearest balls are prioritized
 */
extern void ia_play(Paddle* paddle, Ball_list* balls);

#endif

