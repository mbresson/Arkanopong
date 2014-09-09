
#ifndef INC_AUDIO_
#define INC_AUDIO_


#define AUDIO_DEFAULT_VOLUME 50


typedef enum {
  SOUND_HIT,
  SOUND_PADDLE_ENLARGE,
  SOUND_PADDLE_SHRINK,
  SOUND_HEART_PLUS,
  SOUND_HEART_MINUS,
  SOUND_BALL_SPAWN,
  SOUND_SHIELD_ON,
  SOUND_SHIELD_OFF,
  SOUND_TICK,
  SOUND_TOCK,

  __AUDIO_OBJECT_LAST
} Audio_Object;


/**
 * initializes the audio system, must be run before playing anything
 * this function is automatically called by window_open
 */
extern void audio_init(void);


/**
 * @param object_id
 * one of the SOUND_ constants
 */
extern void audio_play(Audio_Object object_id);


/**
 * @param volume comprised between 0 and 100
 */
extern void audio_set_volume(int volume);


#endif

