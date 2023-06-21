#include "ballc_index.h"
#include "utils.h"
// #include <limits>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "timer.h"

BAllcIndex::BAllcIndex(const char* ballc_path):ballc(BAllc(ballc_path)){

    // Timer timer;

    this->ballc_path = ballc_path;
    this->index_path = ballc_path;
    this->index_path += ".bci";

    std::ifstream index_file;
    index_file.open(this->index_path.c_str());
    if(index_file){
        ReadIndex();//TODO
        is_indexed = true;
    }

    // std::cout << "Time taken by init ballc in ballc_index" << timer.elapsed() << "s\n";timer.reset();

    InitHeader();
    // std::cout << "Time taken by init header in ballc_index" << timer.elapsed() << "s\n";timer.reset();

}

void BAllcIndex::InitHeader(){
    std::memcpy(this->header.magic,BALLC_INDEX_MAGIC,sizeof(BALLC_INDEX_MAGIC));
    this->header.n_refs = this->ballc.header.n_refs;
    this->header.refs = this->ballc.header.refs;
}

BAllcIndex::~BAllcIndex(){
    // bgzf_close(this->bgzf);
}


int BAllcIndex::RegToBin(int beg, int end){
    --end;
    if (beg >> 14 == end >> 14) return ((1<<15)-1)/7 + (beg>>14);
    if (beg >> 17 == end >> 17) return ((1<<12)-1)/7 + (beg>>17);
    if (beg >> 20 == end >> 20) return ((1<<9)-1)/7 + (beg>>20);
    if (beg >> 23 == end >> 23) return ((1<<6)-1)/7 + (beg>>23);
    if (beg >> 26 == end >> 26) return ((1<<3)-1)/7 + (beg>>26);
    return 0;
}

void BAllcIndex::BuildIndex(){

    MCRecord record;
    uint64_t offset;

    while ((offset = ballc.Tell()) >= 0 && ballc.ReadMcRecord(record)) {
        IndexKey key = {record.ref_id, RegToBin(record.pos, record.pos + 1)};
        // if bin does not exist in index, initialize it
        // this->working_index.AddChunk(key, {offset, offset});
        this->working_index.AddChunk(key, offset, offset);
        // if(this->working_index[key].empty()){
        //     this->working_index[key] = {{offset, offset}};
        // } 
        // else{
        //     // otherwise, extend the last chunk
        //     this->working_index[key].back().second = offset;
        // }
    }

    // for(auto x: this->working_index.ref_id_book){
    //             std::cout << "ha" << x.first << " | " << x.second.first  << " | " << x.second.second <<"\n";
    //         }

    // for(auto x: this->working_index.index_vec){
    //     std::cout << "key: " << x.key.ref_id << " : " << x.key.bin << " | " << x.chunks.size() << "\n";
    // }
    // std::cout << "index built"<< std::endl;
    // WorkingIndexToFileIndex();//TODO
}


// void BAllcIndex::WorkingIndexToFileIndex(){//TODO
//     this->index.clear();
//     IndexKey index_key;
//     //std::vector<std::pair<int64_t, int64_t>>& offset_pair_list;
//     for(const auto& it : this->working_index){
//         index_key = it.first;
//         // offset_pair_list = it.second;
//         IndexValues index_values = {it.second.size(), it.second};
//         this->index.push_back(std::pair<IndexKey, IndexValues>(index_key, index_values));
//     }
//     this->n_indexes = this->index.size();
// }

// void BAllcIndex::FileIndexToWorkingIndex(){//TODO
//     this->working_index.clear();
//     for(const auto& it : this->index){
//         this->working_index[it.first] = it.second.offset_pairs;
//     }
// }

