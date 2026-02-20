#define _DEFAULT_SOURCE

#include "list.h"

void print_usage(void)
{
    printf("usage: list [OPTION]... DIRECTORY...\n");
    printf("\nList the contents of a given directory (or directories)\n");
    printf("\nOptions:\n");
    printf("    -i, --ino                 print each directory entry's serial number\n");
    printf("    -f, --files               read each file and print basic information\n");
    printf("    -v, --verbose             print more detailed progress of this program while running\n");
    printf("    -h, --help                print this message and exit\n");
    printf("    -V, --version             print the program version and exit\n");
    printf("\nPositionals:\n");
    printf("    DIRECTORY...              the directory (or directories) to look through\n");
    printf("\nCopyright (c) 2026 Addison Kline (GitHub: @addisonkline)\n");
}

void read_file(
    const char* file_path,
    int verbose
) {
    FILE* file_p;
    size_t ret;
    unsigned char buffer[4];

    file_p = fopen(file_path, "rb");
    if (file_p == NULL) {
        printf("\nfatal: failed to open file %s\n", file_path);
        exit(ERROR_FILE_OPEN);
    }

    ret = fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), file_p);
    if (ret != ARRAY_SIZE(buffer)) {
        if (ret == 0) {
            printf("(empty); ");
            fclose(file_p);
            return;
        }

        printf("\nfatal: failure while reading file: %zu\n", ret);
        exit(ERROR_FREAD);
    }

    printf("ELF magic %#04x%02x%02x%02x; ", buffer[0], buffer[1], buffer[2], buffer[3]);

    ret = fread(buffer, 1, 1, file_p);
    if (ret != 1) {
        printf("\nfatal: failure while reading file: %zu\n", ret);
        exit(ERROR_FREAD);
    }

    printf("class %#04x; ", buffer[0]);

    fclose(file_p);
}

void read_directory(
    const char* dir_path,
    int ino,
    int files,
    int verbose
) {
    DIR* dir;
    struct dirent* entry;

    dir = opendir(dir_path);
    if (dir == NULL) {
        printf("fatal: failed to open directory %s\n", dir_path);
        exit(ERROR_DIR_OPEN);
    }

    printf("%s\n", dir_path);
    while ((entry = readdir(dir)) != NULL) {
        const char* entry_name = entry->d_name;
        unsigned char entry_type = entry->d_type;
        const ino_t entry_ino = entry->d_ino;
        char* entry_type_str;

        switch (entry_type) {
            case DT_REG:
                entry_type_str = "F";
                break;
            case DT_DIR:
                entry_type_str = "D";
                break;
            default:
                entry_type_str = "?";
        }

        printf("> ");
        printf("%s ", entry_type_str);
        printf("%s; ", entry_name);
        if (ino) printf("serial %lu; ", entry_ino);
        if (files) {
            if (entry_type == DT_REG) {
                char* file_path;
                size_t file_path_len = strlen(dir_path) + strlen("/") + strlen(entry_name) + 1;
                file_path = malloc(sizeof(*file_path) * file_path_len);
                if (file_path == NULL) {
                    printf("\nfatal: malloc failed\n");
                    exit(ERROR_MALLOC);
                }

                strcpy(file_path, dir_path);
                strcat(file_path, "/");
                strcat(file_path, entry_name);

                read_file(file_path, verbose);

                free(file_path);
                file_path = NULL;
            }
        }
        printf("\n");
    }

    if (closedir(dir) == -1) {
        printf("fatal: unable to close directory %s\n", dir_path);
        exit(ERROR_DIR_CLOSE);
    }
}

int main(int argc, char* argv[])
{
    int ino = 0;
    int files = 0;
    int verbose = 0;

    static struct option longopts[] = {
        { "ino",            no_argument, 0, 'i' },
        { "files",          no_argument, 0, 'f' },
        { "verbose",        no_argument, 0, 'v' },
        { "help",           no_argument, 0, 'h' },
        { "version",        no_argument, 0, 'V' },
        { 0,                0,           0,  0  }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "ifvhV", longopts, NULL)) != -1) {
        switch (opt) {
            case 'i':
                ino = 1;
                break;
            case 'f':
                files = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'h':
                print_usage();
                return 0;
            case 'V':
                printf("%s\n", VERSION);
                return 0;
            default:
                return ERROR_INVALID_OPTION;
        }
    }

    if (optind >= argc) {
        printf("fatal: no directories given\n");
        return ERROR_NO_DIRECTORIES_GIVEN;
    };

    if (verbose) {
        printf("[main] verbose output enabled\n");
        printf("[main] ino = %i\n", ino);
        printf("[main] files = %i\n", files);
    }

    for (int i = optind; i < argc; i++) {
        if (verbose) printf("[main] reading directory %s\n", argv[i]);
        read_directory(argv[i], ino, files, verbose);
    }

    return 0;
}
