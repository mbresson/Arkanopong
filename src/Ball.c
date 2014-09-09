
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "Ball.h"
#include "limits.h"
#include "mathutils.h"
#include "Theme.h"
#include "Window.h"


static int s_default_speed = BALL_MIN_SPEED;


struct Ball_list {
  Ball ball;
  Ball_list *next;
};


void ball_append_to_list(Ball_list* list, Ball ball) {
  assert(list != NULL);

  Ball_list *current = list;
  while(current->next != NULL) {
    current = current->next;
  }

  Ball_list *new = malloc(sizeof(Ball_list));
  if(!new) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(Ball_list));
    exit(EXIT_FAILURE);
  }

  new->next = NULL;
  new->ball = ball;

  current->next = new;
}


void ball_free_list(Ball_list **list) {
  assert(list != NULL);
  assert(*list != NULL);

  Ball_list *current = *list;
  do {
    Ball_list *tmp = current->next;
    free(current);

    current = tmp;
  } while(current != NULL);

  *list = NULL;
}


int ball_get_default_speed(void) {
  return s_default_speed;
}


Ball* ball_get_from_list(Ball_list* list, int index) {
  assert(list != NULL);
  assert(index >= 0);
  assert(index < ball_list_length(list));

  Ball_list *current = list->next;
  int it = 0;
  while(it < index) {
    current = current->next;
    it++;
  }

  return &(current->ball);
}


int ball_list_length(Ball_list *list) {
  assert(list != NULL);

  int length = 0;
  Ball_list *current = list->next;

  while(current != NULL) {
    length++;
    current = current->next;
  }

  return length;
}


Ball ball_new(Ball_Type type, int x, int y, int player) {
  assert(type < __BALL_TYPE_LAST);
  assert(player == 0 || player == 1);

  Ball ball = {
    player, // last_player
    type,
    x, y, // x, y
    0, 0 // x_speed, y_speed
  };

  return ball;
}

Ball_list* ball_new_list(void) {
  Ball_list *list = malloc(sizeof(Ball_list));
  if(!list) {
    fprintf(stderr, "Couldn't allocate %zu bytes!\n", sizeof(Ball_list));
    exit(EXIT_FAILURE);
  }

  list->next = NULL;
  return list;
}

void ball_remove_from_list(Ball_list *list, int index) {
  assert(list != NULL);
  assert(index >= 0);
  assert(index < ball_list_length(list));

  Ball_list *current = list->next, *previous = list;
  int it = 0;
  while(it != index) {
    previous = current;
    current = current->next;
    it++;
  }

  previous->next = current->next;
  free(current);
}


void ball_set_default_speed(int speed) {
  s_default_speed = speed;
}

