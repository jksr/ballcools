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

    std::cout << "Writing BAllC header to " << std::string(ballc_path) << std::endl;
    ballc.WriteHeader();

    std::cout << "Converting AllC to BAllC" << std::endl;
    while(true){
        line = allc.ReadLine();
        if(line==""){
            break;
        }
        MCRecord rec = ballc.AllcLineToMcRecord(line);
        ballc.WriteMcRecord(rec);
    }
    std::cout << "Converting AllC to BAllC finished" << std::endl;
}

void IndexBallc(const char* ballc_path){
    std::cout << "Building index for " << std::string(ballc_path) << std::endl;
    BAllCIndex index(ballc_path);
    index.BuildIndex();
    std::cout << "Writing the index file " << std::string(ballc_path) << ".bci" << std::endl;
    index.WriteIndex();
    std::cout << "Indexing "<< std::string(ballc_path) << " finished" << std::endl;
}

void ExtractAllCMeta(const char* fasta_path, const char* cmeta_path){
    std::cout << "Extracting meta of all Cytosines from  " << std::string(fasta_path) 
            << " to " << std::string(cmeta_path) << ". This step may take a while" << std::endl;
    ExtractCMeta(fasta_path, cmeta_path);
    std::cout << "indexing  " << std::string(cmeta_path)  << std::endl;
    IndexCMeta(cmeta_path);
    std::cout << "Extracting and indexing all Cytosines finished." << std::endl;
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
                            const CContextMatcher& matcher, std::ostream& os = std::cout
                         ){
    if(matcher.Matches(context)){
        os << chrom << "\t" << pos << "\t" << strand << "\t" << context 
                << "\t" << mc << "\t" << cov << "\t1" << std::endl;
    }
}

void QueryBallcWithMeta_Iter(const char* ballc_path, const char* cmeta_path, const char* range, 
                        bool warn_mismatch=true, bool err_mismatch=true, bool skip_mismatch=true,
                        std::string c_context="*", std::ostream& os=std::cout){
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
                OutputMatched(record2.chrom, record2.pos, "?", "C??", record2.mc, record2.cov, context_matcher, os);
                // std::cout << record2.chrom << "\t" << record2.pos << "\t"
                //     << "?" << "\t" << "C??" << "\t"
                //     << record2.mc << "\t" << record2.cov << "\t1\n";                    
            }            
        }
        else{
            tbx_itr_next(fp, tbx, itr, &str);
            auto elems = utils::split(str.s, '\t');
            tbx_itr_destroy(itr);
            OutputMatched(record2.chrom, record2.pos, elems[2], elems[3], record2.mc, record2.cov, context_matcher, os);

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



void MergeBAllC(const std::vector<std::string> & in_ballc_paths,   const std::string& out_ballc_path, int chunksize=5000000){
    // vector to store the input file streams
    std::vector<BAllCIndex> bcis;
    for(int i = 0; i < in_ballc_paths.size(); i++){
        BAllCIndex bci(in_ballc_paths[i].c_str());
        bcis.push_back(std::move(bci));
    }
    BAllC ballc(in_ballc_paths[0].c_str());
    BAllC out(out_ballc_path.c_str(), false, ballc.header.assembly_text, "", ballc.header.refs, 'w');
    out.WriteHeader();

    //////////////////////////
    for(auto ref:ballc.header.refs){
        // std::cout << "Merging " << ref.ref_name << std::endl;
        for(int s=0; s<ref.ref_length; s+=chunksize){
            // std::cout << "Merging " << ref.ref_name << "\t" << s << " " << s+chunksize << std::endl;
            std::vector<std::vector<MCRecord2>> rec_vectors(bcis.size());
            for (size_t i = 0; i < bcis.size(); ++i) {
                auto mciter = bcis[i].QueryMcRecords_Iter(ref.ref_name, s, s+chunksize-1);
                while(mciter.HasNext()){
                    rec_vectors[i].push_back(mciter.Next());
                }
            }
            
            std::unordered_map<uint32_t, std::pair<uint16_t,uint16_t>> dict;
            for(auto vec:rec_vectors){
                for(auto rec:vec){
                    if(dict.find(rec.pos)==dict.end()){
                        dict[rec.pos] = {rec.mc, rec.cov};
                    }
                    else{
                        dict[rec.pos].first += rec.mc;
                        dict[rec.pos].second += rec.cov;
                    }
                }
            }

            std::map<uint32_t, std::pair<uint16_t,uint16_t>> ordered(dict.begin(), dict.end());
            MCRecord outrec;
            for(auto it = ordered.begin(); it != ordered.end(); ++it){
                outrec.ref_id = out.ref_dict.at(ref.ref_name);
                // outrec.ref_id = out.ref_dict.get(ref.ref_name);
                outrec.pos = it->first;
                outrec.mc = it->second.first;
                outrec.cov = it->second.second;
                out.WriteMcRecord(outrec);
            }
        }
    }
    out.Close();
    std::cout << "Merging finished." << std::endl;
}

void MergeBAllC(const std::string& file_of_paths,  const std::string& out_ballc_path, int chunksize=5000000){
    std::ifstream fin(file_of_paths);
    if(!fin){
        throw std::runtime_error("Cannot open "+file_of_paths+".");
    }
    std::string line;
    std::vector<std::string> in_ballc_paths;
    while (std::getline(fin, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());//strip whitespace
        in_ballc_paths.push_back(line);
    }
    fin.close();
    
    MergeBAllC(in_ballc_paths, out_ballc_path, chunksize);
}



void BAllCToAllC(const char* ballc_path, const char* cmeta_path, const char* allc_path){
    // QueryBallcWithMeta_Iter(ballc_path, cmeta_path, range, 
    //                     bool warn_mismatch=true, bool err_mismatch=true, bool skip_mismatch=true,
    //                     std::string c_context="*", std::ostream& os)

    //
}





}

#endif//_ROUTINES_H_
