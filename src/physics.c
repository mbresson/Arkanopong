
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "bool.h"
#include "mathutils.h"
#include "Ball.h"
#include "physics.h"
#include "mathutils.h"
#include "Theme.h"
#include "Audio.h"


typedef enum {
  RESULT_NONE,
  RESULT_LEAVE,
  RESULT_ADD_BALL,
  RESULT_REMOVE_BALL,

  __RESULT_LAST
} Result;


/**
 * compute a coefficient used to modify the ball's horizontal speed
 * according to where it lands on the paddle
 *
 * e.g., if it comes from the left and lands on the rightmost part of the paddle,
 * it's horizontal speed will increase a lot
 *
 * or if it landed on the leftmost part of the paddle,
 * it's horizontal speed would decrease significantly
 *
 * the coefficient is comprised between 0 and 2
 */
static float get_ball_bounce_x_multiplier(Ball* ball, int r_x, int r_w) {
  float ball_cx = ball->x - r_x + THEME_BALL_WIDTH / 2.;
  float rec_cx = r_w / 2.;

  float coefficient = ball_cx / rec_cx;

  if(ball->x_speed < 0) {
    coefficient = CONVERT(coefficient, 0, 2, 2, 0);
  }

  return coefficient;
}


static bool ball_collide_rec(Ball* ball, int r_x, int r_y, int r_w, int r_h) {
  int ball_cx = ball->x + THEME_BALL_WIDTH / 2;
  int ball_cy = ball->y + THEME_BALL_HEIGHT / 2;

  int rec_cx = r_x + r_w / 2;
  int rec_cy = r_y + r_h / 2;

  int dist_x = ABS(ball_cx - rec_cx);
  int dist_y = ABS(ball_cy - rec_cy);

  float radius = THEME_BALL_WIDTH / 2.;

  if(dist_x >= r_w / 2 + radius) {
    return false;
  }

  if(dist_y >= r_h / 2 + radius) {
    return false;
  }

  if(dist_x < r_w / 2) {
    // collision on the top or the bottom side of the rectangle
    if(ball->y_speed > 0) {
      ball->y = r_y - (THEME_BALL_HEIGHT + 1);
    } else {
      ball->y = r_y + r_h + 1;
    }

    ball->y_speed = -ball->y_speed;
    
    // increase/descrease the ball's horizontal speed
    float x_multiplier = get_ball_bounce_x_multiplier(ball, r_x, r_w);

    ball->x_speed = (int)(ball->x_speed * x_multiplier);

    if(ball->x_speed < 0) {
      ball->x_speed = -CLAMP(BALL_MIN_SPEED, BALL_MAX_SPEED/2, -ball->x_speed);
    } else {
      ball->x_speed = CLAMP(BALL_MIN_SPEED, BALL_MAX_SPEED/2, ball->x_speed);
    }

    return true;
  }

  if(dist_y < r_h / 2) {
    if(ball->x_speed > 0) {
      ball->x = r_x - (THEME_BALL_WIDTH + 1);
    } else {
      ball->x = r_x + r_w + 1;
    }

    ball->x_speed = -ball->x_speed;
    return true;
  }

  float square_dist_corner =
    mathutils_pow(dist_x - r_w/2, 2) +
    mathutils_pow(dist_y - r_h/2, 2);

  if(square_dist_corner <= powf(radius, 2)) {
    ball->x_speed = -ball->x_speed;
    ball->y_speed = -ball->y_speed;
    return true;
  }

  return false;
}


static bool ball_collide_paddle(Ball* ball, Paddle* paddle) {
  assert(ball != NULL);
  assert(paddle != NULL);

  bool collision = ball_collide_rec(
    ball, paddle->x, paddle->y, paddle->width, paddle->height
  );

  return collision;
}


