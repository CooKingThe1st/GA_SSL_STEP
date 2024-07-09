// File:          judferee.cpp
// Date:
// Description:
// Author:
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <webots/Robot.hpp>

#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

#include "interupt.h"
#include "plan.h"
#include "chrono.h"
#include "keyboard_event.h"

#include "scripted_command.h"

#define INFO_MODE 0
#define DECENTRALIZED 1
#define DEBUG_MODE 0
#define CHECK_COMMAND 0
#define DEBUG_WEBOT_API 0
#define WIRELESS_DEBUG_MODE 0
#define WIRELESS_INTERUPT_MODE 0
#define PAPER_STAT_MODE 0
#define HANI_CHECK_MODE 1
#define RANDOM_FILTER 0


#define TEAM_A "SPN"
#define TEAM_B "NON"


#define FIXED_FLOAT(x) std::fixed <<std::setprecision(2)<<(x)

using namespace std;


string init_ball_side = "Red_";

// referee vari
int i, j;
const double TIME_FULL_MATCH = 90; //240;
double match_time = TIME_FULL_MATCH;  // a match lasts for 3 minutes

// referee sensor
WbDeviceTag non_emitter, non_receiver, spn_emitter, spn_receiver;
// judge sensor
// WbDeviceTag ga_emitter, ga_receiver;

// player return value

  // initial must be -1
void restart_all();

//--------------------

//------------------------init function--------------------------

void init_robot()
{
  const int temp = wb_robot_get_basic_time_step();
  TIME_STEP = temp;

  goal_node[0] = wb_supervisor_node_get_from_def("BLUE_GOAL");
  goal_node[1] = wb_supervisor_node_get_from_def("RED_GOAL");

  vpoint = wb_supervisor_node_get_from_def("VPOINT");

  pole_ref[0] = wb_supervisor_node_get_from_def("ADD_ON.POLE_1");
  pole_ref[1] = wb_supervisor_node_get_from_def("ADD_ON.POLE_2");
  pole_ref[2] = wb_supervisor_node_get_from_def("ADD_ON.POLE_3");

}

void init_sensor_actuator()
{
  spn_emitter = wb_robot_get_device("spn_emitter");
  non_emitter = wb_robot_get_device("non_emitter");
  // ga_emitter = wb_robot_get_device("ga_emitter");

  spn_receiver = wb_robot_get_device("spn_receiver");
  non_receiver = wb_robot_get_device("non_receiver");
  // ga_receiver = wb_robot_get_device("ga_receiver");

  wb_receiver_enable(spn_receiver, TIME_STEP);
  wb_receiver_enable(non_receiver, TIME_STEP);
  // wb_receiver_enable(ga_receiver, TIME_STEP);

  wb_keyboard_enable(TIME_STEP);

}

void init_player_ball_position()
{
  NAN_DEF = wb_supervisor_node_get_from_def("NAN");

  ball_node = wb_supervisor_node_get_from_def("BALL");
  wb_supervisor_node_enable_pose_tracking(ball_node, TIME_STEP, NULL);

  const double *tempPost = wb_supervisor_node_get_position( goal_node[0]);
  GOAL_X_LIMIT = (tempPost[0]);

  cout << "X LIMIT " << GOAL_X_LIMIT << '\n';
  for (i = 0; i < ROBOTS; i++) {
    player_def[i] = wb_supervisor_node_get_from_def(robot_name[i]);
    // if (isnan(player_def[i])) missing_player[i] = 1;
    if (player_def[i] == NAN_DEF) missing_player[i] = 1;

    else wb_supervisor_node_enable_pose_tracking(player_def[i], TIME_STEP, NULL);
  }

  for (i = 0; i < 2; i++){
    const double *tempPost = wb_supervisor_node_get_position(goal_node[i]);

    goal_position_x[i][0] = tempPost[0]; goal_position_y[i][0] = tempPost[1];
    goal_position_x[i][1] = tempPost[0]; goal_position_y[i][1] = tempPost[1] - 0.6;
    goal_position_x[i][2] = tempPost[0]; goal_position_y[i][2] = tempPost[1] + 0.6;

    set_up_goal(abs(tempPost[0]), tempPost[1]);
  }
  build_grid_field();
}


//----------------------timer update function-----------------

