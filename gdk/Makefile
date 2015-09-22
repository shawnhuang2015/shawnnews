SHELL := /bin/bash
cppFlagCmd = ../src/er/buildenv/get_cpp_flags
ifneq ("$(wildcard $(cppFlagCmd))","")
CFLAGS := $(shell $(cppFlagCmd) )
else
$(error CFLAGS is not set. Please run "gpkg.sh" instead)
endif

export CFLAGS

all:
	./gpkg.sh

debug:
	./gpkg.sh -d

gpe:
	@make clean
	@make -f MakeGPE all

rest:
	@make clean
	@make -f MakeREST all

demo:
	@make clean
	@make -f MakeDemo all

clean:
	# querydispatcher.o is NOT compiled by Makefile
	\cp objs/querydispatcher.o /tmp/querydispatcher.o
	\rm -rf objs/*.o
	\mv /tmp/querydispatcher.o objs/
