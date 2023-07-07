#include <fstream>
#include <sstream>
#include <zlib.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include "allc.h"
#include "utils.h"



AllC::AllC(const char* file_path) {
    this->is_gzip_ = IsGzipFile(file_path);
    if (this->is_gzip_) {
        this->gzip_file_ = gzopen(file_path, "rb");
        if (this->gzip_file_ == nullptr) {
            throw std::runtime_error("cannot open the file "+std::string(file_path)+".");
        }
    } else {
        this->plain_file_.open(file_path, std::ios::in);
        if (this->plain_file_.fail()) {
            throw std::runtime_error("cannot open the file "+std::string(file_path)+".");
        }
    }
}

AllC::~AllC() {
    if (this->is_gzip_ && this->gzip_file_ != nullptr) {
        gzclose(this->gzip_file_);
    }
}


std::string AllC::ReadLine() {
    std::string line;
    if (this->is_gzip_) {
        char buffer[1024];
        if (gzgets(this->gzip_file_, buffer, sizeof(buffer)) != Z_NULL) {
            line = std::string(buffer);
            // remove trailing newline if present
            if (!line.empty() && line[line.length() - 1] == '\n') {
                line.erase(line.length() - 1);
            }
            return line;
        } else {
            return "";
        }
    } else {
        if(static_cast<bool>(std::getline(this->plain_file_, line))){
            return line;
        }else{
            return "";
        }
    }
}

bool AllC::IsGzipFile(const char* file_path) {
    std::string filename(file_path);
    return filename.size() > 3 && filename.substr(filename.size() - 3) == ".gz";
}
