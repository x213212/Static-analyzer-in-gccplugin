# Make file for parallel BZIP2
SHELL = /bin/sh

PLUGIN=-fplugin=./misra.so
# Compiler to use
# CXX = g++
LTO=-flto
FLAGS=-O1   -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta  -pthread 
# Thread-related flags
# On some compilers -pthreads
CXXFLAGS_PTHREAD = -pthread

# Comment out CXXFLAGS line below to disable pthread semantics in code
CXXFLAGS_PTHREAD += -D_POSIX_PTHREAD_SEMANTICS

LDLIBS_PTHREAD = -lpthread


# Optimization flags
CXXFLAGS = -O2

#CXXFLAGS += -g -Wall
#CXXFLAGS += -ansi
#CXXFLAGS += -pedantic
#CXXFLAGS += -std=c++0x

# Comment out CXXFLAGS line below for compatability mode for 32bit file sizes
# (less than 2GB) and systems that have compilers that treat int as 64bit
# natively (ie: modern AIX)
CXXFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

# Uncomment CXXFLAGS line below if you want to compile pbzip2 without load
# average support for systems that do not support it
#CXXFLAGS += -DPBZIP_NO_LOADAVG

# Uncomment CXXFLAGS line below to get debug output
#CXXFLAGS += -DPBZIP_DEBUG

# Comment out CXXFLAGS line below to disable Thread stack size customization
CXXFLAGS += -DUSE_STACKSIZE_CUSTOMIZATION

# Comment out CXXFLAGS line below to explicity set ignore trailing garbage
# default behavior: 0 - disabled; 1 - enabled (ignore garbage by default)
# If IGNORE_TRAILING_GARBAGE is not defined: behavior is automatically determined
# by program name: bzip2, bunzip2, bzcat - ignore garbage; otherwise - not.
#CXXFLAGS += -DIGNORE_TRAILING_GARBAGE=1

# Add thread-related flags
CXXFLAGS += $(CXXFLAGS_PTHREAD)


# Linker flags
LDFLAGS =


# External libraries
LDLIBS = -lbz2
LDLIBS += $(LDLIBS_PTHREAD)


# Where you want pbzip2 installed when you do 'make install'
PREFIX = /usr


all: pbzip2

# Standard pbzip2 compile
pbzip2: pbzip2.cpp BZ2StreamScanner.cpp ErrorContext.cpp
	$(CXX) $(CXXFLAGS) $(PLUGIN) $(LTO)  $(FLAGS)  $(LDFLAGS) $^ -o pbzip2 2> $@.log $(LDLIBS)

# Choose this if you want to compile in a static version of the libbz2 library
pbzip2-static: pbzip2.cpp BZ2StreamScanner.cpp ErrorContext.cpp libbz2.a
	$(CXX) $(CXXFLAGS)   $(FLAGS)  $(LDFLAGS) $^ -o pbzip2 -I. -L. $(LDLIBS)

# Install the binary pbzip2 program and man page
install: pbzip2
	if ( test ! -d $(PREFIX)/bin ) ; then mkdir -p $(PREFIX)/bin ; fi
	if ( test ! -d $(PREFIX)/man ) ; then mkdir -p $(PREFIX)/man ; fi
	if ( test ! -d $(PREFIX)/man/man1 ) ; then mkdir -p $(PREFIX)/man/man1 ; fi
	cp -f pbzip2 $(PREFIX)/bin/pbzip2
	chmod a+x $(PREFIX)/bin/pbzip2
	ln -s -f $(PREFIX)/bin/pbzip2 $(PREFIX)/bin/pbunzip2
	ln -s -f $(PREFIX)/bin/pbzip2 $(PREFIX)/bin/pbzcat
	cp -f pbzip2.1 $(PREFIX)/man/man1
	chmod a+r $(PREFIX)/man/man1/pbzip2.1

clean:
	rm -f *.o pbzip2
