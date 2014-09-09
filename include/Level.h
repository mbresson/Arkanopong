
#ifndef INC_LEVEL_
#define INC_LEVEL_

#include "bool.h"
#include "limits.h"
#include "Settings.h"
#include "Theme.h"
#include "Block.h"
#include "Window.h"


typedef struct {
  int width, height;

  Block_Type blocks[LIM_LEVEL_HEIGHT][LIM_LEVEL_WIDTH];
} Level;


/**
 * @param sets_names
 * will point to an array of names of available level sets read from file data/levels/sets.list
 *
 * @param sets_nums_of_levels
 * will point to an array containing the number of levels in each level set
 *
 * @return
 * the length of the two arrays
 */
extern int level_enumerate_levelsets(char*** sets_names, int** sets_nums_of_levels);


/**
 * @param sets_enum
 * the same as the one allocated by level_enumerate_levelsets, will be freed and set to NULL
 *
 * @param levels_enum
 * the same as the one allocated by level_enumerate_levelsets, will be freed and set to NULL
 */
extern void level_free_enumeration(char*** sets_enum, int** levels_enum, int length);


/**
 * @param levelset
 * the name of the set in which is contained the level
 *
 * @param id
 * the level number
 *
 * @param level
 * a structure which will be filled with the level's information
 */
extern bool level_load(const char* levelset, int id, Level* level);


/**
 * @param level
 * must be filled with correct level information
 */
extern void level_run(Level* level, const Settings* settings);


#endif

