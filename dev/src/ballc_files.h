#ifndef _BALLCFILE_H_
#define _BALLCFILE_H_
#include <string>
#include <vector>

struct RefRecord {
    uint32_t l_name;
    std::string ref_name;
};

constexpr int BALLC_VERSION = 0;
constexpr int BALLC_VERSION_MINOR = 1;

struct BAllcHeader {
    char magic[6];
    uint8_t version = BALLC_VERSION;
    uint8_t version_minor = BALLC_VERSION_MINOR;
    uint8_t sc;
    uint32_t l_assembly;
    std::string assembly_text;
    uint32_t l_text;
    std::string header_text;
    uint16_t n_refs;
    std::vector<RefRecord> refs;
    // uint32_t n_contexts;
    // std::vector<std::string> contexts;
};

struct MCRecord {
	uint32_t pos;
    uint16_t ref_id;
	// char strand;
	// uint8_t c_context;
    uint16_t mc;
    uint16_t cov;
    // uint32_t l_text;
    // std::string mc_text;
    // ~MCRecord(){}
};

struct MCRecord2 {
	uint32_t pos;
    std::string chrom;
	// char strand;
	// uint8_t c_context;
    uint16_t mc;
    uint16_t cov;
    // uint32_t l_text;
    // std::string mc_text;
    // ~MCRecord(){}
};

struct BAllcFile {
    BAllcHeader header;
    std::vector<MCRecord> mc_records;
};


struct IndexHeader{
    char magic[8];
    uint16_t n_refs;
    std::vector<RefRecord> refs;
};



// struct IndexKey {
//     int ref_id;
//     int bin;

//     bool operator<(const IndexKey &other) const {
//         if (ref_id == other.ref_id) {
//             return bin < other.bin;
//         }
//         return ref_id < other.ref_id;
//     }
// };

struct IndexKey {
    int ref_id;
    int bin;

    bool operator<(const IndexKey &other) const {
        return ref_id == other.ref_id && bin < other.bin;
    }
    bool operator==(const IndexKey &other) const{
        return ref_id == other.ref_id && bin == other.bin;
    }
};


// struct IndexValues{
//     int n_pairs;
//     std::vector<std::pair<int64_t, int64_t>> offset_pairs;
// };

struct IndexEntry{
    IndexKey key;
    int n_chunks;
    std::vector<std::pair<int64_t, int64_t>> chunks;
    bool operator<(const IndexEntry &other) const {
        // return key.ref_id == other.key.ref_id && key.bin < other.key.bin;
        return key.bin < other.key.bin;
    } 
};

// struct BAllcIndexFile{
//     IndexHeader header;
//     uint64_t n_indexes;
//     std::vector<std::pair<IndexKey, IndexValues>> index;
// };

struct BAllcIndexFile{
    IndexHeader header;
    uint64_t n_entries;
    std::vector<IndexEntry> index;
};

#endif//_BALLCFILE_H_
