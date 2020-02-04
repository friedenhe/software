#!/bin/bash

set -u
set -e

DATE_TAG=$(date +%Y-%m)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
OUTPUT=`mktemp -d -t singularity.XXXXXXX`
umask 022

python make_dockerfiles.py

cp $DIR/test/*.py $DIR/docker/nompi
cp $DIR/*.tar.gz $DIR/docker/nompi
docker build $DIR/docker/nompi \
             -t glotzerlab-private/software:nompi

cp $DIR/test/*.py $DIR/docker/comet
cp $DIR/*.tar.gz $DIR/docker/comet
docker build $DIR/docker/comet \
             -t glotzerlab-private/software:comet

cp $DIR/test/*.py $DIR/docker/greatlakes
cp $DIR/*.tar.gz $DIR/docker/greatlakes
docker build $DIR/docker/greatlakes \
             -t glotzerlab-private/software:greatlakes

cp $DIR/test/*.py $DIR/docker/bridges
cp $DIR/*.tar.gz $DIR/docker/bridges
docker build $DIR/docker/bridges \
             -t glotzerlab-private/software:bridges

cp $DIR/test/*.py $DIR/docker/stampede2
cp $DIR/*.tar.gz $DIR/docker/stampede2
docker build $DIR/docker/stampede2 \
             -t glotzerlab-private/software:stampede2

for label in nompi greatlakes comet bridges stampede2
do
    docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${OUTPUT}:/output singularityware/docker2singularity:v2.6 --name software-${label} glotzerlab-private/software:${label}
    mv ${OUTPUT}/*.simg /nfs/turbo/glotzer/containers/glotzerlab-private
done
