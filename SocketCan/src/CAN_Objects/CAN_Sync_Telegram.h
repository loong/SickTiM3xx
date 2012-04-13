/*
 * Sync_Thread.h
 *
 *  Created on: 28.11.2010
 *      Author: locher
 */

#ifndef CAN_SYNC_TELEGRAM_H_
#define CAN_SYNC_TELEGRAM_H_


#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can/raw.h>
#include <linux/can/bcm.h>


#include "Thread.h"
#include "CAN_types_and_tools.h"

namespace sfzcan {

	/**
	 * Class creates an instance of a BCM_socket sending periodically the SYNC
	 * Telegram
	 *
	 * TODO: setter Method for setting period_timeval
	 * TODO: Implement an Instance Counter, throwing Errors if more than one Instance is alive
	 *
	 *
	 */



	class CAN_Sync_Telegram {

	private:

		std::string can_interface_name;

		struct {
			struct bcm_msg_head msg_head;
			struct can_frame sync_frame;
		} bcm_to_send_sync;


		int my_bcm_sync_telegram_socket;




	public:

		CAN_Sync_Telegram()throw();
		virtual ~CAN_Sync_Telegram();


		void set_can_interface(std::string can_interface);
		void start_sync_telegram(timeval period_timval)throw(E_CAN_Bind_Exception);


	};


}  // namespace sfzcan

#endif /* CAN_SYNC_TELEGRAM_H_ */
