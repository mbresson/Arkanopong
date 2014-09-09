
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "bool.h"
#include "Ball.h"
#include "Audio.h"
#include "filesystem.h"
#include "mathutils.h"
#include "glutils.h"
#include "sdlutils.h"
#include "Menu.h"
#include "Event.h"
#include "Level.h"
#include "Theme.h"
#include "Timer.h"
#include "Window.h"
#include "assets.h"


#define TEXT_SECOND_PLAYER "Second player"
#define TEXT_COMPUTER "Computer"


static bool s_preloaded = false;


typedef enum {
  /*
   * menu's components are used to draw variable text
   * e.g. computer or human option, +++---- for sound volume control
   */

  COMPONENT_ACTIVE,
  COMPONENT_COMPUTER,
  COMPONENT_HUMAN,
  COMPONENT_PLUS,
  COMPONENT_MINUS,

  __MENU_COMPONENT_LAST
} Menu_Component;

typedef enum {
  /*
   * menu's option labels are static and never change
   */

  LABEL_SECOND_PLAYER,
  LABEL_SOUND_VOLUME,
  LABEL_LEVEL_SET,
  LABEL_LEVEL,
  LABEL_THEME,
  LABEL_BALL_SPEED,
  LABEL_QUIT,

  __MENU_LABEL_LAST
} Menu_Label;

static Menu_Label s_focus = LABEL_SECOND_PLAYER;
static int s_level_set_index = 0, s_theme_index = 0;

static int s_options_y[__MENU_LABEL_LAST];

static const char s_str_components[][LIM_STR_LEN] = {
  "<",
  "computer",
  "human",
  "+",
  "-"
};

static const char s_str_labels[][LIM_STR_LEN] = {
  "Second player",
  "Sound volume",
  "Level set",
  "Level",
  "Theme",
  "Ball speed",
  "Quit"
};

// OpenGL pre-rendered lists
static GLuint s_pre_tex_main_title = 0; // pre-rendered game main title image
static GLuint s_pre_tex_components[__MENU_COMPONENT_LAST]; // pre-rendered menu components
static GLuint s_pre_tex_components_focus[__MENU_COMPONENT_LAST]; // pre-rendered menu components, when focused on
static GLuint s_pre_tex_labels[__MENU_LABEL_LAST]; // pre-rendered menu labels
static GLuint s_pre_tex_labels_focus[__MENU_LABEL_LAST]; // pre-rendered menu labels, when focused on
static GLuint s_pre_tex_numbers[10]; // pre-rendered numbers from 0 to 9, to display any number
static GLuint s_pre_tex_numbers_focus[10]; // pre-rendered numbers from 0 to 9, to display any number
static GLuint* s_pre_tex_theme_names = NULL; // pre-rendered themes names
static GLuint* s_pre_tex_theme_names_focus = NULL; // pre-rendered themes names
static GLuint* s_pre_tex_set_names = NULL; // pre-rendered sets names
static GLuint* s_pre_tex_set_names_focus = NULL; // pre-rendered sets names


static GLuint s_list_main_title = 0;
static GLuint s_list_components[__MENU_COMPONENT_LAST];
static GLuint s_list_components_focus[__MENU_COMPONENT_LAST];
static GLuint s_list_labels[__MENU_LABEL_LAST];
static GLuint s_list_labels_focus[__MENU_LABEL_LAST];
static GLuint s_list_numbers[10];
static GLuint s_list_numbers_focus[10];
static GLuint* s_list_theme_names = NULL;
static GLuint* s_list_theme_names_focus = NULL;
static GLuint* s_list_set_names = NULL;
static GLuint* s_list_set_names_focus = NULL;


static char** s_sets_names = NULL;
static char** s_themes_names = NULL;
static int* s_sets_levels = NULL;
static int s_sets_names_length = 0;
static int s_themes_names_length = 0;


/*
 * explanation on number witdh:
 * when a number is drawn on the screen,
 * its width is variable, depending on its digits
 * to position the focus hint correctly around it,
 * we need to have its width
 *
 * this value is only used when a numeric option has the focus
 * (e.g. the level number or the ball speed)
 */
