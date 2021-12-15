Dockerized ICU4C Demos
###

- Build with `sh build.sh`

- Run with `docker run --rm -p 18080:8080 unicode/icu4c-demos:latest` - will listen on port 18080

## Building with a special ICU version

Use an ICU source tarball such as those in [ICU
releases](https://github.com/unicode-org/icu/tags):

Run this from the icu-demos directory:
```
export RELEASE_PATH=https://github.com/unicode-org/icu/releases/download/release-70-1
export LIB_TGZ=icu4c-70_1-src.tgz
export RELEASE_LIB=$RELEASE_PATH/$LIB_TGZ

echo $RELEASE_LIB

docker build --build-arg ICU_PATH=$RELEASE_LIB -t icu4c-demos:my-demos  . -f icu-kube/docker.d/icu4c-demos/Dockerfile
```

- If all goes well, you can now run

```sh
docker run --rm -p 8888:8080 icu4c-demos:my-demos
```

… That will serve up the demos at http://localhost:8888/icu-bin/icudemos
