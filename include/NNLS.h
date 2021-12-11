#ifndef _NNLS_H
#define _NNLS_H

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define NNLS_MAX(a, b) ((a) >= (b) ? (a) : (b))
#define NNLS_ABS(x) ((x) >= 0 ? (x) : -(x))

int nnls(double **a, int m, int n, double *b, double *x, double *rnorm,
         double *w, double *zz, int *index);

int h12(int mode, int lpivot, int l1, int m, double *u, int u_dim1, double *up,
        double *cm, int ice, int icv, int ncv);

void g1(double a, double b, double *cterm, double *sterm, double *sig);

#endif
