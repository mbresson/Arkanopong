
#ifndef INC_THEME_
#define INC_THEME_

#include "Block.h"

#define THEME_BLOCK_WIDTH 80
#define THEME_BLOCK_HEIGHT 30
#define THEME_BALL_WIDTH 20
#define THEME_BALL_HEIGHT 20
#define THEME_HEALTH_WIDTH 30
#define THEME_HEALTH_HEIGHT 30
#define THEME_BONUS_WIDTH 60
#define THEME_BONUS_HEIGHT 30
#define THEME_PADDLE_WIDTH 20
#define THEME_PADDLE_HEIGHT 20
#define THEME_SHIELD_HEIGHT 15
#define THEME_SHIELD_WIDTH 30

// top = 0, left = 0, bottom = 180, right = 150
#define THEME_BLOCK_NEUTRAL_X 0
#define THEME_BLOCK_NEUTRAL_Y 0
#define THEME_BALL_MAIN_X (95 - THEME_BALL_WIDTH/2)
#define THEME_BALL_MAIN_Y (15 - THEME_BALL_HEIGHT / 2)
#define THEME_BALL_NEUTRAL_X (125 - THEME_BALL_WIDTH/2)
#define THEME_BALL_NEUTRAL_Y (15 - THEME_BALL_HEIGHT/2)

#define THEME_BLOCK_BONUS_X 0
#define THEME_BLOCK_BONUS_Y 30
#define THEME_PADDLE_1_X (95 - THEME_PADDLE_WIDTH / 2)
#define THEME_PADDLE_1_Y (45 - THEME_PADDLE_HEIGHT / 2)
#define THEME_PADDLE_2_X (125 - THEME_PADDLE_WIDTH / 2)
#define THEME_PADDLE_2_Y (45 - THEME_PADDLE_HEIGHT / 2)

#define THEME_HEALTH_1_X 0
#define THEME_HEALTH_1_Y 60
#define THEME_HEALTH_2_X 30
#define THEME_HEALTH_2_Y 60
#define THEME_BLOCK_SHIELD_X 80
#define THEME_BLOCK_SHIELD_Y 60

#define THEME_BLOCK_SHRINK_X 0
#define THEME_BLOCK_SHRINK_Y 90
#define THEME_BLOCK_ENLARGE_X 80
#define THEME_BLOCK_ENLARGE_Y 90

#define THEME_BLOCK_HEART_PLUS_X 0
#define THEME_BLOCK_HEART_PLUS_Y 120
#define THEME_BLOCK_HEART_MINUS_X 80
#define THEME_BLOCK_HEART_MINUS_Y 120

#define THEME_BLOCK_BONUSBALL_X 0
#define THEME_BLOCK_BONUSBALL_Y 150
#define THEME_SHIELD_100_X 80
#define THEME_SHIELD_100_Y 150
#define THEME_SHIELD_75_X 110
#define THEME_SHIELD_75_Y 150
#define THEME_SHIELD_50_X 80
#define THEME_SHIELD_50_Y 165
#define THEME_SHIELD_25_X 110
#define THEME_SHIELD_25_Y 165


typedef enum {
  THEME_BLOCK_NEUTRAL,
  THEME_BLOCK_BONUS,
  THEME_BLOCK_SHIELD,
  THEME_BLOCK_SHRINK,
  THEME_BLOCK_ENLARGE,
  THEME_BLOCK_HEART_PLUS,
  THEME_BLOCK_HEART_MINUS,
  THEME_BLOCK_BALL,
  THEME_BLOCK_EMPTY,

  __THEME_BLOCK_LAST
} Theme_Block;

typedef enum {
  THEME_BALL_MAIN,
  THEME_BALL_NEUTRAL,

  __THEME_BALL_LAST
} Theme_Ball;

typedef enum {
  THEME_SHIELD_100,
  THEME_SHIELD_75,
  THEME_SHIELD_50,
  THEME_SHIELD_25, // almost completely broken

  __THEME_SHIELD_LAST
} Theme_Shield;


/**
 * draw current theme's background
 */
extern void theme_draw_background(void);


/**
 * draw a ball
 */
extern void theme_draw_ball(int x, int y, Theme_Ball ball);


/**
 * draw a block
 */
extern void theme_draw_block(int x, int y, Block_Type block);


/**
 * draw the health bar for one player (0 or 1)
 */
extern void theme_draw_health(int player, int health);


/**
 * @param width
 * in pixels, must be a multiple of THEME_PADDLE_WIDTH
 *
 * draw the paddle for one player (0 or 1)
 */
extern void theme_draw_paddle(int player, int x, unsigned int width);


/**
 * draw the shield for the one player (0 or 1)
 */
extern void theme_draw_shield(int player, Theme_Shield state);


/**
 * @param lines
 * will be used to store the length of the array
 *
 * @return
 * an array allocated on the heap (must be freed after use)
 * it contains a list of available themes read from file data/themes/themes.list
 */
extern char** theme_enumerate_themes(int* lines);


/**
 * @param enumeration
 * the result of theme_enumerate_themes, will be freed and set to NULL
 */
extern void theme_free_enumeration(char*** enumeration, int length);


/**
 * loads the theme file for later use
 * if the theme cannot be loaded, exits with an error message
 */
extern void theme_load(const char* name);


extern void theme_unload(void);


#endif

