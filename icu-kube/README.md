Dockerized ICU4C Demos
###

- Build with `sh build.sh`

- Run with `docker run --rm -p 18080:8080 unicode/icu4c-demos:latest` - will listen on port 18080

## Building with a special ICU version

(Advanced use)

- use the [icu-docker repo with #15 fixed](https://github.com/unicode-org/icu-docker/pull/15) and create an alpine binary package:

```sh
$ make dist-some DISTROS_SMALL=alpine
```

Now serve up the `icu-r84d16d8c6c-x86_64-pc-linux-gnu-Alpine_Linux-3.10.2.tgz` (or similar) binary file:

```sh
$ cd icu-docker/src/dist/
$ npx serve
```

Leave `npx serve` running in another window.

Make a note of the IP address given by the `npx serve` command. Visit that URL and construct a URL to the `icu-r84d16d8c6c-x86_64-pc-linux-gnu-Alpine_Linux-3.10.2.tgz` file such as

    http://999.999.999.999:5000/icu-r84d16d8c6c-x86_64-pc-linux-gnu-Alpine_Linux-3.10.2.tgz

- Now you are ready to build:

```sh
$ docker build --build-arg ICU_PATH=http://999.999.999.999:5000/icu-r84d16d8c6c-x86_64-pc-linux-gnu-Alpine_Linux-3.10.2.tgz -t icu4c-demos:my-demos  . -f icu-kube/docker.d/icu4c-demos/Dockerfile
```

- If all goes well (it did, right?): you can now run

```sh
$ docker run --rm -p 8888:8080 icu4c-demos:my-demos
```

… That will serve up the demos at http://localhost:8888/icu-bin/icudemos
