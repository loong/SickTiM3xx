/*
 * Opaque_Proxy.cpp
 *
 *  Created on: 26.03.2011
 *      Author: locher
 */

#include "Example_Cam_Proxy.h"

Example_Cam_Proxy::Example_Cam_Proxy(PlayerCc::PlayerClient* thePlayerClient,uint32_t index):

		my_PlayerClientRef(thePlayerClient),
		my_opaque_proxy(my_PlayerClientRef,index)
{



	memset(&player_opaque_data ,0,sizeof(player_opaque_data));
	memset(&my_opaque_data,0,sizeof(example_data_t));

	// Mapping der Zeiger
	player_opaque_data.data = reinterpret_cast<uint8_t *> (&my_opaque_data);
	player_opaque_data.data_count = sizeof(example_data_t);


//	player_opaque_cmd.data = reinterpret_cast<uint8_t *> (&my_opaque_cmd);
//	player_opaque_cmd.data_count = sizeof(idefix::opaque_cmd_t);


	std::cout << "Example Cam Proxy Konstruktor" << std::endl;

	std::cout << "Check if the size of the opaque data in the config file has the following size:\n"<<
			sizeof(example_data_t)<< std::endl;

}



Example_Cam_Proxy::~Example_Cam_Proxy() {

	std::cout << "Example Cam Proxy Destruktor" << std::endl;

}


void Example_Cam_Proxy::read_opaque_data(){

	my_PlayerClientRef->Read();

	my_opaque_proxy.GetCount();
	my_opaque_proxy.GetData(player_opaque_data.data);

}



double  Example_Cam_Proxy::getter_example(){

//	std::string result(my_opaque_data.example);
//	std::string result("XXXXXXX");

	std::cout << "Player_cam_proxy: " << my_opaque_data.example<< std::endl;

	return my_opaque_data.example;
}



//void Player_Cam_Proxy::send_cmd_data(){
//
//	my_opaque_proxy.SendCmd(&player_opaque_cmd);
//}
