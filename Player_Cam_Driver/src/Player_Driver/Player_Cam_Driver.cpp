/*
 * multi.cpp
 *
 *  Created on: 08.04.2010
 *      Author: adrian
 */

#include "Player_Cam_Driver.hpp"




// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* Player_Cam_Driver_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return ((Driver*) (new Player_Cam_Driver(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void Player_Cam_Driver_Register(DriverTable* table)
{
  table->AddDriver("Player_Cam_Driver", Player_Cam_Driver_Init);
}


////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C"
{
  int player_driver_init(DriverTable* table)
  {
    Player_Cam_Driver_Register(table);
    return(0);
  }
}


////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
Player_Cam_Driver::Player_Cam_Driver(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section),
      debug(false)
{


	memset(&(this->m_opaque_addr),0,sizeof( player_devaddr_t ));

	main_loop_sleep_time_us=cf->ReadFloat(section,"main_loop_sleep_time_us",5000);

	 // Look for size of opaque Data
	  data_size=cf->ReadInt(section,"opaque_data_size",8);
	  std::cout << "Check if sizeof opaque data struct in config file has the right value " << std::endl;
	  std::cout << "Opaque Data Size is now: "<< data_size << std::endl;

	  named_pipe_name=cf->ReadString(section,"named_pipe","/tmp/cam_pipe");


  // Look for Opaque Interface
  if (cf->ReadDeviceAddr(&(this->m_opaque_addr), section,
                         "provides", PLAYER_OPAQUE_CODE, -1, NULL) != 0)
  {
	PLAYER_ERROR("NO OPAQUE ADDR PROVIDED");
    this->SetError(-1);
    return;
  }

  if (this->AddInterface(this->m_opaque_addr))
  {
    this->SetError(-1);
    return;
  }



  cout << "Subscribed" << endl;



}

Player_Cam_Driver::~Player_Cam_Driver(){



}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int Player_Cam_Driver::MainSetup()
{
  puts("Player_Cam_Driver initialising");

  // Here you do whatever else is necessary to setup the device, like open and
  // configure a serial port.


	// Look for named pipe

	if(access(named_pipe_name.c_str(), F_OK)==-1){

		res=mkfifo(named_pipe_name.c_str(),0777);
		if(res!=0){
			fprintf(stderr, "Could not create Named-Pipe %s\n", named_pipe_name.c_str());
			exit(EXIT_FAILURE);
		}

	}
  puts("Player_Cam_Driver initialising 2");

	named_pipe_fd=open(named_pipe_name.c_str(),O_RDONLY );

	if(named_pipe_fd==-1){

		puts("Could not open named pipe");
		return -1;

	}
  puts("Player_Cam_Driver initialising 3");





  puts("Example driver ready");
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void Player_Cam_Driver::MainQuit()
{
  puts("Shutting example driver down");


  std::cout << "Player_Cam_Driver: cloasing named pipe: fd: " << named_pipe_fd << std::endl;

  close(named_pipe_fd);

  // Here you would shut the device down by, for example, closing a
  // serial port.
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Player_Cam_Driver::Main()
{



//	std::cout << "Player_cam_driver Main" << std::endl;

  // The main loop; interact with the device here
  for(;;)
  {
    // test if we are supposed to cancel
    pthread_testcancel();

//	std::cout << "Player_cam_driver Main Loop" << std::endl;


    // Process incoming messages.  Calls ProcessMessage() on each pending
     // message.
     this->ProcessMessages();




    // Send out new messages with Driver::Publish()
    //
    PublishOpaque();


    // Sleep (or you might, for example, block on a read() instead)
   usleep(main_loop_sleep_time_us);
  }
  return;
}


int Player_Cam_Driver::ProcessMessage(QueuePointer & resp_queue,
                                player_msghdr * hdr,
                                void * data)
{
	assert(hdr);


	if(handle_opaque_0_Interface_messages(resp_queue,hdr,data)==0){

		return 0;

	}



	puts("other message");

  // Tell the caller that you don't know how to handle this message
  return(-1);
}



int  Player_Cam_Driver::handle_opaque_0_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data){

	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_CMD,PLAYER_OPAQUE_CMD_DATA, this->m_opaque_addr)){


		return 0;


	}




		else if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,PLAYER_OPAQUE_CMD_DATA, m_opaque_addr)){


//				idefix::I_C::opaque_drive_data_t* opaque_data= reinterpret_cast<idefix::I_C::opaque_drive_data_t* > (
//						reinterpret_cast< player_opaque_data_t *> (data)->data);

				std::cout << "PlayerCam Driver :: Opaque Data Request not handled " << std::endl;

	//			my_drives.set_distance_data(*opaque_data);
	//
	//			my_drives.set_angle_front(opaque_data->new_angle_front);
	//			my_drives.set_angle_rear(opaque_data->new_angle_rear);
	//			my_drives.set_velocity_ms(opaque_data->new_velocity);
	//
	//				Publish(m_opaque_addr, PLAYER_MSGTYPE_, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);




			return 0;

		}


	return -1;


}






void Player_Cam_Driver::PublishOpaque(){


	bytes_read=read(named_pipe_fd,buffer,data_size);
	if(bytes_read==data_size){

		player_opaque_data_t data;
		data.data_count=bytes_read;
		data.data=reinterpret_cast<uint8_t*>(buffer);

//		std::cout << "Player_Cam_Driver: Publish Opaque Data" << bytes_read << " " << std::endl;

//		char test[50];
//		strcpy(test,buffer);
//		std::cout << "Player_Cam_Driver:" << test << std::endl;

	//	data.data=reinterpret_cast<uint8_t*>(buffer);

		Publish(m_opaque_addr, PLAYER_MSGTYPE_DATA,PLAYER_OPAQUE_DATA_STATE,(void*)&data,sizeof(buffer),NULL);
	}else{

		std::cerr << "Player_Cam_Driver: Read wrong data size:" << bytes_read << std::endl;

	}

//	std::cout  << " Publish Opaque  " << std::endl;

}



