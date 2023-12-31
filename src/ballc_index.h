#ifndef _BALLC_INDEX_H_
#define _BALLC_INDEX_H_

#include <map>
#include <vector>
#include "hashtable.h"
#include "ballc.h"
#include "ballc_files.h"
#include "ballc_index_core.h"

#include "ballc_iterator.h"


class BAllCIndex:private BAllCIndexFile{
public:
    BAllCIndex(const char* ballc_path);
    ~BAllCIndex();
    void BuildIndex();
    void WriteIndex(bool override=false);


    // std::vector<MCRecord> QueryMcRecords(const std::string& range);
    // std::vector<std::string> QueryLines(const std::string& range);
    // std::vector<MCRecord2> QueryMcRecord2s(const std::string& range);
    
    MCRecordIterator QueryMcRecords_Iter(const std::string& range);
    MCRecordIterator QueryMcRecords_Iter(const std::string& chrom, int start, int end);

    static const int GRANGE_END_MAX = (1 << 29) - 1;
    // static const int GRANGE_END_MAX = (1 << 26) - 1;


private:

    int ReadIndex();
    int RegToBin(int beg, int end);
    void ParseGenomeRange(const std::string& range, std::string& chrom, int& start, int& end);
    void InitHeader();
    void BuildRefDict();

    bool IsIndexOld();

    bool is_indexed = false;
    IndexCore working_index;

    std::string ballc_path;
    std::string index_path;
    // HashTable ref_dict;
    std::unordered_map<std::string, unsigned int> ref_dict;
    BAllC ballc;
};



#endif//_BALLC_INDEX_H_
