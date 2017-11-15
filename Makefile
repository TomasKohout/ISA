CC=g++
CFLAGS=-c -g -pedantic 
LDFLAGS=-L/etc/ssl/lib -lssl -lcrypto
EXECUTABLE=popcl

SOURCES=$(addprefix ./src/, main.cpp Connection.cpp Error.cpp ConnectionInterface.cpp TLSConnection.cpp ParseParameters.cpp FileManipulator.cpp)

OBJECTS=$(SOURCES:.cpp=.o)


.PHONY: all clean

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)

