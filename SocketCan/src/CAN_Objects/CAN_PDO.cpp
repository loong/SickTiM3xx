/*
 * Can_PDO.cpp
 *
 *  Created on: 30.03.2010
 *      Author: locher
 */

#include "CAN_PDO.h"

namespace sfzcan {




	CAN_PDO::CAN_PDO(canid_t nodeId,pdo_nr_t pdo_nr):
			dataPointer(0)

	{

		memset(&my_can_frame,0,sizeof(can_frame) );

		switch(pdo_nr){
		case PDO_1:
			my_can_frame.can_id=0x200+nodeId;
		break;

		case PDO_2:
			my_can_frame.can_id=0x300+nodeId;
		break;

		case PDO_3:
			my_can_frame.can_id=0x400+nodeId;
		break;

		case PDO_4:
			my_can_frame.can_id=0x500+nodeId;
		break;


	}
		my_can_frame.can_dlc=8;

	}

	CAN_PDO::~CAN_PDO() {
		// TODO Auto-generated destructor stub
	}


	void CAN_PDO::appendMsgData(msgdata_t data, uint_t numberOfBytesToAppend)throw(E_PDO_ToManyDataBytesAppended){
		uint_t end=	dataPointer+ numberOfBytesToAppend;
		if(end<=8){
			for(uint_t i=dataPointer,count=0 ; i< end; i++, count++ ){
				my_can_frame.data[i]=__u8(data>>(8*count));
			}
			dataPointer=end;

		}else{
			throw(E_PDO_ToManyDataBytesAppended());
		}
	}

	void CAN_PDO::setNewData(msgdata_t data,uint_t numberOfBytesToAppend)throw(E_PDO_ToManyDataBytesAppended){
		dataPointer=0;
		appendMsgData(data,numberOfBytesToAppend);
	}




	std::ostream& operator<<(std::ostream& out, const CAN_PDO& pdo){

		out << pdo.my_can_frame ;
		return out;

	}


}  // namespace sfzcan
