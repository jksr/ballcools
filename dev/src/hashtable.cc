#include "hashtable.h"

constexpr int TABLE_SIZE = 512;
constexpr unsigned int NOT_FOUND = -1;

HashTable::HashTable() : table(TABLE_SIZE) {}

 unsigned int HashTable::hash(const std::string& key)
 {
      unsigned int hash = 1315423911;

      for(std::size_t i = 0; i < key.length(); i++)
      {
          hash ^= ((hash << 5) + key[i] + (hash >> 2));
      }

      return (hash & 0x7FFFFFFF) % TABLE_SIZE;
 }

void HashTable::insert(const std::string& key, unsigned int value) {
    int index = hash(key);
    while (table[index].value!=-1 && table[index].key != key) {
        index = (index + 1) % TABLE_SIZE; // linear probing
    }
    table[index].key=key;
    table[index].value=value;
    //  = (KeyValue){.key=key, .value=value};
}

unsigned int HashTable::get(const std::string& key) {
    int index = hash(key);
    while (table[index].value!=-1 && table[index].key != key) {
        index = (index + 1) % TABLE_SIZE; // linear probing
    }
    if (table[index].value!=-1) {
        return table[index].value;
    }
    return NOT_FOUND; // Not found
}
