#!/bin/sh
set -x
# the project id "icu4c-demos" cannot be changed.
# Update the final part of the tag for each new release.
docker build -t us-central1-docker.pkg.dev/goog-unicode-dev/unicode-jsps/icu4j-demos:74.1 .
