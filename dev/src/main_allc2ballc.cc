#include "allc.h"
#include "ballc.h"
// #include "ballc_index.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <htslib/bgzf.h>
#include <cstring>

int main(int argc, char* argv[]){
    Allc allc(argv[1]);
    std::string line;
    BAllc ballc(argv[2], true, "hg38", "haha la", "/gale/netapp/home/wtian/refs/human/hg38/genome/hg38.chrom_chrL.sizes", 'w');
    ballc.WriteHeader();

    int count=0;
    while(true){
        line = allc.ReadLine();
        if(line==""){
            break;
        }

        MCRecord rec = ballc.AllcLineToMcRecord(line);
        if ((count++)%5000000==0){
            std::cout << "count " << count << " :: " <<  ballc.McRecordToLine(rec);
        }
        ballc.WriteMcRecord(rec);
    }
    std::cout << "total " << count << std::endl;

}