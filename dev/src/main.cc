#include "CLI11.hpp"
#include "routines.h"

// #include "merge_ballc.h"


#include <iostream>

#include <string>

int main(int argc, char **argv) {
    CLI::App app{"BAllCools: Binary AllC file tools"};

    // Adding subcommands
    CLI::App* cmd_view = app.add_subcommand("view", "View data stored in a BAllC file.");
    CLI::App* cmd_index = app.add_subcommand("index", "Index a BAllC file.");
    CLI::App* cmd_a2ballc = app.add_subcommand("a2b", "Convert an AllC file to a BAllc file.");
    CLI::App* cmd_meta = app.add_subcommand("meta", "Extract and index C from a genome sequence file (fasta) and store in a CMeta file (bed format).");
    CLI::App* cmd_query = app.add_subcommand("query", "Query info from a BAllC file");
    CLI::App* cmd_check = app.add_subcommand("check", "check a BAllC file");
    CLI::App* cmd_merge = app.add_subcommand("merge", "Merge BAllC files");

    // Defining options for each subcommand
    std::string allc_path, ballc_path, chrom_size_path, assembly, header_text;
    std::string fasta_path, cmeta_path;
    std::string grange;
    std::string c_context;
    bool sc=true;
    bool q_warn=false, q_err=false, q_skip=false;
    bool header=false, refs=false, records=false;
    std::vector<std::string> ballc_paths;
    std::string out_path, file_of_path;
    int chunksize = 1000000;

    // allc-to-ballc
    cmd_a2ballc->add_option("allcpath", allc_path, "AllC file path")->required();
    cmd_a2ballc->add_option("ballcpath", ballc_path, "BAllC file path")->required();
    cmd_a2ballc->add_option("chrompath", chrom_size_path, "Path to the chromosome size file or the .fai file")->required();
    cmd_a2ballc->add_flag("-s,--sc", sc, "Whether or not to convernt the AllC file to the single-cell (sc) BAllC format. \
uint8 data type is used to store methylation info in sc BAllC format, while uint16 is used in non-sc format, \
resulting in smaller disk usage for sc BAllC format. \
However, converting a non-sc AllC file to a sc BAllC file format may result in broken data in the BAllC file. \
Default: true")->default_val(true);
    cmd_a2ballc->add_option("-a,--assembly_info", assembly, "Info of the assembly used in the AllC file. eg, mm10, hg38, hg38-donor1, etc. Default: \"\""
                            )->default_val("");
    cmd_a2ballc->add_option("-n,--note", header_text, "Note related to the AllC file. Default: \"\""
                            )->default_val("");


    // index ballc
    cmd_index->add_option("ballcpath", ballc_path, "BAllC file path")->required();

    // extract cmeta
    cmd_meta->add_option("fastapath", fasta_path, "Genome FASTA file path")->required();
    cmd_meta->add_option("cmetapath", cmeta_path, "Output path")->required();

    // query ballc
    cmd_query->add_option("ballcpath", ballc_path, "BAllC file path")->required();
    cmd_query->add_option("genomerange", grange, "Genome range of interests. Supported formats are chrX, chrX:\
    chrX:XXX-XXX, chrX:-XXX, chrX:XXX-")->required();
    cmd_query->add_option("-c,--cmetapath", cmeta_path, "Output path")->default_val("");
    cmd_query->add_option("-x,--c_context", c_context, "Filter Cs only in desired context. \
The context code is three letters from IUPAC nucleotide symbols. The first letter should always be C.\
For example, the c_context CGN matches CGA, CGT, CGC, CGG, and CGN, \
and only C records in these contexts will be displayed.")->default_val("*");
    cmd_query->add_flag("-w,--warn_mismatch", q_warn, "Warning message will be displayed if mismatches detected between the BAllC file and the CMeta.\
The program WILL NOT halt. Default: false."
                            );
    cmd_query->add_flag("-e,--err_mismatch", q_err, "Runtime error will be caused if mismatches detected between the BAllC file and the CMeta.\
The program WILL halt. Default: false."
                            );
    cmd_query->add_flag("-s,--skip_mismatch", q_skip, "Skip displaying the C record which has mismatch between the BAllC file and the CMeta.\
If not skip, the strandness and the C-context of the C will be displayed as \"?\tC??\". Default: truee."
                            );


    cmd_view->add_option("ballcpath", ballc_path, "BAllC file path")->required();
    cmd_view->add_flag("-d,--header", header, "View header")->default_val(false);
    cmd_view->add_flag("-f,--refs", refs, "View the references")->default_val(false);
    cmd_view->add_flag("-r,--records", records, "View the records")->default_val(false);
    cmd_view->add_option("-c,--cmeta_path", cmeta_path, "CMeta path. If provided, strandness and C-context will be shown as well"
                            )->default_val("");


    cmd_check->add_option("ballcpath", ballc_path, "BAllC file path")->required();

    cmd_merge->add_option("outputpath", out_path, "Output BAllC file path")->required();
    cmd_merge->add_option("ballcpaths", ballc_paths, "Paths of BAllC files to be merged. \
This option is exclusive from [-f,--filelist].")->expected(-1);
    cmd_merge->add_option("-f,--filelist", file_of_path, "File contains the paths to the BAllC files to be merged. \
One path per line. This option is exclusive from [ballcpaths]."
                            )->default_val("");
    cmd_merge->add_option("-c,--chunksize", chunksize, "Chunksize in bp used in merging BAllCs. \
Bigger value may result in slightly faster running speed but larger memory.")->default_val(5000000);

    // Define options for other subcommands here as needed.
    try {
        app.parse(argc, argv);

        // If no subcommand is selected, throw an error.
        if(app.get_subcommands().empty()) {
            throw CLI::CallForHelp();
        }
    } catch(const CLI::ParseError &e) {
        return app.exit(e);
    }

    // Call the appropriate function based on the subcommand chosen.
    if(*cmd_view) {
        routine::ViewBallc(ballc_path.c_str(), header, refs, records, cmeta_path.c_str());
    }
    else if(*cmd_a2ballc) {
        utils::AddSuffixIfNeeded(ballc_path, ".ballc");
        routine::AllCToBallC(allc_path.c_str(), ballc_path.c_str(), chrom_size_path, assembly, header_text, sc);
    }
    else if(*cmd_index) {
        routine::IndexBallc(ballc_path.c_str());
    }
    else if(*cmd_meta) {
        routine::ExtractAllCMeta(fasta_path.c_str(), cmeta_path.c_str());
    }
    else if(*cmd_query) {
        if(cmeta_path==""){
            routine::QueryBallc_Iter(ballc_path.c_str(), grange.c_str());
            // routine::QueryBallc(ballc_path.c_str(), grange.c_str());
        }
        else{
            routine::QueryBallcWithMeta_Iter(ballc_path.c_str(), cmeta_path.c_str(), grange.c_str(), 
                                            q_warn, q_err, q_skip, c_context);
            // routine::QueryBallcWithMeta(ballc_path.c_str(), cmeta_path.c_str(), grange.c_str(), q_warn, q_err, q_skip);
        }
    }
    else if(*cmd_check) {
        routine::CheckBallc(ballc_path.c_str());
    }
    else if(*cmd_merge) {
        utils::AddSuffixIfNeeded(out_path, ".ballc");
        if(file_of_path!="" && ballc_paths.size()>0){
            std::cerr << "Only one of the options between filelist and ballcpaths can be used." << std::endl;
        }
        else if(file_of_path!=""){
            routine::MergeBAllC(file_of_path, out_path, chunksize);
        }
        else if(ballc_paths.size()>0){
            routine::MergeBAllC(ballc_paths, out_path, chunksize);
        }
        else{
            std::cerr << "One of the options between filelist and ballcpaths should be used." << std::endl;
        }
    }

    return 0;
}
