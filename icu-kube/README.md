# Dockerized ICU4C Demos

## Building with the system ICU

WARNING: It will build the demos with whatever ICU version it finds on the system.
You should not deploy demos with images built this way!

- Build with `sh build.sh`

- Run with `docker run --rm -p 18080:8080 unicode/icu4c-demos:latest` - will listen on port 18080

## Building with a special ICU version

Use an ICU source tarball such as those in
[ICU releases](https://github.com/unicode-org/icu/tags):

Run this from the icu-demos directory:

```sh
export ICU_VER=78.3
export RELEASE_PATH=https://github.com/unicode-org/icu/releases/download/release-${ICU_VER}
export LIB_TGZ=icu4c-${ICU_VER}-sources.tgz
export RELEASE_LIB=$RELEASE_PATH/$LIB_TGZ

echo $RELEASE_LIB

docker build --build-arg ICU_PATH=$RELEASE_LIB -t icu4c-demos:my-demos  . -f icu-kube/docker.d/icu4c-demos/Dockerfile
```

- If all goes well, you can now run

```sh
docker run --name icu4c-demos --rm -p 8888:8080 icu4c-demos:my-demos
```

… That will serve up the demos at http://localhost:8888/icu-bin/icudemos

## Building a release for deployment and deploying it

Run this from the icu-demos directory:

```sh
build-deploy.sh
```

If everything looks good you can deploy it by running

```sh
push-deploy.sh
```

## Publishing the GCloud images to public demos

See the `README.md` in the `../icu4jweb` folder ([here](../icu4jweb/README.md)).
