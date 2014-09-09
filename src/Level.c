
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

#include "bool.h"
#include "filesystem.h"
#include "stringutils.h"
#include "limits.h"
#include "Level.h"
#include "Event.h"
#include "Paddle.h"
#include "Block.h"
#include "Ball.h"
#include "IA.h"
#include "physics.h"
#include "Timer.h"
#include "Theme.h"
#include "Window.h"


// returns -1 on failure
static int read_next_int_from_file(FILE* file) {
  assert(file != NULL);

  int max_digits = (int) log10(__THEME_BLOCK_LAST - 1) + 1;
  size_t length = max_digits + 1;

  char *buffer = malloc(sizeof(char) * length);
  if(!buffer) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char) * length);
    exit(EXIT_FAILURE);
  }

  unsigned int index = 0;

  char c;

  while(index < length) {
    c = fgetc(file);

    if(!isdigit(c)) {
      if(c == '#') {
        do {
          c = fgetc(file);
        } while(c != '\n' && c != EOF);

        if(c == EOF) {
          break;
        }
      }

      if(index == 0) {
        // nothing read for now
        if(c == ' ' || c == '\t' || c == '\n') {
          continue;
        }
      }
      break;
    }

    buffer[index] = c;
    index++;
  }

  if(index == length) {
    // number was too long than the maximum numeric code for a block
    free(buffer);
    return -1;
  }

  buffer[index] = '\0';

  if(buffer[0] == '\0') {
    // no number was read at all
    free(buffer);
    return -1;
  }

  int number = atoi(buffer);

  free(buffer);

  return number;
}


int level_enumerate_levelsets(char*** sets_names, int** sets_nums_of_levels) {
  assert(sets_names != NULL);
  assert(sets_nums_of_levels != NULL);

  const char* path = DATA_PATH "levels/sets.list";

  FILE *file = fopen(path, "r");
  if(!file) {
    fprintf(stderr, "Couldn't open file '%s'!\n", path);
    exit(EXIT_FAILURE);
  }

  int length = filesystem_count_lines(file);
  if(!length) {
    fprintf(stderr, "File '%s' is empty!\n", path);
    exit(EXIT_FAILURE);
  }

  // because a set takes 2 lines: its name and its number of levels
  length /= 2;

  *sets_names = (char**) malloc(sizeof(char*) * length);
  if(!(*sets_names)) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char*) * length);
    exit(EXIT_FAILURE);
  }

  int index = 0;
  while(index < length) {
    (*sets_names)[index] = (char*) malloc(sizeof(char) * LIM_STR_LEN);
    if(!(*sets_names)[index]) {
      fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char) * LIM_STR_LEN);
      exit(EXIT_FAILURE);
    }

    index++;
  }

  *sets_nums_of_levels = (int*) malloc(sizeof(int) * length);
  if(!(*sets_nums_of_levels)) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(int) * length);
    exit(EXIT_FAILURE);
  }

  index = 0;
  bool read_number = false;

  char buffer[LIM_STR_LEN];

  while(index < length) {
    if(!fgets(buffer, LIM_STR_LEN, file)) {
      break;
    }

    if(stringutils_is_empty_line(buffer)) {
      continue;
    }

    char* linefeed = strchr(buffer, '\n');

    if(linefeed != NULL) {
      *linefeed = '\0';
    }

    if(!read_number) {
      strncpy((*sets_names)[index], buffer, LIM_STR_LEN);
    } else {
      (*sets_nums_of_levels)[index] = atoi(buffer);
      index++;
    }

    read_number = !read_number;
  }

  fclose(file);

  return length;
}

void level_free_enumeration(char*** sets_enum, int** levels_enum, int length) {
  assert(sets_enum != NULL);
  assert(levels_enum != NULL);
  assert(length > 0);

  int index = 0;
  for(; index < length; index++) {
    assert((*sets_enum)[index] != NULL);
    free((*sets_enum)[index]);
  }

  free((*sets_enum));
  free((*levels_enum));

  *sets_enum = NULL;
  *levels_enum = NULL;
}

