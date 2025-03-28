#ifndef __CEX_ARGUE_H__
#define __CEX_ARGUE_H__

#include "core.h"

typedef int (*ArgueParseFn)(void* out, const char* value);

typedef struct {
    const char* name;
    const char* description;
    const char* shorthand;
    ArgueParseFn parse;
} ArgueFlag;

typedef struct {
    const char* key;
    const char* value;
} ArgueArg;

typedef DynArray(ArgueArg) ArgueArgArray;

void argue_print_help(const char* description, const ArgueFlag flags[], size_t flagsz);
ArgueArgArray argue_get_args(size_t argc, char* argv[]);
bool argue_is_flag_name(const char* name, const char* flagname, const char* shorthand);
const ArgueFlag* argue_get_parser(const char* argkey, const ArgueFlag flags[], size_t flagsz);

////////// Default parsers ////////

typedef enum {
    ArgueParseNumOk,
    ArgueParseNumWrongFmt,
    ArgueParseNumOutOfRange,
} ArgueParseNumResult;

int argue_parse_int(void* out, const char* value);
int argue_parse_float(void* out, const char* value);
int argue_parse_str(void* out, const char* value);

#endif