void update_robot_pose()
{
  const double *tempBall = wb_supervisor_node_get_position(ball_node);
  for (j = 0; j < 3; j++) old_ball_position[j] = ball_position[j], ball_position[j] = tempBall[j];
  ball_moving_direction[0] = ball_position[0] - old_ball_position[0];
  ball_moving_direction[1] = ball_position[1] - old_ball_position[1];

  tempBall = wb_supervisor_node_get_velocity(ball_node);
  ball_velo = fabs(sqrt(tempBall[0] * tempBall[0] + tempBall[1] * tempBall[1]));

  for (i = 0; i < ROBOTS; i++) {
    if (missing_player[i]) continue;
    tempBall = wb_supervisor_node_get_position(player_def[i]);
    for (j = 0; j < 3; j++)
      player_position[i][j] = tempBall[j];
    if (INFO_MODE) printf("PLAYER %d is at %f %f %f\n", i, player_position[i][0], player_position[i][1], player_position[i][2]);
    WbFieldRef temp = wb_supervisor_node_get_field(player_def[i], "rotation");
    tempBall = wb_supervisor_field_get_sf_rotation(temp);

    for (j = 0; j < 4; j++)
      player_rotation[i][j] = tempBall[j];
  }
}

void reset_receiver(){
  while (wb_receiver_get_queue_length(non_receiver) > 0) wb_receiver_next_packet(non_receiver);
  while (wb_receiver_get_queue_length(spn_receiver) > 0) wb_receiver_next_packet(spn_receiver);
  // while (wb_receiver_get_queue_length(ga_receiver) > 0) wb_receiver_next_packet(ga_receiver);
}

void update_wireless_receiver()
{
  while (wb_receiver_get_queue_length(non_receiver) > 0) {
      const int *message = (int const*)wb_receiver_get_data(non_receiver);
      // const int data_length = wb_receiver_get_data_size(non_receiver) /sizeof(int);

      int true_id = robot_decrypt(*message);
      if (WIRELESS_DEBUG_MODE) 
          cout << "RECEIVED: robot_id " << *message << " name " << robot_name[true_id] << " state " <<  *(message+1) << " ball_" << *(message+2) << '\n';   
      player_state[true_id] = *(message + 1);
      player_ball[true_id] = *(message + 2);

      if (player_ball[true_id] == 2) non_last_ball = true_id;
      if (player_ball[true_id] > 0) player_last_touch = true_id;

      wb_receiver_next_packet(non_receiver);
   }

  while (wb_receiver_get_queue_length(spn_receiver) > 0) {
      const int *message = (int const*)wb_receiver_get_data(spn_receiver);
      // const int data_length = wb_receiver_get_data_size(spn_receiver) /sizeof(int);

      int true_id = robot_decrypt(*message);
      if (WIRELESS_DEBUG_MODE) 
          cout << "RECEIVED: robot_id " << *message << " name " << robot_name[true_id] << " state " <<  *(message+1) << " ball_" << *(message+2) << '\n';   
      player_state[true_id] = *(message + 1);
      player_ball[true_id] = *(message + 2);

      if (player_ball[true_id] == 2) spn_last_ball = true_id;
      if (player_ball[true_id] > 0) player_last_touch = true_id;

      wb_receiver_next_packet(spn_receiver);
   }

  // while (wb_receiver_get_queue_length(ga_receiver) > 0) {
  //     const int *message = (int const*)wb_receiver_get_data(ga_receiver);

  //     if (GA_SIGNAL_TEST != 1) GA_SIGNAL_TEST = (*message);

  //     wb_receiver_next_packet(ga_receiver);
  //  }

   // cout << "final check " << pass_attempt[0] << " " << pass_attempt[1] << '\n';

  // while (wb_receiver_get_queue_length(ga_receiver) > 0) {
  //     const int *message = (int const*)wb_receiver_get_data(ga_receiver);
  //     // const int data_length = wb_receiver_get_data_size(ga_receiver) /sizeof(int);

  //     int true_id = robot_decrypt(*message);
  //     if (WIRELESS_INTERUPT_MODE) 
  //         cout << "RECEIVED: robot_id " << *message << " name " << robot_name[true_id] << " interupt_state " <<  *(message+1) << '\n';   
  //     player_interupt_state[true_id] = *(message + 1);

  //     wb_receiver_next_packet(ga_receiver);
  //  }
}

void update_goal()
{
    if (ball_reset_timer == 0) {
      int any_goal = check_goal();
      if (any_goal == 1) {  // ball in the blue goal
        set_scores(++score[0], score[1], 0.01, 0, 1);
        ball_reset_timer = 3;                            // wait for 3 seconds before reseting the ball
      } else if (any_goal == -1) {  // ball in the yellow goal
        set_scores(score[0], ++score[1], 0.01, 0, 1);
        ball_reset_timer = 3;  // wait for 3 seconds before reseting the ball
      }
    } else {
      ball_reset_timer -= (double)TIME_STEP / 1000.0;
      if (ball_reset_timer <= 0) {
        ball_reset_timer = 0;
        

        if (SPECIAL_INPUT_DEBUG == 0) restart_all();
        else ball_reset_timer = 100000;

      }
    }
}

// bool isolate_debug = 0;
// Point save_point_1, save_point_2;
//------------------- coaching function-------------
int rid = rand() % GRIDIFY_FIELD;

