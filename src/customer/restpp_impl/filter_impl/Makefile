CC=g++
CFLAGS=-c -w -lstdc++ -fPIC
LIB_NAME=filters
LDFLAGS=-lstdc++ -shared
INCLUDE=-I$(GSQL_PROJ_HOME)/src/third_party/jsoncpp/include -I$(GSQL_PROJ_HOME)/src/realtime/
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=$(LIB_NAME).so
all: $(SOURCES) $(EXECUTABLE)

debug: CC += -g
debug: $(EXECUTABLE)

opt: CC += -O3
opt: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $< -o $@


clean:
		rm -rf *.o *.so
