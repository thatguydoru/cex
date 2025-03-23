#ifndef __CEX_H__
#define __CEX_H__

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/////////// Dynamic Array ///////////

#define dynarray(T)      \
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

#define dynarray_from_c_array(T, arr, c_arr, sz)  \
    {                                             \
        dynarray_with_capacity(T, arr, sz);       \
        memcpy(arr.items, c_arr, sz * sizeof(T)); \
        arr.size = sz;                            \
    }

#define dynarray_push(arr, item)                                        \
    {                                                                   \
        if ((arr)->size == (arr)->capacity) {                           \
            size_t sz = sizeof(item);                                   \
            (arr)->capacity *= 2;                                       \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity); \
        }                                                               \
        (arr)->items[(arr)->size++] = item;                             \
    }

#define dynarray_extend(arr, other)                                           \
    {                                                                         \
        size_t sz = sizeof((arr)->items[0]);                                  \
        if ((arr)->capacity - (arr)->size < (other)->size) {                  \
            (arr)->capacity += (other)->capacity;                             \
            (arr)->items = realloc((arr)->items, sz * (arr)->capacity);       \
        }                                                                     \
        memcpy(&(arr)->items[(arr)->size], (other)->items, sz*(other)->size); \
        (arr)->size += (other)->size;                                         \
    }

#define dynarray_free(arr)   \
    {                        \
        free((arr)->items);  \
        (arr)->items = NULL; \
        (arr)->size = 0;     \
        (arr)->capacity = 0; \
    }

/////////// char String ///////////

typedef dynarray(char) CharString;

CharString char_string_with_capacity(size_t capacity);
CharString char_string_default(void);
CharString char_string_from_c_str(const char c_str[]);
void char_string_concat_inplace(CharString* dest, const CharString* src);
CharString char_string_concat(const CharString* a, const CharString* b);
bool char_string_eq(const CharString* a, const CharString* b);
int char_string_find(const CharString* s, const char pat[]);
int char_string_rfind(const CharString* s, const char pat[]);
void char_string_to_buffer(const CharString* s, char out[], size_t size);
void char_string_free(CharString* s);

/////////// Math ///////////

#define min(a, b) ((a) <= (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))

/////////// Panic ///////////

void panic(const char* message);

#endif
