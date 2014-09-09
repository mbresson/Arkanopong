
#ifndef INC_SETTINGS_
#define INC_SETTINGS_

#include "bool.h"
#include "limits.h"

typedef struct {
  // = true if the menu is opened for the first time (then the struct is filled with unreliable values)
  bool first_run;
  bool play_with_computer;
  int volume;

  char level_set[LIM_STR_LEN];
  int level;

  int ball_speed;

  char theme[LIM_STR_LEN];

} Settings;

#endif

