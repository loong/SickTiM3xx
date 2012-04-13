/*
 * Drive_Configuration.h
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */

#ifndef STEERING_CONFIGURATION_H_
#define STEERING_CONFIGURATION_H_

#include <boost/date_time.hpp>

#include <auto_ptr.h>

#include "Idefix_Data_Structs.h"
#include "Idefix_Config_Parameters.h"

#include "Maxon_Epos_24_5_Profile.h"

#include "CAN_PDO.h"
#include "CAN_EDS_SDO.h"
#include "CAN_types_and_tools.h"
#include "CAN_Config_EDS.h"

#include "Configuration.h"


using namespace sfzcan;


namespace idefix{

	/**
	 * \brief Class provides static methods of CAN SDO Sequences to configure the steering
	 */





	class Steering_Configuration :public Configuration {



	public:



		Steering_Configuration();
		virtual ~Steering_Configuration();



		static void setProfilePositionMode(CAN_SDO_Socket& s,CAN_PDO_Socket& t,robo_mount_pos_t mount_pos)throw(E_CAN_IO_Exception);

		static void  currentThreshHoldPositiveSpeedHoming(CAN_SDO_Socket& s,CAN_PDO_Socket& t, robo_mount_pos_t mount_pos)throw(E_CAN_IO_Exception);

		static void  currentThreshHoldNegativeSpeedHoming(CAN_SDO_Socket& s,CAN_PDO_Socket& t, robo_mount_pos_t mount_pos)throw(E_CAN_IO_Exception);

		static void waitForReachingTargetPosition(CAN_SDO_Socket& s,CAN_PDO_Socket& t)throw(E_SDO_Exception);

		static void configure_Drive(CAN_SDO_Socket& s,CAN_PDO_Socket& t)throw(E_SDO_Exception);

		static void set_position(CAN_PDO_Socket& t, double value ,robo_mount_pos_t mount_pos)throw(E_SDO_Exception);
	};

} // namespace idefix


#endif /* STEERING_CONFIGURATION_H_ */
