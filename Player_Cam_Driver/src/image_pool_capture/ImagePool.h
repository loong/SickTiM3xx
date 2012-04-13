/*
 * ImagePool.h
 *
 *  Created on: 09.06.2010
 *      Author: locher
 */

#ifndef IMAGEPOOL_H_
#define IMAGEPOOL_H_


#include <iostream>
#include <boost/thread/mutex.hpp>

#include <opencv/cv.h>



class ImagePool {

public:
	typedef struct image_pool_img_data{
		volatile long img_count;
		IplImage* img;
	} image_pool_img_data_t;


private:

	long img_counter;

	typedef	enum{FREE,LOCKED} locke_state_t;

	boost::mutex my_mutex;



	image_pool_img_data_t img1;
	image_pool_img_data_t img2;
	image_pool_img_data_t img3;

	image_pool_img_data_t no_image;

	volatile locke_state_t lock_state_img1,lock_state_img2,lock_state_img3;

	volatile int new_write_count,last_write_count;
	volatile bool read_flag_img1,read_flag_img2,read_flag_img3;

public:


	ImagePool();
	virtual ~ImagePool();


	image_pool_img_data_t& lock_Img_to_read_img();
	void unlock_Img_from_read(IplImage* img);
	void unlock_Img_from_write(IplImage* img);

	void create_pool_images(int width, int height);

	IplImage* lock_Img_to_write_img();

	void set_pause_capture(bool pause);

};

#endif /* IMAGEPOOL_H_ */
