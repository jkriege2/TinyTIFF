.PHONY = clean all pdf

OS := $(shell uname -o)
TIFF_INC=-I../../../../../FCSTOOLS/trunk/QuickFit3/extlibs/libtiff/include
TIFF_LIB=-L../../../../../FCSTOOLS/trunk/QuickFit3/extlibs/libtiff/lib -ltiff -lz -L../../../../../FCSTOOLS/trunk/QuickFit3/extlibs/libjpeg/lib -ljpeg

ifeq ($(findstring Msys,$(OS)),Msys)
PREFIX=/mingw
EXE_SUFFIX=.exe
SO_SUFFIX=.dll
SO_PATH=$(PREFIX)/bin
else
PREFIX=/usr/local
EXE_SUFFIX=
SO_SUFFIX=.so
SO_PATH=$(PREFIX)/lib
endif


INCLUDE_PATH=$(PREFIX)/include
LIB_PATH=$(PREFIX)/lib
BIN_PATH=$(PREFIX)/bin
LIB_NAME = tinytiffwriter

ifeq ($(findstring Msys,$(OS)),Msys)
SHARED_OPTS=-Wl,--out-implib,$(SO_NAME).a,-soname,$(SO_NAME)
else
SHARED_OPTS=
endif

SRC_LIB = ../../tinytiffwriter.cpp
#HPP_LIB = yaid.hpp serialization.hpp exception.hpp types.hpp endianness.hpp
SRC_TEST = ../../tinytiffhighrestimer.cpp  ../../libtiff_tools/libtiff_tools.cpp main.cpp
#HPP_TEST = yaid_test.hpp


TAR_STATIC_LIB = $(subst .cpp,.static.o,$(SRC_LIB))
TAR_TEST = $(subst .cpp,.o,$(SRC_TEST))

CC = g++

OPTIMIZATION_OPTS = -O2
LD_OPTS = $(DBG_LD) -ltiff -lz
CC_OPTS = $(DBG_CC) -Wall -DTEST_LIBTIFF $(OPTIMIZATION_OPTS) 

debug: CC_OPTS += -DDEBUG -g

#ifeq (LIBTIFF_EXISTS,1)
CC_OPTS = -Wall -DTEST_LIBTIFF $(OPTIMIZATION_OPTS) $(TIFF_INC)
LD_OPTS = $(TIFF_LIB)
#endif
#-D__USE_LIBC_FOR_TIFF__

CC_STATIC_OPTS = $(CC_OPTS) $(OPTIMIZATION_OPTS)
LD_STATIC_OPTS = $(CC_STATIC_OPTS) -Wl,-rpath $(LIB_PATH)


debug: lib$(LIB_NAME).a test$(EXE_SUFFIX)
all: lib$(LIB_NAME).a test$(EXE_SUFFIX)

-include $(TAR_STATIC_LIB:.o=.d) $(TAR_TEST:.o=.d)

$(TAR_STATIC_LIB): %.static.o: %.cpp
	$(CC) $(CC_STATIC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.static.d
	@mv -f $*.static.d $*.d.tmp
	@sed -e 's|.*:|$*.static.o:|' < $*.d.tmp > $*.static.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.static.d
	@rm -f $*.d.tmp

$(TAR_TEST): %.o: %.cpp
	$(CC) $(CC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

lib$(LIB_NAME).a: $(TAR_STATIC_LIB) $(HPP_LIB)
	ar -cvur lib$(LIB_NAME).a $(TAR_STATIC_LIB)

install: lib$(LIB_NAME).a $(HPP_LIB)
	mkdir -p $(INCLUDE_PATH)/yaid
	cp $(HPP_LIB) $(INCLUDE_PATH)/yaid/
	cp lib$(LIB_NAME).a $(LIB_PATH)

test$(EXE_SUFFIX): $(TAR_TEST) lib$(LIB_NAME).a
	$(CC) $(CC_OPTS) $(TAR_TEST) -o $@ $(LD_OPTS) -Wl,--library-path,./ -L./ -l$(LIB_NAME)



clean:
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f lib$(LIB_NAME).a
	-rm -f test
	-rm -f *.exe
