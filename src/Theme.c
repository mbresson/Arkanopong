
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <SDL/SDL_image.h>

#include "filesystem.h"
#include "glutils.h"
#include "stringutils.h"
#include "Level.h"
#include "Theme.h"
#include "limits.h"


static char s_current_theme[LIM_STR_LEN] = { '\0' };

static GLuint s_pre_tex_theme = 0;
static GLuint s_pre_tex_background = 0;

static GLuint s_list_paddle[2];
static GLuint s_list_health[2];
static GLuint s_list_shield[__THEME_SHIELD_LAST];
static GLuint s_list_ball[__THEME_BALL_LAST];
static GLuint s_list_block[THEME_BLOCK_EMPTY];
static GLuint s_list_background;


void theme_draw_background(void) {
  glCallList(s_list_background);
}


void theme_draw_ball(int x, int y, Theme_Ball ball) {
  glPushMatrix();
    glTranslatef(
      DIMX_TO_GL(x), -DIMY_TO_GL(y), 0
    );

    glCallList(s_list_ball[ball]);
  glPopMatrix();
}


void theme_draw_block(int x, int y, Block_Type block) {
  int list_id = 0;

  switch(block) {
    case BLOCK_NEUTRAL: list_id = THEME_BLOCK_NEUTRAL; break;
    case BLOCK_SHIELD: list_id = THEME_BLOCK_SHIELD; break;
    case BLOCK_SHRINK: list_id = THEME_BLOCK_SHRINK; break;
    case BLOCK_ENLARGE: list_id = THEME_BLOCK_ENLARGE; break;
    case BLOCK_HEART_PLUS: list_id = THEME_BLOCK_HEART_PLUS; break;
    case BLOCK_HEART_MINUS: list_id = THEME_BLOCK_HEART_MINUS; break;
    case BLOCK_BALL_NEUTRAL: list_id = THEME_BLOCK_BALL; break;
    default: return;
  }

  y += LIM_PLAYER_SPACE_HEIGHT;

  glPushMatrix();
    glTranslatef(
      DIMX_TO_GL(x), -DIMY_TO_GL(y), 0
    );

    if(block > BLOCK_NEUTRAL) {
      glCallList(s_list_block[THEME_BLOCK_BONUS]);
    }

    glCallList(s_list_block[list_id]);
  glPopMatrix();
}


void theme_draw_health(int player, int health) {
  if(health <= 0) {
    return;
  }

  int x = 0, y = 0;
  if(player == 1) {
    y = WINDOW_HEIGHT - THEME_HEALTH_HEIGHT;
    x = WINDOW_WIDTH - THEME_HEALTH_WIDTH*health;
  }

  glPushMatrix();
    glTranslatef(DIMX_TO_GL(x), -DIMY_TO_GL(y), 0);

    while(health > 0) {
      glCallList(s_list_health[player]);
      glTranslatef(DIMX_TO_GL(THEME_HEALTH_WIDTH), 0, 0);

      health--;
    }
  glPopMatrix();
}


void theme_draw_paddle(int player, int x, unsigned int width) {
  assert(width % THEME_PADDLE_WIDTH == 0);

  // place the paddle just below the paddle
  int y = THEME_SHIELD_HEIGHT;
  if(player == 1) {
    // or above, if it's the player on the bottom of the screen
    y = WINDOW_HEIGHT - THEME_PADDLE_HEIGHT - THEME_SHIELD_HEIGHT;
  }

  int num_of_drawings = width / THEME_PADDLE_WIDTH;

  glPushMatrix();
    glTranslatef(DIMX_TO_GL(x), -DIMY_TO_GL(y), 0);

    while(num_of_drawings > 0) {
      glCallList(s_list_paddle[player]);

      glTranslatef(DIMX_TO_GL(THEME_PADDLE_WIDTH), 0, 0);

      num_of_drawings--;
    }
  glPopMatrix();
}


