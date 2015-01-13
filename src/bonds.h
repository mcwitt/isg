#ifndef BONDS_H
#define BONDS_H

#include "rng.h"
#include "sample.h"

typedef struct
{
    int j;
    double v;
    void *next;
} node;

typedef struct { node *head[N]; } bonds;

void bonds_init(bonds *b);

void bonds_add(bonds *b, int i, int j, double v);

int bonds_have(bonds *b, int i, int j);

void bonds_to_list(bonds *b, int *bond_site, double *bond_value);

void bonds_free(bonds *b);

#endif
