function [methodinfo,structs,enuminfo,ThunkLibName]=FLIMGlobalAnalysisProto_PCWIN
%FLIMGLOBALANALYSISPROTO_PCWIN Create structures to define interfaces found in 'FLIMGlobalAnalysis'.

%This function was generated by loadlibrary.m parser version 1.1.6.35 on Mon Nov 28 18:05:19 2011
%perl options:'FLIMGlobalAnalysis.i -outfile=FLIMGlobalAnalysisProto_PCWIN.m'
ival={cell(1,0)}; % change 0 to the actual number of functions to preallocate the data.
structs=[];enuminfo=[];fcnNum=1;
fcns=struct('name',ival,'calltype',ival,'LHS',ival,'RHS',ival,'alias',ival);
ThunkLibName=[];
%  int FLIMGlobalGetUniqueID (); 
fcns.name{fcnNum}='FLIMGlobalGetUniqueID'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}=[];fcnNum=fcnNum+1;
%  void FLIMGlobalRelinquishID ( int id ); 
fcns.name{fcnNum}='FLIMGlobalRelinquishID'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}=[]; fcns.RHS{fcnNum}={'int32'};fcnNum=fcnNum+1;
%  int FLIMGlobalFit ( int c_idx , int n_group , int n_px , int n_regions [], int global_mode , int data_type , double data [], int mask [], int n_t , double t [], int n_irf , double t_irf [], double irf [], double pulse_pileup , int n_exp , int n_fix , double tau_min [], double tau_max [], int single_guess , double tau_guess [], int fit_beta , double fixed_beta [], int fit_t0 , double t0_guess , int fit_offset , double offset_guess , int fit_scatter , double scatter_guess , int fit_tvb , double tvb_guess , double tvb_profile [], int n_fret , int n_fret_fix , int inc_Rinf , double R_guess [], int pulsetrain_correction , double t_rep , int ref_reconvolution , double ref_lifetime_guess , int algorithm , double tau [], double I0 [], double beta [], double R [], double gamma [], double t0 [], double offset [], double scatter [], double tvb [], double ref_lifetime [], int calculate_errs , double tau_err [], double beta_err [], double R_err [], double offset_err [], double scatter_err [], double tvb_err [], double ref_lifetime_err [], double chi2 [], int ierr [], int n_thread , int run_async , int use_callback , int (* callback )()); 
fcns.name{fcnNum}='FLIMGlobalFit'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'int32', 'int32Ptr', 'int32', 'int32', 'doublePtr', 'int32Ptr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'double', 'int32', 'int32', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'doublePtr', 'int32', 'int32', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32Ptr', 'int32', 'int32', 'int32', 'FcnPtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalFitMemMap ( int c_idx , int n_group , int n_px , int n_regions [], int global_mode , int data_type , char * data_file , int mask [], int n_t , double t [], int n_irf , double t_irf [], double irf [], double pulse_pileup , int n_exp , int n_fix , double tau_min [], double tau_max [], int single_guess , double tau_guess [], int fit_beta , double fixed_beta [], int fit_t0 , double t0_guess , int fit_offset , double offset_guess , int fit_scatter , double scatter_guess , int fit_tvb , double tvb_guess , double tvb_profile [], int n_fret , int n_fret_fix , int inc_Rinf , double R_guess [], int pulsetrain_correction , double t_rep , int ref_reconvolution , double ref_lifetime_guess , int algorithm , double tau [], double I0 [], double beta [], double R [], double gamma [], double t0 [], double offset [], double scatter [], double tvb [], double ref_lifetime [], int calculate_errs , double tau_err [], double beta_err [], double R_err [], double offset_err [], double scatter_err [], double tvb_err [], double ref_lifetime_err [], double chi2 [], int ierr [], int n_thread , int runAsync , int use_callback , int (* callback )()); 
fcns.name{fcnNum}='FLIMGlobalFitMemMap'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'int32', 'int32Ptr', 'int32', 'int32', 'cstring', 'int32Ptr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'double', 'int32', 'int32', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'doublePtr', 'int32', 'int32', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32Ptr', 'int32', 'int32', 'int32', 'FcnPtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalPolarisationFitMemMap ( int c_idx , int n_group , int n_px , int n_regions [], int global_mode , int data_type , char * data_file , int mask [], int n_t , double t [], int n_irf , double t_irf [], double irf [], double pulse_pileup , int n_exp , int n_fix , double tau_min [], double tau_max [], int single_guess , double tau_guess [], int fit_beta , double fixed_beta [], int use_magic_decay , double magic_decay [], int n_theta , int n_theta_fix , int inc_rinf , double theta_guess [], int fit_t0 , double t0_guess , int fit_offset , double offset_guess , int fit_scatter , double scatter_guess , int fit_tvb , double tvb_guess , double tvb_profile [], int pulsetrain_correction , double t_rep , int ref_reconvolution , double ref_lifetime_guess , int algorithm , double tau [], double I0 [], double beta [], double theta [], double r [], double t0 [], double offset [], double scatter [], double tvb [], double ref_lifetime [], int calculate_errs , double tau_err [], double beta_err [], double theta_err [], double offset_err [], double scatter_err [], double tvb_err [], double ref_lifetime_err [], double chi2 [], int ierr [], int n_thread , int runAsync , int use_callback , int (* callback )()); 
fcns.name{fcnNum}='FLIMGlobalPolarisationFitMemMap'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'int32', 'int32Ptr', 'int32', 'int32', 'cstring', 'int32Ptr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'double', 'int32', 'int32', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'int32', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32Ptr', 'int32', 'int32', 'int32', 'FcnPtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalPolarisationFit ( int c_idx , int n_group , int n_px , int n_regions [], int global_mode , int data_type , double data [], int mask [], int n_t , double t [], int n_irf , double t_irf [], double irf [], double pulse_pileup , int n_exp , int n_fix , double tau_min [], double tau_max [], int single_guess , double tau_guess [], int fit_beta , double fixed_beta [], int use_magic_decay , double magic_decay [], int n_theta , int n_theta_fix , int inc_rinf , double theta_guess [], int fit_t0 , double t0_guess , int fit_offset , double offset_guess , int fit_scatter , double scatter_guess , int fit_tvb , double tvb_guess , double tvb_profile [], int pulsetrain_correction , double t_rep , int ref_reconvolution , double ref_lifetime_guess , int algorithm , double tau [], double I0 [], double beta [], double theta [], double r [], double t0 [], double offset [], double scatter [], double tvb [], double ref_lifetime [], int calculate_errs , double tau_err [], double beta_err [], double theta_err [], double offset_err [], double scatter_err [], double tvb_err [], double ref_lifetime_err [], double chi2 [], int ierr [], int n_thread , int runAsync , int use_callback , int (* callback )()); 
fcns.name{fcnNum}='FLIMGlobalPolarisationFit'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'int32', 'int32Ptr', 'int32', 'int32', 'doublePtr', 'int32Ptr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'double', 'int32', 'int32', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'int32', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32Ptr', 'int32', 'int32', 'int32', 'FcnPtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalGetChi2Map ( int c_idx , int data_type , double data [], int n_t , double t [], int n_irf , double t_irf [], double irf [], double pulse_pileup , int n_exp , int n_fix , double tau_min [], double tau_max [], double tau_guess [], int fit_beta , double fixed_beta [], int fit_t0 , double t0_guess , int fit_offset , double offset_guess , int fit_scatter , double scatter_guess , int fit_tvb , double tvb_guess , double tvb_profile [], int n_fret , int n_fret_fix , int inc_Rinf , double R_guess [], int pulsetrain_correction , double t_rep , int ref_reconvolution , double ref_lifetime_guess , int grid_size , double grid [], double tau [], double I0 [], double beta [], double R [], double gamma [], double t0 [], double offset [], double scatter [], double tvb [], double ref_lifetime [], double chi2 [], int runAsync , int use_callback , int (* callback )()); 
fcns.name{fcnNum}='FLIMGlobalGetChi2Map'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'doublePtr', 'int32', 'doublePtr', 'int32', 'doublePtr', 'doublePtr', 'double', 'int32', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'int32', 'double', 'doublePtr', 'int32', 'int32', 'int32', 'doublePtr', 'int32', 'double', 'int32', 'double', 'int32', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'doublePtr', 'int32', 'int32', 'FcnPtr'};fcnNum=fcnNum+1;
%  int FLIMGetFitStatus ( int c_idx , int * group , int * n_completed , int * iter , double * chi2 , double * progress ); 
fcns.name{fcnNum}='FLIMGetFitStatus'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32Ptr', 'int32Ptr', 'int32Ptr', 'doublePtr', 'doublePtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalTerminateFit ( int c_idx ); 
fcns.name{fcnNum}='FLIMGlobalTerminateFit'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32'};fcnNum=fcnNum+1;
%  int FLIMGlobalGetFit ( int c_idx , int ret_group_start , int n_ret_groups , int n_fit , int fit_mask [], int n_t , double t [], double fit []); 
fcns.name{fcnNum}='FLIMGlobalGetFit'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32', 'int32', 'int32', 'int32', 'int32Ptr', 'int32', 'doublePtr', 'doublePtr'};fcnNum=fcnNum+1;
%  int FLIMGlobalClearFit ( int c_idx ); 
fcns.name{fcnNum}='FLIMGlobalClearFit'; fcns.calltype{fcnNum}='cdecl'; fcns.LHS{fcnNum}='int32'; fcns.RHS{fcnNum}={'int32'};fcnNum=fcnNum+1;
methodinfo=fcns;