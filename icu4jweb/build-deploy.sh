#!/bin/sh
set -x

# Update ICU_VER for each new release.
# It should match the version part of the release number in GitHub (without the `release-` part)
# https://github.com/unicode-org/icu/releases

export ICU_VER=79.1

docker build -t us-central1-docker.pkg.dev/goog-unicode-dev/unicode-jsps/icu4j-demos:${ICU_VER} .
