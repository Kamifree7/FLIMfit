
#define CMINPACK_NO_DLL

#define INVALID_INPUT -1

#include "VariableProjector.h"

#include "cminpack.h"
#include <math.h>
#include "util.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>



VariableProjector::VariableProjector(FitModel* model, int smax, int l, int nl, int nmax, int ndim, int p, double *t) : 
    model(model), smax(smax), l(l), nl(nl), nmax(nmax), ndim(ndim), p(p), t(t)
{

   // Set up buffers for levmar algorithm
   //---------------------------------------------------
   int buf_dim = max(1,nl);
   
   fjac = new double[buf_dim * buf_dim];
   diag = new double[buf_dim];
   qtf  = new double[buf_dim];
   wa1  = new double[buf_dim];
   wa2  = new double[buf_dim];
   wa3  = new double[buf_dim];
   wa4  = new double[buf_dim];
   ipvt = new int[buf_dim];

   for(int i=0; i<nl; i++)
      diag[i] = 1;


   // Set up buffers for variable projection
   //--------------------------------------------------

   a   = new double[ nmax * ( l + smax ) ]; //free ok
   b   = new double[ ndim * ( p + 3 ) ]; //free ok
   u   = new double[ l ];
   kap = new double[ nl + 1 ];

   lp1 = l+1;


   Init();
   
}

VariableProjector::~VariableProjector()
{

   delete[] fjac;
   delete[] diag;
   delete[] qtf;
   delete[] wa1;
   delete[] wa2;
   delete[] wa3;
   delete[] wa4;
   delete[] ipvt;

   delete[] a;
   delete[] b;
   delete[] u;
   delete[] kap;
   
}


int VariableProjectorCallback(void *p, int m, int n, const double *x, double *fnorm, double *fjrow, int iflag)
{
   VariableProjector *vp = (VariableProjector*) p;
   return vp->varproj_local(m, n, x, fnorm, fjrow, iflag);
}


int VariableProjector::Init()
{
   int j, k, inckj, p_inc;

   // Check for valid input
   //----------------------------------

   if  (!(             l >= 0
          &&          nl >= 0
          && (nl<<1) + 3 <= ndim
          &&           n <  nmax
          &&           n <  ndim
          && !(nl == 0 && l == 0)))
   {
      return INVALID_INPUT;
   }


   // Get inc matrix and check for valid input
   // Determine number of constant functions
   //------------------------------------------

   nconp1 = l+1;
   philp1 = l == 0;
   p_inc = 0;

   if ( l > 0 && nl > 0 )
   {
      model->SetupIncMatrix(inc);

      p_inc = 0;
      for (j = 0; j < lp1; ++j) 
      {
         if (p_inc == 0) 
            nconp1 = j + 1;
         for (k = 0; k < nl; ++k) 
         {
            inckj = inc[k + j * 12];
            if (inckj != 0 && inckj != 1)
               break;
            if (inckj == 1)
               p_inc++;
         }
      }

      // Determine if column L+1 is in the model
      //---------------------------------------------
      philp1 = false;
      for (k = 0; k < nl; ++k) 
         philp1 = philp1 | (inc[k + lp1 * 12] == 1); 
   }

   if (p_inc != p)
      return INVALID_INPUT;

   ncon = nconp1 - 1;

   return 0;
}


int VariableProjector::Fit(int s, int n, float* y, float *w, int* irf_idx, double *alf, double *lin_params, int thread, int itmax, double chi2_factor, int& niter, int &ierr, double& c2)
{

   this->n = n;
   this->s = s;
   this->y = y;
   this->w = w;
   this->irf_idx = irf_idx;
   this->cur_chi2 = &c2;
   this->chi2_factor = chi2_factor / (n - ((double)nl)/s - l);

   int lnls1 = l + s + nl + 1;
   int lp1   = l + 1;
   int nsls1 = n * s - l * (s - 1);
   

   this->thread = thread;

   double ftol = sqrt(dpmpar(1));
   double xtol = sqrt(dpmpar(1));
   double gtol = 0.;
   double factor = 1;

   int    maxfev = 100;

   int nfev, info;
   double rnorm; 

   info = lmstx(VariableProjectorCallback, (void*) this, nsls1, nl, alf, fjac, nl,
                 ftol, xtol, gtol, itmax, diag, 1, factor, -1,
                 &nfev, &niter, &rnorm, ipvt, qtf, wa1, wa2, wa3, wa4 );

   if (info < 0)
      ierr = info;
   else
      ierr = niter;
   return 0;

}


