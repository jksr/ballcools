#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_
#include <vector>
#include <string>

// constexpr unsigned int NOT_FOUND = -1;


struct KeyValue {
    std::string key = "";
    unsigned int value = -1;
};

class HashTable {
private:
    std::vector<KeyValue> table;
    unsigned int  hash(const std::string& key);

public:
    HashTable();
    void insert(const std::string& key, unsigned int value);
    unsigned int get(const std::string& key);
};
#endif//_HASHTABLE_H_
