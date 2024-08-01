#include "GeneticUtils.h"
#include "__CHECKSUM__.h"
#include "GA_history.h"

#include <webots/Robot.hpp>

#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <cassert>  
#include <time.h>       /* time_t, struct tm, time, localtime */

#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/robot.h>
#include <webots/supervisor.h>

#define FITNESS_MULTIPLE_MEAN 1


#define mems(a, x) memset((a), (x), sizeof(a))


// WbDeviceTag ga_emitter, ga_receiver;


Genome background_color;

int eval_count = 0;
double TIME_STEP;

double fitness_function_single(Genome gene){
  eval_count += 1;
  // marked4func = 0;
  // send signal to judferee


  clear_file("..\\log\\GA_SIGNAL.txt");
  log_to_file("..\\log\\GA_SIGNAL.txt", "1");

  clear_file("..\\log\\GA_RETURN.txt");
  log_to_file("..\\log\\GA_RETURN.txt", "0 ");

  clear_file("..\\log\\GA_GENE.txt");

  // marked4func_genome = gene;
  string str_gene = "";

  for (auto i = 0; i < (int)gene.adn.size(); i++)
    // str_gene += std::to_string(gene.adn[i])
    str_gene = str_gene + std::to_string(gene.adn[i]) + ' ';

  str_gene = str_gene + '\n';
  log_to_file("..\\log\\GA_GENE.txt", str_gene);



  double fitness_return = -MAX_BOUND_FITNESS;

  // reset buffer
  // while (wb_receiver_get_queue_length(ga_receiver) > 0) wb_receiver_next_packet(ga_receiver);
  // // send signal
 //    int data_send[1] = {1};
 //    wb_emitter_send(ga_emitter, data_send, sizeof(int) * 1);
  vector<double> return_signal;
    int counter = 0;
  while (true) {

    if (counter % 100 == 0) {
      // wb_emitter_send(ga_emitter, data_send, sizeof(int) * 1);
      cerr << " WAITING CYCLE END\n";
    }
    return_signal = read_file("..\\log\\GA_RETURN.txt");
    if (return_signal.size() < 2) {
      wb_robot_step(TIME_STEP);
      counter += 1;
      continue;
    }

    // if (wb_receiver_get_queue_length(ga_receiver) == 0)
    // {
    //  wb_robot_step(TIME_STEP);
    //  counter += 1;
    //  counter %= 10;
    //  continue;
    // }

    // const int *message = (int const*)wb_receiver_get_data(ga_receiver);
    // cout << (*message) << " GOT MESS \n";

    // marked4func = (*message);
    // wb_receiver_next_packet(ga_receiver);
    break;
  }

  clear_file("..\\log\\GA_SIGNAL.txt");
  log_to_file("..\\log\\GA_SIGNAL.txt", "0");

  clear_file("..\\log\\GA_RETURN.txt");
  log_to_file("..\\log\\GA_RETURN.txt", "0 ");

  fitness_return = return_signal[1];

  cout << "     CYCLE END: FITNESS = " << fitness_return << '\n';

  log_to_file("..\\log\\GA_LOG.txt", str_gene);
  log_to_file("..\\log\\GA_LOG.txt", std::to_string(fitness_return));
  log_to_file("..\\log\\GA_LOG.txt", "\n");

  return fitness_return;
}


double fitness_function(Genome gene){
  vector<double> multiple_run;
  double sum_fitness = 0;
  int limit = 1;
  if (FITNESS_MULTIPLE_MEAN) limit = 6;

  while (multiple_run.size() < limit)
  {
    multiple_run.push_back(fitness_function_single(gene));
    sum_fitness += multiple_run.back();
  }
  return sum_fitness / multiple_run.size();
}


// double fitness_function(Genome gene){
//  eval_count += 1;
//  return MAX_BOUND_VALUE*gene.adn.size() - (gene - background_color);
// }

// void GENOME_test(int num_village, int num_gen_run, int num_era){
//  for (int i = 0; i < 10; i++)
//    background_color = Genome(simple_move2ball_param_lb, simple_move2ball_param_ub);
//  // background_color = Genome(std::vector<double>{125, 125, 125});
//    std::cout << "BACK COLOR " << background_color << '\n';
//  Town oneshot(num_village, GA_popu_size, simple_move2ball_param_lb, simple_move2ball_param_ub);

