#ifndef SKILL_PARAM   /* Include guard */
#define SKILL_PARAM

#include <vector>

using namespace std;

// MANUAL INSTRUCTIONS
		// omni_mobile update_ga_param to sync with the current edited genome
		// skills, update the param correlated with the edited genome (may need to remove the unnecesary randomness)
		// judferee, update the corresponding fitness function, be it single or multi objectives
		// scripted_command, update the corresponding script for planning the robots on field; also the terminating signal

		// GA_thread, for the desired genome

//

							// include, K_0, K_1, K_2
		// 0.630512 1.986450 4.540391 9.961241 1.772881 1.551234 2.660878 2.029267 7.047945 9.195837 

// 6200 point 1.061739 0.882748 2.576678 6.616718 1.991974 0.400040 1.196844 2.517869 10.522477 7.814570 

vector<double > chase_param 
 		{ 3.730430,3.335420,3.522981,6.909391,1.952976}; 
		// { 4.412019,0.927147,0.086851,5.246892,1.178347 };
		// {2.965322,1.064897,0.614072,5.641337,1.423737}; 
		//{2, 1.2, 1.3, 3, 0.5}; // the current version

vector<double > chase_param_ub{5, 5, 5, 10, 3};
vector<double > chase_param_lb{0, 0, 0, 0, 0};

							// inlcude, K_move, steady_velo_radius, steady_velo_function
vector<double > simple_move2ball_param   
										{10, 3.9, 3.3, 20, 1.2}; // the current version
										// {12.952830,3.642449,4.137686,12.249567,0.637410 };// 

vector<double > simple_move2ball_param_ub{20,   5,   5, 20,   3};
vector<double > simple_move2ball_param_lb{ 3, 2.5,   2,  1, 0.5};

vector<double > pass_param 
						{ 1.675304,1.549873,2.674245 };
						// {0.986995,1.077532,1.497668};
						// {1.241185, 0.957835, 1.407819 };
						// { 0.65, 1.35, 1.47};

vector<double > pass_param_ub{2.5,  3,   3};
vector<double > pass_param_lb{0.3,   1,  1};


vector<double > pass_strategy_param
						{ 0, 0};
vector<double > pass_strategy_param_ub{18, 20};
vector<double > pass_strategy_param_lb{0, 0};

vector<double > shoot_param;




vector<double> target_ub, target_lb;

#endif 