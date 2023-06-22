#include "ballc_index.h"
#include "utils.h"
// #include <limits>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "timer.h"

BAllCIndex::BAllCIndex(const char* ballc_path):ballc(BAllC(ballc_path)){

    // Timer timer;

    this->ballc_path = ballc_path;
    this->index_path = ballc_path;
    this->index_path += ".bci";

    std::ifstream index_file;
    index_file.open(this->index_path.c_str());
    if(index_file){
        ReadIndex();//TODO
        this->is_indexed = true;
    }

    // std::cout << "Time taken by init ballc in ballc_index" << timer.elapsed() << "s\n";timer.reset();

    InitHeader();
    // std::cout << "Time taken by init header in ballc_index" << timer.elapsed() << "s\n";timer.reset();

}

void BAllCIndex::InitHeader(){
    std::memcpy(this->header.magic,BALLC_INDEX_MAGIC,sizeof(BALLC_INDEX_MAGIC));
    this->header.n_refs = this->ballc.header.n_refs;
    this->header.refs = this->ballc.header.refs;
}

BAllCIndex::~BAllCIndex(){
    // bgzf_close(this->bgzf);
}


int BAllCIndex::RegToBin(int beg, int end){
    --end;
    if (beg >> 14 == end >> 14) return ((1<<15)-1)/7 + (beg>>14);
    if (beg >> 17 == end >> 17) return ((1<<12)-1)/7 + (beg>>17);
    if (beg >> 20 == end >> 20) return ((1<<9)-1)/7 + (beg>>20);
    if (beg >> 23 == end >> 23) return ((1<<6)-1)/7 + (beg>>23);
    if (beg >> 26 == end >> 26) return ((1<<3)-1)/7 + (beg>>26);
    return 0;
}

void BAllCIndex::BuildIndex(){

    MCRecord record;
    uint64_t offset;

    while ((offset = ballc.Tell()) >= 0 && ballc.ReadMcRecord(record)) {
        IndexKey key = {record.ref_id, RegToBin(record.pos, record.pos + 1)};
        this->working_index.AddChunk(key, offset, offset);
    }
    this->is_indexed = true;
}

void BAllCIndex::WriteIndex(bool override){//TODO
    BGZF* index_file = bgzf_open(this->index_path.c_str(), "w9");
    bgzf_write(index_file, &(this->header.magic), sizeof(this->header.magic));
    this->working_index.WriteIndex(index_file);
    bgzf_close(index_file);
}

int BAllCIndex::ReadIndex(){//TODO
    BGZF* index_file = bgzf_open(this->index_path.c_str(), "r");
    bgzf_read(index_file, &(this->header.magic), sizeof(this->header.magic));
    this->working_index.ReadIndex(index_file);
}




void BAllCIndex::ParseGenomeRange(const std::string& range, std::string& chrom, int& start, int& end){
    if (range == "*") {
        chrom = "*";
        start = 0;
        end = GRANGE_END_MAX;
    }
    else{
        std::vector<std::string> elems = utils::split(range, ':');
        chrom = elems[0];

        if (elems.size() == 2) {
            std::vector<std::string> range_elems = utils::split(elems[1], '-');
            start = range_elems[0].empty() ? 0 : std::stoi(range_elems[0]);
            end = (range_elems.size() == 2 && !range_elems[1].empty()) ? std::stoi(range_elems[1]) : GRANGE_END_MAX;
        } else {
            start = 0;
            end = GRANGE_END_MAX;
        }
    }
}

/*------------- legacy code ------------*/ 
// std::vector<MCRecord> BAllCIndex::QueryMcRecords(const std::string& range){
//     Timer timer;
//     std::string chrom;
//     int start, end;
//     ParseGenomeRange(range, chrom, start, end);
//     end++; //to include end position if it's in the file, ie return [start, end] instead of [start, end). this matches the behavior of tabix 

//     if(chrom=="*"){
//         throw std::runtime_error(" chrom * not implemented yet");
//     }
//     int ref_id = this->ballc.ref_dict.get(chrom);

//     IndexKey start_key = {ref_id, RegToBin(start, start + 1)};
//     IndexKey end_key = {ref_id, RegToBin(end, end + 1)};
//     auto start_iter = this->working_index.LowerBound(start_key);
//     auto end_iter = this->working_index.UpperBound(end_key);

//     // std::cout << "\t\tTime taken by ballcindex get ranges: "<< timer.elapsed() << " s" << std::endl;timer.reset();

//     std::vector<MCRecord> results;
//     for (auto iter = start_iter; iter != end_iter; ++iter) {
//         if(iter->key.ref_id!=ref_id){
//             continue;
//         }
//         if (this->ballc.Seek(iter->chunk_start) < 0) {
//             throw std::runtime_error("Error seeking to position in file");
//         }
//         MCRecord record;
//         while (this->ballc.Tell() <= iter->chunk_end && ballc.ReadMcRecord(record)) {
//             if(record.ref_id == ref_id && record.pos >= start && record.pos < end){
//                 results.push_back(record);
//             }
//         }
//     }

//     // std::cout << "\t\tTime taken by ballcindex get records: "<< timer.elapsed() << " s" << std::endl;timer.reset();
    
//     return results;
// }


/*------------- legacy code ------------*/ 
// std::vector<std::string> BAllCIndex::QueryLines(const std::string& range){
//     auto records = QueryMcRecords(range);
//     std::vector<std::string> results;
//     for(auto record: records){
//         results.push_back(this->ballc.McRecordToLine(record));
//     }
//     return results;
// }

/*------------- legacy code ------------*/ 
// std::vector<MCRecord2> BAllCIndex::QueryMcRecord2s(const std::string& range){
//     Timer timer;
//     auto records = QueryMcRecords(range);
//     std::cout << "\tTime taken by ballcindex QueryMcRecords: "<< timer.elapsed() << " s" << std::endl;timer.reset();

//     std::vector<MCRecord2> results;
//     for(auto record: records){
//         results.push_back(this->ballc.McRecordToMcRecord2(record));
//     }

//     std::cout << "\tTime taken by ballcindex McRecordToMcRecord2 conversion: "<< timer.elapsed() << " s" << std::endl;timer.reset();
    
//     return results;
// }


MCRecordIterator BAllCIndex::QueryMcRecords_Iter(const std::string& range){
    std::string chrom;
    int start, end;
    ParseGenomeRange(range, chrom, start, end);
    end++; //to include end position if it's in the file, ie return [start, end] instead of [start, end). this matches the behavior of tabix 

    int ref_id = -1;
    // this->working_index::For
    IndexVec::iterator start_iter, end_iter;
    if(chrom!="*"){
        ref_id = this->ballc.ref_dict.get(chrom);
        IndexKey start_key = {ref_id, RegToBin(start, start + 1)};
        IndexKey end_key = {ref_id, RegToBin(end, end + 1)};
        start_iter = this->working_index.LowerBound(start_key);
        end_iter = this->working_index.UpperBound(end_key);
    }
    else{
        start_iter = this->working_index.Begin();
        end_iter = this->working_index.End();
    }

    return MCRecordIterator(this->ballc, start_iter, end_iter, ref_id, start, end);
}
