#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int count_pattern_in_line(
    const char* line,
    const char* pattern
) {
    int len_line = strlen(line);
    int len_pattern = strlen(pattern);

    if (len_pattern > len_line) return 0;

    int idx_line = 0;
    int idx_pattern = 0;
    int len_equal = 0;
    int count = 0;

    while (idx_line < len_line) {
        if (line[idx_line] == pattern[idx_pattern]) {
            len_equal++;
            idx_pattern++;

            if (len_equal == len_pattern) {
                count++;
                len_equal = 0;
            }
        }
        else {
            idx_pattern = 0;
            len_equal = 0;
        }
        
        idx_line++;
    }

    return count;
}

int count_pattern_in_file(
    const char* file,
    const char* pattern,
    int get_line_numbers
) {
    FILE* stream;
    char* line = NULL;
    size_t size = 0;
    ssize_t nread;

    stream = fopen(file, "r");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    while ((nread = getline(&line, &size, stream)) != -1) {
        count += count_pattern_in_line(line, pattern);
    }

    free(line);
    fclose(stream);

    return count;
}

int main(int argc, char *argv[])
{
    int line_numbers = 0;
    char* pattern;

    static struct option longopts[] = {
        {"line-number",  no_argument,       0, 'n'},
        {"pattern",      required_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "ne:", longopts, NULL)) != -1) {
        switch (opt) {
            case 'n': line_numbers = 1; break;
            case 'e': pattern = optarg; break;
            default:
                fprintf(stderr, "Usage: ...\n");
                return 2;
        }
    }

    printf("line_numbers = %d\n", line_numbers);
    printf("===\n");
    for (int i = optind; i < argc; i++) {
        printf("reading file %s...\n", argv[i]);
        int result = count_pattern_in_file(argv[i], pattern, line_numbers);
        printf("found %i occurrences\n", result);
    }
    return 0;
}
