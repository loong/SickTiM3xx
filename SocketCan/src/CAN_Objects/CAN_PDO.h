/*
 * Can_PDO.h
 *
 *  Created on: 30.03.2010
 *      Author: locher
 */

#ifndef CAN_PDO_H_
#define CAN_PDO_H_


#include <exception>
#include <cstring>


#include "CAN_types_and_tools.h"


namespace sfzcan {


	class E_PDO_ToManyDataBytesAppended : public E_CAN_Exception {



	public:
		E_PDO_ToManyDataBytesAppended(): E_CAN_Exception("E_PDO_ToManyDataBytesAppended"){}

		explicit E_PDO_ToManyDataBytesAppended(const char* message ):
				E_CAN_Exception(message){};

		virtual ~E_PDO_ToManyDataBytesAppended()throw(){};

	};


	/**
	 * \brief Use CAN_PDO to fill a can_frame with can_id, dlc, and data
	 *
	 *
	 */


	class CAN_PDO {

		// @TODO Find out if read and write are Threadsave

	friend std::ostream& operator<<(std::ostream& out, const CAN_PDO& pdo);


	private:


		uint_t dataPointer;

		can_frame my_can_frame;

	public:

		typedef enum PDO_NR{PDO_1,PDO_2,PDO_3,PDO_4} pdo_nr_t;

		explicit CAN_PDO(canid_t nodeId,pdo_nr_t pdoNr);
		virtual ~CAN_PDO();


		void setNewData(msgdata_t data,uint_t numberOfBytesToAppend)
			throw(E_PDO_ToManyDataBytesAppended);
		void appendMsgData(msgdata_t data ,uint_t numberOfBytesToAppend)
			throw(E_PDO_ToManyDataBytesAppended);

		const can_frame* get_can_frame_reference() const{
			return &my_can_frame;
		}

	};


	std::ostream& operator<<(std::ostream& out, const CAN_PDO& pdo);

}  // namespace sfzcan

#endif /* CAN_PDO_H_ */
