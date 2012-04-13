/*
 * CV_Cam_Capture_Thread.h
 *
 *  Created on: 25.09.2010
 *      Author: obelix
 */

#ifndef CV_CAM_CAPTURE_THREAD_H_
#define CV_CAM_CAPTURE_THREAD_H_

#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>


#include <boost/thread/mutex.hpp>



#include "Thread.h"

#include "ImagePool.h"

class CV_Cam_Capture_Thread : public Thread {

private:

	boost::mutex my_mutex;

	CvCapture* cap_ptr;

	volatile bool pause_capture;

	ImagePool& myImagePoolRef;



public:
	CV_Cam_Capture_Thread(int cam_index, ImagePool& img_pool,int img_width,int img_height);
	virtual ~CV_Cam_Capture_Thread();

	void set_pause_capture(bool pause);

	virtual void  process();

};

#endif /* CV_CAM_CAPTURE_THREAD_H_ */
