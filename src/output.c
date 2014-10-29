#include "output.h"
#include <stddef.h>

void index_print(const index_t *x, double T, FILE *fp)
{
#define X(type, name, fmt, width) fprintf(fp, fmt, width, x->name);
    INDEX_FIELDS
#undef X
    fprintf(fp, "%*g", 9, T);
}

void print_index_header(FILE *fp)
{
#define X(type, name, fmt, width) fprintf(fp, "%*s", width, #name);
    INDEX_FIELDS
#undef X
    fprintf(fp, "%*s", 9, "T");
}

