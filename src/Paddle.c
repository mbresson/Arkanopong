
#include <assert.h>
#include <stdlib.h>

#include "Paddle.h"
#include "Window.h"
#include "Theme.h"

void paddle_enlarge(Paddle* paddle) {
  assert(paddle != NULL);

  if(paddle->width == PADDLE_MAX_WIDTH) {
    return;
  }

  paddle->width += THEME_PADDLE_WIDTH;

  // adapt its x position
  paddle->x -= THEME_PADDLE_WIDTH / 2;

  if(paddle->x < 0) {
    paddle->x = 0;
  } else if(paddle->x > WINDOW_WIDTH - paddle->width) {
    paddle->x = WINDOW_WIDTH - paddle->width;
  }
}

void paddle_move(Paddle* paddle, int offset) {
  assert(paddle != NULL);
  assert(offset == -1 || offset == 1);

  int x_offset = offset == -1 ? -paddle->speed : paddle->speed;

  if(paddle->x + x_offset + paddle->width > WINDOW_WIDTH) {
    //printf("RIGHT: x = %d, off = %d, width = %d\n", paddle->x, x_offset, paddle->width);
    paddle->x = WINDOW_WIDTH - paddle->width;
  } else if((paddle->x + x_offset) < 0) {
    paddle->x = 0;
  } else {
    paddle->x += x_offset;
  }
}

Paddle paddle_new(int player, unsigned int ball_speed) {
  assert(player == 0 || player == 1);

  Paddle paddle = {
    WINDOW_WIDTH / 2 - PADDLE_INIT_WIDTH / 2, // x
    PADDLE_INIT_WIDTH, // width
    3, // health
    (player == 0 ? THEME_SHIELD_HEIGHT : WINDOW_HEIGHT - THEME_SHIELD_HEIGHT - THEME_PADDLE_HEIGHT), // y
    THEME_PADDLE_HEIGHT, // height
    ball_speed * 2, // speed
    player,
    __THEME_SHIELD_LAST // shield
  };

  return paddle;
}

void paddle_shrink(Paddle* paddle) {
  assert(paddle != NULL);

  if(paddle->width == PADDLE_MIN_WIDTH) {
    return;
  }

  // adapt its x position
  paddle->width -= THEME_PADDLE_WIDTH;
  paddle->x += THEME_PADDLE_WIDTH / 2;
}

