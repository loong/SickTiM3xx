/*
 * Steering.cpp
 *
 *  Created on: 04.04.2010
 *      Author: locher
 */

#include "Steering.h"
namespace idefix {



	Steering::Steering(canid_t nodeId,robo_mount_pos_t _mount_pos):
		CAN_PDO_Socket(nodeId),
		mount_pos(_mount_pos),
		can_interface(CAN_INTERFACE_NAME),
		sockets_bound(false)
	{

		number_of_pdos_to_read=2;


	}

	Steering::~Steering()throw() {


		//sendStopRemoteNodeMSG();


	}


	void Steering::bind_sockets(){
		if(!sockets_bound){

			std::cout<<"Steering::bind_sockets with interface: "<<can_interface<<std::endl;

			bind_socket_to_can_interface(can_interface);
			my_sdo_socket.connect_socket_to_can_interface(can_interface);
			sockets_bound=true;
		}
	}

	void Steering::configure_Drive()throw(std::exception){


		if(!sockets_bound)bind_sockets();


		Steering_Configuration::configure_Drive(my_sdo_socket,*this);
		Steering_Configuration::setProfilePositionMode(my_sdo_socket,*this,mount_pos);

		if(Steering_Configuration::isStatusFault(my_sdo_socket,*this)){
			Steering_Configuration::resetFault(my_sdo_socket,*this);
		}


		sendStartRemoteNodeMSG();

	}


	void Steering::start_homing()throw(std::exception){

		if(!sockets_bound)bind_sockets();

		Steering_Configuration::currentThreshHoldPositiveSpeedHoming(my_sdo_socket,*this,mount_pos);
//		Steering_Configuration::currentThreshHoldNegativeSpeedHoming(my_sdo_socket,*this,mount_pos);

		Steering_Configuration::waitForReachingTargetPosition(my_sdo_socket,*this);

		Steering_Configuration::setProfilePositionMode(my_sdo_socket,*this,mount_pos);


	}

	void Steering::read_next_pdos()throw(std::exception){



		for (uint_t i = 0; i < number_of_pdos_to_read; ++i) {


			can_frame f;
			if(read(my_socket,&f,sizeof(f))==-1){
				perror("Drive: read next pdo");
			}else{

				if( f.can_id == ( canid_t(0x180) +get_node_id())   ){

					position=CAN_Tools::PDO_To_msgdata(f,4,4);

					velocity=CAN_Tools::PDO_To_msgdata(f,0,4);


				}

				if( f.can_id == ( canid_t(0x280) +get_node_id())   ){

					statusword= CAN_Tools::PDO_To_msgdata(f,0,2);


				}

			}
		}




	}

	msgdata_t Steering::get_position(){
		return position;
	}

	void Steering::set_angle(double value){

		Steering_Configuration::set_position(*this,value, mount_pos);

	}


	void Steering::disable_drive()throw(std::exception){


			Steering_Configuration::disable_Device(*this);

	}


	void Steering::enable_drive()throw(std::exception){

		Steering_Configuration::enable_Device(my_sdo_socket,*this);
	}

	void Steering::setCanInterfaceName(std::string _can_interface_name){
		can_interface = _can_interface_name;
	}



}  // namespace idefix
