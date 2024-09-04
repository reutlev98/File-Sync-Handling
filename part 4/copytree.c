// Reut lev 207385741
#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    struct stat statbuf;

    // Handle symbolic links
    if (copy_symlinks) {
        if (lstat(src, &statbuf) == -1) {
            perror("copy_file: lstat failed");
            return;
        }
        if (S_ISLNK(statbuf.st_mode)) {
            char symlink_target[PATH_MAX];
            ssize_t len = readlink(src, symlink_target, sizeof(symlink_target) - 1);
            if (len == -1) {
                perror("copy_file: readlink failed");
                return;
            }
            symlink_target[len] = '\0';
            if (symlink(symlink_target, dest) == -1) {
                perror("copy_file: symlink failed");
                return;
            }
            return;
        }
    }

    // Open source file
    int src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("copy_file: open source file failed");
        return;
    }

    // Open or create destination file
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("copy_file: open destination file failed");
        close(src_fd);
        return;
    }

    // Copy file content
    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(src_fd, buf, sizeof(buf))) > 0) {
        if (write(dest_fd, buf, bytes) != bytes) {
            perror("copy_file: write failed");
            close(src_fd);
            close(dest_fd);
            return;
        }
    }

    if (bytes == -1) {
        perror("copy_file: read failed");
    }

    close(src_fd);
    close(dest_fd);

    // Copy file permissions
    if (copy_permissions) {
        if (stat(src, &statbuf) == -1) {
            perror("copy_file: stat failed");
            return;
        }
        if (chmod(dest, statbuf.st_mode) == -1) {
            perror("copy_file: chmod failed");
        }
    }
}

void create_directory(const char *path) {
    if (mkdir(path, 0777) == -1 && errno != EEXIST) {
        perror("create_directory: mkdir failed");
    }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
    DIR *dir = opendir(src);
    if (dir == NULL) {
        perror("copy_directory: opendir failed");
        return;
    }

    struct stat statbuf;
    if (stat(src, &statbuf) == -1) {
        perror("copy_directory: stat source directory failed");
        closedir(dir);
        return;
    }

    // Create destination directory with the same permissions as source if copy_permissions is set
    if (copy_permissions) {
        if (mkdir(dest, statbuf.st_mode) == -1 && errno != EEXIST) {
            perror("copy_directory: mkdir failed");
            closedir(dir);
            return;
        }
    } else {
        create_directory(dest);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_path[PATH_MAX];
        char dest_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);
        snprintf(dest_path, sizeof(dest_path), "%s/%s", dest, entry->d_name);

        if (lstat(src_path, &statbuf) == -1) {
            perror("copy_directory: lstat failed");
            closedir(dir);
            return;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            copy_directory(src_path, dest_path, copy_symlinks, copy_permissions);
        } else {
            copy_file(src_path, dest_path, copy_symlinks, copy_permissions);
        }
    }

    if (closedir(dir) == -1) {
        perror("copy_directory: closedir failed");
    }
}
