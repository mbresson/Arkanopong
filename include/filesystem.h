
#ifndef INC_FILESYSTEM_
#define INC_FILESYSTEM_

#include <stdio.h>


#ifndef ROOT_PATH
#define ROOT_PATH "./"

#define DATA_PATH ROOT_PATH "data/"
#endif

#ifndef DATA_PATH
#define DATA_PATH ROOT_PATH "data/"
#endif


/**
 * @return
 * an int containing the number of lines in the file
 * empty lines (containing only spaces or EOF) are skipped
 */
extern int filesystem_count_lines(FILE* file);


/**
 * @param path
 * an uncomplete file path with the extension (e.g. '.png') missing
 *
 * @param extensions
 * a list of possible extensions (e.g. '.png', '.jpg', etc)
 *
 * @return
 * the index of the correct extension
 * if there is no correct extension, returns -1
 *
 * this function tries to open the file with each given extension
 * it stops at the first extension successfully opened
 */
extern int filesystem_detect_extension(const char* path, const char** extensions, int length);


#endif