double VariableProjector::d_sign(double *a, double *b)
{
   double x;
   x = (*a >= 0 ? *a : - *a);
   return( *b >= 0 ? x : -x);
}

int VariableProjector::varproj(int nsls1, int nls, const double *alf, double *rnorm, double *fjrow, int iflag)
{
   int j, k, kp1, i__1, i__2;
   int lastca, firstca, firstcb, firstr;

   int isel;
   int d_idx = -1;

   int     lnls = l + nl + s;
   int     lps  = l + s;

   double *r__  = a + l * n;

//   if (terminate)
//      return -9;

   // Matrix dimensions
   int r_dim1 = n;
   int y_dim1 = nmax;
   int a_dim1 = n;
   int b_dim1 = ndim;
   int t_dim1 = nmax;

   double d__1;
   double rn;

   double beta, acum;
   double alpha;

/*     ============================================================== */

/*        COMPUTE THE NORM OF THE RESIDUAL (IF ISEL = 1 OR 2), OR THE */
/*        (N-L) X NL X S DERIVATIVE OF THE MODIFIED RESIDUAL (N-L) BY S */
/*        MATRIX Q2*Y (IF ISEL = 1 OR 3).  HERE Q * PHI = TRI, I.E., */

/*         L     ( Q1 ) (     .   .        )   (TRI . R1 .  F1  ) */
/*               (----) ( PHI . Y . D(PHI) ) = (--- . -- . ---- ) */
/*         N-L   ( Q2 ) (     .   .        )   ( 0  . R2 .  F2  ) */

/*                 N       L    S      P         L     S     P */

/*        WHERE Q IS N X N ORTHOGONAL, AND TRI IS L X L UPPER TRIANGULAR. */
/*        THE NORM OF THE RESIDUAL = FROBENIUS NORM(R2), AND THE DESIRED */
/*        DERIVATIVE ACCORDING TO REF. (5), IS */
/*                                                 -1 */
/*                    D(Q2 * Y) = -Q2 * D(PHI)* TRI  * Q1* Y. */

/*        THE THREE-TENSOR DERIVATIVE IS STORED IN COLUMNS L+S+1 THROUGH */
/*        L+S+NL AND ROWS L+1 THROUGH S*N - (S-1)*L OF THE MATRIX A. */
/*        THE MATRIX SLAB OF THE DERIVATIVE CORRESPONDING TO THE K'TH */
/*        RIGHT HAND SIDE (FOR K=1,2,...,S) IS IN ROWS L+(K-1)*(N-L)+1 */
/*        THROUGH L+K*(N-L). */

/*     .................................................................. */


   isel = iflag + 1;
   if (isel > 3)
   {
      d_idx = isel - 3;
      
      jacb_row(s, kap, r__, d_idx, rnorm, fjrow);
      return iflag;
   }


   if (isel == 1)
   {
      firstca = 0;
      lastca = lps;
      firstcb = 0;
      firstr = l;
      i__1 = 1;
            
      model->ada(a, b, kap, alf, 0, 1, thread);
   }
   else
   {
      i__1 = min(isel,3);
      model->ada(a, b, kap, alf, 0, i__1, thread);

      if (isel > 2)
      {
         // *isel = 3 or 4
         firstcb = 0;
         firstca = -1;
         firstr = (4 - isel) * l;
      }
      else
      {
         // *isel = 2
         firstca = ncon;
         firstcb = -1;
      }
   }

   if (isel < 3)
   {
      // *isel = 1 or 2
      if (!philp1)
      {
         // Store the data in r__
         #pragma omp parallel for
         for (j = 0; j < s; ++j)
            for (int i = 0; i < n; ++i)
               r__[i + j * r_dim1] = y[i + j * y_dim1];
      }
      else
      {
         // Store the data in r__, subtracting the column l+1 which does not
         // have a linear parameter
         #pragma omp parallel for
         for(j=s-1; j > 0; --j)
            for(int i=0; i < n; ++i)
               r__[i + j * r_dim1] = y[i + j * y_dim1] - r__[i + r_dim1];
        
         for(int i=0; i < n; ++i)
            r__[i + r_dim1] = y[i + y_dim1] - r__[i + r_dim1];
      }
   }
    
   // Weight columns
   if (w != NULL)
   { 
      if (firstca >= 0)
      {
         #pragma omp parallel for
         for (j = firstca; j < lps; ++j)
            for (int i = 0; i < n; ++i)
               a[i + j * a_dim1] *= w[i];
      }
      if (firstcb >= 0)
      {
         for (j = firstcb; j < p; ++j)
            for (int i = 0; i< n; ++i)
               b[i + j * b_dim1] *= w[i];
      }
   }
   

/*           COMPUTE ORTHOGONAL FACTORIZATIONS BY HOUSEHOLDER */
/*           REFLECTIONS.  IF ISEL = 1 OR 2, REDUCE PHI (STORED IN THE */
/*           FIRST L COLUMNS OF THE MATRIX A) TO UPPER TRIANGULAR FORM, */
/*           (Q*PHI = TRI), AND TRANSFORM Y (STORED IN COLUMNS L+1 */
/*           THROUGH L+S), GETTING Q*Y = R.  IF ISEL = 1, ALSO TRANSFORM */
/*           J = D PHI (STORED IN COLUMNS L+S+1 THROUGH L+P+S OF THE */
/*           MATRIX A), GETTING Q*J = F.  IF ISEL = 3 OR 4, PHI HAS */
/*           ALREADY BEEN REDUCED, TRANSFORM ONLY J.  TRI, R, AND F */
/*           OVERWRITE PHI, Y, AND J, RESPECTIVELY, AND A FACTORED FORM */
/*           OF Q IS SAVED IN U AND THE LOWER TRIANGLE OF PHI. */

   if (l > 0)
   {
      // Compute orthogonal factorisations by householder reflection (phi)
      for (k = 0; k < l; ++k) 
      {
         kp1 = k + 1;

         // If *isel=1 or 2 reduce phi (first l columns of a) to upper triangular form
         if (isel <= 2 && !(isel == 2 && k<ncon))
         {
            i__2 = n - k;
            d__1 = enorm(i__2, &a[k + k * a_dim1]);
            alpha = d_sign(&d__1, &a[k + k * a_dim1]);
            u[k] = a[k + k * a_dim1] + alpha;
            a[k + k * a_dim1] = -alpha;
            firstca = kp1;
            if (alpha == (float)0.)
            {
               isel = -8;
               goto L99;
            }
         }

         beta = -a[k + k * a_dim1] * u[k];

         // Compute householder reflection of phi
         if (firstca >= 0)
         {
            for (j = firstca; j < l; ++j)
            {
               acum = u[k] * a[k + j * a_dim1];

               for (int i = kp1; i < n; ++i) 
                  acum += a[i + k * a_dim1] * a[i + j * a_dim1];
               acum /= beta;

               a[k + j * a_dim1] -= u[k] * acum;
               for (int i = kp1; i < n; ++i) 
                  a[i + j * a_dim1] -= a[i + k * a_dim1] * acum;
            }
         }

      }

      for (k = 0; k < l; ++k) 
      {
         kp1 = k + 1;

         beta = -a[k + k * a_dim1] * u[k];

         // Transform Y, getting Q*Y=R 
         if (firstca >= 0)
         {
            for (j = l; j < lps; ++j)
            {
               acum = u[k] * a[k + j * a_dim1];

               for (int i = kp1; i < n; ++i) 
                  acum += a[i + k * a_dim1] * a[i + j * a_dim1];
               acum /= beta;

               a[k + j * a_dim1] -= u[k] * acum;
               for (int i = kp1; i < n; ++i) 
                  a[i + j * a_dim1] -= a[i + k * a_dim1] * acum;
            }
         }

         // Transform J=D(phi)
         if (firstcb >= 0) 
         {
            for (j = firstcb; j < p; ++j)
            {
               acum = u[k] * b[k + j * b_dim1];
               for (int i = k; i < n; ++i) 
                  acum += a[i + k * a_dim1] * b[i + j * b_dim1];
               acum /= beta;

               b[k + j * b_dim1] -= u[k] * acum;
               for (int i = k; i < n; ++i) 
                  b[i + j * b_dim1] -= a[i + k * a_dim1] * acum;
            }
         }
      }
   }

   if (isel < 3)
   {
   /*           COMPUTE THE FROBENIUS NORM OF THE RESIDUAL MATRIX: */
   
      *rnorm = 0.0;
      rn = 0;

      int nml = n-l;
      #pragma omp parallel for reduction(+: rn) private(d__1)  
      for (j = 0; j < s; ++j) 
      {
         d__1 = enorm(nml, &r__[l + j * r_dim1]);
         rn += d__1 * d__1;
      }

      *cur_chi2 = rn * chi2_factor / s;

      rn += kap[0] * kap[0];
      *rnorm = sqrt(rn);
   
   }
   else
   {
   /*           F2 IS NOW CONTAINED IN ROWS L+1 TO N AND COLUMNS L+S+1 TO */
   /*           L+P+S OF THE MATRIX A.  NOW SOLVE THE S (L X L) UPPER */
   /*           TRIANGULAR SYSTEMS TRI*BETA(J) = R1(J) FOR THE LINEAR */
   /*           PARAMETERS BETA.  BETA OVERWRITES R1. */
   
      if (l != 0)
      {   
         #pragma omp parallel for
         for (j = 0; j < s; ++j) 
            bacsub_(n, l, a, &r__[j * r_dim1]);
      }

      jacb_row(s, kap, r__, 0, rnorm, fjrow);
      
   }

L99:
   if (isel < 0)
      iflag = isel;
    return iflag;
}





