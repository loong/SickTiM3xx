/*
 * CAN_SDO_Socket.h
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#ifndef CAN_SDO_SOCKET_H_
#define CAN_SDO_SOCKET_H_

#include <iostream>

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

#include "CAN_SDO.h"

namespace sfzcan {


	/**
	 * \brief Use this class to send CAN Service Data Objects SDOs.
	 * The sendSDO Method waits for the returned SDO answer
	 *
	 */

	class CAN_SDO_Socket {


	private:
		bool socket_is_bound;

		int my_socket;





	public:



		CAN_SDO_Socket()throw();
		virtual ~CAN_SDO_Socket()throw();

		virtual void connect_socket_to_can_interface(const std::string& can_interface_name)
			 throw(E_CAN_Bind_Exception);



		// SDO

		virtual void sendSDO(CAN_SDO& p, timeout_t max_wait_time_mus=200000)
			throw(E_CAN_Exception);


	};




}  // namespace sfzcan

#endif /* CAN_SDO_SOCKET_H_ */
