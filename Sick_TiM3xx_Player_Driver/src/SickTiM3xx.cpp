/*
 * SickLMS100.cpp
 *
 *  Created on: 08.09.2010
 *      Author: asterix
 */

#include "SickTiM3xx.hpp"

uint8_t SickTim3xx::start_continuous_scan[]={SickTim3xx::DATA_STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','1',SickTim3xx::DATA_ETX};
uint8_t SickTim3xx::stop_continuous_scan[]={SickTim3xx::DATA_STX,'s','E','N',' ','L','M','D','s','c','a','n','d','a','t','a',' ','0',SickTim3xx::DATA_ETX};

/**
 * Constructor of the driver. Retrieve options from the configuration file and set
 * the variables
 *
 * @param cf configuration file player is started with
 *
 * @param section current section
 */
SickTim3xx::SickTim3xx(ConfigFile* cf, int section) :
	ThreadedDriver(
			cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN,
			PLAYER_LASER_CODE
	),
	m_usb_device_handle(0),
	m_usb_context(0)

{

	bzero(&playerData,sizeof(player_laser_data_t));

	playerData.ranges_count = 270;
	playerData.min_angle = DTOR(-135);
	playerData.max_angle = DTOR(135);
	playerData.resolution = DTOR(1);
	playerData.intensity_count =270;

	// Laser geometry.
	pose[0] = cf->ReadTupleLength(section, "pose", 0, 0.0);
	pose[1] = cf->ReadTupleLength(section, "pose", 1, 0.0);
	pose[2] = cf->ReadTupleLength(section, "pose", 2, 0.0);

	size[0] = 0.10;
	size[1] = 0.10;

	debug = cf->ReadInt(section, "debug", 0);

	upside_down = cf->ReadBool(section, "upside_down", 0);
}

/**
 * Destructor
 */
SickTim3xx::~SickTim3xx() {

}

/**
 * Set up the LMS100
 *
 * @return 0 if successful, -1 otherwise
 */
int SickTim3xx::MainSetup() {

	std::cout << "SickTiM3xx::MainSetup()" << std::endl;

	int init_result= libusb_init(&m_usb_context);
	if(init_result){
		std::cerr << "USB init failed: "
#if defined WITH_LIB_USB_ERROR_NAME
				<< libusb_error_name(init_result) << std::endl;
#else
		<< init_result << std::endl;
#endif
//		exit(EXIT_FAILURE);
	}


	libusb_set_debug(m_usb_context,3);



	m_usb_device_handle = libusb_open_device_with_vid_pid(m_usb_context, VENDOR_ID, PRODUCT_ID);
	if( !m_usb_device_handle){

		std::cerr << "Error open device:" <<  std::endl;
		PLAYER_ERROR("> SICK TiM3xx: Open USB device failed");

	}else{

		if(libusb_kernel_driver_active(m_usb_device_handle,the_usb_interface_number)){

			std::cout << "Error: Kernel driver found" << std::endl;
			PLAYER_WARN("> SICK TiM3xx: Open USB device failed");

			if (int error=libusb_detach_kernel_driver(m_usb_device_handle,the_usb_interface_number)){
#if defined WITH_LIB_USB_ERROR_NAME
				PLAYER_ERROR1("> Detaching Kernel Driver failed: %s ", libusb_error_name(error) );
#else
				PLAYER_ERROR1("> Detaching Kernel Driver failed: %s ", error );
#endif
			}

		}else{
			PLAYER_MSG0(1, "> No Kernel driver for attached Device found, thas's good, libusb handels the device");
		}


		// Now we claim the device
		int error = libusb_claim_interface(m_usb_device_handle,the_usb_interface_number);
		if(error){

#if defined WITH_LIB_USB_ERROR_NAME
			PLAYER_ERROR1("Claiming interface failed %s" , libusb_error_name(error));
#else
			PLAYER_ERROR1("Claiming interface failed %i" , error);
#endif
			error= libusb_release_interface(m_usb_device_handle,the_usb_interface_number);

			if(error){

#if defined WITH_LIB_USB_ERROR_NAME
				PLAYER_ERROR1("Releasing interface failed: %s ", libusb_error_name(error));
#else
				PLAYER_ERROR1("Releasing interface failed: %i ", error);
#endif

			}
			libusb_close(m_usb_device_handle);
			m_usb_device_handle=0;
		}
	}

	// Send start Measurement cmd

	std::cout << "Main Setup:Send start Measurment" << std::endl;

	if(m_usb_device_handle ){

		std::cout << "Main Setup:Send start Measurment 2" << std::endl;

		int error =libusb_bulk_transfer(
				m_usb_device_handle,
				write_endpoint,
				start_continuous_scan,
				sizeof(start_continuous_scan),
				&transferred_data_size,
				timeout_millis);

		if(error){
#if defined WITH_LIB_USB_ERROR_NAME
			PLAYER_ERROR1("Write Bulk Transfer failed: %s" , libusb_error_name(error));
#else
			PLAYER_ERROR1("Write Bulk Transfer failed: %i" , error);
#endif
		}
	}

	return (0);
}

/**
 * Shut down the LMS100
 *
 * returns nothing in player 3 !!
 */
void SickTim3xx::MainQuit() {
	// Stop Measurment
	if(m_usb_device_handle){

		std::cout << "Sending stopping measurment message" << std::endl;

		int error =libusb_bulk_transfer(
				m_usb_device_handle,
				write_endpoint,
				stop_continuous_scan,
				sizeof(stop_continuous_scan),
				&transferred_data_size,
				timeout_millis);

		if(error){

#if defined WITH_LIB_USB_ERROR_NAME
			std::cout << "Write Bulk Transfer failed " << libusb_error_name(error) << std::endl;
#else
			std::cout << "Write Bulk Transfer failed: " << error << std::endl;
#endif


	//		exit(EXIT_FAILURE);

		}

		std::cout<< "Transfered Data written: " << transferred_data_size<< std::endl;

		error =libusb_bulk_transfer(
				m_usb_device_handle,
				read_endpoint,
				receive_buf ,
				sizeof(receive_buf)-1, // leave one for termination
				&transferred_data_size,
				timeout_millis);

		if(error){

#if defined WITH_LIB_USB_ERROR_NAME
			std::cout << "Read Bulk Transfer failed " << libusb_error_name(error) << std::endl;
#else
			std::cout << "Read Bulk Transfer failed " << error<< std::endl;
#endif
			//		exit(EXIT_FAILURE);

		}

		std::cout<< "Transfered Data read: " << transferred_data_size<< std::endl;
		// String terminierung
		receive_buf[transferred_data_size]=0;
		std::cout << receive_buf << std::endl;

		error= libusb_release_interface(m_usb_device_handle,the_usb_interface_number);

		if(error){

#if defined WITH_LIB_USB_ERROR_NAME
			std::cout << "Releasing interface failed " << libusb_error_name(error) << std::endl;
#else
			std::cout << "Releasing interface failed " << error << std::endl;
#endif
	//		exit(EXIT_FAILURE);

		}


		if( m_usb_device_handle){

			libusb_close(m_usb_device_handle);

		}
	}

	libusb_exit(m_usb_context);

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
int SickTim3xx::ProcessMessage(QueuePointer &resp_queue, player_msghdr* hdr,
		void* data) {

	assert(hdr);

	// Get Geometry
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ,
			PLAYER_LASER_REQ_GET_GEOM, device_addr)) {
		player_laser_geom_t geom;
		memset(&geom, 0, sizeof(geom));
		geom.pose.px = pose[0];
		geom.pose.py = pose[1];
		geom.pose.pyaw = pose[2];
		geom.size.sl = size[0];
		geom.size.sw = size[1];

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,
				PLAYER_LASER_REQ_GET_GEOM, (void*) &geom, sizeof(geom), NULL);
		return (0);
	}

	// Get ID information
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ,
			PLAYER_LASER_REQ_GET_ID, device_addr)) {

		player_laser_get_id_config_t idconfig  ;// =   lms100->GetConfigurationID();

		Publish(device_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,
				PLAYER_LASER_REQ_GET_ID, (void*) &idconfig, sizeof(idconfig),
				NULL);
		return (0);
	}

}

