/*
 * Drive_Configuration.h
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */

#ifndef DRIVE_CONFIGURATION_H_
#define DRIVE_CONFIGURATION_H_


#include <auto_ptr.h>

#include "Maxon_Epos_24_5_Profile.h"

#include "CAN_PDO_Socket.h"
#include "CAN_SDO_Socket.h"
#include "CAN_EDS_SDO.h"
#include "CAN_types_and_tools.h"
#include "CAN_Config_EDS.h"

#include "Configuration.h"

using namespace sfzcan;


namespace idefix{

	/**
	 * \brief Class provides static methods of CAN SDO Sequences to configure the drives
	 */


	class Drive_Configuration : public Configuration{



	public:
		Drive_Configuration();
		virtual ~Drive_Configuration();

		static void configure_Drive(CAN_SDO_Socket& n, CAN_PDO_Socket& t)throw(E_SDO_Exception);

		static void setProfileVelocityMode(CAN_SDO_Socket& n, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception);
		static void setProfilePositionMode(CAN_SDO_Socket& n, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception);



		static void setVelocity_rpm(CAN_PDO_Socket& t,msgdata_t value_rpm)throw(E_CAN_IO_Exception);

		static void moveDistance(CAN_PDO_Socket& t,msgdata_t quadcounts)throw(E_CAN_IO_Exception);


	};

} // namespace idefix


#endif /* DRIVE_CONFIGURATION_H_ */
