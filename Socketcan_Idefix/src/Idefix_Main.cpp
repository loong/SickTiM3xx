/*
 * Idefix_Main.cpp
 *
 *  Created on: 02.04.2010
 *      Author: locher
 */

#include "CAN_SDO_Socket.h"
#include "CAN_PDO_Socket.h"
#include "Steering.h"
#include "Steering_Configuration.h"
#include "Drive_Configuration.h"
#include "All_Drives.h"
#include "EPOS_Emergency_Thread.h"

#include "AIM04_Data_Thread.h"

int main(int argc, char **argv) {

	using namespace idefix;
	using namespace sfzcan;



	std::cout << "HW" << std::endl;

//	AIM04_Data_Thread a;
//	a.config();

	EPOS_Emergency_Thread et;

sleep(1);

	Drive t(I_C::NODE_ANTRIEB_VORNE);

	t.configure_Drive();
//	for(int i=0 ; i< 1000 ; i+=100 ){
//
//		t.set_Velocity(i);
//		std::cout << i << std::endl;
//		usleep(50000);
//
//	}
//
//
//	t.set_Velocity(0);
//	sleep(2);

	t.set_Velocity_m_s(0.2);
	sleep(2);
	t.set_Velocity_m_s(-0.4);
	sleep(1);
	t.set_Velocity_m_s(0);

	sleep(1);

	CAN_PDO_Socket s(I_C::NODE_ANTRIEB_VORNE);
	s.bind_socket_to_can_interface("can0");


	t.move_distance(1);


	for(int i=0;i<60;i++){
		s.sendSyncTelegrammMSG();
		t.read_next_pdos();
		std::cout << " Waiting for reaching Position: " << t.get_driven_distance_m() <<std::endl;
		usleep(90000);
	}


//	sleep(5

	s.sendSyncTelegrammMSG();
	t.read_next_pdos();


	t.move_distance(-0.5);

	s.sendSyncTelegrammMSG();
	t.read_next_pdos();

	for(int i=0;i<60;i++){

		s.sendSyncTelegrammMSG();
		t.read_next_pdos();
		std::cout << " Waiting for reaching Position: " << t.get_driven_distance_m() <<std::endl;
		usleep(90000);
	}


	t.set_Velocity_m_s(0.5);

	sleep(5);

	t.set_Velocity_m_s(-0.5);

	sleep(5);

	t.move_distance(2);
	for(int i=0;i<60;i++){

		s.sendSyncTelegrammMSG();
		t.read_next_pdos();
		std::cout << " Waiting for reaching Position: " << t.get_driven_distance_m() <<std::endl;
		usleep(90000);
	}

	t.move_distance(2);
	for(int i=0;i<60;i++){

		s.sendSyncTelegrammMSG();
		t.read_next_pdos();
		std::cout << " Waiting for reaching Position: " << t.get_driven_distance_m() <<std::endl;
		usleep(90000);
	}

	sleep(3);



	t.disable_drive();


}
