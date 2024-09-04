#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#define LOCKFILE "lockfile.lock"

// Function to check if a string is a positive integer
int is_positive_integer(const char *str) {
    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

// Function to acquire the lock
void acquire_lock() {
    while (open(LOCKFILE, O_CREAT | O_EXCL, 0444) == -1) {
        usleep(1000); // Sleep for 1 millisecond before retrying
    }
}

// Function to release the lock
void release_lock() {
    unlink(LOCKFILE);
}

// Function to write messages to a file with locking
void write_message(const char *filename, const char *message) {
    acquire_lock();
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        release_lock();
        exit(1);
    }
    fprintf(file, "%s\n", message);
    fclose(file);
    release_lock();
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> ... <order> <count>\n", argv[0]);
        return 1;
    }

    // Validate the count argument
    if (!is_positive_integer(argv[argc - 1])) {
        fprintf(stderr, "Error: The last argument must be a positive integer representing the count of messages to be written.\n");
        return 1;
    }

    const char *order = argv[argc - 2];
    int count = atoi(argv[argc - 1]);

    if (count <= 0) {
        fprintf(stderr, "Error: The count must be a positive integer.\n");
        return 1;
    }

    // Create the output file
    const char *filename = "output2.txt";
    FILE *file = fopen(filename, "w"); // Change mode to "w" for writing
    if (file == NULL) {
        perror("fopen");
        return 1;
    }
    fclose(file);

    // Fork the child processes
    int num_messages = argc - 3; // Excluding the program name, order, and count
    pid_t *pids = malloc(num_messages * sizeof(pid_t));
    if (pids == NULL) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < num_messages; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return 1;
        } else if (pids[i] == 0) {
            // Child process
            exit(0);
        }
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < num_messages; i++) {
        wait(NULL);
    }

    // Tokenize the order string and write messages in the specified order
    char *order_copy = strdup(order);
    if (order_copy == NULL) {
        perror("strdup");
        return 1;
    }

    int order_length = strlen(order_copy);
    for (int i = 0; i < count; i++) {
        int *printed = calloc(num_messages, sizeof(int));
        if (printed == NULL) {
            perror("calloc");
            free(order_copy);
            return 1;
        }

        for (int j = 0; j < order_length; j++) {
            if (order_copy[j] >= '1' && order_copy[j] <= '9') {
                int index = order_copy[j] - '1';
                if (index < 0 || index >= num_messages) {
                    fprintf(stderr, "Invalid order character: %c\n", order_copy[j]);
                    free(order_copy);
                    free(printed);
                    return 1;
                }
                if (!printed[index]) {
                    write_message(filename, argv[index + 1]);
                    printed[index] = 1;
                }
            }
        }
        free(printed);
        write_message(filename, order); // Print the order string once per cycle
    }

    free(order_copy);
    free(pids);
    return 0;
}
