# © 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

FROM alpine:latest as build
LABEL maintainer="srl@icu-project.org"

USER root
ENV HOME /home/build

RUN apk --update add gcc make python g++ ccache valgrind doxygen tar zip curl wget git bash bsd-compat-headers
RUN addgroup build && adduser -g "Build user" -h $HOME -S -G build -D -s /bin/sh build

## Change this to a file:///mnt/icu/blah.tgz to replace, or another URL.
ARG ICU_PATH
ENV ICU_PATH=${ICU_PATH}

RUN if [ $ICU_PATH = "system" ]; then apk --update add icu-dev; else curl -L $ICU_PATH | (cd / ; tar xvfpz - --strip-components 2 ); fi
USER build

# Wanted to use this: --wildcards -s '%icu[^/]*%%' 

ENV LD_LIBRARY_PATH /usr/local/lib

# Make sure ICU is installed and working
# VOLUME /mnt/icu
RUN icuinfo | tee /tmp/icuinfo.txt

USER build
# VOLUME /home/build
RUN mkdir /home/build/build /home/build/src
WORKDIR /home/build/build
ADD . /home/build/src/
ENV PREFIX=/home/build/icu
RUN ~/src/configure  --prefix=${PREFIX} ICU_CONFIG=$(which icu-config) 
RUN make all OBS= && make install OBS= DESTDIR=${PREFIX}
#make -k DEPS= clean && make -k DEPS= distclean && sudo apt-get purge -y git subversion python3 doxygen zip curl g++ && sudo apt-get -y autoremove && sudo apt-get clean -y
#RUN make all

FROM alpine:latest as httpd
USER root
ARG ICU_PATH
ENV ICU_PATH=${ICU_PATH}
RUN apk --update add lighttpd
# run this again. Two downloads, but keeps the layers smaller..
RUN if [ $ICU_PATH = "system" ]; then apk --update add icu-dev ; else apk --update add libstdc++ curl ; curl -L $ICU_PATH | (cd / ; tar xvfpz - --strip-components 2 ); fi
COPY --from=build /home/build/icu /home/build/icu
RUN if [ -d /home/build/icu/usr/local ]; then (cd /home/build/icu/usr/; ln -sv local/* .); fi; ls -l /home/build/icu/usr/bin/
ENV LD_LIBRARY_PATH /home/build/icu/usr/lib
EXPOSE 80
COPY icu-kube/lighttpd.conf /etc/lighttpd/lighttpd.conf
CMD ["lighttpd", "-D", "-f", "/etc/lighttpd/lighttpd.conf"]
