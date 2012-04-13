/*
 * aim04.hpp
 *
 *  Created on: 07.04.2010
 *      Author: locher
 */

#ifndef AIM04_HPP_
#define AIM04_HPP_

/*
 * Player 3.0.1 plugin and driver for the Eckelmann aim04 analog to can adapter.
 * 	(c) 2010 Adrian Haarbach
 * 
 * helpful links:
 * 	http://psurobotics.org/wiki/index.php?title=Writing_a_Player_Plugin
 *
 * start with:
 * $ player aim04.cfg
 * Can-Messages simulated with socketcan. Start socketcan in background
 * $ ./can_if start
 * And simulate some messages:
 * $ ./cangen vcan0 -I 1E5 -L 8
 * or to manually start the aim04:
 * $ ./cansend can0 000#01.00
 * stop the aim04:
 * $ ./cansend can0 000#02.00
 * see output:
 * $ ./candump can0
 *
 * To view the results, use
 * $ playerv
 * and then subscribe to the sonar:0 interface
 *
 */

#include <string.h>

//cmake should have configured the makefile so that this include is found, even when eclipse doesn't find it
#include <libplayercore/playercore.h>

////redundant, for code completion in eclipse, all linked in playercore.h
//#include <player-3.0/libplayercore/playercore.h>
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


//socketcan includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string>


#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class Aim04driver : public ThreadedDriver
{
  public:
    
    // Constructor; need that
    Aim04driver(ConfigFile* cf, int section);

    virtual int ProcessMessage(QueuePointer & resp_queue, 
                               player_msghdr * hdr, 
                               void * data);

  private:
    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();

    //socketcan
    int skt;
    struct can_frame frame;
    std::string port;
    static const int nodeID=0x65;
    static const int canID=0x180+nodeID;

    bool debug;

    float tometers(unsigned int val);
};





#endif /* AIM04_HPP_ */
