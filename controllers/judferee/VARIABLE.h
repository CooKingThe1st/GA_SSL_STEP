#ifndef VAR_H
#define VAR_H

#include <string.h>
#include <cassert>  

#include <webots/display.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/robot.h>
#include <webots/supervisor.h>

#ifdef __linux__ 
	#include "../omni_mobile/geometry.h"
#elif _WIN32
	#include "..\omni_mobile\geometry.h"
#else

#endif


// SYSTEM ARG

bool MANUAL_MODE = 1;
bool CHRONO_MODE = 0;
bool RANDOM_MODE = 0;
int SPECIAL_INPUT_DEBUG = 0;

bool LOG_MODE = 1;

// GA ARG
int GA_SIGNAL_TEST = 0;

// UI ARG


#define UI_ON 1
#define LAYOUT_STAT 0
#define LAYOUT_BVIOR 1
bool GUESS_BALL_UI = 1;

// CHRONO CONTROL

bool chrono_control_key[2] = {false, false}; // = pressed key [F, G]

// SYSTEM VAR
#define ROBOTS (7*2)  // number of robots
int CURRENT_BRAIN_LEVEL = 3;// 0-> random, 1->Naive, 3->mixed
double TIME_STEP;
unsigned int time_step_counter = 1;

WbNodeRef vpoint;

// BALL VAR

int player_ball[ROBOTS];
// ball variable
int non_last_ball = -1, spn_last_ball = -1;
int player_last_touch = -1, cached_player_last_touch = -1;
bool ball_check_hold = 1;


//  ball and robot pose
WbNodeRef ball_node;
WbNodeRef goal_node[2];
double ball_reset_timer = 0;
double ball_velo = 0;
double ball_position[3] = {0, 0, 0.2};
double old_ball_position[3] = {0, 0, 0.2}; 
double ball_moving_direction[2] = {0, 0};
 
double ball_initial_position[3] = {0, 0, 0.2};
double goal_position_x[2][3] = {0}, goal_position_y[2][3] = {0}; 

// PLAYER VAR
WbNodeRef player_def[ROBOTS];
WbNodeRef NAN_DEF;
double player_position[ROBOTS][3], player_rotation[ROBOTS][4];
double player_initial_position[ROBOTS][3], player_initial_rotation[ROBOTS][4];

const char *robot_name[ROBOTS] = {"NON_GK", "NON_CB", "NON_LB", "NON_RB", "NON_ST", "NON_LW", "NON_RW",
                                  "SPN_GK", "SPN_CB", "SPN_LB", "SPN_RB", "SPN_ST", "SPN_LW", "SPN_RW"};
const char *robot_webot_name[ROBOTS] = {"NON_001", "NON_002", "NON_005", "NON_004", "NON_009", "NON_010", "NON_011",
                                        "SPN_001", "SPN_002", "SPN_005", "SPN_004", "SPN_009", "SPN_010", "SPN_011"};
int map_id[12] = {-1, 0, 1, -1, 3, 2, -1, -1, -1, 4, 5, 6};
                //   gk  cb     rb lb             st lw rw
inline int robot_decrypt(int en_id) { return (en_id >= 500) ? map_id[en_id - 500] : (map_id[en_id] + 7); }

bool missing_player[ROBOTS] = {0};


// PLAYER ZONE -> for randomize
std::pair<double, double> bound_Y_zone[ROBOTS] = {make_pair(-SOFT_BOUND_Y, SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y*1/3), make_pair(-SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y*1/3), make_pair(-SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, SOFT_BOUND_Y),
												 make_pair(-SOFT_BOUND_Y, SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y*1/3), make_pair(-SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y*1/3), make_pair(-SOFT_BOUND_Y*1/3, -SOFT_BOUND_Y), make_pair(SOFT_BOUND_Y*1/3, SOFT_BOUND_Y)};

std::pair<double, double> bound_X_zone[ROBOTS] = {make_pair(SOFT_BOUND_X, SOFT_BOUND_X*0.8), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0.8), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0.8), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0.8), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0), make_pair(SOFT_BOUND_X*0.4, SOFT_BOUND_X*0),
											 make_pair(-SOFT_BOUND_X, -SOFT_BOUND_X*0.8), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0.8), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0.8), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0.8), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0), make_pair(-SOFT_BOUND_X*0.4, -SOFT_BOUND_X*0)};


// PLAYER COMMAND

struct Command_Pack {
	int player_id;
	int player_state;
	double sub_param_0, sub_param_1;
};
    // SOCCER VARIABLE
int player_state[ROBOTS];
double player_param_main[ROBOTS] = {-1000,-1000,-1000,-1000, -1000,-1000,-1000,-1000, -1000,-1000,-1000,-1000, -1000, -1000};
double player_param_sub[ROBOTS] = {-1000,-1000,-1000,-1000, -1000,-1000,-1000,-1000, -1000,-1000,-1000,-1000, -1000, -1000};

double old_player_param_main[ROBOTS];
double old_player_param_sub[ROBOTS];
int old_player_state[ROBOTS];

int manual_player = -1;

// SETUP_UI VAR


// REFEREE STAT CHECK
int score[2] = {0, 0};

double time_possession[2] = {1, 1};
double ball_attack_time[2] = {0, 0}; // total time in opponent field
double dead_ball_cached_time = 0;
int pass_attempt[2] = {0, 0}; int pass_cached = 0;
int self_pass[2] = {0, 0};
int pass_success[2] = {0, 0};

int shoot_attempt[2] = {0, 0};

int euler_cur_touch = -1, euler_cached_touch = -1;

// POLE VAR

bool pole_visibility[3] = {0, 0, 0};
WbNodeRef pole_ref[3];



// function - definition
void restart_all();


#endif
