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
    index.WriteIndex();
}