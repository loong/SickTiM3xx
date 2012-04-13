/**
 * @file    lms100_cola.cc
 * @author 	Kai Brach
 * @date    04/2010
 * @brief
 *
 */

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <libplayercore/playercore.h>

#include "htwg_LMS100_cola.h"
#include <unistd.h>




/**
 * Constructor for initializing the variables, send-, receive- and and value buffer
 *
 * @param host: Hostname of the LMS100 (e.g. "169.254.134.157")
 *
 * @param port: (e.g. 2111)
 *
 * @param debug_mode: 0 (disabled), 1 (enabled for standard messages), 2 (enabled  all messages including measurements)
 * - 0 marked with >
 * - 1 marked with >>
 * - 2 marked with >>>
 */
lms100_cola::lms100_cola(const char* host, int port, int debug_mode) {
	// init variables
	portno 			= port;
	hostname 		= host;
	debug 			= debug_mode;
	rbufferlength 	= 0;
	commandlength 	= 0;

	bzero(command, SEND_BUFFER_SIZE);
    bzero(rbuffer,READ_BUFFER_SIZE);
    bzero(vbuffer,VAL_BUFFER_SIZE);

    // Init all messages
	InitMessages();

}





/**
 * Connect to the LMS100 using hostname:portno
 *
 * @return  0 if connection was successful, -1 otherwise
 */
int lms100_cola::Connect() {
	// Create a socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "> Error: Open stream socket %d\n", sockfd);
		return -1;
	}

	// Get the network host entry
	server = gethostbyname((const char *) hostname);
	if (server == NULL) {
		fprintf(stderr, "> Error: Unknown host %s.\n", hostname);
		return -1;
	}

	// Fill in the sockaddr_in structure values
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port 	= htons(portno);
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

	// Attempt to connect
	if (int error = connect (sockfd, (const sockaddr*)&serv_addr, sizeof (serv_addr)) < 0) {
		fprintf(stderr, "> Error: Connecting stream socket %d\n", error);
		return -1;
	}

	return 0;
}





/**
 * Disconnect from the LMS100
 *
 * @return 0 if disconnection was successful, -1 otherwise
 */
int lms100_cola::Disconnect() {
	return (close(sockfd));
}





/**
 * Get the laser configuration
 *
 * @return player laser configuration (player_laser_config_t)
 */
player_laser_config lms100_cola::GetConfiguration() {
	player_laser_config_t cfg;
	cfg = playerConfig;
	return cfg;
}


/**
 * Get the laser configurationID
 *
 * @return player laser configuration ID (player_laser_get_id_config)
 */
player_laser_get_id_config lms100_cola::GetConfigurationID(){
	player_laser_get_id_config cfg;
	cfg.serial_number = this->measHeader.SerialNumber;
	return cfg;
}


