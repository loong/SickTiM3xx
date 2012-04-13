#include "roboOne.hpp"

/**
 * RoboOne::RoboOne
 * Constructor.  Retrieve options from the configuration file and do any
 * pre-Setup() setup.
 * All interfaces we need are created here.
 *
 * @param cf
 * @param section
 */
RoboOne::RoboOne(ConfigFile* cf, int section) : ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN),driverName("RoboOne")
{
    this->port = (char*)cf->ReadString(section, "port", "");
    this->pid_p1 = cf->ReadFloat(section, "pid_p1", 0);
    this->pid_p2 = cf->ReadFloat(section, "pid_p2", 0);
    this->pid_i1 = cf->ReadFloat(section, "pid_i1", 0);
    this->pid_i2 = cf->ReadFloat(section, "pid_i2", 0);
    this->break_amp = cf->ReadFloat(section, "break_amp", 0);

    this->baudrate = cf->ReadInt(section, "baudrate", 38400);
    this->wheel_distance = (double)cf->ReadFloat(section, "wheel_distance", 0.292);
    this->wheel_diameter = (double)cf->ReadFloat(section, "wheel_diameter", 0.0695);

		// debug ausgaben?
	m_bDebug = cf->ReadBool(section,"debug",false);


    //Creating Interfaces

    if(cf->ReadDeviceAddr(&(this->position_id), section, "provides",
        PLAYER_POSITION2D_CODE, -1, NULL) == 0)
    {
        if(this->AddInterface(this->position_id) != 0)
        {
            this->SetError(-1);
            return;
        }
    }

    odoRobot = new RoboOneMotors(this->port, this->baudrate);
}


//
/**
 * RoboOne::Setup
 * Set up the device.  Return 0 if things go well, and -1 otherwise.
 *
 * @return on success return 0
 */
int RoboOne::Setup()
{
  if(odoRobot->Connect()) // error
  {
	ERROR("Couldn't establish a connection to avr device");
	return (-1);
  }
  else
  {
	//printf();
    LOG("Odometry Eurobot connected");

  // Setzte Regelparameter falls in der Konfiguration welche
	// angegeben wurden
	if (pid_p1 != 0 || pid_p2 != 0 || pid_i1 != 0 || pid_i2 != 0)
	{
		// cout << "Setze Regelparameter\n";
		odoRobot->SetPIDparams(pid_p1,pid_p2,pid_i1,pid_i2);
		usleep(30000);
	}
	// Setze Bremskraftverstärker
	if (break_amp != 0)
	{
		odoRobot->SetBreakAmp(break_amp);
		usleep(30000);
	}
	odoRobot->SetSpeed(0, 0);
	usleep(10000);
	x = y = yaw = 0;
	ResetOdometry();
  }

  // Start the device thread; spawns a new thread and executes
  // RoboOne::Main(), which contains the main loop for the driver.
  this->StartThread();

  return 0;
}


/**
 * RoboOne::Shutdown
 * Shutdown the device
 * Stop all motors of NXT and close the Bluetooth connection.
 *
 * @return on success return 0
 */
int RoboOne::Shutdown()
{
  LOG("Shutting driver down");

  StopThread();

  //odoRobot->SetSpeed(0, 0);
  odoRobot->Disconnect();

  // Stop and join the driver thread
  // move up to prevent terrible errors
  //this->StopThread();

  LOG("Driver has been shutdown");

  return 0;
}

/**
 * RoboOne::ProcessMessages
 *
 * @param resp_queue
 * @param hdr
 * @param data
 *
 * @return on success return 0
 */
int RoboOne::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
	assert(hdr);
