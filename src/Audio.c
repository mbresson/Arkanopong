
#include <assert.h>
#include <stdlib.h>
#include <SDL/SDL_mixer.h>


#include "Audio.h"
#include "assets.h"
#include "filesystem.h"
#include "bool.h"
#include "mathutils.h"
#include "limits.h"


static int s_volume = AUDIO_DEFAULT_VOLUME;
static bool s_active = false;

static Mix_Chunk *s_sounds[__AUDIO_OBJECT_LAST];

static char s_sounds_path[][LIM_STR_LEN] = {
  AUDIO_HIT,
  AUDIO_ENLARGE,
  AUDIO_SHRINK,
  AUDIO_HEART_PLUS,
  AUDIO_HEART_MINUS,
  AUDIO_SPAWN_BALL,
  AUDIO_SHIELD_ON,
  AUDIO_SHIELD_OFF,
  AUDIO_TICK,
  AUDIO_TOCK
};


static void audio_quit(void) {
  int index = 0;
  for(; index < __AUDIO_OBJECT_LAST; index++) {
    Mix_FreeChunk(s_sounds[index]);
    s_sounds[index] = NULL;
  }

  Mix_CloseAudio();
  Mix_Quit();
}


static void audio_preload(void) {
  int index = 0;

  char path[LIM_STR_LEN] = DATA_PATH;
  char *path_end = strchr(path, '\0');

  for(; index < __AUDIO_OBJECT_LAST; index++) {
    strcpy(path_end, s_sounds_path[index]);
    s_sounds[index] = Mix_LoadWAV(path);
    if(!s_sounds[index]) {
      fprintf(stderr, "Couldn't load sound: %s!\nDisabling sound.\n", Mix_GetError());
      s_active = false;
      return;
    }
  }
}


void audio_init(void) {
  // init mixer, we need only to read WAV files
  Mix_Init(0);

  int audio_rate = 44100;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;

  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
    fprintf(stderr, "Unable to initialize audio: %s\nDisabling audio.\n", Mix_GetError());
    s_active = false;
    Mix_Quit();
    return;
  }

  audio_preload();
  audio_set_volume(s_volume);

  atexit(audio_quit);
  s_active = true;
}


void audio_play(Audio_Object object_id) {
  assert(object_id >= 0 && object_id < __AUDIO_OBJECT_LAST);

  if(!s_active) {
    return;
  }

  Mix_PlayChannel(-1, s_sounds[object_id], 0);
}


void audio_set_volume(int volume) {
  assert(volume >= 0 && volume <= 100);

  s_volume = volume;

  Mix_Volume(
    -1,
    CONVERT(volume, 0, 100, 0, 128)
  );
}

