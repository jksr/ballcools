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
#include "context_matcher.h"

namespace routine{

void AllCToBallC(const char* allc_path, const char* ballc_path, std::string chrom_size_path, 
                std::string assembly_text, std::string header_text="", bool sc=true){

    AllC allc(allc_path);

    BAllC ballc(ballc_path, sc, assembly_text, header_text, chrom_size_path, 'w');
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
    BAllCIndex index(ballc_path);
    index.BuildIndex();
    index.WriteIndex();
}

void ExtractAllCMeta(const char* fasta_path, const char* cmeta_path){
    ExtractCMeta(fasta_path, cmeta_path);
    IndexCMeta(cmeta_path);
}

/*------------- legacy code ------------*/ 
// void QueryBallc(const char* ballc_path, const char* range){
//     BAllCIndex index(ballc_path);
//     auto records = index.QueryLines(range);
//     for(auto it: records){
//         std::cout << it;
//     }
// }

void QueryBallc_Iter(const char* ballc_path, const char* range){
    BAllCIndex index(ballc_path);
    auto mciter = index.QueryMcRecords_Iter(range);
    MCRecord2 rec;
    while(mciter.HasNext()){
        rec = mciter.Next();
        std::cout << rec.chrom << "\t" << rec.pos << "\t" << rec.mc << "\t" << rec.cov << std::endl;
    }
}

/*------------- legacy code ------------*/ 
// void QueryBallcWithMeta(const char* ballc_path, const char* cmeta_path, const char* range, 
//                         bool warn_mismatch=true, bool err_mismatch=true, bool skip_mismatch=true){
//     BAllCIndex index(ballc_path);
//     auto records = index.QueryMcRecord2s(range);

//     if(records.size()>0){
//         std::string good_range = records.front().chrom;
//         good_range+=":";
//         good_range+=std::to_string(records.front().pos);
//         good_range+="-";
//         good_range+=std::to_string(records.back().pos);


//         htsFile *fp = hts_open(cmeta_path, "r");
//         tbx_t *tbx = tbx_index_load(cmeta_path);
//         kstring_t str = {0, 0, nullptr};

//         hts_itr_t *itr = tbx_itr_querys(tbx, good_range.c_str());
//         if (itr == NULL) {
//             throw std::runtime_error("Failed to query region.");
//         }
//         std::unordered_map<std::pair<std::string, int>, std::pair<std::string, std::string>, utils::pair_hash> meta_map;
//         while (tbx_itr_next(fp, tbx, itr, &str) >= 0) {
//             auto elems = utils::split(str.s, '\t');
//             meta_map[std::pair<std::string, int>(elems[0], stoi(elems[1]))] = std::pair<std::string, std::string>(elems[2],elems[3]);
//         }
//         free(str.s);
//         tbx_itr_destroy(itr);
//         tbx_destroy(tbx);
//         hts_close(fp);

//         for(auto record:records){
//             auto meta_ptr = meta_map.find(std::pair<std::string, int>(record.chrom, record.pos));
//             if(meta_ptr==meta_map.end()){
//                 if(warn_mismatch){
//                     std::cerr << "Mismatch detected at " << record.chrom << "\t" << record.pos << 
//                     " between ballc file and meta file.\n";
//                 }
//                 if(err_mismatch){
//                     throw std::runtime_error("Ballc file and meta file do not match!");
//                 }
//                 if(!skip_mismatch){
//                     std::cout << record.chrom << "\t" << record.pos << "\t"
//                         << "?" << "\t" << "C??" << "\t"
//                         << record.mc << "\t" << record.cov << "\t1\n";                    
//                 }
//             }
//             else{
//                 std::cout << record.chrom << "\t" << record.pos << "\t"
//                     << meta_ptr->second.first << "\t" << meta_ptr->second.second << "\t"
//                     << record.mc << "\t" << record.cov << "\t1\n";
//             }
//         }
//     }
// }


inline void OutputMatched(std::string chrom, uint32_t pos, std::string strand,
                            std::string context, uint16_t mc, uint16_t cov, 
                            const CContextMatcher& matcher
                         ){
    if(matcher.Matches(context)){
        std::cout << chrom << "\t" << pos << "\t" << strand << "\t" << context 
                << "\t" << mc << "\t" << cov << "\t1" << std::endl;
    }
}

void QueryBallcWithMeta_Iter(const char* ballc_path, const char* cmeta_path, const char* range, 
                        bool warn_mismatch=true, bool err_mismatch=true, bool skip_mismatch=true,
                        std::string c_context="*"){
    BAllCIndex index(ballc_path);
    MCRecordIterator mciter = index.QueryMcRecords_Iter(range);
    CContextMatcher context_matcher(c_context);

    htsFile *fp = hts_open(cmeta_path, "r");
    tbx_t *tbx = tbx_index_load(cmeta_path);
    kstring_t str = {0, 0, nullptr};
    
    // std::string strand, mc_ctxt;
    while(mciter.HasNext()){
        MCRecord2 record2 = mciter.Next();
        std::string range = record2.chrom+":"+std::to_string(record2.pos)+"-"+std::to_string(record2.pos);
        hts_itr_t *itr = tbx_itr_querys(tbx, range.c_str());
        if (itr == NULL) {
            // throw std::runtime_error("Failed to query region.");//TODO

            if(warn_mismatch){
                std::cerr << "Mismatch detected at " << record2.chrom << "\t" << record2.pos << 
                " between ballc file and meta file.\n";
            }
            if(err_mismatch){
                throw std::runtime_error("Ballc file and meta file do not match!");
            }
            if(!skip_mismatch){
                OutputMatched(record2.chrom, record2.pos, "?", "C??", record2.mc, record2.cov, context_matcher);
                // std::cout << record2.chrom << "\t" << record2.pos << "\t"
                //     << "?" << "\t" << "C??" << "\t"
                //     << record2.mc << "\t" << record2.cov << "\t1\n";                    
            }            
        }
        else{
            tbx_itr_next(fp, tbx, itr, &str);
            auto elems = utils::split(str.s, '\t');
            tbx_itr_destroy(itr);
            OutputMatched(record2.chrom, record2.pos, elems[2], elems[3], record2.mc, record2.cov, context_matcher);

            // std::cout << record2.chrom << "\t" << record2.pos << "\t"
            //     << elems[2] << "\t" << elems[3] << "\t"
            //     << record2.mc << "\t" << record2.cov << "\t1\n";
        }
    }

    free(str.s);
    tbx_destroy(tbx);
    hts_close(fp);
}

void ViewBallc(const char* ballc_path, bool header, bool refs, bool records, const char* cmeta_path=""){
    BAllC ballc(ballc_path, 'r');
    if(header){
        std::cout << "BAllC file: " << ballc_path << "\n";
        std::cout << "Assembly: " << ballc.header.assembly_text << "\n";
        std::cout << "Note: " << ballc.header.header_text << "\n";
    }
    if(refs){
        std::cout << "References [length]: ";
        for(int i=0; i<ballc.header.refs.size()-1; i++){
            std::cout << ballc.header.refs[i].ref_name << " [" << ballc.header.refs[i].ref_length << "],";
        }
        std::cout << ballc.header.refs.back().ref_name << " [" << ballc.header.refs.back().ref_length << "]\n";
    }
    if(records){
        //TODO
    }
}

void CheckBallc(const char* ballc_path){
    BAllC ballc(ballc_path);
    int count = 0;
    MCRecord rec;
    while(ballc.ReadMcRecord(rec)){
        if ((count++)%5000000==0){
            std::cout << "count " << count << " :: " <<  ballc.McRecordToLine(rec);
        }
    }
    std::cout << "total " << count << std::endl;

}


}

#endif//_ROUTINES_H_
