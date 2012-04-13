/*
 * ComputerVisionClass.h
 *
 *  Created on: 28.05.2011
 *      Author: locher
 */

#ifndef COMPUTERVISIONBASECLASS_H_
#define COMPUTERVISIONBASECLASS_H_

#include <unistd.h>
#include <cstdlib>
#include <cstdio>

#include <cstring>
#include <string>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/thread/mutex.hpp>


#include "CV_Cam_Capture_Thread.h"

#include "Thread.h"


#include "Player_Cam_Driver.hpp"




#define NAMED_PIPE_NAME "/tmp/lulo_pipe"

/**
 * Base Class for Image Processing with Player_Cam_Driver
 *
 * Overwrite process image Method
 *
 * Define a struct for the Opaque data you like to send
 *
 * Write a Proxy Class which provides getter methods to recieve this data
 *
 */



class ComputerVisionBaseClass : public Thread {


private:

	// Named Pipe
	int named_pipe_fd;
	int res;
	int bytes_written;
	char buffer[PIPE_BUF+1];


	boost::mutex my_ip_mutex;

	int cap_img_width;
	int cap_img_height;
	ImagePool image_pool;

	std::string named_pipe_name;
	int cam_index;

	CV_Cam_Capture_Thread* cam_capture_thread;

	void process();

protected:


	/**
	 * Overwrite this method for Image Processing
	 * Method is called from process loop
	 * Dont forget to send Data to Player Cam Driver
	*/
	virtual void process_image(IplImage*)=0;

	/**
	 *  Use this method for sending opaque data to Player Cam driver
	 */
	size_t send_opaque_data_to_player_cam_driver(void* data, size_t  data_length);






public:
	ComputerVisionBaseClass(std::string named_pipe_name, int cam_index);
	virtual ~ComputerVisionBaseClass();

	int get_cap_img_width();
	int get_cap_img_hight();
};

#endif /* COMPUTERVISIONBASECLASS_H_ */
