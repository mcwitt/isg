#ifndef MACROS_H
#define MACROS_H

#define SET(x, n, c) \
    do { int i_; for (i_ = 0; i_ < (n); ++i_) (x)[i_] = (c); } while (0)

#define ZERO(x, n) SET(x, n, 0)

#define IMPLIES(x, y) (!(x) || (y))

#endif
