#ifndef _META_INDEXING_H_
#define _META_INDEXING_H_

#include <unordered_map>
#include <utility>
#include <string>


void ExtractCMeta(const char* fasta_path, const char* cmeta_path);
void IndexCMeta(const char* cmeta_path);

// std::unordered_map<
//     std::pair<std::string, unsigned int>, 
//     std::pair<std::string, std::string>
// >  QueryCMeta(const char* cmeta_path);

#endif//_META_INDEXING_H_
