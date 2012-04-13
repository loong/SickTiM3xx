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
 * The sicklms100 driver controls the SICK LMS 100 scanning laser range-finder.
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

 - hostname (string)
 - Default: "169.254.134.157"
 - IP address of the SICK LMS 100 (Ethernet version)

 - port (integer)
 - Default: 2111
 - TCP port of the SICK LMS 100 (Ethernet version)

 - angular_resolution (float)
 - Default: 0.5 degrees
 - Angular resolution. Valid values are: 0.25 or 0.5

 - scanning_frequency (float)
 - Default: 50 Hz.
 - Scanning frequency. Valid values are:
 - 25 or 50 Hz

 - min_angle (float)
 - Default: -90 degrees.
 - Defines the minimum angle of the laser unit (where the scan should start).
 minimum -135 degrees

 - max_angle (float)
 - Default: 90 degrees.
 - Defines the maximum angle of the laser unit (where the scan should end).
 max 135 degrees

 - pose (length tuple)
 - Default: [0.0 0.0 0.0]
 - Pose (x,y,theta) of the laser, relative to its parent object (e.g.,
 the robot to which the laser is attached).

 - size (length tuple)
 - Default: [0.10 0.10]
 - Footprint (x,y) of the laser.

 - password (string)
 - Default: servicelevel/81BE23AA
 - Service (userlevel 4) password. Used for enabling/disabling and/or setting
 the filter parameters.

 - debug (int)
 - Default: 0
 - Enable debugging mode (read/writes to the device are printed on screen).
 Valid values: 0 (disabled), 1 (enabled for standard messages), 2 (enabled
 for all messages including measurements - warning: this slows down your
 throughoutput date!).

 @param Example

 driver
 (
 name "sicklms100"
 plugin "libsicklms100"
 provides ["laser:0"]
 hostname "169.254.134.157"
 port 2111

 # Set the angular resolution (0.5 degrees) and scanning frequency (50 Hz)
 angular_resolution 0.5
 scanning_frequency 50

 # Userlevel password (hashed).
 password "81BE23AA"

 #(if 2 read/writes to the device are printed on screen)
 debug 0

 #(x,y,theta) of the laser, relative to its parent object
 pose [0.25 0.0 0.0]

 alwayson 1

 )
 */

#ifndef SICKLMS100_HPP_
#define SICKLMS100_HPP_

#include <string>
#include <player-3.0/libplayercore/playercore.h>
#include <player-3.0/libplayerinterface/playerxdr.h>
#include <player-3.0/libplayerc++/playerc++.h>
#include <iostream>

#include <math.h>
#include "LMS100_cola.h"

#define DEFAULT_LMS100_HOSTNAME    	"169.254.134.157"
#define DEFAULT_LMS100_PORT         2111
#define DEFAULT_LMS100_PASSWORD 	"NULL"
#define DEFAULT_LMS100_L2_PASSWORD 	"B21ACE26"	// Maintenance personnel
#define DEFAULT_LMS100_L3_PASSWORD 	"F4724744"	// Authorised client
#define DEFAULT_LMS100_L4_PASSWORD 	"81BE23AA"	// Service
#define DEFAULT_LMS100_FREQUENCY    50
#define DEFAULT_LMS100_ANGULAR_RES  0.5
#define DEFAULT_LMS100_MINANGLE     -90.0
#define DEFAULT_LMS100_MAXANGLE     90.0

/**
 * The SICK LMS100 Device Class
 */
class SickLMS100 : public ThreadedDriver{
public:
	// Constructor
	SickLMS100(ConfigFile* cf, int section);

	// MessageHandler
	int ProcessMessage(QueuePointer &resp_queue, player_msghdr* hdr,void* data);
private:
	// Main function for device thread.
	virtual void Main();
	virtual int MainSetup();
	virtual void MainQuit();

	// Destructor
	virtual ~SickLMS100();

	// Changing Userlevel
	int ChangeUserlevel();

	// Laser pose in robot coordinate system.
	double pose[3];
	double size[2];

	// TCP/IP connection parameters
	const char* hostname;
	int port;

	// Reference to lms100_cola
	lms100_cola* lms100;

	// Turn intensity data on/off
	bool intensity;

	// Turn laser on/off
	bool laser_power_on;

	//if laserscanenr is mounted upside_down
	bool upside_down;

	// Basic measurement parameters
	double angular_resolution, scanning_frequency;
	double min_angle, max_angle;

	// Password for changing userlevel
	const char* password;
	int8_t userlevel;
	bool logged_in;

	int debug;

	//refreshrates:

};

#endif /* SICKLMS100_HPP_ */
