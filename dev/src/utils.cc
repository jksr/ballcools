#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "utils.h"


namespace utils{

// https://gist.github.com/ScottHutchinson/6b699c997a33c33130821922c11d25c3
std::vector<std::string> split(const std::string s, const char delim, const size_t maxFields) {
    std::vector<std::string> elems;
    size_t start{};
    size_t end{};
    size_t numFieldsParsed{};

    do {
        end = s.find_first_of(delim, start);
        elems.emplace_back(s.substr(start, end - start));
        start = end + 1;
    } while (end != std::string::npos && (maxFields == 0 || ++numFieldsParsed < maxFields));
    return elems;
}


std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::string joined;
    joined.reserve(1024);

    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (it != vec.begin()) {
            joined.append(delimiter);
        }
        joined.append(*it);
    }
    return joined;
}


void AddSuffixIfNeeded(std::string& str, const std::string& suffix) {
    if (str.length() >= suffix.length() && str.substr(str.length() - suffix.length()) != suffix) {
        str += suffix;
    }
}

std::string RStrip(const std::string& str, const std::string& substr) {
    std::string result = str;
    size_t pos = result.rfind(substr);
    if (pos != std::string::npos && pos == result.length() - substr.length()) {
        result.erase(pos);
    }
    return result;
}

// template <class T1, class T2>
// std::size_t pair_hash::operator () (const std::pair<T1, T2> &p) const {
//     auto h1 = std::hash<T1>{}(p.first);
//     auto h2 = std::hash<T2>{}(p.second);

//     // Mainly for demonstration purposes, i.e. works but is overly simple
//     // In the real world, use a proper hash function
//     return h1 ^ h2;  
// }





}

