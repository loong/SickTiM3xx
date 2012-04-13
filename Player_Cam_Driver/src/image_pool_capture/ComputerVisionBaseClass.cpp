/*
 * ComputerVisionClass.cpp
 *
 *  Created on: 28.05.2011
 *      Author: locher
 */

#include "ComputerVisionBaseClass.h"

ComputerVisionBaseClass::ComputerVisionBaseClass(std::string named_pipe_name, int cam_index):
	cap_img_width(640),
	cap_img_height(480),
	named_pipe_name(named_pipe_name ),
	cam_index(cam_index),
	cam_capture_thread(0)

{


	if(access(named_pipe_name.c_str(), F_OK)==-1){

		res=mkfifo(named_pipe_name.c_str() ,0777);
		if(res!=0){
			fprintf(stderr, "Could not create Named-Pipe %s\n", named_pipe_name.c_str());
			exit(EXIT_FAILURE);
		}

	}


//	named_pipe_fd=open(NAMED_PIPE_NAME,O_WRONLY | O_NONBLOCK );
	named_pipe_fd=open(named_pipe_name.c_str(),O_WRONLY  );

	if(named_pipe_fd==-1){

		fprintf(stderr, "Could not open Named-Pipe %s\n", named_pipe_name.c_str() );
		exit(EXIT_FAILURE);

	}






	image_pool.create_pool_images(cap_img_width,cap_img_height);

	cam_capture_thread=new CV_Cam_Capture_Thread(cam_index,image_pool,cap_img_width,cap_img_height);

	cam_capture_thread->set_pause_capture(false);
	cam_capture_thread->setSleepTime(20000);
	cam_capture_thread->start_the_thread();



}

ComputerVisionBaseClass::~ComputerVisionBaseClass() {

	if(cam_capture_thread!=0){
		cam_capture_thread->stop_the_thread();
		delete cam_capture_thread;

	}
	close(named_pipe_fd);


}


void ComputerVisionBaseClass::process(){

//	std::cout << "ComputerVisionClass process" << std::endl;

	boost::mutex::scoped_lock   sl(my_ip_mutex);


	ImagePool::image_pool_img_data_t img_data=image_pool.lock_Img_to_read_img();

	if(img_data.img!=0){

		process_image(img_data.img);


	}else{
		static int counter=0;
		if(++counter>100){
			counter=0;
			std::cout << std::endl;
		}
		std::cout << ".";

	}


	image_pool.unlock_Img_from_read(img_data.img);




}




size_t ComputerVisionBaseClass::send_opaque_data_to_player_cam_driver( void* data, size_t  data_length){


	if(data_length>PIPE_BUF){

		std::cerr << "ComputerVisionClass: Error: data_lenght of opaque data is greater than PIPE_BUF" << std::endl;
		exit(EXIT_FAILURE);

	}

	bytes_written=write(named_pipe_fd,data, data_length);

//	std::cout << "ComputerVisionClass: bytes written: "<< bytes_written << std::endl;

	return bytes_written;

}


int ComputerVisionBaseClass::get_cap_img_width(){

	return cap_img_width;
}

int ComputerVisionBaseClass::get_cap_img_hight(){

	return cap_img_height;
}

