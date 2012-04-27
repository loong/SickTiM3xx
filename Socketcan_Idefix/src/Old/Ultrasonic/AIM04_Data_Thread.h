/*
 * Configuraton.h
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#ifndef AIM04_DATA_THREAD_H_
#define AIM04_DATA_THREAD_H_

#include <boost/date_time/posix_time/posix_time.hpp>

#include "CAN_SDO_Socket.h"
#include "CAN_PDO_Socket.h"
#include "CAN_EDS_SDO.h"
#include "CAN_Config_EDS.h"
#include "Thread.h"

#include "EstAIM04_Profile.h"


#include "Idefix_Config_Parameters.h"
#include "CAN_types_and_tools.h"



namespace idefix {


	/**
	 * \brief Class collects data from the Ultrasonic sensors and provides access for the Idefix-Player Driver to this Data
	 */

	class AIM04_Data_Thread : public CAN_PDO_Socket , public Thread{


	public:
		AIM04_Data_Thread()throw();
		virtual ~AIM04_Data_Thread()throw();


		virtual void process();


		void config(uint_t analog_delta_interrupt_time =0x40);


		void get_ranges(float* my_ranges);
		//I_C::ultrasonic_data_t get_data();

		void setCanInterfaceName(std::string _can_interface_name);


	private:

		boost::mutex my_mutex;

		CAN_SDO_Socket sdo_socket;
		//I_C::ultrasonic_data_t data;

		bool aim04_is_configured;

		float ranges[4];
		float tometers(unsigned int val);

		std::string can_interface_name;
	};

}  // namespace idefix


#endif /* AIM04_DATA_THREAD_H_ */
