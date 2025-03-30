#include <assert.h>
#include <errno.h>
#include <float.h>
#include <limits.h>

#include "argue.h"

#define PADS 20

typedef const char* PrefixedKey;
typedef const char* StrippedKey;

const char* rstrstr(const char* haystack, const char needle[]) {
    size_t needlesz = strlen(needle);
    for (size_t i = strlen(haystack); i > 0; i--) {
        const char* ptr = &haystack[i - 1];
        if (!strncmp(ptr, needle, needlesz)) {
            return ptr + sizeof(char);
        }
    }

    return NULL;
}

void print_flag_help_full(
    const char* name,
    const char* shorthand,
    const char* description,
    int pads
) {
    int rpad = strlen(name) - pads + 3;
    printf("  --%s, -%*s%s\n", name, rpad, shorthand, description);
}

void print_flags_help(const ArgueFlag flags[], size_t flagsz) {
    puts("FLAGS:");
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (flag->shorthand && flag->description) {
            print_flag_help_full(flag->name, flag->shorthand, flag->description, PADS);
        } else if (!flag->shorthand && flag->description) {
            printf("  --%*s%s\n", -PADS, flag->name, flag->description);
        } else {
            printf("  --%s\n", flag->name);
        }
    }
    print_flag_help_full("help", "h", "print this help information", PADS);
}

void print_help_flat(
    const char* bin,
    const char* description,
    const ArgueFlag flags[],
    size_t flagsz,
    const ArgueArgConfig* config
) {
    puts(description);
    puts("\nUSAGE:");
    bin = rstrstr(bin, "/");
    if (config) {
        printf("  %s [FLAGS] [ARGS]\n\n", bin);
    } else {
        printf("  %s [FLAGS]\n\n", bin);
    }
    puts("ARGS:");
    if (config->required && config->variadic) {
        printf("  +%s...%*s\n", config->name, PADS + 3, config->description);
    } else if (config->variadic) {
        printf("  %s...%*s\n", config->name, PADS + 4, config->description);
    } else if (config->required) {
        printf("  +%*s%s\n", -(PADS + 1), config->name, config->description);
    } else {
        printf("  %*s%s\n", -(PADS + 2), config->name, config->description);
    }
    putchar('\n');
    print_flags_help(flags, flagsz);
}

bool is_flag_key_longhand(PrefixedKey key) {
    return key && key[0] == '-' && key[1] == '-';
}

bool is_flag_key_shorthand(PrefixedKey key) {
    return key && key[0] == '-' && key[1] != '-';
}

bool is_flag_key(PrefixedKey key) {
    return is_flag_key_longhand(key) || is_flag_key_shorthand(key);
}

StrippedKey strip_flag_key_prefix(PrefixedKey key) {
    if (is_flag_key_longhand(key)) {
        return &key[2];
    }
    if (is_flag_key_shorthand(key)) {
        return &key[1];
    }

    return key;
}

const ArgueFlag* get_flag(StrippedKey key, const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (!strcmp(key, flag->name) || (flag->shorthand && !strcmp(key, flag->shorthand))) {
            return flag;
        }
    }

    return NULL;
}

MaybeIndex get_first_unnamed_flag_index(const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (!flag->name) {
            return Some(MaybeIndex, i);
        }
    }

    return None(MaybeIndex);
}

const char* get_duplicate_flag_name(const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        for (size_t j = i + 1; j < flagsz; j++) {
            if (i == j) {
                break;
            }
            const ArgueFlag* next_flag = &flags[j];
            if (flag->name && next_flag->name && !strcmp(flag->name, next_flag->name)) {
                return flag->name;
            }
            if (flag->shorthand && next_flag->shorthand &&
                !strcmp(flag->shorthand, next_flag->shorthand)) {
                return flag->shorthand;
            }
        }
    }

    return NULL;
}

MaybeIndex get_first_flag_without_out(const ArgueFlag flags[], size_t flagsz) {
    for (size_t i = 0; i < flagsz; i++) {
        const ArgueFlag* flag = &flags[i];
        if (!flag->out) {
            return Some(MaybeIndex, i);
        }
    }

    return None(MaybeIndex);
}

bool has_help_flag_defined(const char* const argv[], size_t argc) {
    for (size_t i = 0; i < argc; i++) {
        const char* s = strip_flag_key_prefix(argv[i]);
        if (!strcmp(s, "help") || !strcmp(s, "h")) {
            return true;
        }
    }

    return false;
}