void update_ga_signal(){
  vector<double> receive_signal = read_file("..\\log\\GA_SIGNAL.txt");
  GA_SIGNAL_TEST = int(receive_signal[0]);
}

// 1 equal to NON, 0 for SPN
void update_fitness_value(unsigned int time_step_now, int scripted_id){
  GA_SIGNAL_TEST = 0;
    // cout << " DONE 1 cycle \n";
  double fitness_return = 10000;

  if (scripted_id == 1 || scripted_id == 2)
    fitness_return = fitness_return - wb_robot_get_time();
  else if (scripted_id == 3){
    fitness_return = time_possession[1];

    if (score[1] == 1 && pass_success[1] >= 1){
      fitness_return = 6000 + time_possession[1] - wb_robot_get_time();
    }
    else if (score[1] == 0){
      if (score[0] == 1) fitness_return = 400 + time_possession[1];
      else fitness_return = 550 + time_possession[1];
    }
  }
  else if (scripted_id == 4){
    fitness_return = fitness_return - wb_robot_get_time();

    if (score[1] > 0 && pass_success[1] >= 1)
      fitness_return = 6000 - wb_robot_get_time();
    else if (score[1] == 0 && pass_success[1] >= 1)
      fitness_return = 400 - wb_robot_get_time();
    else fitness_return = 50;
  }
  else if (scripted_id == 5){

// TESTING

    fitness_return = 1000 + 1000 * (score[1] > 0) + (300) * (pass_success[1] >= 1) - 20 * wb_robot_get_time();

    // now split into 2 option

    // balance, a bit focus on shoot
    // fitness_return += 30 * pass_success[1] + 5 * (pass_attempt[1] + shoot_attempt[1]) + 70 * shoot_attempt[1] - 10 * wb_robot_get_time();

    fitness_return += 50 * pass_success[1] + 10 * (pass_attempt[1] + shoot_attempt[1]) + 50 * shoot_attempt[1] - 5 * wb_robot_get_time();

        
  }

  // transmit signal
    cout << "fitness end " << fitness_return << '\n';
    string return_signal = std::to_string(1) + " " + std::to_string(fitness_return);
    clear_file("..\\log\\GA_RETURN.txt");
    log_to_file("..\\log\\GA_RETURN.txt", return_signal);

  // int data_send[1] = {9};
  // wb_emitter_send(ga_emitter, data_send, sizeof(int) * 1);
}


void command_decen(unsigned int time_step_now){

    for (int player = 0; player < ROBOTS; player++)
    {
      if (missing_player[player]) continue;
      try{
        Command_Pack get_command = Command_Pack{player, 0, -1000, -1000};

        if (score[0] + score[1] > 0){
          ;
          // player_state[player] = 0; player_param_main[player] = -1000; player_param_sub[player] = -1000;
        }
        else{


          if (IntPack.vio_type == -1 || player != IntPack.executor){

            double t_ball_position[2] = {ball_position[0], ball_position[1]}; 
            if (IntPack.vio_type != -1) 
              t_ball_position[0] = IntPack.VioPos.first,
              t_ball_position[1] = IntPack.VioPos.second;

            get_command = normal_gameplay(time_step_now, CURRENT_BRAIN_LEVEL, missing_player, player_position, player_ball, t_ball_position, player, Point{ball_moving_direction[0], ball_moving_direction[1]}, ball_velo, IntPack);

            if (IntPack.vio_type != -1)
              get_command = free_kick_mode(IntPack, get_command, player_position, player_ball);
            // cout << "player " << player << " return " << get_command.player_id << '\n';
          }
          else {
            if (player_ball[player] < 1 || length_dist_vector(ball_position[0],ball_position[1], IntPack.VioPos.first, IntPack.VioPos.second) > 0.5)
              get_command = free_kick_mode(IntPack, get_command, player_position, player_ball);
            else{
              get_command = normal_gameplay(time_step_now, CURRENT_BRAIN_LEVEL, missing_player, player_position, player_ball, ball_position, player, Point{ball_moving_direction[0], ball_moving_direction[1]}, ball_velo, IntPack);

              // cout << "    PENALTY RETURN  " << get_command.player_state << ' ' << get_command.sub_param_0 << ' ' << get_command.sub_param_1 << '\n';
              get_command = free_kick_mode(IntPack, get_command, player_position, player_ball);
            }
          }

        }

        // cout << " COMMAND DECEN " << player << " returnby " << get_command.player_id << " command " << get_command.player_state << '\n';

        assert(player == get_command.player_id);


        if (SPECIAL_INPUT_DEBUG > 0){

          if (GYM_TERMINATED_FLAG(time_step_now, SPECIAL_INPUT_DEBUG)) {
            update_fitness_value(time_step_now, SPECIAL_INPUT_DEBUG);
            restart_all();
            return;
          }
          else 
            get_command = GYM_SCRIPTED_COMMAND(time_step_now, player, get_command, SPECIAL_INPUT_DEBUG);
        }

        player_state[player] = get_command.player_state;
        player_param_main[player] = get_command.sub_param_0;
        player_param_sub[player] = get_command.sub_param_1;


        // if (player_ball[player]== 2) {
        //   isolate_debug = 1;
        //   save_point_1={ball_position[0],ball_position[1]};
        // }
        // if (isolate_debug){
        //   if (ball_velo > 0.34)cout << ball_velo << " ball velo " << '\n';
        //   if (ball_velo > 0.31 && ball_velo < 0.34) {
        //     save_point_2 = {ball_position[0],ball_position[1]};
        //     cout << length_dist_point(save_point_2, save_point_1) << " length ball \n";
        //   }
        //   player_state[player] = 0;
        // }

// FLOGGING BEHAVIOR

      } catch(...){
        cout << " player " << player << " ASSERTION \n";
      }
    }

    // if (non_last_ball == -1)
    //   player_state[robot_decrypt(501)] = 1,
    //   player_param_main[robot_decrypt(501)] = robot_decrypt(511);
    // else if (non_last_ball == robot_decrypt(501))
    //   player_state[robot_decrypt(511)] = 1,
    //   player_param_main[robot_decrypt(511)] = robot_decrypt(509);
    // else if (non_last_ball == robot_decrypt(511))
    //   player_state[robot_decrypt(509)] = 2,
    //   player_param_main[robot_decrypt(509)] = 0;

}

