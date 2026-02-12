/*
 * build.h - Single-header build system for C projects
 * 
 * USAGE:
 *   Define BUILD_IMPLEMENTATION in exactly one C file before including this header.
 *   
 *   Example build.c:
 *     #define BUILD_IMPLEMENTATION
 *     #include "build.h"
 *     
 *     void build_project(void) {
 *         BUILD_CMD("gcc", "-o", "myapp", "main.c");
 *     }
 *     
 *     int main(int argc, char **argv) {
 *         BUILD_REBUILD_URSELF(argc, argv);
 *         build_project();
 *         return 0;
 *     }
 *   
 *   Compile: gcc -o build build.c
 *   Run:     ./build
 *
 * FEATURES:
 *   - Self-rebuilding build scripts
 *   - Automatic change detection
 *   - Hot reload mode with automatic rebuild on file changes
 *   - Process management for running built applications
 *   - Simple macro-based API
 *
 * CUSTOMIZATION:
 *   #define BUILD_PRINTF to customize logging (default: printf)
 *
 */

#ifndef BUILD_H_
#define BUILD_H_

#include <stddef.h>  /* size_t */

#ifndef BUILD_PRINTF
#define BUILD_PRINTF printf
#endif

/*
 * BUILD_CMD(...) - Execute a command and wait for completion
 * 
 * Executes a command with arguments, prints the command being run, and exits
 * the build process if the command fails. Automatically adds NULL terminator.
 * 
 * Example:
 *   BUILD_CMD("gcc", "-Wall", "-o", "output", "source.c");
 */
#define BUILD_CMD(...) build_cmd(__FILE__, __LINE__, __VA_ARGS__, NULL)

/*
 * BUILD_REBUILD_URSELF(argc, argv) - Automatically rebuild build script if changed
 * 
 * Checks if the build script source is newer than the binary. If so, recompiles
 * the build script and re-executes it with the same arguments.
 * 
 * Call this at the start of main().
 * 
 * Example:
 *   int main(int argc, char **argv) {
 *       BUILD_REBUILD_URSELF(argc, argv);
 *       // ... rest of build logic
 *   }
 */
#define BUILD_REBUILD_URSELF(argc, argv) build_rebuild_urself(argc, argv, __FILE__)

/*
 * BUILD_WATCH_AND_REBUILD(watch_files, build_func) - Watch files and rebuild on changes
 * 
 * Watches specified source files and calls build_func() whenever any file changes.
 * Runs in an infinite loop until interrupted (Ctrl+C).
 * 
 * Parameters:
 *   watch_files - Array of file paths to watch (const char *[])
 *   build_func  - Function to call when changes detected (void (*)(void))
 * 
 * Example:
 *   const char *sources[] = {"main.c", "utils.c"};
 *   BUILD_WATCH_AND_REBUILD(sources, build_project);
 */
#define BUILD_WATCH_AND_REBUILD(watch_files, build_func) \
    build_watch_and_rebuild(watch_files, sizeof((watch_files))/sizeof((watch_files)[0]), build_func)

/*
 * BUILD_RUN_PROCESS(file, ...) - Run a process in the background
 * 
 * Forks and executes a program without waiting. Returns the child PID.
 * Use BUILD_KILL_PROCESS() to terminate it later. Automatically adds NULL terminator.
 * 
 * Parameters:
 *   file - Program to execute
 *   ...  - Arguments
 * 
 * Returns: Process ID of the child process
 * 
 * Example:
 *   int pid = BUILD_RUN_PROCESS("./myapp", "--option", "value");
 *   // ... do other work ...
 *   BUILD_KILL_PROCESS(pid);
 */
#define BUILD_RUN_PROCESS(file, ...) build_run_process(file, __VA_ARGS__, NULL)

/*
 * BUILD_KILL_PROCESS(pid) - Terminate a background process
 * 
 * Sends SIGTERM to the specified process and waits for it to exit.
 * 
 * Parameters:
 *   pid - Process ID returned by BUILD_RUN_PROCESS()
 * 
 * Example:
 *   BUILD_KILL_PROCESS(app_pid);
 */
#define BUILD_KILL_PROCESS(pid) build_kill_process(pid)

/* Function declarations - do not call these directly, use macros above */
void build_kill_process(int pid);
int build_run_process(const char *file, ...);
int build_cmd(const char *file, int line, ...);
void build_rebuild_urself(int argc, char **argv, const char *source_file);
int build_needs_rebuild(const char *output, const char *input);
void build_watch_and_rebuild(const char **watch_files, size_t watch_count, void (*build_func)(void));

/*
 * ============================================================================
 * IMPLEMENTATION
 * ============================================================================
 */

#ifdef BUILD_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int build_needs_rebuild(const char *output, const char *input) {
    struct stat output_stat, input_stat;
    
    if (stat(output, &output_stat) != 0) return 1;
    if (stat(input, &input_stat) != 0) return 1;
    
    return input_stat.st_mtime > output_stat.st_mtime;
}