bool level_load(const char* levelset, int id, Level* level) {
  assert(levelset != NULL);
  assert(id > 0);
  assert(level != NULL);

  int reset_x_i = 0, reset_y_i = 0;
  for(; reset_x_i < LIM_LEVEL_WIDTH; reset_x_i++) {
    for(reset_y_i = 0; reset_y_i < LIM_LEVEL_HEIGHT; reset_y_i++) {
      level->blocks[reset_y_i][reset_x_i] = BLOCK_EMPTY;
    }
  }

  const char* root = DATA_PATH "levels/";
  char tail[LIM_STR_LEN];
  snprintf(tail, LIM_STR_LEN, "%d.level", id);

  int len_levelset = strlen(levelset), len_root = strlen(root), len_tail = strlen(tail);
  int length = len_levelset + len_root + len_tail;

  char *path = malloc(sizeof(char) * (length + 2)); // + 1 for the / char and +1 for \0
  if(!path) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char) * (length + 2));
    exit(EXIT_FAILURE);
  }

  char *pointer = path;
  strcpy(pointer, root);
  pointer = &(path[len_root]);

  strcpy(pointer, levelset);
  pointer = &(path[len_root + len_levelset]);
  *pointer = '/';
  pointer++;

  strcpy(pointer, tail);

  path[length+1] = '\0';

  FILE *file = fopen(path, "r");
  if(!file) {
    fprintf(stderr, "Couldn't open file %s!\n", path);
    exit(EXIT_FAILURE);
  }

  char buffer[LIM_STR_LEN];

  bool read = false; // = 1 when width and height have been read, 2 when all has been read

  while(!read && fgets(buffer, LIM_STR_LEN, file) != NULL) {
    if(!stringutils_is_empty_line(buffer)) {
      if(
        sscanf(buffer, "%d %d", &(level->width), &(level->height)) == EOF ||
        level->width < 0 || level->height < 0 ||
        (!level->width ^ !level->height /* either width Xor height null is not allowed */)
      ) {
        fprintf(stderr, "File %s is malformatted!\n", path);
        exit(EXIT_FAILURE);
      }

      if(level->width > LIM_LEVEL_WIDTH) {
        fprintf(stderr, "Your level is too big (%d horizontal blocks) to fit in the window! "
          "Maximum allowed number of horizontal blocks: %d\n", level->width, LIM_LEVEL_WIDTH);
        exit(EXIT_FAILURE);
      }

      if(level->height > LIM_LEVEL_HEIGHT) {
        fprintf(stderr, "Your level is too big (%d vertical blocks) to fit in the window! "
          "Maximum allowed number of vertical blocks: %d\n", level->height, LIM_LEVEL_HEIGHT);
        exit(EXIT_FAILURE);
      }

      read = true;
    }
  }

  if(!read) {
    fprintf(stderr, "File %s is malformatted! Missing level width and height information.\n", path);
    exit(EXIT_FAILURE);
  }

  int row_index, col_index;
  for(row_index = 0; row_index < level->height; row_index++) {
    for(col_index = 0; col_index < level->width; col_index++) {
      int number = read_next_int_from_file(file);
      if(number == -1) {
        fprintf(stderr, "File %s is malformatted!\nMissing blocks.\n", path);
        exit(EXIT_FAILURE);
      }

      if(number >= __BLOCK_TYPE_LAST) {
        fprintf(stderr, "File %s is malformatted!\nIllegal block code.\n", path);
        exit(EXIT_FAILURE);
      }

      level->blocks[row_index][col_index] = number;
    }
  }

  fclose(file);

  free(path);

  return true;
}


