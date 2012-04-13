/*
 * CamSFZ.hpp
 *
 *  Created on: 24.09.2010
 *      Author: asterix
 */

#ifndef CAMSFZ_HPP_
#define CAMSFZ_HPP_

#include <string>
#include <player-3.0/libplayercore/playercore.h>
#include <player-3.0/libplayerinterface/playerxdr.h>
#include <iostream>

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>


class CamSFZ : public ThreadedDriver{
public:
	CamSFZ(ConfigFile* cf, int section);

	// MessageHandler
	virtual int ProcessMessage(QueuePointer &resp_queue, player_msghdr* hdr,void* data);

private:
	// Main function for device thread.
	virtual void Main();
	virtual int MainSetup();
	virtual void MainQuit();

	void PublishData();

	int device;

	CvCapture* capture;
	IplImage* frame;

	int sleep_nsec;

};

#endif /* CAMSFZ_HPP_ */
