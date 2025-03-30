#ifndef __CEX_ARGUE_H__
#define __CEX_ARGUE_H__

#include "core.h"

#define argue_eprintf(fmt, ...) eprintf("%s: " fmt, bin, __VA_ARGS__)
#define argue_eprintln(message) argue_eprintf(bin, "%s\n", message)

typedef bool (*ArgueParseFn)(void* out, const char bin[], const char flag_name[], const char value[]);

typedef struct {
    const char* name;
    const char* shorthand;
    const char* description;
    void* out;
    const ArgueParseFn parsefn;
} ArgueFlag;

typedef struct {
    const char* name;
    const char* description;
} ArgueArgConfig;

typedef enum {
    ArgueParsePrintHelp,
    ArgueParseParseFnFail,
    ArgueParseFlagDoesNotExist,
    ArgueParseFlagMissingValue,
} ArgueParseError;

typedef Result(size_t, ArgueParseError) ArgueParseResult;

ArgueParseResult argue_parse_flat(
    const char* args_out[],
    const char description[],
    const char* const argv[],
    size_t argc,
    const ArgueFlag flags[],
    size_t flagsz,
    const ArgueArgConfig* config
);

////////// Default parsers  ////////

bool argue_parse_int(void* out, const char bin[], const char flag_name[], const char value[]);
bool argue_parse_float(void* out, const char bin[], const char flag_name[], const char value[]);
bool argue_parse_string(void* out, const char bin[], const char flag_name[], const char value[]);

#endif
