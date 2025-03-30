#include "argue.h"

typedef struct {
    int numi;
    float numf;
    char* str;
    bool boolean;
} Flags;

int main(int argc, const char* argv[]) {
    Flags conf = {0};
    const char* description = "some tool description";
    const ArgueFlag flags[] = {
        {
            // --numi <int> or -n <int>
            .name = "numi",
            .shorthand = "n",
            .description = "takes in an int",
            .out = &conf.numi,
            .parsefn = &argue_parse_int,
        },
        {
            // --numf <float>
            .name = "numf",
            .shorthand = NULL,
            .description = "takes in a float",
            .out = &conf.numf,
            .parsefn = &argue_parse_float,
        },
        {
            // --str <char string>
            .name = "str",
            .shorthand = NULL,
            .description = NULL,
            .out = &conf.str,
            .parsefn = &argue_parse_string,
        },
        {
            // --boolean
            .name = "boolean",
            .shorthand = NULL,
            .description = "takes in a bool",
            .out = &conf.boolean,
            .parsefn = NULL,
        },
    };
    ArgueArgConfig args_config = {"args", "some args"};
    const char* args_out[argc];
    ArgueParseResult res =
        argue_parse_flat(args_out, description, argv, argc, flags, arrsize(flags), &args_config);

    if (!res.ok) {
        if (res.value.error != ArgueParsePrintHelp) {
            eprintln("bruh");
            return 1;
        }
        return 0;
    }
    printf("numi: %d, numf: %f, boolean: %d\n", conf.numi, conf.numf, conf.boolean);
    if (conf.str) {
        printf("str: %s\n", conf.str);
    }
    for (size_t i = 0; i < res.value.data; i++) {
        puts(args_out[i]);
    }

    return 0;
}
