#ifndef ROBOONEMOTORS_H
#define ROBOONEMOTORS_H

#include <stdint.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <pthread.h>

#define BAUDRATE B115200

#define MSG_MOTOR_STOP 0x10U
#define MSG_MOTOR_VELOCITY_CMD 0x20U
#define MSG_ODO_DATA 0x30U
#define MSG_RESET_ODO 0x40U
#define MSG_KEEP_ALIVE 0x50U

#define MSG_REG_PARAM_P 0x70U
#define MSG_REG_PARAM_I 0x71U
#define MSG_REG_BREAK_AMP 0x72U

// Defines für Verbindungsaufbau
#define MSG_START_WORD "START"
#define MSG_START_LENGTH 5


#define MAX_TICK_CHANGE 1000

#define EUROBOT_INT_MAX 32767
#define EUROBOT_INT_MIN -32768

#define NUM_RETRYS 5



struct message{
	uint8_t type;
	int16_t data0;
	int16_t data1;
	int16_t data2;
	int16_t data3;
	uint8_t stall;
}  __attribute__((__packed__));

struct message_pid{
	uint8_t type;
	float param0;
	float param1;
	uint8_t check;
}  __attribute__((__packed__));



using namespace std;

class RoboOneMotors
{
	public:
		RoboOneMotors(char* port, unsigned long baud);
		~RoboOneMotors();
		
		int Connect();
		int Disconnect();

		int WaitData();

		int SetPIDparams(float pid_p1, float pid_p2, float pid_i1, float pid_i2);
		int SetBreakAmp(float break_amp);
		int SetSpeed(int16_t left, int16_t right);
		int GetStall();
		void GetSpeed(int16_t& left,int16_t& right);
		void GetTicks(int16_t& left,int16_t& right);
		void ResetOdometry();
		void SendKeepAlive();
		
		void EmergencyStop();
	private:
		RoboOneMotors();

		// Variablen
		struct termios oldtio,newtio;
		struct message recv_message;
		struct message send_message;
		char port[255];
		unsigned long baudrate;
		int fd;

		int speed_left,speed_right;
		int ticks_left,ticks_right;
		int m_iStall;

	
		pthread_t mreadThread;
		pthread_mutex_t send_mutex, data_mutex, new_data_mutex;
		void* work_readThread(void*);
		// Damit die Adresse des Threads gefunden werden kann
		// Workaround
		static void *readThread(void *a)
		{
			RoboOneMotors *mt = static_cast<RoboOneMotors *>(a);
			mt->work_readThread(a);
			return (void*)0;
		}
};



#endif

