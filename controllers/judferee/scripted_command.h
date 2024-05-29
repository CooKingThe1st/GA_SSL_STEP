#ifndef SCRIPTED_COMMAND   /* Include guard */
#define SCRIPTED_COMMAND

#include "VARIABLE.h"
#include <vector>


Command_Pack GYM_SCRIPTED_COMMAND(unsigned int time_step_now, int player_ask_id, Command_Pack origin_command, int script_id){
		Command_Pack return_command = Command_Pack{player_ask_id, 0, -1000, -1000};

		if (script_id == 2){
		          if (wb_robot_get_time() < 1.8){
		            if (player_ask_id == 1) return_command = Command_Pack{1, 8, -1000, -1000};
		            if (player_ask_id == 4) return_command = Command_Pack{4, 0, -1000, -1000};
		          }
		          else if (player_ball[1] == 1){
		            if (player_ask_id == 4) return_command = Command_Pack{4, 0, -1000, -1000};
		            if (player_ask_id == 1) return_command = Command_Pack{8,  9, player_position[4][0], player_position[4][1]};
		            // if (player_ask_id == 8) return_command = Command_Pack{8,  9, grid_x[rid],grid_y[rid]};
		          }
		          else if (player_ball[4] != 1) {
		            if (player_ask_id == 1) return_command = Command_Pack{1, 0, -1000, -1000};

		            // if (player_ask_id == 11) cout << "MAIN CONCERN " << return_command.player_state << ' ' << return_command.sub_param_0 << ' ' << return_command.sub_param_1 << " here at rob 11 \n"; 
		            // if (player_ask_id == 11 && return_command.player_state != 68) return_command = Command_Pack{11, 8, -1000, -1000};

		            if (player_ask_id == 4) return_command = origin_command;
		          }
        }
        else if (script_id == 1)
        {
        	vector<int> player_on_field;
        	for (int i = 0; i < ROBOTS; i++) if (!missing_player[i]) player_on_field.push_back(i);

        	if (player_ball[player_on_field[0]] == 0){

        		if (player_ask_id == player_on_field[0])
        			return_command = Command_Pack{player_on_field[0], 8, -1000, -1000 };
	        }

        }
        else if (script_id == 3){
        	return_command = origin_command;
        }
        return return_command;
}

bool GYM_TERMINATED_FLAG(unsigned int time_step_now, int script_id){

	if (script_id == 1){ // move 2 ball training
		if (wb_robot_get_time() > 10) return true;

		vector<int> player_on_field;
		for (int i = 0; i < ROBOTS; i++) if (!missing_player[i]) player_on_field.push_back(i);
		return (player_ball[player_on_field[0]]);
	}
	else if (script_id == 2){ // simple pass ball training
		if (wb_robot_get_time() > 12) return true;

		return player_ball[4];
	}
	else if (script_id == 3){
		if (wb_robot_get_time() > 20) return true;
		return ((score[0] + score[1]) > 0);
	}
	else return true;
	// if (time_step_now > 20) return true;
	// else return false;
}

// fitnessfunction

#endif 