static void menu_display_focus_hint(int number_width) {
  int width = 0, tmp_h = 0, hint_width = 0;
  int y = s_options_y[s_focus], x = 0;

  glutils_get_texture_dim(s_pre_tex_components[COMPONENT_ACTIVE], &hint_width, &tmp_h);

  switch(s_focus) {
  case LABEL_SECOND_PLAYER:
    glutils_get_texture_dim(s_pre_tex_components[COMPONENT_COMPUTER], &width, &tmp_h);
    x = -width / 2 - hint_width;
    break;

  case LABEL_SOUND_VOLUME:
    glutils_get_texture_dim(s_pre_tex_components[COMPONENT_PLUS], &width, &tmp_h);
    x = -width * 5 - hint_width;
    break;

  case LABEL_LEVEL_SET:
    glutils_get_texture_dim(s_pre_tex_set_names[s_level_set_index], &width, &tmp_h);
    x = -width / 2 - hint_width;
    break;

  case LABEL_THEME:
    glutils_get_texture_dim(s_pre_tex_theme_names[s_theme_index], &width, &tmp_h);
    x = -width / 2 - hint_width;
    break;

  case LABEL_LEVEL:
  case LABEL_BALL_SPEED:
    width = number_width;
    x = -width / 2 - hint_width;
    break;

  default: return;
  }

  glPushMatrix();
    glTranslatef(
      DIMX_TO_GL(x),
      -DIMY_TO_GL(y),
      0
    );

    glCallList(s_list_components_focus[COMPONENT_ACTIVE]);
  glPopMatrix();

  glPushMatrix();
    glTranslatef(
      -DIMX_TO_GL(x),
      -DIMY_TO_GL(y),
      0
    );

    glScalef(
      -1,
      1,
      1
    );

    glCallList(s_list_components_focus[COMPONENT_ACTIVE]);
  glPopMatrix();
}


// return the width of the displayed number
static int menu_display_numbers(int number, int y, bool focus) {
  int num_digits = (int) log10(number) + 1;

  int index = num_digits - 1;
  int sub = 0;

  int displayed_width = 0;
  int width = 0, height = 0;

  while(index >= 0) {
    int exp = mathutils_pow(10, index);

    int digit = (int)(((float)number - sub) / (float)exp);

    glutils_get_texture_dim(s_pre_tex_numbers[digit], &width, &height);

    sub += digit * exp;

    index--;
    displayed_width += width;
  }

  glPushMatrix();
    glTranslatef(
      DIMX_TO_GL(WINDOW_WIDTH/2 - (displayed_width + width*2) / 2),
      -DIMY_TO_GL(y),
      0
    );

    index = num_digits - 1;
    sub = 0;

    // forced to compute the same digits once again, to have the whole thing centered
    while(index >= 0) {
      int exp = mathutils_pow(10, index);

      int digit = (int)(((float)number - sub) / (float)exp);

      glutils_get_texture_dim(s_pre_tex_numbers[digit], &width, &height);

      glTranslatef(
        DIMX_TO_GL(width),
        0, 0
      );

      if(focus) {
        glCallList(s_list_numbers_focus[digit]);
      } else {
        glCallList(s_list_numbers[digit]);
      }

      sub += digit * exp;

      index--;
    }
  glPopMatrix();

  return displayed_width;
}


static void menu_display_volume(int volume) {
  assert(volume >= 0 && volume <= 100);

  bool active = s_focus == LABEL_SOUND_VOLUME;

  int ten_vol = CONVERT(volume, 0, 100, 0, 10);
  int width, tmp_h;
  glutils_get_texture_dim(s_pre_tex_components_focus[COMPONENT_PLUS], &width, &tmp_h);
  int total_width = width * 10;

  glPushMatrix();
    glTranslatef(
      -DIMX_TO_GL(total_width/2 + width/2),
      0,
      0
    );

    int index = 0;
    for(; index < ten_vol; index++) {
      glTranslatef(
        DIMX_TO_GL(width),
        0,
        0
      );

      if(active) {
        glCallList(s_list_components_focus[COMPONENT_PLUS]);
      } else {
        glCallList(s_list_components[COMPONENT_PLUS]);
      }
    }

    for(index = 0; index < 10 - ten_vol; index++) {
      glTranslatef(
        DIMX_TO_GL(width),
        0,
        0
      );

      if(active) {
        glCallList(s_list_components_focus[COMPONENT_MINUS]);
      } else {
        glCallList(s_list_components[COMPONENT_MINUS]);
      }

    }

  glPopMatrix();
}


