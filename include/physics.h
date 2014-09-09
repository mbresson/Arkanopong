
#ifndef INC_PHYSICS_
#define INC_PHYSICS_

#include "bool.h"
#include "Ball.h"
#include "Level.h"
#include "Paddle.h"


/**
 * give initial momentum to the ball
 */
extern void physics_ball_kick(Ball* ball, Paddle* paddle, int speed);


/**
 * positions the ball close to the paddle
 */
extern void physics_ball_stick_to_paddle(Ball* ball, Paddle* paddle);


/**
 * this function moves the ball around and checks for collisions
 *
 * if there is a collision with a paddle, the ball information is updated
 * if there is a collision with a block, the bonus is distributed
 * and so on
 *
 * @return
 * if the game stops because one of the players has lost, returns true
 * else, returns false
 */
extern bool physics_live(Ball_list* balls, Level* level, Paddle* p1, Paddle* p2);

#endif
