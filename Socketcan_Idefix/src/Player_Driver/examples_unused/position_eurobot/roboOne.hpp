/*
 * roboOne.h
 *
 *  Created on: 07.04.2010
 *      Author: locher
 */

#ifndef ROBOONE_H_
#define ROBOONE_H_


#include <unistd.h>
#include <string>
#include <typeinfo>
#include <math.h>
#include <stdlib.h>

#include <libplayercore/playercore.h>
#include "roboOneMotors.h"

#include "driver_logging.h"

#define TICKS_PER_ROTATION 1850




/**
 * RoboOne_Init
 * A factory creation function, declared outside of the class so that it
 * can be invoked without any object context (alternatively, you can
 * declare it static in the class).  In this function, we create and return
 * (as a generic Driver*) a pointer to a new instance of this driver.
 *
 * @param cf
 * @param section
 *
 * @return Driver*
 */
Driver* RoboOne_Init(ConfigFile* cf, int section);
//{
//  // Create and return a new instance of this driver
//  return((Driver*)(new RoboOne(cf, section)));
//}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void RoboOne_Register(DriverTable* table);
//{
//  char driverName[] = "RoboOne";
//  table->AddDriver(driverName, RoboOne_Init);
//}




/**
 * The class for the driver
 *
 * @version 1.0
 */
class RoboOne : public ThreadedDriver
{
  public:

    RoboOne(ConfigFile* cf, int section);
//    virtual ~RoboOne(){};

    virtual int Setup();
    virtual int Shutdown();

    virtual int ProcessMessage(QueuePointer &resp_queue,
                               player_msghdr * hdr,
                               void * data);

  private:

    virtual void Main();
    void UpdateData();
    void UpdatePosData(player_position2d_data_t *d);
    int GetStall();
    int ReadPos(int * pos1, int * pos2);
    int ReadSpeed(int * pos1, int * pos2);
    int ResetOdometry();
    int SetOdometry(double xNew, double yNew, double yawNew);
    int getTickDiff(int16_t oldTicks, int16_t newTicks);
    int meterPerSec2ticksPerTenMs(double speed);  // converts m/s in ticks/10ms
    double ticksPerTenMs2meterPerSec(int16_t speed); // converts ticks/10ms to m/s

    // interfaces
    player_devaddr_t position_id;

    RoboOneMotors* odoRobot;

    char* port;
    int baudrate;

    // odometry stuff
    int last_lpos, last_rpos;
    double x,y,yaw;
    double wheel_distance;
    double wheel_diameter;
    double mVelocity;
    double mRotationSpeed;

    // Regelparameter
    float pid_p1,pid_p2,pid_i1,pid_i2;
    float break_amp;

    // debugging
    bool m_bDebug;
    struct timeval m_tOldTime;
    int m_iReps;

    // driver name for logging support
    const char* driverName;
};

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C" {
  int player_driver_init(DriverTable* table);
//  {
//    RoboOne_Register(table);
//    return(0);
//  }
}



#endif /* ROBOONE_H_ */
