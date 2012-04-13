/*
 * Moves.cpp
 *
 *  Created on: 02.06.2010
 *      Author: locher
 */

#include "Moves.h"

namespace idefix{

	Moves::Moves(
			PlayerCc::PlayerClient& player_client,
			PlayerCc::OpaqueProxy& opaque_proxy

	) : my_player_client_ref(player_client),
		my_opaque_proxy_ref(opaque_proxy)

	{

	}

	Moves::~Moves() {
	}


	/*
	 * *****************************************************************************************************
	 */

	void Moves::move_parallel_profile_position(double signed_distance, double steering_angle){

		move_distance_with_profile_position(signed_distance,steering_angle,MOVE_TYPE_PARALLEL);

	}
	void Moves::move_circle_profile_position(double signed_distance, double steering_angle){

		move_distance_with_profile_position(signed_distance,steering_angle,MOVE_TYPE_CIRCLE);

	}

	/*
	 * *****************************************************************************************************
	 */


	void Moves::move_parallel_profile_velocity(
			double unsigned_distance, double signed_velocity, double steering_angle){

		move_distance_with_profile_velocity(unsigned_distance,signed_velocity,steering_angle,MOVE_TYPE_PARALLEL);


	}
	void Moves::move_circle_profile_velocity(
			double unsigned_distance, double signed_velocity, double steering_angle){

		move_distance_with_profile_velocity(unsigned_distance,signed_velocity,steering_angle,MOVE_TYPE_CIRCLE);

	}

	/*
	 * *****************************************************************************************************
	 */

