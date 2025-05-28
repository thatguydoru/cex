#ifndef __CEX_H__
#define __CEX_H__

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

#define Some(maybe_t, ...)                   \
    (maybe_t) {                              \
        .exists = true, .value = __VA_ARGS__ \
    }

#define None(maybe_t)   \
    (maybe_t) {         \
        .exists = false \
    }

#define unwrap_some(m, out)                        \
    if (m.exists) {                                \
        out = &m.value;                            \
    } else {                                       \
        panic("unwrap_some: unwrapped on a None"); \
    }

/////////// Result ///////////

#define Result(T, E) \
    struct {         \
        bool ok;     \
        union {      \
            T data;  \
            E error; \
        } value;     \
    }

#define Ok(result_t, ...)                     \
    (result_t) {                              \
        .ok = true, .value.data = __VA_ARGS__ \
    }

#define Err(result_t, ...)                      \
    (result_t) {                                \
        .ok = false, .value.error = __VA_ARGS__ \
    }

#define unwrap_ok(r, out)                        \
    if (r.ok) {                                  \
        out = &r.value.data;                     \
    } else {                                     \
        panic("unwrap_ok: unwrapped on an Err"); \
    }

#define unwrap_err(r, out)                       \
    if (!r.ok) {                                 \
        out = &r.value.error;                    \
    } else {                                     \
        panic("unwrap_err: unwrapped on an Ok"); \
    }

/////////// Panic ///////////

#define panicf(fmt, ...)                                                         \
    {                                                                            \
        fprintf(stderr, "[PANIC] %s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); \
        _Exit(-1);                                                               \
    }

#define panic(message) panicf("%s\n", (message))

/////////// Dynamic Array ///////////

#define DynArray(T)      \
    struct {             \
        T* items;        \
        size_t size;     \
        size_t capacity; \
    }

#define dynarray_with_capacity(T, arr, cap)                                 \
    {                                                                       \
        arr.capacity = cap;                                                 \
        arr.size = 0;                                                       \
        arr.items = malloc(sizeof(T) * cap);                                \
        if (!arr.items) {                                                   \
            panic("dynamic array allocation failed, may be out of memory"); \
        }                                                                   \
    }

#define dynarray_default(T, arr) dynarray_with_capacity(T, arr, 8)

#define dynarray_new(T, arr, src, sz)           \
    {                                           \
        dynarray_with_capacity(T, arr, sz);     \
        memcpy(arr.items, src, sz * sizeof(T)); \
        arr.size = sz;                          \
    }

#define dynarray_push(arr, item)                                                  \
    {                                                                             \
        if ((arr)->size == (arr)->capacity) {                                     \
            size_t sz = sizeof((arr)->items[0]);                                  \
            (arr)->capacity *= 2;                                                 \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity);           \
            if (!(arr)->items) {                                                  \
                panic("dynamic array reallocation failed, may be out of memory"); \
            }                                                                     \
        }                                                                         \
        (arr)->items[(arr)->size++] = item;                                       \
    }

#define dynarray_extend(arr, other, othersz)                                      \
    {                                                                             \
        size_t sz = sizeof((arr)->items[0]);                                      \
        if ((arr)->capacity - (arr)->size < (othersz)) {                          \
            (arr)->capacity += (othersz);                                         \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity);           \
            if (!(arr)->items) {                                                  \
                panic("dynamic array reallocation failed, may be out of memory"); \
            }                                                                     \
        }                                                                         \
        memmove(&(arr)->items[(arr)->size], (other), sz*(othersz));               \
        (arr)->size += (othersz);                                                 \
    }

#define dynarray_resize(arr)                                                  \
    {                                                                         \
        size_t sz = sizeof((arr)->items[0]);                                  \
        (arr)->capacity = (arr)->size;                                        \
        (arr)->items = realloc((arr)->items, sz * (arr)->capacity);           \
        if (!(arr)->items) {                                                  \
            panic("dynamic array reallocation failed, may be out of memory"); \
        }                                                                     \
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

/////////// Logging ///////////

#define eprintf(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#define eprintln(message) eprintf("%s", message)

/////////// Extras ///////////

#define arrsize(arr) (sizeof(arr) / sizeof(arr[0]))
#define unused(x) (void)x;

#endif
