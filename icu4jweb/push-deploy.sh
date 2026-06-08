#!/bin/sh
set -x

# Update ICU_VER for each new release.
# It should match the one in build-deploy.sh
export ICU_VER=78.3

docker push us-central1-docker.pkg.dev/goog-unicode-dev/unicode-jsps/icu4j-demos:${ICU_VER}