void BAllcIndex::WriteIndex(bool override){//TODO
    BGZF* index_file = bgzf_open(this->index_path.c_str(), "w9");
    bgzf_write(index_file, &(this->header.magic), sizeof(this->header.magic));
    this->working_index.WriteIndex(index_file);
    // bgzf_write(index_file, &(this->header.n_refs), sizeof(this->header.n_refs));
    // for(const auto& it : this->header.refs){
    //     bgzf_write(index_file, &it.l_name, sizeof(it.l_name));
    //     bgzf_write(index_file, it.ref_name.c_str(), it.l_name);
    // }

    // bgzf_write(index_file, &(this->n_indexes), sizeof(this->n_indexes));
    // for(const auto& it : this->index){
    //     bgzf_write(index_file, &it.first.ref_id, sizeof(it.first.ref_id));
    //     bgzf_write(index_file, &it.first.bin, sizeof(it.first.bin));
    //     bgzf_write(index_file, &it.second.n_pairs, sizeof(it.second.n_pairs));
    //     for(const auto& jt : it.second.offset_pairs){
    //         bgzf_write(index_file, &jt.first, sizeof(jt.first));
    //         bgzf_write(index_file, &jt.second, sizeof(jt.second));
    //     }
    // }
    bgzf_close(index_file);
}

int BAllcIndex::ReadIndex(){//TODO
    BGZF* index_file = bgzf_open(this->index_path.c_str(), "r");
    bgzf_read(index_file, &(this->header.magic), sizeof(this->header.magic));
    this->working_index.ReadIndex(index_file);
    // bgzf_read(index_file, &(this->header.n_refs), sizeof(this->header.n_refs));
    // this->header.refs.clear();

    // uint32_t l_name;
    // std::string ref_name;
    // char* tmp = new char[2024];
    // for(int i = 0; i < this->header.n_refs; i++){
    //     bgzf_read(index_file, &l_name, sizeof(l_name));
    //     bgzf_read(index_file, tmp, l_name);
    //     this->header.refs.push_back(RefRecord({l_name, std::string(tmp)}));
    // }

    // bgzf_read(index_file, &(this->n_indexes), sizeof(this->n_indexes));
    // IndexKey index_key;
    // IndexValues index_values;
    // int64_t offset1, offset2;
    // for(int i = 0; i < this->n_indexes; i++){
    //     bgzf_read(index_file, &index_key.ref_id, sizeof(index_key.ref_id));
    //     bgzf_read(index_file, &index_key.bin, sizeof(index_key.bin));
    //     bgzf_read(index_file, &index_values.n_pairs, sizeof(index_values.n_pairs));

    //     index_values.offset_pairs.clear();
    //     for(int j = 0; j < index_values.n_pairs; j++){
    //         bgzf_read(index_file, &offset1, sizeof(offset1));
    //         bgzf_read(index_file, &offset2, sizeof(offset2));
    //         index_values.offset_pairs.push_back(std::pair<int64_t, int64_t>(offset1, offset2));
    //     }

    //     this->index.push_back(std::pair<IndexKey,IndexValues>(index_key,index_values));
    // }
    // // delete tmp;
    // bgzf_close(index_file);
    // FileIndexToWorkingIndex();
    // for(auto x: this->working_index.index_vec){
    //     std::cout << "key: " << x.key.ref_id << " : " << x.key.bin << " | " << x.chunks.size() << "\n";
    // }
    // std::cout << "index read"<< std::endl;

}




void BAllcIndex::ParseGenomeRange(const std::string& range, std::string& chrom, int& start, int& end){
    if (range == "*") {
        chrom = "*";
        start = 0;
        // end = std::numeric_limits<int>::max();
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
            // end = std::numeric_limits<int>::max();
            end = GRANGE_END_MAX;
        }
    }
    std::cout<<chrom<<" "<<start<<" "<<end<<std::endl;
}

// void BAllcIndex::BuildRefDict(){
    // // HashTable* ref_dict = new HashTable();
    // for(unsigned int i = 0; i<header.n_refs; i++){
    //     this->ref_dict.insert(this->header.refs[i].ref_name, i);
    // }
    // // this->ref_dict = ref_dict;
    // // return ref_dict;