//	assert(data);

    	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, position_id))
	{
		player_position2d_cmd_vel_t * poscmd = reinterpret_cast<player_position2d_cmd_vel_t *> (data);

	//	LOG("Setting speed to " << poscmd->vel.px << ", rotation to " << poscmd->vel.pa);

		// need to calculate the left and right velocities
		mVelocity = poscmd->vel.px;
		mRotationSpeed = poscmd->vel.pa;

		return 0;
	}
	else if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_POS, position_id))
	{
		WARNING("PLAYER_POSITION2D_CMD_POS ==> Not implemented - use nd or vfh driver");
		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_GET_GEOM, position_id))
	{
		player_position2d_geom_t geom;

		geom.pose.px = 0;
		geom.pose.py = 0;
		geom.pose.pyaw = 0;
		geom.size.sw = 0.25;
		geom.size.sl = 0.425;
		Publish(position_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_GET_GEOM, &geom, sizeof(geom),NULL);

		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_MOTOR_POWER, position_id))
  	{
		player_position2d_power_config_t * powercfg = reinterpret_cast<player_position2d_power_config_t *> (data);

		LOG("Got motor power req: " << powercfg->state);

		//We don't need to activate motors, so we always send an ACK
		Publish(position_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_MOTOR_POWER);
		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_SET_ODOM, position_id))
  	{
  		player_position2d_set_odom_req_t* odomcfg = reinterpret_cast<player_position2d_set_odom_req_t *> (data);

		LOG("Setting odometry to x: " << odomcfg->pose.px << ", y: " << odomcfg->pose.py << ", yaw: " << odomcfg->pose.pa);
		usleep(40000);
	//	ResetOdometry();
		SetOdometry(odomcfg->pose.px, odomcfg->pose.py, odomcfg->pose.pa);
		Publish(position_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);

		return 0;
	}
	else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_RESET_ODOM, position_id))
  	{
		LOG("Reset odometry");
		usleep(40000);
		ResetOdometry();
		Publish(position_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK,hdr->subtype);
		return 0;
	}


	LOG("Other message");
	return -1;
}



/**
 * RoboOne::Main
 * Main function for device thread
 */
void RoboOne::Main()
{
    struct timeval tmpTime;
	double dDiffMilSec;
	double dHz;

	for(;;)
    {
		pthread_testcancel();

        // Process incoming messages.  RoboOne::ProcessMessage() is
        // called on each message.
        ProcessMessages();

        // Interact with the device, and push out the resulting data, using
        // Driver::Publish()
	    UpdateData();

	    if(m_bDebug)
	    {
		    m_iReps++;
		    if(m_iReps >= 10)
		    {
			    // aktuelle Zeit
			    gettimeofday(&tmpTime,0);
			    // berechne benötigte Zeit
			    dDiffMilSec = (tmpTime.tv_sec - m_tOldTime.tv_sec) * 1000 + (tmpTime.tv_usec - m_tOldTime.tv_usec) / 1000;
			    // berechne Hz
			    dHz = 1 / ((dDiffMilSec/1000) / m_iReps);
			    cout << "[htwg_xsens] Datenrate: " << dHz  << " Hz \n";
			    m_tOldTime = tmpTime;
			    m_iReps = 0;
		    }
	    }

    	//    usleep(10000); // not used cause of blocking read

		// convert xspeed and yawspeed into wheelspeeds
		double rotationalTerm = mRotationSpeed * wheel_distance / 2.0;
		double leftVel = mVelocity - rotationalTerm;
		double rightVel = mVelocity + rotationalTerm;

		int rMotorSpeed = meterPerSec2ticksPerTenMs(rightVel);
		int lMotorSpeed = meterPerSec2ticksPerTenMs(leftVel);

		odoRobot->SetSpeed(lMotorSpeed, rMotorSpeed);
    }
}


/**
 * RoboOne::UpdateData
 * Update sensor data of all connected sensors
 * Update position-data
 * The data will be transfered to player by Publish-function
 */
void RoboOne::UpdateData()
{
    // position data
    player_position2d_data_t position_data;
    UpdatePosData(&position_data);
    position_data.stall = (uint8_t)GetStall();

    struct timeval time;
	GlobalTime->GetTime(&time);
	double messageTime = time.tv_sec + time.tv_usec/1e6;

	Publish(
    	position_id,
    	PLAYER_MSGTYPE_DATA,
    	PLAYER_POSITION2D_DATA_STATE,
    	(void*)&position_data,
    	sizeof(player_position2d_data_t),
		&messageTime
    );

}


/**
 * RoboOne::ResetOdometry
 * Reset the odometry data
 *
 * @return on success return 0
 */
int RoboOne::ResetOdometry()
{
  LOG("Reset Odometry");
  odoRobot->ResetOdometry();

  last_lpos = 0;
  last_rpos = 0;

  player_position2d_data_t data;
  memset(&data,0,sizeof(player_position2d_data_t));

 // Publish(position_id, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, &data, sizeof(data),NULL);
  x=y=yaw=0;
  return 0;
}


/**
 * RoboOne::SetOdometry
 * Set odometry data
 *
 * @return on success return 0
 */
