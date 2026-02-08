#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int count_pattern_in_line(
    const char* line,
    const char* pattern,
    int verbose
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
                if (verbose) {
                    printf("> > found occurrence at column %i\n", idx_line);
                }
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
    int get_line_numbers,
    int verbose
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
    int line_num = 0;
    while ((nread = getline(&line, &size, stream)) != -1) {
        int line_count = count_pattern_in_line(line, pattern, verbose);
        line_num++;
        count += line_count;
        if (line_count > 0 && get_line_numbers) {
            printf("> found %i occurrences in line %i\n", line_count, line_num);
        }
        else if (verbose) {
            printf("> found %i occurrences in line %i\n", line_count, line_num);
        }
    }

    free(line);
    fclose(stream);

    return count;
}

int main(int argc, char *argv[])
{
    int line_numbers = 0;
    int verbose = 0;
    char* pattern;

    static struct option longopts[] = {
        {"line-number",  no_argument,       0, 'n'},
        {"verbose", no_argument, 0, 'v'},
        {"pattern",      required_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "nve:", longopts, NULL)) != -1) {
        switch (opt) {
            case 'n': line_numbers = 1; break;
            case 'v': verbose = 1; break;
            case 'e': pattern = optarg; break;
            default:
                fprintf(stderr, "Usage: ...\n");
                return 2;
        }
    }

    if (verbose) {
        printf("line_numbers = %d\n", line_numbers);
        printf("pattern = '%s'\n", pattern);
        printf("===\n");
    }
    
    for (int i = optind; i < argc; i++) {
        printf("reading file %s...\n", argv[i]);
        int result = count_pattern_in_file(argv[i], pattern, line_numbers, verbose);
        printf("> TOTAL: %i occurrences\n", result);
    }
    return 0;
}
