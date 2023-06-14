
#include <vector>
#include <unordered_map>
#include  <stdexcept>
// #include <functional>
#include <algorithm>
#include "ballc_files.h"
#include "ballc_index_core.h"



std::size_t IndexKeyHash::operator()(const IndexKey& key) const {
    std::size_t h1 = std::hash<int>()(key.ref_id);
    std::size_t h2 = std::hash<int>()(key.bin);
    return h1 ^ (h2 << 1);
}




void IndexCore::AddChunk(IndexKey key, std::pair<int64_t, int64_t> chunk){
    if((!this->index_vec.empty()) && (key < this->index_vec.back().key)){
        throw std::runtime_error("The records are unsorted");
    }
    auto pos = this->pos_book.find(key);
    if (pos == this->pos_book.end()) {
        this->index_vec.push_back(IndexEntry({.key=key, .n_chunks=1, .chunks={chunk}}));
        this->pos_book[key] = this->index_vec.size() - 1;

        auto ref_rec = this->ref_id_book.find(key.ref_id);
        if ( ref_rec == this->ref_id_book.end()){
            this->ref_id_book[key.ref_id] = {this->index_vec.size()-1, this->index_vec.size()};
        }
        else{
            this->ref_id_book[key.ref_id].second = this->index_vec.size();
        }
    } else {
        this->index_vec[pos->second].chunks.push_back(chunk);
        this->index_vec[pos->second].n_chunks++;
    }
    
}


IndexVec::iterator IndexCore::UpperBound(const IndexKey& key) {
    auto pos = this->pos_book.find(key);
    if ( pos == this->pos_book.end()){
        auto ref_id_pos = this->ref_id_book.find(key.ref_id);
        if(ref_id_pos != this-> ref_id_book.end()){
            return std::upper_bound (this->index_vec.begin() + ref_id_pos->second.first,
                                    this->index_vec.begin() + ref_id_pos->second.second,
                                    IndexEntry({.key=key}));
        }
    }
    else{
        return this->index_vec.begin()+pos->second +1; //+1?
    }
}

IndexVec::iterator IndexCore::LowerBound(const IndexKey& key) {
    auto pos = this->pos_book.find(key);
    if ( pos == this->pos_book.end()){
        auto ref_id_pos = this->ref_id_book.find(key.ref_id);
        if(ref_id_pos != this-> ref_id_book.end()){
            return std::lower_bound(this->index_vec.begin() + ref_id_pos->second.first,
                                    this->index_vec.begin() + ref_id_pos->second.second,
                                    IndexEntry({.key=key}));
        }
    }
    else{
        if(pos->second>0){
            auto prev = (this->index_vec.begin()+pos->second-1);
            if(prev->key.ref_id==key.ref_id){
                return prev;
            }
        } //is this necessary?
        return this->index_vec.begin()+pos->second;
    }
}


void IndexCore::WriteIndex(BGZF* bgzf){
    size_t size = this->index_vec.size();
    bgzf_write(bgzf, &size, sizeof(size));
    for(auto index_entry:this->index_vec){
        bgzf_write(bgzf, &index_entry.key.ref_id, sizeof(index_entry.key.ref_id));
        bgzf_write(bgzf, &index_entry.key.bin, sizeof(index_entry.key.bin));
        bgzf_write(bgzf, &index_entry.n_chunks, sizeof(index_entry.n_chunks));
        for(auto chunk:index_entry.chunks){
            bgzf_write(bgzf, &chunk.first, sizeof(chunk.first));
            bgzf_write(bgzf, &chunk.second, sizeof(chunk.second));
        }
    }
    size = this->pos_book.size();
    bgzf_write(bgzf, &size, sizeof(size));
    for(auto key_n_pos:this->pos_book){
        bgzf_write(bgzf, &key_n_pos.first.ref_id, sizeof(key_n_pos.first.ref_id));
        bgzf_write(bgzf, &key_n_pos.first.bin, sizeof(key_n_pos.first.bin));
        bgzf_write(bgzf, &key_n_pos.second, sizeof(key_n_pos.second));
    }
    size = this->ref_id_book.size();
    bgzf_write(bgzf, &size, sizeof(size));
    for(auto refid_n_startend:this->ref_id_book){
        bgzf_write(bgzf, &refid_n_startend.first, sizeof(refid_n_startend.first));
        bgzf_write(bgzf, &refid_n_startend.second.first, sizeof(refid_n_startend.second.first));
        bgzf_write(bgzf, &refid_n_startend.second.second, sizeof(refid_n_startend.second.second));
    }

}

int IndexCore::ReadIndex(BGZF* bgzf){
    size_t size, pos1, pos2;
    int ref_id, bin, n_chunks;
    int64_t chunk_start, chunk_end;
    std::vector<std::pair<int64_t, int64_t>> chunks; 
    bgzf_read(bgzf, &size, sizeof(size));
    this->index_vec.reserve(size);
    for(int i = 0; i < size; i++){
        bgzf_read(bgzf, &ref_id, sizeof(ref_id));
        bgzf_read(bgzf, &bin, sizeof(bin));
        bgzf_read(bgzf, &n_chunks, sizeof(n_chunks));
        chunks.clear();
        chunks.reserve(n_chunks);
        for(int j = 0; j < n_chunks; j++){
            bgzf_read(bgzf, &chunk_start, sizeof(chunk_start));
            bgzf_read(bgzf, &chunk_end, sizeof(chunk_end));
            chunks.push_back({chunk_start, chunk_end});
        }
        this->index_vec.push_back(IndexEntry({IndexKey({ref_id, bin}), n_chunks, chunks}));
    }
    bgzf_read(bgzf, &size, sizeof(size));
    this->pos_book.reserve(size);
    for(int i = 0; i < size; i++){
        bgzf_read(bgzf, &ref_id, sizeof(ref_id));
        bgzf_read(bgzf, &bin, sizeof(bin));
        bgzf_read(bgzf, &pos1, sizeof(pos1));
        this->pos_book[IndexKey({ref_id, bin})] = pos1;
    }
    bgzf_read(bgzf, &size, sizeof(size));
    this->ref_id_book.reserve(size);
    for(int i = 0; i < size; i++){
        bgzf_read(bgzf, &ref_id, sizeof(ref_id));
        bgzf_read(bgzf, &pos1, sizeof(pos1));
        bgzf_read(bgzf, &pos2, sizeof(pos2));
        this->ref_id_book[ref_id] = {pos1, pos2};
    }
}
