#ifndef __CEX_ARGUE_H__
#define __CEX_ARGUE_H__

/**
    app := <command> <app> | <command> <args>*

    args := <flag>* <args>*  | <argument>* <args>*

    flag := <longname> | <shortname> | <bundled>
    longname := --<char>+
    shortname := -<char>
    bundled := -<shortname>+

    command := <char>+
    argument := <char>+

    char := 0x00..0xFF
*/

#include "core.h"

typedef Result(int, int) ArgueSubcommandResult;

typedef ArgueSubcommandResult (*ArgueSubcommandHandler)(int argc, cstr argv[]);

typedef struct {
    cstr name;
    cstr description;
    cstr usage;
    const ArgueSubcommandHandler handler;
} ArgueSubcommand;

typedef enum {
    ArgueFlagInt,
    ArgueFlagFloat,
    ArgueFlagCStr,
} ArgueFlagType;

typedef struct {
    cstr longname;
    cstr shortname;
    ArgueFlagType type;
    void* out;
} ArgueFlag;

typedef struct {
    cstr name;
    cstr usage;
    cstr description;
    const ArgueSubcommand* subcommands;
    size_t subcommandsz;
    const ArgueFlag* flags;
    size_t flagsz;
} ArgueCommand;

void argue_parse(ArgueCommand cmd, int argc, cstr argv[]);
void argue_print_help(ArgueCommand cmd);

#endif