void level_run(Level* level, const Settings* settings) {
  assert(level != NULL);
  assert(settings != NULL);

  SDL_EnableKeyRepeat(2000 / WINDOW_FRAMERATE, 1000 / WINDOW_FRAMERATE);
  ball_set_default_speed(settings->ball_speed);

  glClearColor(1, 1, 1, 1);

  Timer timer = timer_new(WINDOW_FRAMERATE);

  Paddle paddle_1 = paddle_new(0, settings->ball_speed);
  Paddle paddle_2 = paddle_new(1, settings->ball_speed);

  Ball_list *balls = ball_new_list();

  ball_append_to_list(
    balls,
    ball_new(BALL_NORMAL, 0, 0, 0)
  );

  ball_append_to_list(
    balls,
    ball_new(BALL_NORMAL, 0, 0, 1)
  );

  Ball* ball_1 = ball_get_from_list(balls, 0),
    *ball_2 = ball_get_from_list(balls, 1);

  physics_ball_stick_to_paddle(ball_1, &paddle_1);
  physics_ball_stick_to_paddle(ball_2, &paddle_2);

  int i = 0, j = 0; // used to read the blocks

  const bool* events = NULL;
  bool leave = false;
  bool pause = false;

  if(settings->play_with_computer) {
    physics_ball_kick(ball_2, &paddle_2, settings->ball_speed);
  }

  while(!leave) {
    events = event_poll();

    if(events[EVENT_QUIT] || events[EVENT_KEY_ESCAPE]) {
      leave = true;
    }

    if(events[EVENT_PAUSE]) {
      pause = true;
    }

    if(events[EVENT_RESUME]) {
      pause = false;
    }

    if(!pause) {
      if(events[EVENT_KEY_LEFT]) {
        paddle_move(&paddle_1, -1);
      }

      if(events[EVENT_KEY_RIGHT]) {
        paddle_move(&paddle_1, 1);
      }

      if(!settings->play_with_computer) {
        if(events[EVENT_KEY_A]) {
          paddle_move(&paddle_2, -1);
        }

        if(events[EVENT_KEY_Z]) {
          paddle_move(&paddle_2, 1);
        }
      }

      int ball_length = ball_list_length(balls);
      int ball_index = 0;
      while(ball_index < ball_length) {
        Ball *ball = ball_get_from_list(balls, ball_index);

        if(ball->y_speed == 0) {
          if(ball->last_player == 0) {
            if(events[EVENT_KEY_DOWN]) {
              physics_ball_kick(ball, &paddle_1, settings->ball_speed);
            } else {
              physics_ball_stick_to_paddle(ball, &paddle_1);
            }
          } else {
            if(settings->play_with_computer ^ events[EVENT_KEY_S]) {
              physics_ball_kick(ball, &paddle_2, settings->ball_speed);
            } else {
              physics_ball_stick_to_paddle(ball, &paddle_2);
            }
          }
        }
        ball_index++;
      }

/*
      if(ball_1->y_speed == 0) {
        
      }

      if(ball_2->y_speed == 0) {
        if(!settings->play_with_computer) {
          if(events[EVENT_KEY_S]) {
            physics_ball_kick(ball_2, &paddle_2, settings->ball_speed);
          } else {
            physics_ball_stick_to_paddle(ball_2, &paddle_2);
          }
        }
      }
      */

      if(settings->play_with_computer) {
        ia_play(&paddle_2, balls);
      }

      if(physics_live(balls, level, &paddle_1, &paddle_2)) {
        leave = true;
        break;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

      theme_draw_background();

      for(i = 0; i < level->height; i++) {
        for(j = 0; j < level->width; j++) {
          theme_draw_block(j * THEME_BLOCK_WIDTH, i * THEME_BLOCK_HEIGHT, level->blocks[i][j]);
        }
      }

      int balls_length = ball_list_length(balls);
      int ball_index = 0;
      while(ball_index < balls_length) {
        Ball *ball = ball_get_from_list(balls, ball_index);
        theme_draw_ball(ball->x, ball->y, (Theme_Ball) ball->type);
        ball_index++;
      }

      theme_draw_paddle(paddle_1.player, paddle_1.x, paddle_1.width);
      theme_draw_paddle(paddle_2.player, paddle_2.x, paddle_2.width);

      theme_draw_shield(0, paddle_1.shield);
      theme_draw_shield(1, paddle_2.shield);

      theme_draw_health(0, paddle_1.health);
      theme_draw_health(1, paddle_2.health);

    glPopMatrix();

    SDL_GL_SwapBuffers();

    timer_sleep(&timer);
  }

  ball_free_list(&balls);
}


