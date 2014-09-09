
#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "glutils.h"
#include "Window.h"
#include "Settings.h"
#include "Menu.h"
#include "Theme.h"
#include "Level.h"
#include "limits.h"

int main(void) {
  window_open();

  glutils_init();

  Settings settings;
  settings.first_run = true;
  Level level;

  bool leave = false;
  while(!leave) {
    if(!menu_run(&settings)) {
      leave = true;
      break;
    }

    theme_load(settings.theme);
    level_load(settings.level_set, settings.level, &level);

    level_run(&level, &settings);
  }

  /*
   * unload the last used theme
   * if we loaded several themes one after another,
   * they have been automatically freed by theme_load
   */
  theme_unload();

  return EXIT_SUCCESS;
}

