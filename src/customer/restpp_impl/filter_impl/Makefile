CC=g++
CFLAGS=-c -w -lstdc++ -fPIC
LIB_NAME=filters
LDFLAGS=-lstdc++ -shared
RESTPP_PATH = $(GSQL_PROJ_HOME)/src/realtime/restpp
ifndef GSQL_PROJ_HOME:
	GSQL_PROJ_HOME=../../../../
endif
ifneq "$(wildcard $(GSQL_PROJ_HOME)/src/realtime/restpp)" ""
	JSON_PATH = $(GSQL_PROJ_HOME)/src/third_party/jsoncpp
	RESTPP_PATH = $(GSQL_PROJ_HOME)/src/realtime
else
	JSON_PATH = $(GSQL_PROJ_HOME)/gdk/gsdk/include/third_party/jsoncpp
	RESTPP_PATH = $(GSQL_PROJ_HOME)/gdk/gsdk/include/realtime
endif
INCLUDE=-I$(JSON_PATH)/include -I$(RESTPP_PATH)
SYS_SRC=$(JSON_PATH)/json_reader.cpp $(JSON_PATH)/json_writer.cpp $(JSON_PATH)/json_value.cpp
SOURCES=$(wildcard *.cpp)
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
		rm -rf *.o *.so $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_reader.o $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_writer.o $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_value.o
