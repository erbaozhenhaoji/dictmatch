#
#
#
#
PLIB=..
INCLUDE = -I./ -I$(PLIB)/include
LDFLAGS     = -L./ -L$(PLIB)/lib -lpylib -lpthread -lm -lz -g
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

EXECUTABLE =  test_pdict \
	      test_pdict_create \
	      test_log \
	      test_code \
	      split_file \
	      test_split

TEST_EXEC = 

all	:  $(EXECUTABLE) $(LIBS) $(TEST_EXEC)

deps :
	$(CC) -MM -MG *.cpp >depends

test_split: test_split.o
	$(CC) -o $@ $^ $(LDFLAGS)

split_file : split_file.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_term_cache : test_term_cache.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_wordseg : test_wordseg.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_pdict : test_pdict.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_code : test_code.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_svr : test_svr.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_cli : test_cli.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_pack_svr : test_pack_svr.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_pack_cli : test_pack_cli.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_log : test_log.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_timer : test_timer.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_pdict_create : test_pdict_create.o
	$(CC) -o $@ $^ $(LDFLAGS)

rebuild : clean all
clean   :
	/bin/rm -f *.o core.* *~ $(EXECUTABLE) $(TEST_EXEC) 


release : all
withpg  : all

-include depends

