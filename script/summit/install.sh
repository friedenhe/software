#!/usr/bin/bash

set -u
set -e

usage="$(basename "$0") root -- Build software and install in root."

if [[ $# -lt 1 || $# -gt 1  || $1 == "-h" ]]
then
    echo "$usage"
    exit 0
fi

ROOT=$1
module reset
module load gcc/7.4.0
module load python/3.7.0
python3 -m venv $ROOT --system-site-packages

cat >$ROOT/environment.sh << EOL
module reset
module load gcc/7.4.0
module load cuda
module load cmake
module load git
module load netlib-lapack/3.8.0
module load hdf5/1.10.3

export LD_LIBRARY_PATH=$ROOT/lib:\$LD_LIBRARY_PATH
export PATH=$ROOT/bin:\$PATH
export CPATH=$ROOT/include:\$CPATH
export LIBRARY_PATH=$ROOT/lib:\$LIBRARY_PATH
export CC=\${OLCF_GCC_ROOT}/bin/gcc
export CXX=\${OLCF_GCC_ROOT}/bin/g++
export LDSHARED="\${OLCF_GCC_ROOT}/bin/gcc -shared"
export VIRTUAL_ENV=$ROOT
EOL

source $ROOT/environment.sh

mkdir -p /tmp/$USER-glotzerlab-software
cd /tmp/$USER-glotzerlab-software

python3 -m pip install --progress-bar off --no-deps --no-binary :all: cython mpi4py six numpy tables numexpr deprecation

# TBB
curl -sSLO https://github.com/01org/tbb/archive/2019_U8.tar.gz \
    && echo "7b1fd8caea14be72ae4175896510bf99c809cd7031306a1917565e6de7382fba  2019_U8.tar.gz" | sha256sum -c - \
    && tar -xzf 2019_U8.tar.gz -C . \
    && cd tbb-2019_U8 \
    && make \
    && install -d $ROOT/lib \
    && install -m755 build/linux_*release/*.so* ${ROOT}/lib \
    && install -d $ROOT/include \
    && cp -a include/tbb $ROOT/include \
    && cd .. \
    && rm -rf tbb-2019_U8 \
    && rm 2019_U8.tar.gz \
    || exit 1

# embree is not available for power9

# scipy
curl -sSLO https://github.com/scipy/scipy/releases/download/v1.3.1/scipy-1.3.1.tar.gz \
    && echo "2643cfb46d97b7797d1dbdb6f3c23fe3402904e3c90e6facfe6a9b98d808c1b5  scipy-1.3.1.tar.gz" | sha256sum -c - \
    && tar -xzf scipy-1.3.1.tar.gz -C . \
    && cd scipy-1.3.1 \
    && LAPACK=${OLCF_NETLIB_LAPACK_ROOT}/lib64/liblapack.so BLAS=${OLCF_NETLIB_LAPACK_ROOT}/lib64/libblas.so python3 setup.py install \
    && rm -rf scipy-1.3.1 \
    || exit 1



 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/freud/freud-v2.1.0.tar.gz \
    && echo "f75174e3885b9a00b44002b7b90b1a1d2b2b918cb69040f59aef8afea448075e  freud-v2.1.0.tar.gz" | sha256sum -c - \
    && tar -xzf freud-v2.1.0.tar.gz -C . \
    && rm -f freud-v2.1.0/*.toml \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./freud-v2.1.0 \
    && rm -rf freud-v2.1.0 \
    && rm freud-v2.1.0.tar.gz \
    || exit 1



 curl -sSLO https://glotzerlab.engin.umich.edu/downloads/garnett/garnett-v0.6.1.tar.gz \
    && echo "47d4c59d2042bdb8ded415e8a5782d50777c112196070401d744df494ac3e134  garnett-v0.6.1.tar.gz" | sha256sum -c - \
    && tar -xzf garnett-v0.6.1.tar.gz -C . \
    && rm -f garnett-v0.6.1/*.toml \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./garnett-v0.6.1 \
    && rm -rf garnett-v0.6.1 \
    && rm garnett-v0.6.1.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/gsd/gsd-v2.0.0.tar.gz \
    && echo "659221326bf09548f0c8c0e4f9a781585891da41c70a8a62f186895739957eef  gsd-v2.0.0.tar.gz" | sha256sum -c - \
    && tar -xzf gsd-v2.0.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./gsd-v2.0.0 \
    && rm -rf gsd-v2.0.0 \
    && rm gsd-v2.0.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/libgetar/libgetar-v1.0.1.tar.gz \
    && echo "0a438dc8336103158fc4dbb7ebcbc011279d7a8ae134824dda5946e6b9042039  libgetar-v1.0.1.tar.gz" | sha256sum -c - \
    && tar -xzf libgetar-v1.0.1.tar.gz -C . \
    && rm -f libgetar-v1.0.1/*.toml \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./libgetar-v1.0.1 \
    && rm -rf libgetar-v1.0.1 \
    && rm libgetar-v1.0.1.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/rowan/rowan-v1.2.2.tar.gz \
    && echo "88ed2c0003543d3154374f741a04fcb21cce32180e7b29152dc93c4c887e0c51  rowan-v1.2.2.tar.gz" | sha256sum -c - \
    && tar -xzf rowan-v1.2.2.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./rowan-v1.2.2 \
    && rm -rf rowan-v1.2.2 \
    && rm rowan-v1.2.2.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/plato/plato-v1.7.0.tar.gz \
    && echo "8268ab6982f9135d1f7788309804eb15a1e98ef1d2f6d959733a3e23a1595def  plato-v1.7.0.tar.gz" | sha256sum -c - \
    && tar -xzf plato-v1.7.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./plato-v1.7.0 \
    && rm -rf plato-v1.7.0 \
    && rm plato-v1.7.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/pythia/pythia-v0.2.5.tar.gz \
    && echo "5dfe1efeb7343fbfc7260dc6f05a8ee17908add288c67094d3740c7056627140  pythia-v0.2.5.tar.gz" | sha256sum -c - \
    && tar -xzf pythia-v0.2.5.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./pythia-v0.2.5 \
    && rm -rf pythia-v0.2.5 \
    && rm pythia-v0.2.5.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/signac/signac-v1.3.0.tar.gz \
    && echo "cc9b1af90ee98746ead05e5d84adb662b60aa861953d71294bac9eb64175b61e  signac-v1.3.0.tar.gz" | sha256sum -c - \
    && tar -xzf signac-v1.3.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./signac-v1.3.0 \
    && rm -rf signac-v1.3.0 \
    && rm signac-v1.3.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/signac-flow/signac-flow-v0.9.0.tar.gz \
    && echo "59633ae44c37d77acde70af9a2be483a84ecb08eb742bdfc2f682b94466758b9  signac-flow-v0.9.0.tar.gz" | sha256sum -c - \
    && tar -xzf signac-flow-v0.9.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./signac-flow-v0.9.0 \
    && rm -rf signac-flow-v0.9.0 \
    && rm signac-flow-v0.9.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/hoomd/hoomd-v2.9.0.tar.gz \
    && echo "e004da89657894e51c91359d85156debad1725291b69278c4eeb89401b0933da  hoomd-v2.9.0.tar.gz" | sha256sum -c - \
    && tar -xzf hoomd-v2.9.0.tar.gz -C . \
    && cd hoomd-v2.9.0 \
    && mkdir -p build \
    && cd build \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && cmake ../ -DPYTHON_EXECUTABLE="`which python3`" -DENABLE_CUDA=on -DENABLE_MPI=on -DENABLE_TBB=off -DBUILD_JIT=off -DBUILD_TESTING=off -DENABLE_MPI_CUDA=on -DCMAKE_INSTALL_PREFIX=`python3 -c "import site; print(site.getsitepackages()[0])"` \
    && make install -j20 \
    && cd ../../ \
    && rm -rf /root/hoomd-v2.9.0 \
    && rm hoomd-v2.9.0.tar.gz \
    || exit 1

