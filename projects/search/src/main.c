#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#define VERSION "1.0.0"

void print_usage(void) {
    printf("usage: search [OPTION]... LITERAL [FILE]...\n");
    printf("\nSearch for a string literal in the given file(s)\n");
    printf("\nOptions:\n");
    printf("    -n, --line-number: include line numbers for each literal found\n");
    printf("    -v, --verbose: print more detailed search info\n");
    printf("    -h, --help: show this message and exit\n");
    printf("    -V, --version: show the program version and exit\n");
    printf("    -o OUTFILE, --outfile OUTFILE: write the search results to the specified file\n");
    printf("\nPositionals:\n");
    printf("    LITERAL: the string literal to search for\n");
    printf("    FILE...: the file(s) to search\n");
}

void write_find_to_file(
    const char* file,
    const char* write_file,
    int line_num,
    int col_num
) {
    FILE* stream;
    stream = fopen(write_file, "a");
    if (stream == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // 1. write the filename
    int len_file = strlen(file);
    fwrite(file, len_file, 1, stream);
    // 1.5. write a colon
    fwrite(":", 1, 1, stream);
    // 2. write the line number
    int line_num_strlen = (int)((floor(log10(line_num))+1)*sizeof(char));
    char line_num_str[line_num_strlen];
    snprintf(line_num_str, line_num_strlen + 1, "%d", line_num);
    fwrite(line_num_str, line_num_strlen, 1, stream);
    // 2.5. write a colon
    fwrite(":", 1, 1, stream);
    // 3. write the column number
    int col_num_strlen = (int)((floor(log10(col_num))+1)*sizeof(char));
    char col_num_str[col_num_strlen];
    snprintf(col_num_str, col_num_strlen + 1, "%d", col_num);
    fwrite(col_num_str, col_num_strlen, 1, stream);
    // 3.5. write the final newline
    fwrite("\n", 1, 1, stream);
    
    fclose(stream);
}

int count_pattern_in_line(
    const char* line,
    const char* pattern,
    const char* file,
    const char* outfile,
    int line_num,
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
                if (outfile != NULL) {
                    write_find_to_file(file, outfile, line_num, idx_line);
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
    const char* outfile,
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
        int line_count = count_pattern_in_line(line, pattern, file, outfile, line_num, verbose);
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
    char* outfile;
    char* pattern;

    static struct option longopts[] = {
        {"line-number",  no_argument,       0, 'n'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {"outfile", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "nvhVo:", longopts, NULL)) != -1) {
        switch (opt) {
            case 'n': line_numbers = 1; break;
            case 'v': verbose = 1; break;
            case 'o':
                outfile = optarg;
                break;
            case 'V': 
                printf("%s\n", VERSION);
                return 0;
            case 'h':
                print_usage();
                return 0;
            default:
                fprintf(stderr, "Usage: ...\n");
                return 2;
        }
    }

    // extract literal (pattern)
    pattern = argv[optind];
    optind++;

    if (verbose) {
        printf("line_numbers = %d\n", line_numbers);
        printf("verbose = %d\n", verbose);
        printf("pattern = '%s'\n", pattern);
        printf("outfile = '%s'\n", outfile);
        printf("===\n");
    }
    
    for (int i = optind; i < argc; i++) {
        printf("reading file %s...\n", argv[i]);
        int result = count_pattern_in_file(argv[i], pattern, outfile, line_numbers, verbose);
        printf("> TOTAL: %i occurrences\n", result);
    }
    return 0;
}