	void Moves::move_90_degree_forward_left_profile_position(){

		move_distance_with_profile_position(1.2,-1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_forward_right_profile_position(){

			move_distance_with_profile_position(1.2,1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_backward_left_profile_position(){

			move_distance_with_profile_position(-1.2,-1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_backward_right_profile_position(){

			move_distance_with_profile_position(-1.2,1,MOVE_TYPE_CIRCLE);

	}  //by Simon :-)

	/*
	 * *****************************************************************************************************
	 *

	***Funktionieren Noch Nicht. Die strecke die der Roboter Fahren muss muss abhängig von der Geschwindigkeit sein.***

	void Moves::move_90_degree_forward_left_profile_velocity(double velocity){

			move_distance_with_profile_velocity(1.2,fabs(velocity),-1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_forward_right_profile_velocity(double velocity){

			move_distance_with_profile_velocity(1.2,fabs(velocity),1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_backward_left_profile_velocity(double velocity){

			move_distance_with_profile_velocity(1.2,-fabs(velocity),-1,MOVE_TYPE_CIRCLE);

	}   //by Simon :-)

	void Moves::move_90_degree_backward_right_profile_velocity(double velocity){

			move_distance_with_profile_velocity(1.2,-fabs(velocity),1,MOVE_TYPE_CIRCLE);

	}  //by Simon :-)

<<<<<<< Moves.cpp
/*
 * ***************************************************************************************
 */



	void Moves::move_W_backwardright_profile_position(){

			move_parallel_profile_position(-0.4,0);
			move_parallel_profile_position(-0.5,-1);
			move_parallel_profile_position(0.5,1);
			move_parallel_profile_position(-0.5,-1);
			move_parallel_profile_position(0.5,1);

		}// by LUHLEN


		void Moves::move_W_forwardright_profile_position(){

			move_parallel_profile_position(0.4,0);
			move_parallel_profile_position(0.5,1);
			move_parallel_profile_position(-0.5,-1);
			move_parallel_profile_position(0.5,1);
			move_parallel_profile_position(-0.5,-1);

		} //by LUHLEN

		void Moves::move_W_forwardleft_profile_position(){

			move_parallel_profile_position(0.4,0);
			move_parallel_profile_position(0.5,-1);
			move_parallel_profile_position(-0.5,1);
			move_parallel_profile_position(0.5,-1);
			move_parallel_profile_position(-0.5,1);



		}
		void Moves::move_W_backwardleft_profile_position(){

			move_parallel_profile_position(-0.4,0);
			move_parallel_profile_position(-0.5,1);
			move_parallel_profile_position(0.5,-1);
			move_parallel_profile_position(-0.5,1);
			move_parallel_profile_position(0.5,-1);




		}

		void Moves::move_Y_forwardright_profile_position(){

			//move_parallel_profile_position(0.25,0);
			move_parallel_profile_position(1,1);
			move_parallel_profile_position(-1,-1);
			move_parallel_profile_position(-0.25,0);



		}

		void Moves::move_Y_backwardright_profile_position(){

			//move_parallel_profile_position(-0.25,0);
			move_parallel_profile_position(-1,-1);
			move_parallel_profile_position(1,1);
			move_parallel_profile_position(0.25,0);


		}

		void Moves::move_Y_forwardleft_profile_position(){


			//move_parallel_profile_position(0.25,0);
			move_parallel_profile_position(1,-1);
			move_parallel_profile_position(-1,1);
			move_parallel_profile_position(-0.25,0);


		}

		void Moves::move_Y_backwardleft_profile_position(){

			//move_parallel_profile_position(-0.25,0);
			move_parallel_profile_position(-1,1);
			move_parallel_profile_position(1,-1);
			move_parallel_profile_position(0.25,0);

		}




	/*
>>>>>>> 1.8
	 * *****************************************************************************************************
	 */


	void Moves::move_turn_same_row_profile_position(){
		move_90_degree_forward_right_profile_position();
		move_distance_with_profile_position(-1.5,0,MOVE_TYPE_PARALLEL);
		move_90_degree_forward_right_profile_position();
	}
	/*
	 * *********************************************************************************************************
	 */
	void Moves::move_distance_with_profile_position(
			double signed_distance,
			double steering_angle,
			move_type_t move_type)
	{


		idefix::opaque_cmd_t cmd;
		idefix::opaque_data_t data;

		player_opaque_data_t opaque_data;
		player_opaque_data_t opaque_cmd;

		cmd.move_distance_m=signed_distance;
		cmd.drive_profile=idefix::PROFILE_POSITION;

		cmd.new_angle_front=steering_angle;

		if(move_type==MOVE_TYPE_CIRCLE){

			cmd.new_angle_rear=steering_angle;

		}
		else if(move_type==MOVE_TYPE_PARALLEL){

			cmd.new_angle_rear=-steering_angle;

		}else{

			std::cerr  << "Error Move with undefined steering_angle rear: Set zo zero" << std::endl;
			cmd.new_angle_rear=0;

		}

		cmd.reset_distance=true;

		// Umbiegen der Zeiger in opaque_cmd und opaque_data
		// so dass diese auf die cmd und data verweisen

		opaque_cmd.data=reinterpret_cast<uint8_t *>(&cmd);
		opaque_cmd.data_count=sizeof(idefix::opaque_cmd_t);


		opaque_data.data=reinterpret_cast<uint8_t *>(&data);
		opaque_data.data_count=sizeof( idefix::opaque_data_t);

		// Buggy workaround
		// check if odometrie is really reseted
		// send command as long as odometrie data is realy resetted
		// Wait for steering angle reached
		do{
			my_player_client_ref.Read();
			my_opaque_proxy_ref.SendCmd(&opaque_cmd);
			my_opaque_proxy_ref.GetData(opaque_data.data);
			std::cout<< std::endl << "Reset_Distance check " << data.distance_front_m << " " << data.distance_rear_m
					<< " A_f " << data.steering_angle_front
					<< " A_r " << data.steering_angle_rear
					<<	std::endl;
		}while(
				(data.distance_front_m ==0)
						&&
				(data.distance_rear_m ==0)

					&&
				( fabs( fabs(data.steering_angle_front) - fabs(steering_angle) )  < I_C::DELTA_FOR_STEERING_ANGLE_REACHED )
				&&
				( fabs( fabs(data.steering_angle_rear) - fabs(steering_angle) )  <I_C::DELTA_FOR_STEERING_ANGLE_REACHED )
				);




		cmd.reset_distance=false;
		bool end_of_loop=false;
		// Wait until both drives have moved the desired distance
		while(!end_of_loop){



			my_player_client_ref.Read();
			my_opaque_proxy_ref.GetData(opaque_data.data);

			std::cout  << "+";

//			std::cout << "moving desired distance "
//					<< ", V_f " 	<<  data.velocity_front_mps
//					<< ", V_r " << data.velocity_rear_mps
//					<< ", D_f " << data.distance_front_m
//					<< ", D_r " << data.distance_rear_m
//					<< std::endl;

			end_of_loop=(
					(data.velocity_front_mps==0) &&
					(data.velocity_rear_mps==0) &&
					//Wenigstens die halbe Fahrstrecke sollte gefahren sein
					(data.distance_front_m >= fabs(signed_distance*0.5) ) &&
					(data.distance_rear_m>=fabs(signed_distance*0.5))
			);

			usleep(10000);

		}

		std::cout << std::endl;


	}


	void Moves::move_distance_with_profile_velocity(
			double unsigned_distance,
			double signed_velocity,
			double steering_angle,
			move_type_t move_type){


		idefix::opaque_cmd_t cmd;
		idefix::opaque_data_t data;

		player_opaque_data_t opaque_data;
		player_opaque_data_t opaque_cmd;

		cmd.move_distance_m=0;
		cmd.drive_profile=idefix::PROFILE_VELOCITY;
		cmd.new_velocity=signed_velocity;
		cmd.new_angle_front=steering_angle;

		if(move_type==MOVE_TYPE_CIRCLE){

			cmd.new_angle_rear=steering_angle;

		}
		else if(move_type==MOVE_TYPE_PARALLEL){

			cmd.new_angle_rear=-steering_angle;

		}else{

			cmd.new_angle_rear=0;

		}


		cmd.reset_distance=true;

		opaque_cmd.data=reinterpret_cast<uint8_t *>(&cmd);
		opaque_cmd.data_count=sizeof(idefix::opaque_cmd_t);

		opaque_data.data=reinterpret_cast<uint8_t *>(&data);
		opaque_data.data_count=sizeof( idefix::opaque_data_t);

		my_player_client_ref.Read();
		my_opaque_proxy_ref.SendCmd(&opaque_cmd);
		my_opaque_proxy_ref.GetData(opaque_data.data);



		bool end_of_loop=false;
		cmd.reset_distance=false;


		// Buggy workaround
		// check if odometrie is really reseted
		// send command as long as odometrie data is realy reseted

		do{
			my_player_client_ref.Read();
			my_opaque_proxy_ref.SendCmd(&opaque_cmd);
			my_opaque_proxy_ref.GetData(opaque_data.data);
		//	std::cout<< std::endl << "Rest_Distance check " << data.distance_front_m << " " << data.distance_rear_m<<  std::endl;
		}while( !(
				(data.distance_front_m ==0)
						&&
				(data.distance_rear_m ==0)
				)
			);


		// wait until both drives have moved the desired distance
		while(!end_of_loop){

			usleep(20000);

			my_player_client_ref.Read();
			my_opaque_proxy_ref.GetData(opaque_data.data);
			my_opaque_proxy_ref.SendCmd(&opaque_cmd);

			std::cout << ".";

			if(
					( data.distance_front_m > fabs(unsigned_distance))
					&&
					( data.distance_rear_m > fabs(unsigned_distance))
			){
				std::cout<< std::endl << "Distance reached " << data.distance_front_m << " " << data.distance_rear_m<<  std::endl;
				end_of_loop=true;
			}

		}

		std::cout << std::endl;

	}



}