// }
std::vector<MCRecord> BAllcIndex::IterQueryMcRecords(const std::string& range){
// MCRecordIterator BAllcIndex::IterQueryMcRecords(const std::string& range){
    std::string chrom;
    int start, end;
    ParseGenomeRange(range, chrom, start, end);
    end++; //to include end position if it's in the file, ie return [start, end] instead of [start, end). this matches the behavior of tabix 

    int ref_id = this->ballc.ref_dict.get(chrom);
    IndexKey start_key = {ref_id, RegToBin(start, start + 1)};
    IndexKey end_key = {ref_id, RegToBin(end, end + 1)};
    auto start_iter = this->working_index.LowerBound(start_key);
    auto end_iter = this->working_index.UpperBound(end_key);

    MCRecordIterator mciter(this->ballc, start_iter, end_iter, ref_id, start, end);

    std::vector<MCRecord> results;
    while(mciter.HasNext()){
        results.push_back(mciter.Next());
    }
    return results;
}


std::vector<MCRecord> BAllcIndex::QueryMcRecords(const std::string& range){
    Timer timer;
    std::string chrom;
    int start, end;
    ParseGenomeRange(range, chrom, start, end);
    end++; //to include end position if it's in the file, ie return [start, end] instead of [start, end). this matches the behavior of tabix 

    int ref_id = this->ballc.ref_dict.get(chrom);
    std::cout << "ref_id " << ref_id << std::endl;

    IndexKey start_key = {ref_id, RegToBin(start, start + 1)};
    IndexKey end_key = {ref_id, RegToBin(end, end + 1)};
    auto start_iter = this->working_index.LowerBound(start_key);
    auto end_iter = this->working_index.UpperBound(end_key);
    // if (start_iter == this->working_index.end() || start_iter->first.ref_id != ref_id) {
    //     throw std::runtime_error("Start position not found in index1");
    // }
    // if (end_iter == this->working_index.end() || end_iter->first.ref_id != ref_id) {
    //     // throw std::runtime_error("End position not found in index2");
    //     end_iter = this->working_index.end();
    // }

    std::cout << "\t\tTime taken by ballcindex get ranges: "<< timer.elapsed() << " s" << std::endl;timer.reset();

    std::vector<MCRecord> results;
    // std::vector<std::string> results;
    for (auto iter = start_iter; iter != end_iter; ++iter) {
        if(iter->key.ref_id!=ref_id){
            continue;
        }
        // for(auto chunk: iter->chunks){
            // if (this->ballc.Seek(chunk.first) < 0) {
            if (this->ballc.Seek(iter->chunk_start) < 0) {
                throw std::runtime_error("Error seeking to position in file");
            }
            MCRecord record;
            // while (this->ballc.Tell() <= chunk.second && ballc.ReadMcRecord(record)) {
            while (this->ballc.Tell() <= iter->chunk_end && ballc.ReadMcRecord(record)) {
                if(record.ref_id == ref_id && record.pos >= start && record.pos < end){
                    results.push_back(record);
                }
            }
        // }
    }

    std::cout << "\t\tTime taken by ballcindex get records: "<< timer.elapsed() << " s" << std::endl;timer.reset();
    
    return results;
}


std::vector<std::string> BAllcIndex::QueryLines(const std::string& range){
    auto records = QueryMcRecords(range);
    std::vector<std::string> results;
    for(auto record: records){
        results.push_back(this->ballc.McRecordToLine(record));
    }
    return results;
}

std::vector<std::string> BAllcIndex::IterQueryLines(const std::string& range){
    auto records = IterQueryMcRecords(range);
    std::vector<std::string> results;
    for(auto record: records){
        results.push_back(this->ballc.McRecordToLine(record));
    }
    return results;
}

