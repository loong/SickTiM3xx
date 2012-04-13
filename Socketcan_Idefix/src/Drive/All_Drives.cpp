/*
 * All_Drives.cpp
 *
 *  Created on: 08.04.2010
 *      Author: locher
 */



#include "All_Drives.h"

namespace idefix {
All_Drives::All_Drives():
		Thread("All_Drives_Thread"),
		can_interface_name(CAN_INTERFACE_NAME),
		sync_socket_is_bound(false),
		bcm_socket_is_bound(false),
		drives_configured(false),
		is_homing_state_running(false),
		D_front(I_C::NODE_ANTRIEB_VORNE),
		D_rear(I_C::NODE_ANTRIEB_HINTEN),
		S_front(I_C::NODE_LENKUNG_VORNE,MOUNT_POS_FRONT),
		S_rear(I_C::NODE_LENKUNG_HINTEN,MOUNT_POS_REAR)
	{



		// Broadcast Manager Can_Socket configurieren fuer
		// das Synctelegramm
		memset(&bcm_to_filter_pdos, 0, sizeof(bcm_to_filter_pdos));
		bcm_to_filter_pdos.msg_head.opcode  = RX_SETUP;
		bcm_to_filter_pdos.msg_head.flags= RX_FILTER_ID |SETTIMER | STARTTIMER;
		bcm_to_filter_pdos.msg_head.ival1.tv_sec=0;
		bcm_to_filter_pdos.msg_head.ival1.tv_usec=0;
		bcm_to_filter_pdos.msg_head.ival2.tv_sec=0;
		bcm_to_filter_pdos.msg_head.ival2.tv_usec=0; //
		bcm_to_filter_pdos.msg_head.count=1;	//
		bcm_to_filter_pdos.msg_head.nframes=0; //




		my_bcm_socket=socket(PF_CAN,SOCK_DGRAM,CAN_BCM );
			if(my_bcm_socket <0){

				perror("All Drives: bcm_socket") ;
				std::cout << "Failed to create Socket:" <<  std::endl;
				//exit(EXIT_FAILURE);
			}


		memset(&sync_frame,0,sizeof(sync_frame));
		sync_frame.can_id=0x80;
		sync_frame.can_dlc=1;
		sync_frame.data[0]=0;

		my_sync_telegram_socket=socket(PF_CAN,SOCK_RAW,CAN_RAW );
			if(my_sync_telegram_socket <0){

				perror("AllDrives sync socket") ;
				std::cout << "Failed to create Socket:" <<  std::endl;
				//exit(EXIT_FAILURE);
			}


		drives_nodes[0]=I_C::NODE_ANTRIEB_HINTEN;
		drives_nodes[1]=I_C::NODE_ANTRIEB_VORNE;
		drives_nodes[2]=I_C::NODE_LENKUNG_HINTEN;
		drives_nodes[3]=I_C::NODE_LENKUNG_VORNE;


		set_warn_if_process_duration_lasts_longer_then_sleep_time(true);
		setSleepTime(I_C::ALL_DRIVES_THREAD_SLLEP_TIME);

	}

	All_Drives::~All_Drives() {

		stop_the_thread();
		try{

			disable_drives();

		}catch(std::exception e){
			std::cerr<<"All_Drives::Destruktor: "<<  e.what() << std::endl;
		}

		if( close(my_bcm_socket)==-1){
			perror("CAN_SDO_Socket: Closing Socket");
		}

		if( close(my_sync_telegram_socket)==-1){
			perror("CAN_SDO_Socket: Closing Socket");
		}



	}

