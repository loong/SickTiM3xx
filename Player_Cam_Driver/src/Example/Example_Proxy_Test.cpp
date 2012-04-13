/*
 * Example_Proxy_Test.cpp
 *
 *  Created on: 05.06.2011
 *      Author: locher
 */

#include <iostream>

#include <player-3.0/libplayerc++/playerc++.h>
#include "Example_Cam_Proxy.h"





int main(int argc, char **argv) {


	{
		PlayerCc::PlayerClient client("IdefixPlayerHost",6667);
		Example_Cam_Proxy cam_prox(&client,0);


		while(true){

			std::cout <<"Loop: " ;
//			client.Read();

			cam_prox.read_opaque_data();
//
//
			std::cout << cam_prox.getter_example()  << std::endl;

		}
	}

}
