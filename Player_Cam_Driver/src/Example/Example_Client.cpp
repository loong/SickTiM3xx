/*
 * Test_Client.cpp
 *
 *  Created on: 28.05.2011
 *      Author: locher
 */

#include <iostream>

#include <player-3.0/libplayerc++/playerc++.h>

#include "Example_Cam_Proxy.h"


int main(int argc, char **argv) {


	PlayerCc::PlayerClient client("IdefixPlayerHost",6667);
	PlayerCc::OpaqueProxy opaque_proxy(&client,0);


	player_opaque_data_t opaque_data;

	Example_Cam_Proxy::example_data_t recieved_data;

	opaque_data.data=reinterpret_cast<uint8_t*>(&recieved_data);



	while(true){

//		std::cout << "Client Read" << std::endl;

		client.Read();
//		client.ReadIfWaiting();
//		std::cout << "Client Read after Waiting" << std::endl;

//		if(client.Peek(0)){
			std::cout << opaque_proxy.GetCount() << std::endl;
			opaque_proxy.GetData(opaque_data.data);
			std::cout<< "Test CLient: " << recieved_data.example<< std::endl;;
//		}





	}


}
