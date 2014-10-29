#ifndef MODULE_H
#define MODULE_H

#include <stdio.h>
#include "output.h"
#include "state.h"

#include "mod_meas.h"
#include "mod_hist.h"
/*#include "mod_modes.h"*/
#include "mod_corr.h"
#include "mod_spinstate.h"

#define MODULES(_, ...)      \
    _(meas, __VA_ARGS__)     \
    _(hist, __VA_ARGS__)     \
    /*_(modes, __VA_ARGS__)*/    \
    _(corr, __VA_ARGS__)\
    _(spinstate, __VA_ARGS__)\
    

#define MODULE_DECLARE(name, dummy) mod_##name##_t name;

#define MODULE_INIT(name, mods, files) \
    mod_##name##_init(&mods.name, files->name);

#define MODULE_RESET(name, mods) mod_##name##_reset(&mods.name);
#define MODULE_UPDATE(name, mods, s) mod_##name##_update(&mods.name, s);

#define MODULE_OUTPUT(name, mods, s, idx, num_updates, files) \
    mod_##name##_output(&mods.name, s, idx, num_updates, files->name);

#define MODULE_INTERFACE(_, dummy) \
    void mod_##_##_init(mod_##_##_t *self, FILE *fp);\
    void mod_##_##_reset(mod_##_##_t *self);\
    void mod_##_##_update(mod_##_##_t *self, const state_t *s);\
    void mod_##_##_output(const mod_##_##_t *self,\
                          const state_t *s,\
                          const index_t *idx,\
                          int num_updates, \
                          FILE *fp);

#define DECLARE_STRUCT_MEMBER(name, type) type name;

typedef struct { MODULES(MODULE_DECLARE) } modules_t;
typedef struct { MODULES(DECLARE_STRUCT_MEMBER, FILE *) } output_files_t;

MODULES(MODULE_INTERFACE)
#endif
