#include <unordered_map>
#include "context_matcher.h"

CContextMatcher::CContextMatcher(const std::string &pattern="*") : pattern(pattern) {}

bool CContextMatcher::Matches(const std::string &test_str) const{
    if(this->pattern=="*"){
        return true;
    }

    if (test_str.length() != this->pattern.length()) {
        return false;
    }

    for (int i = 0; i < this->pattern.length(); ++i) {
        if (!IsMatch(this->pattern[i], test_str[i])) {
            return false;
        }
    }

    return true;
}

bool CContextMatcher::IsMatch(char pattern_char, char test_char) const{
    static const std::unordered_map<char, std::string> iupac_codes = {
        {'A', "A"},
        {'C', "C"},
        {'G', "G"},
        {'T', "T"},
        {'U', "U"},
        {'R', "AG"},
        {'Y', "CT"},
        {'S', "CG"},
        {'W', "AT"},
        {'K', "GT"},
        {'M', "AC"},
        {'B', "CGT"},
        {'D', "AGT"},
        {'H', "ACT"},
        {'V', "ACG"},
        {'N', "ACGTN"}
    };

    auto it = iupac_codes.find(pattern_char);
    if (it == iupac_codes.end()) {
        return false;
    }

    return it->second.find(test_char) != std::string::npos;
}