ArgueParseResult argue_parse_flat(
    const char* args_out[],
    const char* description,
    const char* const argv[],
    size_t argc,
    const ArgueFlag flags[],
    size_t flagsz,
    const ArgueArgConfig* config
) {
    MaybeIndex unnamed_idx = get_first_unnamed_flag_index(flags, flagsz);
    if (unnamed_idx.exists) {
        panicf("argue_parse_flat: flag #%zu is unnamed.\n", unnamed_idx.value + 1);
    }
    const char* duplicate_name = get_duplicate_flag_name(flags, flagsz);
    if (duplicate_name) {
        panicf("argue_parse_flat: \"%s\" is a duplicate name/shorthand.\n", duplicate_name);
    }
    MaybeIndex no_out = get_first_flag_without_out(flags, flagsz);
    if (no_out.exists) {
        panicf("argue_parse_flat: flag #%zu has no out variable.\n", no_out.value);
    }

    const char* bin = argv[0];
    if (argc == 1 || has_help_flag_defined(argv, argc)) {
        print_help_flat(bin, description, flags, flagsz, config);
        return Err(ArgueParseResult, ArgueParsePrintHelp);
    }

    const char* flag_keys[argc];
    size_t flag_keysz = 0;
    const char* flag_values[argc];
    size_t flag_valuesz = 0;
    size_t arg_valuesz = 0;

    // sort into flag keys and values, and args
    for (size_t i = 1; i < argc; i++) {
        const char* argv_arg = argv[i];
        if (flags && is_flag_key(argv_arg)) {
            StrippedKey key = strip_flag_key_prefix(argv_arg);
            const ArgueFlag* flag = get_flag(key, flags, flagsz);
            if (!flag) {
                argue_eprintf("%s is not a flag\n", key);
                return Err(ArgueParseResult, ArgueParseFlagDoesNotExist);
            }
            flag_keys[flag_keysz++] = key;
            size_t next = i + 1;
            if (next < argc && !is_flag_key(argv[next]) && flag->parsefn) {
                flag_values[flag_valuesz] = argv[next];
                i++;
            } else {
                flag_values[flag_valuesz] = NULL;
            }
            flag_valuesz++;
        } else if (args_out) {
            args_out[arg_valuesz++] = argv_arg;
        }
    }
    assert(flag_keysz == flag_valuesz);

    if (!config->variadic && arg_valuesz > 1) {
        argue_eprintln("too many arguments");
        return Err(ArgueParseResult, ArgueParseArgsTooMany);
    }
    if (config->required && arg_valuesz == 0) {
        argue_eprintf("%s: missing argument\n", config->name);
        return Err(ArgueParseResult, ArgueParseArgsMissingValue);
    }

    // evaluate flags
    if (flags) {
        for (size_t i = 0; i < flag_keysz; i++) {
            StrippedKey key = flag_keys[i];
            const char* value = flag_values[i];
            const ArgueFlag* flag = get_flag(key, flags, flagsz);
            assert(flag);
            if (flag->parsefn) {
                if (!value) {
                    argue_eprintf("%s: missing value\n", flag->name);
                    return Err(ArgueParseResult, ArgueParseFlagMissingValue);
                }
                if (!flag->parsefn(flag->out, bin, flag->name, value)) {
                    return Err(ArgueParseResult, ArgueParseParseFnFail);
                }
            } else {
                *((bool*)flag->out) = true;
            }
        }
    }

    return Ok(ArgueParseResult, arg_valuesz);
}

////////// Default parser implementations //////////

bool argue_parse_int(void* out, const char bin[], const char flag_name[], const char value[]) {
    errno = 0;
    char* end = NULL;
    *((int*)out) = strtol(value, &end, 10);
    if (end != &value[strlen(value)]) {
        argue_eprintf("%s: %s is not an int\n", flag_name, value);
        return false;
    }
    if (errno == ERANGE) {
        argue_eprintf("%s: value must be between %d and %d\n", flag_name, INT_MIN, INT_MAX);
        return false;
    }

    return true;
}

bool argue_parse_float(void* out, const char bin[], const char flag_name[], const char value[]) {
    errno = 0;
    char* end = NULL;
    *((float*)out) = strtof(value, &end);
    if (end != &value[strlen(value)]) {
        argue_eprintf("%s: %s is not a float\n", flag_name, value);
        return true;
    }
    if (errno == ERANGE) {
        argue_eprintf("%s: value must be between %.2f and %.2f\n", flag_name, FLT_MIN, FLT_MAX);
    }

    return true;
}

bool argue_parse_string(void* out, const char bin[], const char flag_name[], const char value[]) {
    unused(bin);
    unused(flag_name);

    *((const char**)out) = value;

    return true;
}
