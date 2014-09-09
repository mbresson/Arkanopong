
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filesystem.h"
#include "limits.h"
#include "stringutils.h"


int filesystem_count_lines(FILE* file) {
  assert(file != NULL);

  rewind(file);

  char buffer[LIM_STR_LEN];

  int lines = 0;
  while(fgets(buffer, LIM_STR_LEN, file) != NULL) {
    if(!stringutils_is_empty_line(buffer)) {
      lines++;
    }
  }

  rewind(file);

  return lines;
}


int filesystem_detect_extension(const char* path, const char** extensions, int length) {
  assert(path != NULL);
  assert(extensions != NULL);
  assert(length > 0);

  FILE* test = NULL;
  int index = 0;

  char final_path[LIM_STR_LEN];
  strncpy(final_path, path, LIM_STR_LEN);
  int strlength = strlen(final_path);
  char *pointer = strrchr(final_path, '\0');

  while(index < length) {
    strncpy(pointer, extensions[index], LIM_STR_LEN - strlength);

    test = fopen(final_path, "r");
    if(test != NULL) {
      fclose(test);
      return index;
    }

    index++;
  }

  return -1;
}

