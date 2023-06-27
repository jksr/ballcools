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


out/allc.o: allc.cc allc.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc.o: ballc.cc ballc.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_index.o: ballc_index.cc  ballc_index.h 
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_index_core.o: ballc_index_core.cc ballc_index_core.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/ballc_iterator.o: ballc_iterator.cc ballc_iterator.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/context_matcher.o: context_matcher.cc context_matcher.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

# out/hashtable.o: hashtable.cc hashtable.h
# 	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/meta_indexing.o: meta_indexing.cc meta_indexing.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/utils.o: utils.cc utils.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

out/timer.o: timer.cc timer.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@


# LIBS=out/hashtable.o out/allc.o out/ballc_index_core.o out/ballc_index.o out/ballc_iterator.o out/ballc.o out/context_matcher.o out/meta_indexing.o out/timer.o out/utils.o
LIBS=out/allc.o out/ballc_index_core.o out/ballc_index.o out/ballc_iterator.o out/ballc.o out/context_matcher.o out/meta_indexing.o out/timer.o out/utils.o

ballcools: $(LIBS) main.cc routines.h version.h
	g++ -I$(CINCLUDE) $(CFLAGS) main.cc -o $@ $(LIBS) $(CLIB)

clean:
	rm ballcools out/*.o

### clean just src
#.PHONY: clean
#clean :
#	rm -f *.o tbmate
#	make -C $(LHTSLIB_DIR) clean
#
