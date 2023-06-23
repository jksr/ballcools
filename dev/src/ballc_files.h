#ifndef _BALLCFILE_H_
#define _BALLCFILE_H_
#include <string>
#include <vector>

struct RefRecord {
    uint32_t l_name;
    std::string ref_name;
    uint32_t ref_length;
};

constexpr int BALLC_VERSION = 0;
constexpr int BALLC_VERSION_MINOR = 1;
constexpr char BALLC_MAGIC[] = "BALLC\1";
constexpr char BALLC_INDEX_MAGIC[] = "BALLCIDX\1";

struct BAllCHeader {
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
};


// internal record to store in file
struct MCRecord {
	uint32_t pos;
    uint16_t ref_id;
    uint16_t mc;
    uint16_t cov;
};

// record to display
struct MCRecord2 {
	uint32_t pos;
    std::string chrom;
    uint16_t mc;
    uint16_t cov;
};

struct BAllCFile {
    BAllCHeader header;
    std::vector<MCRecord> mc_records;
};


struct IndexHeader{
    char magic[8];
    uint16_t n_refs;
    std::vector<RefRecord> refs;
};




struct IndexKey {
    uint16_t ref_id;
    uint32_t bin;

    bool operator<(const IndexKey &other) const {
        return ref_id == other.ref_id && bin < other.bin;
        // if (ref_id == other.ref_id) {
        //     return bin < other.bin;
        // }
        // return ref_id < other.ref_id;
    }
    bool operator==(const IndexKey &other) const{
        return ref_id == other.ref_id && bin == other.bin;
    }
};



struct IndexEntry{
    IndexKey key;
    // IndexChunk chunk;
    uint64_t chunk_start;
    uint64_t chunk_end;
    bool operator<(const IndexEntry &other) const {
        return this->key < other.key;
    } 
};


struct BAllCIndexFile{
    IndexHeader header;
    uint32_t n_entries;
    std::vector<IndexEntry> index;
};

#endif//_BALLCFILE_H_
