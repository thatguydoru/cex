#include "core.h"

CharString char_string_with_capacity(size_t capacity) {
    CharString s;
    dynarray_with_capacity(char, s, capacity);

    return s;
}

CharString char_string_default(void) {
    return char_string_with_capacity(8);
}

CharString char_string_with_size(const char str[], size_t size) {
    CharString s;
    dynarray_new(char, s, str, size + 1);
    s.items[size] = '\0';

    return s;
}

CharString char_string_new(const char str[]) {
    return char_string_with_size(str, strlen(str) + 1);
}

void char_string_concat_inplace(CharString* dest, cstr src, size_t size) {
    dynarray_extend(dest, src, size + 1);
}

CharString char_string_concat(const CharString* a, cstr src, size_t size) {
    CharString out = char_string_with_capacity(a->capacity + size + 1);
    char_string_concat_inplace(&out, a->items, a->size);
    char_string_concat_inplace(&out, src, size);
    out.items[out.size] = '\0';

    return out;
}

bool char_string_eq(const CharString* a, cstr b, size_t size) {
    if (a->size != size) {
        return false;
    }

    return !strncmp(a->items, b, a->size);
}

MaybeUsize char_string_find(const CharString* s, cstr pat, size_t patlen) {
    if (patlen > s->size) {
        return None(MaybeUsize);
    }
    if (patlen == s->size) {
        return !strncmp(s->items, pat, patlen) ? Some(MaybeUsize, 0) : None(MaybeUsize);
    }
    for (size_t i = 0; i < s->size; i++) {
        size_t rem = &s->items[s->size - 1] - &s->items[i] + 1;
        if (rem < patlen) {
            break;
        }
        if (!strncmp(&s->items[i], pat, patlen)) {
            return Some(MaybeUsize, i);
        }
    }

    return None(MaybeUsize);
}

MaybeUsize char_string_rfind(const CharString* s, cstr pat, size_t patlen) {
    if (patlen > s->size) {
        return None(MaybeUsize);
    }
    if (patlen == s->size) {
        return !strncmp(s->items, pat, patlen) ? Some(MaybeUsize, 0) : None(MaybeUsize);
    }
    for (size_t i = s->size; i > 0; i--) {
        size_t rem = &s->items[i - 1] - &s->items[0] + 1;
        if (rem < patlen) {
            break;
        }
        if (!strncmp(&s->items[i - 1], pat, patlen)) {
            return Some(MaybeUsize, i - 1);
        }
    }

    return None(MaybeUsize);
}

size_t char_string_to_buffer(const CharString* s, char out[], size_t size) {
    size_t sz = min(s->size, size);
    strncpy(out, s->items, sz);

    return sz;
}

void char_string_free(CharString* s) {
    dynarray_free(s);
}

////////// C string utilities ////////// 
 
cstr rstrstr(cstr haystack, cstr needle) {
    size_t sz = strlen(needle);
    for (size_t i = strlen(haystack); i > 0; i--) {
        const char* ptr = &haystack[i - 1];
        if (!strncmp(ptr, needle, sz)) {
            return ptr;
        }
    }

    return NULL;
}
