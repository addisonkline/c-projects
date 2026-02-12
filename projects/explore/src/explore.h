#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

#define VERSION "1.0.0"

#define ILLEGAL_OPTION 2
#define DIRECTORY_OPEN_FAILURE 3
#define DIRECTORY_CLOSE_FAILURE 4
#define NO_FILENAME_GIVEN 5
#define NO_DIRECTORY_GIVEN 6
#define INVALID_LOOKUP_CODE 7
#define OUTFILE_OPEN_FAILURE 8

void print_usage(void);

void lookup_exit_code(const char*);

void write_find_to_file(const char*, const char*, const char*, int);

void check_directory(const char*, const char*, const char*, int);

int main(int, char**);
