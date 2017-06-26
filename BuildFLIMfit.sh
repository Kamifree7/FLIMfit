#!/bin/bash

if [ -z ${OME+x} ]; then export OME=5.2; echo "Setting OME=5.2"; fi
if [ -z ${BIO+x} ]; then export BIO=5.2; echo "Setting BIO=5.2"; fi

if [ -z ${MATLAB_VER+x} ]; then export MATLAB_VER=R2016b; echo "Setting MATLAB_VER=R2016b"; fi

export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
export LDFLAGS="-L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib"
export MACOSX_DEPLOYMENT_TARGET=10.10

conan install . -s compiler=clang -s compiler.version=4.0 --build missing

cmake -G"Unix Makefiles" -H. -BGeneratedProjects/Unix

if ! cmake --build "GeneratedProjects/Unix" --config Debug; then
    echo 'Error building project'
    exit 1
fi

# we don't want to compile the front end yet
exit 0;

export PATH=/Applications/MATLAB_${MATLAB_VER}.app/bin:$PATH
# compile the Matlab code to generate the FLIMfit_MACI64.app
cd FLIMfitFrontEnd

if [ -z ${VERSION+x} ]; then export VERSION=$(git describe); fi
echo "VERSION = $VERSION"

cur_dir=$(grealpath .)
if ! matlab -nodisplay -nosplash -r "cd('${cur_dir}'); compile(true); exit"; then
    echo 'Error building frontend'
    exit 1
fi

cd ../FLIMfitStandalone/BuiltApps
zip -r FLIMfit_${VERSION}_MACI64.zip *.app/
cd ../..

echo "Build complete"