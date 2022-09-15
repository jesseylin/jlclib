/* Basic testing library */
/* TODO: do error checking, and finish generated main */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "jlib-string.h"

#define assert(e) ((e) ? (true) : printf("\033[31;1;4m ERROR \033[0m | \033[1m assertion '%s' failed \033[0m in file: %s, line: %d\n", #e, __FILE__, __LINE__), false)
#define MAX_LINE_LENGTH 256

void *debug_malloc(size_t sz, int line, char *file)
{
    //    printf("Mallocing pointer on line %d in file %s\n", line, file);
    void *ptr = malloc(sz);
    printf("Malloced pointer %p on line %d in file %s\n", ptr, line, file);
    return ptr;
}
void debug_free(void *ptr, int line, char *file)
{
    printf("Freeing pointer %p on line %d in file %s\n", ptr, line, file);
    free(ptr);
    printf("Freed pointer %p\n", ptr);
    return;
}

bool parse_function(const char *line)
{
    char *p = strchr(line, ')');
    return (bool)p;
}

char *transform_function(const char *line)
{
    // Caller is responsible for freeing returned char *

    /* append boilerplate */
    /* seek beginning of function */
    assert(strchr(line, '('));

    const char *const name_endp = strchr(line, '(');
    char *const fn_name = malloc(strlen(line) * sizeof(fn_name) + 1);
    strncpy(fn_name, line, name_endp - line);
    fn_name[name_endp - line] = '\0';

    char *name_begp = strchr(fn_name, '*');
    if (name_begp == NULL)
    {
        name_begp = strrchr(fn_name, ' ');
    }

    /* prepend test_ */
    struct jString *s[2];
    for (unsigned i = 0; i < 2; i++)
    {
        s[i] = malloc(sizeof(*s[i]));
    }

    s[0] = string_create(s[0], fn_name);
    s[1] = string_create(s[1], "test_");
    s[0] = string_insert(s[0], s[1], name_begp - fn_name);

    struct jString *tail_s = malloc(sizeof(*tail_s));
    tail_s = string_create(tail_s, &line[name_endp - line]);
    tail_s = string_slice(tail_s, tail_s, 0, tail_s->len - 2); // -2 to peel off newline and the ending semicolon

    s[0] = string_cat(s[0], tail_s);

    /* append curly braces */
    struct jString *curly = malloc(sizeof(*curly));
    curly = string_create(curly, "\n{\n}\n");
    s[0] = string_cat(s[0], curly);

    char *c = malloc(s[0]->len * sizeof(*c) + 1);
    c = string_realize(c, s[0]);

    /* free everything */
    for (unsigned i = 0; i < 2; i++)
    {
        string_destroy(s[i]);
    }
    free(fn_name);
    string_destroy(curly);

    return c;
}

void extract_functions(const char *src_filepath, const char *dest_filepath)
{
    // Process functions from headerfile and output to dest_filepath
    // Returns char array of all the function names appended with ();
    char line[MAX_LINE_LENGTH];
    FILE *src = fopen(src_filepath, "r");
    FILE *dest = fopen(dest_filepath, "w");

    while (fgets(line, MAX_LINE_LENGTH, src))
    {
        if (parse_function(line))
        {
            char *out_line = transform_function(line);
            fputs(out_line, dest);
            free(out_line);
        }
        else
        {
            fputs(line, dest);
        }
    }

    fclose(src);
    fclose(dest);

    return;
}

void write_main(const char *fns[])
{
}

char *file_rename(const char *filepath)
{
    struct jString *r = malloc(sizeof(*r));
    struct jString *s = malloc(sizeof(*s));
    r = string_create(r, "test_");
    s = string_create(s, filepath);
    const size_t offset = string_rseekc(s, '/');
    s = string_insert(s, r, offset);

    char *c = malloc(s->len * sizeof(*c) + 1);
    c = string_realize(c, s);

    free(r);
    free(s);
    return c;
}

int test_parse_function(void)
{
    const char fn[] = "void calculate(const int x, const int y);\n";
    assert(parse_function(fn));
    const char not_fn[] = "#define MY_VAR 1\n";
    assert(!(parse_function(not_fn)));
    return 0;
}

int test_transform_function(void)
{
    const char fn[] = "void calculate(const int x, const int y);\n";
    const char *fn_beg = transform_function(fn);
    assert(!strcmp(fn_beg, "void test_calculate(const int x, const int y)\n{\n}\n"));
    return 0;
}

int test_extract_functions(void)
{
    // generate fake headerfile
    const char txt[] = "#include <stdlib.h>\nvoid calculate(const int x, const int y);\nvoid *hello(void);\n";
    const char src_filepath[] = "./src.tmp.c";
    const char dest_filepath[] = "./dest.tmp.c";
    FILE *src = fopen(src_filepath, "w");
    fputs(txt, src);
    fclose(src);

    extract_functions(src_filepath, dest_filepath);

    //    remove(src_filepath);
    //    remove(dest_filepath);
    return 0;
}

int test_rename_file(void)
{
    // generate fake headerfile
    const char txt[] = "test123";
    const char src_filepath[] = "./src.tmp.c";
    const char new_filepath[] = "./test_src.tmp.c";
    FILE *src = fopen(src_filepath, "w");
    fputs(txt, src);
    fclose(src);

    rename(src_filepath, file_rename(src_filepath));
    assert(fopen(new_filepath, "r"));
    return 0;
}

int run_tests(void)
{
    test_parse_function();
    test_transform_function();
    //    test_extract_functions();
    test_rename_file();
    printf("Passed!");
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        run_tests();
        printf("Bad\n");
        return -1;
    }
    else
    {
        const char *const src = argv[1];
        const char *const dest = file_rename(src);

        extract_functions(src, dest);
        return 0;
    }
}