//  while (num_era > 0){
//    num_era--;
//    for (int i = 0; i < num_gen_run; i++){
//      if (oneshot.terminate_ok()) break;
//      // cout << " COUNTER " << oneshot.diversity_counter + oneshot.fitness_counter  << '\n';
//      // if (oneshot.diversity_counter + oneshot.fitness_counter > 0) std::cout << "Gen " << oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << '\n';
//      oneshot.town_gen();
//    }
//    cout << " NEW ERA " <<  oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << '\n';
//    cout << " PACK LEADER " << oneshot.village[0].cell[0].second << '\n';
//    oneshot.end_of_an_era();
//  }

//  cout << "EVAL COUNT " << eval_count << '\n';
// }

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

#define MAXIMUM_STORE_VALUE 50
double store_value[MAXIMUM_STORE_VALUE] = {0};

uint32_t get_checksum(int num_village, int num_gen_run, int num_era){
  mems(store_value, MAXIMUM_STORE_VALUE);

  int num_store_value_this_run = 3;
  // get the important value to store here
  store_value[0] = num_village;
  store_value[1] = num_gen_run;
  store_value[2] = num_era;

  num_store_value_this_run += target_ub.size() + target_lb.size();
  for (std::vector<int>::size_type i = 0; i < target_ub.size(); i++)
    store_value[3 + i] = target_ub[i];

  for (std::vector<int>::size_type i = 0; i < target_lb.size(); i++)
    store_value[3 + target_ub.size() + i] = target_lb[i];


  return vector_checksum_crc(store_value, num_store_value_this_run);
}

bool compare_checksum(uint32_t previous_run, uint32_t current_run){
  return fabs(previous_run - current_run) < 0.0001;
}


// CHECKSUM 2673870707
// TIME _20-06-2024-17-41-43
// ERA_NUMGEN 4 FITNESS 9996.864000 DIVERSITY 107.238675
// LEADER 1.735973 1.118648 1.897374 1.576486 2.150994 1.223151 0.870090 2.526435 
// ERA_NUMGEN 4 FITNESS 9996.928000 DIVERSITY 76.802167
// LEADER 1.398816 1.118648 1.806057 2.656941 0.831690 1.223151 1.128117 2.526435 
// ERA_NUMGEN 4 FITNESS 9996.928000 DIVERSITY 34.654562
// LEADER 1.411771 0.819657 1.806057 2.690440 1.271997 1.165333 1.098171 2.645536 
// ERA_NUMGEN 4 FITNESS 9996.928000 DIVERSITY 44.264898
// LEADER 1.897564 1.720789 2.926071 2.690440 0.010987 0.558842 1.344035 2.749182 
// ERA_NUMGEN 0 FITNESS 9996.928000 DIVERSITY 30.755423
// LEADER 4.351634 0.392315 0.387585 4.847560 2.175726 0.935954 1.096408 1.258156 
// ERA_NUMGEN 0 FITNESS 4994.176000 with Div 70.679377
// LEADER 2.965322 1.064897 0.614072 5.641337 1.423737 1.241185 0.957835 1.407819 
// ERA_NUMGEN 5 FITNESS 4994.176000 DIVERSITY 145.760033
// LEADER 4.413892 0.900906 0.079501 4.570452 1.175481 0.997287 1.077181 1.497543 
// ERA_NUMGEN 3 FITNESS 4994.400000 DIVERSITY 74.714629
// LEADER 4.412019 0.927147 0.086851 5.246892 1.178347 0.986995 1.077532 1.497668 



pair<int, FitGene> check_memory(uint32_t current_checksum){

    int count_era = 0; Genome blank_gene; double blank_fit = 0;
    pair<int, FitGene> blank_state = make_pair(-1, FitGene(0, blank_gene));

  std::ifstream inFile("..\\log\\GA_LOG_VIP.txt");
  std::string line;
  if (!(std::getline(inFile, line))) return blank_state;
  // the 1st line contain the CHECKSUM
    std::istringstream iss(line);
    string tagger; uint32_t last_checksum;

    if (!(iss >> tagger >> last_checksum)) { return blank_state; } // error
    if (tagger.find("CHECKSUM") == std::string::npos) {return blank_state; }

      cout << "mem_checksum " << last_checksum << '\n';
    if (!(compare_checksum(last_checksum, current_checksum))) {return blank_state; }

    // MATCH CHECKSUM, begin merging data
      std::getline(inFile, line); // read time
      cout << " READ GENE \n"; 
    while (std::getline(inFile, line))
  {
    // 1st line have numgen double fitness double diversity double
    std::istringstream fss(line);
    fss >> tagger >> blank_fit;
    fss >> tagger >> blank_fit;

    // 2nd line have leader genome adn
    std::getline(inFile, line);

    std::istringstream gss(line);
    gss >> tagger;

    std::vector<double> imported_adn;
    double adn_t;
    while (gss >> adn_t) imported_adn.push_back(adn_t);

    cout << blank_fit << " fitgene " << imported_adn.size() << '\n';

    blank_gene = Genome(imported_adn);
    // import value to genome
    count_era++;
  }

  return make_pair(count_era, FitGene(-MAX_BOUND_FITNESS, blank_gene));

  // return make_pair(count_era, FitGene(blank_fit, blank_gene));
}


