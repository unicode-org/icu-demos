#  Copyright (c) 2010 IBM Corporation and Others. All Rights Reserved.

ALLJAVA=$(shell ls *.java)
ALLCLASS=$(ALLJAVA:%.java=%.class)
ALLAPP=$(ALLJAVA:%.java=%)
CLASSPATH=.:$(ICU4J)

# create a 'local.mk' with a line like this:
## ICU4J=/Users/srl/Downloads/icu4j-4_4.jar
include local.mk

all: $(ALLCLASS)

check: all
	for class in $(ALLAPP); do echo ____ $$class ____; java -classpath $(CLASSPATH):. $$class; echo; done

check-%: %.class
	echo running %*
	java -classpath $(CLASSPATH) $*

%.class: %.java
	javac -classpath $(CLASSPATH) $^

clean:
	-rm *.class
	-rm -f myapp.lst

CDATA=../c/data

#
# This target rebuilds data from C
#
rebuild-data: $(CDATA)/myapp.dat
	-rm -rf data/myapp/*.res
	-mkdir -p data/myapp
	icupkg -l $(CDATA)/myapp.dat > myapp.lst
	icupkg -x myapp.lst -tb -d data $(CDATA)/myapp.dat
	-rm -f myapp.lst

$(CDATA)/myapp.dat:
	make -C $(CDATA) clean all
