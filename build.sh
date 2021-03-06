#!/bin/bash

set -u
set -e

python make_dockerfiles.py

DATE_TAG=$(date +%Y.%m)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
OUTPUT=`mktemp -d -t singularity.XXXXXXX`

umask 022

cp $DIR/test/*.py $DIR/docker/nompi
docker build $DIR/docker/nompi \
             -t glotzerlab/software:latest \
             -t glotzerlab/software:nompi \
             -t glotzerlab/software:${DATE_TAG}-cuda10

cp $DIR/test/*.py $DIR/docker/greatlakes
docker build $DIR/docker/greatlakes \
             -t glotzerlab/software:greatlakes \
             -t glotzerlab/software:${DATE_TAG}-skylakex-cuda10-mlx-openmpi4.0.1

cp $DIR/test/*.py $DIR/docker/comet
docker build $DIR/docker/comet \
             -t glotzerlab/software:comet \
             -t glotzerlab/software:${DATE_TAG}-haswell-cuda9-mlx-openmpi1.8.4

cp $DIR/test/*.py $DIR/docker/bridges
docker build $DIR/docker/bridges \
             -t glotzerlab/software:bridges \
             -t glotzerlab/software:${DATE_TAG}-haswell-cuda10-hfi1-openmpi2.1.2

cp $DIR/test/*.py $DIR/docker/stampede2
docker build $DIR/docker/stampede2 \
             -t glotzerlab/software:stampede2 \
             -t glotzerlab/software:${DATE_TAG}-skylakex-cuda10-hfi1-mvapich2.3

for label in nompi greatlakes comet bridges stampede2
do
    docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${OUTPUT}:/output singularityware/docker2singularity:v2.6 --name software-${label} glotzerlab/software:${label}
    mv ${OUTPUT}/*.simg /nfs/turbo/glotzer/containers/glotzerlab
done
