#ifndef OUTPUT_H
#define OUTPUT_H

#include "sample.h"
#include <math.h>
#include <stdio.h>

#define COL_PAD 3
#define COL_WIDTH_FLOAT 15
#define NUM_DIGITS_N ((int) ceil(LOG_N * M_LN2/M_LN10))

#define INDEX_FIELDS \
    X(int, b, "%*d", 3)

#define X(type, name, fmt, width) type name;
typedef struct { INDEX_FIELDS } index_t;
#undef X

void index_print(const index_t *x, double T, FILE *fp);

void print_index_header(FILE *fp);

#endif