	void All_Drives::sendSyncTelegramm()throw(std::exception){


			if(!sync_socket_is_bound){

				sockaddr_can addr;
				ifreq ifr;

				addr.can_family = AF_CAN;
				strcpy(ifr.ifr_name, can_interface_name.c_str());
				if (ioctl(my_sync_telegram_socket, SIOCGIFINDEX, &ifr) < 0){

					perror("All Drives SIOCGIFINDEX");

					throw(E_CAN_Bind_Exception("All Drives: ioctrl exception") );

				}

				addr.can_ifindex = ifr.ifr_ifindex;

				if (!addr.can_ifindex) {
					perror("All Drives: invalid bridge interface");
					throw(E_CAN_Bind_Exception("All Drives:  No ifindex") );
				}




				if(bind(my_sync_telegram_socket, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
					perror("Bind");
					throw(E_CAN_Bind_Exception("All Drives:  Exception while binding socket") );
				}

				sync_socket_is_bound=true;
			}

			if( write(my_sync_telegram_socket,&sync_frame,sizeof(sync_frame)) ==-1 ){

				perror("All_Drives: sned sync socket");

			}
	}




	void All_Drives::config_Drives(){

		std::cout << "config_Drives" << std::endl;

		boost::mutex::scoped_lock sl(my_mutex);

		if(!drives_configured){

			try{
				D_front.configure_Drive();
				D_rear.configure_Drive();
				S_front.configure_Drive();
				S_rear.configure_Drive();

				start_the_thread();

				drives_configured=true;

			}catch(std::exception e){
				std::cerr<< "All_Drives::config_Drives(): " <<e.what() << std::endl;
			}

		}
		std::cout << "config_Drives end " << std::endl;

	}


	void All_Drives::start_homing(robo_mount_pos_t mount_pos){

//		boost::mutex::scoped_lock sl(my_mutex);

		try{
			if(mount_pos==MOUNT_POS_FRONT){
				S_front.start_homing();
			}
			if(mount_pos==MOUNT_POS_REAR){
				S_rear.start_homing();
			}


		}catch(std::exception e){
			std::cerr<< "All_Drives::start_homing(): " <<e.what() << std::endl;
		}

	}


	void All_Drives::set_velocity_ms(double val){

//		boost::mutex::scoped_lock sl(my_mutex);

		if(drives_configured){

			try{
				D_front.set_Velocity_m_s(val);
				D_rear.set_Velocity_m_s(val);

			}catch(std::exception e){
				std::cerr<< "All_Drives::set_velocity(): " <<e.what() << std::endl;
			}

		}
	}


	void All_Drives::set_move_distance_m(double val){

//		boost::mutex::scoped_lock sl(my_mutex);

		D_front.move_distance(val);
		D_rear.move_distance(val);

	}

//	bool All_Drives::get_moved_distance_reached(){
//
////		boost::mutex::scoped_lock sl(my_mutex);
//
//		return (
//				D_front.get_position_reached()
//				&&
//				D_rear.get_position_reached()
//				);
//
//	}

	void All_Drives::set_angle_front(double val){

//		boost::mutex::scoped_lock sl(my_mutex);
		if(drives_configured){

			try{

				S_front.set_angle(val);

			}catch(std::exception e){
				std::cerr<< "All_Drives::set_angle_front(): " <<e.what() << std::endl;
			}

		}
	}
	void All_Drives::set_angle_rear(double val){

//		boost::mutex::scoped_lock sl(my_mutex);

		if(drives_configured){

			try{
				S_rear.set_angle(val);
			}catch(std::exception e){
				std::cerr<< "All_Drives::set_angle_rear(): " <<e.what() << std::endl;
			}

		}
	}



	void All_Drives::disable_drives(){

//		boost::mutex::scoped_lock sl(my_mutex);

		if(drives_configured){

			try{


				D_front.disable_drive();
				D_rear.disable_drive();

				S_front.disable_drive();
				S_rear.disable_drive();

			}catch(std::exception e){
				std::cerr<< "All_Drives::disable_drives(): " <<e.what() << std::endl;
			}

		}
	}

	void All_Drives::reenable_drives(){

//		boost::mutex::scoped_lock sl(my_mutex);


		if(drives_configured){

			try{
				D_front.enable_drive();

				D_rear.enable_drive();

				S_front.enable_drive();
				S_rear.enable_drive();
			}catch(std::exception e){
				std::cerr<< "All_Drives::reenable_drives(): " <<e.what() << std::endl;
			}

		}


	}

	void All_Drives::update_drive_data()throw(std::exception){


				// connecting the socket and setting the bcm_receive filters

				// bind the socket only once
				if(!bcm_socket_is_bound){

					sockaddr_can addr;
					ifreq ifr;

					addr.can_family = AF_CAN;

					strcpy(ifr.ifr_name, can_interface_name.c_str());

					if (ioctl(my_bcm_socket, SIOCGIFINDEX, &ifr) < 0){

						perror("SIOCGIFINDEX");

						throw(E_CAN_Bind_Exception("CAN_Node: ioctrl exception") );

					}

					addr.can_ifindex = ifr.ifr_ifindex;

					if (!addr.can_ifindex) {
						perror("invalid bridge interface");
						throw(E_CAN_Bind_Exception("CAN_Node: No ifindex") );
					}


					if(connect(my_bcm_socket, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
						perror("Bind");
						throw(E_CAN_Bind_Exception("CAN_Node: Exception while binding socket") );
					}



					/*
					 *  timeout für read festlegen
					 */
					timeval timeout;
					timeout.tv_sec=0;
					timeout.tv_usec=100000;


					if(setsockopt(my_bcm_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==-1){
						perror("All Drives: setsockopt: ");
					}


					// Array with noes of the drive
					canid_t nodes[]={
							I_C::NODE_ANTRIEB_HINTEN,
							I_C::NODE_ANTRIEB_VORNE,
							I_C::NODE_LENKUNG_HINTEN,
							I_C::NODE_LENKUNG_VORNE,
					};

					// Setting Filters for PDO1 of each drives

					for(int i=0 ; i<4 ; i++){


						bcm_to_filter_pdos.msg_head.can_id = 0x180+nodes[i];


						if (write(my_bcm_socket, &bcm_to_filter_pdos, sizeof(bcm_to_filter_pdos)) < 0){

							std::cout << "Write Error " << std::endl;

							perror("All Drives: startSyncTelegramm");

						}

					}

					bcm_socket_is_bound=true;



				} // End  connect socket



				sendSyncTelegramm();


				// wait for all drive PDOS
				bool timeout=false;

				for (int i = 0; i < 4; ++i) {


					if(	read(my_bcm_socket, &recived_bcm_pdo,sizeof(recived_bcm_pdo)) ==-1){

						perror("All_Drives: read bcm");
						if(errno==EAGAIN){

							timeout=true;

						}
					}

//					if(recived_bcm_pdo.msg_head.opcode!=RX_CHANGED){
//						dout << sfzdebug::linestart<<  "Wrong  PDO received: "
//								<<sfzdebug::hex << recived_bcm_pdo.msg_head.can_id
//								<< " " << recived_bcm_pdo.frame <<sfzdebug::endl;
//					}

				}


				if(!timeout){

					S_front.read_next_pdos();
					S_rear.read_next_pdos();
					D_front.read_next_pdos();
					D_rear.read_next_pdos();

				}



	}


	void All_Drives::process(){

		boost::mutex::scoped_lock sl(my_mutex);

		if(!is_homing_state_running){

			try{
				update_drive_data();
			}catch(std::exception e){
				std::cerr<< "All_Drives::process(): " <<e.what() << std::endl;
			}

		}

	}

	void All_Drives::get_drive_data(opaque_data_t& data ){

		boost::mutex::scoped_lock sl(my_mutex);

		//data.distance_reached=get_moved_distance_reached();
		data.distance_front_m=D_front.get_driven_distance_m();
		data.distance_rear_m=D_rear.get_driven_distance_m();
		data.velocity_front_mps=D_front.get_velocity_mps();
		data.velocity_rear_mps=D_rear.get_velocity_mps();
		data.steering_angle_front=S_front.get_position();
		data.steering_angle_rear=S_rear.get_position();




	}

	void All_Drives::reset_distance_measuring(){

		D_front.reset_distance_measuring();
		D_rear.reset_distance_measuring();

	}

	void All_Drives::setCanInterfaceName(std::string _can_interface_name){
		can_interface_name = _can_interface_name;
		D_front.setCanInterfaceName(_can_interface_name);
		D_rear.setCanInterfaceName(_can_interface_name);
		S_front.setCanInterfaceName(_can_interface_name);
		S_rear.setCanInterfaceName(_can_interface_name);
		et.setCanInterfaceName(_can_interface_name);
	}
}  // namespace idefix
