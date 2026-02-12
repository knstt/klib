#define BUILD_IMPLEMENTATION
#include "../build.h"

#define FLAGS "-Wall", "-Wextra", "-O2"
#define COMPILER "gcc"
#define SOURCES "main.c"

#define RAYLIB_FOLDER "./raylib/"
#define RAYLIB_URL "https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz"
#define RAYLIB "-I./raylib/include", "./raylib/lib/libraylib.a", "-lm", "-lpthread"

static int app_pid = 0;

void project(void) {
    // Ensure Raylib is available
    if (BUILD_CMD("test", "-d", RAYLIB_FOLDER) != 0) {
        BUILD_CMD("mkdir", "-p", RAYLIB_FOLDER);
        BUILD_CMD("wget", "-O", "/tmp/raylib.tar.gz", RAYLIB_URL);
        BUILD_CMD("tar", "-xzf", "/tmp/raylib.tar.gz", "-C", RAYLIB_FOLDER, "--strip-components=1");
        BUILD_CMD("rm", "/tmp/raylib.tar.gz");
    }

    // Build the project
    BUILD_CMD(COMPILER, FLAGS, "-o", "main", SOURCES, RAYLIB);

    // Execute the built application
    if (app_pid > 0) BUILD_KILL_PROCESS(app_pid);
    app_pid = BUILD_RUN_PROCESS("./main", NULL);
}

int main(int argc, char **argv) {
    BUILD_REBUILD_URSELF(argc, argv);
    
    const char *watch_files[] = {SOURCES};
    BUILD_WATCH_AND_REBUILD(watch_files, project);
    
    return 0;
}