/**
 * Defines which data are output with the message
 * !!!!! This function does not work yet! Damn, don't know why. The LMS100 answers with error code "sFA 8" !!!!!
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::ConfigureScanDataOutput() {
	char cmd[CMD_BUFFER_SIZE];


	// Declare Output Chanel [0x01 - 0xff]
	uint16_t OutputChannel = 0x01;

	// Return Remission Values [0 = no; 1 = yes]
	uint8_t Remission = 0;

	// Resolution of the Remission Values [0 = 8Bit; 1 = 16Bit]
	uint8_t Resolution = 1;

	// Unit of the Resolution Value [0 = Digits]
	uint8_t Unit = 0;

	// Encoder Values [0x00 = no Encoder; 0x01 = Encoder Channel 1; 0x02 - 0xff reserved]
	uint16_t Encoder = 0x00;

	// Return position values [0x00 = no; 0x01 = yes]
	uint8_t Position = 0x00;

	// Return  device name [0x00 = no; 0x01 = yes]
	uint8_t DeviceName = 0x00;

	// Return comment [0x00 = no; 0x01 = yes]
	uint8_t Comment = 0x00;

	// Return time [0x00 = no; 0x01 = yes]
	uint8_t Time = 0x00;

	// Determines which scan should be returned [1 = every scan; 2 = every second .... 50000 = every 50000]
	uint16_t Outputinterval = 1;

	// put msg into cmd
	snprintf(cmd, CMD_BUFFER_SIZE, this->Messages[REQ][SET_SCAN_OUTPUT],
			OutputChannel, Remission, Resolution, Unit,0, Encoder,00, Position,
			DeviceName, Comment, Time, Outputinterval);

	// sent the command
	SendCommand(cmd);

	// read the result
	int error = ReceiveResults();

	if (error == 0)
	{
		if (debug) {
			fprintf(stdout, ">> Output Channel: 0x0%d [0x01 - 0xff]\n", OutputChannel);

			fprintf(stdout, ">> Remission: %d [0 = no; 1 = yes]\n", Remission);

			fprintf(stdout, ">> Resolution: %d [0 = 8Bit; 1 = 16Bit]\n", Resolution);

			fprintf(stdout, ">> Encoder: 0x0%d [0x00 = no Encoder; 0x01 = Encoder Channel 1; 0x02 - 0xff reserved]\n", Encoder);

			fprintf(stdout, ">> Position: 0x0%d [0x00 = no; 0x01 = yes]\n", Position);

			fprintf(stdout, ">> Device Name: 0x0%d [0x00 = no; 0x01 = yes]\n", DeviceName);

			fprintf(stdout, ">> Comment: 0x0%d [0x00 = no; 0x01 = yes]\n", Comment);

			fprintf(stdout, ">> Output interval: %d [1 = every scan; 2 = every second .... 50000 = every 50000]\n", Outputinterval);
		}
	}

	return error;
}





/**
 * Set the desired userlevel by logging in with the appropriate password
 * By means of the selection of a user level and transfer of the corresponding password,
 * permits configuration of the LMS via messages. The LMS expects the password in the
 * message in coded form (hash value).
 *
 * @param userlevel
 *
 * @param password
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::SelectUserLevel(int8_t userlevel, const char* password) {
	char cmd[CMD_BUFFER_SIZE];

	// put message into cmd
	snprintf(cmd, CMD_BUFFER_SIZE, this->Messages[REQ][SET_USER_LEVEL],
			userlevel, password);

	// send the command
	SendCommand(cmd);

	// read the answer
	return ReceiveResults();
}




/**
 * The device is returned to the measurement mode after configuration.
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::StartDevice() {
	// put the message into cmd
	const char* cmd = this->Messages[REQ][SET_COMPLETE_CFG];

	// send the command
	SendCommand(cmd);

	// read the answer
	return ReceiveResults();
}




/**
 * Configures the LMS by means of the exact definition of scanning frequency, angular
 * resolution and starting/stopping angle. The LMS calculates the technically possible values
 * based on these parameters and supplies the parameters actually used in the response as
 * result.
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::SetResolutionAndFrequency(float freq, float ang_res, float angle_start, float angle_stop) {
	char cmd[CMD_BUFFER_SIZE];

	uint32_t frequ 			= (uint32_t) (freq * 100);
	uint8_t nSegments 		= 1;
	uint32_t aresolution 	= (uint32_t) (ang_res * 10000);
	int32_t astart 			= (int32_t) ((angle_start + 45) * 10000); //Add +45 to go to 270 in addition, Used for the settings. Dont know why!!
	int32_t astop 			= (int32_t) ((angle_stop + 45) * 10000);
	angle_range 			= angle_stop;

	//	printf("astart (berechnet)			=  %d\n",astart);
	//	printf("astop  (berechnet)			=  %d\n",astop);
	//	printf("Angle Start (uebergabe) 	=  %f\n",angle_start);
	//	printf("Angle Stop (uebergabe)		=  %f\n",angle_stop);
	//	printf("ANGLE RANGE 				= %d\n",angle_range);

	// put the message into cmd
	snprintf(cmd, CMD_BUFFER_SIZE, this->Messages[REQ][SET_SCAN_CFG], frequ,
			nSegments, aresolution, astart, astop);

	// send the command
	SendCommand(cmd);

	int error = ReceiveResults();

	// If no error, parse the results
	if (error == 0) {
		strtok((char*) rbuffer, " "); // Command Type
		strtok(NULL, " "); // Command
		int ErrorCode = strtol(strtok(NULL, " "), NULL, 16); // Error Code
		float sf = strtol(strtok(NULL, " "), NULL, 16); // Scan Frequency
		strtok(NULL, " "); // Segment Count
		float re = strtol(strtok(NULL, " "), NULL, 16); // Resolution
		//		float sa = strtol (strtok (NULL, " "), NULL, 16); 		// Start angle
		//		float ea = strtol (strtok (NULL, " "), NULL, 16); 		// Stop angle

		if ((ErrorCode != 0) && (debug))
			fprintf(stderr, ">> Warning: Got an error code %d\n", ErrorCode);

		memcpy(&playerConfig.scanning_frequency, &sf, sizeof(float));
		memcpy(&playerConfig.resolution, &re, sizeof(float));

		playerConfig.min_angle = angle_start * 10000;
		playerConfig.max_angle = angle_stop * 10000; // use the angle params, because setting start and stop angle are not working in LMS100


		// Check if scanning frequency is within the LMS100 parameters
		if (((playerConfig.scanning_frequency) != 5000) && ((playerConfig.scanning_frequency) != 2500)) {
			fprintf(stderr,"> Error: Frequency only can be 0.5 or 0.25! (Val = %f)\n",playerConfig.scanning_frequency / 100);
			return -1;
		}

		// Check if resolution is within the LMS100 parameters
		if (((playerConfig.resolution) != 5000) && ((playerConfig.resolution)
				!= 2500)) {
			fprintf(stderr,"> Error: Resolution only can be 0.5 or 0.25! (Val = %f)\n",playerConfig.resolution / 10000);
			return -1;
		}

		// Check angle_range
		if (((angle_range) < 1) || ((angle_range) > LMS100_MAXRANGE)) {
			fprintf(stderr,"> Error: Angle range not within [1° - 270°]! (Val = %d)\n",angle_range);
			return -1;
		}

		playerConfig.scanning_frequency = (float) (playerConfig.scanning_frequency / 100);
		playerConfig.resolution = (float) (playerConfig.resolution / 10000);
		// LMS100 cannot configure the angles
		playerConfig.min_angle = (float) (playerConfig.min_angle / 10000);//(float)(sa/10000);
		playerConfig.max_angle = (float) (playerConfig.max_angle / 10000);
		playerConfig.max_range = 20.0;
		playerConfig.range_res = 0.5;

		if (debug) {
			fprintf(stdout, ">> Scan Frequency: %f [25 - 50] Hz\n",
					playerConfig.scanning_frequency);
			fprintf(stdout, ">> Resolution: %f [0.25 - 0.5]\n",
					playerConfig.resolution);
			fprintf(stdout, ">> Min Angle: %f [-135° - +135°]\n",
					playerConfig.min_angle);
			fprintf(stdout, ">> Max Angle: %f [-135° - +135°]\n",
					playerConfig.max_angle);
			fprintf(stdout, ">> Angle Range: %d [270°]\n", angle_range);

		}
	}

	return error;

}



/*
 * Prints the LMS initialisation states
 */