void transmit_coach()
{
    vector<int> changed;
    for (i = 0; i < 7; i++){
      if (missing_player[i]) continue;
      if ((player_state[i] != old_player_state[i]) or (player_param_main[i] != old_player_param_main[i]) or (player_param_sub[i] != old_player_param_sub[i])) changed.push_back(i);
    }
    float data_send[changed.size() * 4] = {0};
    for (i = 0; i < int(changed.size()); i++){
      int c_id = changed[i];
      data_send[i*4] = c_id, data_send[i*4+1] = player_state[c_id], data_send[i*4+2] = player_param_main[c_id], data_send[i*4+3] = player_param_sub[c_id];

      if (CHECK_COMMAND){
        if (HANI_CHECK_MODE && player_state[c_id] != 13) continue;
        string data_out = "ROBOT-> " + std::to_string(c_id) + " COMMAND->"+std::to_string(player_state[c_id]) + "\n";
        if (PAPER_STAT_MODE && player_state[c_id] == old_player_state[c_id]) continue;
        write_to_file("FLOG_"+init_ball_side, data_out);
        // cout << " COACH_PACK->rob: " << c_id << " state " << player_state[c_id] << " param_m " << FIXED_FLOAT(player_param_main[c_id]) << " param_s " << FIXED_FLOAT(player_param_sub[c_id]) << '\n';
      }
    }
    if (changed.size()) wb_emitter_send(non_emitter, data_send, sizeof(float) * changed.size() * 4);


    changed = vector<int>();
    for (i = 7; i < ROBOTS; i++){
      if (missing_player[i]) continue;
      if ((player_state[i] != old_player_state[i]) or (player_param_main[i] != old_player_param_main[i]) or (player_param_sub[i] != old_player_param_sub[i])) changed.push_back(i);
    }
    float _data_send[changed.size() * 4] = {0};
    for (i = 0; i < int(changed.size()); i++){
      int c_id = changed[i];
      _data_send[i*4] = c_id, _data_send[i*4+1] = player_state[c_id], _data_send[i*4+2] = player_param_main[c_id], _data_send[i*4+3] = player_param_sub[c_id];

      if (CHECK_COMMAND){
        if (HANI_CHECK_MODE && player_state[c_id] != 13) continue;
        string data_out = "ROBOT-> " + std::to_string(c_id) + " COMMAND->"+std::to_string(player_state[c_id]) + "\n";
        if (PAPER_STAT_MODE && player_state[c_id] == old_player_state[c_id]) continue;
        write_to_file("FLOG_"+init_ball_side, data_out);
        // cout << " COACH_PACK->rob: " << changed[i] << " state " << player_state[changed[i]] << " param_m " << FIXED_FLOAT(player_param_main[changed[i]]) << " param_s " << FIXED_FLOAT(player_param_sub[changed[i]]) << '\n';
      }
    }
    if (changed.size()) wb_emitter_send(spn_emitter, _data_send, sizeof(float) * changed.size()*4);

    for (i = 0; i < ROBOTS; i++)
      old_player_param_main[i] = player_param_main[i],
      old_player_param_sub[i] = player_param_sub[i],
      old_player_state[i] = player_state[i];
}