int build_cmd(const char *file, int line, ...) {
    va_list args;
    va_start(args, line);
    
    const char *arg = va_arg(args, const char *);
    BUILD_PRINTF("[CMD] ");
    while (arg != NULL) {
        BUILD_PRINTF("%s ", arg);
        arg = va_arg(args, const char *);
    }
    BUILD_PRINTF("\n");
    
    va_end(args);
    va_start(args, line);
    
    pid_t pid = fork();
    if (pid < 0) {
        BUILD_PRINTF("ERROR: Could not fork: %s\n", strerror(errno));
        exit(1);
    }
    
    if (pid == 0) {
        char **argv_child = NULL;
        size_t argv_count = 0;
        
        arg = va_arg(args, const char *);
        while (arg != NULL) {
            argv_child = realloc(argv_child, (argv_count + 1) * sizeof(char *));
            argv_child[argv_count++] = (char *)arg;
            arg = va_arg(args, const char *);
        }
        argv_child = realloc(argv_child, (argv_count + 1) * sizeof(char *));
        argv_child[argv_count] = NULL;
        
        execvp(argv_child[0], argv_child);
        BUILD_PRINTF("ERROR: Could not exec %s: %s\n", argv_child[0], strerror(errno));
        exit(1);
    }
    
    va_end(args);
    
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0) {
            BUILD_PRINTF("ERROR: Command exited with code %d\n", exit_code);
        }
    } else {
        BUILD_PRINTF("ERROR: Command terminated abnormally\n");
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

void build_rebuild_urself(int argc, char **argv, const char *source_file) {
    const char *binary = argv[0];
    
    if (!build_needs_rebuild(binary, source_file)) {
        return;
    }
    
    BUILD_PRINTF("[INFO] Rebuilding %s from %s\n", binary, source_file);
    int res = build_cmd(source_file, 0, "gcc", "-o", binary, source_file, NULL);
    if (res != 0) {
        BUILD_PRINTF("[ERROR] Could not rebuild build script\n");
        exit(1);
    }
    BUILD_PRINTF("[INFO] Restarting...\n");
    execvp(binary, argv);
    BUILD_PRINTF("[ERROR] Could not restart: %s\n", strerror(errno));
    exit(1);
}

int build_run_process(const char *file, ...) {
    va_list args;
    va_start(args, file);
    
    pid_t pid = fork();
    if (pid < 0) {
        BUILD_PRINTF("ERROR: Could not fork: %s\n", strerror(errno));
        exit(1);
    }
    
    if (pid == 0) {
        char **argv_child = NULL;
        size_t argv_count = 0;
        
        const char *arg = file;
        while (arg != NULL) {
            argv_child = realloc(argv_child, (argv_count + 1) * sizeof(char *));
            argv_child[argv_count++] = (char *)arg;
            arg = va_arg(args, const char *);
        }
        argv_child = realloc(argv_child, (argv_count + 1) * sizeof(char *));
        argv_child[argv_count] = NULL;
        
        execvp(argv_child[0], argv_child);
        BUILD_PRINTF("ERROR: Could not exec %s: %s\n", argv_child[0], strerror(errno));
        exit(1);
    }
    
    va_end(args);
    
    return pid;
}

void build_watch_and_rebuild(const char **watch_files, size_t watch_count, void (*build_func)(void)) {
    BUILD_PRINTF("[INFO] Hot reload mode enabled. Watching files...\n");
    
    time_t *last_modified = malloc(watch_count * sizeof(time_t));
    for (size_t i = 0; i < watch_count; i++) {
        struct stat st;
        if (stat(watch_files[i], &st) == 0) {
            last_modified[i] = st.st_mtime;
        } else {
            last_modified[i] = 0;
        }
    }
    
    build_func();
    BUILD_PRINTF("[INFO] Watching for changes... (Press Ctrl+C to stop)\n");
    
    while (1) {
        sleep(1);
        
        int changed = 0;
        for (size_t i = 0; i < watch_count; i++) {
            struct stat st;
            if (stat(watch_files[i], &st) == 0) {
                if (st.st_mtime != last_modified[i]) {
                    printf("[INFO] Detected change in %s\n", watch_files[i]);
                    last_modified[i] = st.st_mtime;
                    changed = 1;
                }
            }
        }
        
        if (changed) {
            BUILD_PRINTF("[INFO] Rebuilding...\n");
            build_func();
            BUILD_PRINTF("[INFO] Build complete. Watching for changes...\n");
        }
    }
    
    free(last_modified);
}

void build_kill_process(int pid) {
    if (kill(pid, SIGTERM) != 0) {
        BUILD_PRINTF("ERROR: Could not kill process %d: %s\n", pid, strerror(errno));
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

#endif // BUILD_IMPLEMENTATION

#endif // BUILD_H_
