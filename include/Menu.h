
#ifndef INC_MENU_
#define INC_MENU_

#include "bool.h"
#include "Settings.h"


/**
 * @param settings
 * a pointer to a Settings structure filled by the function
 *
 * @return
 * True if a game is started, false if the user chose to quit.
 */
extern bool menu_run(Settings *settings);

#endif

