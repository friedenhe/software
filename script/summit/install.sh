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



 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/freud/freud-v1.2.2.tar.gz \
    && echo "53e58f157be07e6fcf90431c19188874c5cba9ac48bffc19f631930d176b6c9d  freud-v1.2.2.tar.gz" | sha256sum -c - \
    && tar -xzf freud-v1.2.2.tar.gz -C . \
    && rm -f freud-v1.2.2/*.toml \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./freud-v1.2.2 \
    && rm -rf freud-v1.2.2 \
    && rm freud-v1.2.2.tar.gz \
    || exit 1



 curl -sSLO https://glotzerlab.engin.umich.edu/downloads/garnett/garnett-v0.5.0.tar.gz \
    && echo "9be24f8332757c00945849784316b06a2db58f06569a988bfb88c0af6455df1f  garnett-v0.5.0.tar.gz" | sha256sum -c - \
    && tar -xzf garnett-v0.5.0.tar.gz -C . \
    && rm -f garnett-v0.5.0/*.toml \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./garnett-v0.5.0 \
    && rm -rf garnett-v0.5.0 \
    && rm garnett-v0.5.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/gsd/gsd-v1.8.1.tar.gz \
    && echo "bb38d00b83b982904aade2013d0072f66c84faa52cec01df1b23cbbf62707b68  gsd-v1.8.1.tar.gz" | sha256sum -c - \
    && tar -xzf gsd-v1.8.1.tar.gz -C . \
    && cd gsd-v1.8.1 \
    && mkdir build \
    && cd build \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && cmake ../ -DPYTHON_EXECUTABLE="`which python3`" -DCMAKE_INSTALL_PREFIX=`python3 -c "import site; print(site.getsitepackages()[0])"` \
    && make install -j20 \
    && cd ../../ \
    && rm -rf gsd-v1.8.1 \
    && rm gsd-v1.8.1.tar.gz \
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

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/rowan/rowan-v1.2.1.tar.gz \
    && echo "c59ad2dded288e5626db19869ed2915852734520347f7e0afd9dd08171d4541b  rowan-v1.2.1.tar.gz" | sha256sum -c - \
    && tar -xzf rowan-v1.2.1.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./rowan-v1.2.1 \
    && rm -rf rowan-v1.2.1 \
    && rm rowan-v1.2.1.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/plato/plato-v1.6.0.tar.gz \
    && echo "d7282e2b7a8cd641e41a845f1a7075031b821440ab1ea9ad11a0dc991b86f91d  plato-v1.6.0.tar.gz" | sha256sum -c - \
    && tar -xzf plato-v1.6.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./plato-v1.6.0 \
    && rm -rf plato-v1.6.0 \
    && rm plato-v1.6.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/pythia/pythia-v0.2.5.tar.gz \
    && echo "5dfe1efeb7343fbfc7260dc6f05a8ee17908add288c67094d3740c7056627140  pythia-v0.2.5.tar.gz" | sha256sum -c - \
    && tar -xzf pythia-v0.2.5.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./pythia-v0.2.5 \
    && rm -rf pythia-v0.2.5 \
    && rm pythia-v0.2.5.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/signac/signac-v1.2.0.tar.gz \
    && echo "38591f2c0dcb77d28dfeb7906b2c25bfc3df39d67486ff819872ba69be4fa187  signac-v1.2.0.tar.gz" | sha256sum -c - \
    && tar -xzf signac-v1.2.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./signac-v1.2.0 \
    && rm -rf signac-v1.2.0 \
    && rm signac-v1.2.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/signac-flow/signac-flow-v0.8.0.tar.gz \
    && echo "ae933964e2786f22963f70ff147a2cbd152e7e32c903ba550154b91db890451b  signac-flow-v0.8.0.tar.gz" | sha256sum -c - \
    && tar -xzf signac-flow-v0.8.0.tar.gz -C . \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && python3 -m pip install --no-deps --ignore-installed ./signac-flow-v0.8.0 \
    && rm -rf signac-flow-v0.8.0 \
    && rm signac-flow-v0.8.0.tar.gz \
    || exit 1

 curl -sSLO https://glotzerlab.engin.umich.edu/Downloads/hoomd/hoomd-v2.6.0.tar.gz \
    && echo "f926b2d39cf58916794edbf4c619d0542a9b1e66b6c6a1b6a01b70f5617ec728  hoomd-v2.6.0.tar.gz" | sha256sum -c - \
    && tar -xzf hoomd-v2.6.0.tar.gz -C . \
    && cd hoomd-v2.6.0 \
    && mkdir build \
    && cd build \
    && export CFLAGS="-mcpu=power9 -mtune=power9" CXXFLAGS="-mcpu=power9 -mtune=power9" \
    && cmake ../ -DPYTHON_EXECUTABLE="`which python3`" -DENABLE_CUDA=on -DENABLE_MPI=on -DENABLE_TBB=off -DBUILD_JIT=off -DBUILD_TESTING=off -DENABLE_MPI_CUDA=on -DCMAKE_INSTALL_PREFIX=`python3 -c "import site; print(site.getsitepackages()[0])"` \
    && make install -j20 \
    && cd ../../ \
    && rm -rf /root/hoomd-v2.6.0 \
    && rm hoomd-v2.6.0.tar.gz \
    || exit 1

