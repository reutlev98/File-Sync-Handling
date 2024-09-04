// Reut lev 207385741
#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>\n", prog_name);
    fprintf(stderr, "  -l: Copy symbolic links as links\n");
    fprintf(stderr, "  -p: Copy file permissions\n");
}

int main(int argc, char *argv[]) {
    int opt;
    int copy_symlinks = 0;
    int copy_permissions = 0;

    while ((opt = getopt(argc, argv, "lp")) != -1) {
        switch (opt) {
            case 'l':
                copy_symlinks = 1;
                break;
            case 'p':
                copy_permissions = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (optind + 2 != argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *src_dir = argv[optind];
    const char *dest_dir = argv[optind + 1];

    // Check if source directory can be opened
    DIR* src_dir_ptr = opendir(src_dir);
    if (!src_dir_ptr) {
        perror("main: opendir source directory failed");
        return EXIT_FAILURE;
    }
    closedir(src_dir_ptr);

    // Check if destination directory already exists
    DIR* dest_dir_ptr = opendir(dest_dir);
    if (dest_dir_ptr) {
        closedir(dest_dir_ptr);
        fprintf(stderr, "Error: Destination directory already exists.\n");
        return EXIT_FAILURE;
    } else if (errno != ENOENT) {
        perror("main: opendir destination directory failed");
        return EXIT_FAILURE;
    }

    copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

    return 0;
}
