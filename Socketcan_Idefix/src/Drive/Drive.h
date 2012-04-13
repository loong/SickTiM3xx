/*
 * Drive.h
 *
 *  Created on: 08.04.2010
 *      Author: locher
 */

#ifndef DRIVE_H_
#define DRIVE_H_

#include "Idefix_Data_Structs.h"
#include "CAN_PDO_Socket.h"
#include "Drive_Configuration.h"

namespace idefix {


	/**
	 * \brief Class which represents the main drives
	 *
	 * Use this class to
	 * <ul>
	 * <li>set Velocity
	 * <li>request position and velocity
	 * <li>enable and disable drives
	 * </ul>
	 *
	 */



	class Drive :public CAN_PDO_Socket {

	friend class All_Drives;

	private:

		uint_t number_of_pdos_to_read;

		profile_t actual_profile;

		boost::mutex my_mutex;


		std::string can_interface;

		CAN_SDO_Socket my_sdo_socket;


		//msgdata_t current;
		msgdata_t position_qc;
		msgdata_t velocity_qc;
		msgdata_t statusword;

	public:
		virtual void read_next_pdos()throw(std::exception);

	private:
		msgdata_t last_position_qc;
		volatile double actual_distance_m;
		double velocity_m;

		bool first_time;

//		msgdata_t move_distance_start_value_qc;
//		double move_distance_desired_value_m;


	public:


		msgdata_t get_position();

		void configure_Drive()throw(std::exception);


		void set_Velocity_m_s(double value)throw(std::exception);
		void move_distance(double distance_m)throw(std::exception);

		void disable_drive()throw(std::exception);;
		void enable_drive()throw(std::exception);;

		void reset_distance_measuring();
		double get_driven_distance_m();
		double get_velocity_mps();

//		bool get_position_reached();

		Drive(canid_t nodeId);
		virtual ~Drive()throw();

		void setCanInterfaceName(std::string _can_interface_name);
	};

}  // namespace idefix


#endif /* DRIVE_H_ */