int VariableProjector::varproj_local(int nsls1, int nls, const double *alf, double *rnorm, double *fjrow, int iflag)
{
   int j, k, m, kp1, i__1, i__2;
   int firstca, firstcb, firstr;

   int isel;
   int d_idx = -1;

   int     lnls = l + nl + s;
   int     lps  = l + s;

   double *r__  = a + l * n;

//   if (terminate)
//      return -9;

   // Matrix dimensions
   int r_dim1 = n;
   int y_dim1 = nmax;
   int a_dim1 = n;
   int b_dim1 = ndim;
   int t_dim1 = nmax;

   double d__1;
   double rn;

   double beta, acum;
   double alpha;

/*     ============================================================== */

/*        COMPUTE THE NORM OF THE RESIDUAL (IF ISEL = 1 OR 2), OR THE */
/*        (N-L) X NL X S DERIVATIVE OF THE MODIFIED RESIDUAL (N-L) BY S */
/*        MATRIX Q2*Y (IF ISEL = 1 OR 3).  HERE Q * PHI = TRI, I.E., */

/*         L     ( Q1 ) (     .   .        )   (TRI . R1 .  F1  ) */
/*               (----) ( PHI . Y . D(PHI) ) = (--- . -- . ---- ) */
/*         N-L   ( Q2 ) (     .   .        )   ( 0  . R2 .  F2  ) */

/*                 N       L    S      P         L     S     P */

/*        WHERE Q IS N X N ORTHOGONAL, AND TRI IS L X L UPPER TRIANGULAR. */
/*        THE NORM OF THE RESIDUAL = FROBENIUS NORM(R2), AND THE DESIRED */
/*        DERIVATIVE ACCORDING TO REF. (5), IS */
/*                                                 -1 */
/*                    D(Q2 * Y) = -Q2 * D(PHI)* TRI  * Q1* Y. */

/*        THE THREE-TENSOR DERIVATIVE IS STORED IN COLUMNS L+S+1 THROUGH */
/*        L+S+NL AND ROWS L+1 THROUGH S*N - (S-1)*L OF THE MATRIX A. */
/*        THE MATRIX SLAB OF THE DERIVATIVE CORRESPONDING TO THE K'TH */
/*        RIGHT HAND SIDE (FOR K=1,2,...,S) IS IN ROWS L+(K-1)*(N-L)+1 */
/*        THROUGH L+K*(N-L). */

/*     .................................................................. */


   isel = iflag + 1;
   if (isel > 3)
   {
      d_idx = isel - 3;
      
      jacb_row(s, kap, r__, d_idx, rnorm, fjrow);
      return iflag;
   }


   for (int j=s-1; j>=0; j--)
   {

      if (isel == 1)
      {
         firstca = 0;
         firstcb = 0;
         firstr = l;
         i__1 = 1;

      }
      else
      {
         i__1 = min(isel,3);

         if (isel > 2)
         {
            // *isel = 3 or 4
            firstcb = 0;
            firstca = -1;
            firstr = (4 - isel) * l;
         }
         else
         {
            // *isel = 2
            firstca = ncon;
            firstcb = -1;
         }
      }

      model->ada(a, b, kap, alf, irf_idx[j], i__1, thread);

      if (isel < 3)
      {
         // *isel = 1 or 2
         if (!philp1)
         {
            for (int i = 0; i < n; ++i)
               r__[i + j * r_dim1] = y[i + j * y_dim1];
         }
         else
         {
            // Store the data in r__, subtracting the column l+1 which does not
            // have a linear parameter
            for(int i=0; i < n; ++i)
               r__[i + j * r_dim1] = y[i + j * y_dim1] - r__[i + r_dim1];
         }
      }

      // Weight columns
      if (w != NULL)
      { 
         if (firstca >= 0)
         {
            for (m = firstca; m < l; ++m)
               for (int i = 0; i < n; ++i)
                  a[i + m * a_dim1] *= w[i];
            for (int i = 0; i < n; ++i)
               r__[i + j * r_dim1] *= w[i];

         }
         if (firstcb >= 0)
         {
            for (m = firstcb; m < p; ++m)
               for (int i = 0; i< n; ++i)
                  b[i + m * b_dim1] *= w[i];
         }
      }


   
      if (l > 0)
      {
         // Compute orthogonal factorisations by householder reflection (phi)
         for (k = 0; k < l; ++k) 
         {
            kp1 = k + 1;

            // If *isel=1 or 2 reduce phi (first l columns of a) to upper triangular form
            if (isel <= 2 && !(isel == 2 && k<ncon))
            {
               i__2 = n - k;
               d__1 = enorm(i__2, &a[k + k * a_dim1]);
               alpha = d_sign(&d__1, &a[k + k * a_dim1]);
               u[k] = a[k + k * a_dim1] + alpha;
               a[k + k * a_dim1] = -alpha;
               firstca = kp1;
               if (alpha == (float)0.)
               {
                  isel = -8;
                  goto L99;
               }
            }

            beta = -a[k + k * a_dim1] * u[k];

            // Compute householder reflection of phi
            if (firstca >= 0)
            {
               for (m = firstca; m < l; ++m)
               {
                  acum = u[k] * a[k + m * a_dim1];

                  for (int i = kp1; i < n; ++i) 
                     acum += a[i + k * a_dim1] * a[i + m * a_dim1];
                  acum /= beta;

                  a[k + m * a_dim1] -= u[k] * acum;
                  for (int i = kp1; i < n; ++i) 
                     a[i + m * a_dim1] -= a[i + k * a_dim1] * acum;
               }
            }

         }

         for (k = 0; k < l; ++k) 
         {
            kp1 = k + 1;

            beta = -a[k + k * a_dim1] * u[k];

            // Transform Y, getting Q*Y=R 
            if (firstca >= 0)
            {
               int jpl = j+l;
               acum = u[k] * a[k + jpl * a_dim1];

               for (int i = kp1; i < n; ++i) 
                  acum += a[i + k * a_dim1] * a[i + jpl * a_dim1];
               acum /= beta;

               a[k + jpl * a_dim1] -= u[k] * acum;
               for (int i = kp1; i < n; ++i) 
                  a[i + jpl * a_dim1] -= a[i + k * a_dim1] * acum;
            }

            // Transform J=D(phi)
            if (firstcb >= 0) 
            {
               for (m = firstcb; m < p; ++m)
               {
                  acum = u[k] * b[k + m * b_dim1];
                  for (int i = k; i < n; ++i) 
                     acum += a[i + k * a_dim1] * b[i + m * b_dim1];
                  acum /= beta;

                  b[k + m * b_dim1] -= u[k] * acum;
                  for (int i = k; i < n; ++i) 
                     b[i + m * b_dim1] -= a[i + k * a_dim1] * acum;
               }
            }
         }
      }

      if (isel >= 3 && l > 0)
      {
         bacsub(&r__[j * r_dim1]);
      }


   }
    


   if (isel < 3)
   {
   /*           COMPUTE THE FROBENIUS NORM OF THE RESIDUAL MATRIX: */
   
      *rnorm = 0.0;
      rn = 0;

      int nml = n-l;
      #pragma omp parallel for reduction(+: rn) private(d__1)  
      for (j = 0; j < s; ++j) 
      {
         d__1 = enorm(nml, &r__[l + j * r_dim1]);
         rn += d__1 * d__1;
      }

      *cur_chi2 = rn * chi2_factor / s;

      rn += kap[0] * kap[0];
      *rnorm = sqrt(rn);
   
   }
   else
   {
   /*           F2 IS NOW CONTAINED IN ROWS L+1 TO N AND COLUMNS L+S+1 TO */
   /*           L+P+S OF THE MATRIX A.  NOW SOLVE THE S (L X L) UPPER */
   /*           TRIANGULAR SYSTEMS TRI*BETA(J) = R1(J) FOR THE LINEAR */
   /*           PARAMETERS BETA.  BETA OVERWRITES R1. */
      jacb_row(s, kap, r__, 0, rnorm, fjrow);
      
   }

L99:
   if (isel < 0)
      iflag = isel;
    return iflag;
}








