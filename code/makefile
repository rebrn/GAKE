CC=g++
CFLAGS=-c -Wall
LDFLAGS=-pthread
SOURCES=main.cpp common.cpp CBOWPath.cpp TripleClassification.cpp numeric.cpp °∆GenerateContext.txt
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

