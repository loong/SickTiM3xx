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

using namespace std;


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

    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();

    // My position interface
    player_devaddr_t m_position_addr;

     // My Joystick interface
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



    //eigene funktionen
    void PublishPosition();
    void PublishOpaque();

    int  handle_position2d_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);
    int  handle_joystick_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);
    int  handle_opaque_0_Interface_messages(QueuePointer & resp_queue,player_msghdr * hdr,void * data);

    bool debug;
};

#endif /* IDEFIX_HPP */