#include <sys/stat.h>
#include <unistd.h>
uint32_t current_checksum;

bool check_content(const std::string& name){
  // first line have CHECKSUM, omit
  // then log record of each generation in that era
  // with the form:
  //    GEN + gen_id + NUM_GENOME + num_genome + NUM_VILLAGE + num_village + '\n'
  // then the followings num_genome with its initial value (could have NULL value) at construction
  //    Genome ADN Genome Initial Value
  // this is must have
  //    FILLING
  // then the followings num_genome with its filled value
  //    Genome ADN Genome FILLED value
  // after check that all of the genomes has been tested and filled its value
  //    BEST_ADN ADN
  //    THIS_GEN DIVERSITY diver_value FITNESS fitness_value 

  // LOOP this until break or reach all generation, output era info -> LOG_VIP
  //    END_OF_AN_ERA

  // the process writes/reads all the genome infos IMMEDIATELY after construction.
  // the FILLING process is DISCONTINUOUS

  // CHECK CONTENT if all of the things has been filled then return True

    std::ifstream inFile(name);
    std::string line;

    if (!(std::getline(inFile, line))) return 0;

      // Genome blank_gene; double blank_fit = 0;

      // pair<int, FitGene> blank_state = make_pair(0, FitGene(0, blank_gene));

    // the 1st line contain the CHECKSUM
      std::istringstream iss(line);
      string tagger[5]; uint32_t history_era_checksum;

      if (!(iss >> tagger[0] >> history_era_checksum) or (tagger[0].find("CHECKSUM") == std::string::npos) 
       or (!(compare_checksum(history_era_checksum, current_checksum))) ) { 
        clear_file(name);
        std::ofstream(name) << "CHECKSUM " << std::to_string(current_checksum) << '\n';
        return 0; 
      } // error

      // MATCH CHECKSUM, begin CHECKING data

      while (std::getline(inFile, line))
    {
      // CHECK 1st line info, or it will contain 
      iss.clear(); iss.str(line);
      if (!(iss >> tagger[0])) assert(0);
      if ( !(tagger[0].find("END_OF_AN_ERA") == std::string::npos) )
        return 1; // CHECK TO THE END OF THE ERA
      if ( (tagger[0].find("GEN") == std::string::npos) ) assert(0);

      int tmp; int num_genome, num_village;
      iss >> tmp >> tagger[1] >> num_genome >> tagger[2] >> num_village;

      assert(num_genome % num_village == 0);
      
      for (int i = 0; i < num_genome; i++){
        if (!(std::getline(inFile, line))) assert(0);
        // get the history genome
      }
      if (!(std::getline(inFile, line))) assert(0); // the FILLING line
      for (int i = 0; i < num_genome; i++){
        if (!(std::getline(inFile, line))) return 0; // still filling
      }
      if (!(std::getline(inFile, line))) assert(0); // the best adn line
      if (!(std::getline(inFile, line))) assert(0); // the gen stat line
    }
  return 0;
}

int check_historia_era(int assumed_current_era_id){
  for (int i = 0; i <= assumed_current_era_id; i++)
  {
    string era_log_file = "..\\live_history\\ERA_" + std::to_string(i) + ".txt";
    if (!(check_file_exist(era_log_file))) {
      // create file
      std::ofstream(era_log_file) << "CHECKSUM " << std::to_string(current_checksum) << '\n';
      return i;
    }

    if (!(check_content(era_log_file)) && i != assumed_current_era_id)
      return i;
  }
  return assumed_current_era_id;
}

int history_counter = -1;

