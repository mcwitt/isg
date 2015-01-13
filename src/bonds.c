#include "bonds.h"

void bonds_init(bonds *b)
{
    int i;

    for (i = 0; i < N; i++)
        b->head[i] = NULL; 
}

void bonds_add(bonds *b, int i, int j, double v)
{
    node *new = malloc(sizeof(node));
    new->j = j;
    new->v = v;
    new->next = b->head[i];
    b->head[i] = new;
}

int bonds_have(bonds *b, int i, int j)
{
    node *next = b->head[i];

    while (next)
    {
        if (next->j == j)
            return 1;

        next = next->next;
    }

    return 0;
}

void bonds_to_list(bonds *b, int *bond_site, double *bond_value)
{
    int i;

    for (i = 0; i < N; i++)
    {
        node *next = b->head[i];

        while (next)
        {
            *(bond_site++) = i;
            *(bond_site++) = next->j;
            *(bond_value++) = next->v;
            next = next->next;
        }
    }
}

void bonds_free(bonds *b)
{
    int i;

    for (i = 0; i < N; i++)
    {
        node *this = b->head[i];

        while (this)
        {
            node *next = this->next;
            free(this);
            this = next;
        }
    }
}

