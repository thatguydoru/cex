#include <errno.h>

#include "argue.h"

void argue_print_help(const char* description, const ArgueFlag flags[], size_t flagsz) {
    printf("%s\n\n", description);
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (flag->shorthand) {
            printf("\t%s, %s\t\t%s", flag->name, flag->shorthand, flag->description);
        } else {
            printf("\t%s\t\t%s", flag->name, flag->description);
        }
    }
}

bool is_short_key(const char* s) {
    return s[0] == '-' && s[1] != '-';
}

bool is_long_key(const char* s) {
    return s[0] == '-' && s[1] == '-';
}

bool is_flag_key(const char* s) {
    return is_long_key(s) && is_short_key(s);
}

const char* strip_arg_prefix(const char* s) {
    return is_long_key(s) ? &s[2] : is_short_key(s) ? &s[1] : s;
}

ArgueArgArray argue_get_args(size_t argc, char* argv[]) {
    ArgueArgArray args;
    dynarray_default(ArgueArg, args);

    for (size_t i = 0; i < argc; i++) {
        ArgueArg arg = {0};
        const char* argkey = argv[i];
        if (is_flag_key(argkey)) {
            arg.key = strip_arg_prefix(argkey);
            size_t next = i + 1;
            if (next < argc && !is_flag_key(argv[next])) {
                arg.value = argv[next];
                i++;
            }
            dynarray_push(&args, arg);
        }
    }

    return args;
}

bool argue_is_flag_name(const char* name, const char* longname, const char* shorthand) {
    return !strcmp(name, longname) || !strcmp(name, shorthand);
}

const ArgueFlag* argue_get_parser(const char* argkey, const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (argue_is_flag_name(argkey, flag->name, flag->shorthand)) {
            return flag;
        }
    }

    return NULL;
}

////////// Default parser implementations //////////

int argue_parse_int(void* out, const char* value) {
    errno = 0;
    size_t sz = strlen(value);
    char* end = NULL;
    long n = strtol(value, &end, 10);
    if (end != &value[sz - 1]) {
        return ArgueParseNumWrongFmt;
    }
    if (errno == ERANGE) {
        return ArgueParseNumOutOfRange;
    }
    *((int*)out) = n;

    return ArgueParseNumOk;
}

int argue_parse_float(void* out, const char* value) {
    errno = 0;
    size_t sz = strlen(value);
    char* end = NULL;
    float n = strtof(value, &end);
    if (end != &value[sz - 1]) {
        return ArgueParseNumWrongFmt;
    }
    if (errno == ERANGE) {
        return ArgueParseNumOutOfRange;
    }
    *((float*)out) = n;

    return ArgueParseNumOk;
}

int argue_parse_str(void* out, const char* value) {
    *((char**)out) = (char*)value;
    return 0;
}
