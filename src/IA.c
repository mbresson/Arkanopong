
#include <stdlib.h>
#include <assert.h>

#include "IA.h"
#include "Theme.h"
#include "Window.h"

void ia_play(Paddle* paddle, Ball_list* balls) {
  assert(paddle != NULL);
  assert(balls != NULL);

  int nearest_dist = WINDOW_HEIGHT, nearest_x = 0, nearest_x_speed = 0;

  /*
   * retrieve the coordinates of the closest ball
   * which moves toward the paddle
   */

  int index = 0, length = ball_list_length(balls);
  for(; index < length; index++) {
    Ball* ball = ball_get_from_list(balls, index);

    if(
      (ball->y > paddle->y && ball->y_speed < 0) ||
      (ball->y - THEME_BALL_HEIGHT < paddle->y && ball->y_speed > 0)
    ) {
      // the ball is moving toward us
      int dist = ABS(ball->y - paddle->y);

      if(dist < nearest_dist) {
        nearest_dist = dist;
        nearest_x = ball->x;
        nearest_x_speed = ball->x_speed;
      }
    }
  }

  if(nearest_dist == WINDOW_HEIGHT) {
    return;
  }

  int ball_cx = nearest_x + THEME_BALL_WIDTH / 2;

  if(ball_cx + nearest_x_speed > paddle->x + paddle->width) {
    paddle_move(paddle, 1);
  } else if(ball_cx + nearest_x_speed < paddle->x) {
    paddle_move(paddle, -1);
  }
}

