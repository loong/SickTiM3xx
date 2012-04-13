/*
 * Drive.cpp
 *
 *  Created on: 08.04.2010
 *      Author: locher
 */

#include "Drive.h"

namespace idefix {




	Drive::Drive(canid_t nodeId) :
			CAN_PDO_Socket(nodeId),
			can_interface(CAN_INTERFACE_NAME),
			actual_distance_m(0),
			first_time(true)
	{

		number_of_pdos_to_read=2;



	}

	Drive::~Drive()throw() {


	}


	void Drive::configure_Drive()throw(std::exception){

		std::cout<<"Drive::configure_Drive with interface: "<<can_interface<<std::endl;

		bind_socket_to_can_interface(can_interface);
		my_sdo_socket.connect_socket_to_can_interface(can_interface);


//		try {
			Drive_Configuration::configure_Drive(my_sdo_socket,*this);
//		} catch (std::exception e) {
//			std::cout<<"Drive::configure_Drive E_SDO_Exception:"<<e.what()<<std::endl;
//		}


		actual_profile=PROFILE_VELOCITY;

//		try {
			Drive_Configuration::setProfileVelocityMode(my_sdo_socket,*this);
//		} catch (std::exception e) {
//			std::cout<<"Drive::configure_Drive E_CAN_IO_Exception:"<<e.what()<<std::endl;
//		}

//		actual_profile=PROFILE_POSITION;
//		Drive_Configuration::setProfilePositionMode(my_sdo_socket,*this);

		if(Drive_Configuration::isStatusFault(my_sdo_socket,*this)){
			Drive_Configuration::resetFault(my_sdo_socket,*this);
		}


		sendStartRemoteNodeMSG();


	}

	void Drive::set_Velocity_m_s(double value)throw(std::exception){

		if(actual_profile!=PROFILE_VELOCITY){
			actual_profile=PROFILE_VELOCITY;
			Drive_Configuration::setProfileVelocityMode(my_sdo_socket,*this);
		}
		// TODO Geschwindigkeitsberechnung (Faktor 4000! ist aus der Luft gegriffen)
		Drive_Configuration::setVelocity_rpm(*this,
				msgdata_t(value*I_C::VELOCITY_MS_TO_RPM_FACTOR));

	}


	void Drive::move_distance(double distance_m)throw(std::exception){


		if(actual_profile!=PROFILE_POSITION){
			actual_profile=PROFILE_POSITION;
			Drive_Configuration::setProfilePositionMode(my_sdo_socket,*this);
		}

//		move_distance_start_value_qc=position_qc;
//		move_distance_desired_value_m=distance_m;

		Drive_Configuration::moveDistance(*this,msgdata_t(distance_m/I_C::QC_TO_METER_FACTOR_DRIVES));
//		Drive_Configuration::moveDistance(*this,msgdata_t(100000));

	}

	void Drive::disable_drive()throw(std::exception){

		Drive_Configuration::disable_Device(*this);
	}


	void Drive::enable_drive()throw(std::exception){

		Drive_Configuration::enable_Device(my_sdo_socket,*this);
	}

	void Drive::read_next_pdos()throw(std::exception){

		for (uint_t i = 0; i < number_of_pdos_to_read; ++i) {


			can_frame f;


			if(read(my_socket,&f,sizeof(f))==-1){
				perror("Drive: read next pdo");
			}else{

				/* thread lock */
				boost::mutex::scoped_lock sl(my_mutex);

				if( f.can_id == ( canid_t(0x180) +get_node_id())   ){

					position_qc=CAN_Tools::PDO_To_msgdata_signed(f,4,4);


					if(first_time){
						last_position_qc=position_qc;
						first_time=false;
					}

					msgdata_t delta_qc=	position_qc-last_position_qc;
					// Betrag nehmen
					if(position_qc<last_position_qc){
						delta_qc=-delta_qc;
					}

					actual_distance_m+=delta_qc *I_C::QC_TO_METER_FACTOR_DRIVES;
//					std::cout<< position_qc <<" " << last_position_qc << "  "  << delta_qc << " Drive: " << actual_distance << std::endl;

					last_position_qc=position_qc;

					velocity_qc=CAN_Tools::PDO_To_msgdata_signed(f,0,4);
					velocity_m=velocity_qc*I_C::QC_TO_METER_FACTOR_DRIVES;

//					std::cout << "Drive velocity" << velocity << std::endl;


				}

				if( f.can_id == ( canid_t(0x280) +get_node_id())   ){

					statusword= CAN_Tools::PDO_To_msgdata(f,0,2);

					//std::cout << "Statusword: " << std::hex << statusword << std::dec << std::endl;



				}

			}
		}

	}

	void Drive::reset_distance_measuring(){
		boost::mutex::scoped_lock(my_mutex);
		actual_distance_m=0;
	}

	double Drive::get_driven_distance_m(){

		boost::mutex::scoped_lock sl(my_mutex);
		return actual_distance_m;

	}

	double Drive::get_velocity_mps(){

		boost::mutex::scoped_lock sl(my_mutex);
		return velocity_m;


	}

	void Drive::setCanInterfaceName(std::string _can_interface_name){
		can_interface = _can_interface_name;
	}


//	bool Drive::get_position_reached(){
//
//		std::cout << "V: " << fabs(velocity_m) << std::endl;
//		std::cout << "Delta " <<
//				fabs(
//									fabs(move_distance_start_value_qc-position_qc)*I_C::QC_TO_METER_FACTOR_DRIVES
//									-fabs(move_distance_desired_value_m)
//									) << std::endl;
//
//		if(
//				(fabs(velocity_m)< I_C::MAX_ALLOWED_SPEED_MEANING_ZERO)
//				&&
//				fabs(
//					fabs(move_distance_start_value_qc-position_qc)*I_C::QC_TO_METER_FACTOR_DRIVES
//					-fabs(move_distance_desired_value_m)
//					) < I_C::MAX_ALLOWED_DISTANCE_MEANING_ZERO){
//			return true;
//		}else{
//			return false;
//		}
//
//		return false;
//
//	}

}  // namespace idefix
