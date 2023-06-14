#include <unordered_map>
#include <utility>
#include <string>


void ExtractCMeta(const char* fasta_path, const char* cmeta_path);
void IndexCMeta(const char* cmeta_path);
std::unordered_map<
    std::pair<std::string, unsigned int>, 
    std::pair<std::string, std::string>
>  QueryCMeta(const char* cmeta_path);