void fill_processing_era(int assumed_current_era_id, Town& ghost){

  // now modifying the oneshot_town, filled it with modified value of villagers
// ONLY RUN ONCE
    string name =  "..\\live_history\\ERA_" + std::to_string(assumed_current_era_id) + ".txt";
    std::ifstream inFile(name);
    std::string line;

    if (!(std::getline(inFile, line))) return;

      // Genome blank_gene; double blank_fit = 0;

      // pair<int, FitGene> blank_state = make_pair(0, FitGene(0, blank_gene));

    // the 1st line contain the CHECKSUM
      std::istringstream iss(line);
      string tagger[5]; uint32_t history_era_checksum;

      if (!(iss >> tagger[0] >> history_era_checksum) or (tagger[0].find("CHECKSUM") == std::string::npos) 
       or (!(compare_checksum(history_era_checksum, current_checksum))) ) { 
        clear_file(name);
        std::ofstream(name) << "CHECKSUM " << std::to_string(current_checksum) << '\n';
        return; 
      } // error

        log_to_file("..\\live_history\\event_log.txt", " checksum pass --------------------\n");

      // MATCH CHECKSUM, begin CHECKING data

    while (std::getline(inFile, line))
    {
      // CHECK 1st line info, or it will contain EOAE
      iss.clear(); iss.str(line); 
      if (!(iss >> tagger[0])) assert(0);
      if ( (tagger[0].find("GEN") == std::string::npos) ) assert(0);

          log_to_file("..\\live_history\\event_log.txt", " header pass --------------------\n");


      int num_generation; int num_genome, num_village;
      iss >> num_generation >> tagger[1] >> num_genome >> tagger[2] >> num_village;

      assert(num_genome % num_village == 0);
      
          log_to_file("..\\live_history\\event_log.txt", " module pass --------------------\n");
          cerr << " CHECK LINE " << line << '\n';
          cerr << " CHECK TAGGER " << tagger[0] << ' ' << tagger[1] << ' ' << tagger[2]  << '\n';
          cerr << "  NOW READING " << num_generation << " generation with " << num_genome << " genomes of " << num_village << " villages " << '\n';
          history_counter = -1;
      vector<FitGene> saved_gen_genomes;

      for (int i = 0; i < num_genome; i++){
        if (!(std::getline(inFile, line))) assert(0);
        // get the history genome
        iss.clear(); iss.str(line); 

        std::vector<double> imported_adn; double adn_t;
        while (iss >> adn_t) imported_adn.push_back(adn_t);
        double initialFitnessValue = imported_adn.back(); imported_adn.pop_back();

        // fillinto the vector<FitGene>
        saved_gen_genomes.push_back(make_pair(initialFitnessValue, Genome( imported_adn )));
      }
      // now import the value into ghost town

      if (!(std::getline(inFile, line))) assert(0); // the FILLING line

      for (int i = 0; i < num_genome; i++){
        // THIS IS TBA
        if (!(std::getline(inFile, line))) break; // still filling

        // get the history genome
        iss.clear(); iss.str(line); 

        std::vector<double> imported_adn; double adn_t;
        while (iss >> adn_t) imported_adn.push_back(adn_t);
        double filledFitnessValue = imported_adn.back(); imported_adn.pop_back();

        saved_gen_genomes[i].first = filledFitnessValue;
        history_counter ++;
      }

        cerr << " NOW LOADING this generation TO town \n";

      ghost.history_town_load(num_generation, num_village, saved_gen_genomes);
      bool x =  ghost.terminate_ok(50);
      cout << " TOWN TERMINATE " << x << '\n';

        cerr << " DONE LOADING this generation \n";
      if (!(std::getline(inFile, line))) assert(0); // the best adn line
      if (!(std::getline(inFile, line))) assert(0); // the gen stat line
    }
  return ;
}

void live_filling_value(Town &ghost, int h_counter, int which_era){

  string name =  "..\\live_history\\ERA_" + std::to_string(which_era) + ".txt";

  vector<FitGene> gen_genomes = ghost.compress_village();
  for (int i = 0; i <= h_counter; i++)
    assert(gen_genomes.size() > i and gen_genomes[i].first > -MAX_BOUND_FITNESS);

  for (int i = h_counter + 1; i < int(gen_genomes.size()); i++){
      if (gen_genomes[i].first - 1  < -MAX_BOUND_FITNESS){
        gen_genomes[i].first = fitness_function(gen_genomes[i].second);
      }
      string FitGeneData = "";
      for (auto aptx : gen_genomes[i].second.adn) FitGeneData = FitGeneData + std::to_string(aptx) + ' ';
      FitGeneData = FitGeneData + std::to_string(gen_genomes[i].first) + '\n';
      log_to_file(name, FitGeneData);
      //  write to TBA
    }
  ghost.live_genome_load(gen_genomes);
}

