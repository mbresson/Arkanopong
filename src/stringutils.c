
#include <stdio.h>

#include "stringutils.h"


// returns 1 if the line contains only spaces, tabulations, \n or #
int stringutils_is_empty_line(const char* line) {
  int index = 0;
  while(line[index] != '\0') {
    if(
      line[index] != ' ' &&
      line[index] != '\n' &&
      line[index] != '\t' &&
      line[index] != '#'
    ) {
      return 0;
    }

    if(line[index] == '#') {
      return 1;
    }

    index++;
  }

  return 1;
}