static void menu_display(Settings* settings) {
  assert(settings != NULL);

  glCallList(s_list_main_title);

  unsigned int index = 0;
  for(; index < __MENU_LABEL_LAST; index++) {
    if(index == s_focus) {
      glCallList(s_list_labels_focus[index]);
    } else {
      glCallList(s_list_labels[index]);
    }
  }

  int list_index = 0;
  GLuint list_id = 0;

  // display enemy choice
  if(settings->play_with_computer) {
    list_index = COMPONENT_COMPUTER;
  } else {
    list_index = COMPONENT_HUMAN;
  }

  if(s_focus == LABEL_SECOND_PLAYER) {
    list_id = s_list_components_focus[list_index];
  } else {
    list_id = s_list_components[list_index];
  }

  glCallList(list_id);

  // display volume choice
  menu_display_volume(settings->volume);

  // display set choice
  if(s_focus == LABEL_LEVEL_SET) {
    list_id = s_list_set_names_focus[s_level_set_index];
  } else {
    list_id = s_list_set_names[s_level_set_index];
  }

  glCallList(list_id);

  // display theme choice
  if(s_focus == LABEL_THEME) {
    list_id = s_list_theme_names_focus[s_theme_index];
  } else {
    list_id = s_list_theme_names[s_theme_index];
  }

  glCallList(list_id);

  // display level number
  int number_width = 0;
  if(s_focus == LABEL_LEVEL) {
    number_width = menu_display_numbers(settings->level, s_options_y[LABEL_LEVEL], true);
  } else {
    menu_display_numbers(settings->level, s_options_y[LABEL_LEVEL], false);
  }

  // display ball speed
  if(s_focus == LABEL_BALL_SPEED) {
    number_width = menu_display_numbers(settings->ball_speed, s_options_y[LABEL_BALL_SPEED], true);
  } else {
    menu_display_numbers(settings->ball_speed, s_options_y[LABEL_BALL_SPEED], false);
  }

  menu_display_focus_hint(number_width);
}


static void menu_free(void) {
  assert(s_preloaded);

  theme_free_enumeration(&s_themes_names, s_themes_names_length);
  level_free_enumeration(&s_sets_names, &s_sets_levels, s_sets_names_length);

  // free all allocated textures

  glDeleteTextures(1, &s_pre_tex_main_title);
  glDeleteLists(s_list_main_title, 1);

  int index = 0;
  for(; index < __MENU_LABEL_LAST; index++) {
    glDeleteTextures(1, &(s_pre_tex_labels[index]));
    glDeleteTextures(1, &(s_pre_tex_labels_focus[index]));
    glDeleteLists(s_list_labels[index], 1);
    glDeleteLists(s_list_labels_focus[index], 1);
  }

  for(index = 0; index < __MENU_COMPONENT_LAST; index++) {
    glDeleteTextures(1, &(s_pre_tex_components[index]));
    glDeleteTextures(1, &(s_pre_tex_components_focus[index]));
    glDeleteLists(s_list_components[index], 1);
    glDeleteLists(s_list_components_focus[index], 1);
  }

  for(index = 0; index < s_sets_names_length; index++) {
    glDeleteTextures(1, &(s_pre_tex_set_names[index]));
    glDeleteTextures(1, &(s_pre_tex_set_names_focus[index]));
    glDeleteLists(s_list_set_names[index], 1);
    glDeleteLists(s_list_set_names_focus[index], 1);
  }

  for(index = 0; index < s_themes_names_length; index++) {
    glDeleteTextures(1, &(s_pre_tex_theme_names[index]));
    glDeleteTextures(1, &(s_pre_tex_theme_names_focus[index]));
    glDeleteLists(s_list_theme_names[index], 1);
    glDeleteLists(s_list_theme_names_focus[index], 1);
  }

  for(index = 0; index < 10; index++) {
    glDeleteTextures(1, &(s_pre_tex_numbers[index]));
    glDeleteTextures(1, &(s_pre_tex_numbers_focus[index]));
    glDeleteLists(s_list_numbers[index], 1);
    glDeleteLists(s_list_numbers_focus[index], 1);
  }


  GLuint **free_array[] = {
    &s_pre_tex_set_names,
    &s_pre_tex_set_names_focus,
    &s_list_set_names,
    &s_list_set_names_focus,
    &s_pre_tex_theme_names,
    &s_pre_tex_theme_names_focus,
    &s_list_theme_names,
    &s_list_theme_names_focus
  };

  for(index = 0; index < 8; index++) {
    free(*(free_array[index]));
    *(free_array[index]) = NULL;
  }

  s_preloaded = false;
}