// checks for collision with a block and adjusts ball's position if there is one
// if there is no collision, returns BLOCK_EMPTY
static Block_Type ball_collide_blocks(Ball* ball, Level* level) {
  assert(ball != NULL);
  assert(level != NULL);

  if(
    ball->y + THEME_BALL_HEIGHT < LIM_PLAYER_SPACE_HEIGHT ||
    ball->y > WINDOW_HEIGHT - LIM_PLAYER_SPACE_HEIGHT
  ) {
    return BLOCK_EMPTY;
  }

  // select the area in which we need to check for collisions
  int x1 = ball->x / THEME_BLOCK_WIDTH;
  int y1 = (ball->y - LIM_PLAYER_SPACE_HEIGHT) / THEME_BLOCK_HEIGHT;
  int x2 = (ball->x + THEME_BALL_WIDTH) / THEME_BLOCK_WIDTH;
  int y2 = (ball->y - LIM_PLAYER_SPACE_HEIGHT + THEME_BALL_HEIGHT) / THEME_BLOCK_HEIGHT;
  if(y2 >= LIM_LEVEL_HEIGHT) {
    y2 = LIM_LEVEL_HEIGHT - 1;
  }

  int x_index = x1, y_index = y1;
  for(; x_index <= x2; x_index++) {
    for(y_index = y1; y_index <= y2; y_index++) {
      if(level->blocks[y_index][x_index] != BLOCK_EMPTY) {
        bool collision = ball_collide_rec(ball,
          x_index * THEME_BLOCK_WIDTH, y_index * THEME_BLOCK_HEIGHT + LIM_PLAYER_SPACE_HEIGHT, THEME_BLOCK_WIDTH, THEME_BLOCK_HEIGHT
        );

        if(collision) {
          Block_Type block = level->blocks[y_index][x_index];
          level->blocks[y_index][x_index] = BLOCK_EMPTY;
          return block;
        }
      }
    }
  }

  return BLOCK_EMPTY;
}


void physics_ball_kick(Ball* ball, Paddle* paddle, int speed) {
  assert(ball != NULL);
  assert(paddle != NULL);

  ball->x_speed = mathutils_random_in_range(-speed, speed);

  ball->y_speed = speed;
  if(paddle->player == 1) {
    ball->y_speed = -speed;
  }
}


void physics_ball_stick_to_paddle(Ball* ball, Paddle* paddle) {
  assert(ball != NULL);
  assert(paddle != NULL);

  ball->x = paddle->x + paddle->width / 2 - THEME_BALL_WIDTH / 2;

  if(paddle->player == 0) {
    ball->y = paddle->y + paddle->height;
  } else {
    ball->y = paddle->y - THEME_BALL_WIDTH;
  }
}


