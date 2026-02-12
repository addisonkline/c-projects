#include "write.h"

void print_usage(void) {
    printf("usage: write [OPTION]... CONTENT FILE...\n");
    printf("\nWrite the given content to file(s)\n");
    printf("\nOptions:\n");
    printf("    -h, --help: show this message and exit\n");
    printf("    -V, --version: show the program version and exit\n");
    printf("\nPositionals:\n");
    printf("    CONTENT: the content to write to the file(s)\n");
    printf("    FILE...: the file(s) to write the content to\n");
    printf("\nCopyright (c) 2026 Addison Kline (GitHub: @addisonkline)\n");
}

void write_file(
    const char* content,
    const char* file
) {
    FILE* stream;
    stream = fopen(file, "w");
    if (stream == NULL) {
        printf("failed to open file%s\n", file);
        exit(FILE_OPEN_FAILURE);
    }

    int content_len = strlen(content);
    fwrite(content, content_len, 1, stream);

    fclose(stream);
}

int main(int argc, char* argv[]) {
    char* content;

    static struct option long_options[] = {
        { "help",    no_argument, 0, 'h' },
        { "version", no_argument, 0, 'V' },
        { 0,         0,           0,  0  }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hV", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage();
                return 0;
            case 'V':
                printf("%s\n", VERSION);
                return 0;
            default:
                return ILLEGAL_OPTION;
        }
    }

    // extract content
    if (optind >= argc) {
        printf("no content given\n");
        return NO_CONTENT_GIVEN;
    }
    content = argv[optind];
    optind++;

    // extract file(s)
    if (optind >= argc) {
        printf("no file(s) given\n");
        return NO_FILES_GIVEN;
    }
    for (int i = optind; i < argc; i++) {
        write_file(content, argv[i]);
    }

    return 0;
}
