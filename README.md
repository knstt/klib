# klib

A collection of lightweight, header-only data structure and utility libraries for C. Inspired by the [STB](https://github.com/nothings/stb) single-header style.

## Libraries

| Header    | Description                    |
|-----------|--------------------------------|
| `vec.h`   | Generic dynamic array (vector) |
| `stack.h` | Generic LIFO stack             |
| `queue.h` | Generic FIFO queue             |
| `list.h`  | Generic doubly-linked list     |
| `build.h` | Single-header build system     |

## Usage

Copy the header files you need into your project and `#include` them. All data structure libraries are macro-based and require no separate compilation step.

See the header comments in each file for usage instructions and API details.

## Build System

```c
#define BUILD_IMPLEMENTATION
#include "build.h"

void build_project(void) {
    BUILD_CMD("gcc", "-o", "myapp", "main.c");
}

int main(int argc, char **argv) {
    BUILD_REBUILD_URSELF(argc, argv);
    build_project();
    return 0;
}
```

See the header comment in each file for the full list of customization macros.
