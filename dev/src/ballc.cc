#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "ballc.h"

#include "utils.h"


BAllc::BAllc(const char* ballc_path, const char mode){
    if(mode!='r'){
        throw std::runtime_error("Inappropriate mode1");
    }
    this->bgzf = bgzf_open(ballc_path, "r");
    this->writing_mode = false;
    ReadHeader();
    BuildRefDict();
    SetupIO();    
}


BAllc::BAllc(const char* ballc_path, bool sc, std::string assembly_text, std::string header_text, std::string chrom_size_path, const char mode){
    if(mode!='w'){
        throw std::runtime_error("Inappropriate mode2");
    }
    this->bgzf = bgzf_open(ballc_path, "w9");
    this->writing_mode = true;

    BAllcHeader &header = this->header;
    std::memcpy(header.magic,"BALLC\1",6);
    header.version = BALLC_VERSION;
    header.version_minor = BALLC_VERSION_MINOR;
    header.sc = uint8_t(sc);
    header.assembly_text = assembly_text;
    header.l_assembly = header.assembly_text.size();

    header.header_text = header_text;
    header.l_text = header.header_text.size();
    // header.refs = refs;
    // header.n_refs = header.refs.size();
    std::ifstream file(chrom_size_path);
    std::string line;
    while(std::getline(file, line)){
        line = utils::split(line, '\t')[0];
        RefRecord ref({.l_name=uint32_t(line.size()), .ref_name=line});
        header.refs.push_back(ref);
    }
    header.n_refs = header.refs.size();

    BuildRefDict();
    SetupIO();
} 


BAllc::~BAllc(){
    bgzf_close(this->bgzf);
}


MCRecord BAllc::AllcLineToMcRecord(std::string line){
    MCRecord rec;
    std::vector<std::string> elems = utils::split(line, '\t',7);
    rec.ref_id = this->ref_dict.get(elems[0]);
    rec.pos = std::stoi(elems[1]);
    // rec.strand = elems[2][0];
    // rec.c_context = this->context_dict.get(elems[3]);
    rec.mc = uint16_t(std::stoi(elems[4]));
    rec.cov = uint16_t(std::stoi(elems[5]));
    // rec.mc = std::stoi(elems[4]);
    // rec.cov = std::stoi(elems[5]);
    // rec.mc_text = elems[6];
    // rec.l_text = mc_text.size();

    return rec;
}

std::string BAllc::McRecordToLine(const MCRecord& record){
    std::string line = this->header.refs[record.ref_id].ref_name;
    line += "\t";
    line += std::to_string(record.pos);
    line += "\t";
    // line += record.strand;
    // line += "\t";
    // line += this->header.contexts[record.c_context];
    // line += "\t";
    line += std::to_string(uint32_t(record.mc));
    line += "\t";
    line += std::to_string(uint32_t(record.cov));
    line += "\n";
    return line;
}

MCRecord2 BAllc::McRecordToMcRecord2(const MCRecord& record){
    return MCRecord2({.pos = record.pos, 
                      .chrom=this->header.refs[record.ref_id].ref_name,
                      .mc = record.mc,
                      .cov = record.cov});
}



void BAllc::WriteMcRecord(const MCRecord& record){
    return (this->*func_write_record)(record);
}
int BAllc::ReadMcRecord(MCRecord& record){
    return (this->*func_read_record)(record);
}

void BAllc::WriteHeader(){
    if(!this->writing_mode){
        throw std::runtime_error("Inappropriate mode3");
    }
    BAllcHeader &header = this->header;
    BGZF* bgzf = this->bgzf;
    bgzf_write(bgzf, &header.magic, sizeof(header.magic));
    bgzf_write(bgzf, &header.version, sizeof(header.version));
    bgzf_write(bgzf, &header.version_minor, sizeof(header.version_minor));
    bgzf_write(bgzf, &header.sc, sizeof(header.sc));
    bgzf_write(bgzf, &header.l_assembly, sizeof(header.l_assembly));
    bgzf_write(bgzf, header.assembly_text.c_str(), header.l_assembly);
    bgzf_write(bgzf, &header.l_text, sizeof(header.l_text));
    bgzf_write(bgzf, header.header_text.c_str(), header.l_text);
    bgzf_write(bgzf, &header.n_refs, sizeof(header.n_refs));
    for(int i=0; i<header.n_refs; i++){
        bgzf_write(bgzf, &header.refs[i].l_name, sizeof(header.refs[i].l_name));
        bgzf_write(bgzf, header.refs[i].ref_name.c_str(), header.refs[i].l_name);
    }
}

