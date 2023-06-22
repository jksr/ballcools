#ifndef _ALLC_H_
#define _ALLC_H_
#include <string>
#include <zlib.h>
#include <fstream>


class AllC {
public:
    AllC(const char* file_path);
    ~AllC();
    std::string ReadLine();

private:
    bool IsGzipFile(const char* file_path);
    bool is_gzip_;
    gzFile gzip_file_;
    std::ifstream plain_file_;
};
#endif//_ALLC_H_
