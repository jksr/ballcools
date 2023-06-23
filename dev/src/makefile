CC = g++
CFLAGS = -w -std=c++11 -O3 #-finline-functions -fPIC -Wno-unused-result
CLIB = -lhts -ldeflate -lbz2 -lpthread -lz -L/data/wtian/local/miniconda3/envs/sapiens/lib
CINCLUDE = /data/wtian/local/miniconda3/envs/sapiens/include
# CF_OPTIMIZE = 1

# OS := $(shell uname)
# ifeq ($(OS),  Darwin)
# 	CFLAGS += -Wno-unused-function
# else
# #	CLIB += -lrt -ltinfo
# endif

# INCLUDE = include

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

# LHTSLIB_DIR = htslib
# CINCLUDE = htslib/htslib
# LHTSLIB = $(LHTSLIB_DIR)/libhts.a
# $(LHTSLIB) :
# 	make -C $(LHTSLIB_DIR) libhts.a

###################
### subcommands ###
###################


out/allc.o: allc.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc.o: ballc.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_index.o: ballc_index.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_index_core.o: ballc_index_core.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_iterator.o: ballc_iterator.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/context_matcher.o: context_matcher.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/hashtable.o: hashtable.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/meta_indexing.o: meta_indexing.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/utils.o: utils.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/timer.o: timer.cc
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

#chunk.o: chunk.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(CINCLUDE) $< -o $@
#
#header.o: header.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(CINCLUDE) $< -o $@
#
#bundle.o: bundle.c
#	$(CC) -c $(CFLAGS) -I$(LUTILS_DIR) -I$(CINCLUDE) $< -o $@
#
LIBS=out/allc.o out/ballc_index_core.o out/ballc_index.o out/ballc_iterator.o out/ballc.o out/context_matcher.o out/hashtable.o out/meta_indexing.o out/timer.o out/utils.o

ballcools: $(LIBS) main.cc merge_ballc.h
	g++ -I$(CINCLUDE) $(CFLAGS) main.cc -o $@ $(LIBS) $(CLIB)


### clean just src
#.PHONY: clean
#clean :
#	rm -f *.o tbmate
#	make -C $(LHTSLIB_DIR) clean
#
