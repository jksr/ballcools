#include "allc.h"
#include "ballc.h"
#include "ballc_index.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <htslib/bgzf.h>
#include <cstring>

int main(int argc, char* argv[]){

    BAllc ballc(argv[1]);
    int count = 0;
    MCRecord rec;
    while(ballc.ReadMcRecord(rec)){
        if ((count++)%5000000==0){
            std::cout << "count " << count << " :: " <<  ballc.McRecordToLine(rec);
        }
    }
    std::cout << "total " << count << std::endl;

}