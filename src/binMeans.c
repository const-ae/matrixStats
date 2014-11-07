/***************************************************************************
 Public methods:
 binMeans(SEXP y, SEXP x, SEXP bx, SEXP retCount, SEXP right)

 Copyright Henrik Bengtsson, 2012-2013
 **************************************************************************/
/* Include R packages */
#include <Rdefines.h> 
#include <R.h>
#include <R_ext/Error.h>

#define BIN_BY 'L'
#include "binMeans-BINBY-template.h"

#define BIN_BY 'R'
#include "binMeans-BINBY-template.h"


SEXP binMeans(SEXP y, SEXP x, SEXP bx, SEXP retCount, SEXP right) {
  SEXP ans = NILSXP, count = NILSXP;
  int nx, ny, nbins;
  int closedRight, retcount;
  int *count_ptr = NULL;

  /* Argument 'y': */
  if (!isVector(y))
    error("Argument 'y' must be a vector.");
  ny = Rf_length(y);

  /* Argument 'x': */
  if (!isVector(x))
    error("Argument 'x' must be a vector.");
  nx = Rf_length(x);
  if (nx != ny) {
    error("Argument 'y' and 'x' are of different lengths: %d != %d", ny, nx);
  }

  /* Argument 'bx': */
  if (!isVector(bx))
    error("Argument 'bx' must be a vector.");
  nbins = Rf_length(bx)-1;

  /* Argument 'right': */
  if (!isLogical(right))
    error("Argument 'right' must be logical.");
  closedRight = asLogical(right);

  /* Argument 'retCount': */
  if (!isLogical(retCount))
    error("Argument 'right' must be logical.");
  retcount = asLogical(retCount);
  if (retcount != TRUE && retcount != FALSE) {
    error("Argument 'retCount' must be either TRUE or FALSE.");
  } 

  PROTECT(ans = allocVector(REALSXP, nbins));
  if (retcount) {
    PROTECT(count = allocVector(INTSXP, nbins));
    count_ptr = INTEGER(count);
  }

  if (closedRight == 0) {
    binMeans_L(REAL(y), ny, REAL(x), nx, REAL(bx), nbins, REAL(ans), count_ptr);
  } else if (closedRight == 1) {
    binMeans_R(REAL(y), ny, REAL(x), nx, REAL(bx), nbins, REAL(ans), count_ptr);
  } else {
    error("Unknown value of argument 'right': %d", closedRight);
  }

  if (retcount) {
    setAttrib(ans, install("count"), count);
    UNPROTECT(1); // 'count'
  }
  UNPROTECT(1); // 'ans'

  return ans;


  return(ans);
} // binMeans()



/***************************************************************************
 HISTORY:
 2014-10-06 [HB]
  o CLEANUP: All argument validation is now done by the high-level C API.
 2014-06-02 [HB]
  o CLEANUP: Removed unused variable in binMeans().
 2013-10-08 [HB]
  o Now binCounts() calls binCounts_<Lr|lR>().
 2013-05-10 [HB]
  o SPEEDUP: binMeans() no longer tests in every iteration (=for every
    data point) whether the last bin has been reached or not.
 2012-10-10 [HB]
  o BUG FIX: binMeans() would return random/garbage means/counts for
    bins that were beyond the last data point.
  o BUG FIX: In some cases binMeans() could try to go past the last bin.
 2012-10-03 [HB]
  o Created binMeans(), which was adopted from from code proposed by
    Martin Morgan (Fred Hutchinson Cancer Research Center, Seattle) as
    a reply to HB's R-devel thread 'Fastest non-overlapping binning mean
    function out there?' on Oct 3, 2012.
 **************************************************************************/