//----------------------referee function

void update_ball_touch_euler(){

  // if (IntPack.vio_type != -1) {
  //   euler_cur_touch = -1;
  //   return;
  // }

  euler_cur_touch = -1;
  for (i = 0; i < ROBOTS; i++)
  {
    // cout << player_ball[i] << " ";]
    if (euler_cur_touch == -1000) continue;
    if (missing_player[i]) continue;

    if (player_ball[i] > 0) {
      if (euler_cur_touch == -1)
        euler_cur_touch = i;
      else if (euler_cur_touch >= 0 && euler_cur_touch < ROBOTS)
        euler_cur_touch = -1000;
    }
  }

  if (euler_cur_touch == -1)
    for (i = 0; i < ROBOTS; i++){
      if (missing_player[i]) continue;
      if (euler_cur_touch == -1000) continue;

      if (length_dist_vector(ball_position[0], ball_position[1], player_position[i][0], player_position[i][1]) <= 0.227+0.06 + 0.03){
        if (euler_cur_touch == -1)
          euler_cur_touch = i;
        else if (euler_cur_touch >= 0 && euler_cur_touch < ROBOTS)
          euler_cur_touch = -1000;
      }
    }

  if (euler_cur_touch != -1) euler_cached_touch = euler_cur_touch;

  // set_single(euler_cached_touch, 0.3, 0.5, 55);
  // cout << " euler_cached_touch " << euler_cached_touch << '\n';
}

void update_ball_teamCon_Stat()
{
  if (ball_position[0] >= 0) ball_attack_time[0]++; // in non field
  else ball_attack_time[1]++; // in spn field

  ball_check_hold = 1;
  // cout << "PLAYER BALL: " << '\n';
  for (i = 0; i < ROBOTS; i++)
  {
    // cout << player_ball[i] << " ";
    if (player_ball[i] > 0) ball_check_hold = 0;
    int _team = (i >= 7) ? 0 : 1;

    if (player_ball[i] == 2){
      // cout << "WTH " << player_state[i] << '\n';
      if (player_state[i] == 9) // PASS
        pass_attempt[_team] += 1; // * (IntPack.vio_type == -1);
      else if (player_state[i] == 2) // SHOOT
        shoot_attempt[_team] += 1; // * (IntPack.vio_type == -1);
      else cout << "CQQJZ\n";

      string StartMarker = std::string("Start_of_a_")+(player_state[i] == 9 ? "Pass" : "Shoot") + " from " + std::to_string(i) +" \n"; 
      if (pass_cached == 0) write_to_file("FLOG_"+init_ball_side, StartMarker);
      if (player_state[i] == 9) pass_cached = 1;
      else if (player_state[i] == 2) pass_cached = -1;
    }
  }

  if (player_last_touch == -1) return;
  // cout << "now ball_last is " << player_last_touch << " with returned " << player_ball[player_last_touch] << " ball_check_hold " << ball_check_hold << '\n';
  // cout << " time possesion yellow " << time_possession[0] << " blue " << time_possession[1] << '\n';
  if (ball_check_hold == 1) dead_ball_cached_time += 1, cached_player_last_touch = player_last_touch;
  else
  {
    if (player_last_touch >= 7 and player_last_touch < 14) time_possession[0] += 1; // * (IntPack.vio_type == -1);
    else if (player_last_touch >= 0 and player_last_touch < 7) time_possession[1] += 1; // * (IntPack.vio_type == -1);

    if (cached_player_last_touch >= 7 && player_last_touch >= 7) {
      time_possession[0] += dead_ball_cached_time; // * (IntPack.vio_type == -1);
      if (cached_player_last_touch != player_last_touch && cached_player_last_touch != -1) {
        if (pass_cached == 1) 
          pass_success[0] += pass_cached, // * (IntPack.vio_type == -1),
          write_to_file("FLOG_"+init_ball_side, std::string("A_good_pass_to ")+ std::to_string(player_last_touch)+"\n");
        else if (pass_cached == -1)
          write_to_file("FLOG_"+init_ball_side, std::string("A_bad_shot_to ")+ std::to_string(player_last_touch)+"\n");
        pass_cached = 0;
        // cout << "spn " << player_last_touch << ' ' << cached_player_last_touch << '\n';
      }
      else if (cached_player_last_touch == player_last_touch){
        self_pass[0] += MAX(pass_cached, 0); // * (IntPack.vio_type == -1);
        pass_cached = 0;
        cout << "SELF PASS\n";
      }
    } else
    if (cached_player_last_touch < 7 && player_last_touch < 7) {
      time_possession[1] += dead_ball_cached_time; // * (IntPack.vio_type == -1);
      if (cached_player_last_touch != player_last_touch && cached_player_last_touch != -1) {
        if (pass_cached == 1) 
          pass_success[1] += pass_cached, // * (IntPack.vio_type == -1),
          write_to_file("FLOG_"+init_ball_side, std::string("A_good_pass_to ")+ std::to_string(player_last_touch)+"\n");
        else if (pass_cached == -1)
          write_to_file("FLOG_"+init_ball_side, std::string("A_bad_shot_to ")+ std::to_string(player_last_touch)+"\n");
        // cout << "non " << player_last_touch << ' ' << cached_player_last_touch << '\n';
        pass_cached = 0;
      }
      else if (cached_player_last_touch == player_last_touch){
        self_pass[1] += MAX(pass_cached, 0); // * (IntPack.vio_type == -1);
        pass_cached = 0;
        cout << "SELF PASS \n";
      }
    }
    else if (cached_player_last_touch != -1){
      pass_cached = 0;
      string ConsiderMarker = std::string("Switch_Possesion from ") + std::to_string(cached_player_last_touch) + " to " + std::to_string(player_last_touch) + "\n";
      write_to_file("FLOG_"+init_ball_side, ConsiderMarker);
    }
    dead_ball_cached_time = 0;
    cached_player_last_touch = -1;
  }
}

