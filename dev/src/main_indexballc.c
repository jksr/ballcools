#include "allc.h"
#include "ballc.h"
#include "ballc_index.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <htslib/bgzf.h>
#include <cstring>

int main(int argc, char* argv[]){

    std::cout << "here" << std::endl;
    BAllcIndex index(argv[1]);
    std::cout << "here" << std::endl;
    index.BuildIndex();
    std::cout << "here" << std::endl;
    index.WriteIndex();
    std::cout << "here" << std::endl;
}