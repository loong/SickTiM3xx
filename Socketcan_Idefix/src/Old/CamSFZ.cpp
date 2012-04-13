/*
 * CamSFZ.cpp
 *
 *  Created on: 24.09.2010
 *      Author: asterix
 */

#include "CamSFZ.hpp"

Driver* CamSFZ_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*) (new CamSFZ(cf, section)));
}

void CamSFZ_Register(DriverTable* table)
{
  table->AddDriver("camsfz", CamSFZ_Init);
}

CamSFZ::CamSFZ(ConfigFile* cf, int section)
	: ThreadedDriver(cf, section, true, 1,
			PLAYER_CAMERA_CODE)
{
	  // Read an option from the configuration file
	  device = cf->ReadInt(section, "dev", 0);
	  return;
}

int CamSFZ::MainSetup()
{
  puts("Example driver initialising");

  // Versuche Device zu oeffnen
  capture = cvCaptureFromCAM(device);
  if (!capture)
  {
    PLAYER_ERROR1("Device", device);
    printf("Was device option given in config file? %d\n", device);
    return -1;
  }


  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.

  puts("Example driver ready");

  return(0);
}

void CamSFZ::MainQuit()
{
  puts("Shutting example driver down");

  // Here you would shut the device down by, for example, closing a
  // serial port.
  // Kamera freigeben
  cvReleaseCapture(&capture);

  puts("Example driver has been shutdown");
}


int CamSFZ::ProcessMessage(QueuePointer &resp_queue,
							player_msghdr* hdr,
							void* data)
{
	return 0;
}


void CamSFZ::Main()
{
//	struct timeval time;
//	struct timespec tspec;
//	sleep_nsec=100;

  // The main loop; interact with the device here
  for(;;)
  {
	  // Schlafen gehen.. (polling loop).
//	   tspec.tv_sec = 0;
//	   tspec.tv_nsec = sleep_nsec;
//	   nanosleep(&tspec, NULL);
    // test if we are supposed to cancel
    pthread_testcancel();

    // Process incoming messages.  ExampleDriver::ProcessMessage() is
    // called on each message.
    ProcessMessages();

      // Hole Bild ab (blocking)
      frame = cvQueryFrame(capture);
      // Funktion fuer Datenversand
      PublishData();

    // Interact with the device, and push out the resulting data, using
    // Driver::Publish()

    // Sleep (you might, for example, block on a read() instead)
  }
}

void CamSFZ::PublishData(){

	player_camera_data_t data;
	 // Bildeigenschaften einstellen
	 data.width       = frame->width;
	 data.height      = frame->height;
	 data.bpp         = frame->depth;
	 data.image_count = frame->imageSize;
	 data.image       = new unsigned char [frame->imageSize];
	 data.format      = PLAYER_CAMERA_FORMAT_MONO8;
	 data.compression = PLAYER_CAMERA_COMPRESS_RAW ;
	 // Bild von RGB nach BGR wandeln
	 cvCvtColor(frame,frame,CV_BGR2RGB);
	 // Bild in Player Bildstruktur kopieren
	 std::cout<<"depth: "<<data.bpp<<std::endl;
	 memcpy(data.image,frame->imageData,data.image_count);
	 // Daten an Player uebergeben
	 Publish(device_addr,
			 PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE,
			 reinterpret_cast<void*>(&data));
	 delete [] data.image;
	 return;

}

extern "C" {
  int player_driver_init(DriverTable* table)
  {
    puts("CamSFZ Init...");
    CamSFZ_Register(table);
    puts("CamSFZ wurde initialisiert.");
    return(0);
  }
}
