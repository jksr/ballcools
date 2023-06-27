#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>
#include <sstream>
#include <htslib/bgzf.h>

namespace utils{

std::vector<std::string> split(const std::string s, const char delim, const size_t maxFields = 0);
std::string join(const std::vector<std::string>& vec, const std::string& delimiter);


struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2> &p) const{
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use a proper hash function
        return h1 ^ h2;  
    }
    
};

void AddSuffixIfNeeded(std::string& str, const std::string& suffix);

}
#endif//_UTILS_H_
