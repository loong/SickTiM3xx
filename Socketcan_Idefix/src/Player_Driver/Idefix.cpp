/*
 * multi.cpp
 *
 *  Created on: 08.04.2010
 *      Author: adrian
 */

#include "Idefix.hpp"

using namespace idefix;

// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* MultiDriver_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return ((Driver*) (new Idefix(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void MultiDriver_Register(DriverTable* table)
{
  table->AddDriver("Idefix", MultiDriver_Init);
}


////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C"
{
  int player_driver_init(DriverTable* table)
  {
    MultiDriver_Register(table);
    return(0);
  }
}


////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
Idefix::Idefix(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section),
      gamepad_controll_is_on(true),
      drives_are_enabled(true),
      pressed_gamepad_button_mask(0),
      debug(false)
{

	can_interface_name = cf->ReadString(section, "can_interface_name", CAN_INTERFACE_NAME);

	my_drives.setCanInterfaceName(can_interface_name);
	aim.setCanInterfaceName(can_interface_name);

	gamepad_max_speed_m_s=cf->ReadFloat(section,"gamepad_max_speed_m_s",0.1);
	main_loop_sleep_time_us=cf->ReadFloat(section,"main_loop_sleep_time_us",50000);


	// Sonar geometry.
	sonar0[0] = cf->ReadTupleLength(section, "sonar0", 0, -0.32);
	sonar0[1] = cf->ReadTupleLength(section, "sonar0", 1, -0.1);
	sonar0[2] = cf->ReadTupleLength(section, "sonar0", 2, 135);

	sonar1[0] = cf->ReadTupleLength(section, "sonar1", 0, 0.32);
	sonar1[1] = cf->ReadTupleLength(section, "sonar1", 1, -0.1);
	sonar1[2] = cf->ReadTupleLength(section, "sonar1", 2, 45);

	sonar2[0] = cf->ReadTupleLength(section, "sonar2", 0, 0.32);
	sonar2[1] = cf->ReadTupleLength(section, "sonar2", 1, 0.1);
	sonar2[2] = cf->ReadTupleLength(section, "sonar2", 2, 315);

	sonar3[0] = cf->ReadTupleLength(section, "sonar3", 0, -0.32);
	sonar3[1] = cf->ReadTupleLength(section, "sonar3", 1, 0.1);
	sonar3[2] = cf->ReadTupleLength(section, "sonar3", 2, 225);

	sonarArr[0]=sonar0;
	sonarArr[1]=sonar1;
	sonarArr[2]=sonar2;
	sonarArr[3]=sonar3;

	std::cout << "Setting up The Drives" <<std::endl;
	my_drives.config_Drives();

	std::cout << "Setting up Ultrasonic" <<std::endl;
	aim.config();

  // Create my position interface
  if (cf->ReadDeviceAddr(&(this->m_position_addr), section,
                         "provides", PLAYER_POSITION2D_CODE, -1, NULL) != 0)
  {
    this->SetError(-1);
    return;
  }
  if (this->AddInterface(this->m_position_addr))
  {
    this->SetError(-1);
    return;
  }



  // Create my sonar interface
  if (cf->ReadDeviceAddr(&(this->m_sonar_addr), section,
                         "provides", PLAYER_SONAR_CODE, -1, NULL) != 0)
  {
    this->SetError(-1);
    return;
  }

  if (this->AddInterface(this->m_sonar_addr))
  {
    this->SetError(-1);
    return;
  }



  // Look for Joystik Interface
  if (cf->ReadDeviceAddr(&(this->m_joystick_addr), section,
                         "requires", PLAYER_JOYSTICK_CODE, -1, NULL) == 0)
  {

	cout << "Joystickaddr:" << m_joystick_addr.robot << endl;

  }else{

	  cout << "No Joystickaddr:"  << endl;
	  this->SetError(-1);

	  memset(&(this->m_joystick_addr), 0, sizeof(player_devaddr_t));
     return;
 }

  // Look for Opaque Interface
  if (cf->ReadDeviceAddr(&(this->m_opaque_addr), section,
                         "provides", PLAYER_OPAQUE_CODE, -1, NULL) != 0)
  {
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

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int Idefix::MainSetup()
{
  puts("Example driver initialising");

  // Here you do whatever else is necessary to setup the device, like open and
  // configure a serial port.

  // Joypad abonnieren

  this->joypad_device_ptr = NULL;
  //   If we're asked, open the position2d device
  if (this->m_joystick_addr.interf)
  {
    if (!(this->joypad_device_ptr = deviceTable->GetDevice(this->m_joystick_addr)))
    {
      PLAYER_ERROR("unable to locate suitable joystick device");
      return -1;
    }
    if (this->joypad_device_ptr->Subscribe(this->InQueue) != 0)
    {
      PLAYER_ERROR("unable to subscribe to joypaddevice");
      return -1;
    }
  }




  puts("Example driver ready");
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void Idefix::MainQuit()
{
  puts("Shutting example driver down");


  my_drives.disable_drives();

  // Here you would shut the device down by, for example, closing a
  // serial port.
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Idefix::Main()
{


  // The main loop; interact with the device here
  for(;;)
  {
    // test if we are supposed to cancel
    pthread_testcancel();



    // Process incoming messages.  Calls ProcessMessage() on each pending
     // message.
     this->ProcessMessages();


    // Send out new messages with Driver::Publish()
    //
    PublishPosition();
    PublishSonar();
    PublishOpaque();


    // Sleep (or you might, for example, block on a read() instead)
   usleep(main_loop_sleep_time_us);
  }
  return;
}


int Idefix::ProcessMessage(QueuePointer & resp_queue,
                                player_msghdr * hdr,
                                void * data)
{
	assert(hdr);


	if( handle_position2d_Interface_messages(resp_queue,hdr,data)==0){

		return 0;

	}

	else if(handle_sonar_Interface_messages(resp_queue,hdr,data)==0) {

		return 0;

	}
	else if(handle_joystick_Interface_messages(resp_queue,hdr,data)==0) {

		return 0;

	}
	else if(handle_opaque_0_Interface_messages(resp_queue,hdr,data)==0){

		return 0;

	}




	puts("other message");

	  // Tell the caller that you don't know how to handle this message
	  return(-1);
}



int  Idefix::handle_position2d_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data){

	//position geometry
	if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_GET_GEOM, m_position_addr))
	{
		player_position2d_geom_t geom;

		geom.pose.px = 0;
		geom.pose.py = 0;
		geom.pose.pyaw = 0;
		geom.size.sw = 0.4;
		geom.size.sl = 0.6;
		Publish(m_position_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_GET_GEOM, &geom, sizeof(geom),NULL);

		return 0;
	}

	//position car command
	else if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_CAR, m_position_addr))
	{
				cout << "PLAYER_MSGTYPE_CMD_CAR" << endl;

				player_position2d_cmd_car_t*  car_cmd= reinterpret_cast<player_position2d_cmd_car_t *> (data);
				my_drives.set_angle_front(car_cmd->angle);
				my_drives.set_angle_rear(-car_cmd->angle);

				my_drives.set_velocity_ms(car_cmd->velocity);
				cout<<"Setting speed to " << car_cmd->velocity<< ", angle to " << car_cmd->angle << endl;
		return 0;
	}

	//position vel command
	else if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, m_position_addr))
	{
				cout << "PLAYER_MSGTYPE_CMD_VEL" << endl;
				player_position2d_cmd_vel_t * poscmd = reinterpret_cast<player_position2d_cmd_vel_t *> (data);
				cout<<"Setting speed to " << poscmd->vel.px << ", rotation to " << poscmd->vel.pa<<endl;
				my_drives.set_angle_front(poscmd->vel.pa);
				my_drives.set_angle_rear(-poscmd->vel.pa);

				my_drives.set_velocity_ms(poscmd->vel.px);
		return 0;
	}

	//position pos command
	else if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_POS, m_position_addr))
	{
		//cout << "PLAYER_POSITION2D_CMD_POS ==> Not implemented - use nd or vfh driver" << endl;
		return 0;
	}


	//andere messages
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_MOTOR_POWER, m_position_addr))
	{
		//		player_position2d_power_config_t * powercfg = reinterpret_cast<player_position2d_power_config_t *> (data);
		//
		//		cout << "Got motor power req: " << powercfg->state << endl;
		//
		//		//We don't need to activate motors, so we always send an ACK
		//		Publish(m_position_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_MOTOR_POWER);
		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_SET_ODOM, m_position_addr))
	{
		//		player_position2d_set_odom_req_t* odomcfg = reinterpret_cast<player_position2d_set_odom_req_t *> (data);
		//
		//		cout <<"Setting odometry to x: "
		//				<< odomcfg->pose.px << ", y: " << odomcfg->pose.py
		//				<< ", yaw: " << odomcfg->pose.pa  << endl;
		//
		//		Publish(m_position_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);

		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_RESET_ODOM, m_position_addr))
	{
	//		cout << "Reset odometry" << endl;
	//		Publish(m_position_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);
		return 0;
	}


	return -1;




}
int  Idefix::handle_sonar_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data){

	//sonar geometry
	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,PLAYER_SONAR_REQ_GET_GEOM,m_sonar_addr)){

		puts("PLAYER_SONAR_REQ_GET_GEOM received");

		player_sonar_geom_t geom;
		memset(&geom,0,sizeof(geom));

		geom.poses_count = 4;
		geom.poses = new player_pose3d_t[geom.poses_count];
		//memset(geom.poses, 0, sizeof(player_pose3d_t) * geom.poses_count);

		if(debug) puts("nach set poses count");

		for(int i=0; i<4; i++){
			geom.poses[i].px=sonarArr[i][0];
			geom.poses[i].py=sonarArr[i][1];
			geom.poses[i].pyaw=DTOR(sonarArr[i][2]);

			geom.poses[i].proll=0;
			geom.poses[i].ppitch=0;
			geom.poses[i].pz=0.1;
		}

		if(debug) puts("vor publish");
		Publish(m_sonar_addr, PLAYER_MSGTYPE_RESP_ACK,PLAYER_SONAR_REQ_GET_GEOM,(void *) &geom);
		if(debug) puts("nach publish");
		return(0);
	}

	return -1;

}
int  Idefix::handle_joystick_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data){

	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_DATA,PLAYER_JOYSTICK_DATA_STATE, m_joystick_addr)){


		player_joystick_data_t * joy_data= reinterpret_cast<player_joystick_data_t *> (data);

//		cout << "\tJoystick  Buttons: "<< joy_data->buttons <<  endl;
//		for (uint_t i = 0; i < joy_data->axes_count; ++i) {
//			cout << "\t\tJoystick  "<< joy_data->pos[i] << " "<< joy_data->scale[i]<<   endl;
//
//		}

		//Gamepad BUTTONS IN GAMEPAD_MODE
		if(gamepad_controll_is_on){

			// Homing nur im Gamepad Mode ermöglichen
			if( (joy_data->buttons & I_C::START_HOMING_MODE_BUTTON_FRONT) >0 ){

					cout<< "Homing Front started" << std::endl;
					my_drives.start_homing(idefix::MOUNT_POS_FRONT);
					cout<< "Homing Front stopped" << std::endl;


			}

			if( (joy_data->buttons & I_C::START_HOMING_MODE_BUTTON_REAR) >0 ){

					cout<< "Homing rear started" << std::endl;
					my_drives.start_homing(idefix::MOUNT_POS_REAR);
					cout<< "Homing  rearstopped" << std::endl;


			}
		}

		// Button Mask speichern zur späteren übergabe an die opaque data struct
		pressed_gamepad_button_mask= ( joy_data->buttons & ALL_CLIENT_BUTTONS_MASK  );





		if( (joy_data->buttons & I_C::SWITCH_TO_AUTO_MODE_BUTTON)  !=0 ){
			if(gamepad_controll_is_on){

				gamepad_controll_is_on=false;

				cout<< "Gamepad Disabled" << std::endl;
			}
		}

		// Falls beide Buttons gedrückt sind gewinnt Gamepad
		if( (joy_data->buttons & I_C::SWITCH_TO_GAMEPAD_MODE_BUTTON ) !=0 ){
			if(!gamepad_controll_is_on){

				gamepad_controll_is_on=true;
				cout<< "Gamepad Enabled" << std::endl;

			}

		}



		if( (joy_data->buttons & I_C::SWITCH_TO_REENABLE_DRIVES) >0 ){

			if(!drives_are_enabled ){
				drives_are_enabled=true;
				my_drives.reenable_drives();
				cout<< "Drives are enabled" << std::endl;
			}
		}
		if( (joy_data->buttons & I_C::SWITCH_TO_DISABLE_DRIVES ) >0 ){

			if(drives_are_enabled ){
				drives_are_enabled=false;

				my_drives.disable_drives();
				cout<< "Drives are disabled" << std::endl;
			}

		}


		int32_t scale=32768;

		if(gamepad_controll_is_on){

			my_drives.set_velocity_ms(-((double)joy_data->pos[0]/(double)scale)*gamepad_max_speed_m_s);
			my_drives.set_angle_front((double)joy_data->pos[2]/scale);
			my_drives.set_angle_rear((double)joy_data->pos[1]/scale);

		}




		//Publish(m_joystick_addr, resp_queue, PLAYER_MSGTYPE_DATA,hdr->subtype);

		return 0;

	}




	return -1;




}
int  Idefix::handle_opaque_0_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data){

	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_CMD,PLAYER_OPAQUE_CMD_DATA, m_opaque_addr)){

			if(!gamepad_controll_is_on){

				idefix::opaque_cmd_t* opaque_data= reinterpret_cast<idefix::opaque_cmd_t* > (
						reinterpret_cast< player_opaque_data_t *> (data)->data);


				my_drives.set_angle_front(opaque_data->new_angle_front);
				my_drives.set_angle_rear(opaque_data->new_angle_rear);

				if(opaque_data->reset_distance){
					std::cout << " Idefix Driver Distance Reset" << std::endl;
					my_drives.reset_distance_measuring();
				}

				if(opaque_data->drive_profile==PROFILE_POSITION){

					my_drives.set_move_distance_m(opaque_data->move_distance_m);
				}
				if(opaque_data->drive_profile==PROFILE_VELOCITY){

					my_drives.set_velocity_ms(opaque_data->new_velocity);

				}

			}

			return 0;

		}


	return -1;


	//	else if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,PLAYER_OPAQUE_CMD_DATA, m_opaque_addr)){
	//
	//
	//			idefix::I_C::opaque_drive_data_t* opaque_data= reinterpret_cast<idefix::I_C::opaque_drive_data_t* > (
	//					reinterpret_cast< player_opaque_data_t *> (data)->data);
	//
	//			std::cout << "Idefix:: Opaque Data Request " << std::endl;
	//
	////			my_drives.set_distance_data(*opaque_data);
	////
	////			my_drives.set_angle_front(opaque_data->new_angle_front);
	////			my_drives.set_angle_rear(opaque_data->new_angle_rear);
	////			my_drives.set_velocity_ms(opaque_data->new_velocity);
	////
	//			Publish(m_opaque_addr, PLAYER_MSGTYPE_, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);
	//
	//
	//
	//
	//		return 0;
	//
	//	}




}




