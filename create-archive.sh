#!/bin/bash

set -u
set -e

dir=$(pwd)
usage="$(basename "$0") repo branch [ver] -- Creates archive for Glotzer package downloads.

where:
    repo: name of repository
    ver:  package version tag to use, automatically detects newest tag if not provided"

if [[ $# -lt 1 || $# -gt 3  || $1 == "-h" ]]
then
    echo "$usage"
    exit 0
fi

repo=$1
branch=$2
ver=${3:-"auto"}
name=$repo

if [[ $ver = "auto" ]]
then
    ver=$(git ls-remote --tags git+ssh://git@github.com/glotzerlab/$repo | sort -t '/' -k 3 -k 4 -V | grep -v { | awk -F/ '{ print $3 }' | tail -n1)
fi

_root="$dir"
_targetpath="${_root}/$name-$ver.tar.gz"
if [[ ! -f ${_targetpath} ]]
then
    echo -e "[$name $ver]\tCreating archive $name/$name-$ver.tar.gz"
    _tmpdir=$(mktemp -d)
    cd ${_tmpdir}
    git clone --recursive --branch $branch ssh://bitbucket.org/glotzer/$repo $name-$ver
    rm -rf $name-$ver/.git

    if [[ $name = "signac-dashboard" ]]; then
        # The docs for the bulma submodule are >60 MB and should be removed from the tarball.
        rm -rf $name-$ver/signac_dashboard/static/scss/bulma/docs
    fi

    tar -czf ${_targetpath} $name-$ver/
    cd ${_root}
    rm -rf ${_tmpdir}
else
    echo -e "[$name $ver]\tArchive already exists in ${_targetpath}. Remove to recreate the archive."
fi
