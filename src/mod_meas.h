#include "exchange.h"
#include "output.h"
#include <stdio.h>

#if REPLICA_EXCHANGE
#define MEAS_FIELDS \
    X(double, u,  "%*e", COL_WIDTH) \
    X(double, q2, "%*e", COL_WIDTH) \
    X(double, q4, "%*e", COL_WIDTH) \
    X(double, ql, "%*e", COL_WIDTH) \
    X(double, rx, "%*g", COL_WIDTH)
#else
#define MEAS_FIELDS \
    X(double, u,  "%*e", COL_WIDTH) \
    X(double, q2, "%*e", COL_WIDTH) \
    X(double, q4, "%*e", COL_WIDTH) \
    X(double, ql, "%*e", COL_WIDTH)
#endif

#define X(type, name, fmt, width) type name[NUM_REPLICAS];
typedef struct { MEAS_FIELDS } mod_meas_t;
#undef X