/**
 * Continous measurements
 */
void SickTim3xx::Main() {

	std::cout << "Entering  Main loop execution " <<std::endl;

		while (true) {

			// test if we are supposed to cancel
			pthread_testcancel();

			// Request/replies handler
			ProcessMessages();

			if(m_usb_device_handle){

				int error =libusb_bulk_transfer(
						m_usb_device_handle,
						read_endpoint,
						receive_buf ,
						sizeof(receive_buf)-1, // leave one for termination
						&transferred_data_size,
						timeout_millis);

				if(error){
#if defined WITH_LIB_USB_ERROR_NAME
					std::cout << "Read Bulk Transfer failed " << libusb_error_name(error) << std::endl;
#else
					std::cout << "Read Bulk Transfer failed " << error << std::endl;
#endif

					exit(EXIT_FAILURE);

				}

				// String Terminierung
				receive_buf[transferred_data_size]=0;

				if(receive_buf[0]!=DATA_STX){ // read from device buff until it is empty, and next data set starts with STX-Code
					std::cout << ".";
				} else {

				m_data_parser.set_pointer_to_data_buf(receive_buf,transferred_data_size);
				m_data_parser.parse_data();

				//Distances
				playerData.ranges_count = (uint32_t) (m_data_parser.datensatz_anzahl);
				playerData.ranges = ranges;

				int j = 0;
				for (uint32_t i = 0; i < playerData.ranges_count; i++) {
					j = i;
					if (upside_down) {
						j = playerData.ranges_count - i - 1;
					}
					playerData.ranges[i] = (((float) m_data_parser.dist_daten[j]) / m_data_parser.skalierungsfaktor / 1000);
					if (playerData.ranges[i] == 0) {
						playerData.ranges[i] = 4; //entspricht max range
					}
					if(debug){
						fprintf(stdout, ">>> [%i] dist: %f\n", i, playerData.ranges[i]);
					}
				}

				//Intensities
				playerData.intensity_count = (uint32_t) (m_data_parser.intens_datensatz_anzahl);
				playerData.intensity = intensities;

				j = 0;
				for (uint32_t i = 0; i < playerData.intensity_count; i++) {
					j = i;
					if (upside_down) {
						j = playerData.intensity_count - i - 1;
					}
					playerData.intensity[i] = m_data_parser.intens_daten[j];
					if(debug){
						fprintf(stdout, ">>> [%i] dist: %f\n", i, playerData.intensity[i]);
					}
				}

				Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_LASER_DATA_SCAN, &playerData);


			}
		}

	}
}

/**
 * Factory creation function. This functions is given as an argument when the
 * driver is added to the driver table
 */
Driver* SickTim3xx_Init(ConfigFile* cf, int section) {
	return ((Driver*) (new SickTim3xx(cf, section)));
}

/**
 * Registers the driver in the driver table. Called from the player_driver_init
 * function that the loader looks for
 */
void SickTim3xx_Register(DriverTable* table) {
	table->AddDriver("sicktim3xx", SickTim3xx_Init);
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
	SickTim3xx_Register(table);
	return (0);
}
}