int RoboOne::SetOdometry(double xNew, double yNew, double yawNew)
{
  odoRobot->ResetOdometry();

  last_lpos = 0;
  last_rpos = 0;

  x = xNew;
  y = yNew;
  yaw = yawNew;
  return 0;
}



/**
 * RoboOne::UpdatePosData
 *
 * @param d
 */
void RoboOne::UpdatePosData(player_position2d_data_t *d)
{
  int16_t left_pos, right_pos;
	int iReturn;

	// Wait if new data is available
	iReturn = odoRobot->WaitData();
	if(iReturn)
	{
		odoRobot->Disconnect();
		puts("###### [ERROR] ##########\n");
		puts("Mutex timed out!.\n");
		puts("Device doesn't response\n");
		puts("######## [END] ##########\n");
		exit(0);
	}

	odoRobot->GetTicks(left_pos, right_pos);
//	LOG("left_ticks: " << left_pos << "  right_ticks: " << right_pos);

  // calculate travelled ticks. Note: ticks can overflow!
  long int change_left = getTickDiff(last_lpos, left_pos);
  long int change_right = getTickDiff(last_rpos, right_pos);

  last_lpos = left_pos;
  last_rpos = right_pos;

  // Calculate translational and rotational change
  // translational change = avg of both changes
  // rotational change is half the diff between both changes
  const double TWOPI = 2.0*M_PI;
  double circumference = TWOPI * (wheel_diameter / 2);
  double dist_per_tick = circumference / TICKS_PER_ROTATION;

  double transchange = (change_left + change_right) * dist_per_tick / 2;
  double rotchange = (change_left - change_right) * dist_per_tick / 2;
  // Radius of the robotwheels
  double r = wheel_distance / 2;  // radabstand / 2

  // calc total yaw, constraining from 0 to 2pi
  yaw -= rotchange/r;
  if ( yaw < 0 )
    yaw += TWOPI;

  if ( yaw > TWOPI )
    yaw -= TWOPI;

  // calc current x and y position
  x += ( transchange * cos( yaw ));
  y += ( transchange * sin( yaw ));

  d->pos.px = x;
  d->pos.py = y;
  d->pos.pa = yaw;

  int16_t left_speed, right_speed;
  odoRobot->GetSpeed(left_speed, right_speed);
  double lv = ticksPerTenMs2meterPerSec(left_speed);
  double rv = ticksPerTenMs2meterPerSec(right_speed);
  double trans_vel = (lv + rv)/2;
  double rot_vel = (rv - lv)/2;
  double rot_vel_deg = 360 * rot_vel/(2 * M_PI * r);

  d->vel.px = trans_vel;
	d->vel.py = 0;
  d->vel.pa = DTOR(rot_vel_deg);
	d->stall = 0;
}

int RoboOne::GetStall()
{
  return odoRobot->GetStall();
}

int RoboOne::getTickDiff(int16_t oldTicks, int16_t newTicks)
{
  long int diff = newTicks - oldTicks;
  if(abs(diff) > MAX_TICK_CHANGE) // check if we have an overflow
  {
  // printf("old diff: %d\n", diff);
	// positive overflow
	if(diff < 0)
	{
		diff += EUROBOT_INT_MAX;
		// printf("positive overflow\n");
	}
	else // negative overflow
	{
		diff += EUROBOT_INT_MIN;
		//printf("negative overflow\n");
	}
  //printf("new diff: %d\n", diff);
  }
  return diff;
}

int RoboOne::meterPerSec2ticksPerTenMs(double speed)
{
	double circumference = M_PI * wheel_diameter;
	double rotations10ms = (speed * 0.01) / circumference;
	double ticks = rotations10ms * TICKS_PER_ROTATION;
	return ticks;
}
double RoboOne::ticksPerTenMs2meterPerSec(int16_t speed)
{
	double circumference = M_PI * wheel_diameter;
	double metersPerTick = circumference / TICKS_PER_ROTATION;
	double metersPerSecond = speed * metersPerTick;
	return metersPerSecond / 0.01;
}





Driver* RoboOne_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new RoboOne(cf, section)));
}


void RoboOne_Register(DriverTable* table)
{
  char driverName[] = "RoboOne";
  table->AddDriver(driverName, RoboOne_Init);
}



extern "C" {
  int player_driver_init(DriverTable* table)
  {
    RoboOne_Register(table);
    return(0);
  }
}

