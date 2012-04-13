/*
 * EDS_Object_Description.h
 *
 *  Created on: 23.02.2009
 *      Author: locher
 */

#ifndef CAN_EDS_SDO_H_
#define CAN_EDS_SDO_H_


#include <cstring>

#include "CAN_types_and_tools.h"

#include "CAN_SDO.h"


namespace sfzcan {


	class EdsError : public E_SDO_Exception{

		friend class CAN_Config_EDS;



	public:
		EdsError();
		EdsError(const char* errorMsg):
			E_SDO_Exception(errorMsg)
		{

		}

		void appendMessage(const char* appendMsg){

			if( (strlen(my_message)+strlen(appendMsg) )< 	MAX_MESSAGE_LENGHT){

				strcat(my_message,appendMsg);

			}else{
				strncat(my_message,appendMsg,MAX_MESSAGE_LENGHT-1);
				my_message[MAX_MESSAGE_LENGHT-1]='\0';
			}

		}

		virtual ~EdsError()throw(){};


	};


	class EdsObjectIsReadOnlyError: public EdsError{

	public:
		EdsObjectIsReadOnlyError();
		virtual ~EdsObjectIsReadOnlyError()throw(){};

	};

	class EdsDataOutOfRangeError:public  EdsError{
	public:
		EdsDataOutOfRangeError();
		virtual ~EdsDataOutOfRangeError()throw(){};
	};

	class EdsNoDataAvailableError:public  EdsError{

	public:
		EdsNoDataAvailableError();
		virtual ~EdsNoDataAvailableError()throw(){};
	};


	class EdsSendingOfThisMessageTypeIsNotSupportedError: public EdsError{

	public:
		EdsSendingOfThisMessageTypeIsNotSupportedError();
		virtual ~EdsSendingOfThisMessageTypeIsNotSupportedError()throw(){};
	};


	class EdsTimeoutError :public EdsError{
	public:
		EdsTimeoutError();
		virtual ~EdsTimeoutError()throw(){}
	};

	class EdsValueOutOfRangeError: public EdsError{
	public:
		EdsValueOutOfRangeError();
		virtual ~EdsValueOutOfRangeError()throw(){}
	};

	class EdsDataTypeIsNotSupportedError: public EdsError{
	public:
		EdsDataTypeIsNotSupportedError();
		virtual~ EdsDataTypeIsNotSupportedError()throw(){};
	};


	class EdsObjectIsWriteOnlyError:public EdsError{
	public:
		EdsObjectIsWriteOnlyError();
		virtual ~EdsObjectIsWriteOnlyError()throw(){};
	};





	/**
	 * \brief The CAN_EDS_SDO is an SDO which could be configured from the
	 * information of an EDS FIle Entry
	 *
	 * With the Information from the EDS-File Entries it is possible to
	 * transform the Data in the can_frame Bytes in the msgdata_t format und viceversa
	 *
	 * It is also possible to check minimum and maximum Values
	 *
	 */


	// For Electronic Data Sheet Conventions see:  CiA 306

	class CAN_EDS_SDO : public CAN_SDO{

		friend class CAN_Config_EDS;

	//friend class EstAIM04_Profile;
	//friend  class BMSxInt_Profile;
	//friend  class Maxon_Epos_24_5_Profile;
	//friend  class Maxon_Mcd_Epos_60W_Profile;


	public:
			enum DATA_TYPE{
				DATA_TYPE_INTEGER_8=0x2,
				DATA_TYPE_INTEGER_16=0x3,
				DATA_TYPE_INTEGER_32=0x4,
				DATA_TYPE_UNSIGNED_8=0x5,
				DATA_TYPE_UNSIGNED_16=0x6,
				DATA_TYPE_UNSIGNED_32=0x7,
				DATA_TYPE_VISIBLE_STRING=0x9,
				DATA_TYPE_UNKNOWN=0xF,
				DATA_TYPE_UNSIGNED_64=0x1B
				};

		enum ACCESS_TYPE{ACCESS_TYPE_RO,
						ACCESS_TYPE_WO,
						ACCESS_TYPE_RW,
						ACCESS_TYPE_RWR,
						ACCESS_TYPE_RWW,
						ACCESS_TYPE_CONST
						};

		enum READ_WRITE_STATE{
			READ_WRITE_STATE_READ,
			READ_WRITE_STATE_WRITE
		};

	private:

		enum DEF_TYPE{DEF_TYPE_VAR,DEF_TYPE_RECORD,DEF_TYPE_COMPACT_RECORD};

		DEF_TYPE def_type;

		uint_t index;
		uint_t subindex;
		uint_t nodeId;

		std::string parameterName;
		uint_t objectType;
		DATA_TYPE dataType;
		ACCESS_TYPE accessType;
		msgdata_t defaultValue;
		uint_t pdoMapping;
		uint_t subNumber;
		msgdata_t lowLimit;
		msgdata_t highLimit;
		uint_t  objFlags;
		uint_t compactSubObj;

		READ_WRITE_STATE readWriteState;

		// EDS State Flags

		bool dataTypeSet;
		bool accessTypeSet;
		bool defaultValueSet;
		bool pdoMapppingSet;
		bool subNumberSet;
		bool lowLimitSet;
		bool highLimitSet;
		bool objFlagsSet;
		bool compactSubObjSet;

		// StateFlags
		bool nodeIdSet;
		bool readWriteStateSet;
		bool dataSet;
		bool messageSent;
		// Debug State

		bool isDebuggingActive;


		void setDefaultValue(msgdata_t defaultValue);
		void setPdoMapping(uint_t pdoMapping);
		void setLowLimit(msgdata_t lowLimit);
		void setHighLimit(msgdata_t highLimit);
		void setObjFlags(uint_t objFlags);

		void setDebugState(bool active=true);



	public:
//		CAN_EDS_SDO(){};
		// Konstruktor für DEFTYPE VAR
		CAN_EDS_SDO(
				uint_t index,
				uint_t subindex,
				std::string parameterName,
				DATA_TYPE dataType,
				ACCESS_TYPE accessType,
				bool defaultValue_set,
				msgdata_t defaultValue,
				bool lowLimit_set,
				msgdata_t lowLimit,
				bool high_Limit_set,
				msgdata_t highLimit,
				bool pdoMappingSet,
				msgdata_t pdoMapping,
				bool objFlags_set,
				msgdata_t objFlags
				);


		// Konstruktor für DEFTYPE RECORD
		// so far we have no RECORDs in pur eds-Files,
		// This Construktor is not tested and used!

		CAN_EDS_SDO(
				uint_t index,
				std::string parameterName,
				uint_t objectType,
				uint_t subNumber
				);

		// Konstruktor für DEFTYPE RECORD
		// so far we have no RECORDs in pur eds-Files,
		// This Construktor is not tested and used!
		CAN_EDS_SDO(
		uint_t index,
		uint_t subindex,
		std::string parameterName,
		DATA_TYPE dataType,
		ACCESS_TYPE accessType,
		uint_t compactSubObj
		);





		msgdata_t getReturnedData() throw(E_SDO_Exception);


		virtual ~CAN_EDS_SDO();

	private:
		void setData(msgdata_t data) throw(E_SDO_Exception);

		void setNodeId(uint_t nodeID);
		void setReadWriteState(READ_WRITE_STATE state)throw(E_SDO_Exception);


	};


}  // namespace sfzcan

#endif /* CAN_EDS_SDO_H_ */
