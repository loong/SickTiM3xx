/*
 * EDS_Object_Description.cpp
 *
 *  Created on: 23.02.2009
 *      Author: locher
 */

#include "CAN_EDS_SDO.h"



namespace sfzcan {



	EdsObjectIsReadOnlyError::EdsObjectIsReadOnlyError():
		EdsError("Eds Object is Read Only")
	{	}

	EdsDataOutOfRangeError::EdsDataOutOfRangeError():
		EdsError("Eds Error:  Data is out of Range")
	{	}


	EdsNoDataAvailableError::EdsNoDataAvailableError():
		EdsError("Eds Error: No Data Available")
		{	}

	EdsSendingOfThisMessageTypeIsNotSupportedError::EdsSendingOfThisMessageTypeIsNotSupportedError():
		EdsError("Eds Error: Sending Of This Message Type Is Not Supported")
		{	}

	EdsTimeoutError::EdsTimeoutError():

		EdsError("Eds Error: Timeout occured while waiting for response")

		{	}

	EdsValueOutOfRangeError::EdsValueOutOfRangeError():

		EdsError("Eds Error: Data Value out of range")

		{	}

	EdsDataTypeIsNotSupportedError::EdsDataTypeIsNotSupportedError():

		EdsError("Eds Error: Data Type is not supported")

		{	}

	EdsObjectIsWriteOnlyError::EdsObjectIsWriteOnlyError():
		EdsError("Eds Error: Data type is write only")

		{	}






	// VAR Type
	CAN_EDS_SDO::CAN_EDS_SDO(
			uint_t index,
			uint_t subindex,
			std::string parameterName,
			DATA_TYPE dataType,
			ACCESS_TYPE accessType,
			bool __defaultValue_set,
			msgdata_t __defaultValue,
			bool __lowLimit_set,
			msgdata_t __lowLimit,
			bool __high_Limit_set,
			msgdata_t __highLimit,
			bool __pdoMappingSet,
			msgdata_t __pdoMapping,
			bool __objFlags_set,
			msgdata_t __objFlags
			)
	:
		def_type(DEF_TYPE_VAR),
		parameterName(parameterName),
		objectType(0x7),
		dataTypeSet(true),
		accessTypeSet(true),
		defaultValueSet(false),
		pdoMapppingSet(false),
		subNumberSet(false),
		lowLimitSet(false),
		highLimitSet(false),
		objFlagsSet(false),
		compactSubObjSet(false),
		nodeIdSet(false),
		readWriteStateSet(false),
		dataSet(false),
		messageSent(false)

	{
				this->index=(__u16)index;
				this->subindex=(__u8)subindex;
				this->dataType=dataType;
				this->accessType=accessType;

				isDebuggingActive=false;

				// Struktur msg mit Nullen füllen
				memset(&my_can_frame_to_send,0,sizeof(my_can_frame_to_send));

				if(__defaultValue_set){
					setDefaultValue(__defaultValue);
				}
				if(__lowLimit_set){
					setLowLimit(__lowLimit);
				}
				if(__high_Limit_set){
					setHighLimit(__highLimit);
				}
				if(__pdoMappingSet){
					setPdoMapping(__pdoMapping);
				}
				if(__objFlags_set){
					setObjFlags(__objFlags);
				}
	}

	// RECORD: Konstruktor is supported
	// but nothing else
	CAN_EDS_SDO::CAN_EDS_SDO(
				uint_t index,
				std::string parameterName,
				uint_t objectType,
				uint_t subNumber
				)
			:
			def_type(DEF_TYPE_RECORD),
			parameterName(parameterName),
			dataTypeSet(false),
			accessTypeSet(false),
			defaultValueSet(false),
			pdoMapppingSet(false),
			subNumberSet(true),
			lowLimitSet(false),
			highLimitSet(false),
			objFlagsSet(false),
			compactSubObjSet(false),
			nodeIdSet(false),
			readWriteStateSet(false),
			dataSet(false)


			{
						this->index=(__u16)index;
						this->objectType=objectType;
						this->subNumber=subNumber;

						isDebuggingActive=false;
						// Struktur msg mit Nullen füllen
						memset(&my_can_frame_to_send,0,sizeof(my_can_frame_to_send));
			}

	// Compact RECORD:  Konstruktor is supported
	// but nothing else
	CAN_EDS_SDO::CAN_EDS_SDO(
				uint_t index,
				uint_t subindex,
				std::string parameterName,
				DATA_TYPE dataType,
				ACCESS_TYPE accessType,
				uint_t compactSubObj
			)
			:
				def_type(DEF_TYPE_COMPACT_RECORD),
				parameterName(parameterName),
				dataTypeSet(true),
				accessTypeSet(true),
				defaultValueSet(false),
				pdoMapppingSet(false),
				subNumberSet(false),
				lowLimitSet(false),
				highLimitSet(false),
				objFlagsSet(false),
				compactSubObjSet(true),
				nodeIdSet(false),
				readWriteStateSet(false),
				dataSet(false)



	{
						this->index=(__u16)index;
						this->subindex=(__u8)subindex;
						this->dataType=dataType;
						this->accessType=accessType;
						this->compactSubObj=compactSubObj;

						isDebuggingActive=false;
						// Struktur msg mit Nullen füllen
						memset(&my_can_frame_to_send,0,sizeof(my_can_frame_to_send));
	}



	CAN_EDS_SDO::~CAN_EDS_SDO() {

	}



