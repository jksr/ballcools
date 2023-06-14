CC = g++
CFLAGS = -W -Wall -finline-functions -fPIC -std=c++11 -Wno-unused-result -O3
#CLIB = -lncurses -lpthread -lz -lm
CLIB = -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib
CF_OPTIMIZE = 1

OS := $(shell uname)
ifeq ($(OS),  Darwin)
	CFLAGS += -Wno-unused-function
else
#	CLIB += -lrt -ltinfo
endif

INCLUDE = include

PROG = ballcools

.PHONY: build
build: exportcf $(PROG)

exportcf:
	$(eval export CF_OPTIMIZE)

#.PHONY: debug
#debug: CF_OPTIMIZE := 0
#debug: CFLAGS += -g # -pg
#debug: CFLAGS := $(filter-out -O3,$(CFLAGS))
#debug: build

#####################
##### libraries #####
#####################

LHTSLIB_DIR = htslib
LHTSLIB_INCLUDE = htslib/htslib
LHTSLIB = $(LHTSLIB_DIR)/libhts.a
$(LHTSLIB) :
	make -C $(LHTSLIB_DIR) libhts.a

###################
### subcommands ###
###################

allc.o: allc.cc
	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@

ballc.o: ballc.cc
	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@

hashtable.o: hashtable.cc
	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@

meta_indexing.o: meta_indexing.cc
	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@

utils.o: utils.cc
	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@

#chunk.o: chunk.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@
#
#header.o: header.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@
#
#bundle.o: bundle.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(LHTSLIB_INCLUDE) $< -o $@
#
LIBS=allc.o ballc.o hashtable.o meta_indexing.o utils.o $(LHTSLIB)

ballcools: $(LIBS) main_allc2ballc.c
	gcc $(CFLAGS) main_allc2ballc.c -o $@ $(LIBS) $(CLIB)


### clean just src
#.PHONY: clean
#clean :
#	rm -f *.o tbmate
#	make -C $(LHTSLIB_DIR) clean
#
