#ifndef SKILL_PARAM   /* Include guard */
#define SKILL_PARAM

#include <vector>

using namespace std;


double 
							// include, K_0, K_1, K_2
vector<double > chase_param {2, 1.2, 1.3}; // the current version
vector<double > chase_param_ub{5, 5, 5};
vector<double > chase_param_lb{0, 0, 0};

							// inlcude, K_move, steady_velo_radius, steady_velo_function
vector<double > simple_move2ball_param   {10, 3.9, 3.3, 20, 1.2}; // the current version
vector<double > simple_move2ball_param_lb{ 3, 2.5,   2,  1, 0.5};
vector<double > simple_move2ball_param_ub{20,   5,   5, 20,   3};

vector<double > pass_param;
vector<double > shoot_param;



#endif 