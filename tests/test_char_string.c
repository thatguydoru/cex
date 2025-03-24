#include <assert.h>
// #include <stdio.h>

#include "cex.h"

void test_equal(void) {
    CharString a = char_string_from_c_str("hello");
    CharString b = char_string_from_c_str("hello");
    assert(char_string_eq(&a, &b));

    a = char_string_from_c_str("hello");
    b = char_string_from_c_str("world");
    assert(!char_string_eq(&a, &b));
}

void test_find(void) {
    CharString a = char_string_from_c_str("he3333llo");
    MaybeIndex idx = char_string_find(&a, "ll");
    assert(idx.exists);
    assert(idx.value == 6);

    idx = char_string_find(&a, "bad");
    assert(!idx.exists);
}

void test_rfind(void) {
    CharString a = char_string_from_c_str("hellolleh");
    MaybeIndex idx = char_string_rfind(&a, "ll");
    assert(idx.exists);
    assert(idx.value == 5);

    idx = char_string_rfind(&a, "bad");
    assert(!idx.exists);
}

int main(void) {
    test_equal();
    test_find();
    test_rfind();

    // CharString a = char_string_from_c_str("hellolleh");
    // fwrite(a.items, sizeof(char), a.size, stdout);

    panic("bruh");

    return 0;
}
