CC=g++
CFLAGS=-c -w -lstdc++ -fPIC
LIB_NAME=filters
LDFLAGS=-lstdc++ -shared
ifeq ($(wildcard file1),)
	JSON_PATH = $(GSQL_PROJ_HOME)/gdk/gsdk/include/third_party/jsoncpp
	RESTPP_PATH = $(GSQL_PROJ_HOME)/src/realtime/restpp
else
	JSON_PATH =  $(GSQL_PROJ_HOME)/src/third_party/jsoncpp
	RESTPP_PATH = $(GSQL_PROJ_HOME)/gdk/gsdk/include/realtime/restpp
endif
INCLUDE=-I$(JSON_PATH)/include -I$(GSQL_PROJ_HOME)/src/realtime/
SYS_SRC=$(JSON_PATH)/json_reader.cpp $(JSON_PATH)/json_writer.cpp $(JSON_PATH)/json_value.cpp
SOURCES=$(wildcard *.cpp) $(RESTPP_PATH)/idset.cpp $(RESTPP_PATH)/schemamanager.cpp
OBJECTS=$(SOURCES:.cpp=.o) $(SYS_SRC:.cpp=.o)
EXECUTABLE=$(LIB_NAME).so

all: $(SOURCES) $(SYS_SRC) $(EXECUTABLE)

debug: CC += -g
debug: $(EXECUTABLE)

opt: CC += -O3
opt: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
		$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDE) $< -o $@


clean:
		rm -rf *.o *.so $(JSON_PATH)/json_reader.o $(JSON_PATH)/json_writer.o $(JSON_PATH)/json_value.o
