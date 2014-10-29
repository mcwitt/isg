#include "module.h"
#include "exchange.h"

#define MEAS_FIELDS \
    X(double, u,    "%*e", COL_WIDTH) \
    X(double, q2,   "%*e", COL_WIDTH) \
    X(double, q4,   "%*e", COL_WIDTH) \
    X(double, ql,   "%*e", COL_WIDTH) \
    X(double, rx,   "%*g", COL_WIDTH)

#define X(type, name, fmt, width) type name;
typedef struct { MEAS_FIELDS } meas_data_t;
#undef X

typedef struct {
    meas_data_t data[NUM_REPLICAS];
    FILE *fp;
} mod_meas_t;

MODULE_INTERFACE(meas);