void lms100_cola::PrintLMSStates(char state) {
	static char old_state = -1;

	if (state != old_state) {
		switch (state) {
		case 1:
			puts("LMS100: Initialisation");
			break;
		case 2:
			puts("LMS100: Configuration");
			break;
		case 3:
			puts("LMS100: IDLE");
			break;
		case 4:
			puts("LMS100: Rotated");
			break;
		case 5:
			puts("LMS100: In Preperation");
			break;
		case 6:
			puts("LMS100: Ready");
			break;
		case 7:
			puts("LMS100: Ready for Measurement");
			break;
		default:
			puts("LMS100: Error State");
			break;
		}

	}

	old_state = state;
}





/**
 * Loop till status is ready for measurement
 * Important: Only when the status 7 = “Ready for measurement” is achieved can measured data be requested from the LMS.
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::CheckState() {
	char cmd[CMD_BUFFER_SIZE];
	char s = -1;

	// IMPORTANT:
	// Status of the LMS must be ASCII 7 which means it is ready for measurement

	snprintf(cmd, CMD_BUFFER_SIZE, "%s", this->Messages[REQ][QUERY_STATE]);

	do {
		SendCommand(cmd);
		//rbuffer[10] = 0;

		if(ReceiveResults() != -1)
		{

			// Read the current state
			s = (uint8_t) ReadMessageBySection(2);
			// Print states on screen
			PrintLMSStates(s);

			// Error on reading message
			if (s < 0)
				return -1;
		}

	} while (s != 7);

	return 0; //Return ready for Measurement
}


int lms100_cola::ReadContaminationLevel()
{
	char cmd[CMD_BUFFER_SIZE];
	char s = -1;
	//Status of the LMS must be ASCII 7 which means it is ready for measurement

	snprintf(cmd, CMD_BUFFER_SIZE, "%s", this->Messages[REQ][READ_CONT_LEVEL]);
	SendCommand(cmd);

	ReceiveResults();

	s = (uint8_t) ReadMessageBySection(2);

	if (s < 0)
		return -1;

	switch (s) {
		case 0:
			puts("LMS100: No contamination");
			break;
		case 1:
			puts("LMS100: Contamination warning");
			break;
		case 2:
			puts("LMS100: Contamination error");
			return -1;
			break;
		case 3:
			puts("LMS100: Serious contamination error");
			return -1;
			break;
		default:
			break;
	}

	return 0;
}


/**
 *
 * The LMS starts measuring distances
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::StartMeasurement() {
	char cmd[CMD_BUFFER_SIZE];

	// Read contamination
	if(ReadContaminationLevel() != -1)
	{
		// write message in cmd
		snprintf(cmd, CMD_BUFFER_SIZE, "%s", this->Messages[REQ][START_MEASUREMENT]);

		// send command
		SendCommand(cmd);

		// read the answer for the message
		ReceiveResults();

		//check if State
		int state = CheckState();

		if (state < 0)
		{
			return -1;
		}
		else
		{

			// Start continous measurement value output
			int startmeasure = 1;
			snprintf(cmd, CMD_BUFFER_SIZE, this->Messages[REQ][READ_SCAN_DATA],startmeasure);

			SendCommand(cmd);

			if (ReceiveResults() < 0)
				return -1;
		}

		return 0;
	}else return -1;

}

/**
 * The measured value output is started using this message. It is a prerequisite for this action
 * that the LMS is in the “Measurement” status. For this purpose the measurement mode
 * must be started to receive new measurement values.
 *
 * @return Player laser data
 *
 */
