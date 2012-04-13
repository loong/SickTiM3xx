#include "roboOneMotors.h"
#include <sys/time.h>

RoboOneMotors::RoboOneMotors(char* port,unsigned long baud)
{
	strcpy(this->port,port);

	switch (baud)
	{
		case 9600:
			this->baudrate = B9600;
			break;
		case 38400:
			this->baudrate = B38400;
			break;
		case 115200:
			this->baudrate = B115200;
			break;
		default:
			cout << "FEHLER: Ungültige Baudrate\n";
			this->baudrate = B9600;
			break;
	}
	pthread_mutex_init(&send_mutex,0);
	pthread_mutex_init(&data_mutex,0);
	pthread_mutex_init(&new_data_mutex,0);
	return;
}

RoboOneMotors::~RoboOneMotors()
{
	this->Disconnect();
}

int RoboOneMotors::WaitData()
{
	struct timespec tTimeOut;
	int iReturn;
	// get actual time for timeout
	clock_gettime(CLOCK_REALTIME,&tTimeOut);
	// set timeout time
	tTimeOut.tv_sec += 2;
	iReturn = pthread_mutex_timedlock(&this->new_data_mutex,&tTimeOut);
	return iReturn;
}

int RoboOneMotors::Connect()
{
	char tmpBuffer[MSG_START_LENGTH + 1];
	char tmpString[MSG_START_LENGTH+1] = MSG_START_WORD;
	int res,retrys,pkgnum,nodata;

	fd = open(this->port, O_RDWR | O_NOCTTY );
	if (fd <=0)
	{
		//cout << "RoboOneMotors: Fehler beim Öffnen\n";
		return -1;
	}

	tcgetattr(fd,&oldtio); /* save current port settings */
	memset(&newtio,0,sizeof(newtio));
	cfsetispeed(&newtio,this->baudrate);
	cfsetospeed(&newtio,this->baudrate);



	newtio.c_cflag |= (CLOCAL | CREAD);
  newtio.c_cflag &= ~(CSIZE|PARENB);
  newtio.c_cflag |= CS8;   // Select 8 data bits
  newtio.c_cflag |= CSTOPB;  // send 2 stop bits
  newtio.c_cflag &= ~CRTSCTS;
  newtio.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	cfsetospeed(&newtio,this->baudrate);

	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = 0;   /* blocking read until num chars received */

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

/*
	// Datenmüll aus Puffer lesen
	res = read(fd,&tmpBuffer,MSG_START_LENGTH);
	while(res > 0)
	{
		res = read(fd,&tmpBuffer,MSG_START_LENGTH);
		printf("Lese Datenmuell aus Puffer\n");
	}
*/
	// Versuche Verbindung aufzubauen
 	for (retrys = 0; retrys < NUM_RETRYS; retrys++)
	{
		// Startwort an uC senden
		//printf("\nSende Verbindungsanfrage Nr.:%d\nWarte auf Antwort...",retrys);
		res = write(fd,MSG_START_WORD,MSG_START_LENGTH);

		// Erwarte Startwort von uC
		memset(&tmpBuffer,0,MSG_START_LENGTH);

		pkgnum = 0;
		nodata = 0;

		// Beenden der Schleife wenn keine Daten empfangen wurden
		// oder das Startpaket korrekt empfangen wurde
		while(pkgnum < MSG_START_LENGTH && nodata < 10)
		{
			res = read(fd,&tmpBuffer,1);
			// Falls keine Daten empfangen wurden
			if (res == 0)
			{
				//printf("Keine Daten vorhanden\n");
				usleep(50000);
				nodata++;
			}
			if (tmpBuffer[0] == tmpString[pkgnum])
			{
				pkgnum++;
			}
			else
			{
				pkgnum = 0;
			}
		}
		// Prüfe ob Paket korrekt empfangen wurde
		if (pkgnum == MSG_START_LENGTH)
		{
			// Ok
			break;
		}
		else
		{
			// nächster Versuch
			//printf("Timeout!\n");
			continue;
		}
	}

	// Es konnte keine Verbindung aufgebaut werden
	if (retrys == NUM_RETRYS)
	{
		return -1;
	}
	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = sizeof(recv_message);   /* blocking read until 5 chars received */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	pthread_mutex_unlock(&send_mutex);
	pthread_mutex_unlock(&data_mutex);
	pthread_mutex_trylock(&new_data_mutex);
	m_iStall = 0;


	usleep(300000);
	pthread_create(&this->mreadThread,NULL,RoboOneMotors::readThread,this);
	usleep(500000);
	return 0;

}

