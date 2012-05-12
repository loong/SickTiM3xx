/*
 * @author 	Long Hoang, Lukas Locher
 * @date    05/2012
 *
 *
 * The sicktim3xx driver controls the SICK TiM3xx scanning laser range-finder.
 *
 @param Provides
 - @ref interface_laser

 @param Requires
 - none

Configuration file options

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


// Some older Version of lib usb don't support libusb_error_name_funktion
// in this case comment the following line
//#define WITH_LIB_USB_ERROR_NAME


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
	// USB Stuff
	static const uint16_t VENDOR_ID =0x19A2;
	static const uint16_t PRODUCT_ID=0x5001;

	static const int the_usb_interface_number=0;

	static const uint8_t DATA_STX=2; //Startsequence
	static const uint8_t DATA_ETX=3; // Endsequence

	static uint8_t start_continuous_scan[19];
	static uint8_t stop_continuous_scan[19];

	static const uint8_t read_endpoint=129;
	static const uint8_t write_endpoint=2;
	static const unsigned int timeout_millis=10000;

	int transferred_data_size;
	uint8_t receive_buf[2049];
	libusb_device_handle* m_usb_device_handle;
	libusb_context* m_usb_context;

	//Data Management for Player
	player_laser_data_t playerData;
	TiM3xx_Data_Parser m_data_parser;
	float ranges[270];
	uint8_t intensities[270];

	// Laser pose in robot coordinate system.
	double pose[3];
	double size[2];

	//if laserscanner is mounted upside_down
	bool upside_down;

	// Basic measurement parameters
	double angular_resolution, scanning_frequency;
	double min_angle, max_angle;

	int debug;

	// Main function for device thread.
	virtual void Main();
	virtual int MainSetup();
	virtual void MainQuit();

	virtual ~SickTim3xx();

};

#endif /* SICKLMS100_HPP_ */
