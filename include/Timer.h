
#ifndef INC_TIMER_
#define INC_TIMER_

#include <SDL/SDL.h>

typedef struct {
  Uint32 start;
  const Uint32 ms;
} Timer;


/**
 * @return
 * the number of milliseconds needed to sleep before the next tick
 */
extern Uint32 timer_get_delay(Timer* timer);


/**
 * @param freq
 * how many times per second the timer should tick
 */
extern Timer timer_new(int freq);


/**
 * reset timer's start time
 */
extern void timer_reset(Timer* timer);


/**
 * sleep as long as needed to the next tick
 */
extern void timer_sleep(Timer* timer);


#endif

