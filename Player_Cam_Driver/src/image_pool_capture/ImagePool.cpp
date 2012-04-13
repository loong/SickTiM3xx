/*
 * ImagePool.cpp
 *
 *  Created on: 09.06.2010
 *      Author: locher
 */

#include "ImagePool.h"




ImagePool::ImagePool():
	img_counter(0),

	new_write_count(1),
	last_write_count(1),
	read_flag_img1(0),
	read_flag_img2(0),
	read_flag_img3(0)


{

	img1.img_count=0;
	img1.img=0;

	img2.img_count=0;
	img2.img=0;

	img3.img_count=0;
	img3.img=0;

	no_image.img_count=0;
	no_image.img=0;


	lock_state_img1=FREE;
	lock_state_img2=FREE;
	lock_state_img3=FREE;



}


void ImagePool::create_pool_images(int width, int height){



	img1.img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
	img2.img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);
	img3.img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,3);


}

ImagePool::~ImagePool() {

	if(img1.img!=0){
		cvReleaseImage(&img1.img);
	}
	if(img2.img!=0){
		cvReleaseImage(&img2.img);
	}
	if(img3.img!=0){
		cvReleaseImage(&img3.img);
	}

}


ImagePool::image_pool_img_data_t& ImagePool::lock_Img_to_read_img(){

	boost::mutex::scoped_lock sl(my_mutex);
	if(lock_state_img1==FREE){

		if( (img1.img_count==last_write_count) && !read_flag_img1 ){

//			std::cout << "RL1" << std::endl;
			lock_state_img1=LOCKED;
			read_flag_img1=true;
			return img1;
		}
	}

	if(lock_state_img2==FREE){

		if( (img2.img_count==last_write_count) && !read_flag_img2){

//			std::cout << "RL2" << std::endl;
			lock_state_img2=LOCKED;
			read_flag_img2=true;
			return img2;
		}
	}

	if(lock_state_img3==FREE){

		if( (img3.img_count==last_write_count) && !read_flag_img3 ){
//			std::cout << "RL3" << std::endl;
			lock_state_img3=LOCKED;
			read_flag_img3=true;
			return img3;
		}
	}

//	std::cout << "No Read Image to return " << std::endl;
	return no_image;
}


IplImage* ImagePool::lock_Img_to_write_img(){

	boost::mutex::scoped_lock sl(my_mutex);
	if(lock_state_img1==FREE){

		if(img1.img_count!=last_write_count){

//			std::cout << "WL1" << std::endl;
			lock_state_img1=LOCKED;


			return img1.img;
		}
	}
	if(lock_state_img2==FREE){

		if(img2.img_count!=last_write_count){
//			std::cout << "WL2" << std::endl;

			lock_state_img2=LOCKED;
			return img2.img;
		}
	}
	if(lock_state_img3==FREE){

		if(img3.img_count!=last_write_count){

//			std::cout << "WL3" << std::endl;
			lock_state_img3=LOCKED;
			return img3.img;
		}
	}

//	std::cout << "No Write Image to return " << std::endl;

	return 0;
}

void ImagePool::unlock_Img_from_read(IplImage* img){

	boost::mutex::scoped_lock sl(my_mutex);
	if(img==img1.img){
		lock_state_img1=FREE;
	}
	if(img==img2.img){
		lock_state_img2=FREE;
	}
	if(img==img3.img){
		lock_state_img3=FREE;
	}

}

void ImagePool::unlock_Img_from_write(IplImage* img){

	boost::mutex::scoped_lock sl(my_mutex);
	if(img1.img==img){
		img1.img_count=new_write_count;
		read_flag_img1=false;
		lock_state_img1=FREE;
	}

	if(img2.img==img){
		img2.img_count=new_write_count;
		read_flag_img2=false;
		lock_state_img2=FREE;
	}

	if(img3.img==img){
		img3.img_count=new_write_count;
		read_flag_img3=false;
		lock_state_img3=FREE;
	}


	last_write_count=new_write_count;
	new_write_count++;

}


