#ifndef _ALLC_H_
#define _ALLC_H_
#include <string>
#include <zlib.h>
#include <fstream>


class Allc {
public:
    Allc(const char* file_path);
    ~Allc();
    std::string ReadLine();

private:
    bool IsGzipFile(const char* file_path);
    bool is_gzip_;
    gzFile gzip_file_;
    std::ifstream plain_file_;
};
#endif//_ALLC_H_
