/**
 * @file    sicklms100.cc
 * @author 	Kai Brach
 * @date    04/2010
 * @brief
 *
 *
 * The sicklms100 driver controls the SICK LMS 100 scanning laser range-finder.

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

#include <math.h>
#include "htwg_LMS100_cola.h"

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

#include <libplayercore/playercore.h>
//#include <libplayerxdr/playerxdr.h> for player version 2 only
#include <libplayerinterface/playerxdr.h>




/**
 * The SICK LMS100 Device Class
 */
class SickLMS100: public ThreadedDriver {
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
	~SickLMS100();

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

	// Basic measurement parameters
	double angular_resolution, scanning_frequency;
	double min_angle, max_angle;

	// Password for changing userlevel
	const char* password;
	int8_t userlevel;
	bool logged_in;

	int debug;
};





/**
 * Constructor of the driver. Retrieve options from the configuration file and set
 * the variables
 *
 * @param cf configuration file player is started with
 *
 * @param section current section
 */
SickLMS100::SickLMS100(ConfigFile* cf, int section) : ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN, PLAYER_LASER_CODE)
{
	// Laser geometry.
	pose[0] = cf->ReadTupleLength(section, "pose", 0, 0.0);
	pose[1] = cf->ReadTupleLength(section, "pose", 1, 0.0);
	pose[2] = cf->ReadTupleLength(section, "pose", 2, 0.0);

	size[0] = 0.10;
	size[1] = 0.10;

	intensity = true;

	// Read TCP/IP connection settings
	hostname = cf->ReadString(section, "hostname", DEFAULT_LMS100_HOSTNAME);
	port = cf->ReadInt(section, "port", DEFAULT_LMS100_PORT);

	// Basic measurement parameters
	angular_resolution = cf->ReadFloat(section, "angular_resolution",DEFAULT_LMS100_ANGULAR_RES);
	scanning_frequency = cf->ReadFloat(section, "scanning_frequency",DEFAULT_LMS100_FREQUENCY);

	min_angle = cf->ReadFloat(section, "min_angle", DEFAULT_LMS100_MINANGLE);
	max_angle = cf->ReadFloat(section, "max_angle", DEFAULT_LMS100_MAXANGLE);

	// Password for changing to userlevel
	password = cf->ReadString(section, "password", DEFAULT_LMS100_PASSWORD);
	logged_in = false;
	userlevel = 0;	// Default: 0
	laser_power_on = true;

	debug = cf->ReadInt(section, "debug", 0);
}




/**
 * Destructor
 */
SickLMS100::~SickLMS100() {

}




/**
 * Set up the LMS100
 *
 * @return 0 if successful, -1 otherwise
 */
int SickLMS100::MainSetup() {

	// Create a lms100_cola object
	lms100 = new lms100_cola(hostname, port, debug);

	// Attempt to connect to the laser unit
	if (lms100->Connect() != 0) {
		PLAYER_ERROR2 ("> Connecting to SICK LMS100 on [%s:%d]...[failed]",
				hostname, port);
		return (-1);
	}
	PLAYER_MSG2 (1, "> Connecting to SICK LMS100 on [%s:%d]... [done]",
			hostname, port);

	// Try to change the userlevel
	if (ChangeUserlevel() != -1)
	{
		if(userlevel > 2)
		{
			// Set scanning parameters
			if (lms100->SetResolutionAndFrequency(scanning_frequency,
					angular_resolution, min_angle, max_angle - min_angle) != 0)
				PLAYER_WARN ("> Couldn't configure resolution, frequency, and min/max angle. Using previously set values... [failed]");
			else
				PLAYER_MSG0 (1, "> Configuring resolution, frequency and min/max angle... [done]");

			// Configure scan data output
			if (lms100->ConfigureScanDataOutput() != 0)
				PLAYER_WARN ("> Couldn't configure scan data output... [failed]");
			else
				PLAYER_MSG0 (1, "> Configuring scan data output... [done]");
		}
		else
			PLAYER_WARN1("> Userlevel %d has no permission to set configuration parameters.",userlevel);
	}


	// Start the device thread
	StartThread();

	return (0);
}




