/*
 * Sync_Thread.cpp
 *
 *  Created on: 28.11.2010
 *      Author: locher
 */

#include "CAN_Sync_Telegram.h"

namespace sfzcan {

	CAN_Sync_Telegram::CAN_Sync_Telegram()throw():
			can_interface_name("")
	{



	}

	CAN_Sync_Telegram::~CAN_Sync_Telegram() {


		if(	close(my_bcm_sync_telegram_socket) ==-1){
			perror("Sync_Telegram Destruktor: ");

		}

	}

	void CAN_Sync_Telegram::set_can_interface(std::string can_interface){
		can_interface_name=can_interface;
	}


	void CAN_Sync_Telegram::start_sync_telegram(timeval period_timval)throw(E_CAN_Bind_Exception){

		if(can_interface_name.length()==0){
			throw(E_CAN_Bind_Exception("CAN_Sync_Telegram: \n\tyou have to set can_interface name before starting the sync telegram") );
		}

		can_frame sync_frame;

		memset(&sync_frame,0,sizeof(sync_frame));
		sync_frame.can_id=0x80;
		sync_frame.can_dlc=1;
		sync_frame.data[0]=0;





		// Broadcast Manager Can_Socket configurieren fuer
		// das Synctelegramm
		memset(&bcm_to_send_sync, 0, sizeof(bcm_to_send_sync));
		bcm_to_send_sync.msg_head.opcode  = TX_SETUP;
		bcm_to_send_sync.msg_head.flags= SETTIMER | STARTTIMER;
		bcm_to_send_sync.msg_head.ival1.tv_sec=0;
		bcm_to_send_sync.msg_head.ival1.tv_usec=0;
		bcm_to_send_sync.msg_head.ival2.tv_sec=period_timval.tv_sec;
		bcm_to_send_sync.msg_head.ival2.tv_usec=period_timval.tv_usec; //
		bcm_to_send_sync.msg_head.count=0;	//
		bcm_to_send_sync.msg_head.nframes=1; //
		bcm_to_send_sync.sync_frame=sync_frame;



		my_bcm_sync_telegram_socket=socket(PF_CAN,SOCK_DGRAM,CAN_BCM);

		if(my_bcm_sync_telegram_socket <0){

			perror("sync socket") ;
			std::cout << "Failed to create Socket:" <<  std::endl;
			//exit(EXIT_FAILURE);
		}



		// Bind socket to can interface
		sockaddr_can addr;
		ifreq ifr;

		addr.can_family = AF_CAN;

		strcpy(ifr.ifr_name, can_interface_name.c_str());

		if (ioctl(my_bcm_sync_telegram_socket, SIOCGIFINDEX, &ifr) < 0){

			perror("SIOCGIFINDEX");

		}

		addr.can_ifindex = ifr.ifr_ifindex;

		if (!addr.can_ifindex) {
			perror("invalid bridge interface");
		}


		if(connect(my_bcm_sync_telegram_socket, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
			perror("Bind");
		}

		if(write(my_bcm_sync_telegram_socket,&bcm_to_send_sync, sizeof( bcm_to_send_sync ))<0){

			std::cout << "Write Error " << std::endl;

			perror("All Drives: startSyncTelegramm");


		}

	}





}  // namespace sfzcan

