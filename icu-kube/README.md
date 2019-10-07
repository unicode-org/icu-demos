Dockerized ICU4C Demos
###

- Build with `sh build.sh`

- Run with `docker run --rm -p 18080:8080 unicode/icu4c-demos:latest` - will listen on port 18080

## Building with a special ICU version

Use an ICU source tarball:

```sh
$ docker build --build-arg ICU_PATH=https://github.com/unicode-org/icu/releases/download/release-65-1/icu4c-65_1-src.tgz -t icu4c-demos:my-demos  . -f icu-kube/docker.d/icu4c-demos/Dockerfile
```

- If all goes well (it did, right?): you can now run

```sh
$ docker run --rm -p 8888:8080 icu4c-demos:my-demos
```

… That will serve up the demos at http://localhost:8888/icu-bin/icudemos