	void CAN_EDS_SDO::setData(msgdata_t data)throw(E_SDO_Exception){

		if(accessType==ACCESS_TYPE_RO || accessType==ACCESS_TYPE_CONST){
			throw(EdsObjectIsReadOnlyError());
		}

		if(lowLimitSet && data<lowLimit){
			EdsDataOutOfRangeError error;
			error.appendMessage(" value is to low!" );
			throw(EdsDataOutOfRangeError(error));
		}
		if(highLimitSet && data>highLimit){
			EdsDataOutOfRangeError error;
			error.appendMessage(" value is to high!" );
			throw(EdsDataOutOfRangeError(error));
		}

		if(dataType==DATA_TYPE_INTEGER_8){
			if(data<-128 || data>127){
				throw(EdsDataOutOfRangeError());
			}
			my_can_frame_to_send.data[4]=__s8(data);
		}
		if(dataType==DATA_TYPE_UNSIGNED_8){
			if(data<0 || data>=256){
				throw(EdsDataOutOfRangeError());
			}
			data=(u_msgdata_t)data;
			my_can_frame_to_send.data[4]=__u8(data);
		}
		if(dataType==DATA_TYPE_INTEGER_16){
			if( (data<(-128*256)) || (data>(128*256-1))  ){
				throw(EdsDataOutOfRangeError());
			}
			my_can_frame_to_send.data[4]=__s8(data);
			my_can_frame_to_send.data[5]=__s8(data>>8);
		}
		if(dataType==DATA_TYPE_UNSIGNED_16){
			if(data<0 || data>=(256*256) ){
				throw(EdsDataOutOfRangeError());
			}

			data=(u_msgdata_t)data;
			my_can_frame_to_send.data[4]=__u8(data);
			my_can_frame_to_send.data[5]=__u8(data>>8);
		}
		if(dataType==DATA_TYPE_INTEGER_32){
			if( (data<(-(msgdata_t)128*256*256*256l) )|| (data>((msgdata_t)128*256*256*256l-1)  ) ){
				throw(EdsDataOutOfRangeError());
			}
			my_can_frame_to_send.data[4]=__s8(data);
			my_can_frame_to_send.data[5]=__s8(data>>8);
			my_can_frame_to_send.data[6]=__s8(data>>16);
			my_can_frame_to_send.data[7]=__s8(data>>24);
		}
		if(dataType==DATA_TYPE_UNSIGNED_32){
			if( (data<0) || (data>=((msgdata_t)256*256*256*256l)) ){
				throw(EdsDataOutOfRangeError());
			}
			data=(u_msgdata_t)data;
			my_can_frame_to_send.data[4]=__u8(data);
			my_can_frame_to_send.data[5]=__u8(data>>8);
			my_can_frame_to_send.data[6]=__u8(data>>16);
			my_can_frame_to_send.data[7]=__u8(data>>24);
		}


		dataSet=true;

	}


	msgdata_t CAN_EDS_SDO::getReturnedData()throw(E_SDO_Exception){

		if(!messageAcknowledged){
			EdsNoDataAvailableError error;
			error.appendMessage(": SDO was not Acknowledged");
			throw(error);

		}


		if(accessType==ACCESS_TYPE_WO){
			EdsNoDataAvailableError error;
			error.appendMessage(": Objekt is Write Only");
			throw(error);
		}


		if(dataType==DATA_TYPE_INTEGER_8){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_s8(my_received_can_frame);
		}
		if(dataType==DATA_TYPE_UNSIGNED_8){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_u8(my_received_can_frame);
		}
		if(dataType==DATA_TYPE_INTEGER_16){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_s16(my_received_can_frame);
		}
		if(dataType==DATA_TYPE_UNSIGNED_16){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_u16(my_received_can_frame);
		}
		if(dataType==DATA_TYPE_INTEGER_32){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_s32(my_received_can_frame);
		}
		if(dataType==DATA_TYPE_UNSIGNED_32){
			return (msgdata_t)CAN_Tools::TPCANMsg_DataTo_u32(my_received_can_frame);
		}

		throw(EdsDataTypeIsNotSupportedError());
	}







	void CAN_EDS_SDO::setDefaultValue(msgdata_t defaultValue){
		defaultValueSet=true;
		this->defaultValue=defaultValue;

	}

	void CAN_EDS_SDO::setLowLimit(msgdata_t lowLimit){
		lowLimitSet=true;
		this->lowLimit=lowLimit;
	}

	void CAN_EDS_SDO::setHighLimit(msgdata_t highLimit){
		highLimitSet=true;
		this->highLimit=highLimit;
	}

	void CAN_EDS_SDO::setObjFlags(uint_t objFlags){
		objFlagsSet=true;
		this->objFlags=objFlags;
	}

	void CAN_EDS_SDO::setPdoMapping(uint_t pdoMapping){
		pdoMapppingSet=true;
		this->pdoMapping=pdoMapping;
	}


	void CAN_EDS_SDO::setNodeId(uint_t nodeId){

		this->nodeId=nodeId;
		nodeIdSet=true;

	}

	void CAN_EDS_SDO::setReadWriteState(READ_WRITE_STATE state)throw(E_SDO_Exception)
	{
		if(state==READ_WRITE_STATE_READ){

			if(accessType==ACCESS_TYPE_WO){
				throw(EdsObjectIsWriteOnlyError());
			}
		}else{ //Writing
			if( (accessType==ACCESS_TYPE_RO ) || (accessType==ACCESS_TYPE_CONST)){
				throw(EdsObjectIsReadOnlyError());
			}
		}

		readWriteStateSet=true;
		readWriteState=state;

	}

}  // namespace sfzcan

