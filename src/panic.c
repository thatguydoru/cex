#include <stdio.h>

#include "cex.h"

void panic(const char* message) {
    fprintf(stderr, "[PANIC] %s\n", message);
    exit(-1);
}
