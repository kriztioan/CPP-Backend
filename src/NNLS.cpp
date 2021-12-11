/*****************************************************************************

 NNLS.cpp

 http://nccastaff.bournemouth.ac.uk/jmacey/MastersProjects/MSc2010/09SteveTwist/html/files.html

 Version 2009-04-16

 Algorithm NNLS (Non-negative least-squares)

 Implements the Lawson-Hanson NNLS algorithm

 Given an m by n matrix A, and an m-vector B, computes an n-vector X, that
 solves the least squares problem A * X = B , subject to X>=0

 Instead of pointers for working space, NULL can be given to let this function
 to allocate and free the required memory.

 Returns:
 Function returns 0 if succesful, 1, if iteration count exceeded 3*N, or 2 in
 case of invalid problem dimensions or memory allocation error. Parameters: a
 On entry, a[ 0... N ][ 0 ... M ] contains the M by N matrix A. On exit, a[][]
 contains the product matrix Q*A, where Q is an m by n orthogonal matrix
 generated implicitly by this function. m 	Matrix dimension m n 	Matrix
 dimension n b 	On entry, b[] must contain the m-vector B. On exit, b[] contains
 Q*B x 	On exit, x[] will contain the solution vector rnorm 	On exit, rnorm
 contains the Euclidean norm of the residual vector. If NULL is given, no rnorm
 is calculated
 wp 	An n-array of working space, wp[]. On exit, wp[] will contain the dual
 solution vector. wp[i]=0.0 for all i in set p and wp[i]<=0.0 for all i in set
 z. Can be NULL, which causes this algorithm to allocate memory for it. zzp
 An m-array of working space, zz[]. Can be NULL, which causes this algorithm to
 allocate memory for it.
 indexp 	An n-array of working space, index[]. Can be NULL, which causes
 this algorithm to allocate memory for it.

 *****************************************************************************/

#include <NNLS.h>

