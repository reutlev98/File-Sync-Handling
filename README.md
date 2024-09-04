

# File Handling and Synchronization in C Repository

This repository contains four parts, each addressing different aspects of file handling and synchronization in C. Below is a brief overview of each part:

## Part 1: Advanced Synchronization of File Access with Naive Methods

**Objective:**  
Understand the challenges of synchronizing file access between processes using naive methods such as `wait` or `sleep`.

**Instructions:**

- **Program:** `part1.c`
- **Task:** Fork two child processes and ensure synchronized writing to `output.txt` using naive methods.
- **Requirements:**
  - Create `output.txt`.
  - Write messages to the file using `wait` or `sleep`.
  - Accept command-line arguments for messages and write count.
- **Example Usage:**
  ```bash
  ./part1 "Parent message" "Child1 message" "Child2 message" 3
  ```
- **Expected Output in `output.txt`:**
  ```
  Child1 message
  Child1 message
  Child1 message
  Child2 message
  Child2 message
  Child2 message
  Parent message
  Parent message
  Parent message
  ```

## Part 2: Implementing a Synchronization Lock for File Access

**Objective:**  
Implement a synchronization lock to ensure only one process writes to a file at a time.

**Instructions:**

- **Program:** `part2.c`
- **Task:** Fork a dynamic number of child processes and use a lock file (`lockfile.lock`) to manage access to `output2.txt`.
- **Requirements:**
  - Use `lockfile.lock` to synchronize file writing.
  - Redirect output to `output2.txt`.
  - Accept command-line arguments for messages and write count.
- **Example Usage:**
  ```bash
  ./part2 "First message" "Second message" "Third message" 3 > output2.txt
  ```
- **Expected Output in `output2.txt`:**
  ```
  First message
  First message
  First message
  Third message
  Third message
  Third message
  Second message
  Second message
  Second message
  ```

## Part 3: Buffered File I/O with O_PREAPPEND Flag

**Objective:**  
Implement a buffered I/O library with support for `O_PREAPPEND`, which allows writing to the beginning of a file.

**Instructions:**

- **Files:**
  - `buffered_open.h`: Header file with function prototypes.
  - `buffered_open.c`: Implementation of buffered I/O functions.
- **Task:**
  - Implement buffered reading/writing with `O_PREAPPEND` flag.
  - Ensure buffered data is managed and flushed correctly.
- **How to use:**
  1. **Compile the Program:**

   ```bash
   gcc -o directory_copy directory_copy.c
   ```

2. **Run the Program:**

   ```bash
   ./directory_copy <source_directory> <destination_directory>
   ```

   Replace `<source_directory>` and `<destination_directory>` with the paths to the directories you want to copy.

## Part 4: Implementing a Directory Copy Library in C

**Objective:**  
Implement a library to copy directory trees, including handling symbolic links and file permissions.

**Instructions:**

- **Files:**
  - `copytree.h`: Header file with function prototypes.
  - `copytree.c`: Implementation of directory copying functions.
  - `part4.c`: Main program to utilize the library.
- **Task:**
  - Implement `copy_file` and `copy_directory` functions.
  - Handle symbolic links and file permissions based on flags.
- **how to use:**
  1. **Compile the Program:**

   ```bash
   gcc -o buffered_io buffered_io.c
   ```

2. **Run the Program:**

   ```bash
   ./buffered_io <input_file> <output_file>
   ```

   Replace `<input_file>` with the path to the file you want to read from and `<output_file>` with the path to the file you want to write to.




