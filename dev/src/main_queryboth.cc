#include <iostream>
#include <htslib/hts.h>
#include <htslib/tbx.h>
#include <htslib/kseq.h>
#include <unordered_map>
#include "ballc_index.h"
#include "utils.h"

#include "timer.h"


int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <ballc_file> <meta_file> <range>\n";
        return 1;
    }


    const char *ballc_file = argv[1];
    const char *meta_file = argv[2];
    const char *range = argv[3];


    Timer timer;

    BAllcIndex index(ballc_file);
    std::cout << "Time taken by ballcindex construction: "<< timer.elapsed() << " s" << std::endl;timer.reset();

    auto records = index.QueryMcRecord2s(range);
    // std::vector<MCRecord2> records;
    // index.QueryMcRecord2s(range, records);


    std::cout << "Time taken by ballc query: "<< timer.elapsed() << " s" << std::endl;

    if(records.size()>0){
        std::string good_range = records.front().chrom;
        good_range+=":";
        good_range+=std::to_string(records.front().pos);
        good_range+="-";
        good_range+=std::to_string(records.back().pos);


        timer.reset();

        htsFile *fp = hts_open(meta_file, "r");
        tbx_t *tbx = tbx_index_load(meta_file);
        kstring_t str = {0, 0, nullptr};

        hts_itr_t *itr = tbx_itr_querys(tbx, good_range.c_str());
        if (itr == NULL) {
            throw std::runtime_error("Failed to query region.");
        }
        std::unordered_map<std::pair<std::string, int>, std::pair<std::string, std::string>, utils::pair_hash> meta_map;
        while (tbx_itr_next(fp, tbx, itr, &str) >= 0) {
            auto elems = utils::split(str.s, '\t');
            meta_map[std::pair<std::string, int>(elems[0], stoi(elems[1]))] = std::pair<std::string, std::string>(elems[2],elems[3]);
        }
        free(str.s);
        tbx_itr_destroy(itr);
        tbx_destroy(tbx);
        hts_close(fp);

        std::cout << "Time taken by tabix query: "<< timer.elapsed() << " s" << std::endl;


        timer.reset();
        for(auto record:records){
            std::cout << record.chrom << "\t" << record.pos << "\t";
            auto meta_ptr = meta_map.find(std::pair<std::string, int>(record.chrom, record.pos));
            if(meta_ptr==meta_map.end()){
                throw std::runtime_error("Ballc file and meta file do not match!");
            }
            std::cout << meta_ptr->second.first << "\t" << meta_ptr->second.second << "\t";
            std::cout << record.mc << "\t" << record.cov << "\t1\n";
        }
        std::cout << "Time taken by combine_out query: "<< timer.elapsed() << " s" << std::endl;

    }



    return 0;
}
