
#pragma once
#include <vector>

/*     SUBROUTINE NNLS  (A,MDA,M,N,B,X,RNORM,W,ZZ,INDEX,MODE) */

/*  Algorithm NNLS: NONNEGATIVE LEAST SQUARES */

/*  The original version of this code was developed by */
/*  Charles L. Lawson and Richard J. Hanson at Jet Propulsion Laboratory */
/*  1973 JUN 15, and published in the book */
/*  "SOLVING LEAST SQUARES PROBLEMS", Prentice-HalL, 1974. */
/*  Revised FEB 1995 to accompany reprinting of the book by SIAM. */

/*     GIVEN AN M BY N MATRIX, A, AND AN M-VECTOR, B,  COMPUTE AN */
/*     N-VECTOR, X, THAT SOLVES THE LEAST SQUARES PROBLEM */

/*                      A * X = B  SUBJECT TO X .GE. 0 */
/*     ------------------------------------------------------------------ */
/*                     Subroutine Arguments */

/*     A(),MDA,M,N     MDA IS THE FIRST DIMENSIONING PARAMETER FOR THE */
/*                     ARRAY, A().   ON ENTRY A() CONTAINS THE M BY N */
/*                     MATRIX, A.           ON EXIT A() CONTAINS */
/*                     THE PRODUCT MATRIX, Q*A , WHERE Q IS AN */
/*                     M BY M ORTHOGONAL MATRIX GENERATED IMPLICITLY BY */
/*                     THIS SUBROUTINE. */
/*     B()     ON ENTRY B() CONTAINS THE M-VECTOR, B.   ON EXIT B() CON- */
/*             TAINS Q*B. */
/*     X()     ON ENTRY X() NEED NOT BE INITIALIZED.  ON EXIT X() WILL */
/*             CONTAIN THE SOLUTION VECTOR. */
/*     RNORM   ON EXIT RNORM CONTAINS THE EUCLIDEAN NORM OF THE */
/*             RESIDUAL VECTOR. */
/*     W()     AN N-ARRAY OF WORKING SPACE.  ON EXIT W() WILL CONTAIN */
/*             THE DUAL SOLUTION VECTOR.   W WILL SATISFY W(I) = 0. */
/*             FOR ALL I IN SET P  AND W(I) .LE. 0. FOR ALL I IN SET Z */
/*     ZZ()     AN M-ARRAY OF WORKING SPACE. */
/*     INDEX()     AN INTEGER WORKING ARRAY OF LENGTH AT LEAST N. */
/*                 ON EXIT THE CONTENTS OF THIS ARRAY DEFINE THE SETS */
/*                 P AND Z AS FOLLOWS.. */

/*                 INDEX(1)   THRU INDEX(NSETP) = SET P. */
/*                 INDEX(IZ1) THRU INDEX(IZ2)   = SET Z. */
/*                 IZ1 = NSETP + 1 = NPP1 */
/*                 IZ2 = N */
/*     MODE    THIS IS A SUCCESS-FAILURE FLAG WITH THE FOLLOWING */
/*             MEANINGS. */
/*             1     THE SOLUTION HAS BEEN COMPUTED SUCCESSFULLY. */
/*             2     THE DIMENSIONS OF THE PROBLEM ARE BAD. */
/*                   EITHER M .LE. 0 OR N .LE. 0. */
/*             3    ITERATION COUNT EXCEEDED.  MORE THAN 3*N ITERATIONS. */

/*     ------------------------------------------------------------------ */
int nnls_(double* a, int* mda, int* m, int* n, double* b, double* x,
   double* rnorm, double* w, double* zz, int* index, int* mode);


// n => n_linear
// m => n_meas

class NonNegativeLeastSquares
{
public:
   NonNegativeLeastSquares(int n_linear, int n_meas) : 
      n_linear(n_linear), n_meas(n_meas)
   {
      w.resize(n_linear);
      zz.resize(n_meas);
      index.resize(n_linear);
   }

   double compute(std::vector<double>& a, int adim, std::vector<double>& b, std::vector<double>& x, double& rnorm)
   {
      nnls_(a.data(), &adim, &n_meas, &n_linear, b.data(), x.data(), &rnorm, w.data(), zz.data(), index.data(), &mode);

      if (mode == 2)
         throw std::runtime_error("Incorrect dimensionality in nonnegative least squares problem");

      return rnorm;
   }

private:

   int n_linear;
   int n_meas;
   int mode;
   double rnorm;

   std::vector<double> w;
   std::vector<double> zz;
   std::vector<int> index;
};