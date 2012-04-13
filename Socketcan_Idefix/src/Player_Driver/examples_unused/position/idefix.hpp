
#ifndef IDEFIX_H_
#define IDEFIX_H_



#include <libplayercore/playercore.h>

#include "All_Drives.h"
#include "Debug_Stream.h"





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
Driver* Idefix_Init(ConfigFile* cf, int section);


// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void Idefix_Register(DriverTable* table);


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



namespace idefix {






	/**
	 * The class for the driver
	 *
	 * @version 1.0
	 */


	class Idefix : public ThreadedDriver
	{
	  public:

		Idefix(ConfigFile* cf, int section);
	//    virtual ~RoboOne(){};

		virtual ~Idefix();

		virtual int Setup();
		virtual int Shutdown();

		virtual int ProcessMessage(QueuePointer &resp_queue,
								   player_msghdr * hdr,
								   void * data);

	  private:

		virtual void Main();
		void UpdateData();
		void UpdatePosData(player_position2d_data_t *d);


		sfzdebug::Debug_Stream dout;

		// interfaces
		player_devaddr_t position_id;

		// driver name for logging support
		const char* driverName;

		bool drives_configured;

//		double mVelocity;
//		double mRotationSpeed;

		uint_t cyclus_time_mus;


		All_Drives my_drives;
		// Can Interface
		char* can_interface;

		// debugging
		bool m_bDebug;
		struct timeval m_tOldTime;

	};


}  // namespace idefix

#endif /* IDEFIX_H_ */
