#include "cex.h"

CharString char_string_with_capacity(size_t capacity) {
    CharString s;
    dynarray_with_capacity(char, s, capacity);

    return s;
}

CharString char_string_default(void) {
    return char_string_with_capacity(8);
}

CharString char_string_new(const char c_str[]) {
    CharString s;
    size_t sz = strlen(c_str);
    dynarray_new(char, s, c_str, sz);

    return s;
}

void char_string_concat_inplace(CharString* dest, const CharString* src) {
    dynarray_extend(dest, src);
}

CharString char_string_concat(const CharString* a, const CharString* b) {
    CharString out = char_string_with_capacity(a->capacity + b->capacity);
    char_string_concat_inplace(&out, a);
    char_string_concat_inplace(&out, b);

    return out;
}

bool char_string_eq(const CharString* a, const CharString* b) {
    if (a->size != b->size) {
        return false;
    }

    return !strncmp(a->items, b->items, a->size);
}

MaybeIndex char_string_find(const CharString* s, const char pat[], size_t patlen) {
    if (patlen > s->size) {
        return None(MaybeIndex);
    }
    if (patlen == s->size) {
        return !strncmp(s->items, pat, patlen) ? Some(MaybeIndex, 0) : None(MaybeIndex);
    }
    for (size_t i = 0; i < s->size; i++) {
        size_t rem = &s->items[s->size - 1] - &s->items[i] + 1;
        if (rem < patlen) {
            return None(MaybeIndex);
        }
        if (!strncmp(&s->items[i], pat, patlen)) {
            return Some(MaybeIndex, i);
        }
    }

    return None(MaybeIndex);
}

MaybeIndex char_string_rfind(const CharString* s, const char pat[], size_t patlen) {
    if (patlen > s->size) {
        return None(MaybeIndex);
    }
    if (patlen == s->size) {
        return !strncmp(s->items, pat, patlen) ? Some(MaybeIndex, 0) : None(MaybeIndex);
    }
    for (size_t i = s->size; i > 0; i--) {
        size_t rem = &s->items[i - 1] - &s->items[0] + 1;
        if (rem < patlen) {
            return None(MaybeIndex);
        }
        if (!strncmp(&s->items[i - 1], pat, patlen)) {
            return Some(MaybeIndex, i - 1);
        }
    }

    return None(MaybeIndex);
}

size_t char_string_to_buffer(const CharString* s, char out[], size_t size) {
    size_t sz = min(s->size, size);
    strncpy(out, s->items, sz);

    return sz;
}

void char_string_free(CharString* s) {
    dynarray_free(s);
}
