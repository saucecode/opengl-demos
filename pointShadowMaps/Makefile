CC=g++-5
CFLAGS=-c -std=c++14
LDFLAGS=-lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -lSOIL
WFLAGS=-Wall -Wextra
SOURCES=main.cpp Shader.cpp Mesh.cpp Light.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=a.out

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(IFLAGS) $(CFLAGS) $(WFLAGS) $< -o $@
