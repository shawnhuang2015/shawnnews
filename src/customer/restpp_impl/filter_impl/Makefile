CC=g++
CFLAGS=-c -w -lstdc++ -fPIC
LIB_NAME=filters
LDFLAGS=-lstdc++ -shared #-Wl,-soname,$(LIB_NAME).so.1 #-L/home/vagrant/ghttp/product/src/third_party/yaml-shared/usr/local/lib -lyaml-cpp
INCLUDE=-I$(GSQL_PROJ_HOME)/src/third_party/jsoncpp/include -I$(GSQL_PROJ_HOME)/src/core/ -I$(GSQL_PROJ_HOME)/src/core/gse/ -I$(GSQL_PROJ_HOME)/src/utility/ -I$(GSQL_PROJ_HOME)/src/third_party/sparsehash/usr/local/include/ -I$(GSQL_PROJ_HOME)/src/third_party/yaml/usr/local/include/ -I$(GSQL_PROJ_HOME)/src/core/topology -I$(GSQL_PROJ_HOME)/src/realtime/
#SYS_SRC will be replaced by static libraries
SYS_SRC=$(GSQL_PROJ_HOME)/src/realtime/restpp/vertexedgeset.cpp $(GSQL_PROJ_HOME)/src/realtime/restpp/graphdatatype.cpp $(GSQL_PROJ_HOME)/src/realtime/restpp/schemamanager.cpp $(GSQL_PROJ_HOME)/src/realtime/restpp/post2binary.cpp $(GSQL_PROJ_HOME)/src/realtime/restpp/deltawriter.cpp $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_reader.cpp $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_writer.cpp $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_value.cpp
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
		rm -rf *.o *.so $(GSQL_PROJ_HOME)/src/realtime/restpp/vertexedgeset.o $(GSQL_PROJ_HOME)/src/realtime/restpp/graphdatatype.o $(GSQL_PROJ_HOME)/src/realtime/restpp/schemamanager.o $(GSQL_PROJ_HOME)/src/realtime/restpp/post2binary.o $(GSQL_PROJ_HOME)/src/realtime/restpp/deltawriter.o $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_reader.o $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_writer.o $(GSQL_PROJ_HOME)/src/third_party/jsoncpp/json_value.o
