#include <stddef.h>

struct jString
{
    const char *ch_arr;
    size_t offset;
    size_t len;
};

struct jString *string_create(struct jString *s, const char *std_str);
void string_destroy(struct jString *);
char *string_realize(char *, const struct jString *);

size_t string_seekc(const struct jString *, const char);
size_t string_rseekc(const struct jString *, const char);
struct jString *string_view(const struct jString *s, const size_t offset, const size_t len);
struct jString *string_insert(struct jString *s1, const struct jString *s2, const size_t offset);
struct jString *string_cat(struct jString *s1, const struct jString *s2);
