/*
 * Configuraton.h
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <boost/date_time/posix_time/posix_time.hpp>

#include "CAN_SDO_Socket.h"
#include "CAN_PDO_Socket.h"
#include "CAN_EDS_SDO.h"
#include "CAN_Config_EDS.h"
#include "Maxon_Epos_24_5_Profile.h"


#include "Idefix_Config_Parameters.h"
#include "EPOS_Emergency_Thread.h"



namespace idefix {



	/**
	 * \brief Class provides static methods of CAN SDO Sequences to configure both drive and steering
	 * Parent class of Drive_Configuration and Steering Configuration
	 */

	class Configuration {
	public:
		Configuration();
		virtual ~Configuration();


		static bool isStatusFault(CAN_SDO_Socket& n,CAN_PDO_Socket& t)throw(E_CAN_IO_Exception,E_SDO_Exception);
		static void resetFault(CAN_SDO_Socket&  n, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception,E_SDO_Exception);

		static void enable_Device(CAN_SDO_Socket& n,CAN_PDO_Socket& t)throw(E_CAN_IO_Exception,E_SDO_Exception);
		static void disable_Device(CAN_PDO_Socket& t)throw(E_CAN_IO_Exception,E_SDO_Exception);


		static void wait_for_reaching_Preoperational_Mode(CAN_SDO_Socket& n, CAN_PDO_Socket& t)throw(E_CAN_IO_Exception,E_SDO_Exception);


	};

}  // namespace idefix


#endif /* CONFIGURATION_H_ */
