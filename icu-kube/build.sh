#!/bin/sh
set -x
cd ..
exec docker build -t unicode/icu4c-demos:latest --build-arg ICU_PATH=system . -f icu-kube/docker.d/icu4c-demos/Dockerfile
