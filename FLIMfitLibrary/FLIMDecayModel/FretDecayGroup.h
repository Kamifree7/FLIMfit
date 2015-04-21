#pragma once
#include "MultiExponentialDecayGroup.h"

class FretDecayGroup : public MultiExponentialDecayGroup
{
public:

   FretDecayGroup(int n_donor_exponential_ = 1, int n_fret_populations_ = 1, bool include_donor_only = true);
   void SetNumFretPopulations(int n_fret_populations_);
   void SetIncludeDonorOnly(bool include_donor_only_);
   void SetIncludeAcceptor(bool include_acceptor_);

   int SetVariables(const double* variables);
   int CalculateModel(double* a, int adim, vector<double>& kap);
   int CalculateDerivatives(double* b, int bdim, vector<double>& kap);

   int GetNonlinearOutputs(float* nonlin_variables, float* output, int& nonlin_idx);
   int GetLinearOutputs(float* lin_variables, float* output, int& lin_idx);

   int SetupIncMatrix(int* inc, int& row, int& col);

   void GetLinearOutputParamNames(vector<string>& names);

protected:

   void Validate();
   void Init();

   void AddAcceptorContribution(int i, double factor, double* a, int adim, vector<double>& kap);
   int AddLifetimeDerivativesForFret(int idx, double* b, int bdim, vector<double>& kap);
   int AddFretEfficiencyDerivatives(double* b, int bdim, vector<double>& kap);
   int AddAcceptorIntensityDerivatives(double* b, int bdim, vector<double>& kap);
   int AddAcceptorLifetimeDerivatives(double* b, int bdim, vector<double>& kap);

   vector<shared_ptr<FittingParameter>> E_parameters;
   shared_ptr<FittingParameter> A0_parameter;
   shared_ptr<FittingParameter> tauA_parameter;

   int n_fret_populations;
   bool include_donor_only;
   bool include_acceptor;
   int n_multiexp_parameters;
   
   vector<double> E;
   vector<vector<double>> tau_fret;
   double A0;
   double tauA;

   vector<vector<ExponentialPrecomputationBuffer>> fret_buffer;
   vector<vector<ExponentialPrecomputationBuffer>> acceptor_buffer_fret;
   unique_ptr<ExponentialPrecomputationBuffer> acceptor_buffer;
   vector<double> donor_channel_factors;
   vector<double> acceptor_channel_factors;
   vector<double> direct_acceptor_channel_factors;
};


class QFretDecayGroupSpec : public QAbstractDecayGroupSpec, virtual public FretDecayGroup
{
   Q_OBJECT

public:

   Q_PROPERTY(int n_exponential MEMBER n_exponential WRITE SetNumExponential USER true);
   Q_PROPERTY(int n_fret_populations MEMBER n_fret_populations WRITE SetNumFretPopulations USER true);
   Q_PROPERTY(bool include_donor_only MEMBER include_donor_only WRITE SetIncludeDonorOnly USER true);
   Q_PROPERTY(bool include_acceptor MEMBER n_fret_populations WRITE SetIncludeAcceptor USER true);
};