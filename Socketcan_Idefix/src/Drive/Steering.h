/*
 * Steering.h
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#ifndef STEERING_H_
#define STEERING_H_

#include "CAN_PDO_Socket.h"
#include "Steering_Configuration.h"

namespace idefix {



	/**
	 * \brief Class which represents the steering drives
	 *
	 * Use this class to
	 * <ul>
	 * <li>set Position
	 * <li>request position
	 * <li>enable and disable drives
	 * <li>start homing
	 * </ul>
	 *
	 */



	class Steering : public CAN_PDO_Socket {

		friend class All_Drives;

	private:

		uint_t number_of_pdos_to_read;

		robo_mount_pos_t mount_pos;

		std::string can_interface;

		CAN_SDO_Socket my_sdo_socket;


		//msgdata_t current;
		msgdata_t position;
		msgdata_t velocity;
		msgdata_t statusword;


		virtual void read_next_pdos()throw(std::exception);;

		void bind_sockets();
		bool sockets_bound;


	public:

		msgdata_t get_position();
		msgdata_t get_velocity();

		void configure_Drive()throw(std::exception);
		void start_homing()throw(std::exception);

		void set_angle(double value);
		void disable_drive()throw(std::exception);;
		void enable_drive()throw(std::exception);;


		explicit Steering(canid_t nodeId,robo_mount_pos_t _mount_pos);
		virtual ~Steering()throw();

		void setCanInterfaceName(std::string _can_interface_name);
	};


}  // namespace idefix

#endif /* STEERING_H_ */