// write_to_file("out_capture.txt", "frame " + to_string(counter_video_frame)+"\n");

void update_current_time()
{
    // Adds TIME_STEP ms to the time
    match_time -= (double)TIME_STEP / 1000;
    if (match_time < 0) {
      match_time = TIME_FULL_MATCH;  // restart
      score[0] = 0; score[1] = 0;
      set_scores(0, 0, 0.01, 0, 1);
      restart_all();
      // time_possession[0] = 1, time_possession[1] = 1;
      // dead_ball_cached_time = 0;
    }
    show_current_time(match_time);
}

void soft_restart_all()
{
  wb_supervisor_simulation_reset();
  // for (int i = 0; i < ROBOTS; i++)
  //   if (!missing_player[i]) wb_supervisor_node_restart_controller(player_def[i]);
  // wb_supervisor_node_restart_controller(wb_supervisor_node_get_self());

  // wb_supervisor_world_reload();

  double ball_velocity[6] = {0, 0, 0, 0, 0, 0};
  wb_supervisor_node_set_velocity(ball_node, ball_velocity);
  wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(ball_node, "translation"), ball_initial_position);

  for (i = 0; i < ROBOTS; i++) {
    if (missing_player[i]) continue;
    wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(player_def[i], "translation"), player_initial_position[i]);
    wb_supervisor_field_set_sf_rotation(wb_supervisor_node_get_field(player_def[i], "rotation"), player_initial_rotation[i]);
  
    old_player_param_main[i] = -1;
    old_player_state[i] = -1;
    player_state[i] = 0;
    player_param_main[i] = -1000, player_param_sub[i] = -1000; player_ball[i] = 0;
  }
  // non_last_ball = -1, spn_last_ball = -1;
  // player_last_touch = -1, cached_player_last_touch = -1;

  reset_receiver();
}

void restart_all()
{
  string Header = std::string("TIME ") + get_file_timestamp() + " STEP " + std::to_string(time_step_counter) + "\n";
  string Possesion = "Possesion " + std::to_string(time_possession[0] / (time_possession[0] + time_possession[1])) + ' ' +  std::to_string(time_possession[1] / (time_possession[0] + time_possession[1])) + '\n';
  string PAttempt = "PassAttempt " + std::to_string(pass_attempt[0] - self_pass[0]) + " " + std::to_string(pass_attempt[1] - self_pass[1]) + " Success " + std::to_string(pass_success[0]) + " " + std::to_string(pass_success[1]) + "\n";
  string SAttempt = "ShootAttempt " + std::to_string(shoot_attempt[0]) + " " + std::to_string(shoot_attempt[1]) + "\n";
  string Ball_AtkRate = "BallAtkTime " + std::to_string(ball_attack_time[0]) + " " + std::to_string(ball_attack_time[1]) + "\n";

  string Goal_R = "Goal " + std::to_string(score[0]) + ' ' + std::to_string(score[1]) + '\n';

  string EndRoll = "EndOfReport\n";

  clear_file("reward.txt");
  clear_file("temp_p.txt");

  write_to_file("FLOG_"+init_ball_side, Header + Possesion + PAttempt + SAttempt + Ball_AtkRate + Goal_R+EndRoll);
  wb_supervisor_simulation_reset();
  for (int i = 0; i < ROBOTS; i++)
    if (!missing_player[i]) wb_supervisor_node_restart_controller(player_def[i]);
  wb_supervisor_node_restart_controller(wb_supervisor_node_get_self());
  // wb_supervisor_world_reload();

  // double ball_velocity[6] = {0, 0, 0, 0, 0, 0};
  // wb_supervisor_node_set_velocity(ball_node, ball_velocity);
  // wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(ball_node, "translation"), ball_initial_position);

  // for (i = 0; i < ROBOTS; i++) {
  //   if (missing_player[i]) continue;
  //   wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(player_def[i], "translation"), player_initial_position[i]);
  //   wb_supervisor_field_set_sf_rotation(wb_supervisor_node_get_field(player_def[i], "rotation"), player_initial_rotation[i]);
  
  //   old_player_param_main[i] = -1;
  //   old_player_state[i] = -1;
  //   player_state[i] = 0;
  //   player_param_main[i] = -1000, player_param_sub[i] = -1000; player_ball[i] = 0;
  // }
  // non_last_ball = -1, spn_last_ball = -1;
  // player_last_touch = -1, cached_player_last_touch = -1;

  // reset_receiver();
}

