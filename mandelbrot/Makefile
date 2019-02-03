CC=g++ -g
CFLAGS=-c -std=c++11 -Iglad/include
LDFLAGS=-lglfw -ldl
WFLAGS=-Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-ignored-qualifiers
SOURCES=src/main.cpp src/Magic.cpp glad/src/glad.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=fragfractal

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(IFLAGS) $(CFLAGS) $(WFLAGS) $< -o $@

clean:
	rm src/*.o
	rm $(EXECUTABLE)