player_laser_data_t lms100_cola::ReadMeasurement() {

	player_laser_data_t playerData;
	playerData.ranges_count = -1;
	playerData.intensity = new uint8_t[0]; //Initializing to avoid getting an error when freeing the memory in the calling function - if this function returns with an error.
	playerData.ranges = new float[0];
	bzero(rbuffer, READ_BUFFER_SIZE);
	bzero(vbuffer, VAL_BUFFER_SIZE);

	// Read the Message answer
	int n = ReceiveResults();

	if (n < 0) {
		if (debug)
			fprintf(stderr, ">> Error: Reading from socket!\n");
		return (playerData);
	}

	// Reading the data from the LMS
	MessageValues();

	// Fill in the received parameters

	measHeader.VersionNumber = (uint16_t) vbuffer[2];
	measHeader.DeviceNumber = (uint16_t) vbuffer[3];
	measHeader.SerialNumber = (uint32_t) vbuffer[4];
	measHeader.DeviceStatus = (uint32_t) vbuffer[5];
	measHeader.DeviceStatus2 = (uint32_t) vbuffer[6];

	measHeader.MessageCounter = (uint16_t) vbuffer[7];
	measHeader.ScanCounter = (uint16_t) vbuffer[8];
	measHeader.PowerUpDuration = (uint32_t) vbuffer[9];
	measHeader.TransmissionDuration = (uint32_t) vbuffer[10];

	measHeader.InputStatus = (uint16_t) vbuffer[11];
	measHeader.InputStatus2 = (uint16_t) vbuffer[12];

	measHeader.OutputStatus = (uint16_t) vbuffer[13];
	measHeader.OutputStatus2 = (uint16_t) vbuffer[14];

	measHeader.ReservedByteA = (uint16_t) vbuffer[15];
	measHeader.ScanningFrequency = (uint32_t) vbuffer[16];
	measHeader.MeasurementFrequency = (uint32_t) vbuffer[17];
	measHeader.NumberEncoders = (uint16_t) vbuffer[18];

	measHeader.EncoderPosition = 0;
	measHeader.EncoderSpeed = 0;
	measHeader.NumberChannels16bit = (uint16_t) vbuffer[19];
	measHeader.MeasuredDataContent = (uint32_t) vbuffer[20];

	//a scaling factor of 1000 is used to convert from mm to m
	measHeader.ScalingFactor = (uint32_t) 1000.0;
	measHeader.ScalingOffset = (uint32_t) vbuffer[22];
	measHeader.StartingAngle = (int32_t) vbuffer[23];
	measHeader.AngularStepWidth = (int16_t) vbuffer[24];
	measHeader.NumberData = (uint16_t) vbuffer[25];

	// Set the configuration carameters
	playerConfig.resolution = measHeader.AngularStepWidth / 10000.0;
	playerConfig.scanning_frequency = measHeader.ScanningFrequency / 100;
	playerConfig.max_range = 20;
	playerConfig.range_res = 0.5;
	playerConfig.intensity = 1; // Enable reflection intensity data


	// Fill in the appropriate values
	playerData.min_angle = DTOR(playerConfig.min_angle);
	playerData.max_angle = DTOR(playerConfig.max_angle);
	playerData.resolution = DTOR(playerConfig.resolution);
	playerData.max_range = playerConfig.max_range; // Maximum range of the LMS100
	playerData.intensity_count = 0;
	playerData.id = measHeader.ScanCounter;
	playerData.intensity = new uint8_t[playerData.intensity_count];


	// starting point in the array
	int starting = 0;

	// copy range data from the buffer into player_data struct.
	// only values between 0 and 180 degrees are used
	// In player_data it is -90 to 90 since we scan in the x-direction
	if ((playerConfig.resolution * 100) == 50) {
		// Example: Für 0.5 Auflösung, brauch ich bei 180′ 360 samples, da 180/0,5 = 360
		// Anfangen zu zählen muss man bei 90 Werten, da scan bereich von -45 bis 225 pro grad
		// daher 45*2 = 90


		playerData.ranges_count = (uint32_t) ((measHeader.NumberData * angle_range) / LMS100_MAXRANGE) + 1;
		starting = (measHeader.NumberData - playerData.ranges_count) / 2; // Starting Point Nomally 45, but since we have 0.5 we use it twice

		playerData.ranges = new float[playerData.ranges_count];

		for (uint32_t i = 0; i < playerData.ranges_count; i++) {
			playerData.ranges[i] = (((float) vbuffer[26 + (starting) + i])
					/ measHeader.ScalingFactor);
			if (debug == 2)
				fprintf(stdout, ">>> [%i] dist: %f\n", i, playerData.ranges[i]);
		}

	}

	else if ((playerConfig.resolution * 100) == 25) {
		// Since this is the half of 0.5 we use twice the values
		playerData.ranges_count = (uint32_t) ((measHeader.NumberData * (angle_range * 2)) / LMS100_MAXRANGE) + 1;
		starting = (measHeader.NumberData - playerData.ranges_count); // Starting Point

		playerData.ranges = new float[playerData.ranges_count];

		for (uint32_t i = 0; i < playerData.ranges_count; i++) {
			playerData.ranges[i] = (((float) vbuffer[26 + (starting) + i])
					/ measHeader.ScalingFactor);
			if (debug == 2)
				fprintf(stdout, ">>> [%i] dist: %f\n", i, playerData.ranges[i]);
		}
	}

	return (playerData);
}






