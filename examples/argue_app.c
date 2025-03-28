#include "argue.h"

typedef struct {
    int foo;
    float bar;
} Conf;

const ArgueFlag flags[] = {
    {
        .name = "foo",
        .description = "a parser for foo",
        .shorthand = "f",
        .parse = &argue_parse_int,
    },
    {
        .name = "bar",
        .description = "the bar",
        .shorthand = NULL,
        .parse = &argue_parse_float,
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        argue_print_help("some random description for this tool", flags, arrsize(flags));
        return 0;
    }

    Conf conf = {0};
    ArgueArgArray args = argue_get_args(argc, argv);
    const ArgueArg* unknown = argue_get_first_unknown_arg(&args, flags, arrsize(flags));
    if (unknown) {
        eprintf("--%s is not a flag\n", unknown->key);
        return 1;
    }

    for (size_t i = 0; i < args.size; i++) {
        ArgueArg* arg = &args.items[i];
        const ArgueFlag* flag = argue_get_flag(arg->key, flags, arrsize(flags));
        if (flag) {
            if (argue_is_flag_name("foo", flag->name, flag->shorthand)) {
                int res = flag->parse(&conf.foo, arg->value);
                if (res) {
                    eprintln("wrong argument for foo");
                    return 1;
                }
            }
            if (argue_is_flag_name("bar", flag->name, flag->shorthand)) {
                int res = flag->parse(&conf.bar, arg->value);
                if (res) {
                    eprintln("wrong argument for bar");
                    return 1;
                }
            }
        } else {
            eprintf("%s is not a valid argument\n", arg->key);
        }
    }

    printf("%d, %f\n", conf.foo, conf.bar);

    return 0;
}
