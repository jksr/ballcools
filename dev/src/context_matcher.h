#ifndef _CONTEXT_MATCHER_H_
#define _CONTEXT_MATCHER_H_


#include <string>

class CContextMatcher {
public:
    CContextMatcher(const std::string &pattern);

    bool Matches(const std::string &test_str) const;

private:

    bool IsMatch(char pattern_char, char test_char) const;

    std::string pattern;

};


#endif//_CONTEXT_MATCHER_H_