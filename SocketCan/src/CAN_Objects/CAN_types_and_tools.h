/*
 * types.h
 *
 *  Created on: 30.03.2010
 *      Author: locher
 */


#ifndef CAN_TYPES_AND_TOOLS_
#define CAN_TYPES_AND_TOOLS_

#include <iostream>
#include <iomanip>
#include <cstring>

#include <sys/socket.h>
#include <linux/can.h>
#include <bits/sockaddr.h>

#define	MAX_MESSAGE_LENGHT 2048

namespace sfzcan {

	class E_CAN_Exception :public std::exception{

	public:
		explicit E_CAN_Exception(const char* message)
			{
				if(strlen(message)> MAX_MESSAGE_LENGHT){
					strncpy(my_message,message,MAX_MESSAGE_LENGHT-1);
					my_message[MAX_MESSAGE_LENGHT-1]='\0'; //Terminierung

				}else{
					strcpy(my_message,message);
				}
			}


		virtual ~E_CAN_Exception()throw(){};

		virtual const char* what() const throw(){
			return my_message;
		}

	protected:
		char my_message[MAX_MESSAGE_LENGHT];

	};


	class E_CAN_Bind_Exception : public E_CAN_Exception{

	public:
		E_CAN_Bind_Exception(): E_CAN_Exception("E_CAN_Bind_Exception"){}

		explicit E_CAN_Bind_Exception(const char* message ):
				E_CAN_Exception(message){};

		virtual ~E_CAN_Bind_Exception()throw(){};


	};



	class E_CAN_IO_Exception : public E_CAN_Exception{

	public:
		E_CAN_IO_Exception(): E_CAN_Exception("E_CAN_IO_Exception"){}

		explicit E_CAN_IO_Exception(const char* message ):
				E_CAN_Exception(message){};

		virtual ~E_CAN_IO_Exception()throw(){};


	};







	typedef	__u16 can_index_t;
	typedef	__u8  can_subindex_t;

	typedef	long	timeout_t;
	typedef unsigned int uint_t;

	typedef long long msgdata_t;
	typedef unsigned long long u_msgdata_t;

	class CAN_Tools{

	public:
		static __u8 TPCANMsg_DataTo_u8(can_frame);
		static __s8 TPCANMsg_DataTo_s8(can_frame);
		static __u16 TPCANMsg_DataTo_u16(can_frame);
		static __s16 TPCANMsg_DataTo_s16(can_frame);
		static __u32 TPCANMsg_DataTo_u32(can_frame);
		static __s32 TPCANMsg_DataTo_s32(can_frame);

		static msgdata_t PDO_To_msgdata(can_frame,unsigned int endBytePos);
		static msgdata_t PDO_To_msgdata(can_frame,unsigned int startBytePos,unsigned int msg_length);

		static msgdata_t PDO_To_msgdata_signed(can_frame,unsigned int endBytePos);
		static msgdata_t PDO_To_msgdata_signed(can_frame,unsigned int startBytePos,unsigned int msg_length);

		static msgdata_t fourCharStringTo_msgdata(std::string fourChars );

	};


	std::ostream& operator<<(std::ostream& out, const  can_frame& f);



}  // namespace sfzcan

#endif /* CAN_TYPES_AND_TOOLS_ */
