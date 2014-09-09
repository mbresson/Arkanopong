
#ifndef INC_GLUTILS_
#define INC_GLUTILS_

#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "mathutils.h"

#define GLUTILS_LEFT -1.0
#define GLUTILS_RIGHT 1.0
#define GLUTILS_BOTTOM -1.0
#define GLUTILS_TOP 1.0

/*
 * convert a coordinate (X or Y)
 * from the window's coordinate system (0-WINDOW_WIDTH and 0-WINDOW_HEIGHT)
 * to OpenGL's system (GLUTILS_LEFT-GLUTILS_RIGHT, GLUTILS_TOP-GLUTILS_RIGHT)
 */
#define X_TO_GL(val) (CONVERT((val), 0, WINDOW_WIDTH, GLUTILS_LEFT, GLUTILS_RIGHT))
#define Y_TO_GL(val) (CONVERT((val), 0, WINDOW_HEIGHT, GLUTILS_TOP, GLUTILS_BOTTOM))
#define DIMX_TO_GL(val) ((val)/(float)WINDOW_WIDTH*(ABS(GLUTILS_LEFT) + ABS(GLUTILS_RIGHT)))
#define DIMY_TO_GL(val) ((val)/(float)WINDOW_HEIGHT*(ABS(GLUTILS_TOP) + ABS(GLUTILS_BOTTOM)))


/**
 * create a list with tex_id rendered according to desired coordinates
 */
extern GLuint glutils_create_list(GLuint tex_id, int w, int h, int x, int y);


/**
 * create a list with tex_id rendered according to desired coordinates
 * only part of the texture (defined by sx, sy, sw, sh) is rendered
 */
extern GLuint glutils_create_list_rec(GLuint tex_id, int w, int h, int x, int y, int sx, int sy, int sw, int sh);


/**
 * draw horizontal and vertical axes crossing at the center of the window
 */
extern void glutils_draw_debug_info(void);


/**
 * retrieves texture's width and height
 */
extern void glutils_get_texture_dim(GLuint tex_id, int* w, int* h);


/**
 * initializes the OpenGL system, must be run before rendering anything
 */
extern void glutils_init(void);


/**
 * loads a texture from a SDL Surface
 */
extern GLuint glutils_load_texture(SDL_Surface* surface);


/**
 * loads a texture from the hard drive and copies it to the graphics card
 */
extern GLuint glutils_load_texture_from_file(const char* path);


/**
 * renders text as a texture
 */
extern GLuint glutils_render_text_to_texture(TTF_Font* font, const char* text, SDL_Color color);


/**
 * renders a texture in a rectangle defined by its width and height
 * the top-left corner of the texture is positioned to the top-left corner of the window
 */
extern void glutils_render_texture_to_corner(GLuint tex_id, int w, int h);


/**
 * @param sx, sy, sw, sh
 * the coordinates and size of the rectangle selection
 *
 * renders part of texture in a rectangle defined by its width and height
 * the top-left corner of the texture is positioned to the top-left corner of the window
 */
extern void glutils_render_texture_to_corner_rec(GLuint tex_id, int w, int h, int sx, int sy, int sw, int sh);


#endif

