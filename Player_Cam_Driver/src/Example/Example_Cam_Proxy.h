/*
 * Opaque_Proxy.h
 *
 *  Created on: 26.03.2011
 *      Author: locher
 */

#ifndef EXAMPLE_CAM_PROXY_H_
#define EXAMPLE_CAM_PROXY_H_


#include <player-3.0/libplayerc++/playerc++.h>





class Example_Cam_Proxy {

public:

	// Struct for  Opaque Data
	typedef struct example_data{

		double example;
//		char example[50];

	} example_data_t;



		private:
		PlayerCc::PlayerClient* my_PlayerClientRef;

		PlayerCc::OpaqueProxy my_opaque_proxy;

		example_data_t my_opaque_data;

		player_opaque_data_t player_opaque_data;


public:
	Example_Cam_Proxy(PlayerCc::PlayerClient*,uint32_t index);
	virtual ~Example_Cam_Proxy();

	virtual void read_opaque_data();
//----------------------------------------
// Methods for accessing opaque data
//----------------------------------------

//  Getters
//----------------------------------------

	double getter_example();
//	void send_cmd_data();


};

#endif /* EXAMPLE_CAM_PROXY_H_ */
