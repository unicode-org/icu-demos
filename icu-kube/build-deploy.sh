#!/bin/sh
set -x
cd ..

# Update ICU_VER for each new release.
# It should match the version part of the release number in GitHub (without the `release-` part)
# https://github.com/unicode-org/icu/releases

export ICU_VER=78.3

export RELEASE_PATH=https://github.com/unicode-org/icu/releases/download/release-${ICU_VER}
export LIB_TGZ=icu4c-${ICU_VER}-sources.tgz
export RELEASE_LIB=$RELEASE_PATH/$LIB_TGZ
echo   $RELEASE_LIB

docker build -t us-central1-docker.pkg.dev/goog-unicode-dev/unicode-jsps/icu4c-demos:${ICU_VER} --build-arg ICU_PATH=$RELEASE_LIB . -f icu-kube/docker.d/icu4c-demos/Dockerfile
