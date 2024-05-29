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
vector<double > chase_param  {4.571229,1.157314,4.421715,1.490760,0.321791}; //{2, 1.2, 1.3, 3, 0.5}; // the current version

vector<double > chase_param_ub{5, 5, 5, 10, 3};
vector<double > chase_param_lb{0, 0, 0, 0, 0};

							// inlcude, K_move, steady_velo_radius, steady_velo_function
vector<double > simple_move2ball_param   {10, 3.9, 3.3, 20, 1.2}; // the current version
// {12.952830,3.642449,4.137686,12.249567,0.637410 };// 
vector<double > simple_move2ball_param_lb{ 3, 2.5,   2,  1, 0.5};
vector<double > simple_move2ball_param_ub{20,   5,   5, 20,   3};

vector<double > pass_param {2.083760,1.375789,2.065974};// { 0.65, 1.35, 1.47};

vector<double > pass_param_ub{0.3, 1, 1};
vector<double > pass_param_lb{1, 1.5, 1.5};

vector<double > shoot_param;




vector<double> target_ub, target_lb;

#endif 