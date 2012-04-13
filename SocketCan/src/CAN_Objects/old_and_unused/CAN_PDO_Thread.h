/*
 * CAN_Node.h
 *
 *  Created on: 29.03.2010
 *      Author: locher
 */

#ifndef CAN_PDO_THREAD_H_
#define CAN_PDO_THREAD_H_


#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>


#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>


#include <linux/can/raw.h>

#include "CAN_types_and_tools.h"

#include "Thread.h"
#include "Debug_Stream.h"
#include "CAN_PDO.h"

#include "CAN_PDO_Socket.h"


namespace sfzcan{







	class CAN_PDO_Thread : public CAN_PDO_Socket, public Thread{

	private:


		/**
		 * \brief called from Thread
		 * calls next_can_frame_to_process
		 */
		virtual void process();

	protected:
		/**
		 * \brief Overwrite this Method to process received can frames
		 */
		virtual void next_can_frame_to_process(const can_frame& f, timeval& tv);

	public:
		explicit CAN_PDO_Thread(canid_t node_id)throw();
		virtual ~CAN_PDO_Thread() throw();



	};

}
#endif /* CAN_PDO_THREAD_H_ */