std::vector<MCRecord2> BAllcIndex::QueryMcRecord2s(const std::string& range){
    Timer timer;
    auto records = QueryMcRecords(range);
    std::cout << "\tTime taken by ballcindex QueryMcRecords: "<< timer.elapsed() << " s" << std::endl;timer.reset();

    std::vector<MCRecord2> results;
    for(auto record: records){
        results.push_back(this->ballc.McRecordToMcRecord2(record));
    }

    std::cout << "\tTime taken by ballcindex McRecordToMcRecord2 conversion: "<< timer.elapsed() << " s" << std::endl;timer.reset();
    
    return results;
}

// void BAllcIndex::QueryMcRecord2s(const std::string& range, std::vector<MCRecord2>& results){
//     Timer timer;
//     auto records = QueryMcRecords(range);
//     std::cout << "\tTime taken by ballcindex QueryMcRecords: "<< timer.elapsed() << " s" << std::endl;timer.reset();

//     for(auto record: records){
//         results.push_back(this->ballc.McRecordToMcRecord2(record));
//     }

//     std::cout << "\tTime taken by ballcindex McRecordToMcRecord2 conversion: "<< timer.elapsed() << " s" << std::endl;timer.reset();
    
// }



// struct index_key {
//     int ref_id;
//     int bin;

//     bool operator<(const index_key &other) const {
//         if (ref_id == other.ref_id) {
//             return bin < other.bin;
//         }
//         return ref_id < other.ref_id;
//     }
// };

// Function to calculate bin number for a given region [beg, end)
// Following the concept from Tabix.


// // Bin size of 16KB, inspired by Tabix linear index
// constexpr int bin_size = 16384;

// typedef std::map<index_key, std::vector<std::pair<int64_t, int64_t>>> Index;

// // Build the index
// Index build_index(const char* filename) {
//     Index index;
//     BGZF* bgzf = bgzf_open(filename, "r");
//     // BGZF *bgzf = bgzf_open(filePath.c_str(), "r");
    
//     if (bgzf == NULL) {
//         std::cerr << "Error opening file" << std::endl;
//         return index;
//     }

//     BAllc ballc(bgzf);

//     MCRecord record;
//     int64_t offset;

//     while ((offset = bgzf_tell(bgzf)) >= 0 && ballc.ReadMcRecord(record, bgzf)) {
//         index_key key = {record.ref_id, reg2bin(record.pos, record.pos + 1)};
//         // if bin does not exist in index, initialize it
//         if(index[key].empty()){
//             index[key] = {{offset, offset}};
//         } 
//         else{
//             // otherwise, extend the last chunk
//             index[key].back().second = offset;
//         }
//     }

//     bgzf_close(bgzf);
//     return index;
// }

// // Query using the index
// std::vector<MCRecord> query(const Index& index, const char* filename, int ref_id, int start, int end) {
//     index_key start_key = {ref_id, reg2bin(start, start + 1)};
//     index_key end_key = {ref_id, reg2bin(end, end + 1)};
//     auto start_iter = index.lower_bound(start_key);
//     auto end_iter = index.upper_bound(end_key);
//     if (start_iter == index.end() || start_iter->first.ref_id != ref_id) {
//         throw std::runtime_error("Start position not found in index");
//     }
//     if (end_iter == index.end() || end_iter->first.ref_id != ref_id) {
//         throw std::runtime_error("End position not found in index");
//     }

//     BGZF* fp = bgzf_open(filename, "r");
//     if (fp == NULL) {
//         throw std::runtime_error("Error opening file");
//     }
//     BAllc ballc(fp);

//     std::vector<MCRecord> results;
//     for (auto iter = start_iter; iter != end_iter; ++iter) {
//         for(auto chunk: iter->second){
//             if (bgzf_seek(fp, chunk.first, SEEK_SET) < 0) {
//                 throw std::runtime_error("Error seeking to position in file");
//             }
//             MCRecord record;
//             while (bgzf_tell(fp) <= chunk.second && ballc.ReadMcRecord(record, fp)) {
//                 if(record.ref_id == ref_id && record.pos >= start && record.pos < end){
//                     results.push_back(record);
//                 }
//             }
//         }
//     }
    
//     bgzf_close(fp);
//     return results;
// }

