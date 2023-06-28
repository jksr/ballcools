CC = g++
CFLAGS = -w -std=c++11 -O3 #-finline-functions -fPIC -Wno-unused-result
CLIB = -lhts -ldeflate -lbz2 -lpthread -lz -L${CONDA_PREFIX}/lib
CINCLUDE = ${CONDA_PREFIX}/include

SRCDIR = src
BINDIR = out
OUTDIR = bin
# CF_OPTIMIZE = 1

# OS := $(shell uname)
# ifeq ($(OS),  Darwin)
# 	CFLAGS += -Wno-unused-function
# else
# #	CLIB += -lrt -ltinfo
# endif

# INCLUDE = include

PROG = ${BINDIR}/ballcools

.PHONY: build
build: exportcf $(PROG)

exportcf:
	$(eval export CF_OPTIMIZE)

${OUTDIR}/allc.o: ${SRCDIR}/allc.cc ${SRCDIR}/allc.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/ballc.o: ${SRCDIR}/ballc.cc ${SRCDIR}/ballc.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/ballc_index.o: ${SRCDIR}/ballc_index.cc  ${SRCDIR}/ballc_index.h 
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/ballc_index_core.o: ${SRCDIR}/ballc_index_core.cc ${SRCDIR}/ballc_index_core.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/ballc_iterator.o: ${SRCDIR}/ballc_iterator.cc ${SRCDIR}/ballc_iterator.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/context_matcher.o: ${SRCDIR}/context_matcher.cc ${SRCDIR}/context_matcher.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

# ${OUTDIR}/hashtable.o: hashtable.cc hashtable.h
# 	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/meta_indexing.o: ${SRCDIR}/meta_indexing.cc ${SRCDIR}/meta_indexing.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/utils.o: ${SRCDIR}/utils.cc ${SRCDIR}/utils.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@

${OUTDIR}/timer.o: ${SRCDIR}/timer.cc ${SRCDIR}/timer.h
	$(CC) -c $(CFLAGS) -I$(CINCLUDE) $< -o $@


# LIBS=${OUTDIR}/hashtable.o ${OUTDIR}/allc.o ${OUTDIR}/ballc_index_core.o ${OUTDIR}/ballc_index.o ${OUTDIR}/ballc_iterator.o ${OUTDIR}/ballc.o ${OUTDIR}/context_matcher.o ${OUTDIR}/meta_indexing.o ${OUTDIR}/timer.o ${OUTDIR}/utils.o
LIBS=${OUTDIR}/allc.o ${OUTDIR}/ballc_index_core.o ${OUTDIR}/ballc_index.o ${OUTDIR}/ballc_iterator.o ${OUTDIR}/ballc.o ${OUTDIR}/context_matcher.o ${OUTDIR}/meta_indexing.o ${OUTDIR}/timer.o ${OUTDIR}/utils.o

${BINDIR}/ballcools: $(LIBS) ${SRCDIR}/main.cc ${SRCDIR}/routines.h ${SRCDIR}/version.h
	g++ -I$(CINCLUDE) $(CFLAGS) ${SRCDIR}/main.cc -o $@ $(LIBS) $(CLIB)

clean:
	rm ${BINDIR}/ballcools ${OUTDIR}/*.o

### clean just src
#.PHONY: clean
#clean :
#	rm -f *.o tbmate
#	make -C $(LHTSLIB_DIR) clean
#