void VariableProjector::jacb_row(int s, double *kap, double* r__, int d_idx, double* res, double* derv)
{
   int m, k, j, ksub, b_dim1, r_dim1;
   double acum;

      /*           MAJOR PART OF KAUFMAN'S SIMPLIFICATION OCCURS HERE.  COMPUTE */
      /*           THE DERIVATIVE OF ETA WITH RESPECT TO THE NONLINEAR */
      /*           PARAMETERS */

      /*   T   D ETA        T    L          D PHI(J)    D PHI(L+1) */
      /*  Q * --------  =  Q * (SUM BETA(J) --------  + ----------)  =  F2*BETA */
      /*      D ALF(K)          J=1         D ALF(K)     D ALF(K) */

      /*           AND STORE THE RESULT IN COLUMNS L+S+1 TO L+NL+S.  THE */
      /*           FIRST L ROWS ARE OMITTED.  THIS IS -D(Q2)*Y.  THE RESIDUAL */
      /*           R2 = Q2*Y (IN COLUMNS L+1 TO L+S) IS COPIED TO COLUMN */
      /*           L+NL+S+1. */

   b_dim1 = ndim;
   r_dim1 = n;

   int lps = l+s;
   int nml = n-l;
   
   if (d_idx == 0)
   {
      *res = kap[0];

      for(j=0; j<nl; j++)
         derv[j] = kap[j+1];
      return;
   }

   d_idx--;
   
   int i = d_idx % nml + l;
   int isback = d_idx / nml; 
   int is = s - isback - 1;
   
   if (l != ncon) 
   {
      m = 0;
      for (k = 0; k < nl; ++k)
      {
         acum = (float)0.;
         for (j = ncon; j < l; ++j) 
         {
            if (inc[k + j * 12] != 0) 
            {
               acum += b[i + m * b_dim1] * r__[j + is * r_dim1];
               ++m;
            }
         }

         ksub = lps + k;
         
         if (inc[k + l * 12] != 0)
         {   
            acum += b[i + m * b_dim1];
            ++m;
         }

         derv[k] = -acum;

      }
   }
   *res = r__[i+is*r_dim1];
}




