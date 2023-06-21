#ifndef _ROUTINES_H_
#define _ROUTINES_H_

#include <string>
#include <htslib/hts.h>
#include <htslib/tbx.h>
#include <htslib/kseq.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <string>
#include "allc.h"
#include "ballc.h"
#include "meta_indexing.h"
#include "ballc_index.h"
#include "utils.h"

void AllcToBallc(const char* allc_path, const char* ballc_path, std::string chrom_size_path, 
                std::string assembly_text, std::string header_text="", bool sc=true){
    Allc allc(allc_path);
    BAllc ballc(ballc_path, sc, assembly_text, header_text, chrom_size_path, 'w');
    std::string line;
    ballc.WriteHeader();

    while(true){
        line = allc.ReadLine();
        if(line==""){
            break;
        }
        MCRecord rec = ballc.AllcLineToMcRecord(line);
        ballc.WriteMcRecord(rec);
    }
}

void IndexBallc(const char* ballc_path){
    BAllcIndex index(ballc_path);
    index.BuildIndex();
    index.WriteIndex();
}

void ExtractAllCMeta(const char* fasta_path, const char* cmeta_path){
    ExtractCMeta(fasta_path, cmeta_path);
    IndexCMeta(cmeta_path);
}

void QueryBallc(const char* ballc_path, const char* range){
    BAllcIndex index(ballc_path);
    auto records = index.QueryLines(range);
    for(auto it: records){
        std::cout << it;
    }
}

void IterQueryBallc(const char* ballc_path, const char* range){
    BAllcIndex index(ballc_path);
    auto records = index.IterQueryLines(range);
    for(auto it: records){
        std::cout << it;
    }
}

void QueryBallcWithMeta(const char* ballc_path, const char* cmeta_path, const char* range, 
                        bool warn_mismatch=true, bool err_mismatch=true, bool skip_mismatch=true){
    BAllcIndex index(ballc_path);
    auto records = index.QueryMcRecord2s(range);

    if(records.size()>0){
        std::string good_range = records.front().chrom;
        good_range+=":";
        good_range+=std::to_string(records.front().pos);
        good_range+="-";
        good_range+=std::to_string(records.back().pos);


        htsFile *fp = hts_open(cmeta_path, "r");
        tbx_t *tbx = tbx_index_load(cmeta_path);
        kstring_t str = {0, 0, nullptr};

        hts_itr_t *itr = tbx_itr_querys(tbx, good_range.c_str());
        if (itr == NULL) {
            throw std::runtime_error("Failed to query region.");
        }
        std::unordered_map<std::pair<std::string, int>, std::pair<std::string, std::string>, utils::pair_hash> meta_map;
        while (tbx_itr_next(fp, tbx, itr, &str) >= 0) {
            auto elems = utils::split(str.s, '\t');
            meta_map[std::pair<std::string, int>(elems[0], stoi(elems[1]))] = std::pair<std::string, std::string>(elems[2],elems[3]);
        }
        free(str.s);
        tbx_itr_destroy(itr);
        tbx_destroy(tbx);
        hts_close(fp);

        for(auto record:records){
            auto meta_ptr = meta_map.find(std::pair<std::string, int>(record.chrom, record.pos));
            if(meta_ptr==meta_map.end()){
                if(warn_mismatch){
                    std::cerr << "Mismatch detected at " << record.chrom << "\t" << record.pos << 
                    " between ballc file and meta file.\n";
                }
                if(err_mismatch){
                    throw std::runtime_error("Ballc file and meta file do not match!");
                }
                if(!skip_mismatch){
                    std::cout << record.chrom << "\t" << record.pos << "\t"
                        << "?" << "\t" << "C??" << "\t"
                        << record.mc << "\t" << record.cov << "\t1\n";                    
                }
            }
            else{
                std::cout << record.chrom << "\t" << record.pos << "\t"
                    << meta_ptr->second.first << "\t" << meta_ptr->second.second << "\t"
                    << record.mc << "\t" << record.cov << "\t1\n";
            }
        }
    }
}

void ViewBallc(const char* ballc_path, bool header, bool refs, bool records, const char* cmeta_path=""){
    BAllc ballc(ballc_path, 'r');
    if(header){
        std::cout << "BAllc file: " << ballc_path << "\n";
        std::cout << "Assembly: " << ballc.header.assembly_text << "\n";
        std::cout << "Note: " << ballc.header.header_text << "\n";
    }
    if(refs){
        std::cout << "References: ";
        for(int i=0; i<ballc.header.refs.size()-1; i++){
            std::cout << ballc.header.refs[i].ref_name << ",";
        }
        std::cout << ballc.header.refs.back().ref_name << "\n";
    }
    if(records){
        //TODO
    }
}

#endif//_ROUTINES_H_
