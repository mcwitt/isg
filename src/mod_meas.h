#include "exchange.h"
#include "output.h"
#include <stdio.h>

#if REPLICA_EXCHANGE
#define MEAS_FIELDS \
    X(double, u,  "%*e", COL_WIDTH_FLOAT) \
    X(double, q2, "%*e", COL_WIDTH_FLOAT) \
    X(double, q4, "%*e", COL_WIDTH_FLOAT) \
    X(double, ql, "%*e", COL_WIDTH_FLOAT) \
    X(double, rx, "%*g", COL_WIDTH_FLOAT)
#else
#define MEAS_FIELDS \
    X(double, u,  "%*e", COL_WIDTH_FLOAT) \
    X(double, q2, "%*e", COL_WIDTH_FLOAT) \
    X(double, q4, "%*e", COL_WIDTH_FLOAT) \
    X(double, ql, "%*e", COL_WIDTH_FLOAT)
#endif

#define X(type, name, fmt, width) type name;
typedef struct { MEAS_FIELDS } meas_data_t;
#undef X

typedef struct { meas_data_t data[NUM_REPLICAS]; } mod_meas_t;

