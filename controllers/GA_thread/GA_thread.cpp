#include "GeneticUtils.h"
#include <webots/Robot.hpp>

#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <cassert>  

#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/robot.h>
#include <webots/supervisor.h>

// WbDeviceTag ga_emitter, ga_receiver;


Genome background_color;

int eval_count = 0;
double MAX_BOUND_VALUE = 1000;
double TIME_STEP;

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

double fitness_function(Genome gene){
	eval_count += 1;
	// marked4func = 0;
	// send signal to judferee


	clear_file("..\\log\\GA_SIGNAL.txt");
	log_to_file("..\\log\\GA_SIGNAL.txt", "1");

	clear_file("..\\log\\GA_RETURN.txt");
	log_to_file("..\\log\\GA_RETURN.txt", "0 ");

	clear_file("..\\log\\GA_GENE.txt");

	// marked4func_genome = gene;
	// string str_gene = "";

	for (auto i = 0; i < (int)gene.adn.size(); i++){
		// simple_move2ball_param[i] = gene.adn[i];
		// str_gene += std::to_string(gene.adn[i])
		string single_gene = std::to_string(gene.adn[i]) + " ";
		log_to_file("..\\log\\GA_GENE.txt", single_gene);
		log_to_file("..\\log\\GA_LOG.txt", single_gene);
		// cout << "TEST ADN " << gene.adn[i] << '\n';
	}
	log_to_file("..\\log\\GA_GENE.txt", "\n");
	log_to_file("..\\log\\GA_LOG.txt", "\n");



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
		// 	wb_robot_step(TIME_STEP);
		// 	counter += 1;
		// 	counter %= 10;
		// 	continue;
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

	log_to_file("..\\log\\GA_LOG.txt", std::to_string(fitness_return));
	log_to_file("..\\log\\GA_LOG.txt", "\n");

	return fitness_return;
}

// double fitness_function(Genome gene){
// 	eval_count += 1;
// 	return MAX_BOUND_VALUE*gene.adn.size() - (gene - background_color);
// }

// void GENOME_test(int num_village, int num_gen_run, int num_era){
// 	for (int i = 0; i < 10; i++)
// 		background_color = Genome(simple_move2ball_param_lb, simple_move2ball_param_ub);
// 	// background_color = Genome(std::vector<double>{125, 125, 125});
// 		std::cout << "BACK COLOR " << background_color << '\n';
// 	Town oneshot(num_village, GA_popu_size, simple_move2ball_param_lb, simple_move2ball_param_ub);

// 	while (num_era > 0){
// 		num_era--;
// 		for (int i = 0; i < num_gen_run; i++){
// 			if (oneshot.terminate_ok()) break;
// 			// cout << " COUNTER " << oneshot.diversity_counter + oneshot.fitness_counter  << '\n';
// 			// if (oneshot.diversity_counter + oneshot.fitness_counter > 0) std::cout << "Gen " << oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << '\n';
// 			oneshot.town_gen();
// 		}
// 		cout << " NEW ERA " <<  oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << '\n';
// 		cout << " PACK LEADER " << oneshot.village[0].cell[0].second << '\n';
// 		oneshot.end_of_an_era();
// 	}

// 	cout << "EVAL COUNT " << eval_count << '\n';
// }


void GA_RUN(int num_village, int num_gen_run, int num_era){
	Town oneshot(num_village, GA_popu_size, target_lb, target_ub);

	clear_file("..\\log\\GA_LOG.txt");

	int env_count = 0;
	// clear_file("..\\log\\GA_ENV.txt");
	// log_to_file("..\\log\\GA_ENV.txt", 0);

	while (num_era > 0){
		num_era--;
		for (int i = 0; i < num_gen_run; i++){
			if (oneshot.terminate_ok()) break;
			// cout << " COUNTER " << oneshot.diversity_counter + oneshot.fitness_counter  << '\n';
			// if (oneshot.diversity_counter + oneshot.fitness_counter > 0) 
			// std::cout << "Gen " << oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << " VILLAGER LEADER " << oneshot.village[0].cell[0].second  << '\n';

			string gen_result = "Gen " + std::to_string(oneshot.num_generation) + " has best " + std::to_string(oneshot.town_fitness_best_gen()) + " with Div " + std::to_string(oneshot.town_diversity_get()) + "\n";
			string gen_leader = " VILLAGER_LEADER ";
			for (auto j = 0; j < (int)oneshot.village[0].cell[0].second.adn.size(); j++)
				gen_leader = gen_leader + std::to_string(oneshot.village[0].cell[0].second.adn[j]) + " ";
			gen_leader = gen_leader + '\n';

			log_to_file("..\\log\\GA_LOG.txt", gen_result);
			log_to_file("..\\log\\GA_LOG.txt", gen_leader);

			oneshot.town_gen();
		}
		// cout << " NEW ERA " <<  oneshot.num_generation << " has best " << oneshot.town_fitness_best_gen() << " with Div " << oneshot.town_diversity_get() << '\n';
		// cout << " PACK LEADER " << oneshot.village[0].cell[0].second << '\n';

		string gen_result = "NEW_ERA " + std::to_string(oneshot.num_generation) + " has best " + std::to_string(oneshot.town_fitness_best_gen()) + " with Div " + std::to_string(oneshot.town_diversity_get()) + "\n";
		string gen_leader = " ERA_LEADER ";
		for (auto j = 0; j < (int)oneshot.village[0].cell[0].second.adn.size(); j++)
			gen_leader = gen_leader + std::to_string(oneshot.village[0].cell[0].second.adn[j]) + " ";
		gen_leader = gen_leader + '\n';

		log_to_file("..\\log\\GA_LOG.txt", gen_result);
		log_to_file("..\\log\\GA_LOG.txt", gen_leader);

		env_count += 1;
		clear_file("..\\log\\GA_ENV.txt");
		log_to_file("..\\log\\GA_ENV.txt", std::to_string(env_count));
		oneshot.end_of_an_era();
	}
	clear_file("..\\log\\GA_ENV.txt");
	log_to_file("..\\log\\GA_ENV.txt", 0);

	cout << "EVAL COUNT " << eval_count << '\n';
}



// 5 100 2
int main(int argc, char **argv){
	srand(time(0));
	wb_robot_init();

	const int temp = wb_robot_get_basic_time_step();
	TIME_STEP = temp;

	target_ub.insert(target_ub.end(), chase_param_ub.begin(), chase_param_ub.end());
	target_ub.insert(target_ub.end(), pass_param_ub.begin(), pass_param_ub.end());

	target_lb.insert(target_lb.end(), chase_param_lb.begin(), chase_param_lb.end());
	target_lb.insert(target_lb.end(), pass_param_lb.begin(), pass_param_lb.end());

	// ga_emitter = wb_robot_get_device("ga_emitter");
	// ga_receiver = wb_robot_get_device("ga_receiver");
	// wb_receiver_enable(ga_receiver, TIME_STEP);

	GA_RUN(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]));
	return 0;
}