int RoboOneMotors::Disconnect()
{

	//cout << "Beende Empfangsthread\n";
	if (mreadThread != 0)
	{
		pthread_cancel(mreadThread);
		usleep(200000);
	}
	if (fd != 0)
	{
//		cout << "disconnecting and resetting serial port" << endl;
		tcsetattr(fd,TCSANOW,&oldtio);
		close(fd);
	}
	return 0;
}

int RoboOneMotors::GetStall()
{
	int iRetval;
	pthread_mutex_lock(&this->data_mutex);
	iRetval = m_iStall;
	pthread_mutex_unlock(&this->data_mutex);
	return iRetval;
}

int RoboOneMotors::SetPIDparams(float pid_p1, float pid_p2, float pid_i1, float pid_i2)
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_REG_PARAM_P;
	memcpy(&send_message.data0,&pid_p1,sizeof(float));
	memcpy(&send_message.data2,&pid_p2,sizeof(float));
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	usleep(10000);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_REG_PARAM_I;
	memcpy(&send_message.data0,&pid_i1,sizeof(float));
	memcpy(&send_message.data2,&pid_i2,sizeof(float));
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));

	pthread_mutex_unlock(&this->send_mutex);
	return 0;

}

int RoboOneMotors::SetBreakAmp(float break_amp)
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_REG_BREAK_AMP;
	memcpy(&send_message.data0,&break_amp,sizeof(float));
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	pthread_mutex_unlock(&this->send_mutex);
	return 0;

}

int RoboOneMotors::SetSpeed(int16_t left, int16_t right)
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_MOTOR_VELOCITY_CMD;
	send_message.data0 = -right;
	send_message.data1 = -left;
	//send_message.data0 = left;
	//send_message.data1 = right;
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	pthread_mutex_unlock(&this->send_mutex);
	/*
	if (left)
	{
		cout << "SetSpeed\n";
	}
	*/
	return 0;
}

void RoboOneMotors::GetSpeed(int16_t& left, int16_t& right)
{
	pthread_mutex_lock(&this->data_mutex);
	left = -this->speed_right;
	right = -this->speed_left;
	//left = this->speed_left;
	//right = this->speed_right;
	pthread_mutex_unlock(&this->data_mutex);
	return;
}

void RoboOneMotors::GetTicks(int16_t& left, int16_t& right)
{
	pthread_mutex_lock(&this->data_mutex);
	left = -this->ticks_right;
	right = -this->ticks_left;
	//left = this->ticks_left;
	//right = this->ticks_right;
	pthread_mutex_unlock(&this->data_mutex);
	return;
}

void RoboOneMotors::EmergencyStop()
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_MOTOR_STOP;
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	pthread_mutex_unlock(&this->send_mutex);
	return;
}

void RoboOneMotors::ResetOdometry()
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_RESET_ODO;
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	pthread_mutex_unlock(&this->send_mutex);
	return;
}

void* RoboOneMotors::work_readThread(void*)
{
	struct timeval startTime,endTime;
	//double timediff;
	int res;
	while (1)
	{
		startTime = endTime;
		gettimeofday(&endTime,NULL);
		res = read(fd,&recv_message,sizeof(recv_message));
/*
		printf("Size:%d\n",res);

		printf("Type:%d Data0:%d Data1:%d Data2:%d Data3:%d Check:%d\n",
		mymessage.type,mymessage.data0,mymessage.data1,mymessage.data2,
		mymessage.data3,mymessage.check);
*/
		// Nachrichten verarbeiten....
//		cout << "Anz. Empfangene Bytes: " << res << endl;
		switch(recv_message.type)
		{
			case MSG_ODO_DATA:
				pthread_mutex_lock(&this->data_mutex);
				ticks_left = recv_message.data0;
				ticks_right = recv_message.data1;
				speed_left = recv_message.data2;
				speed_right = recv_message.data3;
				m_iStall = recv_message.stall;
				pthread_mutex_unlock(&this->data_mutex);
				// signal that new data is ready
				pthread_mutex_unlock(&this->new_data_mutex);
				break;
			default:
				cout << "RoboOneMotors: Unbekanntes Kommando\n";
		}
		/*
		timediff = (endTime.tv_sec - startTime.tv_sec) * 1e3;
		timediff += (endTime.tv_usec - startTime.tv_usec) / 1e3;
		cout << "Zeitdifferenz: " << timediff << "ms" << endl;
		if (speed_left)
		{
			cout << "Motor dreht...\n" << speed_left << "ticks/10ms" << endl;
		}
		*/

	}
}

void RoboOneMotors::SendKeepAlive()
{
	int res;
	// Nachricht bauen
	pthread_mutex_lock(&this->send_mutex);
	memset(&send_message,0,sizeof(send_message));
	send_message.type = MSG_KEEP_ALIVE;
	// Nachricht schicken
	res = write(fd,&send_message,sizeof(send_message));
	pthread_mutex_unlock(&this->send_mutex);
	return;
}
