#include "aim04.hpp"

// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* Aim04driver_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return ((Driver*) (new Aim04driver(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void Aim04driver_Register(DriverTable* table)
{
  table->AddDriver("aim04", Aim04driver_Init);
}


////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C"
{
  int player_driver_init(DriverTable* table)
  {
    Aim04driver_Register(table);
    return(0);
  }
}


////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
Aim04driver::Aim04driver(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN, PLAYER_SONAR_CODE),
      debug(false)
{
	// Read of type c-string/char buffer
	port = cf->ReadString(section, "port", "vcan0");

}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int Aim04driver::MainSetup()
{   
  puts("Aim04 driver initialising");

  /* Create the socket */
  skt = socket( PF_CAN, SOCK_RAW, CAN_RAW );

  /* Locate the interface you wish to use */
  struct ifreq ifr;
  strcpy(ifr.ifr_name, port.c_str());
  ioctl(skt, SIOCGIFINDEX, &ifr); /* ifr.ifr_ifindex gets filled
                                 * with that device's index */

  /* Select that CAN interface, and bind the socket to it. */
  struct sockaddr_can addr;
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  bind( skt, (struct sockaddr*)&addr, sizeof(addr) );

  /* Send start message to the CAN bus */
  frame.can_id = 0x00;
  frame.data[0]=0x01;
  frame.data[1]=0x00;
  frame.can_dlc = 2;
  int bytes_sent = write( skt, &frame, sizeof(frame) );

  // Here you do whatever else is necessary to setup the device, like open and
  // configure a serial port.
    
  puts("Aim04 driver ready");
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void Aim04driver::MainQuit()
{
  puts("Shutting Aim04 driver down");

  frame.can_id = 0x00;
  frame.data[0]=0x02;
  frame.data[1]=0x00;
  frame.can_dlc = 2;
  int bytes_sent = write( skt, &frame, sizeof(frame) );

  // Here you would shut the device down by, for example, closing a
  // serial port.
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Aim04driver::Main()
{
  // The main loop; interact with the device here
  for(;;)
  {
	  std::cout<<"mainloop"<<std::endl;
    // test if we are supposed to cancel
    pthread_testcancel();

    // Process incoming messages.  Calls ProcessMessage() on each pending
    // message.
    this->ProcessMessages();

    // Do work here.  
    //
    // Send out new messages with Driver::Publish()
    //
    // For example, to send a new position pose message:

    unsigned int vals[4]={0.9,0,0,0};

    /* Read a message back from the CAN bus */
    int bytes_read = read( skt, &frame, sizeof(frame) );


    if(debug){
    std::cout<<std::hex<<"id: "<<frame.can_id<<" dlc: "<<(int)frame.can_dlc<<" data: ";
    for(int i=0; i<(int)frame.can_dlc; i++){
 	   std::cout<<(int) frame.data[i]<<" ";
    }
    std::cout<<std::endl;
    }

    if(frame.can_id==canID){
 		vals[0]=(unsigned int)(frame.data[0] | frame.data[1] <<8);
 		vals[1]=(unsigned int)(frame.data[2] | frame.data[3] <<8);
 		vals[2]=(unsigned int)(frame.data[4] | frame.data[5] <<8);
 		vals[3]=(unsigned int)(frame.data[6] | frame.data[7] <<8);

 		if(debug){
 		std::cout<<"sensorwerte: ";
 		for(int i=0; i<4; i++){
 			std::cout<<dec<<(vals[i]+3746)/369.7<<" ";
 		}
 		std::cout<<std::endl;
 		}
    }

	player_sonar_data data;
	memset(&data,0,sizeof(data));

	data.ranges_count=4;

	data.ranges = new float[data.ranges_count];
//	memset(data.ranges, 0, sizeof(float) * data.ranges_count);

	if(debug) puts("nach set ranges count");

	float *rangesIt=data.ranges;
	for(int i=0;i<4;++i, ++rangesIt){
		*rangesIt=tometers(vals[i]);
	}

	if(debug) puts("vor ranges publish");
    Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_SONAR_DATA_RANGES, (void *) &data);
    if(debug) puts("nach ranges publish");
    //    delete[] data.ranges;

//    player_position2d_data_t posdata;
//    posdata.pos.px = (vals[0]+3746)/369.7;
//    posdata.pos.py = (vals[1]+3746)/369.7;
//    posdata.pos.pa = M_PI/3.0;
//    posdata.vel.px = (vals[2]+3746)/369.7;
//    posdata.vel.py = (vals[3]+3746)/369.7;
//    posdata.vel.pa = -M_PI/6.0;
//    posdata.stall = 0;
//
//    this->Publish(device_addr,
//                  PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE,
//                  (void*)&posdata, sizeof(posdata), NULL);

    
    // Sleep (or you might, for example, block on a read() instead)
    usleep(100000);
  }
  //return;
}


int Aim04driver::ProcessMessage(QueuePointer & resp_queue,
                                player_msghdr * hdr, 
                                void * data)
{
	assert(hdr);

	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,PLAYER_SONAR_REQ_GET_GEOM,device_addr)){
		puts("PLAYER_SONAR_REQ_GET_GEOM received");

		player_sonar_geom_t geom;
		memset(&geom,0,sizeof(geom));

		geom.poses_count = 4;

		geom.poses = new player_pose3d_t[geom.poses_count];
		//memset(geom.poses, 0, sizeof(player_pose3d_t) * geom.poses_count);

		if(debug) puts("nach set poses count");

		for(int i=0; i<geom.poses_count;++i){
		geom.poses[i].pz=0.1;
		geom.poses[i].px=(i+1)*0.2;
		geom.poses[i].py=(i+1)*0.2;
		geom.poses[i].proll=0;
		geom.poses[i].ppitch=0;
		geom.poses[i].pyaw=DTOR(360/(i+1));
		}
		geom.poses[2].pyaw=DTOR(270);

		if(debug) puts("vor publish");
		Publish(device_addr, PLAYER_MSGTYPE_RESP_ACK,PLAYER_SONAR_REQ_GET_GEOM,(void *) &geom);
		if(debug) puts("nach publish");

		return(0);

	}
  // Tell the caller that you don't know how to handle this message
  return(-1);
}

//Werteumrechnungsfaktor
float Aim04driver::tometers(unsigned int val){
	return (val+3746)/36970.0;
}
