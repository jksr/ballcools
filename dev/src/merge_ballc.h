#ifndef _MERGE_BALLC_H_
#define _MERGE_BALLC_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <thread>
#include "ballc_files.h"
#include "ballc_index.h"

#include <iostream>

void LoadChunk(BAllCIndex& bci, std::vector<MCRecord2>& results, std::string chrom, int start, int end){
    auto iter = bci.QueryMcRecords_Iter(chrom, start, end);
    while(iter.HasNext()){
        results.push_back(iter.Next());
    }
}


void merge_files(const std::vector<std::string> & in_ballc_paths,   const std::string& out_ballc_path, int chunksize=5000000) {

    // vector to store the input file streams
    std::vector<BAllCIndex> bcis;//(in_ballc_paths.size());
    for(int i = 0; i < in_ballc_paths.size(); i++){
        BAllCIndex bci(in_ballc_paths[i].c_str());
        bcis.push_back(std::move(bci));
    }

    BAllC out(out_ballc_path.c_str(), false, "test", "", "/gale/netapp/home/wtian/scratch2/human_brain_atlas/custom-genome-with_snp/h1930001/h1930001.fa.fai", 'w');
    out.WriteHeader();

    //////////////////////////
    for(auto ref:bcis.front().header.refs){
        std::cout << ref.ref_name << std::endl;
        for(int s=0; s<ref.ref_length; s+=chunksize){
            std::cout << "\t" << s << " " << s+chunksize << std::endl;
            std::vector<std::vector<MCRecord2>> rec_vectors(bcis.size());
            for (size_t i = 0; i < bcis.size(); ++i) {  
                LoadChunk(bcis[i],rec_vectors[i], ref.ref_name, s, s+chunksize);
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
                outrec.ref_id = out.ref_dict.get(ref.ref_name);
                outrec.pos = it->first;
                outrec.mc = it->second.first;
                outrec.cov = it->second.second;
                out.WriteMcRecord(outrec);
            }
        }
    }
    //////////////////////////








    // std::vector<std::vector<MCRecord2>> rec_vectors(bcis.size());
    // std::vector<std::thread> threads;

    // // // Launch threads
    // // for (size_t i = 0; i < bcis.size(); ++i) {
    // //     threads.emplace_back(std::thread(LoadChunk, std::ref(bcis[i]), std::ref(rec_vectors[i]), 
    // //                             std::string("chr1"), 0, 500000));
    // // }
    // // // Wait for threads to finish and process results
    // // for (size_t i = 0; i < threads.size(); ++i) {
    // //     threads[i].join();
    // // }
    // for (size_t i = 0; i < bcis.size(); ++i) {  
    //     LoadChunk(bcis[i],rec_vectors[i], std::string("chr1"), 0, 500000);
    // }
    // std::cout << "lala" << std::endl;
    
    // std::unordered_map<uint32_t, std::pair<uint16_t,uint16_t>> dict;
    // for(auto vec:rec_vectors){
    //     for(auto rec:vec){
    //         if(dict.find(rec.pos)==dict.end()){
    //             dict[rec.pos] = {rec.mc, rec.cov};
    //         }
    //         else{
    //             dict[rec.pos].first += rec.mc;
    //             dict[rec.pos].second += rec.cov;
    //         }
    //     }
    // }

    // std::map<uint32_t, std::pair<uint16_t,uint16_t>> ordered(dict.begin(), dict.end());
    // std::cout << ordered.size() << std::endl;
    // for(auto it = ordered.begin(); it != ordered.end(); ++it){
    //     if(it->first%7==1){
    //     std::cout << "chr1\t" << it->first << "\t" << it->second.first << "\t" << it->second.second << std::endl;
    //     }
    // }
}


#endif//_MERGE_BALLC_H_











