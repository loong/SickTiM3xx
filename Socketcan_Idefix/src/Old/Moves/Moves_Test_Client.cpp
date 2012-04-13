/*
 * Moves_Test_Client.cpp
 *
 *  Created on: 05.06.2010
 *      Author: locher
 */



#include <iostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <player-3.0/libplayerc++/playerc++.h>

#include "Idefix_Data_Structs.h"
#include "Moves.h"

/**
 * Example Client for showing and Testing the static Methods of the Moves Class
 *
 */


int main(int argc, char **argv) {

	using namespace PlayerCc;

	PlayerClient player_client("localhost");

	OpaqueProxy opaque_proxy(&player_client,0);

	SonarProxy sonar_proxy(&player_client,0);


	idefix::Moves the_moves(player_client,opaque_proxy);

	idefix::opaque_cmd_t drive_data_cmd;

	player_opaque_data_t opaque_data;


	int loop_count=0;

	while(true){

		++loop_count;

 		player_client.Read();

		idefix::opaque_data_t drive_data;

		player_opaque_data_t data;
//		data.data_count=sizeof( drive_data);
		data.data=reinterpret_cast<uint8_t*>(&drive_data);

//		opaque_proxy.SendReq(&data );

		opaque_proxy.GetData(data.data);

//		std::cout
//				<<	loop_count
//				<<" Client: distances: front " << drive_data.distance_front_m
//				<< "  rear " << drive_data.distance_rear_m
//				<< "  Velocity  front " << drive_data.velocity_front_mps
//				<< "  rear " << drive_data.velocity_rear_mps
//				<<  std::endl;


		std::cout << "*";

//		drive_data_cmd.new_angle_front=(2*sonar_proxy[0]-1);
//		drive_data_cmd.new_angle_rear=(2*sonar_proxy[1]-1);
//		drive_data_cmd.new_velocity=0;
//		drive_data_cmd.reset_distance=false;

		drive_data_cmd.drive_profile=idefix::PROFILE_VELOCITY;
		drive_data_cmd.new_velocity=0.003;
		drive_data_cmd.new_angle_front=0;
		drive_data_cmd.new_angle_rear=0;
		drive_data_cmd.reset_distance=false;


		if( (loop_count%50) ==0){


//			the_moves.move_circle_profile_velocity(0.4,0.2,1);
//			the_moves.move_circle_profile_position(0.4,-1);
//
//			the_moves.move_parallel_profile_velocity(0.4,-0.2,-1);
//			the_moves.move_parallel_profile_position(-0.4,1);
//
//			the_moves.move_circle_profile_velocity(0.4,-0.2,-1);
//			the_moves.move_circle_profile_position(-0.4,1);
//
//			the_moves.move_parallel_profile_velocity(0.4,0.2,1);
//			the_moves.move_parallel_profile_position(0.4,-1);



//			the_moves.move_90_degree_backward_right_profile_velocity(0.2);
//			the_moves.move_90_degree_backward_right_profile_velocity(0.4);
//			the_moves.move_90_degree_backward_right_profile_velocity(0.6);
//			the_moves.move_90_degree_backward_right_profile_velocity(0.8);


//			the_moves.move_90_degree_backward_left_profile_position();
//			the_moves.move_90_degree_forward_left_profile_position();
//			the_moves.move_90_degree_backward_right_profile_position();
//			the_moves.move_90_degree_forward_right_profile_position();

//			the_moves.move_turn_same_row_profile_position();







		}




		opaque_data.data=reinterpret_cast<uint8_t *>(&drive_data_cmd);

		opaque_data.data_count=sizeof(idefix::opaque_cmd_t);



		opaque_proxy.SendCmd(&opaque_data);



		usleep(20000);


	}



}