int BAllc::Seek(int64_t pos){
    return bgzf_seek(this->bgzf, pos, SEEK_SET);
}

int64_t BAllc::Tell(){
    return bgzf_tell(this->bgzf);
}


int BAllc::ReadHeader(){
    if(this->writing_mode){
        throw std::runtime_error("Inappropriate mode4");
    }
    BGZF* bgzf = this->bgzf;
    // if(bgzf_seek(bgzf, 0, SEEK_SET) < 0) {
    //     throw std::runtime_error("Error seeking to the beginning of the file");
    // }

    BAllcHeader &header = this->header;
    bgzf_read(bgzf, &header.magic, sizeof(header.magic));
    bgzf_read(bgzf, &header.version, sizeof(header.version));
    bgzf_read(bgzf, &header.version_minor, sizeof(header.version_minor));
    bgzf_read(bgzf, &header.sc, sizeof(header.sc));

    bgzf_read(bgzf, &header.l_assembly, sizeof(header.l_assembly));
    char* tmp = new char[header.l_assembly];
    bgzf_read(bgzf, tmp, header.l_assembly);
    header.assembly_text.assign(tmp, header.l_assembly);
    delete tmp;

    bgzf_read(bgzf, &header.l_text, sizeof(header.l_text));
    tmp = new char[header.l_text];
    bgzf_read(bgzf, tmp, header.l_text);
    header.header_text.assign(tmp, header.l_text);
    delete tmp;

    bgzf_read(bgzf, &header.n_refs, sizeof(header.n_refs));
    header.refs = std::vector<RefRecord>(header.n_refs);
    tmp = new char[2048];
    for(int i=0; i<header.n_refs; i++){
        bgzf_read(bgzf, &header.refs[i].l_name, sizeof(header.refs[i].l_name));
        bgzf_read(bgzf, tmp, header.refs[i].l_name);
        header.refs[i].ref_name.assign(tmp, header.refs[i].l_name);
    }
    delete tmp;
}

void BAllc::BuildRefDict(){
    BAllcHeader &header = this->header;
    // HashTable* ref_dict = new HashTable();
    for(unsigned int i = 0; i<header.n_refs; i++){
        this->ref_dict.insert(header.refs[i].ref_name, i);
    }
    // this->ref_dict = ref_dict;
    // return ref_dict;
}

void BAllc::SetupIO(){
    if(this->header.sc){
        this->func_write_record = &BAllc::WriteScMcRecord;
        this->func_read_record = &BAllc::ReadScMcRecord;
    }
    else{
        this->func_write_record = &BAllc::WriteBlkMcRecord;
        this->func_read_record = &BAllc::ReadBlkMcRecord;
    }
}

// BAllc::BAllc(std::string header_text){
    
// }







// void DestroyDict(HashTable* dict){
//     //TODO
// }

// BGZF* BAllc::OpenBgzf(const std::string& filePath, const std::string& mode){
//     BGZF *bgzf = bgzf_open(filePath.c_str(), mode.c_str());
//     return bgzf;
// }

// bool BAllc::CloseBgzf(BGZF* bgzf){
//     bgzf_close(bgzf);
// }