static Result physics_live_ball(Ball_list* balls, Ball* ball, Level* level, Paddle* p1, Paddle* p2) {
  assert(balls != NULL);
  assert(ball != NULL);
  assert(level != NULL);
  assert(p1 != NULL);
  assert(p2 != NULL);

  ball->x += ball->x_speed;
  ball->y += ball->y_speed;

  bool collision = false;

  /*
   * collision with a paddle
   */
  if(ball_collide_paddle(ball, p1)) {
    collision = true;
    ball->last_player = p1->player;
  } else if(ball_collide_paddle(ball, p2)) {
    collision = true;
    ball->last_player = p2->player;
  }


  /*
   * collision with the window's edges
   */
  if(ball->x + THEME_BALL_WIDTH > WINDOW_WIDTH) {
    ball->x = WINDOW_WIDTH - THEME_BALL_WIDTH;
    ball->x_speed = -ball->x_speed;
    collision = true;
  } else if(ball->x < 0) {
    ball->x = 0;
    ball->x_speed = -ball->x_speed;
    collision = true;
  }

  /*
   * collision with a player's shield
   */
  if(
    p2->shield < __THEME_SHIELD_LAST &&
    ball->y + THEME_BALL_HEIGHT > WINDOW_HEIGHT - THEME_SHIELD_HEIGHT
  ) {
    (p2->shield)++;
    if(p2->shield == __THEME_SHIELD_LAST) {
      audio_play(SOUND_SHIELD_OFF);
    }

    ball->y_speed = -ball->y_speed;
    collision = true;
  } else if(
    p1->shield < __THEME_SHIELD_LAST &&
    ball->y < THEME_SHIELD_HEIGHT
  ) {
    (p1->shield)++;
    if(p1->shield == __THEME_SHIELD_LAST) {
      audio_play(SOUND_SHIELD_OFF);
    }

    ball->y_speed = -ball->y_speed;
    collision = true;
  }

  /*
   * collision behind a player
   */
  if(ball->y + THEME_BALL_HEIGHT > WINDOW_HEIGHT) {
    ball->y = WINDOW_HEIGHT - THEME_BALL_HEIGHT;
    ball->y_speed = -ball->y_speed;
    (p2->health)--;
    collision = true;

    audio_play(SOUND_HEART_MINUS);

    if(p2->health <= 0) {
      return RESULT_LEAVE;
    }

    if(ball->type == BALL_BONUS) {
      return RESULT_REMOVE_BALL;
    }

    ball->y_speed = 0;
    ball->x_speed = 0;
    ball->last_player = 1;
    physics_ball_stick_to_paddle(ball, p2);
  } else if(ball->y < 0) {
    ball->y = 0;
    ball->y_speed = -ball->y_speed;
    (p1->health)--;
    collision = true;

    audio_play(SOUND_HEART_MINUS);

    if(p1->health <= 0) {
      return RESULT_LEAVE;
    }

    if(ball->type == BALL_BONUS) {
      return RESULT_REMOVE_BALL;
    }
    ball->y_speed = 0;
    ball->x_speed = 0;
    ball->last_player = 0;
    physics_ball_stick_to_paddle(ball, p1);
  }

  // collision with blocks
  Block_Type block_collision = ball_collide_blocks(ball, level);
  if(block_collision != BLOCK_EMPTY) {
    collision = true;
    switch(block_collision) {
      case BLOCK_SHIELD:
        if(ball->last_player == 0) {
          p1->shield = THEME_SHIELD_100;
        } else {
          p2->shield = THEME_SHIELD_100;
        }
        audio_play(SOUND_SHIELD_ON);
        break;

      case BLOCK_SHRINK:
        if(ball->last_player == 0) {
          paddle_shrink(p2);
        } else {
          paddle_shrink(p1);
        }
        audio_play(SOUND_PADDLE_SHRINK);
        break;

      case BLOCK_ENLARGE:
        if(ball->last_player == 0) {
          paddle_enlarge(p1);
        } else {
          paddle_enlarge(p2);
        }
        audio_play(SOUND_PADDLE_ENLARGE);
        break;

      case BLOCK_HEART_PLUS:
        if(ball->last_player == 0) {
          (p1->health)++;
        } else {
          (p2->health)++;
        }
        audio_play(SOUND_HEART_PLUS);
        break;

      case BLOCK_HEART_MINUS:
        if(ball->last_player == 0) {
          (p2->health)--;
        } else {
          (p1->health)--;
        }
        audio_play(SOUND_HEART_MINUS);
        break;

      case BLOCK_BALL_NEUTRAL:
        audio_play(SOUND_BALL_SPAWN);

        return RESULT_ADD_BALL;

      default: break;
    }
  }

  if(collision) {
    audio_play(SOUND_HIT);
  }

  return RESULT_NONE;
}


bool physics_live(Ball_list* balls, Level* level, Paddle* p1, Paddle* p2) {
  assert(balls != NULL);
  assert(level != NULL);
  assert(p1 != NULL);
  assert(p2 != NULL);

  int length = ball_list_length(balls);
  int index = 0;
  while(index < length) {
    Ball* current = ball_get_from_list(balls, index);

    Result result = physics_live_ball(
      balls, current,
      level, p1, p2
    );

    switch(result) {
      case RESULT_LEAVE:
        return true;

      case RESULT_ADD_BALL:
        ball_append_to_list(
          balls,
          ball_new(BALL_BONUS, current->x, current->y, current->last_player)
        );

        Ball* last = ball_get_from_list(balls, length);
        last->x_speed = -(current->x_speed);
        last->y_speed = -(current->y_speed);
        break;

      case RESULT_REMOVE_BALL:
        ball_remove_from_list(
          balls, index
        );
        length--;
        index--;
        break;

      default: break;
    }
    index++;
  }

  return false;
}

