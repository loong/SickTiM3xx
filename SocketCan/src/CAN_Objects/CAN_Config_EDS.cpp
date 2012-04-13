/*
 * CAN_EDS2SDO_Factory.cpp
 *
 *  Created on: 01.04.2010
 *      Author: locher
 */

#include "CAN_Config_EDS.h"



namespace sfzcan {








	CAN_Config_EDS::CAN_Config_EDS() {

	}

	CAN_Config_EDS::~CAN_Config_EDS() {
	}



	std::auto_ptr<CAN_EDS_SDO> CAN_Config_EDS::get_EDS(
			CAN_EDS_SDO  eds,
			const canid_t nodeId,
			const CAN_EDS_SDO::READ_WRITE_STATE rw_state,
			const msgdata_t msgdata)throw(E_SDO_Exception){

//		if(!messageSent){
//			messageSent=true;
//		}else{
//
//			throw(EdsError("Eds Error: A message could be send only once "));
//		}

		eds.setNodeId(nodeId);
		eds.setReadWriteState(rw_state);
		if(rw_state==CAN_EDS_SDO::READ_WRITE_STATE_WRITE){
			eds.setData(msgdata);
		}


		if( (eds.def_type!=CAN_EDS_SDO::DEF_TYPE_VAR) ){
			throw(EdsSendingOfThisMessageTypeIsNotSupportedError());
		}
		if(!eds.nodeIdSet){
			//@TODO Write new Error Class
			throw(E_SDO_Exception("Eds Error: NodeId not set"));
		}
		if(!eds.readWriteStateSet){
			//@TODO Write new Error Class
			throw(E_SDO_Exception("Eds Error: Read Write State not set"));
		}


		eds.my_can_frame_to_send.can_id=0x600+nodeId;

		if(eds.readWriteState==CAN_EDS_SDO::READ_WRITE_STATE_READ){

			eds.my_can_frame_to_send.data[0]=0x40;
			eds.my_can_frame_to_send.can_dlc=4;

//			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_8)
//				||
//				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_8 )){
//
//
//
//			}
//			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_16)
//			||
//			(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_16)){
//
//				//eds.my_can_frame_to_send.data[0]=0x4B;
//
//
//			}
//			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_32)
//				||
//				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_32
//				||
//				eds.dataType==CAN_EDS_SDO::DATA_TYPE_VISIBLE_STRING))
//			{
//
////				eds.my_can_frame_to_send.data[0]=0x43;
//
//			}

		}else{ //READ_WRITE_STATE_WRITE


			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_8)
				||
				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_8 ))
			{
				eds.my_can_frame_to_send.can_dlc=5;
				eds.my_can_frame_to_send.data[0]=0x2F;

			}

			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_16)
				||
				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_16))
			{
				eds.my_can_frame_to_send.can_dlc=6;
				eds.my_can_frame_to_send.data[0]=0x2B;

			}

			if( (eds.dataType==CAN_EDS_SDO::DATA_TYPE_INTEGER_32)
				||
				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_32)
				||
				(eds.dataType==CAN_EDS_SDO::DATA_TYPE_VISIBLE_STRING) )
			{
				eds.my_can_frame_to_send.can_dlc=8;
				eds.my_can_frame_to_send.data[0]=0x23;

			}
		}

		if((eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNKNOWN  )
			||
			(eds.dataType==CAN_EDS_SDO::DATA_TYPE_UNSIGNED_64) ){
			throw(EdsSendingOfThisMessageTypeIsNotSupportedError());
		}


		eds.my_can_frame_to_send.data[1]=__u8(eds.index);
		eds.my_can_frame_to_send.data[2]=__u8(eds.index>>8);
		eds.my_can_frame_to_send.data[3]=eds.subindex;

		if(eds.readWriteState==CAN_EDS_SDO::READ_WRITE_STATE_WRITE){

			if(!eds.dataSet){

				throw(EdsNoDataAvailableError());

			}

		}



		return std::auto_ptr<CAN_EDS_SDO>(new CAN_EDS_SDO(eds));



	}




}  // namespace sfzcan
