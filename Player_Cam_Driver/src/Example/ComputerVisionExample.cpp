/*
 * ComputerVisionExample.cpp
 *
 *  Created on: 31.05.2011
 *      Author: locher
 */

#include "ComputerVisionExample.h"

ComputerVisionExample::ComputerVisionExample() :
//Don't forget to config the same pipe name in the Player Driver Config File
ComputerVisionBaseClass("/tmp/left_cam", 0)
{
	// TODO Auto-generated constructor stub

}

ComputerVisionExample::~ComputerVisionExample() {
	// TODO Auto-generated destructor stub
}


void ComputerVisionExample::process_image(IplImage* img){

	makeACopyOfThisNewIplImage(img);


	// Write to named pipe
	Example_Cam_Proxy::example_data_t  data;

	static int  count=0;

//	strcpy(data.example,"Hello_Player_World");

	data.example=++count*0.01;

	send_opaque_data_to_player_cam_driver(&data ,sizeof(data));


}
