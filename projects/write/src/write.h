#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define ILLEGAL_OPTION 2
#define NO_CONTENT_GIVEN 3
#define NO_FILES_GIVEN 4
#define FILE_OPEN_FAILURE 5

#define VERSION "1.0.0"

void print_usage(void);

void write_file(const char*, const char*);

int main(int, char**);
