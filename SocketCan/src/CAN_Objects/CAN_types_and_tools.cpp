/*
 * CAN_types_and_tools.cpp
 *
 *  Created on: 31.03.2010
 *      Author: locher
 */

#include "CAN_types_and_tools.h"

namespace sfzcan {



	__u8 CAN_Tools::TPCANMsg_DataTo_u8(can_frame msg){
		return msg.data[4];
	}
	__s8 CAN_Tools::TPCANMsg_DataTo_s8(can_frame msg){
		return (__s8)msg.data[4];
	}
	__u16 CAN_Tools::TPCANMsg_DataTo_u16(can_frame msg){
		return (__u16)(msg.data[4] | (msg.data[5] << 8));
	}
	__s16 CAN_Tools::TPCANMsg_DataTo_s16(can_frame msg){
		return (__s16)(msg.data[4] | (msg.data[5] << 8));
	}
	__u32 CAN_Tools::TPCANMsg_DataTo_u32(can_frame msg){
		return (__u32)(msg.data[4] | (msg.data[5] << 8)| (msg.data[6] << 16)| (msg.data[7] << 24));
	}
	__s32 CAN_Tools::TPCANMsg_DataTo_s32(can_frame msg){
		return (__s32)(msg.data[4] | (msg.data[5] << 8)| (msg.data[6] << 16)| (msg.data[7] << 24));
	}

	msgdata_t CAN_Tools::fourCharStringTo_msgdata(std::string fourChars ){

		// @TODO not tested
		const char* c=fourChars.c_str();
		msgdata_t result=(
				(msgdata_t)c[0] << 24 |
				(msgdata_t)c[1] << 16 |
				(msgdata_t)c[2] << 8 |
				(msgdata_t)c[3]
				);
		return result;
	}



	msgdata_t CAN_Tools::PDO_To_msgdata(can_frame msg, unsigned int startBytePos, unsigned int msgLenght)
	{
		msgdata_t Stellenwerfaktor=1;
		msgdata_t result=0;
		for(unsigned int i=startBytePos ; i< startBytePos+msgLenght; i++){
			result += (msgdata_t)(msg.data[i])*Stellenwerfaktor ;
			Stellenwerfaktor *=256LL;
		}

		return result;

	}

	msgdata_t CAN_Tools::PDO_To_msgdata(can_frame msg, unsigned int endBytePos)
	{
		return PDO_To_msgdata(msg,0,endBytePos);
	}




	msgdata_t CAN_Tools::PDO_To_msgdata_signed(can_frame msg, unsigned int startBytePos, unsigned int msgLenght)
	{
		msgdata_t result=0;
		if(msgLenght==1){
			result =(__s8)(msg.data[startBytePos] );
		}else	if(msgLenght==2){
			result =(__s16)(msg.data[startBytePos] | msg.data[startBytePos+1]<< 8 );
		}else if(msgLenght==4){
			result =(__s32)(
					msg.data[startBytePos]        |
					msg.data[startBytePos+1]<< 8  |
					msg.data[startBytePos+2]<< 16 |
					msg.data[startBytePos+3]<< 24
					);
		}else if(msgLenght==8){
			result =(__s64)(
					msg.data[startBytePos]        |
					msg.data[startBytePos+1]<< 8  |
					msg.data[startBytePos+2]<< 16 |
					msg.data[startBytePos+3]<< 24 |
					msg.data[startBytePos+4]<< 32 |
					msg.data[startBytePos+5]<< 40 |
					msg.data[startBytePos+6]<< 48 |
					msg.data[startBytePos+7]<< 56
					);
		}else{
			std::cerr << "CANTools: PDO_To_MSGDATA_signed:  Conversion not supported " << std::endl;
		}

		return msgdata_t( result);

	}

	msgdata_t CAN_Tools::PDO_To_msgdata_signed(can_frame msg, unsigned int endBytePos)
	{
		return PDO_To_msgdata_signed(msg,0,endBytePos);
	}








/*
 *  logging can_frames
 */



	std::ostream& operator<<(std::ostream& out, const  can_frame& f){

		out << std::setfill('0')<< std::hex ;
		out << std::setw(3)	<<(int) f.can_id;
		out << "  [" << std::setw(2) <<   (int) f.can_dlc <<"] " ;

		for (int i = 0; i < f.can_dlc; ++i) {
			out <<(int)f.data[i] << " " ;
		}
		out << std::dec;


		return out;

	}





}  // namespace sfzcan
