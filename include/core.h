#ifndef __CEX_CORE_H__
#define __CEX_CORE_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/////////// Maybe ///////////

#define Maybe(T)     \
    struct {         \
        bool exists; \
        T value;     \
    }

#define Some(maybe_t, val)           \
    (maybe_t) {                      \
        .exists = true, .value = val \
    }

#define None(maybe_t)   \
    (maybe_t) {         \
        .exists = false \
    }

typedef Maybe(size_t) MaybeIndex;

/////////// Result ///////////

#define Result(T, E) \
    struct {         \
        bool ok;     \
        union {      \
            T data;  \
            E error; \
        } value;     \
    }

#define Ok(result_t, val)             \
    (result_t) {                      \
        .ok = true, .value.data = val \
    }

#define Err(result_t, err)              \
    (result_t) {                        \
        .ok = false, .value.error = err \
    }

/////////// Panic ///////////

#define panicf(fmt, ...)                                                         \
    {                                                                            \
        fprintf(stderr, "[PANIC] %s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); \
        _Exit(1);                                                                \
    }

#define panic(message) panicf("%s\n", (message))

/////////// Dynamic Array ///////////

#define DynArray(T)      \
    struct {             \
        T* items;        \
        size_t size;     \
        size_t capacity; \
    }

#define dynarray_with_capacity(T, arr, cap)  \
    {                                        \
        arr.capacity = cap;                  \
        arr.size = 0;                        \
        arr.items = malloc(sizeof(T) * cap); \
    }

#define dynarray_default(T, arr) dynarray_with_capacity(T, arr, 8)

#define dynarray_new(T, arr, c_arr, sz)           \
    {                                             \
        dynarray_with_capacity(T, arr, sz);       \
        memcpy(arr.items, c_arr, sz * sizeof(T)); \
        arr.size = sz;                            \
    }

#define dynarray_is_init(arr) (bool)(arr)->capacity

#define dynarray_with_capacity_once(T, arr, cap) \
    {                                            \
        if (!dynarray_is_init(&arr)) {           \
            dynarray_with_capacity(T, arr);      \
        }                                        \
    }

#define dynarray_default_once(T, arr)  \
    {                                  \
        if (!dynarray_is_init(&arr)) { \
            dynarray_default(T, arr);  \
        }                              \
    }

#define dynarray_push(arr, item)                                        \
    {                                                                   \
        if ((arr)->size == (arr)->capacity) {                           \
            size_t sz = sizeof((arr)->items[0]);                        \
            (arr)->capacity *= 2;                                       \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity); \
        }                                                               \
        (arr)->items[(arr)->size++] = item;                             \
    }

#define dynarray_extend(arr, other, othersz)                            \
    {                                                                   \
        size_t sz = sizeof((arr)->items[0]);                            \
        if ((arr)->capacity - (arr)->size < (othersz)) {                \
            (arr)->capacity += (othersz);                               \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity); \
        }                                                               \
        memmove(&(arr)->items[(arr)->size], (other), sz*(othersz));     \
        (arr)->size += (othersz);                                       \
    }

#define dynarray_resize(arr)                                        \
    {                                                               \
        size_t sz = sizeof((arr)->items[0]);                        \
        (arr)->capacity = (arr)->size;                              \
        (arr)->items = realloc((arr)->items, sz * (arr)->capacity); \
    }

#define dynarray_free(arr)   \
    {                        \
        free((arr)->items);  \
        (arr)->items = NULL; \
        (arr)->size = 0;     \
        (arr)->capacity = 0; \
    }

/////////// Math ///////////

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))
#define clamp(lo, hi, val) ((lo) > (val) ? (lo) : (hi) < (val) ? (hi) : (val))

/////////// char String ///////////

typedef DynArray(char) CharString;

CharString char_string_with_capacity(size_t capacity);
CharString char_string_default(void);
CharString char_string_new(const char c_str[]);
void char_string_resize(CharString* s);
void char_string_concat_inplace(CharString* dest, const CharString* src);
CharString char_string_concat(const CharString* a, const CharString* b);
bool char_string_eq(const CharString* a, const CharString* b);
MaybeIndex char_string_find(const CharString* s, const char pat[], size_t patlen);
MaybeIndex char_string_rfind(const CharString* s, const char pat[], size_t patlen);
size_t char_string_to_buffer(const CharString* s, char out[], size_t size);
void char_string_free(CharString* s);

/////////// Stack-allocated Array Utilities ///////////

#define arrsize(arr) sizeof(arr) / sizeof(arr[0])

/////////// IO ///////////

#define eprintf(fmt, ...) fprintf(stderr, "[ERROR] " fmt, __VA_ARGS__)
#define eprintln(message) eprintf("%s", message)

#endif
