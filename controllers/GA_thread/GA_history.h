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


void Village::history_gen_write(string file_name)
{
	;
}

void Town::history_town_load(string file_name)
{
	;
}

void Town::history_write_new_era(int era_id){
	;
}

#endif 