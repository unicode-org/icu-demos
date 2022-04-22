#!/bin/sh
set -x
# the project id "icu4c-demos" cannot be changed.
# Update the final part of the tag for each new release.
docker build -t gcr.io/icu4c-demos/icu4jweb:71.1 .
