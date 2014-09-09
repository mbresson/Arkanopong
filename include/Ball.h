
#ifndef INC_BALL_
#define INC_BALL_

#define BALL_DEFAULT_SPEED 5
#define BALL_MIN_SPEED 1

#define BALL_MAX_SPEED 10

typedef enum {
  BALL_NORMAL,
  BALL_BONUS,

  __BALL_TYPE_LAST
} Ball_Type;

typedef struct {
  // the player who last touched the ball, for bonus attribution
  int last_player;
  int type;
  int x, y;
  int x_speed, y_speed;
} Ball;

typedef struct Ball_list Ball_list;


extern void ball_append_to_list(Ball_list* list, Ball ball);

extern void ball_free_list(Ball_list **list);

extern int ball_get_default_speed(void);

extern Ball* ball_get_from_list(Ball_list* list, int index);

extern int ball_list_length(Ball_list *list);

// creates a new immobile ball
extern Ball ball_new(Ball_Type type, int x, int y, int player);

extern Ball_list* ball_new_list(void);

extern void ball_remove_from_list(Ball_list *list, int index);

extern void ball_set_default_speed(int speed);

#endif

