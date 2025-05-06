#include <assert.h>

#include "core.h"

void test_equal(void) {
    CharString a = char_string_new("hello");
    CharString b = char_string_new("hello");
    assert(char_string_eq(&a, b.items, b.size));

    b = char_string_new("world");
    assert(!char_string_eq(&a, b.items, b.size));
}

void test_find(void) {
    CharString a = char_string_new("he3333llo");
    MaybeUsize idx = char_string_find(&a, "ll", 2);
    assert(idx.exists);
    assert(idx.value == 6);

    idx = char_string_find(&a, "bad", 3);
    assert(!idx.exists);
}

void test_rfind(void) {
    CharString a = char_string_new("hellolleh");
    MaybeUsize idx = char_string_rfind(&a, "ll", 2);
    assert(idx.exists);
    assert(idx.value == 5);

    idx = char_string_rfind(&a, "bad", 3);
    assert(!idx.exists);
}

int main(void) {
    test_equal();
    test_find();
    test_rfind();

    return 0;
}
