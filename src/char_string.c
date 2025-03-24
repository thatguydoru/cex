#include "cex.h"

CharString char_string_with_capacity(size_t capacity) {
    CharString s;
    dynarray_with_capacity(char, s, capacity);

    return s;
}

CharString char_string_default(void) {
    return char_string_with_capacity(8);
}

CharString char_string_from_c_str(const char c_str[]) {
    CharString s;
    dynarray_from_c_array(char, s, c_str, strlen(c_str));

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

int char_string_find(const CharString* s, const char pat[]) {
    size_t patlen = strlen(pat);
    for (size_t i = 0; i < s->size; i++) {
        if (!strncmp(&s->items[i], pat, patlen)) {
            return i;
        }
    }

    return -1;
}

int char_string_rfind(const CharString* s, const char pat[]) {
    size_t patlen = strlen(pat);
    for (size_t i = s->size; i > 0; i--) {
        if (!strncmp(&s->items[i - 1], pat, patlen)) {
            return i - 1;
        }
    }

    return -1;
}

void char_string_to_buffer(const CharString* s, char out[], size_t size) {
    memcpy(out, s->items, s->size < size ? s->size : size);
}

void char_string_free(CharString* s) {
    dynarray_free(s);
}