void get_initial_pose()
{

  for (i = 0; i < ROBOTS; i++){
    for (j = 0; j < 3; j++)
      player_initial_position[i][j] = player_position[i][j];
    for (j = 0; j < 4; j++)
      player_initial_rotation[i][j] = player_rotation[i][j];
  }
  for (j = 0; j < 3; j++)
    ball_initial_position[j] = ball_position[j];
}

void little_reroll(){
  if (!RANDOM_MODE) return;

  if (SPECIAL_INPUT_DEBUG == 3 || SPECIAL_INPUT_DEBUG == 0){

    for (int i = 0; i < ROBOTS; i++){
      if (missing_player[i]) continue;

      // srand(i+time(0));
      int choice = rand()%15;
      Point new_pose = bound_p(get_move_to_dir(Point{player_initial_position[i][0], player_initial_position[i][1]}, choice), 0.4);
      player_initial_position[i][0] = new_pose.first;
      player_initial_position[i][1] = new_pose.second;
      wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(player_def[i], "translation"), player_initial_position[i]);
    }
  }
  else if (SPECIAL_INPUT_DEBUG == 1 || SPECIAL_INPUT_DEBUG == 2 || SPECIAL_INPUT_DEBUG >= 4) {

      vector<double> temp = read_file("..\\log\\GA_ENV.txt");

      double fix_rad = 0;
      if (temp.size() > 0) fix_rad = temp[0];

      fix_rad = read_file("..\\log\\GA_BASE_ENV.txt")[fix_rad];

      for (int i = 0; i < ROBOTS; i++) if (!missing_player[i]){
          // srand(i+time(0));
        int STEP_NUM = 36;
        double STEP_ANG = 360 / STEP_NUM;

        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist36(0, 35); // distribution in range [1, 6]

        int rand_choice = dist36(rng);
        // Point new_pose = bound_p( Point{player_position[i][0] + fix_rad * cos(  i*STEP_ANG *  M_PI / 180.0 ),  player_position[i][1] + fix_rad * sin(  i*STEP_ANG *  M_PI / 180.0 )}, 0.4);

        Point new_pose = bound_p( Point{player_initial_position[i][0] + fix_rad * cos(  rand_choice*STEP_ANG *  M_PI / 180.0 ),  player_initial_position[i][1] + fix_rad * sin(  rand_choice*STEP_ANG *  M_PI / 180.0 )}, 0.4);

        player_initial_position[i][0] = new_pose.first;
        player_initial_position[i][1] = new_pose.second;
        wb_supervisor_field_set_sf_vec3f(wb_supervisor_node_get_field(player_def[i], "translation"), player_initial_position[i]);

        break;
      }
  }

}

void process_sysvar(int argc, char **argv){
  // cout << argc << '\n';
  // for (int i = 1; i < argc; i++)
  //   std::cout << "argv[" << i << "]=" << argv[i] << std::endl; 

  string temp = string(argv[1]);
  std::size_t found_level = temp.find("AI_LEVEL");
  if (found_level != std::string::npos)
    // std::cout << "first 'AI' found at: " << temp.substr(found_level) << '\n';
    CURRENT_BRAIN_LEVEL = temp.back() - '0';


  temp = string(argv[2]);
  found_level = temp.find("RAND");
  if (found_level != std::string::npos)
    // std::cout << "first 'AI' found at: " << temp.substr(found_level) << '\n';
    RANDOM_MODE =  ((temp.back() - '0') > 0) ? 1 : 0;

  temp = string(argv[3]);
  found_level = temp.find("MANUAL");
  if (found_level != std::string::npos)
   // std::cout << "first 'AI' found at: " << temp.substr(found_level) << '\n';
    MANUAL_MODE =  ((temp.back() - '0') > 0) ? 1 : 0;  

  temp = string(argv[4]);
  found_level = temp.find("SCRIPTED"); // maximum 9 scripts
  if (found_level != std::string::npos)
    // std::cout << "first 'AI' found at: " << temp.substr(found_level) << '\n';
    SPECIAL_INPUT_DEBUG =  (temp.back() - '0');

  string param_concat = std::to_string(CURRENT_BRAIN_LEVEL) + ' ' + std::to_string(RANDOM_MODE) + ' ' + std::to_string(MANUAL_MODE) + ' ' + std::to_string(SPECIAL_INPUT_DEBUG) + '\n';

  clear_file("..\\log\\JUDFEREE_PARAM.txt");
  log_to_file("..\\log\\JUDFEREE_PARAM.txt", param_concat);
}


