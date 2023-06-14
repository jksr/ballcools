#include "allc.h"
#include "ballc.h"
#include "ballc_index.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <htslib/bgzf.h>
#include <cstring>

int main(int argc, char* argv[]){
    BAllcIndex index(argv[1]);
    index.BuildIndex();
    // auto records = index.QueryLines(std::string(argv[2]));
    auto records = index.QueryLines(argv[2]);
    for(auto it: records){
        std::cout << it;
    }
}