#ifndef GENE_LOAD_SAVE   /* Include guard */
#define GENE_LOAD_SAVE

#include "GeneticUtils.h"
#include "__CHECKSUM__.h"

void log_to_file(string record_file_name, string data)
{
  	ofstream MyLog;
    MyLog.open(record_file_name, ios::app);
    MyLog << data;
    MyLog.close();
}

void clear_file(string file_name){
  std::ofstream ofs;
  ofs.open(file_name, std::ofstream::out | std::ofstream::trunc);
  ofs.close();
}

vector<double> read_file(string file_name){
    std::ifstream ifile(file_name, std::ios::in);
    std::vector<double> return_double;

    //check to see that the file was opened correctly:
    if (!ifile.is_open()) {
        std::cerr << "There was a problem opening the " +file_name + "file!\n";
        exit(1);//exit or do additional error checking
    }

    double num = 0.0;
    //keep storing values from the text file so long as data exists:
    while (ifile >> num) {
        return_double.push_back(num);
    }
    return return_double;
}


bool check_file_exist(const std::string& name){
    return std::filesystem::exists(name);
}


void history_write_new_gen(Town& ghost, int era_id){
    string era_log_file = "..\\live_history\\ERA_" + std::to_string(era_id) + ".txt";
        cerr << "  NOW writing new geneomes data \n";
    if (!(check_file_exist(era_log_file))) assert(0);

        cerr << "  compress town \n";

    vector<FitGene> gen_genomes = ghost.compress_village();

    // GEN + gen_id + NUM_GENOME + num_genome + NUM_VILLAGE + num_village + '\n'
    string generation_data = "GEN " + std::to_string(ghost.num_generation) + " NUM_GENOME " + std::to_string(gen_genomes.size()) + " NUM_VILLAGE " + std::to_string(ghost.numberVillage) + '\n';
    log_to_file(era_log_file, generation_data);

    // THE VALUE
    for (auto fGene : gen_genomes){
        string fitgene_data = "";
        for (auto value: fGene.second.adn)
            fitgene_data = fitgene_data + std::to_string(value) + ' ';
        fitgene_data = fitgene_data + std::to_string(fGene.first) + '\n';
        log_to_file(era_log_file, fitgene_data);
    }
    log_to_file(era_log_file, "FILLING\n");

    // next is value
}


void history_write_new_era(Town& ghost, int era_id, uint32_t current_checksum){
    string old_era_log_file = "..\\live_history\\ERA_" + std::to_string(era_id-1) + ".txt";
    string eoae = "END_OF_AN_ERA\n";
    log_to_file(old_era_log_file, eoae);

        cerr << "                   FINISHING THIS ERA \n";


    string era_log_file = "..\\live_history\\ERA_" + std::to_string(era_id) + ".txt";

    if ((check_file_exist(era_log_file))) {
        // reset data
        clear_file(era_log_file);
    }
    std::ofstream(era_log_file) << "CHECKSUM " << std::to_string(current_checksum) << '\n';

    history_write_new_gen(ghost, era_id);

}

#endif 