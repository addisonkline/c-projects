#define _DEFAULT_SOURCE

#include "explore.h"

void print_usage(void) {
    printf("usage: explore [OPTION]... FILENAME [DIRECTORY]...\n");
    printf("\nSearch for a file by name in the given directory (or directories)\n");
    printf("\nOptions:\n");
    printf("    -v, --verbose                : print more detailed search info\n");
    printf("    -r, --recursive              : recursively search the given directory (or directories)\n");
    printf("    -o OUTFILE, --outfile OUTFILE: write the search results to the specified file\n");
    printf("    -l CODE, --lookup CODE       : determine the meaning of a non-zero status code and exit\n");
    printf("    -h, --help                   : show this message and exit\n");
    printf("    -V, --version                : show the program version and exit\n");
    printf("\nPositionals:\n");
    printf("    FILENAME    : the filename to search for\n");
    printf("    DIRECTORY...: the directory (or directories) to search in\n");
    printf("\nCopyright (c) 2026 Addison Kline (GitHub: @addisonkline)\n");
}

void lookup_exit_code(
    const char* exit_code
) {
    int code = atoi(exit_code);
    if (code == 0) {
        printf("invalid exit code\n");
        exit(INVALID_LOOKUP_CODE);
    }

    switch (code) {
        case 1:
            printf("1: nonspecific error\n");
            printf("The program has failed for a nonspecific reason.\n");
            break;
        case 2:
            printf("2: ILLEGAL_OPTION\n");
            printf("The program was given an unrecognized option or argument.\n");
            printf("Run with '-h' or '--help' to see available options.\n");
            break;
        case 3:
            printf("3: DIRECTORY_OPEN_FAILURE\n");
            printf("The program was unable to open the given directory by name.\n");
            printf("Ensure all given directory values are valid paths.\n");
            break;
        case 4:
            printf("4: DIRECTORY_CLOSE_FAILURE\n");
            printf("The program was unable to close a given directory after opening it.\n");
            break;
        case 5:
            printf("5: NO_FILENAME_GIVEN\n");
            printf("The program was not given a filename to search for.\n");
            printf("Run with '-h' or '--help' to see CLI syntax.\n");
            break;
        case 6:
            printf("6: NO_DIRECTORY_GIVEN\n");
            printf("The program was not given a directory to search for a filename in.\n");
            printf("Run with '-h' or '--help' to see CLI syntax.\n");
            break;
        case 7:
            printf("7: INVALID_LOOKUP_CODE\n");
            printf("The program was given an unrecognized lookup code.\n");
            break;
        case 8:
            printf("8: OUTFILE_OPEN_FAILURE\n");
            printf("The program was unable to open the given output file by name.\n");
            printf("Ensure the given output file name is available to use.\n");
            break;
        default:
            printf("invalid exit code: %i\n", code);
            exit(INVALID_LOOKUP_CODE);
    }
}

void write_find_to_file(
    const char* outfile,
    const char* dir_path,
    const char* filename,
    int verbose
) {
    FILE* stream;
    stream = fopen(outfile, "a");
    if (stream == NULL) {
        printf("failed to open outfile\n");
        exit(OUTFILE_OPEN_FAILURE);
    }

    // 1. write the dir_path
    int dir_path_len = strlen(dir_path);
    fwrite(dir_path, dir_path_len, 1, stream);
    // 2. write the "/"
    fwrite("/", 1, 1, stream);
    // 3. write the filename
    int filename_len = strlen(filename);
    fwrite(filename, filename_len, 1, stream);
    // 4. write the final newline
    fwrite("\n", 1, 1, stream);

    if (verbose) printf("wrote new entry to %s: %s/%s\n", outfile, dir_path, filename);

    fclose(stream);
}

void check_directory(
    const char* dir_path,
    const char* filename,
    const char* outfile,
    int verbose,
    int recursive
) { 
    if (verbose) printf("checking directory: %s\n", dir_path);

    DIR* dir;
    struct dirent* entry;

    dir = opendir(dir_path);
    if (dir == NULL) {
        printf("unable to open directory %s\n", dir_path);
        exit(DIRECTORY_OPEN_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        const char* entry_name = entry->d_name;

        if (recursive) {
            if (entry->d_type == DT_DIR) {
                char path[PATH_MAX_LEN];

                // skip current and parent dirs
                if (strcmp(entry_name, ".") == 0 || strcmp(entry_name, "..") == 0) continue;

                // construct new path for recursive checking
                snprintf(path, sizeof(path), "%s/%s", dir_path, entry_name);

                // recursive call to check_directory
                check_directory(path, filename, outfile, verbose, recursive);
            } else {
                if (verbose) printf("checking %s/%s\n", dir_path, entry_name);
                if (strcmp(filename, entry_name) == 0) {
                    if (verbose) printf("[FIND]\n");
                    printf("found %s in %s\n", filename, dir_path);
                    if (verbose) printf("[/FIND]\n");
                    if (outfile != NULL) write_find_to_file(outfile, dir_path, filename, verbose);
                }
            }
        } else {
            if (verbose) printf("checking %s/%s\n", dir_path, entry_name);
            if (strcmp(filename, entry_name) == 0) {
                if (verbose) printf("[FIND]\n");
                printf("found %s in %s\n", filename, dir_path);
                if (verbose) printf("[/FIND]\n");
                if (outfile != NULL) write_find_to_file(outfile, dir_path, filename, verbose);
            }
        }
    }

    if (closedir(dir) == -1) {
        printf("unable to close directory %s\n", dir_path);
        exit(DIRECTORY_CLOSE_FAILURE);
    }

    if (verbose) printf("===\n");
}

int main(int argc, char* argv[]) {
    int verbose = 0;
    int recursive = 0;
    char* filename;
    char* outfile;
    char* lookup_val;

    static struct option long_options[] = {
        { "verbose",   no_argument,       0, 'v' },
        { "recursive", no_argument,       0, 'r' },
        { "help",      no_argument,       0, 'h' },
        { "version",   no_argument,       0, 'V' },
        { "lookup",    required_argument, 0, 'l' },
        { "outfile",   required_argument, 0, 'o' },
        { 0,           0,                 0, 0   }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "vrhVl:o:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'r':
                recursive = 1;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'l':
                lookup_val = optarg;
                lookup_exit_code(lookup_val);
                return 0;
            case 'h':
                print_usage();
                return 0;
            case 'V':
                printf("%s\n", VERSION);
                return 0;
            default:
                printf("unrecognized option detected. run with '-h'/'--help' to see valid options\n");
                return ILLEGAL_OPTION;
        }
    }

    // extract filename
    if (optind >= argc) {
        printf("no filename given\n");
        exit(NO_FILENAME_GIVEN);
    }
    filename = argv[optind];
    optind++;

    if (verbose) {
        printf("verbose = %i\n", verbose);
        printf("recursive = %i\n", recursive);
        printf("outfile = %s\n", outfile);
        printf("filename = %s\n", filename);
        printf("=====\n");
    }

    // extract directories
    if (optind >= argc) {
        printf("no directory given\n");
        exit(NO_DIRECTORY_GIVEN);
    }
    for (int i = optind; i < argc; i++) {
        check_directory(argv[i], filename, outfile, verbose, recursive);
    }

    return 0;
}
