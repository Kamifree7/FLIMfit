#!/bin/sh
# script for execution of deployed FLIMfit
#
# Sets up the  Library path for the dynamic libraries then executes 
# the script generated by the Matlab Compiler
#
exe_name=$0
echo "Executing FlIMfit from "
exe_dir=`dirname "$0"`
echo $exe_dir
  
echo Setting up environment variables
MCRROOT="/Applications/MATLAB/MATLAB_Compiler_Runtime/v81"
echo ---
DYLD_LIBRARY_PATH=.:${MCRROOT}/runtime/maci64 ;
DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${MCRROOT}/bin/maci64 ;
DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${MCRROOT}/sys/os/maci64;

DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:"$exe_dir";

XAPPLRESDIR=${MCRROOT}/X11/app-defaults ;

export DYLD_LIBRARY_PATH;
export XAPPLRESDIR;

echo DYLD_LIBRARY_PATH is ${DYLD_LIBRARY_PATH};
shift 1
  args=
  while [ $# -gt 0 ]; do
      token=$1
      args="${args} \"${token}\"" 
      shift
  done
  eval "\"${exe_dir}/FLIMfit_MACI64.app/Contents/MacOS/FLIMfit_MACI64\"" $args

exit



  

