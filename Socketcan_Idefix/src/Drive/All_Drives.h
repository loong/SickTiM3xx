/*
 * All_Drives.h
 *
 *  Created on: 08.04.2010
 *      Author: locher
 */

#ifndef ALL_DRIVES_H_
#define ALL_DRIVES_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can/raw.h>
#include <linux/can/bcm.h>

#include <boost/thread/mutex.hpp>


#include "Thread.h"
#include "Idefix_Data_Structs.h"
#include "Idefix_Config_Parameters.h"


#include "Drive.h"
#include "Steering.h"

#include "EPOS_Emergency_Thread.h"

namespace idefix {


/*
 * TODO Thread locking implementieren!!!!
 */


	/**
	 * \brief All drives liefert Player den Zugriff auf die Antriebe.
	 *
	 * In einem eigenen Thread sendet es sync Telegramme und ein BCM socket
	 * wartet auf die PDO1 der Antriebe.
	 * Wenn vier PDOs eingetroffen sind, werden bei den einzelnen Antiebsklassen die PDOs
	 * ausgelesen und ausgewertet.
	 *
	 *
	 *
	 */

	class All_Drives : public Thread {

	private:

		boost::mutex my_mutex;



		std::string can_interface_name;

		EPOS_Emergency_Thread et;

		// Zur Konfiguration des Socketcan broadcast Managers
		struct {
			struct bcm_msg_head msg_head;
//			struct can_frame frame;
		} bcm_to_filter_pdos;


		struct {
			struct bcm_msg_head msg_head;
			struct can_frame frame;
		} recived_bcm_pdo;

		canid_t drives_nodes[4];

		can_frame sync_frame;


		int my_sync_telegram_socket;
		int my_bcm_socket;

		bool sync_socket_is_bound;
		bool bcm_socket_is_bound;

		bool drives_configured;
		bool is_homing_state_running;

		//uint_t cyclus_time_mus;


		Drive D_front;
		Drive D_rear;
		Steering S_front;
		Steering S_rear;

		void sendSyncTelegramm()throw(std::exception);


		virtual void process();
		void update_drive_data()throw(std::exception);


	public:
		All_Drives();
		virtual ~All_Drives();

		void config_Drives();

		void start_homing(robo_mount_pos_t mount_pos);

		void set_velocity_ms(double val);
		void set_move_distance_m(double val);
//		bool get_moved_distance_reached();

		void set_angle_front(double val);
		void set_angle_rear(double val);
		void reset_distance_measuring();

		void disable_drives();
		void reenable_drives();


		void get_drive_data(opaque_data_t& data);

		void setCanInterfaceName(std::string _can_interface_name);

	};


}  // namespace idefix

#endif /* ALL_DRIVES_H_ */
