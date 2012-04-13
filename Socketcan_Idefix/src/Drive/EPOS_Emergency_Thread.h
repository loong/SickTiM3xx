/*
 * EPOS_CAN_Node.h
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */

#ifndef EPOS_EMERGENCY_THREAD_H_
#define EPOS_EMERGENCY_THREAD_H_


#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can/raw.h>
#include <linux/can/bcm.h>


#include "CAN_types_and_tools.h"
#include "Idefix_Config_Parameters.h"

#include "Thread.h"

namespace idefix {



	using namespace sfzcan;

	/**
	 * \brief Thread which listens to CAN PDOs of drives and loggs emergency Messages of the EPOS Drives
	 *
	 */

	class EPOS_Emergency_Thread: public Thread{


	private:
		std::string can_interface_name;
		bool socket_is_bound;

		int my_socket;

		void process();


		void connect_socket();


	public:
		static void report_EPOS_error(const can_frame& f);

		EPOS_Emergency_Thread(std::string _can_interface_name=CAN_INTERFACE_NAME);
		virtual ~EPOS_Emergency_Thread()throw();

		void setCanInterfaceName(std::string _can_interface_name);
	};

}  // namespace idefix

#endif /* EPOS_EMERGENCY_THREAD_H_ */
