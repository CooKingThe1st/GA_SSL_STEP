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

//  4.763024 4.669122 4.986267 9.167455 2.088656 1.939644 1.389263 2.825129 1.204138 9.816039    6520

// 4.763024 4.731375 4.986267 9.167455 2.088656 1.725733 1.389263 2.613269 1.204138 2.979699
// 4.763024 4.669122 4.986267 9.167455 2.088656 1.939644 1.389263 2.825129 1.204138 5.745497 
// 3.434419 3.547634 3.031460 9.870401 2.018959 1.290854 1.208662 2.587026 11.348648 5.045196

vector<double > chase_param 
		// {4.763024,4.731375,4.986267,9.167455,2.088656};
		// {3.434419,3.547634,3.031460,9.870401,2.018959};
 		{ 3.730430,3.335420,3.522981,6.909391,1.952976}; 
		// {2, 1.2, 1.3, 3, 0.5}; // the current version

vector<double > chase_param_ub{5, 5, 5, 10, 3};
vector<double > chase_param_lb{0, 0, 0, 0, 0};

							// inlcude, K_move, steady_velo_radius, steady_velo_function
vector<double > simple_move2ball_param   
										{10, 3.9, 3.3, 20, 1.2}; // the current version
										// {12.952830,3.642449,4.137686,12.249567,0.637410 };// 

vector<double > simple_move2ball_param_ub{20,   5,   5, 20,   3};
vector<double > simple_move2ball_param_lb{ 3, 2.5,   2,  1, 0.5};

vector<double > pass_param 
						// {  1.725733, 1.389263, 2.613269, 1.2};
						// {1.290854,1.208662,2.587026, 1.2};
						{ 1.675304, 1.549873 , 2.674245, 1.2 };
						// {1.241185, 0.957835, 1.407819, 1.2};
						// { 0.65, 1.35, 1.47, 1.2};

vector<double > pass_param_ub{2.5,    5,    5,   3};
vector<double > pass_param_lb{0.3,  0.5,  0.5,   0.3};




vector<double > pass_strategy_param
						// {1.204138,2.979699};
						// {11.348648,5.045196};
						{ 10, 5};
vector<double > pass_strategy_param_ub{30, 30};
vector<double > pass_strategy_param_lb{0, 0};



vector<double > shoot_strategy_param
						{20, 10};
					// 70, 20, negative sign
vector<double > shoot_strategy_param_ub{50, 20};
vector<double > shoot_strategy_param_lb{ 0,  0};



vector<double> target_ub, target_lb;

#endif 