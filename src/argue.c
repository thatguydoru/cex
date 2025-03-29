#include <errno.h>

#include "argue.h"

void argue_print_help(const char* description, const ArgueFlag flags[], size_t flagsz) {
    int pads = 10;
    printf("%s\n\n", description);
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (flag->shorthand) {
            int padding = (int)strlen(flag->name) - pads + 3;  // +3 since there is the --
            printf("  --%s, -%*s %s\n", flag->name, padding, flag->shorthand, flag->description);
        } else {
            printf("  --%*s %s\n", -pads, flag->name, flag->description);
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
    return is_long_key(s) || is_short_key(s);
}

const char* strip_arg_prefix(const char* s) {
    return is_long_key(s) ? &s[2] : is_short_key(s) ? &s[1] : s;
}

ArgueArgArray argue_get_args(size_t argc, char* argv[]) {
    ArgueArgArray args;
    dynarray_default(ArgueArg, args);

    for (size_t i = 1; i < argc; i++) {
        ArgueArg arg = {0};
        const char* argstr = argv[i];
        if (is_flag_key(argstr)) {
            arg.key = strip_arg_prefix(argstr);
            size_t next = i + 1;
            if (next < argc && !is_flag_key(argv[next])) {
                arg.value = argv[next];
                i++;
            }
        } else {
            arg.value = argstr;
        }
        dynarray_push(&args, arg);
    }

    return args;
}

bool argue_is_flag_name(const char* name, const char* longname, const char* shorthand) {
    return (longname && !strcmp(name, longname)) || (shorthand && !strcmp(name, shorthand));
}

const ArgueFlag* argue_get_flag(const char* argkey, const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (argue_is_flag_name(argkey, flag->name, flag->shorthand)) {
            return flag;
        }
    }

    return NULL;
}

bool is_name_in_flags(const char* name, const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (argue_is_flag_name(name, flag->name, flag->shorthand)) {
            return true;
        }
    }

    return false;
}

const ArgueArg* argue_get_first_unknown_arg(
    const ArgueArgArray* args,
    const ArgueFlag flags[],
    size_t flagsz
) {
    for (size_t i = 0; i < args->size; i++) {
        const ArgueArg* arg = &args->items[i];
        if (!arg->key) {
            continue;
        }
        if (!is_name_in_flags(arg->key, flags, flagsz)) {
            return arg;
        }
    }

    return NULL;
}

////////// Default parser implementations //////////

int argue_parse_int(void* out, const char* value) {
    errno = 0;
    char* end = NULL;
    *((int*)out) = strtol(value, &end, 10);
    if (end != &value[strlen(value)]) {
        return ArgueParseNumWrongFmt;
    }
    if (errno == ERANGE) {
        return ArgueParseNumOutOfRange;
    }

    return ArgueParseNumOk;
}

int argue_parse_float(void* out, const char* value) {
    errno = 0;
    char* end = NULL;
    *((float*)out) = strtof(value, &end);
    if (end != &value[strlen(value)]) {
        return ArgueParseNumWrongFmt;
    }
    if (errno == ERANGE) {
        return ArgueParseNumOutOfRange;
    }

    return ArgueParseNumOk;
}

int argue_parse_str(void* out, const char* value) {
    *((char**)out) = (char*)value;
    return 0;
}
