//=========================================================================
//
// Copyright (C) 2013 Imperial College London.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// This software tool was developed with support from the UK 
// Engineering and Physical Sciences Council 
// through  a studentship from the Institute of Chemical Biology 
// and The Wellcome Trust through a grant entitled 
// "The Open Microscopy Environment: Image Informatics for Biological Sciences" (Ref: 095931).
//
// Author : Sean Warren
//
//=========================================================================

#pragma once

#include "DecayModel.h"
#include "ExponentialPrecomputationBuffer.h"
#include "FitResults.h"
#include "RegionData.h"
#include "MeanLifetimeEstimator.h"
#include "ProgressReporter.h"

#include "omp_stub.h"
#include "levmar.h"

#include <cstdio>
#include <iostream>

#include <memory>

class FittingError : public std::runtime_error
{
public: 
   FittingError(const std::string& description, int code) :
      std::runtime_error(description), 
      code_(code)
   {
   }
   
   int code() { return code_; }

protected:
   int code_;
};

class AbstractFitter
{
public:

   AbstractFitter(std::shared_ptr<DecayModel> model, int n_param_extra, int max_region_size, int global_algorithm, int n_thread, std::shared_ptr<ProgressReporter> reporter);

   virtual ~AbstractFitter() {};

   virtual void FitFcn(int nl, std::vector<double>& alf, int itmax, int* niter, int* ierr) = 0;
   virtual void GetLinearParams() = 0;
   
   int Fit(RegionData& region_data, FitResultsRegion& results, int itmax, int& niter, int &ierr, double& c2);
   int GetFit(int irf_idx, const std::vector<double>& alf, float* lin_params, double* fit);
   double ErrMinFcn(double x);
   int CalculateErrors(double conf_limit);

   void GetParams(int nl, const std::vector<double>& alf);
   
   void SetAlf(const double* alf_);
   double* GetModel(const std::vector<double>& alf, int irf_idx, int isel, int thread);


   double* GetModel(const double* alf, int irf_idx, int isel, int thread);
   void ReleaseResidualMemory();

protected:

   int Init();

   std::shared_ptr<DecayModel> model; // reference
   std::vector<std::shared_ptr<DecayModel>> models; // for each thread
   std::shared_ptr<ProgressReporter> reporter;
   
   std::vector<double> alf;
   std::vector<double> err_lower;
   std::vector<double> err_upper;

   // Used by variable projection
   std::vector<int> inc;
   std::vector<int> inc_full;
   //int     ncon;
   //int     nconp1;
   int     philp1;

   std::vector<std::vector<double>> a_;
   std::vector<std::vector<double>> b_;
   std::vector<double> r;
   std::vector<double> kap;
   std::vector<double> params;
   std::vector<double> alf_err;
   std::vector<double> alf_buf;

   int     n;
   int     nl;
   int     ndim;
   int     nmax;
   int     s;
   int     l;
   int     lmax;
   int     n_param;
   int     p;
   int     pmax;

   int     max_region_size;

   float*  y;
   std::vector<float> w;
   std::vector<float> avg_y;
   float *lin_params;
   float *chi2;
   int    *irf_idx;

   float chi2_norm;
   double* cur_chi2;

   int n_thread;
   int variable_phi;

   int    fixed_param;
   double fixed_value_initial;
   double fixed_value_cur;
   double chi2_final;

   bool getting_errs;

   double counts_per_photon;

private:

   MeanLifetimeEstimator lifetime_estimator;

   int global_algorithm;
   double conf_limit;

   int search_dir;

   FILE* f_debug;

   int a_size;
   int b_size;

   int irf_idx_0;
};