void theme_draw_shield(int player, Theme_Shield state) {
  if(state >= __THEME_SHIELD_LAST) {
    return;
  }

  int y = 0;
  if(player == 1) {
    y = WINDOW_HEIGHT - THEME_SHIELD_HEIGHT;
  }

  int index = 0, last = WINDOW_WIDTH / THEME_SHIELD_WIDTH;
  float x_offset = DIMX_TO_GL(THEME_SHIELD_WIDTH);

  glPushMatrix();
    glTranslatef(0, -DIMY_TO_GL(y), 0);

    while(index <= last) {
      glCallList(s_list_shield[state]);

      glTranslatef(x_offset, 0, 0);
      index++;
    }
  glPopMatrix();
}


char** theme_enumerate_themes(int* lines) {
  assert(lines != NULL);

  const char* path = DATA_PATH "themes/themes.list";

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

  *lines = length;

  char** list = (char**) malloc(sizeof(char*) * length);
  if(!list) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char*) * length);
    exit(EXIT_FAILURE);
  }

  int index = 0;
  while(index < length) {
    list[index] = (char*) malloc(sizeof(char) * LIM_STR_LEN);
    if(!list[index]) {
      fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(char) * LIM_STR_LEN);
      exit(EXIT_FAILURE);
    }

    index++;
  }

  index = 0;
  while(fgets(list[index], LIM_STR_LEN, file) != NULL) {
    if(!stringutils_is_empty_line(list[index])) {
      char* linefeed = strchr(list[index], '\n');

      if(linefeed != NULL) {
        *linefeed = '\0';
      }
      index++;
    }

    if(index == length) {
      break;
    }
    // empty lines skipped
  }

  fclose(file);

  return list;
}


void theme_free_enumeration(char*** enumeration, int length) {
  assert(enumeration != NULL);
  assert(length > 0);

  int index = 0;
  for(; index < length; index++) {
    assert((*enumeration)[index] != NULL);
    free((*enumeration)[index]);
  }

  free((*enumeration));
  *enumeration = NULL;
}


