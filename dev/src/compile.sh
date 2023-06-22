g++ -w -O3 -c allc.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/allc.o
g++ -w -O3 -c hashtable.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/hashtable.o
g++ -w -O3 -c ballc.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/ballc.o
g++ -w -O3 -c ballc_index.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/ballc_index.o
g++ -w -O3 -c ballc_index_core.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/ballc_index_core.o
g++ -w -O3 -c utils.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/utils.o
g++ -w -O3 -c timer.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/timer.o
g++ -w -O3 -c meta_indexing.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/meta_indexing.o
g++ -w -O3 -c ballc_iterator.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/ballc_iterator.o
g++ -w -O3 -c context_matcher.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/context_matcher.o
# g++ -w -O3 out/*.o main_allc2ballc.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/allc2ballc.out
# g++ -w -O3 out/*.o main_checkballc.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/checkballc.out
# g++ -w -O3 out/*.o main_indexballc.c -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/indexballc.out
# g++ -w -O3 out/*.o main_queryballc.c -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/queryballc.out
# g++ -w -O3 out/*.o main_queryboth.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts -ldeflate -lbz2 -std=c++11 -lz -o out/queryboth.out
g++ -w -O3 out/*.o main.cc -I /data/wtian/local/miniconda3/envs/sapiens/include -L /data/wtian/local/miniconda3/envs/sapiens/lib -lhts  -ldeflate  -lbz2 -std=c++11 -lz -o out/ballcools
