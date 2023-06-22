#ifndef _BALLC_INDEX_CORE_H_
#define _BALLC_INDEX_CORE_H_


#include <vector>
#include <unordered_map>
#include "ballc_files.h"
#include <htslib/bgzf.h>


struct IndexKeyHash {
    std::size_t operator()(const IndexKey& key) const;
};


typedef std::vector<IndexEntry> IndexVec;

class BallcIndex;


class IndexCore{
public:

    void AddChunk(IndexKey key, uint64_t chunk_start, uint64_t chunk_end);

    IndexVec::iterator UpperBound(const IndexKey& key);
    IndexVec::iterator LowerBound(const IndexKey& key);
    IndexVec::iterator Begin();
    IndexVec::iterator End();

// private:
    IndexVec index_vec;
    std::unordered_map<IndexKey, size_t, IndexKeyHash> pos_book;
    std::unordered_map<uint16_t, std::pair<size_t,size_t>> ref_id_book;

public://TODO
    void WriteIndex(BGZF* bgzf);
    int ReadIndex(BGZF* bgzf);

    // friend class BallcIndex;
};

#endif//_BALLC_INDEX_CORE_H_