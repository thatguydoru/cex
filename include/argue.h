#ifndef __CEX_ARGUE_H__
#define __CEX_ARGUE_H__

#include "core.h"

#define argue_eprintf(fmt, ...) eprintf("%s: " fmt, bin, __VA_ARGS__)
#define argue_eprintln(message) argue_eprintf("%s\n", message)

typedef bool (*ArgueParseFn)(
    void* out,
    const char bin[],
    const char flag_name[],
    const char value[]
);

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
    bool variadic;
    bool required;
} ArgueArgConfig;

typedef enum {
    ArgueParsePrintHelp,
    ArgueParseParseFnFail,
    ArgueParseFlagDoesNotExist,
    ArgueParseFlagMissingValue,
    ArgueParseArgsTooMany,
    ArgueParseArgsMissingValue,
} ArgueParseErrorTag;

typedef struct {
    ArgueParseErrorTag tag;
    union {
        const char* flag_does_not_exist;
        const ArgueFlag* flag_missing_value;
        size_t args_too_many;
    } inner;
} ArgueParseError;

typedef Result(size_t, ArgueParseError) ArgueParseResult;

ArgueParseResult argue_parse_flat(
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
