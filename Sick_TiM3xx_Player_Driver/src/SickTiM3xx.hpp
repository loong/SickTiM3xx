/*
 * SickLMS100.hpp
 *
 *  Created on: 08.09.2010
 *      Author: asterix
 *
 * @file    sicklms100.cc
 * @author 	Kai Brach
 * @date    04/2010
 * @brief
 *
 *
 * The sicktim3xx driver controls the SICK TiM3xx scanning laser range-finder.
 *
 @param Provides

 - @ref interface_laser

 @param Requires

 - none

 @param Configuration requests

 - PLAYER_LASER_REQ_GET_CONFIG
 - PLAYER_LASER_REQ_GET_GEOM
 - PLAYER_LASER_REQ_GET_ID
 - PLAYER_LASER_REQ_SET_CONFIG
 - PLAYER_LASER_REQ_SET_FILTER

 @param Configuration file options


 - pose (length tuple)
 - Default: [0.0 0.0 0.0]
 - Pose (x,y,theta) of the laser, relative to its parent object (e.g.,
 the robot to which the laser is attached).

 - size (length tuple)
 - Default: [0.10 0.10]
 - Footprint (x,y) of the laser.


 - debug (int)
 - Default: 0
 - Enable debugging mode (read/writes to the device are printed on screen).
 Valid values: 0 (disabled), 1 (enabled for standard messages), 2 (enabled
 for all messages including measurements - warning: this slows down your
 throughoutput date!).

 @param Example

 driver
 (
 name "sicktim3xx"
 plugin "libsicktim3xx"
 provides ["laser:0"]



 debug 0

 #(x,y,theta) of the laser, relative to its parent object
 pose [0.25 0.0 0.0]

 )
 */

#ifndef SICKLMS100_HPP_
#define SICKLMS100_HPP_

#include <player-3.0/libplayercore/playercore.h>
#include <player-3.0/libplayerinterface/playerxdr.h>
#include <player-3.0/libplayerc++/playerc++.h>
#include <iostream>

#include  <libusb-1.0/libusb.h>

#include "TiM3xxDataParser.h"

/**
 * The SICK Tim3xx Device Class
 */
class SickTim3xx : public ThreadedDriver{
public:
	// Constructor
	SickTim3xx(ConfigFile* cf, int section);

	// MessageHandler
	int ProcessMessage(QueuePointer &resp_queue, player_msghdr* hdr,void* data);

private:
	// Main function for device thread.
	virtual void Main();
	virtual int MainSetup();
	virtual void MainQuit();

	// Destructor
	virtual ~SickTim3xx();


	// USB Stuff
	static const uint16_t VENDOR_ID=0x19A2;
	static const uint16_t PRODUCT_ID=0x5001;

	static const int the_usb_interface_number=0;

	static const uint8_t	DATA_STX=2; //Start of text
	static const uint8_t	DATA_ETX=3; // End of Text


	static  uint8_t start_continuous_scan[19];//={DATA_STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','1',DATA_ETX};
	static uint8_t stop_continuous_scan[19];//={DATA_STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','0',DATA_ETX};


	static const uint8_t	read_endpoint=129;
	static const uint8_t	write_endpoint=2;
	static const unsigned int timeout_millis=10000;

	int transferred_data_size;

	uint8_t receive_buf[2049];



	libusb_device_handle* m_usb_device_handle;
	libusb_context* m_usb_context;


	TiM3xx_Data_Parser m_data_parser;

	// Laser pose in robot coordinate system.
	double pose[3];
	double size[2];

	//if laserscanenr is mounted upside_down
	bool upside_down;

	// Basic measurement parameters
	double angular_resolution, scanning_frequency;
	double min_angle, max_angle;


	int debug;
	//refreshrates:

};

#endif /* SICKLMS100_HPP_ */
