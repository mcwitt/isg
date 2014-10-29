#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>

#define COL_WIDTH 15

#define INDEX_FIELDS \
    X(int, dec, "%*d", 3)

#define X(type, name, fmt, width) type name;
typedef struct { INDEX_FIELDS } index_t;
#undef X

void index_print(const index_t *x, double T, FILE *fp);

void print_index_header(FILE *fp);

#endif