int main(int argc, char **argv) {

  srand(time(0));
  rid = rand() % GRIDIFY_FIELD;

  wb_robot_init();

  process_sysvar(argc, argv);

  init_robot();

  fix_viewpoint();

  init_player_ball_position();

  init_sensor_actuator();

  update_robot_pose();

  get_initial_pose();

  set_visibility();

  little_reroll();

  set_scores(0, 0, 0.01, 0, 1);

  STAT_UI();

  clear_file("reward.txt");
  clear_file("temp_p.txt");


  if (ball_position[0] > 0)
    init_ball_side = "Blue_";
  write_to_file("FLOG_"+init_ball_side, "GAME_START\n");

  if (MANUAL_MODE) init_keyboard();
  if (CHRONO_MODE) init_chrono();


  // RGB_test(stoi(argv[1]), stoi(argv[2]), stoi(argv[3]));


  while (wb_robot_step(TIME_STEP) != -1) {

      // if (ATTACK_DEBUG_MODE) cout << "       BALL_VELO NOW " << ball_velo << '\n';
      update_robot_pose();
      update_wireless_receiver();

      update_ball_teamCon_Stat();
      update_ball_touch_euler();
      
      if (SPECIAL_INPUT_DEBUG){
        update_ga_signal();
      
        if (GA_SIGNAL_TEST == 1){
          // GA_SIGNAL_TEST = -1;
          // restart_all();
        }
        else if (GA_SIGNAL_TEST == 0) {
          time_step_counter = 0;
          continue;
        }
      }
      // else if (GA_SIGNAL_TEST == -2){
      //   GA_SIGNAL_TEST = 0;

      //   soft_restart_all();
      // }

      time_step_counter += 1;

      // cout << "DONE SENSOR \n";

      if (LAYOUT_STAT) {
       set_percent(time_possession[0] / (time_possession[0] + time_possession[1]), time_possession[1] / (time_possession[0] + time_possession[1]), 0.1, 69, 70);
       set_scores(pass_attempt[0]- self_pass[0], pass_attempt[1]- self_pass[1], 0.2, 9, 10);
       set_scores(pass_success[0] , pass_success[1] , 0.3, 12, 13);
       set_scores(shoot_attempt[0], shoot_attempt[1], 0.4, 14, 15);
      }
      else if (LAYOUT_BVIOR){
         set_percent(time_possession[0] / (time_possession[0] + time_possession[1]), time_possession[1] / (time_possession[0] + time_possession[1]), 0.42, 69, 70);
         set_scores(pass_attempt[0]- self_pass[0], pass_attempt[1]- self_pass[1], 0.45, 9, 10);
         set_scores(pass_success[0] , pass_success[1] , 0.48, 12, 13);
         set_scores(shoot_attempt[0], shoot_attempt[1], 0.51, 14, 15);
      }
      // set_single(ball_velo, 0.3, 0.5, 53);
      // set_single(length_dist_vector(ball_position[0], ball_position[1], -3.1 , -0.12), 0.5, 0.5, 54);
      // if (DECENTRALIZED)

      try{
        if ( CURRENT_BRAIN_LEVEL != BRAIN_LEVEL_0  && (score[0]+ score[1] < 1) )
          check_interupt(missing_player, player_position, euler_cached_touch);
      } catch(...){
        printf(" CHECK INTERUP ASSERTION \n");
      }

      // cout << "DONE CHECK INT\n";

      // set_single(IntPack.executor ,0.3, 0.3, 56);

      command_decen(time_step_counter);

      if (MANUAL_MODE or CHRONO_MODE) check_keyboard();
      // cout << "DONE GET COMMAND\n";
      if (CHRONO_MODE) chrono_control();
      
      transmit_coach();
      show_command(player_state, player_ball, player_param_main, player_param_sub);

      // cout << "DONE TRANSMIT\n";

      update_current_time();
      update_goal();

      // cout << "DONE JUDGEFREE CYCLE \n";
       // cout << "1 \n";
      // cout << "2 \n";
      // cout << "     GA SIGNAL  " <<  GA_SIGNAL_TEST << '\n';

  }

  wb_robot_cleanup();
  return 0;
}