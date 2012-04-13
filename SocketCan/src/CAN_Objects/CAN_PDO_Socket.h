/*
 * CAN_PDO_Socket.h
 *
 *  Created on: 10.04.2010
 *      Author: locher
 */

#ifndef CAN_PDO_SOCKET_H_
#define CAN_PDO_SOCKET_H_

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



#include "CAN_PDO.h"


namespace sfzcan {


	/**
	 * \brief provides a Raw Socket for a Can-Node which filters
	 * only incoming PDOs  from this node
	 */

	class CAN_PDO_Socket {

	public:

		static const unsigned char BLOCK_ALL_PDOS_MASK=0;
		static const unsigned char FILTER_PDO1_MASK=(1<<0);
		static const unsigned char FILTER_PDO2_MASK=(1<<1);
		static const unsigned char FILTER_PDO3_MASK=(1<<2);
		static const unsigned char FILTER_PDO4_MASK=(1<<3);


	private:


		unsigned char filter_pdos_mask;

	protected:
			//uint_t number_of_pdos_to_read;



			bool socket_is_bound;

			timeval read_timeout_timeval;  // how many mycroseconds will read wait

			std::string can_interface_name;
			canid_t node_id;


			int my_socket;





	public:

		virtual void bind_socket_to_can_interface(const std::string& can_interface_name)
				 throw(E_CAN_Bind_Exception);



		virtual const canid_t get_node_id(){
				return node_id;

		}




		// NMT
		virtual void sendStartAllNodesMSG()throw(E_CAN_IO_Exception);
		virtual void sendStopAllNodesMSG()throw(E_CAN_IO_Exception);

		virtual void sendStartRemoteNodeMSG()throw(E_CAN_IO_Exception);
		virtual void sendStopRemoteNodeMSG()throw(E_CAN_IO_Exception);


		virtual void sendEnterPreOperationalMode()throw(E_CAN_IO_Exception);

		virtual void sendResetRemoteNodeMSG()throw(E_CAN_IO_Exception); // initialisierung
		virtual void sendResetCommunicationMSG()throw(E_CAN_IO_Exception); // initialisierung

		virtual void sendSyncTelegrammMSG(void)throw(E_CAN_IO_Exception);

		// PDO

		virtual void sendPDO(CAN_PDO& p)throw(E_CAN_IO_Exception);


		virtual int get_socket(){return my_socket;}
		virtual void set_read_timeout(unsigned int secs,unsigned int mus)throw(E_CAN_Bind_Exception);
		virtual void set_filter_pdo_mask(unsigned char mask)throw(E_CAN_Bind_Exception);


		CAN_PDO_Socket(canid_t node_id)throw();
		virtual ~CAN_PDO_Socket()throw();
	};
}  // namespace sfzcan

#endif /* CAN_PDO_SOCKET_H_ */
