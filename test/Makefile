#
#
#
#
PLIB=../pylib
INCLUDE = -I./ -I../ -I$(PLIB)/include/
LDFLAGS     = -L./ -L../ -L$(PLIB)/lib/ -ldictmatch -lpylib -lpthread -lm -lz -g 
COMMON_DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 $(INCLUDE)  -g 

ifeq "$(MAKECMDGOALS)" "release"
	DEFINES=$(COMMON_DEFINES) -DNDEBUG 
	CPPFLAGS= $(DEFINES) 
else
	ifeq "$(MAKECMDGOALS)" "withpg"
		DEFINES=$(COMMON_DEFINES) 
		CPPFLAGS= -g -pg $(DEFINES) 
	else
		DEFINES=$(COMMON_DEFINES)
		CPPFLAGS= -g -O3 $(DEFINES) 
	endif
endif
CC  = g++
AR  = ar
#=========================================================================

EXECUTABLE =  do_dictmatch build_dmdict 

TEST_EXEC = 

all	:  $(EXECUTABLE) $(LIBS) $(TEST_EXEC)

deps :
	$(CC) -MM -MG *.cpp >depends

do_dictmatch: do_dictmatch.o
	$(CC) -o $@ $^ $(LDFLAGS)


build_dmdict: build_dmdict.o dm_prop.o
	$(CC) -o $@ $^ $(LDFLAGS)


rebuild : clean all
clean   :
	/bin/rm -f *.o core.* *~ $(EXECUTABLE) $(TEST_EXEC) 


release : all
withpg  : all

-include depends