int nnls(double **a, int m, int n, double *b, double *x, double *rnorm,
         double *wp, double *zzp, int *indexp) {

  int pfeas, ret = 0, iz, jz, iz1, iz2, npp1, *index;
  double d1, d2, sm, up = 0.0, ss, *w, *zz;
  int iter, k, j = 0, l, itmax, izmax = 0, nsetp, ii, jj = 0, ip;
  double temp, wmax, t, alpha, asave, dummy, unorm, ztest, cc;

  /* Check the parameters and data */
  if (m <= 0 || n <= 0 || a == NULL || b == NULL || x == NULL)
    return (2);
  /* Allocate memory for working space, if required */
  if (wp != NULL)
    w = wp;
  else
    w = (double *)calloc(n, sizeof(double));
  if (zzp != NULL)
    zz = zzp;
  else
    zz = (double *)calloc(m, sizeof(double));
  if (indexp != NULL)
    index = indexp;
  else
    index = (int *)calloc(n, sizeof(int));
  if (w == NULL || zz == NULL || index == NULL)
    return (2);

  /* Initialize the arrays INDEX[] and X[] */
  for (k = 0; k < n; k++) {
    x[k] = 0.;
    index[k] = k;
  }
  iz2 = n - 1;
  iz1 = 0;
  nsetp = 0;
  npp1 = 0;

  /* Main loop; quit if all coeffs are already in the solution or */
  /* if M cols of A have been triangularized */
  iter = 0;
  if (n < 3)
    itmax = n * 3;
  else
    itmax = n * n;
  while (iz1 <= iz2 && nsetp < m) {
    /* Compute components of the dual (negative gradient) vector W[] */
    for (iz = iz1; iz <= iz2; iz++) {
      j = index[iz];
      sm = 0.;
      for (l = npp1; l < m; l++)
        sm += a[j][l] * b[l];
      w[j] = sm;
    }

    while (1) {

      /* Find largest positive W[j] */
      for (wmax = 0., iz = iz1; iz <= iz2; iz++) {
        j = index[iz];
        if (w[j] > wmax) {
          wmax = w[j];
          izmax = iz;
        }
      }

      /* Terminate if wmax<=0.; */
      /* it indicates satisfaction of the Kuhn-Tucker conditions */
      if (wmax <= 0.0)
        break;
      iz = izmax;
      j = index[iz];

      /* The sign of W[j] is ok for j to be moved to set P. */
      /* Begin the transformation and check new diagonal element to avoid */
      /* near linear dependence. */
      asave = a[j][npp1];
      h12(1, npp1, npp1 + 1, m, &a[j][0], 1, &up, &dummy, 1, 1, 0);
      unorm = 0.;
      if (nsetp != 0)
        for (l = 0; l < nsetp; l++) {
          d1 = a[j][l];
          unorm += d1 * d1;
        }
      unorm = sqrt(unorm);
      d2 = unorm + (d1 = a[j][npp1], fabs(d1)) * 0.01;
      if ((d2 - unorm) > 0.) {
        /* Col j is sufficiently independent. Copy B into ZZ, update ZZ */
        /* and solve for ztest ( = proposed new value for X[j] ) */
        for (l = 0; l < m; l++)
          zz[l] = b[l];
        h12(2, npp1, npp1 + 1, m, &a[j][0], 1, &up, zz, 1, 1, 1);
        ztest = zz[npp1] / a[j][npp1];
        /* See if ztest is positive */
        if (ztest > 0.)
          break;
      }

      /* Reject j as a candidate to be moved from set Z to set P. Restore */
      /* A[npp1,j], set W[j]=0., and loop back to test dual coeffs again */
      a[j][npp1] = asave;
      w[j] = 0.;
    } /* while(1) */
    if (wmax <= 0.0)
      break;

    /* Index j=INDEX[iz] has been selected to be moved from set Z to set P. */
    /* Update B and indices, apply householder transformations to cols in */
    /* new set Z, zero subdiagonal elts in col j, set W[j]=0. */
    for (l = 0; l < m; ++l)
      b[l] = zz[l];
    index[iz] = index[iz1];
    index[iz1] = j;
    iz1++;
    nsetp = npp1 + 1;
    npp1++;
    if (iz1 <= iz2)
      for (jz = iz1; jz <= iz2; jz++) {
        jj = index[jz];
        h12(2, nsetp - 1, npp1, m, &a[j][0], 1, &up, &a[jj][0], 1, m, 1);
      }
    if (nsetp != m)
      for (l = npp1; l < m; l++)
        a[j][l] = 0.;
    w[j] = 0.;

    /* Solve the triangular system; store the solution temporarily in Z[] */
    for (l = 0; l < nsetp; l++) {
      ip = nsetp - (l + 1);
      if (l != 0)
        for (ii = 0; ii <= ip; ii++)
          zz[ii] -= a[jj][ii] * zz[ip + 1];
      jj = index[ip];
      zz[ip] /= a[jj][ip];
    }

    /* Secondary loop begins here */
    while (++iter < itmax) {
      /* See if all new constrained coeffs are feasible; if not, compute alpha
       */
      for (alpha = 2.0, ip = 0; ip < nsetp; ip++) {
        l = index[ip];
        if (zz[ip] <= 0.) {
          t = -x[l] / (zz[ip] - x[l]);
          if (alpha > t) {
            alpha = t;
            jj = ip - 1;
          }
        }
      }

      /* If all new constrained coeffs are feasible then still alpha==2. */
      /* If so, then exit from the secondary loop to main loop */
      if (alpha == 2.0)
        break;

      /* Use alpha (0.<alpha<1.) to interpolate between old X and new ZZ */
      for (ip = 0; ip < nsetp; ip++) {
        l = index[ip];
        x[l] += alpha * (zz[ip] - x[l]);
      }

      /* Modify A and B and the INDEX arrays to move coefficient i */
      /* from set P to set Z. */
      k = index[jj + 1];
      pfeas = 1;
      do {
        x[k] = 0.;
        if (jj != (nsetp - 1)) {
          jj++;
          for (j = jj + 1; j < nsetp; j++) {
            ii = index[j];
            index[j - 1] = ii;
            g1(a[ii][j - 1], a[ii][j], &cc, &ss, &a[ii][j - 1]);
            for (a[ii][j] = 0., l = 0; l < n; l++)
              if (l != ii) {
                /* Apply procedure G2 (CC,SS,A(J-1,L),A(J,L)) */
                temp = a[l][j - 1];
                a[l][j - 1] = cc * temp + ss * a[l][j];
                a[l][j] = -ss * temp + cc * a[l][j];
              }
            /* Apply procedure G2 (CC,SS,B(J-1),B(J)) */
            temp = b[j - 1];
            b[j - 1] = cc * temp + ss * b[j];
            b[j] = -ss * temp + cc * b[j];
          }
        }
        npp1 = nsetp - 1;
        nsetp--;
        iz1--;
        index[iz1] = k;

        /* See if the remaining coeffs in set P are feasible; they should */
        /* be because of the way alpha was determined. If any are */
        /* infeasible it is due to round-off error. Any that are */
        /* nonpositive will be set to zero and moved from set P to set Z */
        for (jj = 0, pfeas = 1; jj < nsetp; jj++) {
          k = index[jj];
          if (x[k] <= 0.) {
            pfeas = 0;
            break;
          }
        }
      } while (pfeas == 0);

      /* Copy B[] into zz[], then solve again and loop back */
      for (k = 0; k < m; k++)
        zz[k] = b[k];
      for (l = 0; l < nsetp; l++) {
        ip = nsetp - (l + 1);
        if (l != 0)
          for (ii = 0; ii <= ip; ii++)
            zz[ii] -= a[jj][ii] * zz[ip + 1];
        jj = index[ip];
        zz[ip] /= a[jj][ip];
      }
    } /* end of secondary loop */

    if (iter >= itmax) {
      ret = 1;
      break;
    }
    for (ip = 0; ip < nsetp; ip++) {
      k = index[ip];
      x[k] = zz[ip];
    }
  } /* end of main loop */
  /* Compute the norm of the final residual vector */
  sm = 0.;

  if (rnorm != NULL) {
    if (npp1 < m)
      for (k = npp1; k < m; k++)
        sm += (b[k] * b[k]);
    else
      for (j = 0; j < n; j++)
        w[j] = 0.;
    *rnorm = sqrt(sm);
  }

  /* Free working space, if it was allocated here */
  if (wp == NULL)
    free(w);
  if (zzp == NULL)
    free(zz);
  if (indexp == NULL)
    free(index);
  return (ret);
} /* nnls_ */

