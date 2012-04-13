/*
 * multi.h
 *
 *  Created on: 08.04.2010
 *      Authors: adrian locher
 */

#ifndef PLAYER_CAM_DRIVER_HPP_
#define PLAYER_CAM_DRIVER_HPP_


#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>


#include <player-3.0/libplayercore/playercore.h>

#include <iostream>




using namespace std;




/**
 * \brief Player-Driver for Cam-DAta-Processing
 *
 * Provides the following Interfaces
 * <ul>
 * <li> Opaque
 * <ul>
 * </ul>
 */

class Player_Cam_Driver : public ThreadedDriver{



public:


    // Constructor; need that
    Player_Cam_Driver(ConfigFile* cf, int section);
    virtual ~Player_Cam_Driver();

    virtual int ProcessMessage(QueuePointer & resp_queue,
                               player_msghdr * hdr,
                               void * data);


    virtual int MainSetup();
    virtual void MainQuit();
    // virtual int Setup();


private:

    int data_size;

    // Main function for device thread.
    virtual void Main();


	int named_pipe_fd;
	int res;
	int bytes_read;
	char buffer[PIPE_BUF+1];

	std::string named_pipe_name;


	  // My opaque interface
	   player_devaddr_t m_opaque_addr;


      //Control off Joystick driver

      int main_loop_sleep_time_us;



    //eigene funktionen
    void PublishOpaque();

    int  handle_opaque_0_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);


    void gtk_main_thread_function();

    bool debug;
};

#endif /* PLAYER_CAM_DRIVER_HPP_ */
