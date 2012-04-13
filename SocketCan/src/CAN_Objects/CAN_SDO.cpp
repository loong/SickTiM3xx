/*
 * CAN_SDO.cpp
 *
 *  Created on: 31.03.2010
 *      Author: locher
 */

#include "CAN_SDO.h"

namespace sfzcan {




	CAN_SDO::CAN_SDO():
		messageAcknowledged(false),
		timeoutOccurred(false),
		sdoAbortOccurred(false),
		sdoDataReceived(false),
		can_frame_is_sdo(false)
	{


	}
	CAN_SDO::CAN_SDO(can_frame& f):
		my_can_frame_to_send(f),

		messageAcknowledged(false),
		timeoutOccurred(false),
		sdoAbortOccurred(false),
		sdoDataReceived(false),
		can_frame_is_sdo(false)

	{
	}

	CAN_SDO::~CAN_SDO() {


	}



	std::string CAN_SDO::get_CANopen_Profile_301_Error_String(){

		if(sdoAbortOccurred){

			msgdata_t errorData=0;

			errorData+= my_received_can_frame.data[7] <<24;
			errorData+= my_received_can_frame.data[6] <<16;
			errorData+= my_received_can_frame.data[5] <<8;
			errorData+= my_received_can_frame.data[4] ;



		  if(errorData==0x00000000) return std::string("No error");

		  if(errorData==0x06020000)return std::string("Object does not exist in the object dictionary");
		  if(errorData==0x06090011) return std::string("Sub-index does not exist");
		  if(errorData==0x05040001) return std::string("Client/server command specifier not valid or unknown");
		  if(errorData==0x05030000) return std::string("Toggle bit not alternated");
		  if(errorData==0x05040000) return std::string("SDO protocol timed out");
		  if(errorData==0x05040005) return std::string("Out of memory");
		  if(errorData==0x06010000) return std::string("Unsupported access to an object");
		  if(errorData==0x06010001) return std::string("Attempt to read a write only object");
		  if(errorData==0x06010002) return std::string("Attempt to write a read only object");
		  if(errorData==0x06040041) return std::string("Object cannot be mapped to the PDO");
		  if(errorData==0x06040042) return std::string("The number and length of the objects to be mapped would exceed PDO length");
		  if(errorData==0x06040043) return std::string("General parameter incompatibility reason");
		  if(errorData==0x06040047) return std::string("General internal incompatibility in the device");
		  if(errorData==0x06060000) return std::string("Access failed due to an hardware error");
		  if(errorData==0x06070010) return std::string("Data type does not match, length of service parameter does not match");
		  if(errorData==0x06070012) return std::string("Data type does not match, length of service parameter too high");
		  if(errorData==0x06070013) return std::string("Data type does not match, length of service parameter too low");
		  if(errorData==0x06090030) return std::string("Value range of parameter exceeded (only for write access)");
		  if(errorData==0x06090031) return std::string("Value of parameter written too high");
		  if(errorData==0x06090032) return std::string("Value of parameter written too low");
		  if(errorData==0x06090036) return std::string("Maximum value is less than minimum value");
		  if(errorData==0x08000000) return std::string("General error");
		  if(errorData==0x08000020) return std::string("Data cannot be transferred or stored to the application");
		  if(errorData==0x08000021) return std::string("Data cannot be transferred or stored to the application because of local control");
		  if(errorData==0x08000022) return std::string("Data cannot be transferred or stored to the application because of the present device state");
		//The following error codes are maxon specific:
		  if(errorData==0x0F00FFC0) return std::string("The device is in wrong NMT state");
		  if(errorData==0x0F00FFBF) return std::string("The RS232 command is illegal");
		  if(errorData==0x0F00FFBE) return std::string("The password is not correct");
		  if(errorData==0x0F00FFBC) return std::string("The device is not in service mode");
		  if(errorData==0x0F00FFB9) return std::string("Error Node-ID");


		// Error not in List

		  return std::string("Occured error is not in my List of errors");
		}else{

			return std::string("SDO recieved no abort Error ");
		}


	}

	std::ostream& operator<<(std::ostream& out, const CAN_SDO& sdo){

		out << sdo.my_can_frame_to_send ;
		return out;

	}


}  // namespace sfzcan
