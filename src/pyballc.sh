swig -c++ -python pyballc.i

libs="allc.cc ballc.cc ballc_index.cc ballc_index_core.cc ballc_iterator.cc context_matcher.cc meta_indexing.cc timer.cc utils.cc"
g++  pyballc_wrap.cxx $libs -fPIC -shared  $(python3-config --includes) -I${CONDA_PREFIX}/include -w -std=c++11 -O3  -L${CONDA_PREFIX}/lib -lhts -ldeflate -lbz2 -lpthread -lz -lstdc++ -o _pyballc.so
