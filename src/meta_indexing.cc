#include "meta_indexing.h"
#include <fstream>
#include <stdexcept>
#include <cstdlib> // for system()
#include "utils.h"
// #include <htslib/hts.h>
// #include <htslib/tbx.h>
// #include <htslib/kseq.h>


char complement(char base) {
    switch (base) {
        case 'A': return 'T';
        case 'T': return 'A';
        case 'C': return 'G';
        case 'G': return 'C';
        case 'N': return 'N';
        default: return 'N';
    }
}

void ExtractCMeta(const char* fasta_path, const char* cmeta_path){
    std::ifstream fasta(fasta_path);
    std::ofstream cmeta(cmeta_path);

    if (!fasta.is_open()) {
        throw std::runtime_error("Cannot open the fasta file "+std::string(fasta_path));
    }

    if (!cmeta.is_open()) {
        throw std::runtime_error("Cannot open the c-index file "+std::string(cmeta_path));
    }    

    std::string line, identifier, sequence = "";
    int position;
    while (std::getline(fasta, line)) {
        if (line[0] == '>') {
            if (!sequence.empty()) {
                for (size_t i = 0; i < sequence.size(); i++) {
                    char base = toupper(sequence[i]);  // convert to uppercase for case-insensitive search
                    std::string context = "";
                    if (base == 'C') {
                        context += base;
                        // Add next two characters to context, if they exist
                        if (i < sequence.size() - 1) context += toupper(sequence[i+1]);
                        else context += 'N';
                        if (i < sequence.size() - 2) context += toupper(sequence[i+2]);
                        else context += 'N';
                        // Output to tsv format: chrom, position, strand, context
                        cmeta << identifier << "\t" << (i+1) << "\t+\t" << context << "\n";
                    } else if (base == 'G') {
                        context += 'C';
                        // Add next two characters to context, if they exist (and take reverse complement)
                        if (i > 1) context += complement(toupper(sequence[i-1]));
                        else context += 'N';
                        if (i > 0) context += complement(toupper(sequence[i-2]));
                        else context += 'N';
                        // Output to tsv format: chrom, position, strand, context
                        cmeta << identifier << "\t" << (i+1) << "\t-\t" << context << "\n";
                    }
                }
            }
            // Prepare for next sequence
            identifier = line.substr(1);  // remove ">" symbol
            identifier = utils::split(identifier,' ')[0];
            identifier = utils::split(identifier,'\t')[0];
            sequence.clear();
        } else {
            sequence += line;
        }
    }
    // Process the last sequence in the file
    if (!sequence.empty()) {
        for (size_t i = 0; i < sequence.size(); i++) {
            char base = toupper(sequence[i]);  // convert to uppercase for case-insensitive search
            std::string context = "";
            if (base == 'C') {
                context += base;
                // Add next two characters to context, if they exist
                if (i < sequence.size() - 1) context += toupper(sequence[i+1]);
                else context += 'N';
                if (i < sequence.size() - 2) context += toupper(sequence[i+2]);
                else context += 'N';
                // Output to tsv format: chrom, position, strand, context
                cmeta << identifier << "\t" << (i+1) << "\t+\t" << context << "\n";
            } else if (base == 'G') {
                context += 'C';
                // Add next two characters to context, if they exist (and take reverse complement)
                if (i > 1) context += complement(toupper(sequence[i-1]));
                else context += 'N';
                if (i > 0) context += complement(toupper(sequence[i-2]));
                else context += 'N';
                // Output to tsv format: chrom, position, strand, context
                cmeta << identifier << "\t" << (i+1) << "\t-\t" << context << "\n";
            }
        }
    }

    fasta.close();
    
}



void IndexCMeta(const char* cmeta_path){
    std::string command;
    int exitCode;

    std::string cmeta_file = cmeta_path;
    command = "bgzip  " + cmeta_file;

    exitCode = system(command.c_str());
    if (exitCode != 0) {
        throw std::runtime_error("Failed to compress the file: " + cmeta_file);
    }

    // Create the index with tabix
    command = "tabix -f -b 2 -e 2 -s 1 " + cmeta_file + ".gz";
    exitCode = system(command.c_str());
    if (exitCode != 0) {
        throw std::runtime_error("Failed to create index for the file: " + cmeta_file + ".gz");
    }
}





// std::unordered_map<
//     std::pair<std::string, unsigned int>, 
//     std::pair<std::string, std::string>
// >  QueryCMeta(const char* cmeta_path){
//     std::string cmeta_file = cmeta_path;

// }





// #include <iostream>

// int main(int argc, char *argv[]) {
//     if (argc < 3) {
//         std::cerr << "Usage: " << argv[0] << " <file.tsv.gz> <region>\n";
//         return 1;
//     }

//     const char *file = argv[1];
//     const char *region = argv[2];

//     htsFile *fp = hts_open(file, "r");
//     tbx_t *tbx = tbx_index_load(file);
//     kstring_t str = {0, 0, nullptr};

//     hts_itr_t *itr = tbx_itr_querys(tbx, region);
//     if (itr == NULL) {
//         std::cerr << "Failed to query region.\n";
//         return 1;
//     }

//     while (tbx_itr_next(fp, tbx, itr, &str) >= 0) {
//         std::cout << "Row: " << str.s << "\n";
//     }

//     free(str.s);
//     tbx_itr_destroy(itr);
//     tbx_destroy(tbx);
//     hts_close(fp);

//     return 0;
// }

