GCC=g++-7

LIBIBERTYDIR=$(shell brew --prefix)/lib/x86_64
GMPDIR=$(shell brew --prefix)/Cellar/gmp4/4.3.2

UNAME:= $(shell uname -s)

RWCALLS=
RWCALLSBIN=


ifneq "$(RDTYPE)" ""
RDOPTS=-D$(RDTYPE)
endif

FOO:= $(shell echo $(UNAME))
#GCCPLUGINS_DIR:= $(shell $(GCC) -print-file-name=plugin)
GCCPLUGINS_DIR:= $(shell $(GCC) -print-file-name=plugin)
CFLAGS+=  -I${GMPDIR}/include/ -I${GCCSRCDIR}/include -I$(GCCPLUGINS_DIR)/include -fPIC -g -fno-rtti $(RDOPTS)  -w   
X=-I${GMPDIR}/include/ -I${GCCSRCDIR}/include
#-fopenmp
#LDFLAGS+= -shared  -fopenmp -flto  -Wl,--print-memory-usage
LDFLAGS+= -shared  -flto 
all: newplugin.so

%.o: %.cpp
	$(GCC) $(CFLAGS) -c $^ -o $@

%.o: %.c
	$(GCC) $(CFLAGS) -c $^ -o $@

newplugin.so: gcc_plugin.o $(RWCALLSBIN) 
	$(GCC)  $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	-rm newplugin.so newplugin.o
