
#ifndef INC_EVENT_
#define INC_EVENT_

#include "bool.h"


typedef enum {
  EVENT_QUIT, // e.g. when trying to close the window
  EVENT_PAUSE, // when window loses focus
  EVENT_RESUME, // when window gains focus

  EVENT_KEY_ENTER,
  EVENT_KEY_LEFT,
  EVENT_KEY_RIGHT,
  EVENT_KEY_A,
  EVENT_KEY_Z,
  EVENT_KEY_S, // used by player 2 to launch the ball

  EVENT_KEY_UP,
  EVENT_KEY_DOWN,

  EVENT_KEY_ESCAPE,

  __EVENT_TYPE_LAST
} Event_Type;


/**
 * @return
 * a statically-allocated array whose length == __EVENT_TYPE_LAST
 * containing boolean values for each element in Event_Type enum
 * if the value is true, then the key is pressed
 */
extern const bool* event_poll(void);


#endif

