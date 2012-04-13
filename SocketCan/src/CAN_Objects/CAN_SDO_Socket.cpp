/*
 * CAN_SDO_Socket.cpp
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#include "CAN_SDO_Socket.h"



namespace sfzcan {



	CAN_SDO_Socket::CAN_SDO_Socket() throw() :

		socket_is_bound(false)


	{

		// Avoid that an Exception can interrupt the Konstruktion
		try{

			// Broadcast Manager Socket einrichten
			my_socket=socket(PF_CAN,SOCK_DGRAM,CAN_BCM );
			if(my_socket <0){

				perror("socket") ;
				std::cout << "Failed to create Socket:" <<  std::endl;
				exit(EXIT_FAILURE);
			}





		}catch(std::exception &e){



			std::cout << "Caught Exceptin in CAN_Node Konstruktor: " << e.what() << std::endl;


		}catch(...){

			std::cout << "Caught Exceptin in CAN_Node Konstruktor" << std::endl;

		}



	}

	CAN_SDO_Socket::~CAN_SDO_Socket() throw() {

		try{
			if( close(my_socket)==-1){
				perror("CAN_SDO_Socket: Closing Socket");
			}
		}catch(...){
			std::cerr << "CAN_SDO_Socket Destruktor: Error occured while closing socket  " << std::endl;
		}


	}



	void CAN_SDO_Socket::connect_socket_to_can_interface(const std::string& can_interface_name)
			throw(E_CAN_Bind_Exception)
	{



		// bind the socket only once

		if(!socket_is_bound){

			sockaddr_can addr;
			ifreq ifr;

			addr.can_family = AF_CAN;

			strcpy(ifr.ifr_name, can_interface_name.c_str());

			if (ioctl(my_socket, SIOCGIFINDEX, &ifr) < 0){

				perror("SIOCGIFINDEX");

				throw(E_CAN_Bind_Exception("CAN_Node: ioctrl exception") );

			}

			addr.can_ifindex = ifr.ifr_ifindex;

			if (!addr.can_ifindex) {
				perror("invalid bridge interface");
				throw(E_CAN_Bind_Exception("CAN_Node: No ifindex") );
			}


			if(connect(my_socket, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
				perror("Bind");
				throw(E_CAN_Bind_Exception("CAN_Node: Exception while binding socket") );
			}

			socket_is_bound=true;
		}
	}






	/*
	 * **************************************************
	 * SDO Messages
	 * **************************************************
	 */

	/**
	 * \brief CAN_Node is a friend of CAN_SDO.
	 * sending SDO blocks until an answer  is received
	 * or until timeout. The SDO gets the received Message and the send status
	 *
	 */

	void CAN_SDO_Socket::sendSDO(CAN_SDO& sdo, timeout_t max_wait_time_mus)throw(E_CAN_Exception){

//		std::cout << "CAN_SDO_Socket: sending SDO " << std::endl;
//		std::cout.flush();

		// Socket Filter einrichten

		/*
		 *  timeout für read festlegen
		 */
//		timeval timeout;
//		timeout.tv_sec=0;
//		timeout.tv_usec=max_wait_time_mus;
//
//
//		if(setsockopt(my_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==-1){
//			perror("setsockopt: ");
//		}


		canid_t nodeid= sdo.my_can_frame_to_send.can_id &  0x7F;

		//dout << linestart << "Node ID " <<uint_t(nodeid) << sfzdebug::endl;

			// Überprüfen ob es sich um ein sdo handelt
			if(
					(  (sdo.my_can_frame_to_send.can_id )  ==0x600 +nodeid)
					&&
					(
							(sdo.my_can_frame_to_send.data[0] ==0x40)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x22)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x23)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x2B)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x2F)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x42)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x43)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x4B)
							||
							(sdo.my_can_frame_to_send.data[0] ==0x4F)

					)

			){ //sdo konfiguration is ok


				sdo.can_frame_is_sdo=true;

//				dout<< linestart <<" sdo  config ist ok " << sfzdebug::endl;


				struct {
					struct bcm_msg_head msg_head;
					struct can_frame frame;
				} txmsg_bcm_filter, sdo_bcm_to_send, rxmsg_bcm_received;


			// Einrichten des bcm Empfangsfilters

				memset(&txmsg_bcm_filter, 0, sizeof(txmsg_bcm_filter)); /* clear timers, nframes, etc. */

				txmsg_bcm_filter.msg_head.opcode  = RX_SETUP;
				// change canid from 0x600+node id to 0x580+nodeid
				txmsg_bcm_filter.msg_head.can_id = sdo.my_can_frame_to_send.can_id-0x80;
				txmsg_bcm_filter.msg_head.flags= RX_FILTER_ID |SETTIMER | STARTTIMER; // filter for can id and start Timer immediately
				txmsg_bcm_filter.msg_head.ival1.tv_sec=0;
				txmsg_bcm_filter.msg_head.ival1.tv_usec=max_wait_time_mus; // Timeout
				txmsg_bcm_filter.msg_head.nframes=1;
				// filter for the same  index und subindex
				txmsg_bcm_filter.frame.data[1]=sdo.my_can_frame_to_send.data[1];
				txmsg_bcm_filter.frame.data[2]=sdo.my_can_frame_to_send.data[2];
				txmsg_bcm_filter.frame.data[3]=sdo.my_can_frame_to_send.data[3];



				if (write(my_socket, &txmsg_bcm_filter, sizeof(txmsg_bcm_filter)) < 0){

					perror("CAN_SDO_Socket: Error while writing bcm Filter");
					throw(E_CAN_IO_Exception("CAN_SDO_Socket: Error while writing bcm Filter"));

				}


		// Senden des SDOs

				memset(&sdo_bcm_to_send, 0, sizeof(sdo_bcm_to_send)); /* clear timers, nframes, etc. */

				sdo_bcm_to_send.msg_head.opcode =TX_SEND;
				sdo_bcm_to_send.msg_head.can_id=sdo.my_can_frame_to_send.can_id;
				sdo_bcm_to_send.msg_head.nframes=1;
				sdo_bcm_to_send.frame=sdo.my_can_frame_to_send;


				if (write(my_socket, &sdo_bcm_to_send, sizeof(sdo_bcm_to_send)) < 0){

					perror("CAN_SDO_Socket: Error while writing SDO");
					throw(E_CAN_IO_Exception("CAN_SDO_Socket: Error while writing SDO"));

				}


		// Empfangen der SDO Antwort

				int nbytes= read(my_socket,&rxmsg_bcm_received,sizeof(rxmsg_bcm_received));


				if(nbytes==-1){

					perror("Socket Read: ");
					throw(E_CAN_IO_Exception("E_CAN_IO_Exception: \n\tCAN_SDO_Socket::sendSDO(): Error while reading RxMessage" ));
				}

				if( (rxmsg_bcm_received.msg_head.opcode &RX_TIMEOUT)== RX_TIMEOUT ){ // Read fehlgeschlagen



						sdo.messageAcknowledged=false;
						sdo.sdoDataReceived=false;
						sdo.timeoutOccurred=true;
						sdo.sdoAbortOccurred=true;



				}else{ //read war erfolgreich


					sdo.my_received_can_frame=rxmsg_bcm_received.frame;

					// receive time ist über ioctrl erhältlich mit timeval in mus
					timeval tv;
					ioctl(my_socket,SIOCGSTAMP, &tv);

					// alternativ mit timespec in nanos
		//			timespec ns;
		//			ioctl(my_socket,SIOCGSTAMPNS, &ns);

					sdo.messageAcknowledged=true;
					sdo.timeoutOccurred=false;

					// is can_frame an AbortMessage
					if(sdo.my_received_can_frame.data[0]==0x80){
						sdo.sdoAbortOccurred=true;
					}

					// is can_frame an upload respose?
					if(
							(sdo.my_received_can_frame.data[0]==0x60)
							||
							(sdo.my_received_can_frame.data[0]==0x42)
							||
							(sdo.my_received_can_frame.data[0]==0x43)
							||
							(sdo.my_received_can_frame.data[0]==0x4B)
							||
							(sdo.my_received_can_frame.data[0]==0x4F)

					){
						sdo.sdoDataReceived=true;
					}



				} //end erfolgreiches read







			}else{ // wrong  sdo konfiguration

				sdo.can_frame_is_sdo=false;
				sdo.messageAcknowledged=false;
				sdo.sdoDataReceived=false;
				sdo.timeoutOccurred=true;
				sdo.sdoAbortOccurred=true;
			}


			// Auswerten der SDO-Flags

			if(!sdo.can_frame_is_sdo){
				throw(E_SDO_Exception("CAN_SDO_Socket: sendSDO Exception: can_frame is no sdo"));
			}

			if(sdo.timeoutOccurred){
				throw(E_SDO_Exception("CAN_SDO_Socket: sendSDO Exception: timeout"));
			}

			if(sdo.sdoAbortOccurred){
				std::cerr  << "sendSDO: Abort "
						<<sdo.get_CANopen_Profile_301_Error_String() << std::endl;

				throw(E_SDO_Exception("CAN_SDO_Socket: sendSDO Exception: MessageAborted"));
			}




			// Löschen des Empfangsfilters


			struct {
				struct bcm_msg_head msg_head;
			} bcm_to_delete_filter;

			memset(&bcm_to_delete_filter,0,sizeof(bcm_to_delete_filter));

			bcm_to_delete_filter.msg_head.opcode=RX_DELETE;
			bcm_to_delete_filter.msg_head.can_id=sdo.my_can_frame_to_send.can_id-0x80;

			if (write(my_socket, &bcm_to_delete_filter, sizeof( bcm_to_delete_filter )) < 0){

				perror("CAN_SDO_Socket: deleting rx-Filter");
				throw(E_SDO_Exception("CAN_SDO_Socket: Write  Error while deleting rx-Filter"));

			}


	}


}  // namespace sfzcan

