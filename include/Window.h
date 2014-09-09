
#ifndef INC_WINDOW_
#define INC_WINDOW_

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_FRAMERATE 60u


/**
 * create the window with the associated OpenGL context
 * if the window cannot be opened, exits with an error message
 */
extern void window_open(void);


#endif