int VariableProjector::GetLinearParams(int s, float* y, int* irf_idx, double* alf, double* beta, double* chi2)
{
   int lnls1 = l + s + nl + 1;
   int nsls1 = n * s - l * (s - 1);

   double *r__  = a + l * n;

   this->y = y;
   this->irf_idx = irf_idx;
   this->thread = thread;

   if (irf_idx == NULL)
   {
      varproj(nsls1, nl, alf, wa1, wa2, 0);
      varproj(nsls1, nl, alf, wa1, wa2, 2);
   }
   else
   {
      varproj_local(nsls1, nl, alf, wa1, wa2, 0);
      varproj_local(nsls1, nl, alf, wa1, wa2, 2);
   }
   
   int nml = n-l;
   #pragma omp parallel for  
   for (int j = 0; j < s; ++j)
   { 
      chi2[j] = enorm(nml, &r__[l + j * n]); 
      chi2[j] *= chi2[j] * chi2_factor;
   }

   int ierr = 0;
   postpr(s, beta);


   return 0;
}


int VariableProjector::postpr(int s, double* u)
{
   int usave;
   int i,k,kback,kp1;
   double acum;

   double *r__  = a + l * n;

   int u_dim1, r_dim1, a_dim1, b_dim1;


   /* Parameter adjustments */
   u_dim1 = l;
   r_dim1 = n;
   a_dim1 = n;
   b_dim1 = ndim;

   if (l > 0) 
   {
      usave = 2;

      for (i = 0; i < l; ++i) 
      {
         b[i + usave * b_dim1] = u[i + u_dim1];
      }

      for (int is = 0; is < s; ++is) 
      {

         for (kback = 0; kback < l; ++kback) 
         {
            k = l - kback - 1;
            kp1 = k + 1;
            acum = (float)0.;

            for (i = k; i < n; ++i) 
            {
               acum += a[i + k * a_dim1] * r__[i + is * r_dim1];   
            }
            u[k + is * u_dim1] = r__[k + is * r_dim1];
            r__[k + is * r_dim1] = acum / a[k + k * a_dim1];
            acum = -acum / (a[k + usave * a_dim1] * a[k + k * a_dim1]);

            for (i = k; i < n; ++i) 
            {
               r__[i + is * r_dim1] -= a[i + k * a_dim1] * acum;
            }
         }
      }
   }

   return 0;


}


