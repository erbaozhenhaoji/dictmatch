#
#
#
#
INCLUDE = -I./ -I../ 
LDFLAGS     = -L./ -L../ -lpycode -lpthread -lcrypto -lm -lz -g
COMMON_DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 $(INCLUDE)  -g

ifeq "$(MAKECMDGOALS)" "release"
	DEFINES=$(COMMON_DEFINES) -DNDEBUG -O3
	CPPFLAGS= $(DEFINES) 
else
	ifeq "$(MAKECMDGOALS)" "withpg"
		DEFINES=$(COMMON_DEFINES) 
		CPPFLAGS= -g -pg $(DEFINES) 
	else
		DEFINES=$(COMMON_DEFINES)
		CPPFLAGS= -g $(DEFINES) 
	endif
endif
CC  = g++
AR  = ar
#=========================================================================
PYCODELIB=../libpycode.a

EXECUTABLE =  trans_code \
              print_unicode \
              gen_utf8_hz \
              make_code_table 

TEST_EXEC = 

all	:  $(EXECUTABLE) $(LIBS) $(TEST_EXEC)

deps :
	$(CC) -MM -MG *.cpp >depends

trans_code : trans_code.o $(PYCODELIB)
	$(CC) -o $@ $^ $(LDFLAGS)

print_unicode : print_unicode.o $(PYCODELIB)
	$(CC) -o $@ $^ $(LDFLAGS)


gen_utf8_hz : gen_utf8_hz.o $(PYCODELIB)
	$(CC) -o $@ $^ $(LDFLAGS)

make_code_table : make_code_table.o $(PYCODELIB)
	$(CC) -o $@ $^ $(LDFLAGS)


rebuild : clean all
clean   :
	/bin/rm -f *.o core.* *~ $(EXECUTABLE) $(TEST_EXEC) 


release : all
withpg  : all

-include depends