void BAllc::WriteScMcRecord(const MCRecord& record){
    BGZF* bgzf = this->bgzf;
    bgzf_write(bgzf, &record.pos, sizeof(record.pos));
    bgzf_write(bgzf, &record.ref_id, sizeof(record.ref_id));
    // bgzf_write(bgzf, &record.ref_id, sizeof(record.ref_id));
    // bgzf_write(bgzf, &record.pos, sizeof(record.pos));
    // bgzf_write(bgzf, &record.strand, sizeof(record.strand));
    // bgzf_write(bgzf, &record.c_context, sizeof(record.c_context));

    uint8_t tmp = uint8_t(record.mc);
    bgzf_write(bgzf, &tmp, sizeof(tmp));
    tmp = uint8_t(record.cov);
    bgzf_write(bgzf, &tmp, sizeof(tmp));
    // bgzf_write(bgzf, &record.l_text, sizeof(record.l_text));
    // bgzf_write(bgzf, record.mc_text.data(), record.l_text);
}

void BAllc::WriteBlkMcRecord(const MCRecord& record){
    BGZF* bgzf = this->bgzf;
    bgzf_write(bgzf, &record.pos, sizeof(record.pos));
    bgzf_write(bgzf, &record.ref_id, sizeof(record.ref_id));
    // bgzf_write(bgzf, &record.ref_id, sizeof(record.ref_id));
    // bgzf_write(bgzf, &record.pos, sizeof(record.pos));
    // bgzf_write(bgzf, &record.strand, sizeof(record.strand));
    // bgzf_write(bgzf, &record.c_context, sizeof(record.c_context));

    bgzf_write(bgzf, &record.mc, sizeof(record.mc));
    bgzf_write(bgzf, &record.cov, sizeof(record.cov));
    // bgzf_write(bgzf, &record.l_text, sizeof(record.l_text));
    // bgzf_write(bgzf, record.mc_text.data(), record.l_text);
}

int BAllc::ReadScMcRecord(MCRecord& record){
    BGZF* bgzf = this->bgzf;
    // int good = bgzf_read(bgzf, &record.ref_id, sizeof(record.ref_id));
    int good = bgzf_read(bgzf, &record.pos, sizeof(record.pos));
    // std::vector<char> buffer;
    if (good>0) {
        bgzf_read(bgzf, &record.ref_id, sizeof(record.ref_id));
        // bgzf_read(bgzf, &record.pos, sizeof(record.pos));
        // bgzf_read(bgzf, &record.strand, sizeof(record.strand));
        // bgzf_read(bgzf, &record.c_context, sizeof(record.c_context));
        uint8_t tmp;
        bgzf_read(bgzf, &tmp, sizeof(tmp));
        record.mc = uint16_t(tmp);
        bgzf_read(bgzf, &tmp, sizeof(tmp));
        record.cov = uint16_t(tmp);
        // bgzf_read(bgzf, &record.l_text, sizeof(record.l_text));
        // buffer.resize(record.l_text);
        // bgzf_read(bgzf, buffer.data(), record.l_text);
        // record.mc_text = std::string(buffer.begin(), buffer.end());
    }
    return good;
}

int BAllc::ReadBlkMcRecord(MCRecord& record){
    BGZF* bgzf = this->bgzf;
    // int good = bgzf_read(bgzf, &record.ref_id, sizeof(record.ref_id));
    int good = bgzf_read(bgzf, &record.pos, sizeof(record.pos));
    // std::vector<char> buffer;
    if (good>0) {
        bgzf_read(bgzf, &record.ref_id, sizeof(record.ref_id));
        // bgzf_read(bgzf, &record.pos, sizeof(record.pos));
        // bgzf_read(bgzf, &record.strand, sizeof(record.strand));
        // bgzf_read(bgzf, &record.c_context, sizeof(record.c_context));
        bgzf_read(bgzf, &record.mc, sizeof(record.mc));
        bgzf_read(bgzf, &record.cov, sizeof(record.cov));
        // bgzf_read(bgzf, &record.l_text, sizeof(record.l_text));
        // buffer.resize(record.l_text);
        // bgzf_read(bgzf, buffer.data(), record.l_text);
        // record.mc_text = std::string(buffer.begin(), buffer.end());
    }
    return good;

}
