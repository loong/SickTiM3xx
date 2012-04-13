/*
 * EPOS_CAN_Node.cpp
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */

#include "EPOS_Emergency_Thread.h"


namespace idefix {

	using namespace sfzcan;

	EPOS_Emergency_Thread::EPOS_Emergency_Thread(std::string _can_interface_name):
			Thread("EPOS_Emergency_Thread"),
			can_interface_name(_can_interface_name),
			socket_is_bound(false)
	{


	}

	EPOS_Emergency_Thread::~EPOS_Emergency_Thread() throw(){

		stop_the_thread();

	}


	void EPOS_Emergency_Thread::process(){


		can_frame f;
		if ( read(my_socket, &f, sizeof(f)) == sizeof(f)) {

			report_EPOS_error(f);
		}


	}

	void EPOS_Emergency_Thread::connect_socket(){


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

			/*
			 *  timeout für read festlegen
			 */
			timeval timeout;
			timeout.tv_sec=0;
			timeout.tv_usec=300000;


			if(setsockopt(my_socket,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout))==-1){
				perror("setsockopt: ");
			}



			can_filter my_filter[4];

			my_filter[0].can_id=0x80+I_C::NODE_ANTRIEB_VORNE;
			my_filter[0].can_mask=CAN_SFF_MASK  ;
			my_filter[1].can_id=0x80+I_C::NODE_ANTRIEB_HINTEN;
			my_filter[1].can_mask=CAN_SFF_MASK  ;
			my_filter[2].can_id=0x80+I_C::NODE_LENKUNG_VORNE;
			my_filter[2].can_mask=CAN_SFF_MASK  ;
			my_filter[3].can_id=0x80+I_C::NODE_LENKUNG_HINTEN;
			my_filter[3].can_mask=CAN_SFF_MASK  ;



			if(setsockopt(my_socket,SOL_CAN_RAW,CAN_RAW_FILTER,&my_filter,sizeof(my_filter))==-1){
				perror("setsocketopt: ");
				throw(E_CAN_Bind_Exception("CAN_Node: Exception while binding socket") );
			}



			if(bind(my_socket, (sockaddr *)(&addr),sizeof(addr) ) < 0 ){
				perror("Bind");
				throw(E_CAN_Bind_Exception("CAN_Node: Exception while binding socket") );
			}


			socket_is_bound=true;

		}


	}





//	void Idefix_PDO_Thread::next_can_frame_to_process(const can_frame& f, timeval& tv){
//
//		if(   f.can_id == ( canid_t(0x80)+get_node_id() )  ){
//
//			std::cout << f << std::endl;
//
//			report_EPOS_error(f);
//
//		}
//
//
//		if( f.can_id == ( canid_t(0x280) +get_node_id())   ){
//			msgdata_t statusword= CAN_Tools::PDO_To_msgdata(f,0,2);
//			std::cout <<"Status :" <<std::hex << statusword<< std::dec<< std::endl;

//			if( (statusword &0x108) ==0x108){
//
//				std::cout << "Sending Contolword fault rest "<< std::endl;
//				using namespace sfzcan;
//				std::auto_ptr<CAN_EDS_SDO> ap= CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,0x80);
//				try{
//					sendSDO(*ap,10000);
//				}catch(E_SDO_Exception e){
//					std::cerr<<" Illegal  Place for SDO" << e.what() << std::endl;
//				}
//
//				try{
//					ap= CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,0x06);
//					sendSDO(*ap,10000);
//
//				}catch(E_SDO_Exception e ){
//					std::cerr<<" Illegal  Place for SDO 1" << e.what() << std::endl;
//
//				}
//
//
//				try{
//					ap= CAN_Config_EDS::get_EDS(Epos_24_5_ControlWord,get_node_id(),CAN_EDS_SDO::READ_WRITE_STATE_WRITE,0x0F);
//					sendSDO(*ap,10000);
//
//				}catch(E_SDO_Exception e ){
//					std::cerr<<" Illegal  Place for SDO 2" << e.what() << std::endl;
//
//				}
//
//
//			}
//
//		}
//
//	}


	void EPOS_Emergency_Thread::report_EPOS_error(const can_frame& f){



		std::string error_message;

		msgdata_t error_code= ( f.data[1]<< 8 ) + f.data[0] ;

		switch (error_code) {
			case 0x0000: error_message="No Error";	break;

			case 0x1000 : error_message="Generic Error";	break;
			case 0x2310 : error_message="Over Current Error";	break;
			case 0x3210 : error_message="Over Voltage Error";	break;
			case 0x3220 : error_message="Under Voltage";	break;
			case 0x4210 : error_message="Over Temperature";	break;
			case 0x5113 : error_message="Supply Voltage (+5V) too low";	break;
			case 0x6100 : error_message="Internal Software Error";	break;
			case 0x6320 : error_message="Software Parameter Error";	break;
			case 0x7320 : error_message="Sensor Position Error";	break;
			case 0x8110 : error_message="CAN Overrun Error (Objects lost)";	break;
			case 0x8111 : error_message="CAN Overrun Error";	break;
			case 0x8120 : error_message="CAN Passive Mode Error";	break;
			case 0x8130 : error_message="CAN Life Guard Error";	break;
			case 0x8150 : error_message="CAN Transmit COB-ID collision";	break;
			case 0x81FD : error_message="CAN Bus Off";	break;
			case 0x81FE : error_message="CAN Rx Queue Overrun";	break;
			case 0x81FF : error_message="CAN Tx Queue Overrun";	break;
			case 0x8210 : error_message="CAN PDO length Error";	break;
			case 0x8611 : error_message="Following Error";	break;
			case 0xFF01 : error_message="Hall Sensor Error";	break;
			case 0xFF02 : error_message="Index Processing Error";	break;
			case 0xFF03 : error_message="Encoder Resolution Error";	break;
			case 0xFF04 : error_message="Hallsensor not found Error";	break;
			case 0xFF06 : error_message="Negative Limit Error";	break;
			case 0xFF07 : error_message="Positive Limit Error";	break;
			case 0xFF08 : error_message="Hall Angle detection Error";	break;
			case 0xFF09 : error_message="Software Position Limit Error";	break;
			case 0xFF0A : error_message="Position Sensor Breach";	break;
			case 0xFF0B : error_message="System Overloaded";	break;

		default: error_message="Error not in Error List"; break;
		}

		std::cerr << "EPOS Error on " << f << "error code  "<<std::hex<< error_code << std::dec<< "  " << error_message <<  std::endl;


	}

	void EPOS_Emergency_Thread::setCanInterfaceName(std::string _can_interface_name){
		can_interface_name=_can_interface_name;


		std::cout << "starting EPOS_Emergency_Thread using can_interface: "<<can_interface_name<< std::endl;

		try{

			my_socket=socket(PF_CAN,SOCK_RAW,CAN_RAW );
			if(my_socket <0){

				perror("socket") ;
				std::cout << "Failed to create Socket:" <<  std::endl;
				exit(EXIT_FAILURE);
			}

			connect_socket();


		}catch(std::exception &e){



			std::cout << "Caught Exceptin in CAN_Node Konstruktor: " << e.what() << std::endl;


		}catch(...){

			std::cout << "Caught Exceptin in CAN_Node Konstruktor" << std::endl;

		}


		setSleepTime(5000);

		start_the_thread();
	}


}  // namespace idefix