/**
 * Shut down the LMS100
 *
 * returns nothing in player 3 !!
 */
void SickLMS100::MainQuit() {
	// shutdown laser device
	StopThread();

	// Stop the measurement process
	if(lms100->StopMeasurement() != -1)
	{

		// Set back to userlevel 0
		lms100->StartDevice();

		// Disconnect from the laser unit
		lms100->Disconnect();

		PLAYER_MSG0 (1, "> SICK LMS100 driver shutting down... [done]");
	}
	else
	{
		// Disconnect from the laser unit
		lms100->Disconnect();
		PLAYER_MSG0 (1, "> SICK LMS100 driver shutting down correctly... [failed]");
	}
}


/**
 * Changes the userlevel with help of the given password
 *
 * @return 0 if successful, -1 otherwise
 */
int SickLMS100::ChangeUserlevel()
{
	if (strncmp(password, "NULL", 4) != 0) {

		// Check passwords and set userlevel
		if(strncmp(password, (const char*)DEFAULT_LMS100_L2_PASSWORD, 8) == 0)
			userlevel = 2;
		else if (strncmp(password, (const char*)DEFAULT_LMS100_L3_PASSWORD, 8) == 0)
			userlevel = 3;
		else if (strncmp(password, (const char*)DEFAULT_LMS100_L4_PASSWORD, 8) == 0)
			userlevel = 4;
		else
		{
			PLAYER_WARN ("> Password incorrect! Configuration is not possible!");
			return -1;
		}

		// Login to userlevel
		if (lms100->SelectUserLevel(userlevel, password) != 0)
		{
			PLAYER_WARN2("> Change to userlevel %d using password %s... [failed]",userlevel, password);
			return -1;
		}
		else {
			logged_in = true;
			// Stop the measurement process (in case it's running from another instance)
			lms100->StopMeasurement();
			PLAYER_MSG1 (1, "> Change to userlevel %d... [done]", userlevel);
			return 0;
		}
	} else
	{
		PLAYER_WARN ("> No password given!");
		return -1;
	}


}

/**
 * This function is called once for each message in the incoming queue. *
 *
 * @param resp_queue The queue to which any response should go.
 * @param hdr The message header
 * @param data The message body
 *
 * @return 0 if the message is handled successful, -1 otherwise
 */
