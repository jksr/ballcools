#ifndef _BALLC_INDEX_H_
#define _BALLC_INDEX_H_

#include <map>
#include <vector>
#include "hashtable.h"
#include "ballc.h"
#include "ballc_files.h"
#include "ballc_index_core.h"

#include "ballc_iterator.h"

constexpr int GRANGE_END_MAX = (1 << 26) - 1;


class BAllcIndex:private BAllcIndexFile{
public:
    BAllcIndex(const char* ballc_path);
    ~BAllcIndex();
    void BuildIndex();
    void WriteIndex(bool override=false);
    // std::vector<std::string> Query(const std::string& range);
    std::vector<MCRecord> QueryMcRecords(const std::string& range);
    // MCRecordIterator IterQueryMcRecords(const std::string& range);
    std::vector<MCRecord> IterQueryMcRecords(const std::string& range);
    std::vector<std::string> IterQueryLines(const std::string& range);
    std::vector<std::string> QueryLines(const std::string& range);
    std::vector<MCRecord2> QueryMcRecord2s(const std::string& range);
    // void QueryMcRecord2s(const std::string& range, std::vector<MCRecord2>& results);


private:

    int ReadIndex();
    int RegToBin(int beg, int end);
    void ParseGenomeRange(const std::string& range, std::string& chrom, int& start, int& end);
    void InitHeader();
    // void WorkingIndexToFileIndex();
    // void FileIndexToWorkingIndex();
    void BuildRefDict();

    bool is_indexed = false;
    IndexCore working_index;
    // std::map<IndexKey, std::vector<std::pair<int64_t, int64_t>>> working_index;

    std::string ballc_path;
    std::string index_path;
    HashTable ref_dict;
    BAllc ballc;
};



#endif//_BALLC_INDEX_H_