/**
 *
 * The LMS stops measuring
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::StopMeasurement() {
	char cmd[CMD_BUFFER_SIZE];
	snprintf(cmd, CMD_BUFFER_SIZE, "%s", Messages[REQ][STOP_MEASUREMENT]);//"sMN mLRstopdata"
	if(SendCommand(cmd) != -1)
		return (ReceiveResults());
	else
		return -1;
}




/**
 * Send a command to the laser unit.
 *
 * @param cmd LMS100 Message
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::SendCommand(const char* cmd) {

	// Print Debug Message
	if (debug && strncmp((const char*) cmd, Messages[REQ][QUERY_STATE], 9) != 0)
		fprintf(stdout, ">> Sent: \"%s\"\n", cmd);

	// assemble the command with <STX> [data] <ETX>
	AssembleCommand((unsigned char *) cmd, strlen(cmd));

	n = write(sockfd, command, commandlength);

	if (n < 0) {
		fprintf(stderr, "> Error: Sending Data.\n");
		return (-1);
	}

	return 0;
}




/**
 * Reads the results and filled the buffer with values
 *
 * @return 0 if successful, -1 otherwise
 */
int lms100_cola::ReceiveResults() {

	char sb_char; // synch break
	int current = 0; // current lenght of the buffer
	rbufferlength = 0; // lenght of the buffer
	bzero(rbuffer, READ_BUFFER_SIZE);

	// Look for STX
	while (true) {
		sb_char = 0x00;

		n = read(sockfd, &sb_char, 1);

		if (n < 0) {
			fprintf(stderr,"> Error: Reading from Socket. Expected STX!\n");
			return (-1);
		}

		if (sb_char == 0x02) // found a STX
		{
			break;
		}

	}

	// Read data packet and look for ETX
	while (true) {
		sb_char = 0x00;
		n = read(sockfd, &sb_char, 1);
		if (n < 0) {
			fprintf(stderr,"> Error: Received an error message with code 0x%s \n",
					strtok(NULL, " "));

			return (-1);
		}

		if (n == 1) {
			rbuffer[current] = sb_char;
			current++;

			// Check if buffer contains error message "sFA"
			if (current > 4) {
				if (strncmp((const char*) rbuffer, "sFA", 3) == 0) {
					strtok((char*) rbuffer, " ");
					return (-1);
				}
			}
		}

		if (sb_char == 0x03) // found a ETX
		{
			rbuffer[current] = 0x03;
			break;
		}
	}

	if (debug && strncmp((const char*) rbuffer, "sAN mLMPsetscancfg", 18) == 0)
		fprintf(stdout, ">> Recv: \"%s\"\n", rbuffer);

	rbufferlength = current;

	return 0;

}



