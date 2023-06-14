#include "meta_indexing.cc"

int main(int argc, char* argv[]){
    ExtractCMeta(argv[1],argv[2]);
    IndexCMeta(argv[2]);
}