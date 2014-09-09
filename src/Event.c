
#include <SDL/SDL.h>

#include "Event.h"

/*
 * an array of events
 * each one is either true or false depending on the user input
 */
static bool s_events[__EVENT_TYPE_LAST];


static void reset_events(void) {
  int index = 0;
  for(; index < __EVENT_TYPE_LAST; index++) {
    s_events[index] = false;
  }
}


const bool* event_poll(void) {
	reset_events();

	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
    
    case SDL_QUIT:
      s_events[EVENT_QUIT] = true;
      break;

    case SDL_ACTIVEEVENT:
      if(event.active.state & SDL_APPINPUTFOCUS || event.active.state & SDL_APPACTIVE) {
        if(!event.active.gain) {
          s_events[EVENT_PAUSE] = true;
        } else {
          s_events[EVENT_RESUME] = true;
        }
      }
      break;

		case SDL_KEYDOWN:

				; SDLKey code = event.key.keysym.sym;

        switch(code) {

        case SDLK_LEFT:
          s_events[EVENT_KEY_LEFT] = true;
          break;

        case SDLK_RIGHT:
          s_events[EVENT_KEY_RIGHT] = true;
          break;

        case SDLK_a:
          s_events[EVENT_KEY_A] = true;
          break;

        case SDLK_z:
          s_events[EVENT_KEY_Z] = true;
          break;

        case SDLK_s:
          s_events[EVENT_KEY_S] = true;
          break;

        case SDLK_UP:
          s_events[EVENT_KEY_UP] = true;
          break;

        case SDLK_DOWN:
          s_events[EVENT_KEY_DOWN] = true;
          break;

        case SDLK_ESCAPE:
          s_events[EVENT_KEY_ESCAPE] = true;
          break;

        case SDLK_RETURN:
          s_events[EVENT_KEY_ENTER] = true;
          break;

        default: break;
        }
    }
  }

	return s_events;
}

