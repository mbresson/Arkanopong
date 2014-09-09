
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "glutils.h"
#include "Window.h"


// initialized by glutils_init
static float s_gl_center_x = 0.;
static float s_gl_center_y = 0.;


GLuint glutils_create_list(GLuint tex_id, int w, int h, int x, int y) {
  return glutils_create_list_rec(tex_id, w, h, x, y, 0, 0, w, h);
}


GLuint glutils_create_list_rec(GLuint tex_id, int w, int h, int x, int y, int sx, int sy, int sw, int sh) {
  GLuint list = glGenLists(1);

  glNewList(list, GL_COMPILE);
    glPushMatrix();
      glTranslatef(
        DIMX_TO_GL(x),
        -DIMY_TO_GL(y),
        0
      );

      glutils_render_texture_to_corner_rec(tex_id, w, h, sx, sy, sw, sh);
    glPopMatrix();
  glEndList();

  return list;
}


void glutils_draw_debug_info(void) {
  glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex2f(0, GLUTILS_TOP);
    glVertex2f(0, GLUTILS_BOTTOM);
    glVertex2f(GLUTILS_LEFT, 0);
    glVertex2f(GLUTILS_RIGHT, 0);
  glEnd();
}


void glutils_get_texture_dim(GLuint tex_id, int* w, int* h) {
  assert(w != NULL);
  assert(h != NULL);

  glBindTexture(GL_TEXTURE_2D, tex_id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, h);
  glBindTexture(GL_TEXTURE_2D, 0);
}


void glutils_init(void) {
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(GLUTILS_LEFT, GLUTILS_RIGHT, GLUTILS_BOTTOM, GLUTILS_TOP);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  s_gl_center_x = (GLUTILS_LEFT + GLUTILS_RIGHT) / 2;
  s_gl_center_y = (GLUTILS_TOP + GLUTILS_BOTTOM) / 2;
}


GLuint glutils_load_texture(SDL_Surface* surface) {
  assert(surface != NULL);
 
  GLenum format;
  switch(surface->format->BytesPerPixel) {
    case 1:
      format = GL_RED;
      break;

    case 3:
      if(surface->format->Rmask == 0x000000ff) {
        format = GL_RGB;
      } else {
        format = GL_BGR;
      }
      break;

    case 4:
      if(surface->format->Rmask == 0x000000ff) {
        format = GL_RGBA;
      } else {
        format = GL_BGRA;
      }
      break;

    default:
      fprintf(stderr, "Unsupported format for pixels of surface\n");
      exit(EXIT_FAILURE);
  }

  GLuint tex_id = 0;
  glGenTextures(1, &tex_id);

  glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      surface->w,
      surface->h,
      0,
      format,
      GL_UNSIGNED_BYTE,
      surface->pixels
    );
  glBindTexture(GL_TEXTURE_2D, 0);

  return tex_id;
}


GLuint glutils_load_texture_from_file(const char* path) {
  assert(path != NULL);

  SDL_Surface* image = IMG_Load(path);
  if(!image) {
    fprintf(stderr, "Error loading '%s': %s\n", path, IMG_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_SetAlpha(image, SDL_SRCALPHA, 0);

  GLuint tex = glutils_load_texture(image);

  SDL_FreeSurface(image);

  return tex;
}


GLuint glutils_render_text_to_texture(TTF_Font* font, const char* text, SDL_Color color) {
  assert(font != NULL);
  assert(text != NULL);

  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);

  GLuint tex_id = glutils_load_texture(surface);

  SDL_FreeSurface(surface);

  return tex_id;
}


void glutils_render_texture_to_corner(GLuint tex_id, int w, int h) {
  glutils_render_texture_to_corner_rec(tex_id, w, h, 0, 0, w, h);
}

void glutils_render_texture_to_corner_rec(GLuint tex_id, int w, int h, int sx, int sy, int sw, int sh) {
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  float gl_topleft_x = X_TO_GL(0), gl_topleft_y = Y_TO_GL(0);

  float gl_w = DIMX_TO_GL(w), gl_h = DIMY_TO_GL(h);

  if(w != sw) {
    gl_w = DIMX_TO_GL(sw);
  }

  if(h != sh) {
    gl_h = DIMY_TO_GL(sh);
  }

  float gl_sw = sw / (float)w, gl_sh = sh / (float)h;
  float gl_sx = sx / (float)w, gl_sy = sy / (float)h;

  glBegin(GL_QUADS);
    glTexCoord2f(gl_sx, gl_sy); // top-left corner
    glVertex2f(gl_topleft_x, gl_topleft_y);

    glTexCoord2f(gl_sx + gl_sw, gl_sy); // top-right corner
    glVertex2f(gl_topleft_x + gl_w, gl_topleft_y);

    glTexCoord2f(gl_sx + gl_sw, gl_sy + gl_sh); // bottom-right corner
    glVertex2f(gl_topleft_x + gl_w, gl_topleft_y - gl_h);

    glTexCoord2f(gl_sx, gl_sy + gl_sh); // bottom-left corner
    glVertex2f(gl_topleft_x, gl_topleft_y - gl_h);
  glEnd();

  glBindTexture(GL_TEXTURE_2D, 0);
}



