#ifndef BUFFERED_OPEN_H
#define BUFFERED_OPEN_H

#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024 // Define the buffer size as needed
#define O_PREAPPEND 0x1000 // Just an example value, ensure it doesn't conflict with other flags


typedef struct {
    int fd; // File descriptor
    char *buffer; // Pointer to the buffer
    size_t buffer_index; // Current index in the buffer
    size_t buffer_size; // Size of the buffer
} buffered_file_t;

// Function prototypes
buffered_file_t* buffered_open(const char *path, int flags, mode_t mode);
int buffered_write(buffered_file_t *bf, const char *data, size_t length);
int buffered_read(buffered_file_t *bf, char *buffer, size_t length);
int buffered_flush(buffered_file_t *bf);
int buffered_close(buffered_file_t *bf);

#endif // BUFFERED_OPEN_H