void GA_RUN(int num_village, int num_gen_run, int num_era){

  clear_file("..\\live_history\\event_log.txt");

  bool RANDOM_RUN = 0;

  vector<double> judferee_param = read_file("..\\log\\JUDFEREE_PARAM.txt");
  RANDOM_RUN = bool(judferee_param[1]);


  current_checksum = get_checksum(num_village, num_gen_run, num_era);
    cout << "current_checksum " << current_checksum << '\n';

  pair<int, FitGene> return_check = check_memory(current_checksum);

  // cout << get_file_timestamp() << '\n';

  cout << " READING MEMORY ---------------  " << return_check.first << '\n';

  // CHECK SUM memory 
  if (return_check.first == -1) {
    cout << "NEW MEMORY ----------------\n";

    log_to_file("..\\live_history\\event_log.txt", "NEW MEMORY --------------------\n");


    clear_file("..\\log\\GA_LOG_VIP.txt");
    log_to_file("..\\log\\GA_LOG_VIP.txt", "CHECKSUM " + std::to_string(get_checksum(num_village, num_gen_run, num_era)) + "\n" );
    log_to_file("..\\log\\GA_LOG_VIP.txt", "TIME " + get_file_timestamp() + "\n" );

    clear_file("..\\log\\GA_LOG.txt");
    return_check.first = 0;
  }

  log_to_file("..\\live_history\\event_log.txt", "CHECK OLD MEMORY --------------------\n");

  int this_era = check_historia_era(return_check.first);
  cout << " CURRENT ERA " << this_era << '\n';

  Town ghost(num_village, GA_popu_size, target_lb, target_ub, RANDOM_RUN, 'T');

  log_to_file("..\\live_history\\event_log.txt", "IMPORT OLD MEMORY --------------------\n");
  fill_processing_era(this_era, ghost);

  log_to_file("..\\live_history\\event_log.txt", "DONE OLD MEMORY --------------------\n");


  clear_file("..\\log\\GA_ENV.txt");
  log_to_file("..\\log\\GA_ENV.txt", std::to_string(this_era) + '\n');

  if (ghost.village.size() == 0){
    // INITIAL, would be a function

    log_to_file("..\\live_history\\event_log.txt", "WRITE NEW MEMORY --------------------\n");

    ghost.suppress_gen();
    history_write_new_gen(ghost, this_era);
  }
  // Town oneshot(num_village, GA_popu_size, target_lb, target_ub, RANDOM_RUN);

  // if (this_era > 0)
  // {
  //   cout << "IMPORTED MEMORY --------------------\n";
  //   log_to_file("..\\live_history\\event_log.txt", "IMPORTING MEMORY --------------------\n");
  //   // import this genome as the last survivor
  // }

    cerr << " NOW PROCESSING ERA " << ghost.num_generation << '\n';

  while (this_era < num_era){
    for (int i = ghost.num_generation; i < num_gen_run; i++){

        cerr << "FILLING FROM INDEX " << history_counter << '\n';
        log_to_file("..\\live_history\\event_log.txt", "FILLING FITNESS -------------------- of gen " + std::to_string(i) + '\n');

      live_filling_value(ghost, history_counter, this_era);
      history_counter = -1;
      ghost.town_sort();
      ghost.village_sort();

      // cout << " COUNTER " << ghost.diversity_counter + ghost.fitness_counter  << '\n';
      // if (ghost.diversity_counter + ghost.fitness_counter > 0) 
      // std::cout << "Gen " << ghost.num_generation << " has best " << ghost.town_fitness_best_gen() << " with Div " << ghost.town_diversity_get() << " VILLAGER LEADER " << ghost.village[0].cell[0].second  << '\n';

      string gen_result = "Gen " + std::to_string(ghost.num_generation) + " FITNESS " + std::to_string(ghost.town_fitness_best_gen()) + " DIVERSITY " + std::to_string(ghost.town_diversity_get()) + "\n";

      int id_max = 0;      
      string gen_leader = " VILLAGER_LEADER F " + std::to_string(ghost.village[id_max].cell[0].first) + ' ';
      for (auto j = 0; j < (int)ghost.village[id_max].cell[0].second.adn.size(); j++)
        gen_leader = gen_leader + std::to_string(ghost.village[id_max].cell[0].second.adn[j]) + " ";
      gen_leader = gen_leader + "\n";

      log_to_file("..\\log\\GA_LOG.txt", gen_result);
      log_to_file("..\\log\\GA_LOG.txt", gen_leader);

      string era_name =  "..\\live_history\\ERA_" + std::to_string(this_era) + ".txt";
      log_to_file(era_name, gen_result);
      log_to_file(era_name, gen_leader);

        log_to_file("..\\live_history\\event_log.txt", "GEN " + std::to_string(i) + " complete " + '\n');

      if (ghost.terminate_ok(num_gen_run)) break;

        if (i + 1 < num_gen_run) {
          ghost.town_gen(1);
          history_write_new_gen(ghost, this_era);
        }
    }
    // cout << " NEW ERA " <<  ghost.num_generation << " has best " << ghost.town_fitness_best_gen() << " with Div " << ghost.town_diversity_get() << '\n';
    // cout << " PACK LEADER " << ghost.village[0].cell[0].second << '\n';

    string gen_result = "ERA_NUMGEN " + std::to_string(num_era) + " FITNESS " + std::to_string(ghost.town_fitness_best_gen()) + " DIVERSITY " + std::to_string(ghost.town_diversity_get()) + "\n";
    string gen_leader = "LEADER ";
    for (auto j = 0; j < (int)ghost.village[0].cell[0].second.adn.size(); j++)
      gen_leader = gen_leader + std::to_string(ghost.village[0].cell[0].second.adn[j]) + " ";
    gen_leader = gen_leader + '\n';

    log_to_file("..\\log\\GA_LOG_VIP.txt", gen_result);
    log_to_file("..\\log\\GA_LOG_VIP.txt", gen_leader);

        log_to_file("..\\live_history\\event_log.txt", " NOW TRANSITING TO NEXT ERA \n" );

    ghost.end_of_an_era(ghost.village[0].cell[0], 1);

      this_era ++;
    if (RANDOM_RUN){
      clear_file("..\\log\\GA_ENV.txt");
      log_to_file("..\\log\\GA_ENV.txt", std::to_string(this_era));
    }
        log_to_file("..\\live_history\\event_log.txt", " NOW WRITING \n");

      history_write_new_era(ghost, this_era, current_checksum);
  }
  clear_file("..\\log\\GA_ENV.txt");
  log_to_file("..\\log\\GA_ENV.txt", 0);

  cout << "EVAL COUNT " << eval_count << '\n';
}



