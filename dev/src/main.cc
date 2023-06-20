#include "CLI11.hpp"
#include "routines.h"
#include <iostream>

#include <string>

int main(int argc, char **argv) {
    CLI::App app{"BAllCools: Binary AllC file tools"};

    // Adding subcommands
    CLI::App* cmd_view = app.add_subcommand("view", "View data stored in a BAllC file.");
    CLI::App* cmd_index = app.add_subcommand("index", "Index a BAllC file.");
    CLI::App* cmd_a2ballc = app.add_subcommand("a2b,allc-to-ballc", "Convert an AllC file to a BAllc file.");
    CLI::App* cmd_meta = app.add_subcommand("meta", "Extract and index C from a genome sequence file (fasta) and store in a CMeta file (bed format).");
    CLI::App* cmd_query = app.add_subcommand("query", "Query info from a BAllC file");

    // Defining options for each subcommand
    std::string allc_path, ballc_path, chrom_size_path, assembly, header_text;
    std::string fasta_path, cmeta_path;
    std::string grange;
    bool sc, q_warn, q_err, q_skip;



    cmd_a2ballc->add_option("allcpath", allc_path, "AllC file path")->required();
    cmd_a2ballc->add_option("ballcpath", ballc_path, "BAllC file path")->required();
    cmd_a2ballc->add_option("chrompath", chrom_size_path, "Path to the chromosome size file or the .fai file")->required();
    cmd_a2ballc->add_option("-s,--sc", sc, "Whether or not to convernt the AllC file to the single-cell (sc) BAllC format. \
uint8 data type is used to store methylation info in sc BAllC format, while uint16 is used in non-sc format, \
resulting in smaller disk usage for sc BAllC format. \
However, converting a non-sc AllC file to a sc BAllC file format may result in broken data in the BAllC file. \
Default: true")->default_val(true);
    cmd_a2ballc->add_option("-a,--assembly_info", assembly, "Info of the assembly used in the AllC file. eg, mm10, hg38, hg38-donor1, etc. Default: \"\""
                            )->default_val("");
    cmd_a2ballc->add_option("-n,--note", header_text, "Note related to the AllC file. Default: \"\""
                            )->default_val("");


    cmd_index->add_option("ballcpath", ballc_path, "BAllC file path")->required();

    cmd_meta->add_option("fastapath", fasta_path, "Genome FASTA file path")->required();
    cmd_meta->add_option("cmetapath", cmeta_path, "Output path")->required();

    cmd_query->add_option("ballcpath", ballc_path, "BAllC file path")->required();
    cmd_query->add_option("genomerange", grange, "Genome range of interests. Supported formats are chrX, chrX:\
    chrX:XXX-XXX, chrX:-XXX, chrX:XXX-")->required();
    cmd_query->add_option("-c,--cmeta_path", cmeta_path, "CMeta path. If provided, strandness and C-context will be shown as well"
                            )->default_val("");
    cmd_query->add_option("-w,--warn_mismatch", q_warn, "Warning message will be displayed if mismatches detected between the BAllC file and the CMeta.\
The program WILL NOT halt. Default: false."
                            )->default_val(false);
    cmd_query->add_option("-e,--err_mismatch", q_err, "Runtime error will be caused if mismatches detected between the BAllC file and the CMeta.\
The program WILL halt. Default: false."
                            )->default_val(false);
    cmd_query->add_option("-s,--skip_mismatch", q_skip, "Skip displaying the C record which has mismatch between the BAllC file and the CMeta.\
If not skip, the strandness and the C-context of the C will be displayed as \"?\tC??\". Default: truee."
                            )->default_val(true);


    cmd_view->add_option("-s,--skip_mismatch", q_skip, "Skip displaying the C record which has mismatch between the BAllC file and the CMeta.\
If not skip, the strandness and the C-context of the C will be displayed as \"?\tC??\". Default: truee."
                            )->default_val(true);

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
        // cmd_viewaram1, a_param2);
    }
    else if(*cmd_a2ballc) {
        AllcToBallc(allc_path.c_str(), ballc_path.c_str(), chrom_size_path, assembly, header_text, sc);
    }
    else if(*cmd_index) {
        IndexBallc(ballc_path.c_str());
    }
    else if(*cmd_meta) {
        ExtractAllCMeta(fasta_path.c_str(), cmeta_path.c_str());
    }
    else if(*cmd_query) {
        if(cmeta_path==""){
            QueryBallc(ballc_path.c_str(), grange.c_str());
        }
        else{
            std::cout << q_warn << "\n";
            std::cout << q_err << "\n";
            std::cout << q_skip << "\n";
            QueryBallcWithMeta(ballc_path.c_str(), cmeta_path.c_str(), grange.c_str(), q_warn, q_err, q_skip);
        }
    }

    return 0;
}
