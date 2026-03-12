// args.h - Minimal single-header CLI argument helpers and a small option
// parser inspired by the example in `arg_test.c`.
//
// USAGE:
//   - Include this header where you need types or macros.
//   - Provide the implementation in exactly one C file by defining
//     ARGS_IMPLEMENTATION before including this header.
//
//   Example:
//     #define ARGS_IMPLEMENTATION
//     #include "args.h"
//
// FEATURES:
//   - Lightweight Option description types: flags, string, int
//   - Macros to declare options (ARGS_FLAG / ARGS_STRING / ARGS_INT)
//   - A simple parser: parse_args()
//   - Convenience getters: ARGS_GET_FLAG, ARGS_GET_STRING, ARGS_GET_INT
//   - Small extra helpers for ad-hoc arg lookup (ARGS_HAS / ARGS_GET)
//
#ifndef ARGS_H_
#define ARGS_H_

#include <stddef.h> // size_t

#ifndef ARGS_PRINTF
#include <stdio.h>
#define ARGS_PRINTF printf
#endif

// ---------------------------------------------------------------------------
// Types from the example
// ---------------------------------------------------------------------------

enum {
    TYPE_FLAG,
    TYPE_STRING,
    TYPE_INT
};

typedef struct {
    int type;
    const char* desc;
    const char* short_name;
    const char* long_name;
    union {
        int bool_value;
        const char* string_value;
        int int_value;
    } value;
} Argument;

typedef struct {
    Argument* arguments;
    size_t count;
} Parser;

// Macros to declare options inline (similar to the example)
#define ARGS_FLAG(short, long, desc) \
    {TYPE_FLAG, desc, "-" short, "--" long, .value.bool_value = 0},
#define ARGS_STRING(short, long, desc, default) \
    {TYPE_STRING, desc, "-" short, "--" long, .value.string_value = (default)},
#define ARGS_INT(short, long, desc, default) \
    {TYPE_INT, desc, "-" short, "--" long, .value.int_value = (default)},

#define ARGS_PARSE_CTX(name, ...) \
    Argument name##_args[] = { {TYPE_FLAG, "Show usage", "-h", "--help", .value.bool_value = 0 }, __VA_ARGS__ }; \
    Parser name = { .arguments = name##_args, .count = sizeof(name##_args) / sizeof(name##_args[0]) };

// ---------------------------------------------------------------------------
// Simple runtime lookup and convenience getters
// ---------------------------------------------------------------------------

// get_value(ctx, name, type) -> returns pointer to union value or NULL
void* get_value(Parser* parser, const char* name, int type);

#define ARGS_GET_FLAG(ctx, name) (get_value(ctx, "--" name, TYPE_FLAG) ? *(int*)get_value(ctx, "--" name, TYPE_FLAG) : 0)
#define ARGS_GET_STRING(ctx, name) (get_value(ctx, "--" name, TYPE_STRING) ? *(const char**)get_value(ctx, "--" name, TYPE_STRING) : NULL)
#define ARGS_GET_INT(ctx, name) (get_value(ctx, "--" name, TYPE_INT) ? *(int*)get_value(ctx, "--" name, TYPE_INT) : 0)

// (No ad-hoc helpers or iterator: the header exposes the Argument types,
// the parser `parse_args`, and runtime lookup `get_value` plus convenience
// getters. This keeps the API minimal and matches usage in `arg_test.c`.)

// ---------------------------------------------------------------------------
// Parser functions
// ---------------------------------------------------------------------------

#define ARGS_PARSE(ctx) parse_args(ctx, argc, argv)
#define ARGS_PRINT_USAGE(ctx) print_usage(ctx.arguments, ctx.count)

int parse_value(Argument* arg, const char* value_str);
int parse_args(Parser* parser, int argc, char *argv[]);
void print_usage(Argument* args, int num_args);

// ============================================================================
// IMPLEMENTATION
// ============================================================================
#define ARGS_IMPLEMENTATION

#ifdef ARGS_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

void* get_value(Parser* parser, const char* name, int type) {
    if (!parser) return NULL;
    for (size_t i = 0; i < parser->count; ++i) {
        if (strcmp(name, parser->arguments[i].long_name) == 0) {
            if (parser->arguments[i].type == type) return &parser->arguments[i].value;
            else return NULL;
        }
    }
    return NULL;
}

int parse_value(Argument* arg, const char* value_str) {
    if (!arg) return -1;
    switch (arg->type) {
        case TYPE_STRING:
            arg->value.string_value = value_str;
            return 0;
        case TYPE_INT:
            arg->value.int_value = atoi(value_str);
            return 0;
        default:
            return -1; // Unknown type
    }
}

void print_usage(Argument* args, int num_args) {
    ARGS_PRINTF("Usage:\n");
    for (int i = 0; i < num_args; ++i) {
        ARGS_PRINTF(" %s, %s\t%s", args[i].short_name, args[i].long_name, args[i].desc);
        switch (args[i].type)
        {
        case TYPE_STRING:
            ARGS_PRINTF(" - (%s)\n", args[i].value.string_value);
            break;
        case TYPE_INT:
            ARGS_PRINTF(" - (%d)\n", args[i].value.int_value);
            break;
        default:
            ARGS_PRINTF("\n");
            break;
        }
    }
    ARGS_PRINTF("\n");
}

int parse_args(Parser* parser, int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(parser->arguments, parser->count);
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        bool found = false;
        for (int j = 0; j < parser->count; ++j) {
            Argument* current_arg = &parser->arguments[j];
            if (strcmp(arg, current_arg->short_name) == 0 || strcmp(arg, current_arg->long_name) == 0) {
                found = true;
                if (current_arg->type == TYPE_FLAG) {
                    current_arg->value.bool_value = 1;
                } else {
                    if (i + 1 >= argc) {
                        ARGS_PRINTF("Expected value after %s\n", arg);
                        return -1;
                    }
                    if (parse_value(current_arg, argv[i + 1]) != 0)
                    {
                        ARGS_PRINTF("Failed to parse value for %s\n", arg);
                        return -1;
                    }
                    i++;
                }
            }
        }
        if (!found) {
            ARGS_PRINTF("Unknown argument: %s\n", arg);
            return -1;
        }
    }
    return 0;
}

#endif // ARGS_IMPLEMENTATION

#endif // ARGS_H_
