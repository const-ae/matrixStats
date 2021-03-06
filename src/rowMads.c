/***************************************************************************
 Public methods:
 SEXP rowMads(SEXP x, ...)

 Authors: Henrik Bengtsson

 Copyright Henrik Bengtsson, 2014
 **************************************************************************/
#include <Rdefines.h>
#include "000.types.h"
#include "rowMads_lowlevel.h"

SEXP rowMads(SEXP x, SEXP dim, SEXP rows, SEXP cols, SEXP constant, SEXP naRm, SEXP hasNA, SEXP byRow) {
  int narm, hasna, byrow;
  SEXP ans;
  R_xlen_t nrow, ncol;
  double scale;

  /* Argument 'x' and 'dim': */
  assertArgMatrix(x, dim, (R_TYPE_INT | R_TYPE_REAL), "x");
  nrow = asR_xlen_t(dim, 0);
  ncol = asR_xlen_t(dim, 1);

  /* Argument 'constant': */
  if (!isNumeric(constant))
    error("Argument 'constant' must be a numeric scale.");
  scale = asReal(constant);

  /* Argument 'naRm': */
  narm = asLogicalNoNA(naRm, "na.rm");

  /* Argument 'hasNA': */
  hasna = asLogicalNoNA(hasNA, "hasNA");

  /* Argument 'rows' and 'cols': */
  R_xlen_t nrows, ncols;
  int rowsType, colsType;
  void *crows = validateIndices(rows, nrow, 0, &nrows, &rowsType);
  void *ccols = validateIndices(cols, ncol, 0, &ncols, &colsType);

  /* Argument 'byRow': */
  byrow = asLogical(byRow);

  if (!byrow) {
    SWAP(R_xlen_t, nrow, ncol);
    SWAP(void*, crows, ccols);
    SWAP(R_xlen_t, nrows, ncols);
    SWAP(int, rowsType, colsType);
  }

  /* R allocate a double vector of length 'nrow'
     Note that 'nrow' means 'ncol' if byrow=FALSE. */
  PROTECT(ans = allocVector(REALSXP, nrows));

  /* Double matrices are more common to use. */
  if (isReal(x)) {
    rowMads_dbl[rowsType][colsType](REAL(x), nrow, ncol, crows, nrows, ccols, ncols, scale, narm, hasna, byrow, REAL(ans));
  } else if (isInteger(x)) {
    rowMads_int[rowsType][colsType](INTEGER(x), nrow, ncol, crows, nrows, ccols, ncols, scale, narm, hasna, byrow, REAL(ans));
  }

  UNPROTECT(1);

  return(ans);
} /* rowMads() */


/***************************************************************************
 HISTORY:
 2015-06-13 [DJ]
  o Supported subsetted computation.
 2014-11-17 [HB]
 o Created from rowMedians.c.
 **************************************************************************/
