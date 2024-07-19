#ifndef FILE_MANI_H
#define FILE_MANI_H

#include <VARIABLE.h>


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

std::string get_pre_set(bool X = 0){
  string pre_set;
  int num_robot = 0;
  for (int i = 0; i < ROBOTS; i++)
    if (!missing_player[i]) num_robot++;

  // if (num_robot == 4) pre_set = "A_" ;
  // else if (num_robot == 10) pre_set = "B_";
  // else if (num_robot == ROBOTS) pre_set = "C_";
  // else assert(false);
  pre_set =  std::to_string(num_robot/2) + "v" + std::to_string(num_robot/2) + "_";
  if      (CURRENT_BRAIN_LEVEL == 0)  pre_set = pre_set + "L0_";
  else if (CURRENT_BRAIN_LEVEL == 1)  pre_set = pre_set + "L1_";
  else if (CURRENT_BRAIN_LEVEL == 2)  pre_set = pre_set + "L2_";
  else if (CURRENT_BRAIN_LEVEL == 3)  pre_set = pre_set + "L3_";
  else assert(false);
  if (X) return pre_set + "SETUP";
  return pre_set;
}


std::string get_string_id(){
  int num_red = 0, num_blue = 0;
  for (int i = 0; i < 7; i++)
    if (!missing_player[i]) num_blue++;
  for (int i = 7; i < ROBOTS; i++)
    if (!missing_player[i]) num_red++;
  string mid_fix = std::to_string(num_red)+"R&"+std::to_string(num_blue)+"B";
  return "_"+mid_fix;
}

std::string get_file_timestamp(int mode = 0)
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  if (mode == 0) strftime(buffer,sizeof(buffer),"_%d-%m-%Y-%H-%M-%S",timeinfo);
  else if (mode==1) strftime(buffer,sizeof(buffer),"_%d-%m-%Y",timeinfo);
  else assert(false);
  return std::string(buffer);
}

ofstream MyFile;

void write_to_file(string record_file_name, string data)
{
    if (!LOG_MODE) return;
    string suffix = get_file_timestamp(1);
    string midfix = get_string_id();
    string prefix = get_pre_set();
    MyFile.open(prefix+record_file_name+midfix+suffix+".txt", ios::app);
    MyFile << data;
    // for (i = 0; i < ROBOTS; i++) {
    //   if (missing_player[i]) continue;
    //   MyFile << robot_name[i] << " pos ";
    //   for (j = 0; j < 3; j++)
    //     MyFile <<  player_position[i][j] << " ";

    //   MyFile << " rot ";
    //   for (j = 0; j < 4; j++)
    //     MyFile <<  player_rotation[i][j] << " ";

    //   MyFile << '\n';
    // }
    MyFile.close();
}

#endif
