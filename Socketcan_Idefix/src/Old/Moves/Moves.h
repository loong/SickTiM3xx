/*
 * Moves.h
 *
 *  Created on: 02.06.2010
 *      Author: locher
 */

#ifndef MOVES_H_
#define MOVES_H_

#include <iostream>

#include <boost/date_time/posix_time/ptime.hpp>

#include <player-3.0/libplayerc++/playerc++.h>

#include "Idefix_Data_Structs.h"
#include "Idefix_Config_Parameters.h"

namespace idefix {



class Moves {


public:
	Moves(PlayerCc::PlayerClient& player_client,
			PlayerCc::OpaqueProxy& opaque_proxy);

	virtual ~Moves();

	void move_parallel_profile_position(double signed_distance,
			double steering_angle);
	void move_circle_profile_position(double signed_distance,
			double steering_angle);

	void move_parallel_profile_velocity(double unsigned_distance,
			double signed_velocity, double steering_angle);
	void move_circle_profile_velocity(double unsigned_distance,
			double signed_velocity, double steering_angle);

	/**
	 * Minimaler Kurvendurchmesser ist 130 cm
	 */
	void move_90_degree_forward_left_profile_position();
	void move_90_degree_forward_right_profile_position();
	void move_90_degree_backward_left_profile_position();
	void move_90_degree_backward_right_profile_position();
	void move_W_forwardright_profile_position();
	void move_W_backwardright_profile_position();
	void move_W_forwardleft_profile_position();
	void move_W_backwardleft_profile_position();

	void move_Y_forwardright_profile_position();
	void move_Y_backwardright_profile_position();

	void move_Y_forwardleft_profile_position();
	void move_Y_backwardleft_profile_position();

	void move_turn_same_row_profile_position();

	void move_90_degree_forward_left_profile_velocity(double velocity);
	void move_90_degree_forward_right_profile_velocity(double velocity);
	void move_90_degree_backward_left_profile_velocity(double velocity);
	void move_90_degree_backward_right_profile_velocity(double velocity);

private:

	typedef enum {
		MOVE_TYPE_PARALLEL, MOVE_TYPE_CIRCLE
	} move_type_t;

	PlayerCc::PlayerClient& my_player_client_ref;
	PlayerCc::OpaqueProxy& my_opaque_proxy_ref;

	/**
	 * Switch to profile position mode for drives
	 * to move a desired distance
	 * velocity and acceleration depend on global parameter settings
	 *
	 * Method blocks client loop until velocity of drives is zero
	 *
	 * @param signed_distance is the desired distance with sign idicating the desired direction
	 * @param steering_angle is between -1 and 1
	 * @param move_type MOVE_TYPE_PARALLEL or MOVE_TYPE_CIRCLE
	 *
	 *
	 */

	void move_distance_with_profile_position(double signed_distance,
			double steering_angle, move_type_t move_type);

	/**
	 * Switch to profile velocity mode for drives
	 * to move a desired distance with desired velocity
	 *
	 *
	 * Method blocks client loop until both drives have reached the
	 * desired distance
	 *
	 * @param distance is the absolute desired distance for the move
	 * @param signed_velocity is the desired velocity,
	 * sign is indicating the desired direction (forward or reverse)
	 * @param steering_angle is between -1 and 1
	 * @param move_type MOVE_TYPE_PARALLEL or MOVE_TYPE_CIRCLE
	 *
	 *
	 */

	void move_distance_with_profile_velocity(double unsigned_distance,
			double signed_velocity, double steering_angle,
			move_type_t move_type);

};

} //end namespace
#endif /* MOVES_H_ */
