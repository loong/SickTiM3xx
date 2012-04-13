/*
 * multi.h
 *
 *  Created on: 08.04.2010
 *      Authors: adrian locher
 */

#ifndef IDEFIX_HPP_
#define IDEFIX_HPP_

#include <string.h>
#include <player-3.0/libplayercore/playercore.h>
#include <string>


#include <iostream>


#include "All_Drives.h"
#include "AIM04_Data_Thread.h"

using namespace std;



// redundant, for code completion in eclipse, all linked in playercore.h
// #include <player-3.0/libplayercore/playercore.h>

//#include <player-3.0/libplayerinterface/interface_util.h>
//#include <player-3.0/libplayerinterface/player.h>
//#include <player-3.0/libplayerinterface/playerxdr.h> // for cleanup and copy methods
//#include <player-3.0/libplayerinterface/addr_util.h>
//#include <player-3.0/libplayercommon/playercommon.h>
//#include <player-3.0/libplayercore/configfile.h>
//#include <player-3.0/libplayercore/device.h>
//#include <player-3.0/libplayercore/devicetable.h>
//#include <player-3.0/libplayercore/driver.h>
//#include <player-3.0/libplayercore/drivertable.h>
//#include <player-3.0/libplayercore/filewatcher.h>
//#include <player-3.0/libplayercore/globals.h>
//#include <player-3.0/libplayercore/message.h>
//#include <player-3.0/libplayercore/playertime.h>
//#include <player-3.0/libplayercore/wallclocktime.h>
//#include <player-3.0/libplayercore/property.h>
//#include <player-3.0/playerconfig.h>
//#include <player-3.0/libplayerc++/playerc++.h>
//#include <player-3.0/libplayerc/playerc.h>
//#include <player-3.0/libplayerinterface/functiontable.h>
//#include <player-3.0/libplayerinterface/player_interfaces.h>


/**
 * \brief Player-Driver for SFZ Fieldrobot Idefix
 *
 * Provides the following Interfaces
 * <ul>
 * <li>Position2d
 * <li> Sonar
 * <li> Opaque
 * <ul>
 * Requires
 * <ul>
 * <li>linuxjoy
 * </ul>
 */

class Idefix : public ThreadedDriver{
public:
    // Constructor; need that
    Idefix(ConfigFile* cf, int section);

    virtual int ProcessMessage(QueuePointer & resp_queue,
                               player_msghdr * hdr,
                               void * data);

  private:


    // Class for Idefix Drive Controll
    idefix::All_Drives my_drives;

    //class for ultrasonic
    idefix::AIM04_Data_Thread aim;


    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();
//    virtual int Setup();

    // My position interface
    player_devaddr_t m_position_addr;

    // My sonar interface
     player_devaddr_t m_sonar_addr;

     // My sonar interface
      player_devaddr_t m_joystick_addr;

      // My opaque interface
       player_devaddr_t m_opaque_addr;


      //Control off Joystick driver

      Device* joypad_device_ptr;
      bool gamepad_controll_is_on; // Button 1
      bool drives_are_enabled; // Button_2
      idefix::gameped_button_mask_t pressed_gamepad_button_mask;

      double gamepad_max_speed_m_s;
      int main_loop_sleep_time_us;

      std::string can_interface_name;

      double sonar0[3];
      double sonar1[3];
      double sonar2[3];
      double sonar3[3];


      double* sonarArr[4];


    //eigene funktionen
    void PublishSonar();
    void PublishPosition();
    void PublishOpaque();

    int  handle_position2d_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);
    int  handle_sonar_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);
    int  handle_joystick_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);
    int  handle_opaque_0_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);



    bool debug;
};

#endif /* IDEFIX_HPP */