/**
 * Adds STX and ETX to the command
 *
 * @param cmd LMS100 Message
 * @param len command length
 *
 * @return length of the entire command
 */
int lms100_cola::AssembleCommand(unsigned char* cmd, int len) {
	int index = 0;

	command[0] = 0x02; // Messages start with 1 STX

	for (index = 0; index < len; index++) {
		command[index + 1] = cmd[index];
	}

	command[1 + len] = 0x03; // Messages end with 1 ETX

	commandlength = 2 + len;
	return commandlength;

}


/**
 * Returns the value of the array, which is calculated byte on byte
 *
 * @param section Array section (see individual command)
 *
 * @return Value of the choosen index
 */
uint64_t lms100_cola::ReadMessageBySection(int section) {
	char* data[READ_BUFFER_SIZE];
	char delimiter[] = " ";
	char *ptr;
	int index = 0;
	uint64_t output = 0;

	// init and create first section
	ptr = strtok((char*) rbuffer, delimiter);


	// put all data into array
	while (ptr != NULL) {

		data[index] = ptr;
		ptr = strtok(NULL, delimiter);
		index++;
	}

	if (section > index) {
		fprintf(stderr,"> Error: Section [%d] is bigger than size of message array [%d]\n",section, index);
		return -1;
	}

	// Convert the string to an integer
	output = (uint64_t) strtol(data[section], NULL, 16);

	return output;
}



/**
 * Puts all values of the LMS100 measurement into an array
 */
