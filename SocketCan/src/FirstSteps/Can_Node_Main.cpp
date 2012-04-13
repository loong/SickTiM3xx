/*
 * Can_Node_Main.cpp
 *
 *  Created on: 29.03.2010
 *      Author: locher
 */

#include <iostream>
#include <sys/wait.h>

#include "CAN_PDO_Socket.h"
#include "CAN_PDO.h"

using namespace sfzcan;


int main(int argc, char **argv) {


	CAN_PDO pdo(10,CAN_PDO::PDO_1);
	pdo.setNewData(0x1234,2);
	pdo.appendMsgData(0x56,1);
	pdo.appendMsgData(0x789ABCDEF,5);



	std::cout << pdo;

	pid_t  pid= fork();

	if(pid==0){
		char*  const cmd[]={};

		std::cout<< "Child starting " << std::endl;

		return  execv("./bin/SendCanFrame.exe",cmd);

	}else{

		try{

			CAN_PDO_Socket n(1);

			n.bind_socket_to_can_interface("vcan0");

			for (int i = 0; i < 10; ++i) {

				n.sendPDO(pdo);

			}

//			CAN_Node n2(1);
//			n2.set_debug_message_on();
//			n2.bind_socket_to_can_interface("can0");

//			n2.sendResetRemoteNodeMSG();
//			usleep(1000000);
//
//			n2.sendEnterPreOperationalMode();
//			usleep(1000000);
//
//			n2.sendStartRemoteNodeMSG();
//			usleep(1000000);
//			n2.sendStartAllNodesMSG();
//
//
//			for (int i = 0; i < 50; ++i) {
//				std::cout << "Sending sync Telegramm" << std::endl;
//				n2.sendSyncTelegrammMSG();
//
//				usleep(50000);
//
//			}

			if(waitpid(pid, 0,0)!=pid){
				perror("Error Waiting for Child");
			}

			std::cout << "Child finished" << std::endl;


		}catch(E_CAN_Bind_Exception &e){

			std::cout <<"Bind exception:" <<  e.what() << std::endl;
		}

		std::cout << "Returning" << std::endl;
		return 0;

	}






}
