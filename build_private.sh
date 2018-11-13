#!/bin/bash

set -u
set -e

DATE_TAG=$(date +%Y.%m)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cp $DIR/test/*.py $DIR/cuda8/comet
cp $DIR/*.tar.gz $DIR/cuda8/comet
docker build $DIR/cuda8/comet \
             -t software/comet
docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${DIR}:/output singularityware/docker2singularity software/comet

cp $DIR/test/*.py $DIR/cuda8/flux
cp $DIR/*.tar.gz $DIR/cuda8/flux
docker build $DIR/cuda8/flux \
             -t software/flux
docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${DIR}:/output singularityware/docker2singularity software/flux

cp $DIR/test/*.py $DIR/cuda8/bridges
cp $DIR/*.tar.gz $DIR/cuda8/bridges
docker build $DIR/cuda8/bridges \
             -t software/bridges
docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${DIR}:/output singularityware/docker2singularity software/bridges

cp $DIR/test/*.py $DIR/cuda8/stampede2
cp $DIR/*.tar.gz $DIR/cuda8/stampede2
docker build $DIR/cuda8/stampede2 \
             -t software/stampede2
docker run -t --rm --privileged -v /var/run/docker.sock:/var/run/docker.sock -v ${DIR}:/output singularityware/docker2singularity software/stampede2