int VariableProjector::bacsub(double *x)
{
   int a_dim1;
   integer i, j, iback;
   double acum;

/*        BACKSOLVE THE N X N UPPER TRIANGULAR SYSTEM A*X = B. */
/*        THE SOLUTION X OVERWRITES THE RIGHT SIDE B. */

   a_dim1 = n;

   x[l-1] /= a[l-1 + (l-1) * a_dim1];
   if (l > 1) 
   {

      for (iback = 1; iback < l; ++iback) 
      {
      /*           I = N-1, N-2, ..., 2, 1 */
         i = l - iback - 1;
         acum = x[i];
         for (j = i; j < l; ++j) 
            acum -= a[i + j * a_dim1] * x[j];
         
         x[i] = acum / a[i + i * a_dim1];
      }
   }

   return 0;
}


int VariableProjector::GetFit(int s, float* y, int* irf_idx, double* alf, float* adjust, double* fit)
{
   int lnls1 = l + s + nl + 1;
   int nsls1 = n * s - l * (s - 1);

   this->y = y;
   
   double *r__  = a + l * n;

   double* lin_params = new double[ s * l ];

   if (irf_idx == NULL)
   {
      varproj(nsls1, nl, alf, wa1, wa2, 0);
      varproj(nsls1, nl, alf, wa1, wa2, 2);
   
      postpr(s, lin_params);

      model->ada(a, b, kap, alf, 0, 1, 0);

      int idx = 0;
      for(int k=0; k<s; k++)
      {
         for(int i=0; i<n; i++)
         {
            fit[idx] = adjust[i];
            for(int j=0; j<l; j++)
               fit[idx] += a[n*j+i] * lin_params[j+k*l];

            fit[idx++] += a[n*l+i];
         }
      }
   }
   else
   {

   }

   delete[] lin_params;

   return 0;

}

