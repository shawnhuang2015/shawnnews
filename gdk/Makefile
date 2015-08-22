cppFlagCmd = ../src/er/buildenv/get_cpp_flags
ifneq ("$(wildcard $(cppFlagCmd))","")
CFLAGS := $(shell $(cppFlagCmd) )
else
$(error CFLAGS is not set. Please run "gpkg.sh" instead)
endif

export CFLAGS

all:
	./gpkg.sh

gpe:
	@make -f MakeGPE all

rest:
	@make -f MakeREST all

demo:
	@make clean
	@make -f MakeDemo all

clean:
	\rm -rf objs/*.o bin/release/*
