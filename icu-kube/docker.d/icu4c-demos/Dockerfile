# © 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html

FROM alpine:latest as build
LABEL maintainer="srl@icu-project.org"

USER root
ENV HOME /home/build

RUN apk --update add gcc make python3 g++ ccache valgrind pkgconfig doxygen tar zip curl wget git bash bsd-compat-headers
RUN addgroup build && adduser -g "Build user" -h $HOME -S -G build -D -s /bin/sh build

## Change this to a file:///mnt/icu/blah.tgz to replace, or another URL.
ARG ICU_PATH
ENV ICU_PATH=${ICU_PATH}

# The fun one. Note DEPS= speeds up the build when we're doing a one time compile.
# nproc gives the number of processors.
RUN if [ $ICU_PATH = "system" ]; then apk --update add icu-dev; \
 else curl -L $ICU_PATH | (mkdir /tmp/icu && cd /tmp/icu && tar xvfpz - && \
  cd icu/source && ./configure --prefix=/usr && make DEPS= -j$(nproc) install && make DEPS= -j$(nproc)  DESTDIR=/tmp/ICU/usr install); fi
USER build

# Wanted to use this: --wildcards -s '%icu[^/]*%%' 

ENV LD_LIBRARY_PATH /usr/local/lib

# Make sure ICU is installed and working
# VOLUME /mnt/icu
RUN icuinfo | tee /tmp/icuinfo.txt

# VOLUME /home/build
RUN mkdir /home/build/build /home/build/src
WORKDIR /home/build/build
ENV PREFIX=/home/build/icu
RUN if [ -d /tmp/ICU ]; then mkdir -p /home/build/icu/ICU && cp -R /tmp/ICU /home/build/icu/ICU; fi
ADD --chown=build . /home/build/src/
# HACK: translitdemo is not out of source enabled
RUN cp -R /home/build/src/translitdemo /home/build/build/translitdemo
#  No -j$(nproc) - not multithread safe!
RUN /home/build/src/configure  --prefix=${PREFIX} ICU_CONFIG=$(which icu-config) && \
 make all install OBS= DESTDIR=${PREFIX} CONTEXTPATH=/icu-bin/ ICU_COMMON_HEADERS=/home/build/src/icu-kube/icuheaders
#make -k DEPS= clean && make -k DEPS= distclean && sudo apt-get purge -y git subversion python3 doxygen zip curl g++ && sudo apt-get -y autoremove && sudo apt-get clean -y
#RUN make all
RUN cp -R /home/build/src/translitdemo /home/build/icu/translitdemo

FROM alpine:latest as httpd
USER root
ARG ICU_PATH
ENV ICU_PATH=${ICU_PATH}
RUN apk --update add lighttpd
COPY --from=build /home/build/icu /home/build/icu
# Try to re-install the same ICU
RUN if [ $ICU_PATH = "system" ]; then apk --update add icu-dev; \
 else apk --update add libstdc++ && cp -Rv /home/build/icu/ICU/ICU/usr/usr / && rm -rf /home/build/icu/ICU/usr ; fi
RUN if [ -d /home/build/icu/usr/local ]; then (cd /home/build/icu/usr/; ln -sv local/* .); fi; ls -l /home/build/icu/usr/bin/
ENV LD_LIBRARY_PATH /home/build/icu/usr/lib
EXPOSE 8080
# this needs special treatment
RUN cp -v /home/build/icu/usr/bin/data/collation.html /var/www/localhost/htdocs/ || true
COPY icu-kube/lighttpd.conf /etc/lighttpd/lighttpd.conf
COPY icu-kube/index.html /var/www/localhost/htdocs/index.html
CMD ["lighttpd", "-D", "-f", "/etc/lighttpd/lighttpd.conf"]
