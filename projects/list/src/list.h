#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#define VERSION "1.0.0"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ERROR_GENERIC 1
#define ERROR_INVALID_OPTION 2
#define ERROR_NO_DIRECTORIES_GIVEN 3
#define ERROR_DIR_OPEN 4
#define ERROR_DIR_CLOSE 5
#define ERROR_FILE_OPEN 6
#define ERROR_FREAD 7
#define ERROR_MALLOC 8

void print_usage(void);

void read_file(const char*, int);

void read_directory(const char*, int, int, int);

int main(int, char*[]);
