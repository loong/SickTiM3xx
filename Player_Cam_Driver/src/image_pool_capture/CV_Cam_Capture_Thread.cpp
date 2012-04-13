/*
 * CV_Cam_Capture_Thread.cpp
 *
 *  Created on: 25.09.2010
 *      Author: obelix
 */

#include "CV_Cam_Capture_Thread.h"

CV_Cam_Capture_Thread::CV_Cam_Capture_Thread(
		int cam_index, ImagePool& img_pool, int img_width, int  img_height)

	:cap_ptr(0),
	pause_capture(true),
	myImagePoolRef(img_pool)


{
	if( !  (cap_ptr=cvCaptureFromCAM(cam_index) ) ){
		std::cerr << "Could not Capture from Cam: " << cam_index << std::endl;
	}else{

//		cvSetCaptureProperty(cap_ptr,CV_CAP_PROP_FRAME_WIDTH,img_width);
//
//		cvSetCaptureProperty(cap_ptr,CV_CAP_PROP_FRAME_HEIGHT,img_height);
	}
}

CV_Cam_Capture_Thread::~CV_Cam_Capture_Thread() {

	if(cap_ptr!=0){
		cvReleaseCapture(&cap_ptr);
	}
}


void CV_Cam_Capture_Thread::process(){
	std::cout <<"CV_CamCapture_Thread" << std::endl;

	boost::mutex::scoped_lock sl(my_mutex);

	if(!pause_capture){

		std::cout <<"CV_CamCapture_Thread 2" << std::endl;

		if(cap_ptr!=0){
			IplImage* cap_img=cvQueryFrame(cap_ptr);

			std::cout <<"CV_CamCapture_Thread 3" << std::endl;

			if(cap_img!=0){

				std::cout <<"CV_CamCapture_Thread 4" << std::endl;
				std::cout <<"Cap img: " <<  cap_img->width <<  " " << cap_img->height << std::endl;

				IplImage* img=myImagePoolRef.lock_Img_to_write_img();

				if(img){

					cvCopy(cap_img,img);

					myImagePoolRef.unlock_Img_from_write(img);
				}else{
					std::cout << "Img Pool: no image available" << std::endl;
				}
			}
		}
	}

}



void CV_Cam_Capture_Thread::set_pause_capture(bool pause){

	boost::mutex::scoped_lock sl(my_mutex);
	pause_capture=pause;


}

