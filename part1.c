// Reut Lev 207385741
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void write_to_file(const char *filename, const char *message, int count) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; ++i) {
        if (fprintf(file, "%s\n", message) < 0) {
            perror("Failed to write to file");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    if (fclose(file) != 0) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *parent_message = argv[1];
    const char *child1_message = argv[2];
    const char *child2_message = argv[3];
    int count = atoi(argv[4]);

    if (count <= 0) {
        fprintf(stderr, "The count must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    pid_t child1_pid, child2_pid;

    child1_pid = fork();
    if (child1_pid < 0) {
        perror("Failed to fork child1");
        return EXIT_FAILURE;
    } else if (child1_pid == 0) {
        // Child1 process
        sleep(1);  // Ensure child1 writes first
        write_to_file("output.txt", child1_message, count);
        exit(EXIT_SUCCESS);
    }

    child2_pid = fork();
    if (child2_pid < 0) {
        perror("Failed to fork child2");
        return EXIT_FAILURE;
    } else if (child2_pid == 0) {
        // Child2 process
        sleep(2);  // Ensure child2 writes after child1
        write_to_file("output.txt", child2_message, count);
        exit(EXIT_SUCCESS);
    }

    // Parent process
    // Wait for both child processes to complete
    if (waitpid(child1_pid, NULL, 0) < 0) {
        perror("Failed to wait for child1");
        return EXIT_FAILURE;
    }
    if (waitpid(child2_pid, NULL, 0) < 0) {
        perror("Failed to wait for child2");
        return EXIT_FAILURE;
    }

    // Now the parent can write
    write_to_file("output.txt", parent_message, count);

    return EXIT_SUCCESS;
}
