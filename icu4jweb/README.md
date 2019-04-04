### ICU Java Web Demos

# BUILDING

- `mvn package`

# RUNNING WITH [docker](https://docker.io)

- `docker build -t icu4jweb:latest .`
- `docker run --rm -p 8083:9080 icu4jweb:latest`  (^C to stop and delete the container)
- now go to [http://127.0.0.1:8083/icu4jweb/](http://127.0.0.1:8083/icu4jweb/)

# License

Please see [../LICENSE](../LICENSE)

> Copyright © 2016 and later Unicode, Inc. and others. All Rights Reserved.
Unicode and the Unicode Logo are registered trademarks 
of Unicode, Inc. in the U.S. and other countries.
[Terms of Use and License](http://www.unicode.org/copyright.html)