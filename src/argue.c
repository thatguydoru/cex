#include "argue.h"

void argue_print_help(const char* description, const ArgueFlag flags[], size_t flagsz) {
    printf("%s\n\n", description);
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        printf("\t%s, %s\t\t%s", flag->name, flag->shorthand, flag->description);
    }
}

bool is_short_key(const char* s) {
    return  s[0] == '-' && s[1] != '-';
}

bool is_long_key(const char* s) {
    return  s[0] == '-' && s[1] == '-';
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