static void menu_preload(void) {
  if(s_preloaded) {
    return;
  }

  int index = 0; // used for the many loops in this function

  TTF_Init();

  // beforehand, load the font file to render texts
  const char* font_path = DATA_PATH FONT_NAME;

  TTF_Font *font_28 = TTF_OpenFont(font_path, 28);
  if(!font_28) {
    fprintf(stderr, "Couldn't open font: %s!\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  const char* sec_font_path = DATA_PATH FONT_SEC_NAME;

  TTF_Font *font_20 = TTF_OpenFont(sec_font_path, 20);
  if(!font_20) {
    fprintf(stderr, "Couldn't open font: %s!\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Color text_color = sdlutils_color(255, 255, 255);
  SDL_Color text_focus_color = sdlutils_color(255, 155, 0);

  // first, load the lists (of themes, level sets, levels)
  s_themes_names = theme_enumerate_themes(&s_themes_names_length);
  s_sets_names_length = level_enumerate_levelsets(&s_sets_names, &s_sets_levels);

  GLuint **alloc_array[] = {
    &s_pre_tex_set_names,
    &s_pre_tex_set_names_focus,
    &s_list_set_names,
    &s_list_set_names_focus,
    &s_pre_tex_theme_names,
    &s_pre_tex_theme_names_focus,
    &s_list_theme_names,
    &s_list_theme_names_focus
  };

  int alloc_length_array[] = {
    s_sets_names_length,
    s_sets_names_length,
    s_sets_names_length,
    s_sets_names_length,
    s_themes_names_length,
    s_themes_names_length,
    s_themes_names_length,
    s_themes_names_length
  };

  for(index = 0; index < 8; index++) {
    *(alloc_array[index]) = malloc(sizeof(GLuint) * alloc_length_array[index]);
    if(!*(alloc_array[index])) {
      fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(GLuint) * alloc_length_array[index]);
      exit(EXIT_FAILURE);
    }
  }

  /*
   * pre-render everything
   */
  int w = 0, h = 0;
  int y_offset = 30;

  // pre-render main title
  s_pre_tex_main_title = glutils_load_texture_from_file(DATA_PATH IMG_MAIN_TITLE);

  glutils_get_texture_dim(s_pre_tex_main_title, &w, &h);

  s_list_main_title = glutils_create_list(
    s_pre_tex_main_title,
    w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
  );


  // pre-render menu text labels
  y_offset = 150;
  for(index = 0; index < __MENU_LABEL_LAST; index++) {
    s_pre_tex_labels[index] = glutils_render_text_to_texture(
      font_28, s_str_labels[index], text_color
    );

    glutils_get_texture_dim(s_pre_tex_labels[index], &w, &h);

    s_options_y[index] = y_offset + h; // store y coordinate

    s_list_labels[index] = glutils_create_list(
      s_pre_tex_labels[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );

    // same for focused labels (only the color changes)
    s_pre_tex_labels_focus[index] = glutils_render_text_to_texture(
      font_28, s_str_labels[index], text_focus_color
    );

    s_list_labels_focus[index] = glutils_create_list(
      s_pre_tex_labels_focus[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );

    y_offset += h*2;
  }


  // pre-render menu text components
  for(index = 0; index < __MENU_COMPONENT_LAST; index++) {
    s_pre_tex_components[index] = glutils_render_text_to_texture(
      font_20, s_str_components[index], text_color
    );

    glutils_get_texture_dim(s_pre_tex_components[index], &w, &h);

    switch(index) {
    case COMPONENT_COMPUTER:
    case COMPONENT_HUMAN:
      y_offset = s_options_y[LABEL_SECOND_PLAYER];

      break;

    case COMPONENT_ACTIVE:
      y_offset = 0;
      break;

    case COMPONENT_PLUS:
    case COMPONENT_MINUS:
      y_offset = s_options_y[LABEL_SOUND_VOLUME];
      break;
    }

    s_list_components[index] = glutils_create_list(
      s_pre_tex_components[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );

    // same for focused components (only the color changes)
    s_pre_tex_components_focus[index] = glutils_render_text_to_texture(
      font_20, s_str_components[index], text_focus_color
    );

    s_list_components_focus[index] = glutils_create_list(
      s_pre_tex_components_focus[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );
  }


  // pre-render level set names
  glutils_get_texture_dim(s_pre_tex_labels[LABEL_LEVEL_SET], &w, &h);
  y_offset = s_options_y[LABEL_LEVEL_SET];

  for(index = 0; index < s_sets_names_length; index++) {
    s_pre_tex_set_names[index] = glutils_render_text_to_texture(
      font_20, s_sets_names[index], text_color
    );

    glutils_get_texture_dim(s_pre_tex_set_names[index], &w, &h);

    s_list_set_names[index] = glutils_create_list(
      s_pre_tex_set_names[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );

    s_pre_tex_set_names_focus[index] = glutils_render_text_to_texture(
      font_20, s_sets_names[index], text_focus_color
    );

    s_list_set_names_focus[index] = glutils_create_list(
      s_pre_tex_set_names_focus[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );
  }


  // pre-render theme names
  glutils_get_texture_dim(s_pre_tex_labels[LABEL_THEME], &w, &h);
  y_offset = s_options_y[LABEL_THEME];

  for(index = 0; index < s_themes_names_length; index++) {
    s_pre_tex_theme_names[index] = glutils_render_text_to_texture(
      font_20, s_themes_names[index], text_color
    );

    glutils_get_texture_dim(s_pre_tex_theme_names[index], &w, &h);

    s_list_theme_names[index] = glutils_create_list(
      s_pre_tex_theme_names[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );

    s_pre_tex_theme_names_focus[index] = glutils_render_text_to_texture(
      font_20, s_themes_names[index], text_focus_color
    );

    s_list_theme_names_focus[index] = glutils_create_list(
      s_pre_tex_theme_names_focus[index],
      w, h, WINDOW_WIDTH / 2 - w / 2, y_offset
    );
  }


  // pre-render numbers
  const char *numbers[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

  for(index = 0; index < 10; index++) {
    s_pre_tex_numbers[index] = glutils_render_text_to_texture(
      font_20, numbers[index], text_color
    );

    glutils_get_texture_dim(s_pre_tex_numbers[index], &w, &h);

    s_list_numbers[index] = glutils_create_list(
      s_pre_tex_numbers[index],
      w, h, 0, 0
    );

    s_pre_tex_numbers_focus[index] = glutils_render_text_to_texture(
      font_20, numbers[index], text_focus_color
    );

    s_list_numbers_focus[index] = glutils_create_list(
      s_pre_tex_numbers_focus[index],
      w, h, 0, 0
    );
  }


  TTF_CloseFont(font_28);
  TTF_CloseFont(font_20);

  s_preloaded = true;

  atexit(menu_free);
}


bool menu_run(Settings *settings) {
  assert(settings != NULL);

  SDL_EnableKeyRepeat(10000 / WINDOW_FRAMERATE, 10000 / WINDOW_FRAMERATE);

  glClearColor(0, 0, 0, 1);

  int level_set_index = 0, theme_index = 0;

  menu_preload();

  // default settings
  if(settings->first_run) {
    settings->play_with_computer = true;
    settings->volume = 100;
    strncpy(settings->level_set, s_sets_names[level_set_index], LIM_STR_LEN);
    settings->level = 1;
    strncpy(settings->theme, s_themes_names[theme_index], LIM_STR_LEN);
    settings->ball_speed = BALL_DEFAULT_SPEED;
  }

  Timer timer = timer_new(WINDOW_FRAMERATE);

  const bool* events = NULL;
  bool leave = false;
  bool vol_updated = false;
  while(!leave) {
    events = event_poll();

    if(events[EVENT_QUIT] || events[EVENT_KEY_ESCAPE]) {
      return false;
    }

    if(events[EVENT_KEY_UP]) {
      if(s_focus == 0) {
        s_focus = __MENU_LABEL_LAST - 1;
      } else {
        s_focus--;
      }

      audio_play(SOUND_TOCK);
    }

    if(events[EVENT_KEY_DOWN]) {
      s_focus++;
      audio_play(SOUND_TOCK);

      if(s_focus == __MENU_LABEL_LAST) {
        s_focus = 0;
      }
    }

    if(events[EVENT_KEY_LEFT]) {
      bool action = true;

      switch(s_focus) {
      case LABEL_SECOND_PLAYER:
        settings->play_with_computer = !settings->play_with_computer;
        break;

      case LABEL_SOUND_VOLUME:
        settings->volume -= 10;
        if(settings->volume < 0) {
          settings->volume = 0;
          action = false;
        } else {
          vol_updated = true;
        }
        break;

      case LABEL_LEVEL_SET:
        s_level_set_index--;
        if(s_level_set_index < 0) {
          s_level_set_index = s_sets_names_length - 1;
        }
        settings->level = 1;
        break;

      case LABEL_LEVEL:
        (settings->level)--;
        if(settings->level <= 0) {
          settings->level = s_sets_levels[s_level_set_index];
        }
        break;

      case LABEL_THEME:
        s_theme_index--;
        if(s_theme_index < 0) {
          s_theme_index = s_themes_names_length - 1;
        }
        break;

      case LABEL_BALL_SPEED:
        (settings->ball_speed)--;
        if(settings->ball_speed < BALL_MIN_SPEED) {
          settings->ball_speed = BALL_MAX_SPEED;
        }
        break;

      default:
        action = false;
        break;
      }

      if(action) {
        audio_play(SOUND_TICK);
      }
    }

    if(events[EVENT_KEY_RIGHT]) {
      bool action = true;

      switch(s_focus) {
      case LABEL_SECOND_PLAYER:
        settings->play_with_computer = !settings->play_with_computer;
        break;

      case LABEL_SOUND_VOLUME:
        settings->volume += 10;
        if(settings->volume > 100) {
          settings->volume = 100;
          action = false;
        } else {
          vol_updated = true;
        }
        break;

      case LABEL_LEVEL_SET:
        s_level_set_index++;
        if(s_level_set_index >= s_sets_names_length) {
          s_level_set_index = 0;
        }
        settings->level = 1;
        break;

      case LABEL_LEVEL:
        (settings->level)++;
        if(settings->level > s_sets_levels[s_level_set_index]) {
          settings->level = 1;
        }
        break;

      case LABEL_THEME:
        s_theme_index++;
        if(s_theme_index >= s_themes_names_length) {
          s_theme_index = 0;
        }
        break;

      case LABEL_BALL_SPEED:
        (settings->ball_speed)++;
        if(settings->ball_speed > BALL_MAX_SPEED) {
          settings->ball_speed = BALL_MIN_SPEED;
        }
        break;

      default:
        action = false;
        break;
      }

      if(action) {
        audio_play(SOUND_TICK);
      }
    }

    if(events[EVENT_KEY_ENTER]) {
      if(s_focus == LABEL_QUIT) {
        return false;
      } else {
        leave = true;
      }
    }

    if(vol_updated) {
      audio_set_volume(settings->volume);
      vol_updated = false;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    menu_display(settings);

    SDL_GL_SwapBuffers();

    timer_sleep(&timer);
  }

  strncpy(settings->level_set, s_sets_names[s_level_set_index], LIM_STR_LEN);
  strncpy(settings->theme, s_themes_names[s_theme_index], LIM_STR_LEN);

  settings->first_run = false;

  return true;
}