int h12(int mode, int lpivot, int l1, int m, double *u, int u_dim1, double *up,
        double *cm, int ice, int icv, int ncv) {
  double d1, b, clinv, cl, sm;
  int k, j;

  /* Check parameters */
  if (mode != 1 && mode != 2)
    return (1);
  if (m < 1 || u == NULL || u_dim1 < 1 || cm == NULL)
    assert(0);

  if (lpivot < 0 || lpivot >= l1 || l1 > m)
    return (1);

  /* Function Body */
  cl = NNLS_ABS(u[lpivot * u_dim1]);

  if (mode == 2) { /* Apply transformation I+U*(U**T)/B to cm[] */
    if (cl <= 0.)
      return (0);
  } else { /* Construct the transformation */

    /* Trying to compensate overflow */
    for (j = l1; j < m; j++) { // Computing MAX
      cl = NNLS_MAX(NNLS_ABS(u[j * u_dim1]), cl);
    }
    // zero vector?

    if (cl <= 0.)
      return (0);

    clinv = 1.0 / cl;

    // Computing 2nd power
    d1 = u[lpivot * u_dim1] * clinv;
    sm = d1 * d1;

    for (j = l1; j < m; j++) {
      d1 = u[j * u_dim1] * clinv;
      sm += d1 * d1;
    }
    cl *= sqrt(sm);

    if (u[lpivot * u_dim1] > 0.)
      cl = -cl;
    up[0] = u[lpivot * u_dim1] - cl;
    u[lpivot * u_dim1] = cl;
  }

  // no vectors where to apply? only change pivot vector!

  b = up[0] * u[lpivot * u_dim1];

  /* b must be nonpositive here; if b>=0., then return */
  if (b == 0)
    return (0);

  // ok, for all vectors we want to apply
  for (j = 0; j < ncv; j++) {
    // take s = c[p,j]*h + sigma(i=l..m){ c[i,j] *v [ i ] }
    sm = cm[lpivot * ice + j * icv] * (up[0]);
    for (k = l1; k < m; k++) {
      sm += cm[k * ice + j * icv] * u[k * u_dim1];
    }

    if (sm != 0.0) {
      sm *= (1 / b);

      cm[lpivot * ice + j * icv] += sm * (up[0]);
      for (k = l1; k < m; k++) {
        cm[k * ice + j * icv] += u[k * u_dim1] * sm;
      }
    }
  }

  return (0);
} /* lss_h12 */

void g1(double a, double b, double *cterm, double *sterm, double *sig) {
  double d1, xr, yr;

  if (fabs(a) > fabs(b)) {
    xr = b / a;
    d1 = xr;
    yr = sqrt(d1 * d1 + 1.);
    d1 = 1. / yr;
    *cterm = (a >= 0.0 ? fabs(d1) : -fabs(d1));
    *sterm = (*cterm) * xr;
    *sig = fabs(a) * yr;
  } else if (b != 0.) {
    xr = a / b;
    d1 = xr;
    yr = sqrt(d1 * d1 + 1.);
    d1 = 1. / yr;
    *sterm = (b >= 0.0 ? fabs(d1) : -fabs(d1));
    *cterm = (*sterm) * xr;
    *sig = fabs(b) * yr;
  } else {
    *sig = 0.;
    *cterm = 0.;
    *sterm = 1.;
  }
} /* g1 */