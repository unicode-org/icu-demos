SOURCE      =collationRegressionTest.cpp
ICU_PREFIX	=/home/michael/Workspace/Install/ICU4C
ICU_INCLUDE	=-I$(ICU_PREFIX)/include
ICU_LIB		=-L$(ICU_PREFIX)/lib
MISC_LIBS   =-lpthread -lm
LIBS		=-licutu -licuuc -licui18n -licuio -licule -licudata

OBJECTS =collationRegressionTest.o
TARGET  =collationRegressionTest

DIR =/home/michael/Desktop/ICUReg/result
DATAFILE =/home/michael/Desktop/ICUReg/collRegTest.dat

all : clean $(TARGET) 

$(TARGET) : $(OBJECTS)
	g++ -Wall -ansi -o $(TARGET) $(OBJECTS) $(MISC_LIBS) $(ICU_LIB) $(LIBS)

$(OBJECTS) : 
	g++ -Wall -ansi $(ICU_INCLUDE) -g -c $(SOURCE)

run : $(TARGET)
	#mkdir $(DIR)
	./$(TARGET) -d $(DIR) -f $(DATAFILE)

clean :
	rm -rf *.o $(OBJECTS) $(TARGET) result result.log
    
