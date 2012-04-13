/**
 * @file    lms100_cola.h
 * @author 	Kai Brach
 * @date    04/2010
 * @brief
 *
 */


#include <sys/types.h>
#include <vector>
#include <netinet/in.h>
#include <libplayercore/playercore.h>
#include <iostream>

// Define Buffer Sizes
#define SEND_BUFFER_SIZE 	1024	//Sending
#define READ_BUFFER_SIZE 	4096	//Reading
#define CMD_BUFFER_SIZE 	255		//Each CMD
#define VAL_BUFFER_SIZE 	2048	//Calculated Values
#define LMS100_MAXRANGE 	270

// Define MessageNumbers (send and receive)
#define MESSAGE_NUMBER		14
#define REQ 				0
#define ANS 				1

// Array Position of the Messages
#define START_MEASUREMENT 	0
#define STOP_MEASUREMENT 	1
#define QUERY_STATE 		2
#define READ_SCAN_DATA		3 //continously
#define READ_SCAN_CFG		4
#define READ_CONT_LEVEL		5
#define SET_USER_LEVEL		6
#define SET_SCAN_CFG		7
#define	SET_SCAN_OUTPUT		8
#define SET_COMPLETE_CFG	9
#define EMPTY_1				10
#define EMPTY_2				11
#define EMPTY_3				12
#define EMPTY_4				13

////////////////////////////////////////////////////////////////////////////////
typedef struct {

	/*Device Information*/
	uint16_t VersionNumber;
	uint16_t DeviceNumber;
	uint32_t SerialNumber;
	uint32_t DeviceStatus;
	uint32_t DeviceStatus2;

	/*Status Information*/
	uint16_t MessageCounter;
	uint16_t ScanCounter;
	uint32_t PowerUpDuration;
	uint32_t TransmissionDuration;
	uint16_t InputStatus;
	uint16_t InputStatus2;
	uint16_t OutputStatus;
	uint16_t OutputStatus2;
	uint16_t ReservedByteA;

	/*Measurement Parameter*/
	uint32_t ScanningFrequency;
	uint32_t MeasurementFrequency;

	/*Encoder*/
	uint16_t NumberEncoders;
	uint32_t EncoderPosition; // Not Implemented (No Message from LMS)
	uint16_t EncoderSpeed; // Not implemented (No Message form LMS)
	uint16_t NumberChannels16bit;

	uint32_t MeasuredDataContent;

	/*Output Chanell (16 Bit)*/
	uint32_t ScalingFactor;
	uint32_t ScalingOffset;
	int32_t StartingAngle;
	uint16_t AngularStepWidth;
	uint16_t NumberData;
} MeasurementHeader_t;

////////////////////////////////////////////////////////////////////////////////
class lms100_cola {
public:
	lms100_cola(const char* host, int port, int debug_mode);

	// creates socket, connects, disconnects
	int Connect();
	int Disconnect();

	// configuration parameters
	int SetResolutionAndFrequency(float freq, float ang_res, float angle_start,float angle_range);

	// determines which data should be returned from the LMS
	int ConfigureScanDataOutput();

	// starts the measurement
	int StartMeasurement();

	// read the current measurement data
	player_laser_data_t ReadMeasurement();

	// stops the current measurement
	int StopMeasurement();

	// setting userlevel for storing configuration parameters
	int SelectUserLevel(int8_t userlevel, const char* password);

	// starting the device
	int StartDevice();

	// returns the current configuration
	player_laser_config GetConfiguration();

	// returns the device id
	player_laser_get_id_config GetConfigurationID();

private:

	// connection variables
	const char* hostname;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// Internal Parameters:
	int debug;

	// calculated range
	uint32_t angle_range;


	// player variables
	player_laser_config playerConfig;
	MeasurementHeader_t measHeader;


	// for reading:
	// buffer for receiving entire protocol (byte on byte)
	unsigned char rbuffer[READ_BUFFER_SIZE];

	// number of bytes in the rbuffer
	unsigned int rbufferlength;

	// buffer which contains parsed rbuffer values
	// each byte as value with index
	uint64_t vbuffer[VAL_BUFFER_SIZE];


	// for sending:
	unsigned char command[SEND_BUFFER_SIZE];
	int commandlength;




	// Declaration of all REQUEST (REQ) and ANSWER (ANS) Messages
	// If more messages shall be included increase Message_Number
	// ** Answer Messages not in use!!!
	const char* Messages[2][MESSAGE_NUMBER];

	// assembles frame to be sent
	int AssembleCommand(unsigned char* command, int len);

	// send the command
	int SendCommand(const char* cmd);

	// receives the answer
	int ReceiveResults();

	// Read the contamination level of the LMS100
	int ReadContaminationLevel();


	// reads an individiual message value by choosing the specified section
	uint64_t ReadMessageBySection(int section);

	// reads all messages and fills up the vbuffer like an array of
	// concrete values
	void MessageValues();

	// inits a matrix of send and receive messages
	void InitMessages();

	// checks the init states of the LMS100
	int CheckState();

	// prints the init states of the LMS100
	void PrintLMSStates(char state);


};