// 5 20 12
int main(int argc, char **argv){
  srand(time(0));
  wb_robot_init();

  const int temp = wb_robot_get_basic_time_step();
  TIME_STEP = temp;

  target_ub.insert(target_ub.end(), chase_param_ub.begin(), chase_param_ub.end());
  target_ub.insert(target_ub.end(), pass_param_ub.begin(), pass_param_ub.end());
  // target_ub.insert(target_ub.end(), simple_move2ball_param_ub.begin(), simple_move2ball_param_ub.end());

  // target_ub.insert(target_ub.end(), pass_strategy_param_ub.begin(), pass_strategy_param_ub.end());
  // target_ub.insert(target_ub.end(), shoot_strategy_param_ub.begin(), shoot_strategy_param_ub.end());

  target_lb.insert(target_lb.end(), chase_param_lb.begin(), chase_param_lb.end());
  target_lb.insert(target_lb.end(), pass_param_lb.begin(), pass_param_lb.end());
  // target_lb.insert(target_lb.end(), simple_move2ball_param_lb.begin(), simple_move2ball_param_lb.end());

  // target_lb.insert(target_lb.end(), pass_strategy_param_lb.begin(), pass_strategy_param_lb.end());
  // target_lb.insert(target_lb.end(), shoot_strategy_param_lb.begin(), shoot_strategy_param_lb.end());


  // ga_emitter = wb_robot_get_device("ga_emitter");
  // ga_receiver = wb_robot_get_device("ga_receiver");
  // wb_receiver_enable(ga_receiver, TIME_STEP);

  GA_RUN(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]));
  return 0;
}
