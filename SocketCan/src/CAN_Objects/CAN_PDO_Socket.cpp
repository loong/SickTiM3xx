/*
 * CAN_PDO_Socket.cpp
 *
 *  Created on: 10.04.2010
 *      Author: locher
 */

#include "CAN_PDO_Socket.h"

namespace sfzcan {

CAN_PDO_Socket::CAN_PDO_Socket(canid_t node_id) throw () :
	filter_pdos_mask(FILTER_PDO1_MASK | FILTER_PDO2_MASK| FILTER_PDO3_MASK | FILTER_PDO4_MASK),
	socket_is_bound(false),
	node_id(node_id)

{

	read_timeout_timeval.tv_sec = 0;
	read_timeout_timeval.tv_usec = 500000;

	// Avoid that an Exception can interrupt the Konstruktion
	try {

		my_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (my_socket < 0) {

			perror("socket");
			std::cout << "Failed to create Socket:" << std::endl;
			exit(EXIT_FAILURE);
		}

	} catch (std::exception &e) {

		std::cout << "Caught Exceptin in CAN_Node Konstruktor: " << e.what()
				<< std::endl;

	} catch (...) {

		std::cout << "Caught Exceptin in CAN_Node Konstruktor" << std::endl;

	}

}

CAN_PDO_Socket::~CAN_PDO_Socket() throw () {

	try{
		if( close(my_socket)==-1){
			perror("CAN_SDO_Socket: Closing Socket");
		}
	}catch(...){
		std::cerr << "CAN_SDO_Socket Destruktor: Error occured while closing socket  " << std::endl;
	}


}

void CAN_PDO_Socket::bind_socket_to_can_interface(
		const std::string& can_interface_name) throw (E_CAN_Bind_Exception) {

	if (!socket_is_bound) {
		sockaddr_can addr;
		ifreq ifr;

		addr.can_family = AF_CAN;
		strcpy(ifr.ifr_name, can_interface_name.c_str());
		if (ioctl(my_socket, SIOCGIFINDEX, &ifr) < 0) {

			perror("SIOCGIFINDEX");

			throw(E_CAN_Bind_Exception("CAN_Node: ioctrl exception"));

		}

		addr.can_ifindex = ifr.ifr_ifindex;

		if (!addr.can_ifindex) {
			perror("invalid bridge interface");
			throw(E_CAN_Bind_Exception("CAN_Node: No ifindex"));
		}

		/*
		 *  timeout für read festlegen
		 */
		//			timeval timeout;
		//			timeout.tv_sec=0;
		//			timeout.tv_usec=read_timeout_timeval;


		if (setsockopt(my_socket, SOL_SOCKET, SO_RCVTIMEO,
				&read_timeout_timeval, sizeof(read_timeout_timeval)) == -1) {
			perror("setsockopt: ");
		}

		/*
		 * Nur die Rx-PDO Nachrichten empfangen,für die ein Filter eingerichtet wurde
		 */

		can_filter my_filter[4];

		int anz_filter=0;

		for(int i=0  ; i< 4 ; i++ ) { // maximal vier PDOs

			if ( (filter_pdos_mask>>i)& 1  ){

				my_filter[anz_filter].can_id = (i+1)*0x100+ 0x80 + node_id;
				my_filter[anz_filter].can_mask = CAN_SFF_MASK;

				std::cout << "CAN_PDO_socket: Filter for node id "
						<< std::hex << my_filter[anz_filter].can_id << std::dec<< " added" << std::endl;

				++anz_filter;

			}

			if(anz_filter==0){ // Block all PDOs

				my_filter[anz_filter].can_id = 0 | CAN_INV_FILTER;
				my_filter[anz_filter].can_mask = 0;

				anz_filter++;
			}
		}



//		my_filter[0].can_id = 0x180 + node_id;
//		my_filter[0].can_mask = CAN_SFF_MASK;
//
//		my_filter[1].can_id = 0x280 + node_id;
//		my_filter[1].can_mask = CAN_SFF_MASK;
//
//		my_filter[2].can_id = 0x380 + node_id;
//		my_filter[2].can_mask = CAN_SFF_MASK;
//
//		my_filter[3].can_id = 0x480 + node_id;
//		my_filter[3].can_mask = CAN_SFF_MASK;

		if (setsockopt(my_socket, SOL_CAN_RAW, CAN_RAW_FILTER, &my_filter,
				sizeof(can_filter)*anz_filter) == -1) {

			perror("setsocketopt: ");
			throw(E_CAN_Bind_Exception(
					"CAN_PDO_Socket: Exception setting filters with setsockopt "));
		}

		if (bind(my_socket, (sockaddr *) (&addr), sizeof(addr)) < 0) {
			perror("Bind");
			throw(E_CAN_Bind_Exception(
					"CAN_PDO_Socket: Exception while binding socket"));
		}

		socket_is_bound = true;
	}
}

/*
 * **************************************************
 * NMT Messages
 * **************************************************
 */

void CAN_PDO_Socket::sendStartAllNodesMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0; // COB-ID
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x01; // Command
	msg.data[1] = (__u8 ) 0x0; // All Nodes


	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("start all Nodes: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendStopAllNodesMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;
	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0; // COB-ID
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x02; // Command
	msg.data[1] = (__u8 ) 0x0; // All Nodes

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("Stop all nodes: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendStartRemoteNodeMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0; // COB-ID
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x01; // Command
	msg.data[1] = (__u8 ) node_id; // Node

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("start Node: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendStopRemoteNodeMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0; // COB-ID
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x02; // Command
	msg.data[1] = (__u8 ) node_id; // Node

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("stop Node: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendEnterPreOperationalMode() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0;
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x80;
	msg.data[1] = (__u8 ) node_id;

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("Enter preoperational Mode: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}

}

void CAN_PDO_Socket::sendResetRemoteNodeMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0;
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x81;
	msg.data[1] = (__u8 ) node_id;

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("Reset remote Node: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendResetCommunicationMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x0;
	msg.can_dlc = 2;
	msg.data[0] = (__u8 ) 0x82;
	msg.data[1] = (__u8 ) node_id;

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("reset Communication: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}
}

void CAN_PDO_Socket::sendSyncTelegrammMSG() throw (E_CAN_IO_Exception) {
	can_frame msg;

	memset(&msg, 0, sizeof(msg));

	msg.can_id = (canid_t) 0x80;
	msg.can_dlc = 1;
	msg.data[0] = (__u8 ) 0x0;

	if (write(my_socket, &msg, sizeof(msg)) != sizeof(msg)) {

		perror("send sync Telegramm: ");
		throw(E_CAN_IO_Exception("Write Error"));

	}

}

/*
 * **************************************************
 * PDO Messages
 * **************************************************
 */

void CAN_PDO_Socket::sendPDO(CAN_PDO& pdo) throw (E_CAN_IO_Exception) {

	if (write(my_socket, pdo.get_can_frame_reference(), sizeof(can_frame))
			!= sizeof(can_frame)) {
		perror("CAN_NODE::sendPDO: ");
		throw(E_CAN_IO_Exception("CAN_NODE::sendPDO: IO_Exeption"));
	}

}

void CAN_PDO_Socket::set_read_timeout(unsigned int secs, unsigned int mus)
		throw (E_CAN_Bind_Exception) {

	if (!socket_is_bound) {

		if (mus >= 1000000) {
			std::cerr << "CAN_PDO_socket: Value for mus exceeds allaow range"
					<< std::endl;
			throw(E_CAN_Bind_Exception(
					"CAN_PDO_socket: Value for mus exceeds allaow range"));

		}

		read_timeout_timeval.tv_usec = mus;
		read_timeout_timeval.tv_sec = secs;

	} else {
		std::cerr
				<< "CAN_PDO_socket: Read Timeout can changed only before binding the Socket"
				<< std::endl;
		throw(E_CAN_Bind_Exception(
				"CAN_PDO_socket Error: Read Timeout can changed only before binding the Socket"));
	}
}

void CAN_PDO_Socket::set_filter_pdo_mask(unsigned char mask)
		throw (E_CAN_Bind_Exception) {

	if (!socket_is_bound) {
		filter_pdos_mask=mask;
	} else {
		std::cerr
				<< "CAN_PDO_socket: Read Timeout can changed only before binding the Socket"
				<< std::endl;
		throw(E_CAN_Bind_Exception(
				"CAN_PDO_socket Error: Read Timeout can changed only before binding the Socket"));
	}
}

} // namespace sfzcan
