
#include <vector>
#include <unordered_map>
#include  <stdexcept>
// #include <functional>
#include <algorithm>
#include "ballc_files.h"
#include "ballc_index_core.h"

#include <iostream>


std::size_t IndexKeyHash::operator()(const IndexKey& key) const {
    std::size_t h1 = std::hash<int>()(key.ref_id);
    std::size_t h2 = std::hash<int>()(key.bin);
    return h1 ^ (h2 << 1);
}




void IndexCore::AddChunk(IndexKey key, uint64_t chunk_start, uint64_t chunk_end){
    // if((!this->index_vec.empty()) && (key < this->index_vec.back().key)){
    //     throw std::runtime_error("The records are unsorted");
    // }
    auto pos = this->pos_book.find(key);
    if (pos == this->pos_book.end()) {
        // this->index_vec.push_back(IndexEntry({.key=key, .n_chunks=1, .chunks={chunk}}));
        // this->index_vec.push_back(IndexEntry({key, chunk}));
        this->index_vec.push_back(IndexEntry({key, chunk_start, chunk_end}));
        this->pos_book[key] = this->index_vec.size() - 1;

        auto ref_rec = this->ref_id_book.find(key.ref_id);
        if ( ref_rec == this->ref_id_book.end()){
            this->ref_id_book[key.ref_id] = {this->index_vec.size()-1, this->index_vec.size()};
        }
        else{
            this->ref_id_book[key.ref_id].second = this->index_vec.size();
        }
    } else {
        // this->index_vec[pos->second].chunks.push_back(chunk);
        // this->index_vec[pos->second].n_chunks++;
        // this->index_vec[pos->second].chunks.back().second=chunk.second;
        // this->index_vec[pos->second].chunk.second=chunk.second;
        this->index_vec[pos->second].chunk_end=chunk_end;
    }
}


IndexVec::iterator IndexCore::UpperBound(const IndexKey& key) {
    std::cout << "upper in\n"; 
    auto pos = this->pos_book.find(key);
    if ( pos == this->pos_book.end()){
        auto ref_id_pos = this->ref_id_book.find(key.ref_id);
        if(ref_id_pos != this-> ref_id_book.end()){
    std::cout << "upper out1\n"; 
            return std::upper_bound (this->index_vec.begin() + ref_id_pos->second.first,
                                    this->index_vec.begin() + ref_id_pos->second.second,
                                    IndexEntry({.key=key}));
        }
        else{

    std::cout << "upper out3\n"; 
            throw std::runtime_error("Unknown chrom id");
            
        }
    }
    else{
    std::cout << "upper out2\n"; 
        return this->index_vec.begin()+pos->second +1; //+1?
    }
}

IndexVec::iterator IndexCore::LowerBound(const IndexKey& key) {
    std::cout << "lower iin\n"; 
    auto pos = this->pos_book.find(key);
    if ( pos == this->pos_book.end()){
        auto ref_id_pos = this->ref_id_book.find(key.ref_id);
        if(ref_id_pos != this-> ref_id_book.end()){
    std::cout << "lower out1\n"; 
            return std::lower_bound(this->index_vec.begin() + ref_id_pos->second.first,
                                    this->index_vec.begin() + ref_id_pos->second.second,
                                    IndexEntry({.key=key}));
        }
        else{
            throw std::runtime_error("Unknown chrom id");
            
        }
    }
    else{
        if(pos->second>0){
            auto prev = (this->index_vec.begin()+pos->second-1);
            if(prev->key.ref_id==key.ref_id){
    std::cout << "lower out3\n"; 
                return prev;
            }
        } //is this necessary?
    std::cout << "lower out2\n"; 
        return this->index_vec.begin()+pos->second;
    }
    std::cout << "lower wrong\n"; 
}

IndexVec::iterator IndexCore::Begin() {
    return this->index_vec.begin();
}

IndexVec::iterator IndexCore::End() {
    return this->index_vec.end();
}


void IndexCore::WriteIndex(BGZF* bgzf){
    size_t size = this->index_vec.size();
    bgzf_write(bgzf, &size, sizeof(size));
    for(auto index_entry:this->index_vec){
        bgzf_write(bgzf, &index_entry.key.ref_id, sizeof(index_entry.key.ref_id));
        bgzf_write(bgzf, &index_entry.key.bin, sizeof(index_entry.key.bin));
        bgzf_write(bgzf, &index_entry.chunk_start, sizeof(index_entry.chunk_start));
        bgzf_write(bgzf, &index_entry.chunk_end, sizeof(index_entry.chunk_end));

    }
}

int IndexCore::ReadIndex(BGZF* bgzf){
    size_t size;
    IndexEntry tmp_idxentry;
    decltype(tmp_idxentry.key.bin) bin;
    decltype(tmp_idxentry.key.ref_id) ref_id;
    decltype(tmp_idxentry.chunk_start) chunk_start;
    decltype(tmp_idxentry.chunk_end) chunk_end;
    bgzf_read(bgzf, &size, sizeof(size));
    this->index_vec.reserve(size);
    for(int i = 0; i < size; i++){
        bgzf_read(bgzf, &ref_id, sizeof(ref_id));
        bgzf_read(bgzf, &bin, sizeof(bin));
        bgzf_read(bgzf, &chunk_start, sizeof(chunk_start));
        bgzf_read(bgzf, &chunk_end, sizeof(chunk_end));

        AddChunk(IndexKey({ref_id, bin}), chunk_start, chunk_end);
    }
}