void Idefix::PublishSonar(){

	player_sonar_data data;
	memset(&data,0,sizeof(data)); //initialisiert mit 0
	data.ranges_count=4;

	float my_ranges[data.ranges_count];

	aim.get_ranges(my_ranges); //my_ranges wird befüllt;
	data.ranges = my_ranges;

	if(debug) puts("vor ranges publish");
	this->Publish(this->m_sonar_addr, PLAYER_MSGTYPE_DATA, PLAYER_SONAR_DATA_RANGES, (void *) &data);
	if(debug) puts("nach ranges publish");

}

void Idefix::PublishPosition(){
    // For example, to send a new position pose message:
    player_position2d_data_t posdata;
    posdata.pos.px = 43.2;
    posdata.pos.py = -12.2;
    posdata.pos.pa = M_PI/3.0;
    posdata.vel.px = 0.25;
    posdata.vel.py = 0.0;
    posdata.vel.pa = -M_PI/6.0;
    posdata.stall = 0;

    this->Publish(this->m_position_addr,
                  PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE,
                  (void*)&posdata, sizeof(posdata), NULL);
}


void Idefix::PublishOpaque(){

	idefix::opaque_data_t opaque_data;
	// Drive Daten abholen
	my_drives.get_drive_data(opaque_data);

	// um Button Mask erweitern
	opaque_data.pressed_gamepad_buttons_mask=pressed_gamepad_button_mask;
	opaque_data.gamepad_mode_active=gamepad_controll_is_on;




//	if(gamepad_controll_is_on){
//		// Damit sollen endlos S_cleifen, die auf das Erreichen der Wegestrecke
//		// warten verhinder werden, wenn das Gamepad übernimmt
//		opaque_data.distance_reached=true;
//	}

	player_opaque_data_t data;
	data.data_count=sizeof(opaque_data);
	data.data=reinterpret_cast<uint8_t*>(&opaque_data);


	Publish(m_opaque_addr, PLAYER_MSGTYPE_DATA,PLAYER_OPAQUE_DATA_STATE,(void*)&data,sizeof(opaque_data),NULL);


}