int SickLMS100::ProcessMessage(QueuePointer &resp_queue, player_msghdr* hdr,void* data) {

	assert (hdr);

	// Get Geometry
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LASER_REQ_GET_GEOM, device_addr)) {
		player_laser_geom_t geom;
		memset(&geom, 0, sizeof(geom));
		geom.pose.px = pose[0];
		geom.pose.py = pose[1];
		geom.pose.pyaw = pose[2];
		geom.size.sl = size[0];
		geom.size.sw = size[1];

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_LASER_REQ_GET_GEOM, (void*) &geom, sizeof(geom), NULL);
		return (0);
	}



	// Set Power
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ,PLAYER_LASER_REQ_POWER, device_addr)) {

		player_laser_power_config_t* config = reinterpret_cast<player_laser_power_config_t *> (data);

		if (config->state == 0) {
			lms100->StopMeasurement();
			laser_power_on = false;
		} else {
			lms100->StartMeasurement();
			laser_power_on = true;
		}

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, hdr->subtype);
		return (0);
	}



	// Set Configuration
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LASER_REQ_SET_CONFIG, device_addr)) {

		player_laser_config_t* config = reinterpret_cast<player_laser_config_t *> (data);

		// Set intensity locally, ignore range_max and range_res (unused for LMS100)
		intensity = config->intensity;

		// Setting {min, max}_angle, resolution (angular), and scanning_frequency
		angular_resolution = RTOD (config->resolution);
		min_angle = RTOD (config->min_angle);
		max_angle = RTOD (config->max_angle);
		scanning_frequency = config->scanning_frequency;

		// Stop the measurement process
		lms100->StopMeasurement();


		// Try to change the userlevel
		if (ChangeUserlevel() != -1) {
			if (userlevel > 2) {
				// Set scanning parameters
				if (lms100->SetResolutionAndFrequency(scanning_frequency,
						angular_resolution, min_angle, max_angle - min_angle) != 0)
					PLAYER_WARN ("> Couldn't configure resolution, frequency, and min/max angle. Using previously set values... [failed]");
				else
					PLAYER_MSG0 (1, "> Configuring resolution, frequency and min/max angle... [done]");

					// Re-start the measurement process
					if (laser_power_on)
						lms100->StartMeasurement();

					// Configuration succeeded; send an ACK
					Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, hdr->subtype);
					return 0;
				}else
					PLAYER_WARN1("> Userlevel %d has no permission to set configuration parameters.",userlevel);
			}

		// Re-start the measurement process
		if (laser_power_on)
			lms100->StartMeasurement();

		// Configuration failed; send a NACK
		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_NACK, hdr->subtype);
		return (-1);
	}



	// Get Configuration
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LASER_REQ_GET_CONFIG, device_addr)) {
		// Get min_angle, max_angle, resolution and scanning_frequency
		player_laser_config_t config = lms100->GetConfiguration();

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,PLAYER_LASER_REQ_GET_CONFIG, (void*) &config, sizeof(config), NULL);
		return (0);
	}



	// Get ID information
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LASER_REQ_GET_ID, device_addr)) {

		player_laser_get_id_config_t idconfig = lms100->GetConfigurationID();

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,PLAYER_LASER_REQ_GET_ID, (void*) &idconfig, sizeof(idconfig), NULL);
		return (0);
	}




	// Set filter settings
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_LASER_REQ_SET_FILTER, device_addr)) {
		PLAYER_WARN ("> LMS100 driver doesn't support filters.");
		return (-1);
	} else
		return (-1);
}

/**
 * Main function for device thread
 */
void SickLMS100::Main() {
	// Start Continous measurements
	if( lms100->StartMeasurement() != -1)
	{

		while (true) {
			// test if we are supposed to cancel
			pthread_testcancel();

			// Request/replies handler
			ProcessMessages();

			// Refresh data only if laser power is on
			if (laser_power_on) {
				player_laser_data_t data = lms100->ReadMeasurement();

				// Make data available
				if (data.ranges_count != (unsigned int) -1)
					Publish(device_addr, PLAYER_MSGTYPE_DATA,
							PLAYER_LASER_DATA_SCAN, &data);

				player_laser_data_t_cleanup(&data);
			}
		}
	}else
	{
		this->MainQuit();
	}
}

/**
 * Factory creation function. This functions is given as an argument when the
 * driver is added to the driver table
 */
Driver* SickLMS100_Init(ConfigFile* cf, int section) {
	return ((Driver*) (new SickLMS100(cf, section)));
}



/**
 * Registers the driver in the driver table. Called from the player_driver_init
 * function that the loader looks for
 */
void SickLMS100_Register(DriverTable* table) {
	table->AddDriver("sicklms100", SickLMS100_Init);
}




/**
 * EXTRA STUFF FOR BUILDING A SHARED OBJECT
 *
 * The main difference between a normal player driver and plugin driver is,
 * that player doesn't has to be recompiled if you compile your driver.
 * The libary containing your plugin driver is linked to the player server at runtime.
 * So a plugin driver is the better choice, because recompiling needs only seconds in comparison
 * with 1-2 minutes if you have to recompile player each time.
 * In difference to a normal player driver there is no entry in the players device table for this driver.
 * Hence there must be a additional function in a player plugin driver to let the player server
 * know that this driver exists and how to create it.
 */

extern "C" {
int player_driver_init(DriverTable * table) {
	SickLMS100_Register(table);
	return (0);
}
}
