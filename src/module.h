#include <stdio.h>
#include "output.h"
#include "state.h"

#define MODULE_INTERFACE(_) \
    void mod_##_##_init(mod_##_##_t *self, FILE *fp);\
    void mod_##_##_reset(mod_##_##_t *self);\
    void mod_##_##_update(mod_##_##_t *self, const state_t *s);\
    void mod_##_##_output(const mod_##_##_t *self,\
                          const index_t *idx,\
                          const state_t *s,\
                          int num_updates);
