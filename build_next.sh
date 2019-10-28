#!/bin/bash

set -u
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
OUTPUT=`mktemp -d -t singularity.XXXXXXX`
umask 022

cp $DIR/test/*.py $DIR/docker/nompi
cp $DIR/*.tar.gz $DIR/docker/nompi
docker build $DIR/docker/nompi \
             -t glotzerlab-next/software:nompi

cp $DIR/test/*.py $DIR/docker/greatlakes
cp $DIR/*.tar.gz $DIR/docker/greatlakes
docker build $DIR/docker/greatlakes \
             -t glotzerlab-next/software:greatlakes

cp $DIR/test/*.py $DIR/docker/comet
cp $DIR/*.tar.gz $DIR/docker/comet
docker build $DIR/docker/comet \
             -t glotzerlab-next/software:comet

cp $DIR/test/*.py $DIR/docker/flux
cp $DIR/*.tar.gz $DIR/docker/flux
docker build $DIR/docker/flux \
             -t glotzerlab-next/software:flux

cp $DIR/test/*.py $DIR/docker/bridges
cp $DIR/*.tar.gz $DIR/docker/bridges
docker build $DIR/docker/bridges \
             -t glotzerlab-next/software:bridges

cp $DIR/test/*.py $DIR/docker/stampede2
cp $DIR/*.tar.gz $DIR/docker/stampede2
docker build $DIR/docker/stampede2 \
             -t glotzerlab-next/software:stampede2

for label in nompi greatlakes flux comet bridges stampede2
do
    docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${OUTPUT}:/output singularityware/docker2singularity:v2.6 --name software-${label} glotzerlab-next/software:${label}
    mv ${OUTPUT}/*.simg /nfs/turbo/glotzer/containers/glotzerlab-next
done