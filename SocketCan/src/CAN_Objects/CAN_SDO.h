/*
 * CAN_SDO.h
 *
 *  Created on: 31.03.2010
 *      Author: locher
 */

#ifndef CAN_SDO_H_
#define CAN_SDO_H_

#include "CAN_types_and_tools.h"


namespace sfzcan {


class E_SDO_Exception : public E_CAN_Exception {



public:
	E_SDO_Exception(): E_CAN_Exception("E_SDO_Exception"){}

	explicit E_SDO_Exception(const char* message ):
		E_CAN_Exception(message){};

	virtual ~E_SDO_Exception()throw(){};


};


	/**
	 * \brief Klass representing CAN Service Data Objects (SDO)
	 *
	 */


	class CAN_SDO {

	friend class CAN_PDO_Thread;
	friend class CAN_SDO_Socket;
	friend std::ostream& operator<<(std::ostream& out, const CAN_SDO& sdo);


	protected:

//		boost::posix_time::ptime send_time;
//		boost::posix_time::ptime receive_time;

		timeout_t	timout_mus;


		can_frame my_can_frame_to_send;
		can_frame my_received_can_frame;


		bool messageAcknowledged; // SDO wurde beantwortet
		bool timeoutOccurred;	  // SDO wurde nicht zeitnah beantwortet
		bool sdoAbortOccurred; 	  // die SDO Kommunikation wurde abgebrochen
		bool sdoDataReceived;	  // Es wurden SDO Daten empfangenen

		bool can_frame_is_sdo;	  // can Frame ist ein sdo



	public:


//		std::string get_index_hex();
//		std::string get_subindex_hex();




		std::string get_CANopen_Profile_301_Error_String();

		CAN_SDO();
		explicit CAN_SDO(can_frame& can_frame);
		virtual ~CAN_SDO();

		virtual const can_frame& get_can_frame_to_send(){

			return my_can_frame_to_send;

		}

		virtual const can_frame& get_can_frame_received(){

			return my_received_can_frame;

		}

//		void set_can_frame(const can_frame &f );



	};


	std::ostream& operator<<(std::ostream& out, const CAN_SDO& sdo);


}  // namespace sfzcan


#endif /* CAN_SDO_H_ */