void lms100_cola::MessageValues() {
	//char* data[READ_BUFFER_SIZE];
	char delimiter[] = " ";
	char *ptr;
	int index = 0;

	// init and create first section
	ptr = strtok((char*) rbuffer, delimiter);

	// put all data into array
	while (ptr != NULL) {

		// Convert the string to an integer
		vbuffer[index] = (uint64_t) strtol(ptr, NULL, 16); //die ersten 2 noch weg machen??

//		if (strncmp ((const char*)ptr, "RSSI1", 5) == 0)
//		{
//			printf("Index[%d] Ptr = %s vBuffer = %lu\n",index, ptr,vbuffer[index]);
//		}


		//if(index >=0 && index <=26)
		//printf("Index[%d] Ptr = %s ValueBuffer = %lu\n",index, ptr,vbuffer[index]);

		ptr = strtok(NULL, delimiter);
		index++;

	}

}

/**
 * Initializes all messages on startup
 */
void lms100_cola::InitMessages() {

	//	#define START_MEASUREMENT 	0
	//	#define STOP_MEASUREMENT 	1
	//	#define QUERY_STATE 		2
	//	#define READ_SCAN_DATA		3 //continously
	//	#define READ_SCAN_CFG		4
	//	#define READ_CONT_LEVEL		5
	//	#define SET_USER_LEVEL		6
	//	#define SET_SCAN_CFG		7
	//	#define	SET_SCAN_OUTPUT		8
	//	#define SET_COMPLETE_CFG	9


	//	sMN = method by Name
	//	sWN = write by Name
	//	sRN = read by Name
	//	sEN = event by Name

	// Requests (SEND)
	Messages[REQ][START_MEASUREMENT] 	= "sMN LMCstartmeas";
	Messages[REQ][STOP_MEASUREMENT] 	= "sMN LMCstopmeas";
	Messages[REQ][QUERY_STATE] 			= "sRN STlms";
	Messages[REQ][READ_SCAN_DATA]		= "sEN LMDscandata %d"; //continous = sEN; once = SRN
	Messages[REQ][READ_SCAN_CFG] 		= "sRN LMPscancfg";
	Messages[REQ][READ_CONT_LEVEL] 		= "sRN LCMstate";
	Messages[REQ][SET_USER_LEVEL] 		= "sMN SetAccessMode %d %s";
	Messages[REQ][SET_SCAN_CFG] 		= "sMN mLMPsetscancfg +%d +%d +%d %d +%d";
	Messages[REQ][SET_SCAN_OUTPUT]		= "sWN LMDscandatacfg %d %d %d %d %d %d %d %d %d %d %d +%d";//"sWN LMDscandatacfg 01 00 0 1 0 00 00 0 0 0 0 +1";
	Messages[REQ][SET_COMPLETE_CFG] 	= "sMN Run";
	Messages[REQ][EMPTY_1] = "";
	Messages[REQ][EMPTY_2] = "";
	Messages[REQ][EMPTY_3] = "";
	Messages[REQ][EMPTY_4] = "";


	// Answer on Request
	Messages[ANS][START_MEASUREMENT] 	= "sAN LMCstartmeas";
	Messages[ANS][STOP_MEASUREMENT] 	= "sAN LMCstopmeas";
	Messages[ANS][QUERY_STATE] 			= "sRA STlms";
	Messages[ANS][READ_SCAN_DATA] 		= "sSN LMDscandata";
	Messages[ANS][READ_SCAN_CFG] 		= "sRA LMPscancfg";
	Messages[ANS][READ_CONT_LEVEL]		= "sRA LCMstate";
	Messages[ANS][SET_USER_LEVEL] 		= "sAN SetAccessMode";
	Messages[ANS][SET_SCAN_CFG] 		= "sAN mLMPsetscancfg";
	Messages[ANS][SET_SCAN_OUTPUT] 		= "sWA LMDscandatacfg";
	Messages[ANS][SET_COMPLETE_CFG] 	= "sAN Run";
	Messages[ANS][EMPTY_1] = "";
	Messages[ANS][EMPTY_2] = "";
	Messages[ANS][EMPTY_3] = "";
	Messages[ANS][EMPTY_4] = "";

}
