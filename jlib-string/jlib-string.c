#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "jlib-string.h"

/* Small library for string-building. Note the internal implementation deals with char arrays which are not null-terminated. */
struct jString *string_create(struct jString *s, const char *std_str)
{
    const size_t len = strlen(std_str);
    char *arr_cpy = malloc(len * sizeof(*arr_cpy));
    memcpy(arr_cpy, std_str, len);

    s->ch_arr = arr_cpy;
    s->offset = 0;
    s->len = len;

    return s;
}

void string_destroy(struct jString *str)
{
    free((void *)str->ch_arr);
    str->ch_arr = NULL; // avoid double frees
    free(str);
    str = NULL;
    return;
}

char *string_realize(char *c, const struct jString *s)
{
    // caller responsible for allocating c
    memcpy(c, s->ch_arr + s->offset, s->len * sizeof(*s->ch_arr));
    c[s->len] = '\0';

    return c;
}

size_t string_seekc(const struct jString *s, const char c)
{
    char *p = memchr(s->ch_arr, c, s->len);
    return p - s->ch_arr;
}

size_t string_rseekc(const struct jString *s, const char c)
{
    char *x = malloc(s->len * sizeof(*x) + 1);
    x = string_realize(x, s);

    const char *const p = strrchr(x, c);
    const ptrdiff_t ind = p - x;
    assert(p - x >= 0);

    free(x);
    return ind;
}

struct jString *string_view(const struct jString *s, const size_t offset, const size_t len)
{
    // Returns a new jString which is a slice of s
    // Caller must free
    struct jString *out = malloc(sizeof(*out));

    out->ch_arr = s->ch_arr;
    out->offset = s->offset + offset;
    out->len = len;

    return out;
}

struct jString *string_insert(struct jString *s1, const struct jString *s2, const size_t offset)
{
    // declare new struct?
    const size_t len = s1->len + s2->len;
    char *ch_arr = malloc(len * sizeof(*ch_arr));

    // concatenate char arrays
    const size_t substr_len[3] = {
        offset + 1,
        s2->len,
        s1->len - offset - 1};

    memcpy(ch_arr, s1->ch_arr, substr_len[0]);
    memcpy(ch_arr + substr_len[0], s2->ch_arr, substr_len[1]);
    memcpy(ch_arr + substr_len[0] + substr_len[1], s1->ch_arr + substr_len[0], substr_len[2]);

    // destroy old ch_arr
    free((void *)s1->ch_arr);

    // initialize new struct
    s1->ch_arr = ch_arr;
    s1->offset = 0;
    s1->len = len;

    return s1;
}

struct jString *string_cat(struct jString *s1, const struct jString *s2)
{
    return string_insert(s1, s2, s1->len - 1);
}

int string_run_tests(void)
{
    /* Simple main for testing purposes */
    const char std_str[] = "Hello world!";
    char *c;

    // string_create
    struct jString *str = malloc(sizeof(*str));
    str = string_create(str, std_str);
    assert(str->ch_arr != std_str);
    assert(str->offset == 0);

    // string_realize
    c = malloc((str->len + 1) * sizeof(*c));
    c = string_realize(c, str);
    assert(!strcmp(c, std_str));
    free(c);

    // string_seekc
    assert(string_seekc(str, '!') == 11);

    // string_rseekc
    assert(string_rseekc(str, 'l') == 9);

    // string_view
    struct jString *slice = string_view(str, 6, 5);
    c = malloc((slice->len + 1) * sizeof(*c));
    c = string_realize(c, slice);
    assert(!strcmp(c, "world"));
    free(c);

    struct jString *tmp = slice;
    slice = string_view(tmp, 1, 3);
    free(tmp);
    c = malloc((slice->len + 1) * sizeof(*c));
    c = string_realize(c, slice);
    assert(!strcmp(c, "orl"));

    // string_insert
    struct jString *str1 = malloc(sizeof(*str1));
    struct jString *str2 = malloc(sizeof(*str2));
    str1 = string_create(str1, "abcde");
    str2 = string_create(str2, "xyz");
    str1 = string_insert(str1, str2, 2);
    c = malloc((str1->len + 1) * sizeof(*c));
    c = string_realize(c, str1);
    assert(!strcmp(c, "abcxyzde"));
    free(c);

    // string_cat
    str1 = string_create(str1, "abcde");
    str2 = string_create(str2, "xyz");
    str1 = string_cat(str1, str2);
    c = malloc((str1->len + 1) * sizeof(*c));
    c = string_realize(c, str1);
    assert(!strcmp(c, "abcdexyz"));
    free(c);

    return 0;
}
