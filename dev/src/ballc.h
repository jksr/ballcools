#ifndef _BALLC_H_
#define _BALLC_H_

#include <string>
#include <vector>
#include "hashtable.h"
#include <htslib/bgzf.h>
#include "ballc_files.h"

class BAllc:public BAllcFile{
friend class BAllcIndex;
public:
    BAllc(const BAllc& ballc);
    BAllc(const char* ballc_path, const char mode='r');
    BAllc(const char* ballc_path, bool sc, std::string assembly_text, std::string header_text, std::string chrom_size_path, const char mode='w');
    ~BAllc();
    MCRecord AllcLineToMcRecord(std::string line);
    std::string McRecordToLine(const MCRecord& record);
    MCRecord2 McRecordToMcRecord2(const MCRecord& record);
    typedef void (BAllc::*FuncWriteRecord)(const MCRecord& record);
    typedef int (BAllc::*FuncReadRecord)(MCRecord& record);
    void WriteMcRecord(const MCRecord& record);
    int ReadMcRecord(MCRecord& record);

    void WriteHeader();
    int Seek(int64_t pos);
    int64_t Tell();



private://TODO
    int ReadHeader();
    void BuildRefDict();
    void SetupIO();
    void WriteScMcRecord(const MCRecord& record);
    void WriteBlkMcRecord(const MCRecord& record);
    int ReadScMcRecord(MCRecord& record);
    int ReadBlkMcRecord(MCRecord& record);

    BGZF* bgzf;
    bool writing_mode;
    FuncWriteRecord func_write_record;
    FuncReadRecord func_read_record;
    HashTable ref_dict;
    const std::string ballc_path;
};

#endif//_BALLC_H_
