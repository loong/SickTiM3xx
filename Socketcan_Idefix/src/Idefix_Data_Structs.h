/*
 * Idefix_Data_Structs.h
 *
 *  Created on: 03.05.2010
 *      Author: locher
 */

#ifndef IDEFIX_DATA_STRUCTS_H_
#define IDEFIX_DATA_STRUCTS_H_

#include <sys/types.h>

namespace idefix {

	typedef enum{MOUNT_POS_FRONT,MOUNT_POS_REAR} robo_mount_pos_t;

	typedef enum{PROFILE_VELOCITY,PROFILE_POSITION} profile_t;


	typedef unsigned int gameped_button_mask_t;

	// Reserved Buttons for Driver : Button 1 2 3 4 5 and 6



	static const gameped_button_mask_t CLIENT_BUTTON_7_MASK=1<<6;
	static const gameped_button_mask_t CLIENT_BUTTON_8_MASK=1<<7;
	static const gameped_button_mask_t CLIENT_BUTTON_9_MASK=1<<8;
	static const gameped_button_mask_t CLIENT_BUTTON_10_MASK=1<<9;

	static const gameped_button_mask_t ALL_CLIENT_BUTTONS_MASK=
			( CLIENT_BUTTON_7_MASK | CLIENT_BUTTON_8_MASK |
					 CLIENT_BUTTON_9_MASK | CLIENT_BUTTON_10_MASK );


	/**
	 * \brief Struct for the Opaque Interface:  Command Data transported from Client to Player
	 */

	typedef struct opaque_cmd{

		double new_velocity;
		double move_distance_m;
		profile_t drive_profile;
		double new_angle_front;
		double new_angle_rear;
		bool reset_distance;


	}opaque_cmd_t;

	/**
	 * \brief Struct for Opaque Interface:  State Data transported from Player to Client
	 */



	typedef struct opaque_data{

		double distance_rear_m;
		double distance_front_m;
		double velocity_front_mps;
		double velocity_rear_mps;
		double steering_angle_front;
		double steering_angle_rear;
		gameped_button_mask_t pressed_gamepad_buttons_mask;
		bool gamepad_mode_active;



	}opaque_data_t;



}  // namespace idefix


#endif /* IDEFIX_DATA_STRUCTS_H_ */
