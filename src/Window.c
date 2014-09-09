
#include <assert.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "Audio.h"
#include "Window.h"
#include "glutils.h"

#define BITS_PER_PIXEL 32


static int s_window_opened = 0;


void window_open(void) {
  assert(!s_window_opened);

  if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL initialization failed!\n");
    exit(EXIT_FAILURE);
  }

  atexit(SDL_Quit);

  if(!SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BITS_PER_PIXEL, SDL_OPENGL)) {
    fprintf(stderr, "Window couldn't be opened!\n");
    exit(EXIT_FAILURE);
  }

  SDL_WM_SetCaption("Arkanopong", NULL);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(GLUTILS_LEFT, GLUTILS_RIGHT, GLUTILS_BOTTOM, GLUTILS_TOP);

  s_window_opened = 1;

  audio_init();
}

