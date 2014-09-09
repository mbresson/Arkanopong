
#ifndef INC_STRINGUTILS
#define INC_STRINGUTILS

/**
 * @return
 * an int containing the number of lines in the file
 * empty lines (containing only spaces or EOF) are skipped
 * lines starting with a # (for comments) are skipped too
 */
extern int stringutils_is_empty_line(const char* line);

#endif

