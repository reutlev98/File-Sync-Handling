#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "buffered_open.h"

// Define BUFFER_SIZE if it's not already defined
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

// Helper function to write data to the file
static int write_data(buffered_file_t *bf, const char *data, size_t length) {
    size_t total_written = 0;
    while (total_written < length) {
        ssize_t written = write(bf->fd, data + total_written, length - total_written);
        if (written < 0) {
            return -1; // Error writing to file
        }
        total_written += written;
    }
    return total_written;
}

// Helper function to read data from the file
static int read_data(buffered_file_t *bf, char *buffer, size_t length) {
    size_t total_read = 0;
    while (total_read < length) {
        ssize_t read_bytes = read(bf->fd, buffer + total_read, length - total_read);
        if (read_bytes < 0) {
            return -1; // Error reading from file
        } else if (read_bytes == 0) {
            break; // End of file
        }
        total_read += read_bytes;
    }
    return total_read;
}

// Helper function to flush the buffer
static int flush_buffer(buffered_file_t *bf) {
    if (bf->buffer_index > 0) {
        if (write_data(bf, bf->buffer, bf->buffer_index) < 0) {
            return -1; // Error writing to file
        }
        bf->buffer_index = 0;
    }
    return 0;
}

buffered_file_t* buffered_open(const char *path, int flags, mode_t mode) {
    int fd;
    buffered_file_t *bf;

    // Open the file descriptor
    if ((fd = open(path, flags, mode)) < 0) {
        return NULL;
    }

    // Handle O_PREAPPEND flag
    if (flags & O_PREAPPEND) {
        // Read existing content
        char *existing_content = NULL;
        off_t file_size = lseek(fd, 0, SEEK_END);
        if (file_size > 0) {
            lseek(fd, 0, SEEK_SET);
            existing_content = (char *)malloc(file_size);
            if (!existing_content || read(fd, existing_content, file_size) < 0) {
                close(fd);
                free(existing_content);
                return NULL;
            }
        }

        // Create a new buffer for the file
        bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
        if (!bf) {
            close(fd);
            free(existing_content);
            return NULL;
        }

        // Initialize buffered file structure
        bf->fd = fd;
        bf->buffer_index = 0;
        bf->buffer_size = BUFFER_SIZE;
        bf->buffer = (char *)malloc(bf->buffer_size);
        if (!bf->buffer) {
            close(fd);
            free(bf);
            free(existing_content);
            return NULL;
        }

        // Write the preappended data
        const char *preappend_data = "Preappended data"; // Adjust this as necessary
        if (write_data(bf, preappend_data, strlen(preappend_data)) < 0) {
            close(fd);
            free(bf->buffer);
            free(bf);
            free(existing_content);
            return NULL;
        }

        // Write existing content after preappended data
        if (existing_content) {
            if (write_data(bf, existing_content, file_size) < 0) {
                close(fd);
                free(bf->buffer);
                free(bf);
                free(existing_content);
                return NULL;
            }
            free(existing_content);
        }
        return bf;
    }

    // Handle other flags
    bf = (buffered_file_t *)malloc(sizeof(buffered_file_t));
    if (!bf) {
        close(fd);
        return NULL;
    }

    // Initialize buffered file structure
    bf->fd = fd;
    bf->buffer_index = 0;
    bf->buffer_size = BUFFER_SIZE;
    bf->buffer = (char *)malloc(bf->buffer_size);
    if (!bf->buffer) {
        close(fd);
        free(bf);
        return NULL;
    }

    return bf;
}

int buffered_write(buffered_file_t *bf, const char *data, size_t length) {
    // Flush buffer if necessary
    if (bf->buffer_index + length > bf->buffer_size) {
        if (flush_buffer(bf) < 0) {
            return -1;
        }
    }

    // Write data to the buffer
    if (length <= bf->buffer_size - bf->buffer_index) {
        memcpy(bf->buffer + bf->buffer_index, data, length);
        bf->buffer_index += length;
    } else {
        // Handle case where data is larger than buffer
        if (flush_buffer(bf) < 0) {
            return -1;
        }
        if (write_data(bf, data, length) < 0) {
            return -1;
        }
    }
    return length;
}

int buffered_read(buffered_file_t *bf, char *buffer, size_t length) {
    // Read from file directly into buffer if no buffered data
    if (bf->buffer_index == 0) {
        return read_data(bf, buffer, length);
    }

    // Handle buffered data
    size_t available = bf->buffer_index;
    size_t to_read = length < available ? length : available;
    memcpy(buffer, bf->buffer, to_read);
    bf->buffer_index -= to_read;
    memmove(bf->buffer, bf->buffer + to_read, bf->buffer_index);
    return to_read;
}

int buffered_flush(buffered_file_t *bf) {
    return flush_buffer(bf);
}

int buffered_close(buffered_file_t *bf) {
    if (flush_buffer(bf) < 0) {
        free(bf->buffer);
        free(bf);
        return -1;
    }

    // Close the file descriptor
    if (close(bf->fd) < 0) {
        free(bf->buffer);
        free(bf);
        return -1;
    }

    free(bf->buffer);
    free(bf);
    return 0;
}