void theme_load(const char* name) {
  assert(name != NULL);

  if(
    s_current_theme != NULL &&
    strcmp(s_current_theme, name) == 0
  ) {
    // the theme is already loaded
    return;
  } else {
    // free previous theme
    theme_unload();
  }

  int index = 0;

  int width = 0, height = 0;
  const char* extensions[] = {
    ".png", ".bmp", ".jpg", ".jpeg"
  };
  int num_extensions = 4;
  int index_extension = -1;

  // load theme image
  char theme_path[LIM_STR_LEN*2] = ROOT_PATH "data/themes/";
  strncat(theme_path, name, LIM_STR_LEN-1);

  index_extension = filesystem_detect_extension(theme_path, extensions, num_extensions);
  if(index_extension == -1) {
    fprintf(stderr, "There is no image file for %s theme!\n", name);
    exit(EXIT_FAILURE);
  }

  strcat(theme_path, extensions[index_extension]);

  s_pre_tex_theme = glutils_load_texture_from_file(theme_path);

  glutils_get_texture_dim(s_pre_tex_theme, &width, &height);

  // load theme lists

  // THEME_PADDLE 1 & 2
  const int paddle_x[] = { THEME_PADDLE_1_X, THEME_PADDLE_2_X };
  const int paddle_y[] = { THEME_PADDLE_1_Y, THEME_PADDLE_2_Y };
  for(index = 0; index < 2; index++) {
    s_list_paddle[index] = glutils_create_list_rec(
      s_pre_tex_theme, 
      width, height, 0, 0,
      paddle_x[index], paddle_y[index],
      THEME_PADDLE_WIDTH, THEME_PADDLE_HEIGHT
    );
  }
 
  // THEME_HEALTH 1 & 2
  const int health_x[] = { THEME_HEALTH_1_X, THEME_HEALTH_2_X };
  const int health_y[] = { THEME_HEALTH_1_Y, THEME_HEALTH_2_Y };
  for(index = 0; index < 2; index++) {
    s_list_health[index] = glutils_create_list_rec(
      s_pre_tex_theme, 
      width, height, 0, 0,
      health_x[index], health_y[index],
      THEME_HEALTH_WIDTH, THEME_HEALTH_HEIGHT
    );
  }
 
  // THEME_BALL_{MAIN, NEUTRAL}
  const int ball_x[] = { THEME_BALL_MAIN_X, THEME_BALL_NEUTRAL_X };
  const int ball_y[] = { THEME_BALL_MAIN_Y, THEME_BALL_NEUTRAL_Y };
  for(index = 0; index < __THEME_BALL_LAST; index++) {
    s_list_ball[index] = glutils_create_list_rec(
      s_pre_tex_theme, 
      width, height, 0, 0,
      ball_x[index], ball_y[index],
      THEME_BALL_WIDTH, THEME_BALL_HEIGHT
    );
  }

  // THEME_BLOCK_*
  const int block_x[] = {
    THEME_BLOCK_NEUTRAL_X,
    THEME_BLOCK_BONUS_X,
    THEME_BLOCK_SHIELD_X,
    THEME_BLOCK_SHRINK_X,
    THEME_BLOCK_ENLARGE_X,
    THEME_BLOCK_HEART_PLUS_X,
    THEME_BLOCK_HEART_MINUS_X,
    THEME_BLOCK_BONUSBALL_X
  };

  const int block_y[] = {
    THEME_BLOCK_NEUTRAL_Y,
    THEME_BLOCK_BONUS_Y,
    THEME_BLOCK_SHIELD_Y,
    THEME_BLOCK_SHRINK_Y,
    THEME_BLOCK_ENLARGE_Y,
    THEME_BLOCK_HEART_PLUS_Y,
    THEME_BLOCK_HEART_MINUS_Y,
    THEME_BLOCK_BONUSBALL_Y
  };

  for(index = 0; index < THEME_BLOCK_EMPTY; index++) {
    s_list_block[index] = glutils_create_list_rec(
      s_pre_tex_theme, 
      width, height, 0, 0,
      block_x[index], block_y[index],
      THEME_BLOCK_WIDTH, THEME_BLOCK_HEIGHT
    );
  }

  // THEME_SHIELD
  const int shield_x[] = {
    THEME_SHIELD_100_X,
    THEME_SHIELD_75_X,
    THEME_SHIELD_50_X,
    THEME_SHIELD_25_X
  };

  const int shield_y[] = {
    THEME_SHIELD_100_Y,
    THEME_SHIELD_75_Y,
    THEME_SHIELD_50_Y,
    THEME_SHIELD_25_Y
  };

  for(index = 0; index < __THEME_SHIELD_LAST; index++) {
    s_list_shield[index] = glutils_create_list_rec(
      s_pre_tex_theme, 
      width, height, 0, 0,
      shield_x[index], shield_y[index],
      THEME_SHIELD_WIDTH, THEME_SHIELD_HEIGHT
    );
  }


  // load theme background image
  char background_path[LIM_STR_LEN*2] = ROOT_PATH "data/themes/";
  strncat(background_path, name, LIM_STR_LEN-1);
  strncat(background_path, "-background", LIM_STR_LEN-1);

  index_extension = filesystem_detect_extension(background_path, extensions, num_extensions);
  if(index_extension == -1) {
    fprintf(stderr, "There is no background file for %s theme!\n", name);
    exit(EXIT_FAILURE);
  }

  strcat(background_path, extensions[index_extension]);

  s_pre_tex_background = glutils_load_texture_from_file(background_path);
  glutils_get_texture_dim(s_pre_tex_background, &width, &height);

  s_list_background = glutils_create_list(
    s_pre_tex_background, width, height, 0, 0
  );

  strncpy(s_current_theme, name, LIM_STR_LEN);
}


void theme_unload(void) {
  if(s_current_theme != NULL) {
    glDeleteTextures(1, &s_pre_tex_theme);
    glDeleteTextures(1, &s_pre_tex_background);
    
    glDeleteLists(s_list_background, 1);
    glDeleteLists(s_list_paddle[0], 1);
    glDeleteLists(s_list_paddle[1], 1);
    glDeleteLists(s_list_health[0], 1);
    glDeleteLists(s_list_health[1], 1);

    int index;
    for(index = 0; index < __THEME_BALL_LAST; index++) {
      glDeleteLists(s_list_ball[index], 1);
    }

    for(index = 0; index < THEME_BLOCK_EMPTY; index++) {
      glDeleteLists(s_list_block[index], 1);
    }

    for(index = 0; index < __THEME_SHIELD_LAST; index++) {
      glDeleteLists(s_list_shield[index], 1);
    }
  }
}

